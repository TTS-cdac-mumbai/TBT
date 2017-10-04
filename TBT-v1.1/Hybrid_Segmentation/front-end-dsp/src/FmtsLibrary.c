/*-------------------------------------------------------------------------
 *  fmts_library.c - Formant extraction utilities
 *  Version:	$Name:  $
 *  Module:	
 *
 *  Purpose:	
 *  See:	
 *
 *  Author:	Hema A Murthy (hema@bhairavi.iitm.ernet.in)
 *
 *  Created:        Wed 31-Oct-2001 23:18:22
 *  Last modified:  Mon 04-Aug-2014 13:39:49 by hema
 *  $Id: FmtsLibrary.c,v 1.4 2007/05/24 12:09:51 hema Exp hema $
 *
 *  Bugs:	
 *
 *  Change Log:	<Date> <Author>
 *  		<Changes>
 -------------------------------------------------------------------------*/

#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "fe/FrontEndDefs.h"
#include "fe/FrontEndTypes.h"
#include "fe/DspLibrary.h"
#include "fe/InitAsdf.h"
/****************************************************************************
*	The following subroutine extracts the formants from the given signal
*	using minimum phase group delay function 
*
*	inputs : DATA (signal) npts points long
*	mfft : fft stages nfft = 2**mfft
*	winlen : window length for minimum phase signal
*	alfa : compression required for magnitude spectrum
*	ans : if alfa is negative the sign of min phs. GD is changed
*	      if ans is 'Y'
*
*	Outputs :
*	num : number of formants picked
*	freq(num) - formant frequency array
*****************************************************************************
*
*/
	float *FmtsMinGd(float *signal,int npts,int nfft,int mfft, 
			  int winlen,float alfa, float *freq,
			  int *num){
	  static int      frameNum = 0;
	  int		  maxPts=1024,maxPtsby2=512;
	  static float	  *sigTemp;
	  static int      flag = 0;
	  static float	  *ax, *ay,*amag,*phase;
	  static float	  *derv;
	  static int	  *peakArr;
	  static int      nfBy2;
	  int		  i,npeaks;
	  //	  FILE            *gdData, *specData;

	  if (flag == 0) {
	    nfBy2 = nfft/2;
	    sigTemp = (float *) AllocFloatArray(sigTemp,nfft+1);
	    freq    = (float *) AllocFloatArray(freq,10);
	    ax =  (float *) AllocFloatArray(ax,nfft+1);
	    ay = (float *) AllocFloatArray(ay,nfft+1);
	    amag = (float *) AllocFloatArray(amag,nfft+1);
	    phase = (float *) AllocFloatArray(phase,nfft+1);
	    derv = (float *) AllocFloatArray(phase,nfft+1);
	    peakArr = (int *) AllocIntArray(peakArr,nfft+1);
	    flag = 1;
	  }
	  //        printf("frameNum = %d\n",frameNum);
        fflush(stdout);
	for (i = 1; i <= npts; i++)
	  sigTemp[i] = signal[i];
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	Rfft(sigTemp,ax,ay,mfft,nfft,-1);
	SpectrumReal(nfft,ax,ay,amag,phase);
        fflush(stdout);
	/*        if (frameNum == fNum) {
          specData = fopen("spec.dat","w");
          for (i = 1;i <= nfBy2; i++)
            if (alfa > 0) 
              fprintf(specData,"%f\n",log(amag[i]));
            else
              fprintf(specData, "%f\n",-log(amag[i]));
          fclose(specData);
	  }*/
	for (i = 1; i <= nfft; i++){
          amag[i] = exp(log(amag[i])*alfa);
	}
	Rfft(amag,sigTemp,ay,mfft,nfft,1);
	for (i = 1; i <= winlen; i++)
	  sigTemp[i] = sigTemp[i]*HanW(i,winlen);
	for (i = winlen+1; i <= nfft; i++)
	  sigTemp[i] = 0.0;
	Rfft(sigTemp,ax,ay,mfft,nfft,-1);
	SpectrumReal(nfft,ax,ay,amag,phase);
	for(i = 1; i <=  nfBy2-1; i++)
	  derv[i] = phase[i] - phase[i+1];
	derv[nfBy2] = derv[nfBy2-1];
	/*        if ((alfa < 0) && (ans == 'Y')) 
	  for (i = 1; i <= nfBy2; i++)
	    derv[i] = -derv[i];
	        if (frameNum == fNum) {
          gdData = fopen("minGd.dat","w");
          for (i = 1;i <= nfBy2; i++)
            fprintf(gdData,"%f\n",derv[i]);
          fclose(gdData);
	  }*/
	FindPeaks(derv,peakArr,nfBy2);
	npeaks = 0;
	for (i =1; i <= nfBy2; i++)
	  if((peakArr[i] != 0) && (derv[i] > 0.0)) {
	    npeaks++;
           freq[npeaks] = (float)i;
	  }
	
	*num = npeaks;
	frameNum++;
	return(freq); 
}
	  
/****************************************************************************
*	The following subroutine extracts the formants from the given signal
*	using Linear prediction analysis (Magnitude Spectrum)
*
*	inputs : DATA npts points long
*	mfft : fft stages nfft = 2**mfft
*	lpOrder: order of the model to be used
*
*	Outputs :
*	num : number of formants picked
*	freq(num) - formant frequency array
******************************************************************************
*/
float	*FmtsLpMag(float *signal,int npts, int frameShift, int nfft,int mfft, 
			int lpOrder,float *freq, int *num) {
	static float    *spectr,*phase,*Res,*coef; 
	static int	*peakArr;
	int		i,npeaks;
        static int      nfBy2;
	float		Gain;
        static int      flag = 0;

       
	if (flag == 0) {
	    nfBy2 = nfft/2;
	    Res = (float *) AllocFloatArray(Res,npts+1);
	    spectr = (float *) AllocFloatArray(spectr,nfft+1);
	    phase = (float *) AllocFloatArray(phase,nfft+1);
	    coef = (float *) AllocFloatArray(coef,lpOrder+3);
	    peakArr = (int *) AllocIntArray(peakArr,nfft+1);
	    flag = 1;
	}
	LpAnal(signal,Res,npts, frameShift, coef,lpOrder,&Gain);
	LPSpectrum(coef,lpOrder,spectr,phase,nfft,mfft,Gain);
	FindPeaks(spectr,peakArr,nfBy2);
	npeaks = 0;
	for (i =1; i <= nfBy2; i++)
	  if(peakArr[i] != 0) {
	    npeaks = npeaks + 1;
	  freq[npeaks] = (float)i;
	  }
	  *num = npeaks;
       *num = npeaks;
       //printf("num = %d\n",*num);
       fflush(stdout); 
       return(freq);
      }
/****************************************************************************
*	The following subroutine extracts the formants from the given signal
*	using Linear prediction analysis using LPPhase spectrum
*
*	inputs : DATA npts points long
*	mfft : fft stages nfft = 2**mfft
*	lpOrder: order of the model to be used
*
*	Outputs :
*	num : number of formants picked
*	freq(num) - formant frequency array
******************************************************************************
*/
float	*FmtsLpPhase(float *signal,int npts, int frameShift, int nfft,int mfft, 
			int lpOrder,float *freq,int *num) {
  static float    *spectr,*phase,*Res,*coef, *gdelay; 
	static int	*peakArr;
	int		i,npeaks;
        static int      nfBy2;
	float		Gain;
        static int      flag = 0;

       
	if (flag == 0) {
	    nfBy2 = nfft/2;
	    Res = (float *) AllocFloatArray(Res,npts+1);
	    spectr = (float *) AllocFloatArray(spectr,nfft+1);
	    phase = (float *) AllocFloatArray(phase,nfft+1);
	    gdelay = (float *) AllocFloatArray(gdelay,nfft+1);
	    coef = (float *) AllocFloatArray(coef,lpOrder+3);
	    peakArr = (int *) AllocIntArray(peakArr,nfft+1);
	    gdelay = (float *) AllocFloatArray(phase,nfft+1);
	    flag = 1;
	}
	LpAnal(signal,Res,npts, frameShift, coef,lpOrder,&Gain);
	LPSpectrum(coef, lpOrder, spectr, phase, nfft, mfft, Gain);
	for (i = 2; i <= nfBy2; i++) {
          gdelay[i] = phase[i]-phase[i+1];
	  if (fabs(gdelay[i]) > PI) gdelay[i] = (gdelay[i-1]+gdelay[i+1])/2.0;
	}
	FindPeaks(gdelay,peakArr,nfBy2);
	npeaks = 0;
	for (i =1; i <= nfBy2; i++)
	  if(peakArr[i] != 0) {
	    npeaks = npeaks + 1;
	  freq[npeaks] = (float)i;
	  }
	  *num = npeaks;
       *num = npeaks;
       //printf("num = %d\n",*num);
       //fflush(stdout); 
       return(freq);
      }
/****************************************************************************
*	The following subroutine extracts the formants from the given signal
*	using Cepstral smoothing of the log magnitude spectrum
*
*	inputs : DATA npts points long
*	mfft : fft stages nfft = 2**mfft
*	winlen : window length for cepstral smoothing
*
*	Outputs :
*	num : number of formants picked
*	freq(num) - formant frequency array
*****************************************************************************
*
*/
float 	*FmtsCepstrum(float *signal,int npts,int nfft,int mfft, int winlen,float *freq,int *num) {
	static float	*sigTemp;
	static float	*ax, *ay,*amag,*phase;
	static int	*peakArr;
	int		i,npeaks;
        static int      nfBy2,flag = 0;
        float           c0;
        if (flag == 0) {
	  nfBy2 = nfft/2;
          sigTemp = (float *) AllocFloatArray(sigTemp, nfft+1);
          freq    = (float *) AllocFloatArray(freq, 10);
          ax = (float *) AllocFloatArray(ax, nfft+1);
          ay = (float *) AllocFloatArray(ay, nfft+1);
          amag = (float *) AllocFloatArray(amag, nfft+1);
          phase = (float *) AllocFloatArray(phase, nfft+1);
          peakArr = (int *) calloc (nfBy2+1, sizeof(int));
	}
	for (i = 1; i <= npts; i++)
	  sigTemp[i] = signal[i];
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	Rfft(sigTemp,ax,ay,mfft,nfft,-1);
	SpectrumReal(nfft,ax,ay,amag,phase);
  	for( i = 1; i <= nfft; i++)
	  ax[i] = amag[i];
	CepSmooth(ax,amag,mfft,nfft,winlen,&c0, 1.0);
        printf("cep smooth spectrum signal computed\n");
        fflush(stdout);
        FindPeaks(amag,peakArr,nfBy2);
	npeaks = 0;
	for (i =1; i <= nfBy2; i++)
	  if(peakArr[i] != 0) {
	    npeaks = npeaks + 1;
	  freq[npeaks] = (float)i;
	  }
       *num = npeaks;
       return(freq);
      }
/****************************************************************************
*	The following subroutine extracts the formants from the given signal
*	using modified group delay functions
*
*	inputs : DATA npts points long
*	mfft : fft stages nfft = 2**mfft
*	winlen : window length for zero spectrum
*
*	Outputs :
*	num : number of formants picked
*	freq(num) - formant frequency array
*	derv(nfft) - smoothed modified group delay function
*****************************************************************************
*
*/
	float *FmtsModGd(float *signal,int npts,int nfft,int mfft, 
                         int winLen, int gdSmthWinSize, float gamma,
                         float gdPosScale, float gdNegScale, 
                         float *freq, int *num){
	static float		*sigTemp;
	static float		*ax, *ay,*amag,*phase;
	static float		*cepAmag,*derv;
	static int		*peakArr;
        static int              nfBy2,flag = 0;
	static complex 	        *cSig,*cfSig;
	static complex		*cfTemp1,*cfTemp2,u;
	int		        i,npeaks;
        float		        c0, Ave, tempVal;

        if (flag == 0) {
	  nfBy2 = nfft/2;
          ax = (float *) AllocFloatArray(ax, nfft+1);
          ay = (float *) AllocFloatArray(ay, nfft+1);
          amag = (float *) AllocFloatArray(amag, nfft+1);
          cepAmag = (float *) AllocFloatArray(cepAmag, nfft+1);
          phase = (float *) AllocFloatArray(phase, nfft+1);
          derv = (float *) AllocFloatArray(derv, nfft+1);
          cSig = (complex *) calloc (nfft+1, sizeof(complex));
          cfSig = (complex *) calloc (nfft+1, sizeof(complex));
          cfTemp1 = (complex *) calloc (nfft+1, sizeof(complex));
          cfTemp2 = (complex *) calloc (nfft+1, sizeof(complex));
          sigTemp = (float *) AllocFloatArray(sigTemp, nfft+1);
	  //          freq    = (float *) AllocFloatArray(freq, 10);
          peakArr = (int *) calloc (nfBy2+1, sizeof(int));
          flag = 1;
	}
	for (i = 1; i <= npts; i++)
	  sigTemp[i] = signal[i];
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	for (i = 1; i < npts; i++){
	  u.re = sigTemp[i];
          u.im = 0.0;
	  cSig[i] = u;
	  u.re = (float)(i-1);
          u.im = 0.0;
	  cmul(cfSig[i],u,cSig[i]);
	}
	for (i = npts+1; i <= nfft; i++) {
	  cSig[i].re = 0.0;
          cSig[i].im = 0.0;
	  cfSig[i].re = 0.0;
          cfSig[i].im = 0.0;
	}
	Cfft(cSig,cfTemp1,mfft,nfft,-1);
	Cfft(cfSig,cfTemp2,mfft,nfft,-1);
	Rfft(sigTemp,ax,ay,mfft,nfft,-1);
	SpectrumReal(nfft,ax,ay,amag,phase);
  	for( i = 1; i <= nfft; i++)
	  ax[i] = amag[i];
	CepSmooth(ax,cepAmag, mfft, nfft, gdSmthWinSize, &c0, 2*gamma);
	for (i = 2; i <= nfBy2+1; i++){
          conjg(u,cfTemp1[i]);
	  cmul(cfTemp2[i],cfTemp2[i],u);
	  u.re = cfTemp2[i].re;
          u.im = cfTemp2[i].im;
          cfTemp2[i].re = u.re/cepAmag[i];
          cfTemp2[i].im = u.im/cepAmag[i];
	  tempVal = fabs(cfTemp2[i].re);
          if (tempVal != 0.0) {
	    derv[i] = cfTemp2[i].re/tempVal;
	    if (cfTemp2[i].re > 0)
	      derv[i] = derv[i]*exp(gdPosScale*log(tempVal));
	    else
	      derv[i] = derv[i]*exp(gdNegScale*log(tempVal));
	  } else 
	    derv[i] = 0;
	  derv[nfft-i+2] = derv[i];
	}
        conjg(u,cfTemp1[1]);
	cmul(cfTemp2[1],cfTemp2[1],u);
        if (cabs2(cfTemp1[1]) == 0.0){ 
	  cfTemp2[1].re = cfTemp2[1].re/0.0001;
          cfTemp2[1].im = cfTemp2[1].im/0.0001;
	}
	else{
          cfTemp2[1].re = cfTemp2[1].re/cepAmag[1];
          cfTemp2[1].im = cfTemp2[1].im/cepAmag[1];
	}
          tempVal = fabs(cfTemp2[1].re);
          if (tempVal != 0.0) {
	    derv[1] = cfTemp2[1].re/tempVal;
          if (cfTemp2[1].re > 0)
	    derv[1] = derv[1]*exp(gdPosScale*log(tempVal));
          else
            derv[1] = derv[1]*exp(gdNegScale*log(tempVal));
	  } else 
	    derv[1] = 0;					    
	Rfft(derv,ax,ay,mfft,nfft,1);
	for (i = 2; i <= winLen; i++) {
	  ax[i] = ax[i]*HanW(i,winLen);
	  ax[nfft-i+2] = ax[i];
	}
	for (i = winLen+1; i <= nfft-winLen+1; i++)
	  ax[i] = 0.0;

	Rfft(ax,derv,ay,mfft,nfft,-1);

	RemoveAverage(derv,nfft,&Ave);

	FindPeaks(derv,peakArr,nfBy2);

	npeaks = 0;
	for (i =1; i <= nfBy2; i++)
	  if((peakArr[i] != 0) && (derv[i] > 0.0)) { 
	    npeaks = npeaks + 1;
	  freq[npeaks] = (float)i;
	  }
	
       *num = npeaks;
       return(freq);
      }
/****************************************************************************
*	The following subroutine extracts the formants from the given signal
*	using modified group delay functions -- smoothing of mag spectrum done using LP
*
*	inputs : DATA npts points long
*	mfft : fft stages nfft = 2**mfft
*	winlen : window length for zero spectrum
*
*	Outputs :
*	num : number of formants picked
*	freq(num) - formant frequency array
*	derv(nfft) - smoothed modified group delay function
*****************************************************************************
*
*/
	float *FmtsModGdLP(float *signal,int npts,int nfft,int mfft, 
			   int winLen, int lpOrder, int medOrder, float gamma,
                         float gdPosScale, float gdNegScale, 
                         float *freq, int *num){
	static float		*derv;
	static float		*ax, *ay;
        static int              flag = 0, nfBy2;
	static int              *peakArr;
        float                   Ave;
        int                     i, npeaks;
	  /*	static float		*sigTemp;
	static int		*peakArr;
        static int              nfBy2,flag = 0;
	static complex 	        *cSig,*cfSig;
	static complex		*cfTemp1,*cfTemp2,u;
	int		        i,npeaks;
        float		        c0, Ave, tempVal; */
	
        if (flag == 0) {
          nfBy2 = nfft/2;
	  derv = (float *) AllocFloatArray(derv, nfft+1);
          peakArr = (int *) calloc (nfBy2+1, sizeof(int));
          ax = (float *) AllocFloatArray(ax, nfft+1);
          ay = (float *) AllocFloatArray(ay, nfft+1);
          flag = 1;
	}
	  /* ( if (flag == 0) {
	  nfBy2 = nfft/2;
          derv = (float *) AllocFloatArray(derv, nfft+1);
          cSig = (complex *) calloc (nfft+1, sizeof(complex));
          cfSig = (complex *) calloc (nfft+1, sizeof(complex));
          cfTemp1 = (complex *) calloc (nfft+1, sizeof(complex));
          cfTemp2 = (complex *) calloc (nfft+1, sizeof(complex));
          sigTemp = (float *) AllocFloatArray(sigTemp, nfft+1);
          freq    = (float *) AllocFloatArray(freq, 10);
          peakArr = (int *) calloc (nfBy2+1, sizeof(int));
          flag = 1;
	}
	for (i = 1; i <= npts; i++)
	  sigTemp[i] = signal[i];
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	for (i = 1; i < npts; i++){
	  u.re = sigTemp[i];
          u.im = 0.0;
	  cSig[i] = u;
	  u.re = (float)(i-1);
          u.im = 0.0;
	  cmul(cfSig[i],u,cSig[i]);
	}
	for (i = npts+1; i <= nfft; i++) {
	  cSig[i].re = 0.0;
          cSig[i].im = 0.0;
	  cfSig[i].re = 0.0;
          cfSig[i].im = 0.0;
	}
	Cfft(cSig,cfTemp1,mfft,nfft,-1);
	Cfft(cfSig,cfTemp2,mfft,nfft,-1);
	Rfft(sigTemp,ax,ay,mfft,nfft,-1);
	SpectrumReal(nfft,ax,ay,amag,phase);
  	for( i = 1; i <= nfft; i++)
	  ax[i] = amag[i];
	CepSmooth(ax,cepAmag,mfft,nfft,gdSmthWinSize,&c0, 2*gamma);
	for (i = 2; i <= nfBy2+1; i++){
          conjg(u,cfTemp1[i]);
	  cmul(cfTemp2[i],cfTemp2[i],u);
	  u.re = cfTemp2[i].re;
          u.im = cfTemp2[i].im;
          cfTemp2[i].re = u.re/cepAmag[i];
          cfTemp2[i].im = u.im/cepAmag[i];
	  tempVal = fabs(cfTemp2[i].re);
          if (tempVal != 0.0) {
	    derv[i] = cfTemp2[i].re/tempVal;
	    if (cfTemp2[i].re > 0)
	      derv[i] = derv[i]*exp(gdPosScale*log(tempVal));
	    else
	      derv[i] = derv[i]*exp(gdNegScale*log(tempVal));
	  } else 
	    derv[i] = 0;
	  derv[nfft-i+2] = derv[i];
	}
        conjg(u,cfTemp1[1]);
	cmul(cfTemp2[1],cfTemp2[1],u);
        if (cabs2(cfTemp1[1]) == 0.0){ 
	  cfTemp2[1].re = cfTemp2[1].re/0.0001;
          cfTemp2[1].im = cfTemp2[1].im/0.0001;
	}
	else{
          cfTemp2[1].re = cfTemp2[1].re/cepAmag[1];
          cfTemp2[1].im = cfTemp2[1].im/cepAmag[1];
	}
          tempVal = fabs(cfTemp2[1].re);
          if (tempVal != 0.0) {
	    derv[1] = cfTemp2[1].re/tempVal;
          if (cfTemp2[1].re > 0)
	    derv[1] = derv[1]*exp(gdPosScale*log(tempVal));
          else
            derv[1] = derv[1]*exp(gdNegScale*log(tempVal));
	  } else 
	    derv[1] = 0;					    
	Rfft(derv,ax,ay,mfft,nfft,1);
	for (i = 2; i <= winLen; i++) {
	  ax[i] = ax[i]*HanW(i,winLen);
	  ax[nfft-i+2] = ax[i];
	}
	for (i = winLen+1; i <= nfft-winLen+1; i++)
	  ax[i] = 0.0;

	  Rfft(ax,derv,ay,mfft,nfft,-1); */
	  derv = (float *) StandardModGdLP (signal, npts, nfft, mfft,lpOrder,
					    gamma, gdPosScale, gdNegScale,
					    medOrder, derv);
	Rfft(derv,ax,ay,mfft,nfft,1);
	for (i = 2; i <= winLen; i++) {
	  ax[i] = ax[i]*HanW(i,winLen);
	  ax[nfft-i+2] = ax[i];
	}
	for (i = winLen+1; i <= nfft-winLen+1; i++)
	  ax[i] = 0.0;

	Rfft(ax,derv,ay,mfft,nfft,-1);

	RemoveAverage(derv,nfft,&Ave);

	FindPeaks(derv,peakArr,nfBy2);

	npeaks = 0;
	for (i =1; i <= nfBy2; i++)
	  if((peakArr[i] != 0) && (derv[i] > 0.0)) { 
	    npeaks = npeaks + 1;
	  freq[npeaks] = (float)i;
	  }
	
       *num = npeaks;
       return(freq);
      }













/*-------------------------------------------------------------------------
 * $Log: FmtsLibrary.c,v $
 * Revision 1.4  2007/05/24 12:09:51  hema
 * Modified all the functions to remove unnecessary variables
 *
 * Revision 1.3  2007/05/23 11:31:42  hema
 * Modified FmtsModGd to include more parameters
 * gdSmthWinSize, gamma, gdPosScale, gdNegScale
 *
 * Revision 1.1  2004/03/11 10:01:19  hema
 * Initial revision
 *
 *
 * Local Variables:
 * time-stamp-active: t
 * time-stamp-line-limit: 20
 * time-stamp-start: "Last modified:[ 	]+"
 * time-stamp-format: "%3a %02d-%3b-%:y %02H:%02M:%02S by %u"
 * time-stamp-end: "$"
 * End:
 *                        End of fmts_library.c
 -------------------------------------------------------------------------*/
