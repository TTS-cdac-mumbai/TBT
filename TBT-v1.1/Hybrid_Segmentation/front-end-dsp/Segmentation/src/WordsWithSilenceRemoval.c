/*-------------------------------------------------------------------------
 *  WordsWithSilenceRemoval.c - Identifies Digit Boundaries a given energy contour
 *  Version:	$Name:  $
 *  Module:	
 *
 *  Purpose:	
 *  See:	
 *
 *  Author:	Hema A Murthy (hema@bhairavi.iitm.ernet.in)
 *
 *  Created:        Wed 06-Feb-2002 12:22:46
 *  Last modified:  Wed 13-Aug-2014 18:44:53 by hema
 *  $Id: WordsWithSilenceRemoval.c,v 1.1 2013/02/08 10:23:34 hema Exp hema $
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
  Uses     : Dsplibrary.c,init-asdf.c, batch-process-waveform.c sphereInterface.c
  Known Bugs : none to date
  ****************************************************************************/



#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "math.h"
#include "malloc.h"
#include "fe/FrontEndDefs.h"
#include "fe/FrontEndTypes.h"
#include "fe/BatchProcessWaveform.h"
#include "fe/InitAsdf.h"
#include "fe/DspLibrary.h"

/*-------------------------------------------------------------------------
 *  Usage -- Command line arguments for executing the program
 *    Args:	None
 *    Returns:	None
 *    Bugs:	
 * -------------------------------------------------------------------------*/
void Usage() {
  printf("WordsWithSilenceRemoval ctrlFile waveFile sigFile spectraFile boundaryFile thresSilence (ms) thresVoiced (ms) thresEnergy1 thresEnergy2\n");
}	/*  End of Usage		End of Usage   */


/*-------------------------------------------------------------------------
 *  main -- main function
 *    Args:	8 arguments
 *    Returns:	none
 *    Bugs:	none
 * -------------------------------------------------------------------------*/

int main(int argc, char *argv[]) {

  FILE             *spectraFile=NULL, *sigFile=NULL, 
                   *ctrlFile=NULL,*boundaryFile=NULL,*symmetryFile=NULL;

  char             *spectraFileName=NULL, 
                   *sigFileName=NULL,
                   *ctrlFilename=NULL, 
                   *boundaryFileName=NULL,
                   *waveFileName=NULL;


  float            *signal=NULL,*sigEgy=NULL, *sigEgyInv=NULL, *sigCopy=NULL,
                   *sigNonZero=NULL,*sigSymmetric=NULL,
                   *ax=NULL,*ay=NULL,
                   *sigCep=NULL,
                   *amag=NULL,*phase=NULL,
                   *derv=NULL,*dervSmth=NULL,
                   *negDerv=NULL,*negDervSmth=NULL;
  int              *peakArr=NULL, *valleyArr=NULL;

  int              nfft, 
                   mfft,
                   frameAdvanceSamples, frameSize, 
                   startVFrameNumber, endVFrameNumber;
  int              i, j,  k, numVoicedFrames, nfby2;
  int              startUVFrameNumber, endUVFrameNumber, segBoundary;
  int              winlen, medianOrder, lpOrder, percentFrames;
  long             numFrames, numSamples, samplingRate;
  float            alfa, rMin, rMax, scaleFactor,
    rMaxDerv, rMaxNegDerv, energyValue;
  ASDF             *asdf=NULL;
  float            thresEnergy, thresZero, thresSpecFlatness, 
                   durationOfSilence, durationOfVoiced, rMaxEgy,
                   thresVoiced, thresSilence, thresEnergy1, thresEnergy2,
		   rmax, average;
  short            *vU=NULL, *vUSmth=NULL;
  F_VECTOR         *waveform=NULL, *energy=NULL; 

  if (argc != 10) {
    Usage();
    _exit(-1);
  }

  ctrlFilename = argv[1];
  waveFileName = argv[2];
  sigFileName = argv[3];
  spectraFileName = argv[4];
  boundaryFileName = argv[5];
  sscanf(argv[6], "%f", &thresSilence);
  sscanf(argv[7], "%f", &thresVoiced);
  sscanf(argv[8], "%f", &thresEnergy1);
  sscanf(argv[9], "%f", &thresEnergy2);

  ctrlFile = fopen(ctrlFilename,"r");
  sigFile = fopen(sigFileName, "w");
  spectraFile = fopen(spectraFileName,"w");
  boundaryFile = fopen(boundaryFileName,"w");
  symmetryFile = fopen("symmetry","w");
  
  asdf = (ASDF *) malloc(sizeof(ASDF));
  InitializeStandardFrontEnd(asdf,ctrlFile);
  GsfOpen(asdf, waveFileName);
  
  alfa = GetFAttribute(asdf,"gamma");
  frameAdvanceSamples = GetIAttribute(asdf, "frameAdvanceSamples");
  medianOrder = (int) GetIAttribute(asdf, "medianOrder");
  frameSize = (int) GetIAttribute(asdf, "windowSize");
  thresEnergy = (float) GetFAttribute(asdf, "thresEnergy");
  thresZero = (float) GetFAttribute(asdf, "thresZero");
  thresSpecFlatness = (float) GetFAttribute(asdf, "thresSpecFlatness");
  numFrames = (int) GetIAttribute(asdf, "numFrames");
  numSamples = (int)GetIAttribute(asdf, "numSamples");
  lpOrder = (int)GetIAttribute(asdf, "lpOrder");
  percentFrames = (int)GetIAttribute(asdf,"percentFrames");
  samplingRate = (int) GetIAttribute(asdf, "samplingRate");
  signal = (float *) AllocFloatArray(signal, numFrames+1); 
  sigNonZero = (float *) AllocFloatArray(sigNonZero, numFrames+1); 
  sigEgy = (float *) AllocFloatArray(sigEgy, numFrames+1); 
  sigEgyInv = (float *) AllocFloatArray(sigEgyInv, numFrames+1); 
  waveform = (F_VECTOR *)AllocFVector(frameSize);
  energy = (F_VECTOR *)AllocFVector(1);
  if ((thresSilence > (float) numSamples/samplingRate*1000.0)|| 
      (thresVoiced > (float) numSamples/samplingRate*1000.0)) 
    {printf("Thresholds too large \n");
      _exit(-1);
    }
  rmax = (float) (asdf->waveform[ImaxShort0(asdf->waveform, numSamples)]);
  for (i = 0; i < numFrames; i++) {
    waveform = (F_VECTOR *) FrameComputeWaveform(asdf, i, waveform);
    for (j = 0; j < frameAdvanceSamples; j++)
      fprintf(sigFile, "%e %e\n", (float)((i)*frameAdvanceSamples+j)/samplingRate, waveform->array[j]/rmax);
    energyValue = ((F_VECTOR *) (FrameComputeEnergy(asdf, i, energy)))->array[0];
      signal[i] = energyValue;
  }
  j = 0;
  for (i = 0; i < numFrames; i++) {
    if (signal[i] != 0.0) {
      sigNonZero[j] = signal[i];
      j++;
    }
  }
  rMin  = sigNonZero[Imin0(signal, j)];
  for (i = 0; i < numFrames; i++) {
    if (signal[i] == 0.0) 
      signal[i] = rMin;
  }
  rMaxEgy = signal[Imax0(signal, numFrames)];
  for (i = 0; i < numFrames; i++) {
    signal[i] = signal[i]/rMaxEgy;
    sigEgy[i] = signal[i];
    //if (signal[i] != 0.0) 
      //signal[i] = expf(alfa*log(signal[i]));
    if ((signal[i] != 0.0) && (signal[i] != 1.0)) {
      sigEgyInv[i] = 1.0/signal[i];
      signal[i] = expf(alfa*log(1.0/signal[i]));
    }
  }


  /* The following code is not much use -- except to
     ensure that the FFT size is set to the largest possible */

  /* begin FFT size fix */
  mfft = ceil(log((float)numFrames*2)/log(2.0)); 
  nfft = (int) pow(2,mfft); 
  nfby2 = nfft/2;
  Cstore(nfft);
  /* end FFT size fix */


 /* Allocating various arrays required for plotting/group delay
   computation */

  /* Allocate arrays to the largest possible FFT size */

  peakArr = (int *) AllocIntArray(peakArr, nfby2+1);
  valleyArr = (int *) AllocIntArray(valleyArr, nfby2+1);
  sigCopy = (float*) AllocFloatArray(sigCopy, nfft+1);
  sigSymmetric = (float*) AllocFloatArray(sigSymmetric, nfft+1);
  sigCep = (float *) AllocFloatArray(sigCep,nfft+1);
  ay = (float *) AllocFloatArray(ay,nfft+1);
  sigCopy[1] = 100.0;
  Rfft(sigCopy, sigCep, ay, mfft, nfft, 1); 
  ax = (float *) AllocFloatArray(ax,nfft+1);
  derv  = (float *) AllocFloatArray(derv,nfby2+1);
  negDerv = (float *) AllocFloatArray(negDerv,nfby2+1);
  dervSmth = (float *) AllocFloatArray(dervSmth,nfby2+1);
  negDervSmth = (float *) AllocFloatArray(negDervSmth,nfby2+1);
  amag = (float *) AllocFloatArray(amag,nfft+1);
  phase = (float *) AllocFloatArray(phase,nfft+1);
  scaleFactor = GetFAttribute(asdf,"winScaleFactor");
  PutIAttribute(asdf,"fftSize",nfft);
  PutIAttribute(asdf,"fftOrder",mfft);


  printf("nfft = %d mfft =  %d numFrames %ld\n",nfft, mfft, numFrames);

  vU = (short *) calloc(numFrames+1, sizeof(short));
  vUSmth = (short *) calloc(numFrames+1, sizeof(short));
  VoicedUnvoiced(asdf->waveform, numSamples, vU, frameAdvanceSamples,
                 frameSize, lpOrder, thresZero, thresEnergy, thresSpecFlatness, percentFrames);
  vU[numFrames] = vU[numFrames-1];
  vUSmth[numFrames] = vUSmth[numFrames-1];
  MedianSmoothShortArray(vU, numFrames, 3, vUSmth);
  i = 0;
  /* If the duration of silence is smaller than an expected
     duration -- change the silences to voiced */
  startVFrameNumber= 0;
  endVFrameNumber = -1;
  startUVFrameNumber = 0;
  endUVFrameNumber = -1;
  while (i < numFrames) {
    if (vUSmth[i] == 0){
      while ((i <  numFrames) && (vUSmth[i] == 0))
	i++;
      startUVFrameNumber = endVFrameNumber+1;
      endUVFrameNumber = i-1;
      durationOfSilence = (endUVFrameNumber - startUVFrameNumber+1)*frameAdvanceSamples/
	(float)samplingRate*1000;
      if (durationOfSilence < thresSilence){
	for (k = startUVFrameNumber; k <= endUVFrameNumber; k++)
	  vUSmth[k] = 1;
	endUVFrameNumber = startVFrameNumber-1;
	i = startVFrameNumber;
      }
    }else {
      while ((i <  numFrames) && (vUSmth[i] == 1))
	i++;
      startVFrameNumber = endUVFrameNumber+1;
      endVFrameNumber = i-1;
      numVoicedFrames = endVFrameNumber - startVFrameNumber + 1;
      durationOfVoiced = numVoicedFrames*frameAdvanceSamples/(float)samplingRate*1000;
      if (durationOfVoiced < thresVoiced) {
	for (k = startVFrameNumber; k <= endVFrameNumber; k++)
	  vUSmth[k] = 0;
	endVFrameNumber = startUVFrameNumber-1;
	i = startUVFrameNumber;
      }
    }
  }
  /* Handle final Voiced/Unvoiced segment separately 
  if (endVFrameNumber > endUVFrameNumber) {
    if (durationOfVoiced < thresVoiced) {
      for (k = startVFrameNumber; k <= endVFrameNumber; k++)
	vU[k] = 0;
    }
  } else {
    if (durationOfSilence < thresSilence) {
      for (k = startUVFrameNumber; k <= endUVFrameNumber; k++)
	vU[k] = 1;
    }
    }*/
  i = 0;
  startVFrameNumber= 0;
  endVFrameNumber = -1;
  startUVFrameNumber = 0;
  endUVFrameNumber = -1; 
  segBoundary = 0;
  while (i < numFrames) {
    if (vUSmth[i] == 0) {
      while ((i < numFrames) && (vUSmth[i] == 0))
	i++;
      endUVFrameNumber = i-1;
      startVFrameNumber = i;
      for (k = startUVFrameNumber; k <= endUVFrameNumber; k++) 
	if (signal[k] != 0.0)
	  fprintf(spectraFile,"\n %f %e %e %e %e %e %e %e %e",(float)(k-1)*frameAdvanceSamples/(float) samplingRate, 
		  signal[k],sigEgy[k], 0.0,0.0,0.0,0.0,0.0, sigEgyInv[k]);
	else
	  fprintf(spectraFile,"\n %f %e %e %e %e %e %e %e %e",(float)(k-1)*frameAdvanceSamples/(float) samplingRate,
		  0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);

      printf("startUVFrameNumber = %d endUVFrameNumber = %d \n", startUVFrameNumber, endUVFrameNumber);
      printf ("numUVFrames = %d \n", (endUVFrameNumber - startUVFrameNumber+1));
      if (endUVFrameNumber > startUVFrameNumber) 
	/*	if (startUVFrameNumber == segBoundary) */
	  fprintf(boundaryFile, "%d %e %d %d %e %d\n", startUVFrameNumber-1, 
		  (float) (startUVFrameNumber-1)*frameAdvanceSamples/(float) samplingRate, -1, endUVFrameNumber-1, 
		  (float) (endUVFrameNumber-1)*frameAdvanceSamples/(float)samplingRate, -1);
    } else {
      while ((i <  numFrames) && (vUSmth[i] == 1))
	i++;
      endVFrameNumber = i-1;
      startUVFrameNumber = i;
      numVoicedFrames = endVFrameNumber - startVFrameNumber+ 1;
      printf("startVFrameNumber = %d endVFrameNumber = %d\n", startVFrameNumber, endVFrameNumber);
	printf("numVoiceFrames = %d \n", numVoicedFrames);
      if (numVoicedFrames > 0) {
	mfft = ceil(log((float)numVoicedFrames*2)/log(2.0)); 
	nfft = (int) pow(2,mfft); 
	nfby2 = nfft/2;
	Cstore(nfft);
	printf("nfft = %d mfft = %d \n", nfft, mfft);
	 
	sigCopy[1] = signal[startVFrameNumber];
	for (k = 2; k <= numVoicedFrames; k++) {
	  sigCopy[k] = signal[startVFrameNumber+k-1];
	  sigCopy[nfft-k+2] = sigCopy[k];
	}
	rMax = sigCopy[Imax(sigCopy, numVoicedFrames)];
	//for (k = 1; k <= numVoicedFrames; k++)
	for (k = 1; k <= nfft; k++)
	  sigCopy[k] = sigCopy[k]/rMax;
	rMin = sigCopy[Imin(sigCopy,numVoicedFrames)];
	for (k = numVoicedFrames + 1; k <= nfft-numVoicedFrames + 1; k++)
	  sigCopy[k] = rMin;
	for (k = 1; k <= nfft; k++)
	  sigSymmetric[k] = sigCopy[k];
	winlen = (int) ceil((float)numVoicedFrames/scaleFactor);
	Rfft(sigCopy,sigCep,ay,mfft,nfft,1);
	
	for (k = 1; k <= winlen; k++){
	  sigCopy[k] = sigCep[k]*HanW(k,winlen);
	}
	for (k = winlen+1; k <= nfft; k++)
	  sigCopy[k] = 0;
	
	Rfft(sigCopy,ax,ay,mfft,nfft,-1);
	SpectrumReal(nfft,ax,ay,amag,phase);
	
	for (k = 1; k < nfby2-1; k++){
	  derv[k] = phase[k] - phase[k+1];  
	  negDerv[k] = phase[k+1] - phase[k];
	}
	derv[nfby2] = derv[nfby2-1];
	negDerv[nfby2] = negDerv[nfby2-1];
	RemoveAverage(derv, nfby2, &average);
	RemoveAverage(negDerv, nfby2, &average);
	dervSmth = (float *) MedianSmoothArray(derv, nfby2, 
					       medianOrder, dervSmth);
	negDervSmth = (float *) MedianSmoothArray(negDerv, nfby2, 
						medianOrder, negDervSmth);
	rMaxDerv = derv[Imax(derv,nfby2)];
	rMaxNegDerv = negDerv[Imax(negDerv, nfby2)];
	for (k = 1; k <= numVoicedFrames; k++)
	  if (signal[k+startVFrameNumber-1] != 0.0)
	    fprintf(spectraFile,"\n %e %e %e %e %e %e %e %e %e %e",
		  (k+startVFrameNumber-1)*(float)frameAdvanceSamples/(float) samplingRate,
		    signal[k+startVFrameNumber-1],sigEgy[k+startVFrameNumber-1],sigCopy[k+startVFrameNumber-1], phase[k], amag[k],
		  dervSmth[k]/fabs(rMaxDerv), negDervSmth[k]/fabs(rMaxNegDerv),sigSymmetric[k+startVFrameNumber-1],sigEgyInv[k+startVFrameNumber-1]);
	  else 
	    fprintf(spectraFile,"\n %e %e %e %e %e %e %e %e %e",
		  (k+startVFrameNumber-1)*(float)frameAdvanceSamples/(float) samplingRate,
		    0.0, 0.0, 0.0, phase[k],amag[k],
		  dervSmth[k]/fabs(rMaxDerv), negDervSmth[k]/fabs(rMaxNegDerv),0.0,0.0);

	FindPeaks(derv,peakArr, nfby2);
	FindPeaks(negDerv,valleyArr,nfby2);
	segBoundary = 0;
	for (j = 1; j <= numVoicedFrames; j++){
	  if(peakArr[j] == 1) { 
	     if(dervSmth[j]/fabs(rMaxDerv) > thresEnergy1) {
	       fprintf(boundaryFile, "%d %f %d ", 
		       segBoundary+startVFrameNumber -1, 
		       (float) (segBoundary+startVFrameNumber-1)*
		       frameAdvanceSamples/(float) samplingRate, 1);
	       fprintf(boundaryFile, "%d %f %d\n", j+startVFrameNumber-2, 
		       (float) (j+startVFrameNumber-2)*
		       frameAdvanceSamples/samplingRate, 1);
	       segBoundary = j;
	     } else if(dervSmth[j]/fabs(rMaxDerv) > thresEnergy2) {
	       fprintf(boundaryFile, "%d %f %d ", 
		       segBoundary+startVFrameNumber -1, 
		       (float) (segBoundary+startVFrameNumber-1)*frameAdvanceSamples/(float) samplingRate, 2);
	       fprintf(boundaryFile, "%d %f %d\n", j+startVFrameNumber-2, 
		       (float) (j+startVFrameNumber-2)*frameAdvanceSamples/samplingRate, 2);
	       segBoundary = j;
	     } else {
               fprintf(boundaryFile, "%d %f %d ",
                       segBoundary+startVFrameNumber -1,
                       (float) (segBoundary+startVFrameNumber-1)*frameAdvanceSamples/(float) samplingRate, 3);
               fprintf(boundaryFile, "%d %f %d\n", j+startVFrameNumber-2,
                       (float) (j+startVFrameNumber-2)*frameAdvanceSamples/samplingRate, 3);
               segBoundary = j;
             }
	  }
	}
	/* Fix end of voiced segment */
	if (segBoundary < numVoicedFrames) {
	  fprintf(boundaryFile, "%d %f %d ", 
		  segBoundary + startVFrameNumber-1, (float) (segBoundary + startVFrameNumber -1)*frameAdvanceSamples/(float) samplingRate, +1);
	fprintf(boundaryFile, "%d %f %d\n", startVFrameNumber+numVoicedFrames-2, 
		(float) (startVFrameNumber+numVoicedFrames-2)*frameAdvanceSamples/samplingRate, +1);
	}
      }
    }
  }

 for (k = 1; k <= nfft; k++)
            fprintf(symmetryFile,"\n%e %e",sigSymmetric[k],sigCep[k]);

  free(ax);
  free(ay);
  free(amag);
  free(phase);
  free(derv);
  free(negDerv);
  free(sigCopy);
  free(sigNonZero);
  free(signal);
  free(peakArr);
  free(sigCep);
  free(dervSmth);
  free(negDervSmth);
  free(waveform->array);
  free(waveform);
  free (energy->array);
  free(energy);			    
  //  fprintf(boundaryFile,"\n");
  fprintf(spectraFile,"\n");
  fclose(spectraFile);
  fclose(ctrlFile);
  fclose(boundaryFile);
  free(valleyArr);
  return(0);
  }		 
	/*  End of main		End of main   */

















/*-------------------------------------------------------------------------
 * $Log: WordsWithSilenceRemoval.c,v $
 * Revision 1.1  2013/02/08 10:23:34  hema
 * Initial revision
 *
 *
 *
 * Local Variables:
 * time-stamp-active: t
 * time-stamp-line-limit: 20
 * time-stamp-start: "Last modified:[ 	]+"
 * time-stamp-format: "%3a %02d-%3b-%:y %02H:%02M:%02S by %u"
 * time-stamp-end: "$"
 * End:
 *                        End of WordsWithSilenceRemoval.c
 -------------------------------------------------------------------------*/
