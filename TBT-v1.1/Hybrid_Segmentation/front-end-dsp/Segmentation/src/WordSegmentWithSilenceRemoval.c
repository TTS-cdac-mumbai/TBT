/*-------------------------------------------------------------------------
 *  words.c - Identifies Digit Boundaries a given energy contour
 *  Version:	$Name:  $
 *  Module:	
 *
 *  Purpose:	
 *  See:	
 *
 *  Author:	Hema A Murthy (hema@bhairavi.iitm.ernet.in)
 *
 *  Created:        Wed 06-Feb-2002 12:22:46
 *  Last modified:  Mon 28-Apr-2014 09:20:08 by hema
 *  $Id: words.c,v 1.2 2002/02/06 06:53:26 hema Exp $
 *
 *  Bugs:	
 *
 *  Change Log:	<Date> <Author>
 *  		<Changes>
 -------------------------------------------------------------------------*/

/*****************************************************************************
  Function : Group-Delay processing of energy files
  Author   : Hema A Murthy
  Date     : Dec 6 1998
  Uses     : Dsplibrary.c,IntAsdf.c, BatchProcessWaveform.c SphereInterface.c
  Known Bugs : none to date
  ****************************************************************************/



#include "stdio.h"
#include "math.h"
#include "fe/FrontEndDefs.h"
#include "fe/FrontEndTypes.h"
#include "fe/InitAsdf.h"
#include "fe/DspLibrary.h"
#include "malloc.h"

/*-------------------------------------------------------------------------
 *  ReadData -- Reads a text file of data into an array and returns the array
 *    Args:	inFile, &numsamples
 *    Returns:	signal
 *    Bugs:	
 * -------------------------------------------------------------------------*/
float *ReadData (FILE *dataFile, int *nsamples) {
  float *signal;
  int i = 1;
  char line[200];
  float max;
  while (fgets(line,200,dataFile) != NULL) 
    i++;
  rewind(dataFile);
  signal = (float *) malloc(i*sizeof(float));
  i = 1;
  while (fgets(line,200,dataFile) != NULL) {
    sscanf(line,"%f",&signal[i]);
    fflush(stdout);
    i++;
  }
  max = signal[Imax(signal,i-1)];
  *nsamples = i-1;
  for (i = 1; i <= *nsamples; i++)
    signal[i] = signal[i]/max;
return(signal);

}	/*  End of ReadData		End of ReadData   */


/*-------------------------------------------------------------------------
 *  Usage -- Command line arguments for executing the program
 *    Args:	None
 *    Returns:	None
 *    Bugs:	
 * -------------------------------------------------------------------------*/
void Usage() {
  printf("words ctrlFile energyFile spectraFile peaksFile signalFile boundaryFile sigCepFile\n");
}	/*  End of Usage		End of Usage   */


/*-------------------------------------------------------------------------
 *  main -- main function
 *    Args:	7 arguments
 *    Returns:	none
 *    Bugs:	none
 * -------------------------------------------------------------------------*/

int main(int argc, char *argv[]) {

  FILE             *energyFile=NULL,*spectraFile=NULL,*peaksFile=NULL,
                   *sigFile=NULL,*ctrlFile=NULL,*digitFile=NULL, *cepFile=NULL;

  char             *energyFileName=NULL, *spectraFileName=NULL, 
                   *peaksFileName=NULL,*sigFileName=NULL,
                   *ctrlFilename=NULL, *digitFileName=NULL,
                   *cepFileName=NULL;

  float            *signal=NULL,*sigCop=NULL,

                   *ax=NULL,*ay=NULL,
                   *sigCep=NULL,
                   *amag=NULL,*phase=NULL,
                   *derv=NULL,*dervSmth=NULL,
                   *negDerv=NULL,*negDervSmth=NULL;
  int              *peakArr=NULL,*valleyArr=NULL;
  float            med[20];

  int              nfft, 
                   mfft,
                   samples,
                   frameAdvanceSamples;
  int              i, j,  nfby2;
  int              winlen, scaleFactor;
  float            alfa, rMin,
                   rMaxDerv, rMaxNegDerv;
  ASDF             *asdf=NULL;


  if (argc != 8) {
    Usage();
    return(-1);
  }

  ctrlFilename = argv[1];
  energyFileName = argv[2];
  spectraFileName = argv[3];
  peaksFileName = argv[4];
  sigFileName = argv[5];
  digitFileName = argv[6];
  cepFileName = argv[7];

  ctrlFile = fopen(ctrlFilename,"r");
  energyFile = fopen(energyFileName,"r");
  spectraFile = fopen(spectraFileName,"w");
  peaksFile = fopen(peaksFileName,"w");
  sigFile = fopen(sigFileName,"w");
  digitFile = fopen(digitFileName,"w");
  cepFile = fopen(cepFileName,"w");

  asdf = (ASDF *) malloc(sizeof(ASDF));
  InitializeStandardFrontEnd(asdf,ctrlFile);
  alfa = GetFAttribute(asdf,"gamma");
  frameAdvanceSamples = GetIAttribute(asdf, "frameAdvanceSamples");
  signal = (float *) ReadData(energyFile,&samples);
  mfft = ceil(log((float)(samples+1)*2)/log(2.0)); 
  nfft = (int) pow(2,mfft); 
  nfby2 = nfft/2;
  scaleFactor = GetFAttribute(asdf,"winScaleFactor");
  PutIAttribute(asdf,"fftSize",nfft);
  PutIAttribute(asdf,"fftOrder",mfft);
  winlen = (int) samples/scaleFactor;
  Cstore(nfft);

  /* Allocating various arrays required for plotting/group delay
   computation */

  ax = (float *) AllocFloatArray(ax,nfft+1);
  ay = (float *) AllocFloatArray(ay,nfft+1);
  derv  = (float *) AllocFloatArray(derv,nfby2+1);
  negDerv = (float *) AllocFloatArray(negDerv,nfby2+1);
  dervSmth = (float *) AllocFloatArray(dervSmth,nfby2+1);
  negDervSmth = (float *) AllocFloatArray(negDervSmth,nfby2+1);
  sigCop = (float *) AllocFloatArray(sigCop,nfft+1);
  amag = (float *) AllocFloatArray(amag,nfft+1);
  sigCep = (float *) AllocFloatArray(sigCep,nfft+1);
  phase = (float *) AllocFloatArray(phase,nfft+1);
  peakArr = (int *) malloc((nfby2+1)*sizeof(int)); 
  valleyArr = (int *) malloc((nfby2+1)*sizeof(int)); 
  peakArr[0] = 0;
  valleyArr[0]=0;
  printf("nfft = %d mfft = %d winlen  %d samples %d\n",nfft, mfft,winlen,samples);
  fflush(stdout); 
  sigCop[1] = 1.0/exp(alfa*log(signal[1]));
  for (i = 2; i <= samples; i++){
    sigCop[i] = 1.0/exp(alfa*log(signal[i]));
    sigCop[nfft-i+2] =sigCop[i];
  }

  rMin = signal[Imin(sigCop,samples)];

  for (i = samples+1; i <= nfft-samples+1; i++){
    sigCop[i] = rMin;
  }

  for(i = 1; i <= samples; i++)
    fprintf(sigFile,"%d %f\n",(i-1)*frameAdvanceSamples, sigCop[i]);
  fclose(sigFile);
  Rfft(sigCop,sigCep,ay,mfft,nfft,1);

  for (i = 1;i <= nfby2; i++)
    fprintf(cepFile,"%d %f\n",(i-1)*frameAdvanceSamples, sigCep[i]);
  fclose(cepFile);

  for (i = 1; i <= winlen; i++){
    sigCop[i] = sigCep[i]*HanW(i,winlen);
  }
  for (i = winlen+1; i <= nfft; i++)
    sigCop[i] = 0;
  Rfft(sigCop,ax,ay,mfft,nfft,-1);
  SpectrumReal(nfft,ax,ay,amag,phase);
  for (i = 1; i <= nfby2-1; i++){
     derv[i] = phase[i] - phase[i+1];  
     negDerv[i] = phase[i+1] - phase[i];
  }
  derv[nfby2] = derv[nfby2-1];
  negDerv[nfby2] = negDerv[nfby2-1];
  for (i = 3; i < nfby2-3; i++) {
    med[1] = derv[i-2];
    med[2] = derv[i-1];
    med[3] = derv[i];
    med[4] = derv[i+1];
    med[5] = derv[i+2];
    dervSmth[i] = (float) Median(med,5);
  }
  for (i = 3; i <= nfby2-2; i++) {
    med[1] = negDerv[i-2];
    med[2] = negDerv[i-1];
    med[3] = negDerv[i];
    med[4] = negDerv[i+1];
    med[5] = negDerv[i+2];
    negDervSmth[i] = (float) Median(med,5);
  }

  for (i = 1; i <=2; i++) {
    dervSmth[i] = derv[i];
    dervSmth[nfby2-2+i] = derv[nfby2-2+i];
    negDervSmth[i] = negDerv[i];
    negDervSmth[nfby2-2+i] = negDerv[nfby2-2+i];
  }
    rMaxDerv = derv[Imax(derv,nfby2)];
    rMaxNegDerv = negDerv[Imax(negDerv, nfby2)];
  for (i = 1; i <= nfby2; i++) 
    fprintf(spectraFile,"\n %d %f %f %f %f %f",(i-1)*frameAdvanceSamples,
    sigCop[i],phase[i],amag[i],dervSmth[i]/fabs(rMaxDerv),negDervSmth[i]/fabs(rMaxNegDerv));
 
  FindPeaks(dervSmth,peakArr, nfby2);
  FindPeaks(negDervSmth,valleyArr,nfby2);
  for (i = 1; i <= samples; i++)
    if((peakArr[i] == 1)&&(dervSmth[i] > 0)) {
       j = i;
       while ((valleyArr[j] != 1) && (j > 1)) 
         j--;
       fprintf(digitFile, "\n %d %d ", j, (j-1)*frameAdvanceSamples);
       j = i;
       while ((valleyArr[j] != 1) && (j < samples)) 
         j++;
       fprintf(digitFile, "%d %d", j, (j-1)*frameAdvanceSamples);
       fprintf(peaksFile,"%d %d \n",(j-1)*frameAdvanceSamples, 1);
    } else
      fprintf(peaksFile,"%d %d\n",(j-1)*frameAdvanceSamples, 0);
      /*printf("wb:  %d",i);*/
  free(signal);
  free(sigCep);
  free(sigCop);
  free(phase);
  free(amag);
  free(derv);
  free(negDerv);
  free(dervSmth);
  free(negDervSmth);
  free(ax);
  free(ay);
  free(peakArr);
  free(valleyArr);
  fprintf(digitFile,"\n");
  fprintf(spectraFile,"\n");
  fclose(energyFile);
  fclose(spectraFile);
  fclose(peaksFile);
  fclose(ctrlFile);
  fclose(digitFile);
  free(asdf);
  return(0);
}	/*  End of main		End of main   */

















/*-------------------------------------------------------------------------
 * $Log: words.c,v $
 * Revision 1.2  2002/02/06 06:53:26  hema
 * Modified window size to use numCepstrum as the window in cepstrum domain
 *
 *
 * Local Variables:
 * time-stamp-active: t
 * time-stamp-line-limit: 20
 * time-stamp-start: "Last modified:[ 	]+"
 * time-stamp-format: "%3a %02d-%3b-%:y %02H:%02M:%02S by %u"
 * time-stamp-end: "$"
 * End:
 *                        End of words.c
 -------------------------------------------------------------------------*/
