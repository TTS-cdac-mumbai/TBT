/*-------------------------------------------------------------------------
 *  PthLibrary.c - Pitch Extraction Utilities
 *  Version:	$Name:  $
 *  Module:	
 *
 *  Purpose:	
 *  See:	
 *
 *  Author:	Hema A Murthy (hema@bhairavi.iitm.ernet.in)
 *
 *  Created:        Wed 31-Oct-2001 23:18:36
 *  Last modified:  Mon 11-Aug-2014 12:48:33 by hema
 *  $Id: PthLibrary.c,v 1.2 2007/05/24 12:08:56 hema Exp hema $
 *
 *  Bugs:	
 *
 *  Change Log:	<Date> <Author>
 *  		<Changes>
 -------------------------------------------------------------------------*/

#include "stdio.h"
#include "math.h"
#include "malloc.h"
#include "fe/FrontEndDefs.h"
#include "fe/FrontEndTypes.h"
#include "fe/InitAsdf.h"
#include "fe/DspLibrary.h"
/****************************************************************************
*	The following subroutine extracts the pitch from the given signal
*	using minimum phase group delay function 
*
*	inputs : DaTa (signal) npts points long
*	mfft : FFT stages nfft = 2**mfft
*	pthLow, pthHgh : Expected range of pitch values
*	alfa : compression required for magnitude SpectrumRealrum
*       fNum : frameNum upto which rootCepstrum shd be 
*       stored.
*	Outputs :
          pitchMingd : pitch value extracted
*****************************************************************************
*
*/
	float PitchMinGd(float *signal,int npts,int nfft,int mfft, 
                         int pthLow, int pthHgh,float alfa){
        static int      frameNum = 0;
	static float	*sigTemp;
        static int      flag = 0;
	static float	*ax, *ay, *amag, *phase;
        static int      nfBy2;
	int		i;
        float           pitch;
        if (flag == 0) {
	  nfBy2 = nfft/2;
	  sigTemp = (float *) AllocFloatArray(sigTemp, nfft+1);
	  ax = (float *) AllocFloatArray(ax, nfft+1);
	  ay = (float *) AllocFloatArray(ay, nfft+1);
	  amag = (float *) AllocFloatArray(amag, nfft+1);
	  phase = (float *) AllocFloatArray(phase, nfft+1);
	  flag = 1;
        }
        printf("frameNum = %d\n",frameNum);
        fflush(stdout);
	for (i = 1; i <= npts; i++)
	  sigTemp[i] = signal[i];
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	Rfft(sigTemp, ax, ay, mfft, nfft, -1);
	SpectrumReal(nfft, ax, ay, amag, phase);
        fflush(stdout);
	for (i = 1; i <= nfft; i++){
          amag[i] = exp(log(amag[i])*alfa);
	}
	Rfft(amag,sigTemp,ay,mfft,nfft,1);
        for (i = 1; i < pthLow; i++)
          sigTemp[i] = 0;
        for (i = pthHgh; i <= nfBy2; i++)
          sigTemp[i] = 0;
	frameNum++;
	pitch =  (float) Imax(sigTemp,nfBy2);
	printf("pitch = %f\n", pitch);
        return(pitch-1);
	}
	 
/****************************************************************************
*	The following subroutine extracts the pitch from the given signal
*	from the log Cepstrum of the given signal
*
*	inputs : DaTa npts points long
*	mfft : FFT stages nfft = 2**mfft
*	pthLow, pthHgh : expected range of pitch values
*
*	Outputs :
*       pitchCepstrum - extracted value of pitch
*****************************************************************************
*
*/
        float 	PitchCepstrum(float *signal,int npts,int nfft,int mfft, int pthLow, int pthHgh) {
	static float		*sigTemp, *Spectrum;
	static float		*ax, *ay,*amag,*phase;
	int		        i;
        static int              nfBy2;
        static int              flag = 0;
        float                   pitch;
        FILE                    *fp;
        if (flag == 0) {      
          sigTemp = (float *) AllocFloatArray(sigTemp,nfft+1);
	  Spectrum = (float *) AllocFloatArray(Spectrum,nfft+1);
          ax = (float *) AllocFloatArray(ax,nfft+1);
          ay = (float *) AllocFloatArray(ay,nfft+1);
          amag = (float *) AllocFloatArray(amag,nfft+1);
          phase = (float *) AllocFloatArray(phase,nfft+1);
	  nfBy2 = nfft/2;
          flag = 1;
        }
	for (i = 1; i <= npts; i++)
	  sigTemp[i] = signal[i];
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	Rfft(sigTemp, ax, ay, mfft, nfft, -1);
	SpectrumReal(nfft, ax, ay, amag, phase);
	for (i = 1; i <= nfft; i++)
          amag[i] = log(amag[i]);
	Rfft(amag,sigTemp, ay, mfft, nfft, 1);
        fp = fopen("cepstrum.dat", "w");
        for (i = 1; i <= nfBy2; i++)
          fprintf(fp, "%d %f\n", i, sigTemp[i]);
        fclose(fp);
	for (i = 1; i <= pthLow; i++)
	  sigTemp[i] = 0;
        for (i = pthHgh+1; i <= nfBy2; i++)
          sigTemp[i] = 0;

	pitch = (float) Imax(sigTemp, nfBy2);
        return(pitch-1);
      }
/****************************************************************************
*	The following subroutine extracts the pitch from the given signal
*	using modified group delay functions
*
*	inputs : DaTa npts points long
*	mfft : FFT stages nfft = 2**mfft
*	WiNLEN : window length for zero SpectrumRealrum
*       pthLow, pthHgh : expected range of pitch values
*
*	Outputs :
*	pitchModgd - value of the pitch picked
*****************************************************************************
*
*/
float PitchModifiedGdLP(float *signal,int npts,int nfft,int mfft, 
			int pthLow,int pthHgh, int winLen, float winScaleFactor, 
			int lpOrder, int medOrder, 
			float gamma, float gdPosScale,
			float gdNegScale){
	static float		*derv;
        static int              flag = 0, nfBy2;
	static int              *peakArr;
        float                   ave, pitch;
        int                     i, npeaks;
	static float		*ax, *ay, *amag, *phase, *cepAmag, *sigTemp;
        static float            c0;

        if (flag == 0) {
	  derv = (float *) AllocFloatArray(derv, nfft+1);
          peakArr = (int *) calloc (nfBy2+1, sizeof(int));
          ax = (float *) AllocFloatArray(ax, nfft+1);
          ay = (float *) AllocFloatArray(ay, nfft+1);
          amag = (float *) AllocFloatArray(amag, nfft+1);
          cepAmag = (float *) AllocFloatArray(cepAmag, nfft+1);
          phase = (float *) AllocFloatArray(phase, nfft+1);
          sigTemp = (float *) AllocFloatArray(sigTemp, nfft+1);
          flag = 1;
	  nfBy2 = nfft/2;
	}

     	for (i = 1; i <= npts; i++)
	  sigTemp[i] = signal[i];
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	Rfft(sigTemp,ax,ay,mfft,nfft,-1);
	SpectrumReal(nfft,ax,ay,amag,phase);
  	for( i = 1; i <= nfft; i++)
	  ax[i] = amag[i];
	CepSmooth(ax,cepAmag,mfft,nfft,winLen,&c0, 1.0);
        /*sigFile = fopen("modGdSig.dat", "w"); */
        for (i = 1; i <= npts/winScaleFactor; i++) {
          sigTemp[i] = amag[i]/cepAmag[i];
	  //          fprintf (sigFile, "%d %f\n", i, sigTemp[i]);
	}
	for(i = npts/winScaleFactor+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
        MedianSmoothArray(sigTemp, npts/4, medOrder, signal);
	derv = (float *) StandardModGdLP(signal, npts/4, nfft, mfft, lpOrder, gamma, 
					 gdPosScale, gdNegScale, medOrder, derv);
    	Rfft(derv,ax,ay,mfft,nfft,1);
	for (i = 2; i <= winLen; i++) {
	  ax[i] = ax[i]*HanW(i,winLen);
	  ax[nfft-i+2] = ax[i];
	}
	for (i = winLen+1; i <= nfft-winLen-1; i++)
	  ax[i] = 0.0;
	  Rfft(ax,derv,ay,mfft,nfft,-1); 
        for (i = 1; i <= pthLow; i++)
          derv[i] = 0;
	for (i = pthHgh+1; i <= nfBy2; i++)
          derv[i] = 0; 
	RemoveAverage(derv,nfBy2,&ave);
	pitch = (float) Imax(derv,nfBy2);
        return(pitch-1);
	}

/****************************************************************************
*	The following subroutine extracts the pitch from the given signal
*	using modified group delay functions
*
*	inputs : DaTa npts points long
*	mfft : FFT stages nfft = 2**mfft
*	WiNLEN : window length for zero SpectrumRealrum
*       pthLow, pthHgh : expected range of pitch values
*
*	Outputs :
*	pitchModgd - value of the pitch picked
*****************************************************************************
*
*/
	float PitchModifiedGd(float *signal,int npts,int nfft,int mfft, 
			      int pthLow,int pthHgh, int winLen, float winScaleFactor,
                              int gdSmthWinSize, int medOrder, float gamma, float gdPosScale,
                              float gdNegScale){
	static float		*sigTemp=NULL;
	static float		*ax=NULL, *ay=NULL, *amag=NULL, *phase=NULL;
	static float		*cepAmag=NULL, *derv=NULL;
	static int		nfBy2;
        float		        c0, ave;
        float                   pitch, tempVal;
        static int              flag = 0;
        int                     i;
        if (flag == 0) {
	  nfBy2 = nfft/2;
	  ax = (float *) AllocFloatArray(ax, nfft+1);
          ay = (float *) AllocFloatArray(ay, nfft+1);
          sigTemp = (float *) AllocFloatArray(sigTemp, nfft+1);
          amag = (float *) AllocFloatArray(amag, nfft+1);
          cepAmag = (float *) AllocFloatArray(cepAmag, nfft+1);
          phase = (float *) AllocFloatArray(phase, nfft+1);
          derv = (float *) AllocFloatArray(derv, nfft+1);
	  flag = 1;
	}
	for (i = 1; i <= nfft; i++)
	  sigTemp[i] = signal[i];
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	Rfft(sigTemp,ax,ay,mfft,nfft,-1);
	SpectrumReal(nfft,ax,ay,amag,phase);
  	for( i = 1; i <= nfft; i++)
	  ax[i] = amag[i];
	CepSmooth(ax,cepAmag,mfft,nfft,winLen,&c0, 1.0);
        for (i = 1; i <= npts/winScaleFactor; i++) {
          sigTemp[i] = amag[i]/cepAmag[i];
	}
	for(i = npts/winScaleFactor+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
        derv = (float *) StandardModGd(sigTemp, npts, nfft, mfft, gdSmthWinSize, gamma, 
				       gdPosScale, gdNegScale, medOrder, derv);
	Rfft(derv,ax,ay,mfft,nfft,1);
	for (i = 2; i <= winLen; i++) {
	  ax[i] = ax[i]*HanW(i,winLen);
	  ax[nfft-i+2] = ax[i];
	}
	for (i = winLen+1; i <= nfft-winLen-1; i++)
	  ax[i] = 0.0;
	  Rfft(ax,derv,ay,mfft,nfft,-1); 
        for (i = 1; i <= pthLow; i++)
          derv[i] = 0;
	for (i = pthHgh+1; i <= nfBy2; i++)
          derv[i] = 0; 
	RemoveAverage(&derv[pthLow],pthHgh-pthLow,&ave);
	pitch = (float) Imax0Actual(&derv[pthLow+1], pthHgh-pthLow)+pthLow;
        return(pitch);
	}

/****************************************************************************
*	The following subroutine extracts the pitch from the LP Residual
*
*	inputs : DaTa npts points long
*	mfft : FFT stages nfft = 2**mfft
*	pthLow, pthHgh : expected range of pitch values
*
*	Outputs :
*       pitchLP - extracted value of pitch
*****************************************************************************
*
*/
float 	PitchLP(float *signal,int npts, int frameShift,int lpOrder, int pthLow, int pthHgh) {
  static float    *residual, *coef, *resAutoCorr; 
  //  static int	  *peakArr;
  int		  i;
  float		  Gain, pitch;
  static int      flag = 0;

       
	if (flag == 0) {
	    residual = (float *) AllocFloatArray(residual, npts+1);
	    resAutoCorr = (float *) AllocFloatArray (resAutoCorr, npts+1);
	    coef = (float *) AllocFloatArray(coef,lpOrder+3);
	    flag = 1;
	}
	LpAnal(signal,residual, npts, frameShift, coef,lpOrder,&Gain);
	resAutoCorr  = (float *) AutoCorr(residual, npts);
        for (i = 0; i <= pthLow; i++)
          resAutoCorr[i] = 0; 
        for (i = pthHgh; i <= npts; i++)
          resAutoCorr[i] = 0; 

	pitch = (float) Imax(resAutoCorr, npts);
        return(pitch-1);
      }










/*-------------------------------------------------------------------------
 * $Log: PthLibrary.c,v $
 * Revision 1.2  2007/05/24 12:08:56  hema
 * Modified PitchModifiedGd to fix a bug
 *
 * Revision 1.1  2007/05/23 11:32:37  hema
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
 *                        End of pth_library.c
 -------------------------------------------------------------------------*/
