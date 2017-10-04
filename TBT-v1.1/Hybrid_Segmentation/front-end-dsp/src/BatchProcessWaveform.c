/*-------------------------------------------------------------------------
 *  BatchProcessWaveform.c - Extracts features from speech signal
 *  ersion:	$Name:  $
 *  Module:	
 *
 *  Purpose:	
 *  See:	
 *
 *  Author:	Hema A Murthy (hema@bhairavi.iitm.ernet.in)
 *
 *  Created:        Fri 19-Apr-2002 16:09:27
 *  Last modified:  Mon 23-Feb-2015 14:35:03 by hema
 *  $Id: BatchProcessWaveform.c,v 1.15 2011/06/08 15:44:08 hema Exp hema $
 *
 *  Bugs:	
 *
 *  Change Log:	<Date> <Author>
 *  		<Changes>
 -------------------------------------------------------------------------*/


#include "stdio.h" 
#include "stdlib.h"
#include "math.h"
#include "string.h"
//#include "malloc.h"
#include "fe/constants.h"
#include "sp/sphere.h"
#include "fe/FrontEndDefs.h"
#include "fe/FrontEndTypes.h"
#include "fe/InitAsdf.h"
#include "fe/DspLibrary.h"
#include "fe/PthLibrary.h"
#include "fe/FmtsLibrary.h"
#include "fe/BatchProcessWaveform.h"

/*--------------------------------------------------------------------------
 FrameComputeWaveform : Gets the frame of speech corresponding to
                          from the speech waveform.

 Inputs : waveform, frameIndex, frameShift, frameSize, total samples in
          waveform.

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeWaveform(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, samples,frameShift, preemphasisDelay;
  static                         short *waveform;
  static float                   preemphasis, gausMin;
  static                         char oldName[500];
  static int                     filterOrder;
  static float                   *filterCoefsNum=NULL,*signal=NULL, *filteredSignal=NULL;
  static int                     samplingRate, numPitch;
  static float                   minFrequency, maxFrequency;
  static int                    windowType, windowShape, pitchSync, flag=0; 
  int                            i;
  int                          pitchValue, pitchIndexInFrame;
  

  if((prevAsdf == NULL) || (strcmp(oldName,asdf->waveFileName) != 0)){
    windowSize = GetIAttribute(asdf,"windowSize");
    samples = GetIAttribute(asdf,"numSamples");
    preemphasis = GetFAttribute(asdf,"preemphasis");
    preemphasisDelay = GetIAttribute (asdf, "preemphasisDelay");
    frameShift =GetIAttribute(asdf,"frameAdvanceSamples");
    minFrequency = GetFAttribute (asdf, "minFrequency");
    maxFrequency = GetFAttribute (asdf, "maxFrequency");
    samplingRate = GetIAttribute (asdf, "samplingRate");
    windowType = asdf->windowType;
    windowShape = asdf->windowShape;
    numPitch = GetIAttribute(asdf, "numPitch");
    pitchSync = GetIAttribute(asdf, "pitchSync");
    gausMin = GetFAttribute (asdf, "gausMin");
    waveform = (short *) GetPtrAttribute(asdf,"waveform");
    filterOrder = GetIAttribute(asdf, "filterOrder");
    if (filterOrder != 0) {
      filterCoefsNum = (float *) malloc ((filterOrder)*sizeof(float));
      // filterCoefsDenom  =(float *) malloc ((filterOrder)*sizeof(float));
      signal = (float *) malloc ((windowSize+filterOrder)*sizeof(float));
           filteredSignal = (float *) malloc ((windowSize+filterOrder)*sizeof(float));
      //ButterWorthFilter(minFrequency, maxFrequency, samplingRate, filterOrder, filterCoefsNum, filterCoefsDenom);
      filterCoefsNum = (float *) BandPassFilter(minFrequency, maxFrequency, samplingRate, filterOrder);
      for (i = 0; i < windowSize+filterOrder; i++) {
	signal[i] = 0;
	filteredSignal[i] = 0;
      }
    }
    oldName[0] = '\0';
    strcpy(oldName,asdf->waveFileName);
    prevAsdf = asdf;
    /*   printf("prevName = %s\n",prevAsdf->waveFileName);*/
  }
  fvect->numElements = windowSize;
  if ((pitchSync == 1) && (asdf->wavePthValues[frameIndex] != 0)){
    pitchValue = (int) asdf->wavePthValues[frameIndex];
    pitchIndexInFrame = ImaxShort0(&(waveform[frameShift*frameIndex+windowSize/2-pitchValue]),2*pitchValue);
    while (2*numPitch*pitchValue > windowSize)
      numPitch--;
    for (i = 0; i < 2*numPitch*pitchValue; i++) 
      if ((((frameShift*frameIndex+i+pitchIndexInFrame-numPitch*pitchValue+preemphasisDelay) > 0) && 
	   ((frameShift*frameIndex+i+pitchIndexInFrame+numPitch*pitchValue+preemphasisDelay) < samples)) && 
	  (((frameShift*frameIndex+i+pitchIndexInFrame -numPitch*pitchValue)> 0) &&
	   ((frameShift*frameIndex+i+pitchIndexInFrame +numPitch*pitchValue)< samples))) {
	fvect->array[i] = waveform[frameShift*frameIndex+i+pitchIndexInFrame-numPitch*pitchValue+preemphasisDelay]
	  - preemphasis*waveform[frameShift*frameIndex+i+pitchIndexInFrame-numPitch*pitchValue];
	if (fvect->array[i] != 0)
	  flag = 1;  /*indicates that there are nonzero values in a frame */
      } else {
	/*      printf("frameIndex = %d waveform ended assigning zeros\n",frameIndex); */
	fvect->array[i] = 0.0;
      }
    for (i = 2*numPitch*pitchValue; i < windowSize; i++)
      fvect->array[i] = 0;
  } else {
    for (i = 0; i < windowSize; i++)
      if ((frameShift*frameIndex+i+preemphasisDelay < samples) 
	  && (frameShift*frameIndex+i< samples)) {
	fvect->array[i] = waveform[frameShift*frameIndex+i+preemphasisDelay]
	  - preemphasis*waveform[frameShift*frameIndex+i];
	if (fvect->array[i] != 0)
	  flag = 1;  /*indicates that there are nonzero values in a frame */
      } else {
	/*      printf("frameIndex = %d waveform ended assigning zeros\n",frameIndex); */
	fvect->array[i] = 0.0;
      }
  }
  if (filterOrder != 0) {
    for (i = 0; i < filterOrder/2; i++){
      signal[i] = signal[i+frameShift];
    }
    for (i = 0; i < windowSize; i++) 
      signal[i+filterOrder/2] = fvect->array[i];
    MASignal(signal, filteredSignal, windowSize, filterCoefsNum, filterOrder);
    for (i = 0; i < windowSize; i++) 
      fvect->array[i] = filteredSignal[i+filterOrder/2];
  }
  if (pitchSync == 0)
    Window(fvect->array, windowSize, (char) windowType, (char) windowShape, gausMin);
  else 
    Window (fvect->array, 2*numPitch*pitchValue, windowType, windowShape, gausMin); 
  /* The Following fix - just in case one ends up with a frame in
     which all values are zeroes */
  /*  if (flag == 0)
    for (i = 0; i < windowSize; i++)
    fvect->array[i] = 1.0; */
  return(fvect);
}


/*--------------------------------------------------------------------------
  FrameComputeSpectralFlatness: Computes the Spectral Flatness of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frame_index           : frame_number 
     energy                : F_VECTOR in which spectral flatness is to be returned
     
  outputs :
     energy        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeSpectralFlatness(ASDF *asdf, int frameIndex, F_VECTOR *spectralFlatness) 
{
  static int                 windowSize, frameShift;
 static ASDF                *prevAsdf=NULL;
 static F_VECTOR            *waveform;
 static float               *residual;
 static int                 LPOrder;
 int i;
 if (prevAsdf != asdf) {
   windowSize = GetIAttribute(asdf,"windowSize");
   frameShift  = GetIAttribute(asdf,"frameAdvanceSamples");
   LPOrder     = GetIAttribute(asdf, "lpOrder");

   waveform = (F_VECTOR *) AllocFVector(windowSize);
   residual = (float *) AllocFloatArray(residual, windowSize+1);
   prevAsdf = asdf;
}
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf,frameIndex,waveform);
  spectralFlatness->numElements = 1;
  //  for (i = 0; i < windowSize; i++)
  //  waveform->array[i] = HamDw(i+1,windowSize)*waveform->array[i];
  spectralFlatness->array[0] =  (float) ComputeSpectralFlatness(waveform->array, residual, LPOrder, windowSize, frameShift);
  return(spectralFlatness);
}


/*--------------------------------------------------------------------------
  FrameComputeZeroCrossing : Computes the ZeroCrossing Rate of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frame_index           : frame_number 
     zerocrossing                : F_VECTOR in which zerocrossing is to be returned
     
  outputs :
     zerocrossing        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeZeroCrossing(ASDF *asdf, int frameIndex, F_VECTOR *zeroCrossing) 
{
 static int                 windowSize;
 static ASDF                *prevAsdf=NULL;
 static F_VECTOR            *waveform;
 float               value;
 int i;
 if (prevAsdf != asdf) {
   windowSize = GetIAttribute(asdf,"windowSize");
   waveform = (F_VECTOR *) AllocFVector(windowSize);
   prevAsdf = asdf;
}
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf,frameIndex,waveform);
  zeroCrossing->numElements = 1;
  //  for (i = 0; i < windowSize; i++)
  //  waveform->array[i] = HamDw(i+1,windowSize)*waveform->array[i];
  value =  (float) ComputeZeroCrossing(waveform->array, windowSize);
  zeroCrossing->array[0] = value;
  return(zeroCrossing);
}
/*--------------------------------------------------------------------------
  FrameComputeEnergy : Computes the energy of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frame_index           : frame_number 
     energy                : F_VECTOR in which energy is to be returned
     
  outputs :
     energy        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeEnergy(ASDF *asdf, int frameIndex, F_VECTOR *energy) 
{
 static int                 windowSize;
 static float               gamma;
 static ASDF                *prevAsdf=NULL;
 static F_VECTOR            *waveform;
 int i;
 if (prevAsdf != asdf) {
   windowSize = GetIAttribute(asdf,"windowSize");
   gamma = GetFAttribute(asdf, "gamma");
   waveform = (F_VECTOR *) AllocFVector(windowSize);
   prevAsdf = asdf;
}
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf,frameIndex,waveform);
  energy->numElements = 1;
  //  for (i = 0; i < windowSize; i++)
  //waveform->array[i] = HamDw(i+1,windowSize)*waveform->array[i];
  energy->array[0] =  (float) ComputeEnergy(waveform->array, windowSize, gamma);	
  return(energy);
}

/*--------------------------------------------------------------------------
  FrameComputeEnergy : Computes the energy of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frame_index           : frame_number 
     energy                : F_VECTOR in which energy is to be returned
     
  outputs :
     energy        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeSpectralFlux(ASDF *asdf, int frameIndex, F_VECTOR *specFlux) 
{
  static int                 fftSize, fftSizeBy2, normalizeSpecFlux;
 static ASDF                *prevAsdf=NULL;
 static F_VECTOR            *FFTSpecPrev=NULL, *FFTSpecCurrent=NULL;
 float                      specAverage;
 int i;
 if (prevAsdf != asdf) {
   fftSize = GetIAttribute(asdf,"fftSize");
   fftSizeBy2 = fftSize/2;
   normalizeSpecFlux = GetIAttribute(asdf,"normalizeSpecFlux");
   FFTSpecPrev = (F_VECTOR *) AllocFVector(fftSizeBy2);
   FFTSpecCurrent = (F_VECTOR *) AllocFVector(fftSizeBy2);
   prevAsdf = asdf;
}
  FFTSpecCurrent = (F_VECTOR *) FrameComputeFFTSpectrum(asdf,frameIndex,FFTSpecCurrent);
  if (frameIndex > 0) 
    FFTSpecPrev = (F_VECTOR *) FrameComputeFFTSpectrum(asdf,frameIndex-1,FFTSpecPrev);
  else 
   FFTSpecPrev = (F_VECTOR *) FrameComputeFFTSpectrum(asdf,frameIndex,FFTSpecPrev);
  if (normalizeSpecFlux == 1) {
    specAverage = 0;
    for (i = 0; i < fftSizeBy2; i++)
      specAverage = specAverage + FFTSpecPrev->array[i]*FFTSpecPrev->array[i];
    for (i = 0; i < fftSizeBy2; i++)
      FFTSpecPrev->array[i] = FFTSpecPrev->array[i]*FFTSpecPrev->array[i]/specAverage;
  } else 
    if (normalizeSpecFlux == 2) {
      specAverage = FFTSpecPrev->array[Imax0Actual(FFTSpecPrev->array, fftSizeBy2)];
      specAverage = specAverage*specAverage;
      for (i = 0; i <= fftSizeBy2; i++)
	FFTSpecPrev->array[i] = FFTSpecPrev->array[i]*FFTSpecPrev->array[i]/specAverage;
    }
  if (normalizeSpecFlux == 1) {
    specAverage = 0;
    for (i = 0; i < fftSizeBy2; i++)
      specAverage = specAverage + FFTSpecCurrent->array[i]*FFTSpecCurrent->array[i];
    for (i = 0; i < fftSizeBy2; i++)
      FFTSpecCurrent->array[i] = FFTSpecCurrent->array[i]*FFTSpecCurrent->array[i]/specAverage;
  } else
    if (normalizeSpecFlux == 2) {
      specAverage = FFTSpecCurrent->array[Imax0Actual(FFTSpecCurrent->array, fftSizeBy2)];
      specAverage = specAverage*specAverage;
      for (i = 0; i < fftSizeBy2; i++)
	FFTSpecCurrent->array[i] = FFTSpecCurrent->array[i]*FFTSpecCurrent->array[i]/specAverage;
    }
  specFlux->array[0] =  0;
  for (i = 0; i < fftSizeBy2; i++)
    specFlux->array[0] = specFlux->array[0] + (FFTSpecCurrent->array[i]- FFTSpecPrev->array[i])*(FFTSpecCurrent->array[i] - FFTSpecPrev->array[i]);

  specFlux->numElements = 1;
  
  specFlux->array[0] =  specFlux->array[0]/fftSizeBy2;
  return(specFlux);
}



F_VECTOR *FrameComputeSpectralFluxBandEnergies(ASDF *asdf, int frameIndex, F_VECTOR *specFlux) 
{
  static int                 fftSize, fftSizeBy2, normalizeSpecFlux;
 static ASDF                *prevAsdf=NULL;
 static F_VECTOR            *FFTSpecPrev=NULL, *FFTSpecCurrent=NULL;
 float                      specAverage, spec1, spec2, spec3, spec4, spec5, spec6, spec7, spec8;
 int i;
 if (prevAsdf != asdf) {
   fftSize = GetIAttribute(asdf,"fftSize");
   fftSizeBy2 = fftSize/2;
   normalizeSpecFlux = GetIAttribute(asdf,"normalizeSpecFlux");
   FFTSpecPrev = (F_VECTOR *) AllocFVector(fftSizeBy2);
   FFTSpecCurrent = (F_VECTOR *) AllocFVector(fftSizeBy2);
   prevAsdf = asdf;
}
  FFTSpecCurrent = (F_VECTOR *) FrameComputeFFTSpectrum(asdf,frameIndex,FFTSpecCurrent);
  if (frameIndex > 0) 
    FFTSpecPrev = (F_VECTOR *) FrameComputeFFTSpectrum(asdf,frameIndex-1,FFTSpecPrev);
  else 
   FFTSpecPrev = (F_VECTOR *) FrameComputeFFTSpectrum(asdf,frameIndex,FFTSpecPrev);
  if (normalizeSpecFlux == 1) {
    specAverage = 0;
    for (i = 0; i < fftSizeBy2; i++)
      specAverage = specAverage + FFTSpecPrev->array[i]*FFTSpecPrev->array[i];
    for (i = 0; i < fftSizeBy2; i++)
      FFTSpecPrev->array[i] = FFTSpecPrev->array[i]*FFTSpecPrev->array[i]/specAverage;
  } else 
    if (normalizeSpecFlux == 2) {
      specAverage = FFTSpecPrev->array[Imax0Actual(FFTSpecPrev->array, fftSizeBy2)];
      specAverage = specAverage*specAverage;
      for (i = 0; i <= fftSizeBy2; i++)
	FFTSpecPrev->array[i] = FFTSpecPrev->array[i]*FFTSpecPrev->array[i]/specAverage;
    }
  if (normalizeSpecFlux == 1) {
    specAverage = 0;
    for (i = 0; i < fftSizeBy2; i++)
      specAverage = specAverage + FFTSpecCurrent->array[i]*FFTSpecCurrent->array[i];
    for (i = 0; i < fftSizeBy2; i++)
      FFTSpecCurrent->array[i] = FFTSpecCurrent->array[i]*FFTSpecCurrent->array[i]/specAverage;
  } else
    if (normalizeSpecFlux == 2) {
      specAverage = FFTSpecCurrent->array[Imax0Actual(FFTSpecCurrent->array, fftSizeBy2)];
      specAverage = specAverage*specAverage;
      for (i = 0; i < fftSizeBy2; i++)
	FFTSpecCurrent->array[i] = FFTSpecCurrent->array[i]*FFTSpecCurrent->array[i]/specAverage;
    }
  specFlux->array[0] =  0;
  spec1=0;
  spec2=0;
  spec3=0;
  spec4=0;
  spec5=0;
  spec6=0;
  spec7=0;
  spec8=0;
  /*for (i = 0; i < fftSize/8; i++)
    spec1 = spec1 + (FFTSpecCurrent->array[i]- FFTSpecPrev->array[i]);

  for (i = fftSize/8; i < fftSize/4; i++)
    spec2 = spec2 + (FFTSpecCurrent->array[i]- FFTSpecPrev->array[i]);

  for (i = fftSize/4; i < (fftSize/8 + fftSize/4); i++)
    spec3 = spec3 + (FFTSpecCurrent->array[i]- FFTSpecPrev->array[i]);

  for (i = (fftSize/8 + fftSize/4); i < fftSize/2; i++)
    spec4 = spec4 + (FFTSpecCurrent->array[i]- FFTSpecPrev->array[i]);
  
  specFlux->array[0] = (spec1*spec1) + (spec2*spec2) + (spec3*spec3) + (spec4*spec4);
  //specFlux->array[0] = spec1; */
  for (i = 0; i < fftSize/8; i++)
    spec1 = spec1 + (FFTSpecCurrent->array[i]);

  for (i = fftSize/8; i < fftSize/4; i++)
    spec2 = spec2 + (FFTSpecCurrent->array[i]);

  for (i = fftSize/4; i < (fftSize/8 + fftSize/4); i++)
    spec3 = spec3 + (FFTSpecCurrent->array[i]);

  for (i = (fftSize/8 + fftSize/4); i < fftSize/2; i++)
    spec4 = spec4 + (FFTSpecCurrent->array[i]);
  
  for (i = 0; i < fftSize/8; i++)
    spec5 = spec5 + (FFTSpecPrev->array[i]);

  for (i = fftSize/8; i < fftSize/4; i++)
    spec6 = spec6 + (FFTSpecPrev->array[i]);

  for (i = fftSize/4; i < (fftSize/8 + fftSize/4); i++)
    spec7 = spec7 + (FFTSpecPrev->array[i]);

  for (i = (fftSize/8 + fftSize/4); i < fftSize/2; i++)
    spec8 = spec8 + (FFTSpecPrev->array[i]);
  
  //specFlux->array[0] = (spec1*spec1) + (spec2*spec2) + (spec3*spec3) + (spec4*spec4);
  specFlux->array[0] = ((spec1-spec5)*(spec1-spec5))+((spec2-spec6)*(spec2-spec6))+((spec3-spec7)*(spec3-spec7))+((spec4-spec8)*(spec4-spec8)); 
  specFlux->numElements = 1;
  
  //specFlux->array[0] =  specFlux->array[0]/fftSizeBy2;
  return(specFlux);
}



/*-------------------------------------------------------------------------
 *  FrameComputeCheckVad -- Determines whether a given frame is voiced or unvoiced
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned a flag in a F_VECTOR (flag =1, if UV, flag=0, if V)
 *    Bugs:	
 * -------------------------------------------------------------------------*/
/*F_VECTOR * FrameComputeCheckVad(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numFrames, vad, numVoicedFrames = 0;
  static int        percentFrames, numSamples, windowSize, frameAdvanceSamples, LPOrder;
  static float     thresEnergy, thresZeroCrossing, thresSpectralFlatness;
  static short     *voicedUnvoiced = NULL;
  static  ASDF     *prevAsdf = NULL;
  int              i;
  if (prevAsdf == NULL) {
    vad = GetIAttribute(asdf, "vad");
    thresEnergy = GetFAttribute(asdf, "thresEnergy");
    thresZeroCrossing = GetFAttribute(asdf, "thresZero");
    thresSpectralFlatness = GetFAttribute(asdf, "thresSpecFlatness");
    percentFrames = GetIAttribute(asdf, "percentFrames");
    windowSize = GetIAttribute (asdf, "windowSize");
    frameAdvanceSamples = GetIAttribute(asdf, "frameAdvanceSamples");
    numSamples = GetIAttribute(asdf, "numSamples");
    numFrames = GetIAttribute(asdf, "numFrames");
    LPOrder = GetIAttribute(asdf, "lpOrder");
    voicedUnvoiced   = (short *)malloc((numFrames+10)*sizeof(short));
    if (vad == 1)
      VoicedUnvoiced(asdf->waveform, numSamples, voicedUnvoiced, frameAdvanceSamples, windowSize, LPOrder, 
		  thresZeroCrossing, thresEnergy, thresSpectralFlatness, percentFrames);                    
    else
      for (i = 0; i < numFrames; i++)
        voicedUnvoiced[i] = 1;
    for (i = 0; i < numFrames; i++)
      if (voicedUnvoiced[i])
        numVoicedFrames++;
    PutIAttribute(asdf, "numVoicedFrames", numVoicedFrames);
    prevAsdf = asdf;
   
  }
  if (voicedUnvoiced[frameIndex]) 
    fvect->array[0] = 1;
  else
    fvect->array[0] = 0;
  return(fvect);
      
}	End of FrameComputeCheckVad	*/	


/*--------------------------------------------------------------------------
  FrameComputeSignificantChange : Determines if there is a significant change in adjacent frames
  inputs :
     asdf                  : front-end-parameters
     frame_index           : frame_number 
     sigChange             : F_VECTOR in which significant change is to be returned
     
  outputs :
     sigChange             : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeSignificantChange(ASDF *asdf, int frameIndex, F_VECTOR *sigChange) 
{
  static int                 windowSize, frameAdvanceSamples;
 static ASDF                *prevAsdf=NULL;
 static F_VECTOR            *waveform1, *waveform2;
 static float               thresEnergy, thresZero, thresSpecFlatness;
 if (prevAsdf != asdf) {
   windowSize = GetIAttribute(asdf,"windowSize");
   frameAdvanceSamples = GetIAttribute(asdf,"frameAdvanceSamples");
   thresEnergy = GetFAttribute(asdf,"thresEnergy");
   thresZero = GetFAttribute(asdf,"thresZero");
   thresSpecFlatness = GetFAttribute(asdf,"thresSpecFlatness");
   waveform1 = (F_VECTOR *) AllocFVector(windowSize);
   waveform2 = (F_VECTOR *) AllocFVector(windowSize);
   prevAsdf = asdf;
}
  waveform1 = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex-1, waveform1);
  waveform2 = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform2);
  sigChange->numElements = 1;
  sigChange->array[0] = (float) (
   GradientFrames(waveform1->array, waveform2->array, windowSize, frameAdvanceSamples, 
		  thresEnergy, thresZero, thresSpecFlatness));
  return(sigChange);
}

/*--------------------------------------------------------------------------
  FrameComputeDeltaEnergy : Computes the deltaEnergy of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaEnergy        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaEnergy(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numEnergy,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numEnergy = 1;
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numEnergy);
   next = (F_VECTOR *) AllocFVector(numEnergy);
   temp = (F_VECTOR *) AllocFVector(numEnergy);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 fvect->array[0] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeEnergy(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeEnergy(asdf,frameIndex+i,next);
   else 
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);   
 }
   LinearVectorScalarDivide(normalizingConst,fvect,fvect);
  return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaEnergy : Computes the deltaDeltaEnergy of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaEnergy returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaEnergy(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numEnergy,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numEnergy = 1;
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numEnergy);
   next = (F_VECTOR *) AllocFVector(numEnergy);
   temp = (F_VECTOR *) AllocFVector(numEnergy);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 fvect->array[0] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])) 
     prev = (F_VECTOR *) FrameComputeDeltaEnergy(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeDeltaEnergy(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeLogEnergy : Computes the log energy of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frame_index           : frame_number 
     energy                : F_VECTOR in which log energy is to be returned
     
  outputs :
     energy        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeLogEnergy(ASDF *asdf, int frameIndex, F_VECTOR *energy) 
{
 static int                 windowSize;
 static ASDF                *prevAsdf=NULL;
 static F_VECTOR            *waveform;
 static float               frameEnergy;
 int i;
 if (prevAsdf != asdf) {
   windowSize = GetIAttribute(asdf,"windowSize");
   waveform = (F_VECTOR *) AllocFVector(windowSize);
   prevAsdf = asdf;
}
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf,frameIndex,waveform);
  energy->numElements = 1;
  //  for (i = 0; i < windowSize; i++)
  //  waveform->array[i] = HamDw(i+1,windowSize)*waveform->array[i];
  frameEnergy = (float) (ComputeEnergy(waveform->array, windowSize, 1.0));
  if (frameEnergy <= 10E-10)
    frameEnergy = 10E-10;
  energy->array[0] =  log(frameEnergy); 
  return(energy);
}

/*--------------------------------------------------------------------------
  FrameComputeDeltaLogEnergy : Computes the deltaLogEnergy of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaEnergy        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaLogEnergy(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numEnergy,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numEnergy = 1;
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numEnergy);
   next = (F_VECTOR *) AllocFVector(numEnergy);
   temp = (F_VECTOR *) AllocFVector(numEnergy);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 fvect->array[0] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeLogEnergy(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeLogEnergy(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaLogEnergy : Computes the deltaDeltaEnergy of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaLogEnergy returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaLogEnergy(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numEnergy,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numEnergy = 1;
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numEnergy);
   next = (F_VECTOR *) AllocFVector(numEnergy);
   temp = (F_VECTOR *) AllocFVector(numEnergy);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 fvect->array[0] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])) 
     prev = (F_VECTOR *) FrameComputeDeltaLogEnergy(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeDeltaLogEnergy(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeLinearCepstrumRaw : Computes the cepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     cepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeLinearCepstrumRaw(ASDF *asdf, int frameIndex, F_VECTOR *cepstrum) 
{
  static int      mfft,nfft,numCepstrum,windowSize;
  static ASDF     *prevAsdf=NULL;
  static float     *resAx, *resAy, *amag, *phase, *signal;
 static F_VECTOR  *waveform;
 int              i;

 if (prevAsdf != asdf) {
   mfft = GetIAttribute(asdf, "fftOrder");
   nfft = GetIAttribute(asdf,"fftSize");
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   windowSize = GetIAttribute(asdf,"windowSize");
   waveform = (F_VECTOR *) AllocFVector(windowSize+1);
   resAx = (float *) malloc((nfft+1)*sizeof(float));
   resAy = (float *) malloc((nfft+1)*sizeof(float));
   amag = (float *) malloc((nfft+1)*sizeof(float));
   phase = (float *) malloc((nfft+1)*sizeof(float));
   signal = (float *) calloc(nfft+1, sizeof(float));
   prevAsdf = asdf;
   waveform = (F_VECTOR *) AllocFVector (windowSize);
}
 waveform = (F_VECTOR *) FrameComputeWaveform(asdf,frameIndex,waveform);
 for (i = 0; i < windowSize; i++)
   signal[i+1] = waveform->array[i];
 Rfft(signal, resAx, resAy, mfft, nfft, -1);
 SpectrumReal(nfft, resAx, resAy, amag, phase);
 for ( i=1; i<=nfft; i++ ) 
   if (amag[i] != 0)
     amag[i] = log(amag[i]);
   else amag[i] = log(1.0E-10);
 Rfft(amag,resAx,resAy,mfft,nfft,1);
 cepstrum->numElements = numCepstrum;
  for ( i=1; i<=numCepstrum; i++ )
     cepstrum->array[i-1] = resAx[i+1];
  return(cepstrum);
}
/*-------------------------------------------------------------------------
 *  FrameComputeLinearCepstrumMean -- Computes the Cepstrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeLinearCepstrumMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames, fftSize;
  static F_VECTOR  *meanVector=NULL;
 int              i,j;
 int              numVoicedFrames = 0;
 numCepstrum = GetIAttribute(asdf,"numCepstrum");
 numFrames = GetIAttribute(asdf, "numFrames");
 fftSize = GetIAttribute(asdf, "fftSize");
 numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
 meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
 for (i = 0; i < numCepstrum; i++)
   meanVector->array[i] = 0;
 for (i = 0; i < numFrames; i++)
   if (asdf ->vU[i]) {
     fvect = (F_VECTOR *) FrameComputeLinearCepstrumRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ )
       meanVector->array[j] = fvect->array[j] + meanVector->array[j];
   }
 for ( i = 0; i < numCepstrum; i++ )
   fvect->array[i] = meanVector->array[i]/numVoicedFrames;
return(fvect);

}	/*  End of FrameComputeLinearCepstrumMean	*/	



/*-------------------------------------------------------------------------
 *  FrameComputeLinearCepstrumVariance -- Computes the 
 *                                     -- LinearCepstrum Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeLinearCepstrumVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeLinearCepstrumMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeLinearCepstrumRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeLinearCepstrumVariance	*/	



/*--------------------------------------------------------------------------
  FrameComputeLinearCepstrum : Computes the cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeLinearCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector, *varVector;
  static int       zeroMean, featureVarNormalize;
  static char      oldName[500];
  int              i;
  static float     sqrtValue;

  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    zeroMean = (int) GetIAttribute(asdf, "zeroMean");
    featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
    if ((zeroMean) || (featureVarNormalize)){
      meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
      varVector = (F_VECTOR *) AllocFVector(numCepstrum);
    }
    prevAsdf = asdf;
  }
  if (((zeroMean) || (featureVarNormalize))&& (strcmp(oldName, asdf->waveFileName) != 0)) {
    meanVector = (F_VECTOR *) FrameComputeLinearCepstrumMean(asdf, 0, meanVector);
    varVector = (F_VECTOR *) FrameComputeLinearCepstrumVariance(asdf, 0, varVector);
    oldName[0] = '\0';
    strcpy(oldName, asdf->waveFileName);
  }
  
  fvect = (F_VECTOR *) FrameComputeLinearCepstrumRaw (asdf, frameIndex, fvect);
  if (zeroMean) {
    for (i = 0; i < numCepstrum; i++)
      fvect->array[i] = fvect->array[i] - meanVector->array[i];
  }
  if (featureVarNormalize) 
    for (i = 0; i < numCepstrum; i++) {
      sqrtValue = sqrtf(varVector->array[i]);
      fvect->array[i] = fvect->array[i]/sqrtValue;
    }
  return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeLinearDeltaCepstrum : Computes the deltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeLinearDeltaCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])) 
     prev = (F_VECTOR *) FrameComputeLinearCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeLinearCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
     LinearVectorDifference(prev,next,temp);
     LinearVectorScalarMultiply((float)(i),temp,temp);
     LinearVectorAddition(temp,fvect,fvect);

 }

   LinearVectorScalarDivide(normalizingConst,fvect,fvect);
  return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeLinearDeltaDeltaCepstrum : Computes the deltaDeltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaCepstrum returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeLinearDeltaDeltaCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])) 
     prev = (F_VECTOR *) FrameComputeLinearDeltaCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeLinearDeltaCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeLinearAugmentedCepstrum : Computes the augmentedCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeLinearAugmentedCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeLinearCepstrum(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeLinearDeltaCepstrum(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeLinearDeltaDeltaCepstrum(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeLogEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaLogEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaLogEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}
 
/*--------------------------------------------------------------------------
  FrameComputeLPCepstrumRaw : Computes the LPcepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     cepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeLPCepstrumRaw(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum, windowSize, frameAdvanceSamples;
  static ASDF      *prevAsdf=NULL;
  static float     *coef, *resEnergy, *signal, *lpCepstrum;
  float            gain;
  static F_VECTOR  *waveform; 
  static int       i, LPOrder;

 if (prevAsdf != asdf) {
   //   mfft = GetIAttribute(asdf, "fftOrder");
   //  nfft = GetIAttribute(asdf,"fftSize");
   LPOrder = GetIAttribute(asdf, "lpOrder");
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   windowSize = GetIAttribute(asdf,"windowSize");
   frameAdvanceSamples = GetIAttribute(asdf,"frameAdvanceSamples");
   waveform = (F_VECTOR *) AllocFVector(windowSize);
   signal = (float *) calloc (windowSize+1, sizeof(float));
   //resAx = (float *) malloc((windowSize+1)*sizeof(float));
   resEnergy = (float *) malloc((windowSize+1)*sizeof(float));
   //   resAy = (float *) malloc((nfft+1)*sizeof(float));
   coef = (float *) malloc((LPOrder+2)*sizeof(float));
   lpCepstrum = (float *) malloc((numCepstrum+1)*sizeof(float));
   prevAsdf = asdf;
}
 waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
 for (i = 0; i < windowSize; i++)
   signal[i+1] = waveform->array[i];
 LpAnal(signal, resEnergy, windowSize, frameAdvanceSamples, coef, LPOrder, &gain);
 LPCepstrum(coef, LPOrder, lpCepstrum, numCepstrum, gain);
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = lpCepstrum[i+1];
  return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeFFTSpectrumRaw : Computes the FFTSpectrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     cepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeFFTSpectrumRaw(ASDF *asdf, int frameIndex, F_VECTOR *spectrum) 
{
  static int      mfft,nfft, nfBy2, windowSize,centOrFreq,
                  minDFTCoefficient, maxDFTCoefficient, numDFTCoefficients;
 static ASDF     *prevAsdf=NULL;
 static float    *resAx, *resAy, *amag, *phase, *signal;
 static float    minFrequency, maxFrequency, dftLineFrequency;
 static F_VECTOR *waveform;
 int             i;


 if (prevAsdf != asdf) {
   mfft = GetIAttribute(asdf, "fftOrder");
   nfft = GetIAttribute(asdf,"fftSize");
   nfBy2 = nfft/2;
   windowSize = GetIAttribute(asdf,"windowSize");
   waveform = (F_VECTOR *) AllocFVector(windowSize);
   resAx = (float *) malloc((nfft+1)*sizeof(float));
   signal = (float *) malloc((nfft+1)*sizeof(float));
   resAy = (float *) malloc((nfft+1)*sizeof(float));
   amag = (float *) malloc((nfft+1)*sizeof(float));
   phase = (float *) malloc((nfft+1)*sizeof(float));
   centOrFreq = (int) GetIAttribute(asdf, "centOrFreq");
   numDFTCoefficients = (int) GetIAttribute(asdf, "numDFTCoefficients");
   maxFrequency = GetFAttribute (asdf, "maxFrequency");
   minFrequency = GetFAttribute (asdf, "minFrequency");
   dftLineFrequency = (float) asdf->samplingRate/nfft;
   if (centOrFreq == 1) {
     maxFrequency = ConvertCentToFreq(asdf->maxFrequency, asdf->tonic);
     minFrequency = ConvertCentToFreq(asdf->minFrequency, asdf->tonic);
   }
   minDFTCoefficient = (int) minFrequency/dftLineFrequency+1;
   maxDFTCoefficient = (int) maxFrequency/dftLineFrequency+1;
   prevAsdf = asdf;
}
 waveform = (F_VECTOR *) FrameComputeWaveform(asdf,frameIndex,waveform);
 for (i = 1; i <= windowSize; i++)
   signal[i] = waveform->array[i-1];
 for (i = windowSize+1; i <= nfft; i++)
   signal[i] = 0;
 Rfft(signal, resAx,resAy, mfft, nfft, -1);
 SpectrumReal (nfft, resAx, resAy, amag, phase);
 for ( i=minDFTCoefficient; i< minDFTCoefficient+numDFTCoefficients; i++ ) if (amag[i] != 0)
    spectrum->array[i-minDFTCoefficient] = 20*log(amag[i]);
   else spectrum->array[i-minDFTCoefficient] = -10;
  return(spectrum);
}

/*-------------------------------------------------------------------------
 *  FrameComputeFFTSpectrumMean -- Computes the FFT Spectrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeFFTSpectrumMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numSpectrum, numFrames, fftSize, numVoicedFrames;
  static F_VECTOR  *meanVector;
  int              i,j;

   numSpectrum = GetIAttribute(asdf,"numDFTCoefficients");
   numFrames = GetIAttribute(asdf, "numFrames");
   numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
   fftSize = GetIAttribute(asdf, "fftSize");
   meanVector = (F_VECTOR *) AllocFVector(numSpectrum);
   for (i = 0; i < numSpectrum; i++)
     meanVector->array[i] = 0;
 for (i = 0; i < numFrames; i++)
   if (asdf->vU[i]) {
     fvect = (F_VECTOR *) FrameComputeFFTSpectrumRaw(asdf,i, fvect);
 for ( j = 0; j < numSpectrum; j++ )
   meanVector->array[j] = fvect->array[j] + meanVector->array[j];
   }
 for ( i = 0; i < numSpectrum; i++ )
   fvect->array[i] = meanVector->array[i]/numVoicedFrames;
 return(fvect);
 
}	/*  End of FrameComputeFFTSpectrumMean	*/	



/*-------------------------------------------------------------------------
 *  FrameComputeFFTSpectrumVariance -- Computes the 
 *                                              -- FFTSpectrum Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeFFTSpectrumVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numSpectrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numSpectrum = GetIAttribute(asdf,"numDFTCoefficients");
   numFrames = GetIAttribute(asdf, "numFrames");
   meanVector = (F_VECTOR *) AllocFVector(numSpectrum);
   meanVector = (F_VECTOR *) FrameComputeFFTSpectrumMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numSpectrum);
   diffVector = (F_VECTOR *) AllocFVector(numSpectrum);
   for (i = 0; i < numSpectrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeFFTSpectrumRaw(asdf,i, fvect);
     for ( j = 0; j < numSpectrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numSpectrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeFFTSpectrumVariance	*/	



/*--------------------------------------------------------------------------
  FrameComputeFFTSpectrum : Computes the FFTSpectrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeFFTSpectrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numDFTCoefficients;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL;
  static char      oldName[500];
  static int       zeroMean, featureVarNormalize;
  int              i;
  static float     sqrtValue;
if (prevAsdf != asdf) {
   numDFTCoefficients = GetIAttribute(asdf,"numDFTCoefficients");
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean) || (featureVarNormalize)) { 
     meanVector = (F_VECTOR *) AllocFVector(numDFTCoefficients);
     varVector = (F_VECTOR *) AllocFVector(numDFTCoefficients);
   }
   prevAsdf = asdf;
}
 if (((zeroMean)||(featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
   meanVector = (F_VECTOR *) FrameComputeFFTSpectrumMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) FrameComputeFFTSpectrumVariance(asdf, 0, varVector);
   oldName[0] = '\0';
   strcpy(oldName, asdf->waveFileName);
 }
   fvect = (F_VECTOR *) FrameComputeFFTSpectrumRaw (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numDFTCoefficients; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numDFTCoefficients; i++){
     sqrtValue = sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtValue;
   }
 return(fvect);
}



/*--------------------------------------------------------------------------
  FrameComputeDeltaFFTSpectrum : Computes the deltaFFTSpectrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaSpectrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaFFTSpectrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numSpectrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numSpectrum = GetIAttribute(asdf,"numDFTCoefficients");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numSpectrum);
   next = (F_VECTOR *) AllocFVector(numSpectrum);
   temp = (F_VECTOR *) AllocFVector(numSpectrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numSpectrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])) 
     prev = (F_VECTOR *) FrameComputeFFTSpectrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
    if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeFFTSpectrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaFFTSpectrum : Computes the deltaDeltaFFTSpectrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaFFTSpectrum returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaFFTSpectrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numSpectrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numSpectrum = GetIAttribute(asdf,"numDFTCoefficients");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numSpectrum);
   next = (F_VECTOR *) AllocFVector(numSpectrum);
   temp = (F_VECTOR *) AllocFVector(numSpectrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numSpectrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])) 
     prev = (F_VECTOR *) FrameComputeDeltaFFTSpectrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
    if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeDeltaFFTSpectrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeAugmentedFFTSpectrum : Computes the augmented FFTSpectrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeAugmentedFFTSpectrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numSpectrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numSpectrum = GetIAttribute(asdf,"numDFTCoefficients");
    temp = (F_VECTOR *) AllocFVector(numSpectrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeFFTSpectrum(asdf,frameIndex, temp);
  for (i = 0; i < numSpectrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeDeltaFFTSpectrum(asdf,frameIndex, temp);
  for (i = numSpectrum; i < 2*numSpectrum; i++)
    fvect->array[i] = temp->array[i-numSpectrum];
  temp = (F_VECTOR *) FrameComputeDeltaDeltaFFTSpectrum(asdf,frameIndex, temp);
  for (i = 2*numSpectrum; i < 3*numSpectrum; i++)
    fvect->array[i] = temp->array[i-2*numSpectrum];
  fvect->array[3*numSpectrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numSpectrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numSpectrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}



/*--------------------------------------------------------------------------
  FrameComputeFFTCepstrumRaw : Computes the FFTcepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     cepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeFFTCepstrumRaw(ASDF *asdf, int frameIndex, F_VECTOR *cepstrum) 
{
  static int      mfft, nfft, numCepstrum, windowSize, frameAdvanceSamples;
  static ASDF     *prevAsdf=NULL;
  static float     *resAx, *resAy, *amag, *phase, *resEnergy, *signal;
  static float            maxFrequency, minFrequency;
  static int       centOrFreq, minDFTCoefficient, maxDFTCoefficient, dftLineFrequency, numDFTCoefficients;
 static F_VECTOR  *fftSpectrum;
 int              i;
 static VECTOR_OF_F_VECTORS           *discreteCosineTransform;
 if (prevAsdf != asdf) {
   mfft = GetIAttribute(asdf, "fftOrder");
   nfft = GetIAttribute(asdf,"fftSize");
   centOrFreq = GetIAttribute(asdf, "centOrFreq");
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   windowSize = GetIAttribute(asdf,"windowSize");
   frameAdvanceSamples = GetIAttribute(asdf,"frameAdvanceSamples");
   numDFTCoefficients = GetIAttribute(asdf, "numDFTCoefficients");
   resAx = (float *) malloc((nfft+1)*sizeof(float));
   resEnergy = (float *) malloc((windowSize+1)*sizeof(float));
   signal = (float *) calloc((nfft+1),sizeof(float));
   resAy = (float *) malloc((nfft+1)*sizeof(float));
   amag = (float *) malloc((nfft+1)*sizeof(float));
   phase = (float *) malloc((nfft+1)*sizeof(float));
   fftSpectrum = (F_VECTOR *) AllocFVector(numDFTCoefficients);
   discreteCosineTransform = (VECTOR_OF_F_VECTORS *) GeneratePseudoDct(1, numCepstrum, numDFTCoefficients);
   prevAsdf = asdf;
 }
 fftSpectrum  = (F_VECTOR *) FrameComputeFFTSpectrumRaw(asdf, frameIndex, fftSpectrum);
 LinearTransformationOfFVector (fftSpectrum, discreteCosineTransform, numCepstrum, numDFTCoefficients, cepstrum);
  return(cepstrum);
}
/*-------------------------------------------------------------------------
 *  FrameComputeFFTCepstrumMean -- Computes the FFT Cepstrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeFFTCepstrumMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames, fftSize, numVoicedFrames;
  static F_VECTOR  *meanVector;
  int              i,j;

   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numFrames");
   numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
   fftSize = GetIAttribute(asdf, "fftSize");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     meanVector->array[i] = 0;
 for (i = 0; i < numFrames; i++)
   if (asdf->vU[i]) {
     fvect = (F_VECTOR *) FrameComputeFFTCepstrumRaw(asdf,i, fvect);
 for ( j = 0; j < numCepstrum; j++ )
   meanVector->array[j] = fvect->array[j] + meanVector->array[j];
   }
 for ( i = 0; i < numCepstrum; i++ )
   fvect->array[i] = meanVector->array[i]/numVoicedFrames;
 return(fvect);
 
}	/*  End of FrameComputeFFTCepstrumMean	*/	



/*-------------------------------------------------------------------------
 *  FrameComputeFFTCepstrumVariance -- Computes the 
 *                                              -- FFTCepstrum Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeFFTCepstrumVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeFFTCepstrumMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeFFTCepstrumRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeFFTCepstrumVariance	*/	




/*--------------------------------------------------------------------------
  FrameComputeFFTCepstrum : Computes the linear FFTCepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeFFTCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL;
  static char      oldName[500];
  static int       zeroMean, featureVarNormalize;
  int              i;
  static float     sqrtValue;
if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean) || (featureVarNormalize)) { 
     meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
     varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   }
   prevAsdf = asdf;
}
 if (((zeroMean)||(featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
   meanVector = (F_VECTOR *) FrameComputeFFTCepstrumMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) FrameComputeFFTCepstrumVariance(asdf, 0, varVector);
   oldName[0] = '\0';
   strcpy(oldName, asdf->waveFileName);
 }

 fvect = (F_VECTOR *) FrameComputeFFTCepstrumRaw (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numCepstrum; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numCepstrum; i++){
     sqrtValue = sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtValue;
   }
 return(fvect);
}



/*--------------------------------------------------------------------------
  FrameComputeDeltaFFTCepstrum : Computes the deltaFFTCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaFFTCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])) 
     prev = (F_VECTOR *) FrameComputeFFTCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
    if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeFFTCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaFFTCepstrum : Computes the deltaDeltaFFTCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaFFTCepstrum returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaFFTCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])) 
     prev = (F_VECTOR *) FrameComputeDeltaFFTCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
    if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeDeltaFFTCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeAugmentedFFTCepstrum : Computes the augmented FFTCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeAugmentedFFTCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeFFTCepstrum(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeDeltaFFTCepstrum(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeDeltaDeltaFFTCepstrum(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}


/*--------------------------------------------------------------------------
  FrameComputeLPSpectrum : Computes the LPSpectrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     cepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeLPSpectrum(ASDF *asdf, int frameIndex, F_VECTOR *spectrum) 
{
  static int      mfft, nfft, nfBy2, numCepstrum, windowSize, frameAdvanceSamples;
  static ASDF     *prevAsdf=NULL;
  static float     *resAx, *resAy, *amag, *phase, *coef, *resEnergy, *signal;
 float            gain;
 static F_VECTOR  *waveform;
 static int       LPOrder;
 int              i;
 if (prevAsdf != asdf) {
   mfft = GetIAttribute(asdf, "fftOrder");
   nfft = GetIAttribute(asdf,"fftSize");
   nfBy2 = nfft/2;
   LPOrder = GetIAttribute(asdf, "lpOrder");
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   windowSize = GetIAttribute(asdf,"windowSize");
   frameAdvanceSamples = GetIAttribute(asdf,"frameAdvanceSamples");
   waveform = (F_VECTOR *) AllocFVector(windowSize);
   resAx = (float *) malloc((nfft+1)*sizeof(float));
   resEnergy = (float *) malloc((windowSize+1)*sizeof(float));
   resAy = (float *) malloc((nfft+1)*sizeof(float));
   amag = (float *) malloc((nfft+1)*sizeof(float));
   phase = (float *) malloc((nfft+1)*sizeof(float));
   signal = (float *) calloc(nfft+1, sizeof(float));
   coef = (float *) malloc((LPOrder+1)*sizeof(float));
   prevAsdf = asdf;
}
 waveform = (F_VECTOR *) FrameComputeWaveform(asdf,frameIndex,waveform);
 for (i = 0; i < windowSize; i++)
   signal[i+1] = waveform->array[i];
LpAnal(signal, resEnergy, windowSize, frameAdvanceSamples, coef, LPOrder, &gain);
 LPSpectrum(coef, LPOrder, amag, phase, nfft, mfft, gain);
 for ( i = 1; i <= nfBy2; i++ ) 
   spectrum->numElements = nfft/2;
  for ( i = 1; i<=nfBy2; i++ )
    spectrum->array[i-1] = log(amag[i]);
  return(spectrum);
}

/*--------------------------------------------------------------------------
  FrameComputeLPCepstrumRawFFT : Computes the LPcepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     cepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeLPCepstrumRawFFT(ASDF *asdf, int frameIndex, F_VECTOR *cepstrum) 
{
  static int      mfft, nfft, numCepstrum, windowSize, frameAdvanceSamples;
  static ASDF     *prevAsdf=NULL;
  static float     *resAx, *resAy, *amag, *phase, *coef, *resEnergy, *signal;
 float            gain;
 static F_VECTOR  *waveform;
 static int       LPOrder;
 int              i;
 if (prevAsdf != asdf) {
   mfft = GetIAttribute(asdf, "fftOrder");
   nfft = GetIAttribute(asdf,"fftSize");
   LPOrder = GetIAttribute(asdf, "lpOrder");
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   windowSize = GetIAttribute(asdf,"windowSize");
   frameAdvanceSamples = GetIAttribute(asdf,"frameAdvanceSamples");
   waveform = (F_VECTOR *) AllocFVector(windowSize);
   resAx = (float *) malloc((nfft+1)*sizeof(float));
   resEnergy = (float *) malloc((windowSize+1)*sizeof(float));
   signal = (float *) calloc((nfft+1),sizeof(float));
   resAy = (float *) malloc((nfft+1)*sizeof(float));
   amag = (float *) malloc((nfft+1)*sizeof(float));
   phase = (float *) malloc((nfft+1)*sizeof(float));
   coef = (float *) malloc((LPOrder+1)*sizeof(float));
   prevAsdf = asdf;
}
 waveform = (F_VECTOR *) FrameComputeWaveform(asdf,frameIndex,waveform);
 for (i = 0; i < windowSize; i++)
   signal[i+1] = waveform->array[i];
 LpAnal(signal, resEnergy, windowSize, frameAdvanceSamples, coef, LPOrder, &gain);
 LPSpectrum(coef, LPOrder, amag, phase, nfft, mfft, gain);
 for ( i = 1; i <= nfft; i++ ) 
   if (amag[i] != 0)
     amag[i] = log(amag[i]);
   else amag[i] = log(1.0E-10);
 Rfft(amag,resAx,resAy,mfft,nfft,1);
 cepstrum->numElements = numCepstrum;
  for ( i = 1; i <= numCepstrum; i++ )
  cepstrum->array[i-1] = resAx[i+1];
  return(cepstrum);
}
/*-------------------------------------------------------------------------
 *  FrameComputeLPCepstrumMeanFFT -- Computes the LP Cepstrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeLPCepstrumMeanFFT(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames, fftSize, numVoicedFrames;
  static F_VECTOR  *meanVector;
  int              i,j;

   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numFrames");
   numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
   fftSize = GetIAttribute(asdf, "fftSize");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     meanVector->array[i] = 0;
 for (i = 0; i < numFrames; i++)
   if (asdf->vU[i]) {
     fvect = (F_VECTOR *) FrameComputeLPCepstrumRawFFT(asdf,i, fvect);
 for ( j = 0; j < numCepstrum; j++ )
   meanVector->array[j] = fvect->array[j] + meanVector->array[j];
   }
 for ( i = 0; i < numCepstrum; i++ )
   fvect->array[i] = meanVector->array[i]/numVoicedFrames;
 return(fvect);
 
}	/*  End of FrameComputeLPCepstrumMeanFFT	*/	



/*-------------------------------------------------------------------------
 *  FrameComputeLPCepstrumVarianceFFT -- Computes the 
 *                                              -- LPCepstrumFFT Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeLPCepstrumVarianceFFT(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeLPCepstrumMeanFFT(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeLPCepstrumRawFFT(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeLPCepstrumVarianceFFT	*/	



/*--------------------------------------------------------------------------
  FrameComputeLPCepstrumFFT : Computes the LP cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeLPCepstrumFFT(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL;
  static char      oldName[500];
  static int       zeroMean, featureVarNormalize;
  int              i;
  static float     sqrtValue;

 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean) || (featureVarNormalize)) 
     meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
     varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
}
 if (((zeroMean) || (featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
  meanVector = (F_VECTOR *) FrameComputeLPCepstrumMeanFFT(asdf, 0, meanVector);
  varVector = (F_VECTOR *) FrameComputeLPCepstrumVarianceFFT(asdf, 0, varVector);
  oldName[0] = '\0';
  strcpy (oldName, asdf->waveFileName);
}
 fvect = (F_VECTOR *) FrameComputeLPCepstrumRawFFT (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numCepstrum; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numCepstrum; i++){
     sqrtValue = sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtValue;
   }
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeDeltaLPCepstrumFFT : Computes the deltaLPCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaLPCepstrumFFT(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])) 
     prev = (F_VECTOR *) FrameComputeLPCepstrumFFT(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
    if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeLPCepstrumFFT(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaLPCepstrumFFT : Computes the deltaDeltaLPCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaLPCepstrum returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaLPCepstrumFFT(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])) 
     prev = (F_VECTOR *) FrameComputeDeltaLPCepstrumFFT(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
    if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeDeltaLPCepstrumFFT(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeAugmentedLPCepstrumFFT : Computes the augmented LPCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeAugmentedLPCepstrumFFT(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeLPCepstrumFFT(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeDeltaLPCepstrumFFT(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeDeltaDeltaLPCepstrumFFT(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}
/*-------------------------------------------------------------------------
 *  FrameComputeLPCepstrumMean -- Computes the LP Cepstrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeLPCepstrumMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames, fftSize;
  static F_VECTOR  *meanVector=NULL;
 int              i,j;
 int              numVoicedFrames = 0;
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numFrames");
   numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
   fftSize = GetIAttribute(asdf, "fftSize");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     meanVector->array[i] = 0;
 for (i = 0; i < numFrames; i++) 
   if (asdf->vU[i]) {
     fvect = (F_VECTOR *) FrameComputeLPCepstrumRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ )
       meanVector->array[j] = fvect->array[j] + meanVector->array[j];
   }
     for ( i = 0; i < numCepstrum; i++ )
     fvect->array[i] = meanVector->array[i]/numVoicedFrames;
  return(fvect);

}	/*  End of FrameComputeLPCepstrumMean	*/	


/*-------------------------------------------------------------------------
 *  FrameComputeLPCepstrumVariance -- Computes the 
 *                                              -- LPCepstrum Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeLPCepstrumVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeLPCepstrumMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeLPCepstrumRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeLPCepstrumVariance	*/	




/*--------------------------------------------------------------------------
  FrameComputeLPCepstrum : Computes the LP cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeLPCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector, *varVector;
  static char      oldName[500];
  static int       zeroMean, featureVarNormalize;
  int              i;
  static float     sqrtValue;

 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean) ||(featureVarNormalize))
     meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
     varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
 }
 if (((zeroMean)||(featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
   meanVector = (F_VECTOR *) FrameComputeLPCepstrumMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) FrameComputeLPCepstrumVariance(asdf, 0, varVector);
   oldName[0] = '\0';
   strcpy (oldName, asdf->waveFileName);
 }
 fvect = (F_VECTOR *) FrameComputeLPCepstrumRaw (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numCepstrum; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numCepstrum; i++){
     sqrtValue = sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtValue;
   }
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeDeltaLPCepstrum : Computes the deltaLPCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaLPCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])) 
     prev = (F_VECTOR *) FrameComputeLPCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
    if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeLPCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaLPCepstrum : Computes the deltaDeltaLPCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaLPCepstrum returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaLPCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])) 
      prev = (F_VECTOR *) FrameComputeDeltaLPCepstrum(asdf,frameIndex-i,prev);
   else 
     InitFVector(prev);
    if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeDeltaLPCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeAugmentedLPCepstrum : Computes the augmented LPCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeAugmentedLPCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeLPCepstrum(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeDeltaLPCepstrum(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeDeltaDeltaLPCepstrum(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}

/*----------------------------------------------------------------------

 FrameComputeFilterbankEnergy : Computes the filterbank energies 
in a frame of speech.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of "numFilters" filterbank energy coefficients

 *---------------------------------------------------------------------*/

 F_VECTOR *FrameComputeFilterbankEnergy(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
   static int         mfft, nfft,  nfBy2, windowSize;
  static ASDF        *prevAsdf=NULL;
  static float       *resAx, *resAy, *amag, *phase, *modGd;
  static F_VECTOR    *waveform, *meanGDVector;
  static float       *signal;
  static int         perceptual, useTrain, useLog, useMinGd;
  static char        oldName[500];
  int                i;

  if (prevAsdf != asdf) {
    mfft = GetIAttribute(asdf, "fftOrder");
    nfft = GetIAttribute(asdf,"fftSize");
    nfBy2 = nfft/2;
    //    Cstore(nfft);
    windowSize = GetIAttribute(asdf,"windowSize");
    perceptual = GetIAttribute(asdf,"perceptualFilterbank");
    useTrain = GetIAttribute(asdf, "useTrain");
    useLog = GetIAttribute(asdf, "useLog");
    useMinGd = GetIAttribute(asdf, "useMinGd");
    waveform = (F_VECTOR *) AllocFVector((windowSize));
    signal = (float *) calloc(nfft+1, sizeof(float));
    resAx = (float *) calloc(nfft+1, sizeof(float));
    resAy = (float *) calloc(nfft+1, sizeof(float));
    amag = (float *) calloc(nfft+1, sizeof(float));
    phase = (float *) calloc(nfft+1, sizeof(float));
    modGd = (float *) calloc(nfft+1, sizeof(float));
    if (perceptual) {
      meanGDVector = (F_VECTOR *) AllocFVector(nfBy2);
      oldName[0] = '\0';
    }
    prevAsdf = asdf;
  }
 if (perceptual && !useTrain && (strcmp(oldName, asdf->waveFileName) != 0)) {
     if (!useMinGd)
       meanGDVector = (F_VECTOR *)FrameComputeModGDelayMean(asdf, 0, meanGDVector);
     else
       meanGDVector = (F_VECTOR *)FrameComputeMinGDelayMean(asdf, 0, meanGDVector);
     oldName[0] = '\0';
     strcpy(oldName, asdf->waveFileName);
 } else if (perceptual && (useTrain)) 
   meanGDVector = asdf->modgd;
 waveform = (F_VECTOR *) FrameComputeWaveform(asdf,frameIndex,waveform);
 for (i = 0; i < windowSize; i++)
   signal[i+1] = waveform->array[i];
 for (i = windowSize+1; i <= nfft; i++)
   signal[i] = 0;
 // Window(signal,windowSize,'M','D',1.0);
 Rfft(signal, resAx, resAy, mfft, nfft, -1);
 SpectrumReal(nfft, resAx, resAy, amag, phase);
 if (perceptual) {
   fvect = (F_VECTOR *) PerceptualFilterbankEnergyIntegration(asdf, amag, meanGDVector->array, useLog, fvect);
 }  else 
   fvect = (F_VECTOR *) FilterbankEnergyIntegration(asdf, amag, fvect);

 return(fvect);
}

/*----------------------------------------------------------------------------
 FrameComputeFilterbankLogEnergyRaw : Computes the filterbank log energies in a frame of speech.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of "numFilters" filterbank log energy coefficients

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeFilterbankLogEnergyRaw(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static int                           numFilters;
  static ASDF                          *prevAsdf=NULL;
 int                                   i;

  if (prevAsdf != asdf) {
    numFilters = GetIAttribute(asdf,"numFilters");
    prevAsdf = asdf;
}
 fvect = (F_VECTOR *) FrameComputeFilterbankEnergy(asdf,frameIndex, fvect);
 for (i = 0; i < numFilters; i++)
   if (fvect->array[i] != 0)
     fvect->array[i] = log(fvect->array[i]);
   else 
     fvect->array[i] = log(1.0E-10);
 return(fvect);
}
/*-------------------------------------------------------------------------
 *  FrameComputeFilterbankLogEnergyMean -- Computes the mean Filterbank Energy for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR *FrameComputeFilterbankLogEnergyMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int       numFrames, numFilters;
  static F_VECTOR  *meanVector=NULL;
  int              i,j;
  int              numVoicedFrames = 0;

  numFrames = GetIAttribute(asdf, "numFrames");
  numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
  numFilters = GetIAttribute(asdf, "numFilters");
  meanVector = (F_VECTOR *) AllocFVector(numFilters);
  for (i = 0; i < numFilters; i++)
     meanVector->array[i] = 0;
  
  for (i = 0; i < numFrames; i++) 
    if (asdf->vU[i]) {
      fvect = (F_VECTOR *) FrameComputeFilterbankLogEnergyRaw(asdf,i, fvect);
      for ( j = 0; j < numFilters; j++ )
	meanVector->array[j] = fvect->array[j] + meanVector->array[j];
    }
  for ( i = 0; i < numFilters; i++ )
    fvect->array[i] = meanVector->array[i]/numVoicedFrames;
  return(fvect);

}	/*  End of FrameComputeFilterbankLogEnergyMean	*/	



/*-------------------------------------------------------------------------
 *  FrameComputFilterbankLogEnergyVariance -- Computes the 
 *                                              -- FilterbankLogEnergy Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeFilterbankLogEnergyVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numFilters, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numFilters = GetIAttribute(asdf,"numFilters");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numFilters);
   meanVector = (F_VECTOR *) FrameComputeFilterbankLogEnergyMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numFilters);
   diffVector = (F_VECTOR *) AllocFVector(numFilters);
   for (i = 0; i < numFilters; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeFilterbankLogEnergyRaw(asdf,i, fvect);
     for ( j = 0; j < numFilters; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numFilters; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeFilterbankLogEnergyVariance	*/	




/*--------------------------------------------------------------------------
  FrameComputeFilterbankLogEnergy : Computes the Energy of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeFilterbankLogEnergy(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numFilters;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL;
  static char      oldName[500];
  static int       zeroMean, featureVarNormalize;
  int              i;
  static float     sqrtValue;
if (prevAsdf != asdf) {
   numFilters = GetIAttribute(asdf,"numFilters");
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean) || (featureVarNormalize)) { 
     meanVector = (F_VECTOR *) AllocFVector(numFilters);
     varVector = (F_VECTOR *) AllocFVector(numFilters);
   }
   prevAsdf = asdf;
}
 if (((zeroMean)||(featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
   meanVector = (F_VECTOR *) FrameComputeFilterbankLogEnergyMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) FrameComputeFilterbankLogEnergyVariance(asdf, 0, varVector);
   oldName[0] = '\0';
   strcpy(oldName, asdf->waveFileName);
 }

 fvect = (F_VECTOR *) FrameComputeFilterbankLogEnergyRaw (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numFilters; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numFilters; i++){
     sqrtValue = sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtValue;
   }
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeDeltaFilterbankLogEnergy : Computes the deltaFilterbankEnergy of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/
F_VECTOR *FrameComputeDeltaFilterbankLogEnergy(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numFilters,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numFilters = GetIAttribute(asdf,"numFilters");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numFilters);
   next = (F_VECTOR *) AllocFVector(numFilters);
   temp = (F_VECTOR *) AllocFVector(numFilters);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
}
 for (i = 0; i < numFilters; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeFilterbankLogEnergy(asdf,frameIndex-i,prev);
   else InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeFilterbankLogEnergy(asdf,frameIndex+i,next);
   else InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
LinearVectorScalarDivide(normalizingConst,fvect,fvect);
return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaLogEnergy : Computes the deltaDeltaLogEnergy of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaLogEnergy returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaFilterbankLogEnergy(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numFilters,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numFilters = GetIAttribute(asdf,"numFilters");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numFilters-1);
   next = (F_VECTOR *) AllocFVector(numFilters-1);
   temp = (F_VECTOR *) AllocFVector(numFilters-1);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numFilters; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeDeltaFilterbankLogEnergy(asdf,frameIndex-i,prev);
   else InitFVector(prev);
  if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])) 
     next = (F_VECTOR *) FrameComputeDeltaFilterbankLogEnergy(asdf,frameIndex+i,next);
   else InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeAugmentedFilterbankLogEnergy : Computes the augmentedFilterbankLogEnergy of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeAugmentedFilterbankLogEnergy(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numFilters;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numFilters = GetIAttribute(asdf,"numFilters");
    temp = (F_VECTOR *) AllocFVector(numFilters-1);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeFilterbankLogEnergy(asdf,frameIndex, temp);
  for (i = 0; i < numFilters; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeDeltaFilterbankLogEnergy(asdf,frameIndex, temp);
  for (i = numFilters; i < 2*(numFilters-1); i++)
    fvect->array[i] = temp->array[i-(numFilters-1)];
  temp = (F_VECTOR *) FrameComputeDeltaDeltaFilterbankLogEnergy(asdf,frameIndex, temp);
  for (i = 2*(numFilters); i < 3*(numFilters); i++)
    fvect->array[i] = temp->array[i-2*(numFilters-1)];
  fvect->array[3*(numFilters)] = ((F_VECTOR *) FrameComputeLogEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*(numFilters)+1] = ((F_VECTOR *) FrameComputeDeltaLogEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*(numFilters)+2] = ((F_VECTOR *) FrameComputeDeltaDeltaLogEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}


/*----------------------------------------------------------------------------
 FrameComputeMelCepstrumRaw : Computes the filterbank log cepstrum in a frame of speech.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of "numFilters" filterbank log cepstrum coefficients

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelCepstrumRaw(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static int                           numFilters,numCepstrum;
  static ASDF                          *prevAsdf=NULL;
  static F_VECTOR                      *tempFVector;
  static VECTOR_OF_F_VECTORS           *melCepstrumCosineTransform;

  if (prevAsdf != asdf) {
    numFilters = (int) GetIAttribute(asdf,"numFilters");
    numCepstrum = (int) GetIAttribute(asdf,"numCepstrum");
    melCepstrumCosineTransform = (VECTOR_OF_F_VECTORS *) 
      GetPtrAttribute(asdf,"melCepstrumCosineTransform");
    tempFVector = (F_VECTOR *) AllocFVector(numFilters);
    prevAsdf = asdf;
}
 tempFVector = (F_VECTOR *) FrameComputeFilterbankLogEnergyRaw(asdf, frameIndex, tempFVector);
 LinearTransformationOfFVector(tempFVector,melCepstrumCosineTransform,  numCepstrum, numFilters, fvect); 
 return(fvect);
}


/*-------------------------------------------------------------------------
 *  FrameComputeMelCepstrumMean -- Computes the Cepstrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeMelCepstrumMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int       numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL;
  int              i,j;
  static int              numVoicedFrames = 0;

  numCepstrum = GetIAttribute(asdf,"numCepstrum");
  numFrames = GetIAttribute(asdf, "numFrames");
  numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
  meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
  for (i = 0; i < numFrames; i++) 
    if (asdf->vU[i]){
      fvect = (F_VECTOR *) FrameComputeMelCepstrumRaw(asdf,i, fvect);
      for ( j = 0; j < numCepstrum; j++ )
	meanVector->array[j] = fvect->array[j] + meanVector->array[j];
    }
  for ( i = 0; i < numCepstrum; i++ )
    fvect->array[i] = meanVector->array[i]/numVoicedFrames;
  return(fvect);
  
}	/*  End of FrameComputeMelCepstrumMean	*/	

/*-------------------------------------------------------------------------
 *  FrameComputeMelCepstrumVariance -- Computes the Cepstrum Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeMelCepstrumVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeMelCepstrumMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeMelCepstrumRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeMelCepstrumVariance	*/	

/*--------------------------------------------------------------------------
  FrameComputeMelCepstrum : Computes the cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL;
  //  static short     *waveform=NULL;
  static int       zeroMean, featureVarNormalize;
  static char      oldName[500];
  static float     sqrtVal;
  int              i;
  
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    zeroMean = (int) GetIAttribute(asdf, "zeroMean");
    featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
    if ((zeroMean) || (featureVarNormalize)) {
      meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
      varVector = (F_VECTOR *) AllocFVector(numCepstrum);
      oldName[0] = '\0';
    }
    prevAsdf = asdf;
  }
  if (((zeroMean)||(featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName)!= 0))
    
    { meanVector = (F_VECTOR *) FrameComputeMelCepstrumMean(asdf, 0, meanVector);
      varVector = (F_VECTOR *) FrameComputeMelCepstrumVariance(asdf, 0, varVector);
      oldName[0] = '\0';
      strcpy (oldName, asdf->waveFileName);
    }
  
  fvect = (F_VECTOR *) FrameComputeMelCepstrumRaw (asdf, frameIndex, fvect);
  if (zeroMean) {
   for (i = 0; i < numCepstrum; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
  }
  if (featureVarNormalize) 
    for (i = 0; i < numCepstrum; i++) {
      sqrtVal = (float) sqrtf(varVector->array[i]);
      fvect->array[i] = fvect->array[i]/sqrtVal;
    }
  return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeMelDeltaCepstrum : Computes the deltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelDeltaCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
}
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
       prev = (F_VECTOR *) FrameComputeMelCepstrum(asdf,frameIndex-i,prev);
   else InitFVector(prev);
   if (((frameIndex+i) < numFrames) && (asdf->vU[frameIndex+i]))
       next = (F_VECTOR *) FrameComputeMelCepstrum(asdf,frameIndex+i,next);
   else InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
LinearVectorScalarDivide(normalizingConst,fvect,fvect);
return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeMelDeltaDeltaCepstrum : Computes the deltaDeltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaCepstrum returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelDeltaDeltaCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeMelDeltaCepstrum(asdf,frameIndex-i,prev);
   else InitFVector(prev);
   if (((frameIndex+i) < numFrames) && (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeMelDeltaCepstrum(asdf,frameIndex+i,next);
   else InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeMelAugmentedCepstrum : Computes the augmentedCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelAugmentedCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeMelCepstrum(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeMelDeltaCepstrum(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeMelDeltaDeltaCepstrum(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeLogEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaLogEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaLogEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}

/*----------------------------------------------------------------------------
 FrameComputeMelRootCepstrumRaw : Computes the root cepstrum in a frame of speech.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of "numFilters" filterbank log cepstrum coefficients

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelRootCepstrumRaw(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static int                           numFilters,numCepstrum;
  static float                         gamma;
  static ASDF                          *prevAsdf=NULL;
  static F_VECTOR                      *tempFVector;
  static VECTOR_OF_F_VECTORS           *melCepstrumCosineTransform;
  static int                           i;
  if (prevAsdf != asdf) {
    numFilters = (int) GetIAttribute(asdf,"numFilters");
    numCepstrum = (int) GetIAttribute(asdf,"numCepstrum");
    gamma = (float) GetFAttribute(asdf,"gamma");
    melCepstrumCosineTransform = (VECTOR_OF_F_VECTORS *) 
      GetPtrAttribute(asdf,"melCepstrumCosineTransform");
    tempFVector = (F_VECTOR *) AllocFVector(numFilters);
    prevAsdf = asdf;
}
 tempFVector = (F_VECTOR *) FrameComputeFilterbankEnergy(asdf,frameIndex, tempFVector);
 for (i = 0; i < numCepstrum; i++)
   tempFVector->array[i] = exp(gamma*log(tempFVector->array[i]));
 LinearTransformationOfFVector(tempFVector,melCepstrumCosineTransform,  numCepstrum, numFilters, fvect); 
 return(fvect);
}
/*-------------------------------------------------------------------------
 *  FrameComputeMelRootCepstrumMean -- Computes the Root Cepstrum mean 
 *                                     for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeMelRootCepstrumMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL;
  int              i,j;
  int              numVoicedFrames = 0;  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numFrames");
   numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     meanVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) 
     if (asdf->vU[i]) {
       fvect = (F_VECTOR *) FrameComputeMelRootCepstrumRaw(asdf,i, fvect);
       for ( j = 0; j < numCepstrum; j++ )
	 meanVector->array[j] = fvect->array[j] + meanVector->array[j];
     }
   for ( i = 0; i < numCepstrum; i++ )
     fvect->array[i] = meanVector->array[i]/numVoicedFrames;
   return(fvect);

}	/*  End of FrameComputeMelRootCepstrumMean	*/	



/*-------------------------------------------------------------------------
 *  FrameComputeMelRootCepstrumVariance -- Computes the Root Cepstrum Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeMelRootCepstrumVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeMelRootCepstrumMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeMelRootCepstrumRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeMelRootCepstrumVariance	*/	

/*--------------------------------------------------------------------------
  FrameComputeMelRootCepstrum : Computes the cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelRootCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector=NULL, *varVector;
  static short     *waveform=NULL;
  static int       zeroMean, featureVarNormalize;
  static float     sqrtValue;
  int              i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    zeroMean = (int) GetIAttribute(asdf, "zeroMean");
    featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
    if ((zeroMean) || (featureVarNormalize)) {
      meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
      varVector = (F_VECTOR *) AllocFVector(numCepstrum);
    }
    prevAsdf = asdf;
  }
  if (((zeroMean)||(featureVarNormalize)) && (waveform != (short *)GetPtrAttribute(asdf, "waveform")))
    
    { 
      meanVector = (F_VECTOR *) FrameComputeMelRootCepstrumMean(asdf, 0, meanVector);
      varVector = (F_VECTOR *) FrameComputeMelRootCepstrumVariance(asdf, 0, varVector);
      waveform = (short *) GetPtrAttribute(asdf, "waveform");
    }
  
  fvect = (F_VECTOR *) FrameComputeMelRootCepstrumRaw (asdf, frameIndex, fvect);
  if (zeroMean) {
    for (i = 0; i < numCepstrum; i++)
      fvect->array[i] = fvect->array[i] - meanVector->array[i];
  }
  if (featureVarNormalize) 
    for (i = 0; i < numCepstrum; i++) {
     sqrtValue = (float) sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtValue;
    }
  return(fvect);
 }
/*--------------------------------------------------------------------------
  FrameComputeMelRootDeltaCepstrum : Computes the deltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelRootDeltaCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
}
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeMelCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeMelCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeMelRootDeltaDeltaCepstrum : Computes the deltaDeltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaCepstrum returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelRootDeltaDeltaCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeMelRootDeltaCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeMelRootDeltaCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeMelRootAugmentedCepstrum : Computes the augmentedCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelRootAugmentedCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeMelRootCepstrum(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeMelRootDeltaCepstrum(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeMelRootDeltaDeltaCepstrum(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}
/*--------------------------------------------------------------------------
 FrameComputeGDelay : Computes the standard group delay 
                          for a frame of speech

 Inputs : waveform, frameIndex, frameShift, frameSize, total samples in
          waveform.

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeGDelay(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize,  
                                 fftSize, fftOrder;
  static int                     i, frameShift;
  static F_VECTOR                *waveform;
  static float                   *gDelay=NULL; 
  static float                   *signal=NULL;

  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    frameShift = GetIAttribute(asdf, "frameAdvanceSamples");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    signal = (float *) AllocFloatArray(signal, windowSize);
    gDelay = (float *) AllocFloatArray(gDelay, fftSize+1);
    waveform = (F_VECTOR *) AllocFVector (windowSize);
  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    signal[i] = waveform->array[i-1]/*HamDw(i,windowSize)*/; 
  StdGroupDelay (signal, windowSize, fftSize, fftOrder, gDelay);
  for (i = 0; i < fftSize/2; i++)
   fvect->array[i] = gDelay[i+1]; 
 return(fvect);
}

/*--------------------------------------------------------------------------
 FrameComputeResidualGDelay : Computes the group delay of the residual
                          from a frame of speech

 Inputs : waveform, frameIndex, frameShift, frameSize, total samples in
          waveform.

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeResidualGDelay(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, resGdWindowSize, resGdWindowSizeBy2, numSamples, 
                                 fftSize, fftSizeBy2, fftOrder, lpOrder;
  static int                     frameNum, numFrames, frameShift, medianOrder, averageOrder;
  static                         char oldName[500];
  static F_VECTOR                *waveform;
  static float                   *resEnergy=NULL, *gDelay=NULL, 
                                 *gDelaySmoothed=NULL, 
                                 *residual=NULL, *residualSmth=NULL;
  int                            i, iloc, numEnergySamples, startEnergyIndex;
  float                          average, *gdAverage=NULL, 
                                 *signal=NULL , *coef=NULL, gain;
  static float                   *gdAverageSmth=NULL;

if((prevAsdf == NULL) || (strcmp(oldName,asdf->waveFileName) != 0)){
   windowSize = GetIAttribute(asdf, "windowSize");
   resGdWindowSize = GetIAttribute(asdf, "resGdWindowSize");
   numSamples = GetIAttribute(asdf, "numSamples");
   frameShift = GetIAttribute(asdf, "frameAdvanceSamples");
   numFrames = GetIAttribute(asdf, "numFrames");
   medianOrder = GetIAttribute(asdf, "medianOrder");
   averageOrder = GetIAttribute(asdf, "gdSmthWinSize");
   fftSize = GetIAttribute(asdf, "fftSize");
   fftOrder = GetIAttribute(asdf, "fftOrder");
   lpOrder = GetIAttribute(asdf, "lpOrder");
   fftSizeBy2 = fftSize/2;
   resGdWindowSizeBy2 = resGdWindowSize/2;
   waveform = (F_VECTOR *) AllocFVector (windowSize);
   //   if (signal == NULL) {
   signal = (float *) AllocFloatArray(signal, windowSize+1);
   resEnergy = (float *) AllocFloatArray(resEnergy, windowSize+1);
   gDelay = (float *) AllocFloatArray(gDelay, fftSize+1);
   gDelaySmoothed = (float *) AllocFloatArray(gDelaySmoothed, fftSize+1);
   coef = (float *) AllocFloatArray(coef, lpOrder+1);
     //}
   if ((gdAverage == NULL) && (residual == NULL)) {
     gdAverage = (float *) AllocFloatArray(gdAverage, numSamples+resGdWindowSize+1);
     gdAverageSmth = (float *) AllocFloatArray(gdAverageSmth, numSamples+resGdWindowSize+1);
     residual = (float *) AllocFloatArray(residual, numSamples+resGdWindowSizeBy2+1);
     residualSmth = (float *) AllocFloatArray(residualSmth, numSamples+resGdWindowSize+1);
   } else {
     gdAverage = (float *) realloc(gdAverage, (numSamples+resGdWindowSize+1)*sizeof(float));
     gdAverageSmth = (float *) realloc(gdAverageSmth, (numSamples+resGdWindowSize+1)*sizeof(float));
     residual = (float *) realloc(residual, (numSamples+resGdWindowSizeBy2+1)*sizeof(float));
     residualSmth = (float *) realloc(residualSmth, (numSamples+resGdWindowSize+1)*sizeof(float));
     if ((gdAverage == NULL) || (residual == NULL)) {
       printf("unable to allocate space for residual/gdAverage\n");
       fflush(stdout);
       exit(-1);
     }
   }
   for (i = 1; i <= resGdWindowSizeBy2; i++)
     gdAverage[i] = 0.0;
   iloc = 0;
   for (frameNum = 0; frameNum < numFrames; frameNum++)
     {	
       waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameNum, waveform);
       for (i = 1; i <= windowSize; i++)
	 signal[i] = waveform->array[i-1];
       /* *HamDw(i,windowSize); */
       LpAnal(signal, resEnergy, windowSize, frameShift, coef, lpOrder, &gain);
       for (i = 1; i <= frameShift; i++) 
	 if (iloc+i <= numSamples)
	   residual[iloc+i] = resEnergy[i];
       iloc = iloc+frameShift;
     }
   iloc = 0;
   residual[numSamples] = 0;
   MedianSmoothArray(residual, numSamples-1, 3, residualSmth);
   while (iloc < numSamples - resGdWindowSizeBy2) {
     for (i = 1; i <= resGdWindowSize; i++) {
       if ((((iloc+i) >= numSamples) || ((iloc-i) <= 3)))
	 numEnergySamples = 4;
       else
	 numEnergySamples = 8;
       if ((iloc + i) <= 3) startEnergyIndex = 0;
       else
	 startEnergyIndex = iloc+i-4;
       if ((iloc+i) < numSamples) 
	 resEnergy[i] = residualSmth[iloc+i]*HanDw(i, resGdWindowSize)
       	 *sqrt(ComputeEnergy(&residualSmth[startEnergyIndex], numEnergySamples, 1.0)/numEnergySamples);
       else
	 resEnergy[i] = 0.0000001;
     }
     StdGroupDelay (resEnergy, resGdWindowSize, fftSize, fftOrder, gDelay);
     MedianSmoothArray (gDelay, fftSize/2, 3, gDelaySmoothed);
     ComputeAverage(gDelaySmoothed, fftSize/2, &average);
     gdAverage[iloc+resGdWindowSizeBy2] = -average;
     iloc++;
   }
   gdAverage[numSamples] = 0;
   MedianSmoothArray(&(gdAverage[resGdWindowSizeBy2-1]), numSamples, medianOrder, &(gdAverageSmth[resGdWindowSizeBy2-1]));
   RemoveAverage(&(gdAverageSmth[resGdWindowSizeBy2-1]), numSamples, &average);
   oldName[0] = '\0';
   strcpy(oldName,asdf->waveFileName);
   prevAsdf = asdf;
   free(gdAverage);
   free(gDelaySmoothed);
   free(gDelay); 
   free(residual);
   free(resEnergy);
   free(signal);
   free(residualSmth);
   free(coef);
   oldName[0] = '\0';
   strcpy(oldName,asdf->waveFileName);
   prevAsdf = asdf;
 }
   for (i = 0; i < windowSize; i++)
   if (frameIndex*frameShift+i+1 <= numSamples+resGdWindowSizeBy2)
     fvect->array[i] = gdAverageSmth[frameIndex*frameShift+i]; 
   else 
     fvect->array[i] = 0.0;
 return(fvect);
}

/*--------------------------------------------------------------------------
 FrameComputeHilbertEnvelopeResidualGDelay : Computes the group delay of the residual
                          from a frame of speech

 Inputs : waveform, frameIndex, frameShift, frameSize, total samples in
          waveform.

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeHilbertEnvelopeResidualGDelay(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, resGdWindowSize, resGdWindowSizeBy2, numSamples, 
                                 fftSize, fftSizeBy2, fftOrder, lpOrder;
  static int                     frameNum, numFrames, frameShift, medianOrder, averageOrder;
  static                         char oldName[500];
  static F_VECTOR                *waveform;
  float                          *resEnergy=NULL, *resEnergySmth=NULL, *gDelay=NULL, 
                                 *gDelaySmoothed=NULL, 
                                 *residual=NULL, *residualSmth=NULL;
  complex                        *inRes=NULL, *hilbertEnvelope=NULL;
  int                            i, iloc;
  float                          average, *gdAverage=NULL;
  static float                          *gdAverageSmth=NULL; 
  float                          *signal=NULL , *coef=NULL, gain;

if((prevAsdf == NULL) || (strcmp(oldName,asdf->waveFileName) != 0)){
  windowSize = GetIAttribute(asdf, "windowSize");
  resGdWindowSize = GetIAttribute(asdf, "resGdWindowSize");
  numSamples = GetIAttribute(asdf, "numSamples");
  frameShift = GetIAttribute(asdf, "frameAdvanceSamples");
  numFrames = GetIAttribute(asdf, "numFrames");
  medianOrder = GetIAttribute(asdf, "medianOrder");
  averageOrder = GetIAttribute(asdf, "gdSmthWinSize");
  fftSize = GetIAttribute(asdf, "fftSize");
  fftOrder = GetIAttribute(asdf, "fftOrder");
  lpOrder = GetIAttribute(asdf, "lpOrder");
  fftSizeBy2 = fftSize/2;
  resGdWindowSizeBy2 = resGdWindowSize/2;
  waveform = (F_VECTOR *) AllocFVector (windowSize);
  signal = (float *) AllocFloatArray(signal, windowSize+1);
  resEnergy = (float *) AllocFloatArray(resEnergy, resGdWindowSize+1);
  resEnergySmth = (float *) AllocFloatArray(resEnergySmth, resGdWindowSize+1);
  gDelay = (float *) AllocFloatArray(gDelay, fftSize+1);
  hilbertEnvelope = (complex *) calloc((fftSize+1), sizeof(complex));
  inRes = (complex *) calloc((fftSize+1), sizeof(complex));
  gDelaySmoothed = (float *) AllocFloatArray(gDelaySmoothed, fftSize+1);
  coef = (float *) AllocFloatArray(coef, lpOrder+1);
  if ((gdAverage == NULL) || (residual == NULL)) {
  gdAverage = (float *) AllocFloatArray(gdAverage, numSamples+resGdWindowSizeBy2+1);
  gdAverageSmth = (float *) AllocFloatArray(gdAverageSmth, numSamples+resGdWindowSizeBy2+1);
  residual = (float *) AllocFloatArray(residual, numSamples+resGdWindowSizeBy2+1);
  residualSmth = (float *) AllocFloatArray(residualSmth, numSamples+resGdWindowSizeBy2+1);
   } else {
     gdAverage = (float *) realloc(gdAverage, (numSamples+resGdWindowSizeBy2+1)*sizeof(float));
     gdAverageSmth = (float *) realloc(gdAverageSmth, (numSamples+resGdWindowSizeBy2+1)*sizeof(float));
     residual = (float *) realloc(residual, (numSamples+resGdWindowSizeBy2+1)*sizeof(float));
     residualSmth = (float *) realloc(residualSmth, (numSamples+resGdWindowSizeBy2+1)*sizeof(float));
     if ((gdAverage == NULL) || (residual == NULL)) {
       printf("unable to allocate space for residual/gdAverage\n");
       fflush(stdout);
       exit(-1);
     }
  }
 iloc = 0;
 for (frameNum = 0; frameNum < numFrames; frameNum++)
   {	
     waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameNum, waveform);;
     for (i = 1; i <= windowSize; i++) 
       signal[i] = waveform->array[i-1];
       /* *HamDw(i,windowSize); */
     LpAnal(signal, resEnergy, windowSize, frameShift, coef, lpOrder, &gain);
     for (i = 1; i <= frameShift; i++) {
       if ((iloc+i-1) < numSamples)
	 residual[iloc+i] = resEnergy[i];
       }
     iloc = iloc+frameShift;
   }
 iloc = 0;
 MedianSmoothArray(residual, numSamples, medianOrder, residualSmth);
 while (iloc < numSamples) {	
   for (i = 1; i <= resGdWindowSize; i++)
       if ((iloc+i-1) < numSamples) {
	 inRes[i].re = residualSmth[iloc+i]*
	   HanDw(i, resGdWindowSize)*
	   sqrt(ComputeEnergy(&residualSmth[iloc+i], 8,1.0))/8;
	 inRes[i].im = 0.0;
       }
       else {
	 inRes[i].re = 0.0000001;
	 inRes[i].im = 0.0;
       }
     for (i = resGdWindowSize+1; i <= fftSize; i++) {
       inRes[i].re = 0.0;
       inRes[i].im = 0.0;
     }
     Cfft(inRes, hilbertEnvelope, fftOrder, fftSize, -1);
     for (i = 1; i <= fftSizeBy2; i++) {
       hilbertEnvelope[i].re = 2*hilbertEnvelope[i].re;
       hilbertEnvelope[i].im  = 2*hilbertEnvelope[i].im;
     }
     for (i = fftSizeBy2+1; i <=fftSize; i++) {
       hilbertEnvelope[i].re = 0.0;
       hilbertEnvelope[i].im  = 0.0;
     }
     Cfft (hilbertEnvelope, inRes, fftOrder, fftSize, 1);
     for (i = 1; i <= resGdWindowSize; i++)
       resEnergy[i] = inRes[i].re*inRes[1].re + inRes[i].im*inRes[i].im;
     MedianSmoothArray(resEnergy, resGdWindowSize, medianOrder, resEnergySmth); 
     StdGroupDelay (resEnergySmth, resGdWindowSize, fftSize, fftOrder, gDelay);
     MedianSmoothArray (gDelay, fftSize/2, 3, gDelaySmoothed);
     ComputeAverage(gDelaySmoothed, fftSize/2, &average);
     gdAverage[iloc+resGdWindowSizeBy2] = -average;
     iloc++;
     printf("gdAverage[%d] = %f\n", iloc, gdAverage[iloc]);
 }
   MedianSmoothArray(gdAverage, numSamples, medianOrder, gdAverageSmth);
   RemoveAverage(gdAverageSmth, numSamples, &average);
   free(gdAverage);
   free(gDelaySmoothed);
   free(gDelay); 
   free(inRes);
   free(hilbertEnvelope);
   free(residual);
   free(resEnergy);
   free(signal);
   free(residualSmth);
   free(coef);
   oldName[0] = '\0';
   strcpy(oldName,asdf->waveFileName);
   prevAsdf = asdf;
 }
 for (i = 0; i < windowSize; i++)
   if (frameIndex*frameShift+i+1 <= numSamples+resGdWindowSizeBy2)
     fvect->array[i] = gdAverageSmth[frameIndex*frameShift+i+1]; 
   else 
     fvect->array[i] = 0.0;
 return(fvect);
}


/*----------------------------------------------------------------------------
 FrameComputeMelResGdCepstrumRaw : Computes the filterbank log cepstrum in a frame of speech.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of "numFilters" filterbank log cepstrum coefficients

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelResGdCepstrumRaw(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static int                           numFrames, numFilters,
                                       fftSize, fftOrder, windowSize, numCepstrum;
  static ASDF                          *prevAsdf=NULL;
  static F_VECTOR                      *tempFVector, *gDelayFvect;
  static VECTOR_OF_F_VECTORS           *melCepstrumCosineTransform;
  static float                         *signal, *resAx, *resAy, *amag, *phase;
  static int                           i; 
  if (prevAsdf != asdf) {
    numFilters = (int) GetIAttribute(asdf,"numFilters");
    fftSize = (int) GetIAttribute(asdf,"fftSize");
    fftOrder = (int) GetIAttribute(asdf,"fftOrder");
    windowSize = (int) GetIAttribute(asdf,"windowSize");
    numCepstrum = (int) GetIAttribute(asdf,"numCepstrum");
    numFrames = (int) GetIAttribute(asdf, "numFrames");
    melCepstrumCosineTransform = (VECTOR_OF_F_VECTORS *) 
      GetPtrAttribute(asdf,"melCepstrumCosineTransform");
    signal = (float *) AllocFloatArray(signal, fftSize+1);
    resAx = (float *) AllocFloatArray(resAx, fftSize+1);
    resAy = (float *) AllocFloatArray(resAy, fftSize+1);
    amag = (float *) AllocFloatArray(amag, fftSize+1);
    phase = (float *) AllocFloatArray(phase, fftSize+1);
    tempFVector = (F_VECTOR *) AllocFVector(numFilters);
    gDelayFvect = (F_VECTOR *) AllocFVector(windowSize);
    prevAsdf = asdf;
  }
  gDelayFvect = (F_VECTOR *) FrameComputeResidualGDelay(asdf, frameIndex, gDelayFvect);
  for (i = 0; i < windowSize; i++)
    signal[i+1] = gDelayFvect->array[i];
 for (i = windowSize; i <= fftSize; i++)
   signal[i] = 0;
 Window(&signal[1],windowSize,'M','D',1.0);
 Rfft(signal, resAx, resAy, fftOrder, fftSize, -1);
 SpectrumReal(fftSize, resAx, resAy, amag, phase);
 tempFVector = (F_VECTOR *) FilterbankEnergyIntegration(asdf,amag,tempFVector);
 for (i = 0; i < tempFVector->numElements; i++)
   if (tempFVector->array[i] != 0)
     tempFVector->array[i] = log(tempFVector->array[i]);
   else
     tempFVector->array[i] = log(1.0E-10);
 LinearTransformationOfFVector(tempFVector,melCepstrumCosineTransform,  numCepstrum, numFilters, fvect); 

 return(fvect);
}
/*-------------------------------------------------------------------------
 *  FrameComputeResGdCepstrumMean -- Computes the Cepstrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeMelResGdCepstrumMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL;
  int              i,j;
  int              numVoicedFrames = 0;
  
  numCepstrum = GetIAttribute(asdf,"numCepstrum");
  numFrames = GetIAttribute(asdf, "numFrames");
  numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
  meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
  for (i = 0; i < numCepstrum; i++)
    meanVector->array[i] = 0;
  for (i = 0; i < numFrames; i++) 
    if (asdf->vU[i]) {
      fvect = (F_VECTOR *) FrameComputeMelResGdCepstrumRaw(asdf,i, fvect);
      for ( j = 0; j < numCepstrum; j++ )
	meanVector->array[j] = fvect->array[j] + meanVector->array[j];
    }
  for ( i = 0; i < numCepstrum; i++ )
    fvect->array[i] = meanVector->array[i]/numVoicedFrames;
  return(fvect);
  
}	/*  End of FrameComputeMelResGdCepstrumMean	*/	


/*-------------------------------------------------------------------------
 *  FrameComputeMelResGdCepstrumVariance -- Computes the Res Gd Cepstrum Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeMelResGdCepstrumVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeMelResGdCepstrumMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeMelResGdCepstrumRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeMelCepstrumVariance	*/	

/*--------------------------------------------------------------------------
  FrameComputeMelResGdCepstrum : Computes the cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelResGdCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector=NULL, *varVector;
  static int       zeroMean, featureVarNormalize;
  static char      oldName[500];
  int              i;
  static float     sqrtVal;

 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean) || (featureVarNormalize)){
     meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
     varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   }
   prevAsdf = asdf;
}
 if (((zeroMean)||(featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
     meanVector = (F_VECTOR *) FrameComputeMelResGdCepstrumMean(asdf, 0, meanVector);
     varVector = (F_VECTOR *) FrameComputeMelResGdCepstrumVariance(asdf, 0, varVector);
   oldName[0] = '\0';
   strcpy(oldName, asdf->waveFileName);
 }

 fvect = (F_VECTOR *) FrameComputeMelResGdCepstrumRaw (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numCepstrum; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numCepstrum; i++) {
     sqrtVal = (float) sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtVal;
   }

 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeMelResGdDeltaCepstrum : Computes the deltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelResGdDeltaCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
}
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeMelResGdCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeMelResGdCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeMelResGdDeltaDeltaCepstrum : Computes the deltaDeltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaCepstrum returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelResGdDeltaDeltaCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeMelResGdDeltaCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeMelResGdDeltaCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeMelResGdAugmentedCepstrum : Computes the augmentedCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelResGdAugmentedCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeMelResGdCepstrum(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeMelResGdDeltaCepstrum(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeMelResGdDeltaDeltaCepstrum(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];

  return(fvect);
}
/*--------------------------------------------------------------------------
 FrameComputeResidualModGDelay : Computes the modified group delay of the residual
                          from a frame of speech

 Inputs : waveform, frameIndex, frameShift, frameSize, total samples in
          waveform.

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeResidualModGDelay(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, resGdWindowSize, numSamples, 
                                 fftSize, fftOrder, lpOrder;
  static int                     frameNum, numFrames, frameShift;
  static float                         alfaP, alfaN, gamma;
  static int                     numCepstrum, smthWinSize, 
                                 startIndex, endIndex, 
                                 minFreq, maxFreq, samplingRate, 
                                 gdSign, removeLPhase;
  static                         char oldName[500];
  static F_VECTOR                *waveform;
  static float                   *resEnergy=NULL, *gDelay=NULL, 
                                 *gDelaySmoothed=NULL, 
                                 *residual=NULL, med[4];
  int                            i, iloc;
  static float                   average, *gdAverage=NULL, 
                                 *signal=NULL , *coef=NULL, gain;

if((prevAsdf == NULL) || (strcmp(oldName,asdf->waveFileName) != 0)){
   windowSize = GetIAttribute(asdf, "windowSize");
   resGdWindowSize = GetIAttribute(asdf, "resGdWindowSize");
   numSamples = GetIAttribute(asdf, "numSamples");
   frameShift = GetIAttribute(asdf, "frameAdvanceSamples");
   numFrames = GetIAttribute(asdf, "numFrames");
   fftSize = GetIAttribute(asdf, "fftSize");
   fftOrder = GetIAttribute(asdf, "fftOrder");
   lpOrder = GetIAttribute(asdf, "lpOrder");
   gdSign = (int) GetIAttribute(asdf, "gdSign");
   smthWinSize = (int) GetIAttribute(asdf, "gdSmthWinSize");
   numCepstrum = (int) GetIAttribute(asdf, "numCepstrum");
   removeLPhase = (int) GetIAttribute (asdf, "gdRemoveLPhase");
   minFreq = (float) GetFAttribute(asdf, "minFrequency");
   maxFreq = (float) GetFAttribute(asdf, "maxFrequency");
   samplingRate = (float) GetFAttribute(asdf, "samplingRate");
   startIndex = minFreq*fftSize/samplingRate;
   endIndex = maxFreq*fftSize/samplingRate;
   alfaN = (float ) GetFAttribute(asdf, "gdNegScale");
   alfaP = (float ) GetFAttribute(asdf, "gdPosScale");
   gamma = (float ) GetFAttribute(asdf, "gamma");
   if (signal == NULL) {
     signal = (float *) AllocFloatArray(signal, windowSize+1);
     resEnergy = (float *) AllocFloatArray(resEnergy, windowSize+1);
     gDelay = (float *) AllocFloatArray(gDelay, fftSize+1);
     gDelaySmoothed = (float *) AllocFloatArray(gDelaySmoothed, fftSize+1);
     coef = (float *) AllocFloatArray(coef, lpOrder+1);
   }
   if ((gdAverage == NULL) && (residual == NULL)) {
     gdAverage = (float *) AllocFloatArray(gdAverage, numSamples+1);
     residual = (float *) AllocFloatArray(residual, numSamples+1);
   } else {
     gdAverage = (float *) realloc(gdAverage, (numSamples+1)*sizeof(float));
     residual = (float *) realloc(residual, (numSamples+1)*sizeof(float));
     if ((gdAverage == NULL) || (residual == NULL)) {
       printf("unable to allocate space for residual/gdAverage\n");
       fflush(stdout);
       exit(-1);
     }
   }
   iloc = 0;
   for (frameNum = 0; frameNum < numFrames; frameNum++)
     {	
       waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameNum, waveform);
       for (i = 1; i <= windowSize; i++)
	 signal[i] = waveform->array[i-1];
       /* *HamDw(i,windowSize); */
       LpAnal(signal,resEnergy,windowSize, frameShift, coef, lpOrder,&gain);
       for (i = 1; i <= frameShift; i++) {
	 if ((iloc+i-1) < numSamples)
	   residual[iloc+i-1] = resEnergy[i];
       }
       iloc = iloc+frameShift;
     }
   iloc = 0;
   while (iloc < numSamples) {	
     for (i = 1; i <= windowSize; i++)
       if ((iloc+i-1) < numSamples) 
	 resEnergy[i] = residual[iloc+i-1];
       else
	 resEnergy[i] = 0.0000001;
     StandardModGd (resEnergy, windowSize, fftSize, fftOrder, smthWinSize,
		    gamma, alfaP, alfaN, 0, gDelay);
     for (i = 2; i <= fftSize-1; i++) {
       med[1] = gDelay[i-1];
       med[2] = gDelay[i];
       med[3] = gDelay[i+1];
       gDelaySmoothed[i] = (float) Median (med,3);
     }
     gDelay[1] = gDelay[2];
     gDelay[fftSize] = gDelay[fftSize-1];
     gDelaySmoothed[1] = gDelaySmoothed[2];
     gDelaySmoothed[fftSize] = gDelaySmoothed[fftSize-1];
     ComputeAverage(gDelaySmoothed, fftSize/2, &average);
     gdAverage[iloc] = average-windowSize/2;
     iloc++;
   }
   oldName[0] = '\0';
   strcpy(oldName,asdf->waveFileName);
   prevAsdf = asdf;
}
 for (i = 0; i < resGdWindowSize; i++)
   fvect->array[i] = gdAverage[frameIndex*frameShift+i]; 
 return(fvect);
}

/*--------------------------------------------------------------------------
 FrameComputeModGDelay : Computes the modified group delay 
                          for a frame of speech

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeModGDelay(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize,
                                 fftSize, fftOrder;
  static float                   alfaP, alfaN, gamma;
  static int                     medianOrder, smthWinSize; 
  static F_VECTOR                *waveform;
  static float                   *gDelay=NULL;
  static float                   *signal=NULL;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    smthWinSize = (int) GetIAttribute(asdf, "gdSmthWinSize");
    medianOrder = (int) GetIAttribute(asdf, "medianOrder");
    gamma = (float) GetFAttribute(asdf, "gamma");
    alfaP = (float) GetFAttribute(asdf, "gdPosScale");
    alfaN = (float) GetFAttribute(asdf, "gdNegScale");
    gDelay = (float *) AllocFloatArray(gDelay, fftSize+1);
    signal = (float *) AllocFloatArray(signal, fftSize+1);
    waveform = (F_VECTOR *) AllocFVector (windowSize);
    prevAsdf = asdf;
  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    signal[i] = waveform->array[i-1];//*HamDw(i,windowSize); 
  StandardModGd (signal, windowSize, fftSize, fftOrder, smthWinSize,
		 gamma, alfaP, alfaN, medianOrder, gDelay);
  for (i = 0; i < fftSize/2; i++)
    fvect->array[i] = gDelay[i]; 
  return(fvect);
}

/*--------------------------------------------------------------------------
 FrameComputeSourceModGDelay : Computes the modified group delay 
                          for the source spectrum of a frame of speech

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeSourceModGDelay(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, winLen, pthSmthWinLen,
                                 fftSize, fftOrder;
  static float                   alfaP, alfaN, gamma, c0, specAverage;
  static int                     medianOrder, smthWinSize, minPitch, maxPitch, windowBeg, windowEnd; 
  static F_VECTOR                *waveform;
  static float                   *gDelay=NULL, *ax=NULL, *ay=NULL, *amag=NULL, *cepAmag=NULL;
  static float                   *sigTemp=NULL, *phase=NULL;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    smthWinSize = (int) GetIAttribute(asdf, "gdSmthWinSize");
    winLen = (int) GetIAttribute(asdf, "numCepstrum");
    pthSmthWinLen = (int) GetIAttribute(asdf, "perceptNumCepstrum");
    medianOrder = (int) GetIAttribute(asdf, "medianOrder");
    minPitch = (int) GetIAttribute(asdf, "minPitch");    
    maxPitch = (int) GetIAttribute(asdf, "maxPitch");
    gamma = (float) GetFAttribute(asdf, "gamma");
    alfaP = (float) GetFAttribute(asdf, "gdPosScale");
    alfaN = (float) GetFAttribute(asdf, "gdNegScale");
    gDelay = (float *) AllocFloatArray(gDelay, fftSize+1);
    waveform = (F_VECTOR *) AllocFVector (windowSize);
    ax = (float *) AllocFloatArray(ax, fftSize+1);
    ay = (float *) AllocFloatArray(ay, fftSize+1);
    amag = (float *) AllocFloatArray(amag, fftSize+1);
    cepAmag = (float *) AllocFloatArray(cepAmag, fftSize+1);
    phase = (float *) AllocFloatArray(phase, fftSize+1);
    sigTemp = (float *) AllocFloatArray(sigTemp, fftSize+1);
    windowBeg = minPitch+1;
    windowEnd = maxPitch+1;
    prevAsdf = asdf;

  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    sigTemp[i] = waveform->array[i-1];//*HamDw(i, windowSize);
  for(i = windowSize+1; i<= fftSize; i++)
    sigTemp[i] = 0.0;
  Rfft(sigTemp,ax,ay,fftOrder,fftSize,-1); 
  SpectrumReal(fftSize,ax,ay,amag,phase);
  for( i = 1; i <= fftSize; i++)
    ax[i] = amag[i];
  CepSmooth(ax,cepAmag,fftOrder,fftSize, winLen, &c0, gamma);
  for (i = windowBeg; i <= windowEnd; i++) {
    sigTemp[i-windowBeg+1] = log(amag[i]/cepAmag[i]);
  } 
  //  RemoveAverage(sigTemp, windowEnd-windowBeg+1, &specAverage);
  for(i = windowEnd-windowBeg+1; i<= fftSize; i++)
    sigTemp[i] = 0.0;
  gDelay = (float *) StandardModGd (sigTemp, windowSize, fftSize, fftOrder, smthWinSize, gamma, 
     alfaP, alfaN, medianOrder, gDelay);
  Rfft(gDelay,ax,ay,fftOrder,fftSize,1);
  for (i = 2; i <= pthSmthWinLen; i++) {
    ax[i] = ax[i]*HanW(i,pthSmthWinLen);
    ax[fftSize-i+2] = ax[i];
  }
  for (i = pthSmthWinLen+1; i <= fftSize-pthSmthWinLen-1; i++)
    ax[i] = 0.0;
  Rfft(ax,gDelay,ay,fftOrder,fftSize,-1); 
  for (i = 0; i < fftSize/2; i++)
    fvect->array[i] = gDelay[i]; 
  return(fvect);
}
/*--------------------------------------------------------------------------
 FrameComputeModGDelayPitch : Computes the modified group delay pitch for a frame of speech
  

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with pitch

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeModGDelayPitch(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, winLen, gdSmthWinSize, samplingRate,
                                 fftSize, fftOrder, timeOrFreq;
  static float                   alfaP, alfaN, gamma, specAverage, winScaleFactor;
  static int                     medianOrder, minPitch, maxPitch; 
  static F_VECTOR                *waveform;
  static float                   *gDelay=NULL, *ax=NULL, *ay=NULL, *amag=NULL, *cepAmag=NULL;
  static float                   *sigTemp=NULL, *phase=NULL, gdelayPitch, tonicFrequency;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    winLen = (int) GetIAttribute(asdf, "numCepstrum");
    samplingRate = (int) GetIAttribute(asdf, "samplingRate");
    gdSmthWinSize = (int) GetIAttribute(asdf, "gdSmthWinSize");
    medianOrder = (int) GetIAttribute(asdf, "medianOrder");
    minPitch = (int) GetIAttribute(asdf, "minPitch");    
    maxPitch = (int) GetIAttribute(asdf, "maxPitch");
    timeOrFreq = (int) GetIAttribute(asdf, "timeOrFreq");
    gamma = (float) GetFAttribute(asdf, "gamma");
    winScaleFactor = (float) GetFAttribute(asdf, "winScaleFactor");
    alfaP = (float) GetFAttribute(asdf, "gdPosScale");
    alfaN = (float) GetFAttribute(asdf, "gdNegScale");
    tonicFrequency = (float) GetFAttribute(asdf, "tonic");
    waveform = (F_VECTOR *) AllocFVector (windowSize);
    sigTemp = (float *) AllocFloatArray(sigTemp, fftSize+1);
    prevAsdf = asdf;

  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    sigTemp[i] = waveform->array[i-1];//*HamDw(i, windowSize);
  for(i = windowSize+1; i<= fftSize; i++)
    sigTemp[i] = 0.0;
  gdelayPitch = (float ) PitchModifiedGd(sigTemp,windowSize,
						fftSize,fftOrder,
						minPitch, maxPitch, 
						winLen, winScaleFactor, gdSmthWinSize, medianOrder, 
                                                gamma, alfaP, alfaN);
  if (timeOrFreq == 0)
    fvect->array[0] = gdelayPitch/samplingRate;
  if (tonicFrequency != 0)
    fvect->array[0] = (float) ConvertFreqToCent (gdelayPitch, tonicFrequency);
    else
      fvect->array[0] = 1.0/gdelayPitch/samplingRate;
  return(fvect);
}

/*--------------------------------------------------------------------------
 FrameComputeMinGDelayPitch : Computes the minphase group delay pitch for a frame of speech
  

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with pitch

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeMinGDelayPitch(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, winLen, pthSmthWinLen, samplingRate,
                                 fftSize, fftOrder, timeOrFreq;
  static float                   gamma;
  static int                     minPitch, maxPitch; 
  static F_VECTOR                *waveform;
  static float                   *gDelay=NULL, *ax=NULL, *ay=NULL, *amag=NULL, *cepAmag=NULL;
  static float                   *sigTemp=NULL, *phase=NULL, minGdelayPitch, tonicFrequency;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    samplingRate = (int) GetIAttribute(asdf, "samplingRate");
    minPitch = (int) GetIAttribute(asdf, "minPitch");    
    maxPitch = (int) GetIAttribute(asdf, "maxPitch");
    timeOrFreq = (int) GetIAttribute(asdf, "timeOrFreq");
    gamma = (float) GetFAttribute(asdf, "gamma");
    tonicFrequency = (float) GetFAttribute(asdf, "tonic");
    waveform = (F_VECTOR *) AllocFVector (windowSize);
    sigTemp = (float *) AllocFloatArray(sigTemp, fftSize+1);
    prevAsdf = asdf;

  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    sigTemp[i] = waveform->array[i-1];//*HamDw(i, windowSize);
  for(i = windowSize+1; i<= fftSize; i++)
    sigTemp[i] = 0.0;
  minGdelayPitch = (float ) PitchMinGd(sigTemp,windowSize,
					   fftSize, fftOrder,
					      minPitch, maxPitch, gamma);
  if (timeOrFreq == 0)
    fvect->array[0] = minGdelayPitch/samplingRate;
  if (tonicFrequency != 0)
    fvect->array[0] = (float) ConvertFreqToCent (minGdelayPitch, tonicFrequency);
    else
      fvect->array[0] = 1.0/minGdelayPitch/samplingRate;
  return(fvect);
}

/*--------------------------------------------------------------------------
 FrameComputeCepstrumPitch : Computes the minphase group delay pitch for a frame of speech
  

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with pitch

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeCepstrumPitch(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, winLen, pthSmthWinLen, samplingRate,
                                 fftSize, fftOrder, timeOrFreq;
  static int                     smthWinSize, minPitch, maxPitch, windowBeg, windowEnd; 
  static F_VECTOR                *waveform;
  static float                   *gDelay=NULL, *ax=NULL, *ay=NULL, *amag=NULL, *cepAmag=NULL;
  static float                   *sigTemp=NULL, *phase=NULL, cepstrumPitch, tonicFrequency;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    samplingRate = (int) GetIAttribute(asdf, "samplingRate");
    minPitch = (int) GetIAttribute(asdf, "minPitch");    
    maxPitch = (int) GetIAttribute(asdf, "maxPitch");
    timeOrFreq = (int) GetIAttribute(asdf, "timeOrFreq");
    tonicFrequency = (float) GetFAttribute(asdf, "tonic");
    waveform = (F_VECTOR *) AllocFVector (windowSize);
    sigTemp = (float *) AllocFloatArray(sigTemp, fftSize+1);
    prevAsdf = asdf;

  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    sigTemp[i] = waveform->array[i-1];//*HamDw(i, windowSize);
  for(i = windowSize+1; i<= fftSize; i++)
    sigTemp[i] = 0.0;
  cepstrumPitch = (float ) PitchCepstrum(sigTemp,windowSize,
					   fftSize, fftOrder,
					      minPitch, maxPitch);
  if (timeOrFreq == 0)
    fvect->array[0] = cepstrumPitch/samplingRate;
  if (tonicFrequency != 0)
    fvect->array[0] = (float) ConvertFreqToCent (cepstrumPitch, tonicFrequency);
    else
      fvect->array[0] = 1.0/cepstrumPitch/samplingRate;
  return(fvect);
}

/*--------------------------------------------------------------------------
 FrameComputeSourceLPModGDelay : Computes the modified group delay 
                          for the source spectrum of a frame of speech

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeSourceLPModGDelay(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, winLen,pthSmthWinLen,
                                 fftSize, fftOrder, lpOrder,minPitch, maxPitch;
  static float                   alfaP, alfaN, gamma, winScaleFactor, c0, specAverage;
  static int                     medianOrder, smthWinSize, windowBeg, windowEnd; 
  static F_VECTOR                *waveform;
  static float                   *gDelay=NULL, *ax=NULL, *ay=NULL, *amag=NULL, *cepAmag=NULL;
  static float                   *sigTemp=NULL, *phase=NULL;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    winScaleFactor = GetFAttribute(asdf, "winScaleFactor");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    lpOrder =  GetIAttribute(asdf, "lpOrder");
    smthWinSize = (int) GetIAttribute(asdf, "gdSmthWinSize");
    winLen = GetIAttribute(asdf, "numCepstrum");
    pthSmthWinLen = GetIAttribute(asdf, "perceptNumCepstrum");
    medianOrder = (int) GetIAttribute(asdf, "medianOrder");
    gamma = (float) GetFAttribute(asdf, "gamma");
    alfaP = (float) GetFAttribute(asdf, "gdPosScale");
    alfaN = (float) GetFAttribute(asdf, "gdNegScale");
    minPitch = (int) GetIAttribute(asdf, "minPitch");
    maxPitch = (float) GetIAttribute(asdf, "maxPitch");
    windowBeg = minPitch+1;
    windowEnd = maxPitch+1;
    gDelay = (float *) AllocFloatArray(gDelay, fftSize+1);
    waveform = (F_VECTOR *) AllocFVector (windowSize);
    ax = (float *) AllocFloatArray(ax, fftSize+1);
    ay = (float *) AllocFloatArray(ay, fftSize+1);
    amag = (float *) AllocFloatArray(amag, fftSize+1);
    cepAmag = (float *) AllocFloatArray(cepAmag, fftSize+1);
    phase = (float *) AllocFloatArray(phase, fftSize+1);
    sigTemp = (float *) AllocFloatArray(sigTemp, fftSize+1);
    prevAsdf = asdf;
  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    sigTemp[i] = waveform->array[i-1];//*HamDw(i, windowSize);
  for(i = windowSize; i<= fftSize; i++)
    sigTemp[i] = 0.0;
  Rfft(sigTemp, ax, ay, fftOrder, fftSize, -1);
  SpectrumReal(fftSize, ax, ay, amag, phase);
  for( i = 1; i <= fftSize; i++)
    ax[i] = amag[i];
  LogCepSmooth(ax,cepAmag,fftOrder,fftSize,winLen,&c0, gamma);
  for (i = windowBeg; i <= windowEnd; i++) {
    sigTemp[i-windowBeg+1] = log(amag[i]/cepAmag[i]);
  }
  //  RemoveAverage(&sigTemp, windowEnd-windowBeg+1, &specAverage);
  for(i = windowEnd-windowBeg+1; i<= fftSize; i++)
    sigTemp[i] = 0.0;
  gDelay = (float *) StandardModGdLP(sigTemp, windowSize, fftSize, fftOrder, lpOrder, gamma, 
				     alfaP, alfaN, medianOrder, gDelay);
  Rfft(gDelay,ax,ay,fftOrder,fftSize,1);
  for (i = 2; i <= pthSmthWinLen; i++) {
    ax[i] = ax[i]*HanW(i,pthSmthWinLen);
    ax[fftSize-i+2] = ax[i];
  }
  for (i = pthSmthWinLen+1; i <= fftSize-pthSmthWinLen-1; i++)
    ax[i] = 0.0;
  Rfft(ax,gDelay,ay,fftOrder, fftSize,-1); 
  for (i = 0; i < fftSize/2; i++)
    fvect->array[i] = gDelay[i]; 
  return(fvect);
}
/*--------------------------------------------------------------------------
 FrameComputeSmthSpectrumRootCepstrum : Computes the  Smoothed
                          spectrum of a frame of speech
                          using root cepstrum smoothing

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeSmthSpectrumRootCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, winLen, 
                                 fftSize, fftOrder;
  static float                   gamma, c0, specAverage;
  static int                     minPitch, maxPitch, windowBeg, windowEnd;
  static F_VECTOR                *waveform;
  static float                   *ax=NULL, *ay=NULL, *amag=NULL, *cepAmag=NULL;
  static float                   *sigTemp=NULL, *phase=NULL;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    winLen = (int) GetIAttribute(asdf, "numCepstrum");
    minPitch = (int) GetIAttribute(asdf, "minPitch");    
    maxPitch = (int) GetIAttribute(asdf, "maxPitch");
    gamma = (float) GetFAttribute(asdf, "gamma");
    waveform = (F_VECTOR *) AllocFVector (windowSize);
    ax = (float *) AllocFloatArray(ax, fftSize+1);
    ay = (float *) AllocFloatArray(ay, fftSize+1);
    amag = (float *) AllocFloatArray(amag, fftSize+1);
    cepAmag = (float *) AllocFloatArray(cepAmag, fftSize+1);
    phase = (float *) AllocFloatArray(phase, fftSize+1);
    sigTemp = (float *) AllocFloatArray(sigTemp, fftSize+1);
    windowBeg = minPitch+1;
    windowEnd = maxPitch+1;
    prevAsdf = asdf;

  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    sigTemp[i] = waveform->array[i-1];//*HamDw(i, windowSize);
  for(i = windowSize+1; i<= fftSize; i++)
    sigTemp[i] = 0.0;
  Rfft(sigTemp,ax,ay,fftOrder,fftSize,-1); 
  SpectrumReal(fftSize,ax,ay,amag,phase);
  for( i = 1; i <= fftSize; i++)
    ax[i] = amag[i];
  CepSmooth(ax,cepAmag,fftOrder,fftSize, winLen, &c0, gamma);
  for (i = windowBeg; i <= windowEnd; i++) {
    sigTemp[i-windowBeg+1] = cepAmag[i];
  } 
  InitFVector(fvect);
  for (i = 0; i < windowEnd-windowBeg+1; i++)
    fvect->array[i] = log(sigTemp[i+1]); 
  return(fvect);
}

/*--------------------------------------------------------------------------
 FrameComputeSpectrumSmthLogCepstrum : Computes the smoothed
                          spectrum of a frame of speech using
                          Log Cepstrum Smoothing

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeSmthSpectrumLogCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, winLen, 
                                 fftSize, fftOrder;
  static float                   gamma, c0, specAverage;
  static int                     minPitch, maxPitch, windowBeg, windowEnd; 
  static F_VECTOR                *waveform;
  static float                   *ax=NULL, *ay=NULL, *amag=NULL, *cepAmag=NULL;
  static float                   *sigTemp=NULL, *phase=NULL;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    winLen = (int) GetIAttribute(asdf, "numCepstrum");
    minPitch = (int) GetIAttribute(asdf, "minPitch");    
    maxPitch = (int) GetIAttribute(asdf, "maxPitch");
    gamma = (float) GetFAttribute(asdf, "gamma");
    waveform = (F_VECTOR *) AllocFVector (windowSize);
    ax = (float *) AllocFloatArray(ax, fftSize+1);
    ay = (float *) AllocFloatArray(ay, fftSize+1);
    amag = (float *) AllocFloatArray(amag, fftSize+1);
    cepAmag = (float *) AllocFloatArray(cepAmag, fftSize+1);
    phase = (float *) AllocFloatArray(phase, fftSize+1);
    sigTemp = (float *) AllocFloatArray(sigTemp, fftSize+1);
    windowBeg = minPitch+1;
    windowEnd = maxPitch+1;
    prevAsdf = asdf;

  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    sigTemp[i] = waveform->array[i-1];//*HamDw(i, windowSize);
  for(i = windowSize+1; i<= fftSize; i++)
    sigTemp[i] = 0.0;
  Rfft(sigTemp,ax,ay,fftOrder,fftSize,-1); 
  SpectrumReal(fftSize,ax,ay,amag,phase);
  for( i = 1; i <= fftSize; i++)
    ax[i] = amag[i];
  LogCepSmooth(ax,cepAmag,fftOrder,fftSize, winLen, &c0, gamma);
  for (i = windowBeg; i <= windowEnd; i++) {
    sigTemp[i-windowBeg+1] = cepAmag[i];
  } 
  InitFVector(fvect);
  for (i = 0; i < windowEnd-windowBeg+1; i++)
    fvect->array[i] = log(sigTemp[i+1]); 
  return(fvect);
}
/*--------------------------------------------------------------------------
 FrameComputeFlatSpectrumRootCepstrum : Computes the  flattened
                          source spectrum of a frame of speech
                          using root cepstrum smoothing

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeFlatSpectrumRootCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, winLen, 
                                 fftSize, fftOrder;
  static float                   gamma, c0, specAverage;
  static int                     minPitch, maxPitch, windowBeg, windowEnd;
  static F_VECTOR                *waveform;
  static float                   *ax=NULL, *ay=NULL, *amag=NULL, *cepAmag=NULL;
  static float                   *sigTemp=NULL, *phase=NULL;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    winLen = (int) GetIAttribute(asdf, "numCepstrum");
    minPitch = (int) GetIAttribute(asdf, "minPitch");    
    maxPitch = (int) GetIAttribute(asdf, "maxPitch");
    gamma = (float) GetFAttribute(asdf, "gamma");
    waveform = (F_VECTOR *) AllocFVector (windowSize);
    ax = (float *) AllocFloatArray(ax, fftSize+1);
    ay = (float *) AllocFloatArray(ay, fftSize+1);
    amag = (float *) AllocFloatArray(amag, fftSize+1);
    cepAmag = (float *) AllocFloatArray(cepAmag, fftSize+1);
    phase = (float *) AllocFloatArray(phase, fftSize+1);
    sigTemp = (float *) AllocFloatArray(sigTemp, fftSize+1);
    windowBeg = minPitch+1;
    windowEnd = maxPitch+1;
    prevAsdf = asdf;

  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    sigTemp[i] = waveform->array[i-1];//*HamDw(i, windowSize);
  for(i = windowSize+1; i<= fftSize; i++)
    sigTemp[i] = 0.0;
  Rfft(sigTemp,ax,ay,fftOrder,fftSize,-1); 
  SpectrumReal(fftSize,ax,ay,amag,phase);
  for( i = 1; i <= fftSize; i++)
    ax[i] = amag[i];
  CepSmooth(ax,cepAmag,fftOrder,fftSize, winLen, &c0, gamma);
  for (i = windowBeg; i <= windowEnd; i++) {
    sigTemp[i-windowBeg+1] = (amag[i]/cepAmag[i]);
  } 
  //  RemoveAverage(sigTemp, windowEnd-windowBeg+1, &specAverage);
  InitFVector(fvect);
  for (i = 0; i < windowEnd-windowBeg+1; i++)
    fvect->array[i] = log(sigTemp[i+1]); 
  return(fvect);
}

/*--------------------------------------------------------------------------
 FrameComputeFlatSpectrumLogCepstrum : Computes the  flattened
                          source spectrum of a frame of speech using
                          Log Cepstrum Smoothing

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeFlatSpectrumLogCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, winLen, 
                                 fftSize, fftOrder;
  static float                   gamma, c0, specAverage;
  static int                     minPitch, maxPitch, windowBeg, windowEnd; 
  static F_VECTOR                *waveform;
  static float                   *ax=NULL, *ay=NULL, *amag=NULL, *cepAmag=NULL;
  static float                   *sigTemp=NULL, *phase=NULL;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    winLen = (int) GetIAttribute(asdf, "numCepstrum");
    minPitch = (int) GetIAttribute(asdf, "minPitch");    
    maxPitch = (int) GetIAttribute(asdf, "maxPitch");
    gamma = (float) GetFAttribute(asdf, "gamma");
    waveform = (F_VECTOR *) AllocFVector (windowSize);
    ax = (float *) AllocFloatArray(ax, fftSize+1);
    ay = (float *) AllocFloatArray(ay, fftSize+1);
    amag = (float *) AllocFloatArray(amag, fftSize+1);
    cepAmag = (float *) AllocFloatArray(cepAmag, fftSize+1);
    phase = (float *) AllocFloatArray(phase, fftSize+1);
    sigTemp = (float *) AllocFloatArray(sigTemp, fftSize+1);
    windowBeg = minPitch+1;
    windowEnd = maxPitch+1;
    prevAsdf = asdf;

  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    sigTemp[i] = waveform->array[i-1];//*HamDw(i, windowSize);
  for(i = windowSize+1; i<= fftSize; i++)
    sigTemp[i] = 0.0;
  Rfft(sigTemp,ax,ay,fftOrder,fftSize,-1); 
  SpectrumReal(fftSize,ax,ay,amag,phase);
  for( i = 1; i <= fftSize; i++)
    ax[i] = amag[i];
  LogCepSmooth(ax,cepAmag,fftOrder,fftSize, winLen, &c0, gamma);
  for (i = windowBeg; i <= windowEnd; i++) {
    sigTemp[i-windowBeg+1] = log(amag[i]/cepAmag[i]);
  } 
  //  RemoveAverage(sigTemp, windowEnd-windowBeg+1, &specAverage);
  InitFVector(fvect);
  for (i = 0; i < windowEnd-windowBeg+1; i++)
    fvect->array[i] = log(sigTemp[i+1]); 
   return(fvect);
}

/*--------------------------------------------------------------------------
 FrameComputeModGDelaySmooth : Computes the smoothed modified group delay 
                          for a frame of speech

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with smoothed modified group delay

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeModGDelaySmooth(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, fftSize, fftOrder, nfBy2;
  static int                     smthWinSize, 
                                 winLen;
  static float                   gamma, alfaP, alfaN; 
  static F_VECTOR                *waveform;
  static float                   *gDelay=NULL, Ave;
  static float                   *signal=NULL, *ax=NULL, *ay=NULL;
  int                            i;
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    nfBy2 = fftSize/2;
    fftOrder = GetIAttribute(asdf, "fftOrder");
    smthWinSize = (int) GetIAttribute(asdf, "gdSmthWinSize");
    winLen = (int) GetIAttribute(asdf, "perceptNumCepstrum");    
    gamma = (float) GetFAttribute(asdf, "gamma");
    alfaP = (float) GetFAttribute(asdf, "gdPosScale");
    alfaN = (float) GetFAttribute(asdf, "gdNegScale");
    ax = (float *) AllocFloatArray(ax, fftSize+1);
    ay = (float *) AllocFloatArray(ay, fftSize+1);
    signal = (float *) AllocFloatArray(signal, fftSize+1);
    waveform = (F_VECTOR *) AllocFVector (windowSize);
    gDelay = (float *) AllocFloatArray(gDelay, fftSize+1);
    prevAsdf = asdf;
  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    signal[i] = waveform->array[i-1];//*HamDw(i,windowSize); 
  StandardModGd (signal, windowSize, fftSize, fftOrder, smthWinSize,
		 gamma, alfaP, alfaN, 0, gDelay);
  Rfft(gDelay, ax, ay, fftOrder, fftSize, 1);
  for (i = 2; i <= winLen; i++) {
    ax[i] = ax[i]*HanW(i,winLen);
    ax[fftSize-i+2] = ax[i];
  }
  for (i = winLen+1; i <= fftSize-winLen+1; i++)
    ax[i] = 0.0;
  Rfft(ax, gDelay, ay, fftOrder, fftSize,-1);
  RemoveAverage(gDelay, fftSize, &Ave);
  for (i = 0; i < nfBy2; i++)
    fvect->array[i] = gDelay[i+1]; 
  return(fvect);
}
/*-------------------------------------------------------------------------
 *  FrameComputeModGDelayMean -- Computes the mean for modified gdelay for voiced frames
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeModGDelayMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      fftSize, numFrames, zeroMean, nfBy2;
  static F_VECTOR  *meanVector=NULL;
  int              i,j, flag;
  static int       numVoicedFrames = 0;
  fftSize = GetIAttribute(asdf,"fftSize");
 nfBy2 = fftSize/2;
 numFrames = GetIAttribute(asdf, "numFrames");
 numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
 zeroMean = GetIAttribute(asdf, "zeroMean");
 meanVector = (F_VECTOR *) AllocFVector(nfBy2);
 for (i = 0; i < nfBy2; i++)
   meanVector->array[i] = 0;
 for (i = 0; i < numFrames; i++) {
   flag = (short) (asdf->vU[i]);
   if (flag) {
     fvect = (F_VECTOR *) FrameComputeModGDelaySmooth(asdf, i, fvect);
     for ( j = 0; j < nfBy2; j++ )
       meanVector->array[j] = fvect->array[j] + meanVector->array[j];
   }
 }
 for ( i = 0; i < nfBy2; i++ )
   meanVector->array[i] = meanVector->array[i]/numVoicedFrames;
 return(meanVector);

}	/*  End of FrameComputeModGDelayMean	*/	


/*-------------------------------------------------------------------------
 *  FrameComputeMinGDelayMean -- Computes the mean for min gdelay for voiced frames
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeMinGDelayMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int       numFrames, zeroMean, nfBy2;
  static F_VECTOR  *meanVector=NULL, *waveform=NULL;
  int              i,j, flag;
  static int       numVoicedFrames = 0;
  static float     *signal=NULL;
  static float     gamma;
  static int       fftSize, fftOrder, winLen, windowSize;


  fftSize = GetIAttribute(asdf,"fftSize");
  fftOrder = GetIAttribute(asdf,"fftOrder");
  windowSize = GetIAttribute(asdf,"windowSize");
  nfBy2 = fftSize/2;
  numFrames = GetIAttribute(asdf, "numFrames");
  numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
  zeroMean = GetIAttribute(asdf, "zeroMean");
  gamma = GetFAttribute (asdf, "gamma");
  winLen = GetIAttribute (asdf, "perceptNumCepstrum");
  meanVector = (F_VECTOR *) AllocFVector(nfBy2);
  waveform = (F_VECTOR *) AllocFVector(windowSize);
  signal = (float *) AllocFloatArray(signal, windowSize+1);
  for (i = 0; i < nfBy2; i++)
    meanVector->array[i] = 0;
  for (i = 0; i < numFrames; i++) {
   flag = (short) (asdf->vU[i]);
   if (flag) {
     waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
     for (j = 1; j <= windowSize; j++)
       signal[j] = waveform->array[j-1];//*HamDw(j,windowSize); 
     meanVector->array = (float *) MinGd(signal, windowSize, fftSize, fftOrder,
			     winLen, meanVector->array, gamma);
     for ( j = 0; j < nfBy2; j++ )
       meanVector->array[j] = fvect->array[j] + meanVector->array[j];
   }
  }
 for ( i = 0; i < nfBy2; i++ )
   meanVector->array[i] = meanVector->array[i]/numVoicedFrames;
 return(meanVector);

}	/*  End of FrameComputeMinGDelayMean	*/	



/*--------------------------------------------------------------------------
 FrameComputeModGDelayLP : Computes the modified group delay 
                          for a frame of speech using LP smoothing

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeModGDelayLP(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize,
                                 fftSize, fftOrder;
  static float                   alfaP, alfaN, gamma;
  static int                     medianOrder, lpOrder; 
  static F_VECTOR                *waveform;
  static float                   *gDelay=NULL;
  static float                   *signal=NULL;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    lpOrder = (int) GetIAttribute(asdf, "lpOrder");
    medianOrder = (int) GetIAttribute(asdf, "medianOrder");
    gamma = (float) GetFAttribute(asdf, "gamma");
    alfaP = (float) GetFAttribute(asdf, "gdPosScale");
    alfaN = (float) GetFAttribute(asdf, "gdNegScale");
    gDelay = (float *) AllocFloatArray(gDelay, fftSize+1);
    signal = (float *) AllocFloatArray(signal, fftSize+1);
    waveform = (F_VECTOR *) AllocFVector (windowSize);
  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    signal[i] = waveform->array[i-1];//*HamDw(i,windowSize); 
  StandardModGdLP (signal, windowSize, fftSize, fftOrder, lpOrder,
		 gamma, alfaP, alfaN, medianOrder, gDelay);
  for (i = 0; i < fftSize/2; i++)
    fvect->array[i] = gDelay[i]; 
  return(fvect);
}
/*--------------------------------------------------------------------------
 FrameComputeModGDelayLPSmooth : Computes the smoothed modified group delay 
                          for a frame of speech using LP smoothing

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with smoothed modified group delay

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeModGDelayLPSmooth(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, fftSize, fftOrder;
  static int                     lpOrder,medianOrder, 
                                 winLen;
  static float                   gamma, alfaP, alfaN; 
  static F_VECTOR                *waveform;
  static float                   *gDelay=NULL, Ave;
  static float                   *signal=NULL, *ax=NULL, *ay=NULL;
  int                            i;
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    lpOrder = (int) GetIAttribute(asdf, "lpOrder");
    winLen = (int) GetIAttribute(asdf, "numCepstrum");    
    medianOrder = (int) GetIAttribute(asdf, "medianOrder");    
    gamma = (float) GetFAttribute(asdf, "gamma");
    alfaP = (float) GetFAttribute(asdf, "gdPosScale");
    alfaN = (float) GetFAttribute(asdf, "gdNegScale");
    gDelay = (float *) AllocFloatArray(gDelay, fftSize+1);
    ax = (float *) AllocFloatArray(ax, fftSize+1);
    ay = (float *) AllocFloatArray(ay, fftSize+1);
    signal = (float *) AllocFloatArray(signal, fftSize+1);
    waveform = (F_VECTOR *) AllocFVector(windowSize);
  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    signal[i] = waveform->array[i-1];//*HamDw(i,windowSize); 
  StandardModGdLP (signal, windowSize, fftSize, fftOrder, lpOrder,
		 gamma, alfaP, alfaN, medianOrder, gDelay);
  Rfft(gDelay, ax, ay, fftOrder, fftSize, 1);
  for (i = 2; i <= winLen; i++) {
    ax[i] = ax[i]*HanW(i,winLen);
    ax[fftSize-i+2] = ax[i];
  }
  for (i = winLen; i <= fftSize-winLen; i++)
    ax[i] = 0.0;
  Rfft(ax,gDelay,ay, fftOrder, fftSize,-1);
  RemoveAverage(gDelay,fftSize,&Ave);
  for (i = 0; i < fftSize/2; i++)
    fvect->array[i] = gDelay[i+1]; 
  return(fvect);
}


/*--------------------------------------------------------------------------
 FrameComputeMinGDelay : Computes the minimum phase group delay 
                          for a frame of speech

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeMinGDelay(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize,
                                 fftSize, fftOrder;
  static float                   gamma;
  static int                     winLen; 
  static F_VECTOR                *waveform=NULL;
  static float                   *gDelay=NULL;
  static float                   *signal=NULL;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    winLen = (int) GetIAttribute(asdf, "numCepstrum");
    gamma = (float) GetFAttribute(asdf, "gamma");
    gDelay = (float *) AllocFloatArray(gDelay, fftSize+1);
    signal = (float *) AllocFloatArray(signal, fftSize+1);
    waveform = (F_VECTOR *)AllocFVector(windowSize);
    prevAsdf = asdf;
  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    signal[i] = waveform->array[i-1];//*HamDw(i,windowSize); 
  MinGd (signal, windowSize, fftSize, fftOrder, winLen,
		 gDelay, gamma);
  for (i = 0; i < fftSize/2; i++)
    fvect->array[i] = gDelay[i+1]; 
  return(fvect);
}


/*--------------------------------------------------------------------------
 FrameComputeLPGDelay : Computes the LP phase group delay 
                          for a frame of speech

 Inputs : asdf, frameIndex, fvect

 Outputs : an F_VECTOR with speech data

---------------------------------------------------------------------------*/


F_VECTOR *FrameComputeLPGDelay(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static ASDF                    *prevAsdf=NULL;
  static int                     windowSize, frameAdvanceSamples,
                                 fftSize, fftOrder, fftBy2;
  static int                     lpOrder; 
  static F_VECTOR                *waveform;
  static float                   *LPGdelay=NULL;
  static float                   *signal=NULL;
  int                            i;
  
  if(prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf, "windowSize");
    frameAdvanceSamples = GetIAttribute(asdf, "frameAdvanceSamples");
    fftSize = GetIAttribute(asdf, "fftSize");
    fftOrder = GetIAttribute(asdf, "fftOrder");
    lpOrder = (int) GetIAttribute(asdf, "lpOrder");
    LPGdelay = (float *) AllocFloatArray(LPGdelay, fftSize+1);
    signal = (float *) AllocFloatArray(signal, windowSize+1);
    waveform = (F_VECTOR *)AllocFVector(windowSize);
    fftBy2 = fftSize/2;
  }
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 1; i <= windowSize; i++)
    signal[i] = waveform->array[i-1]; 
  LPGd (signal, windowSize, frameAdvanceSamples, fftSize, fftOrder, lpOrder,
	LPGdelay);
  fvect->numElements = fftBy2;
  for (i = 1; i <= fftBy2; i++)
    fvect->array[i-1] = LPGdelay[i+1];
  return(fvect);
}

/*----------------------------------------------------------------------------
 FrameComputeMelResModGdCepstrumRaw : Computes the filterbank log cepstrum in a frame of speech.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of "numFilters" filterbank log cepstrum coefficients

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelResModGdCepstrumRaw(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static int                           numFrames, numFilters,
                                       fftSize, fftOrder, windowSize, numCepstrum;
  static ASDF                          *prevAsdf=NULL;
  static F_VECTOR                      *tempFVector, *gDelayFvect;
  static VECTOR_OF_F_VECTORS           *melCepstrumCosineTransform;
  static float                         *signal, *resAx, *resAy, *amag, *phase;
  static int                           i; 
  if (prevAsdf != asdf) {
    numFilters = (int) GetIAttribute(asdf,"numFilters");
    fftSize = (int) GetIAttribute(asdf,"fftSize");
    fftOrder = (int) GetIAttribute(asdf,"fftOrder");
    windowSize = (int) GetIAttribute(asdf,"windowSize");
    numCepstrum = (int) GetIAttribute(asdf,"numCepstrum");
    numFrames = (int) GetIAttribute(asdf, "numFrames");
    melCepstrumCosineTransform = (VECTOR_OF_F_VECTORS *) 
      GetPtrAttribute(asdf,"melCepstrumCosineTransform");
    signal = (float *) AllocFloatArray(signal, fftSize+1);
    resAx = (float *) AllocFloatArray(resAx, fftSize+1);
    resAy = (float *) AllocFloatArray(resAy, fftSize+1);
    amag = (float *) AllocFloatArray(amag, fftSize+1);
    phase = (float *) AllocFloatArray(phase, fftSize+1);
    tempFVector = (F_VECTOR *) AllocFVector(numFilters);
    gDelayFvect = (F_VECTOR *) AllocFVector(windowSize);
    prevAsdf = asdf;
  }
  gDelayFvect = (F_VECTOR *) FrameComputeResidualModGDelay(asdf, frameIndex, gDelayFvect);
  for (i = 0; i < windowSize; i++)
    signal[i+1] = gDelayFvect->array[i];
 for (i = windowSize; i <= fftSize; i++)
   signal[i] = 0;
 Window(&signal[1],windowSize,'M','D',1.0);
 Rfft(signal, resAx, resAy, fftOrder, fftSize, -1);
 SpectrumReal(fftSize, resAx, resAy, amag, phase);
 tempFVector = (F_VECTOR *) FilterbankEnergyIntegration(asdf,amag,tempFVector);
 for (i = 0; i < tempFVector->numElements; i++)
   if (tempFVector->array[i] != 0)
     tempFVector->array[i] = log(tempFVector->array[i]);
   else
     tempFVector->array[i] = log(1.0E-10);
 LinearTransformationOfFVector(tempFVector,melCepstrumCosineTransform,  numCepstrum, numFilters, fvect); 

 return(fvect);
}
/*-------------------------------------------------------------------------
 *  FrameComputeMelResModGdCepstrumMean -- Computes the Cepstrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeMelResModGdCepstrumMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL;
  int              i,j;
  int              numVoicedFrames = 0;

  numCepstrum = GetIAttribute(asdf,"numCepstrum");
  numFrames = GetIAttribute(asdf, "numFrames");
  numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
  meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
  for (i = 0; i < numCepstrum; i++)
    meanVector->array[i] = 0;
  for (i = 0; i < numFrames; i++) 
    if (asdf->vU[i]){
      fvect = (F_VECTOR *) FrameComputeMelResModGdCepstrumRaw(asdf,i, fvect);
      for ( j = 0; j < numCepstrum; j++ )
	meanVector->array[j] = fvect->array[j] + meanVector->array[j];
    }
  for ( i = 0; i < numCepstrum; i++ )
    fvect->array[i] = meanVector->array[i]/numVoicedFrames;
  return(fvect);
 
}	/*  End of FrameComputeMelResModGdCepstrumMean	*/	


/*-------------------------------------------------------------------------
 *  FrameComputeMelResModGdCepstrumVariance -- Computes the Cepstrum Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeMelResModGdCepstrumVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeMelResModGdCepstrumMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeMelResModGdCepstrumRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeMelResModGdCepstrumVariance	*/	


/*--------------------------------------------------------------------------
  FrameComputeMelResModGdCepstrum : Computes the cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelResModGdCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL;
  static int       zeroMean, featureVarNormalize;
  static char      oldName[500];
  int              i;
  static float     sqrtVal;

 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean) || (featureVarNormalize)){
     meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
     varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   }
   prevAsdf = asdf;
}
 if (((zeroMean)||(featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
     meanVector = (F_VECTOR *) FrameComputeMelResModGdCepstrumMean(asdf, 0, meanVector);
     varVector = (F_VECTOR *) FrameComputeMelResModGdCepstrumVariance(asdf, 0, varVector);
   oldName[0] = '\0';
   strcpy(oldName, asdf->waveFileName);
 }

 fvect = (F_VECTOR *) FrameComputeMelResModGdCepstrumRaw (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numCepstrum; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numCepstrum; i++) {
     sqrtVal = (float) sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtVal;
   }
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeMelResModGdDeltaCepstrum : Computes the deltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelResModGdDeltaCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
}
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeMelResModGdCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeMelResModGdCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeMelResModGdDeltaDeltaCepstrum : Computes the deltaDeltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaCepstrum returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelResModGdDeltaDeltaCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeMelResModGdDeltaCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeMelResModGdDeltaCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeMelResModGdAugmentedCepstrum : Computes the augmentedCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelResModGdAugmentedCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeMelResModGdCepstrum(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeMelResModGdDeltaCepstrum(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum]; 
  temp = (F_VECTOR *) FrameComputeMelResModGdDeltaDeltaCepstrum(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];

  return(fvect);
}
  
/*----------------------------------------------------------------------------
 FrameComputeModGdCepstrumNcNRaw : Computes the modified group delay cepstrum
                                     for a frame of speech.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of "numCepstrum" cepstral coefficients

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeModGdCepstrumNcNRaw(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static int                           numFilters, windowSize;
  static int                           numCepstrum;
  static int                           fftOrder, fftSize, samplingRate;
  static int                           smthWinSize, removeLPhase, gdSign;
  static int                           startIndex, endIndex;
  static float                         minFrequency, maxFrequency;
  static int                           removeMin, medianOrder, mgdNormalize;
  static float                         gamma, alfaP, alfaN;
  static ASDF                          *prevAsdf=NULL;
  static F_VECTOR                      *waveform;
  static float                         *farray; 
  int                                  i;
  if (prevAsdf != asdf) {
    numFilters = (int) GetIAttribute(asdf,"numFilters");
    numCepstrum = (int) GetIAttribute(asdf,"numCepstrum");
    fftSize = (int) GetIAttribute(asdf, "fftSize");
    fftOrder = (int) GetIAttribute(asdf, "fftOrder");
    windowSize = (int) GetIAttribute(asdf, "windowSize");
    smthWinSize = (int) GetIAttribute(asdf, "gdSmthWinSize");
    gamma = (float) GetFAttribute(asdf, "gamma");
    alfaP = (float) GetFAttribute(asdf, "gdPosScale");
    alfaN = (float) GetFAttribute(asdf, "gdNegScale");
    mgdNormalize = (int) GetIAttribute(asdf, "mgdNormalize");
    removeLPhase = (int) GetIAttribute(asdf, "gdRemoveLPhase");
    removeMin =  (int) GetIAttribute(asdf, "removeMin");
    samplingRate = (int) GetIAttribute(asdf,"samplingRate");
    gdSign = (int) GetIAttribute(asdf,"gdSign");
    medianOrder = (int) GetIAttribute(asdf,"medianOrder");
    minFrequency = (float) GetFAttribute(asdf,"minFrequency");
    maxFrequency = (float) GetFAttribute(asdf,"maxFrequency");
    startIndex = (int) (minFrequency/samplingRate*fftSize);
    endIndex = (int) (maxFrequency/samplingRate*fftSize);
    waveform = (F_VECTOR *) AllocFVector(windowSize);
    farray = (float *) AllocFloatArray(farray, fftSize+1);
    prevAsdf = asdf;
}
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 0; i < windowSize; i++)
    farray[i+1] = waveform->array[i];
  fvect->array = (float *) ModGdCepstrumNcN(farray, windowSize, fftSize, 
					    fftOrder, numCepstrum, 
					    smthWinSize, fvect->array, 
					    alfaP, alfaN, gamma, gdSign, 
					    removeLPhase, removeMin, 
					    mgdNormalize, medianOrder, 
					    startIndex, endIndex);
 return(fvect);

}
/*-------------------------------------------------------------------------
 *  FrameComputeModGdCepstrumNcNMean -- Computes the Cepstrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeModGdCepstrumNcNMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames, zeroMean;
  static F_VECTOR  *meanVector;
 int              i,j;
 int              numVoicedFrames = 0;
 numCepstrum = GetIAttribute(asdf,"numCepstrum");
 numFrames = GetIAttribute(asdf, "numFrames");
 numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
 zeroMean = GetIAttribute(asdf, "zeroMean");
 meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
 for (i = 0; i < numCepstrum; i++)
   meanVector->array[i] = 0;
 for (i = 0; i < numFrames; i++) 
   if (asdf->vU[i]){
     fvect = (F_VECTOR *) FrameComputeModGdCepstrumNcNRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ )
       meanVector->array[j] = fvect->array[j] + meanVector->array[j];
   }
 for ( i = 0; i < numCepstrum; i++ )
   meanVector->array[i] = meanVector->array[i]/numVoicedFrames;
 return(meanVector);

}	/*  End of FrameComputeModGdCepstrumNcNMean	*/	



/*-------------------------------------------------------------------------
 *  FrameComputeMelModGdCepstrumNcNVariance -- Computes the ModGdCepstrumNcN Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeModGdCepstrumNcNVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeModGdCepstrumNcNMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeModGdCepstrumNcNRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeModGdCepstrumNcNVariance	*/	

/*--------------------------------------------------------------------------
  FrameComputeModGdCepstrumNcN : Computes the modgd cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeModGdCepstrumNcN(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static int       zeroMean, featureVarNormalize;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector, *varVector;
  static char      oldName[500];
  int              i;
  static float     sqrtVal;
  
 if (prevAsdf != asdf) {
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean) || (featureVarNormalize)){
     numCepstrum = GetIAttribute(asdf,"numCepstrum");
     meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
     varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   }
   prevAsdf = asdf;
}
 if (((zeroMean)||(featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
     meanVector = (F_VECTOR *) FrameComputeModGdCepstrumNcNMean(asdf, 0, meanVector);
     varVector = (F_VECTOR *) FrameComputeModGdCepstrumNcNVariance(asdf, 0, varVector);
     oldName[0] = '\0';
     strcpy(oldName, asdf->waveFileName);
 }

 fvect = (F_VECTOR *) FrameComputeModGdCepstrumNcNRaw (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numCepstrum; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numCepstrum; i++) {
     sqrtVal = (float) sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtVal;
   }
   return(fvect);
 }

/*--------------------------------------------------------------------------
  FrameComputeDeltaModGdCepstrumNcN : Computes the modgd deltaCepstrumNcN of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaModGdCepstrumNcN(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
}
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeModGdCepstrumNcN(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeModGdCepstrumNcN(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
   LinearVectorScalarDivide(normalizingConst,fvect,fvect);
  return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaModGdCepstrumNcN : Computes the modgd deltaDeltaCepstrumNcN of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaCepstrumNcN returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaModGdCepstrumNcN(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeDeltaModGdCepstrumNcN(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeDeltaModGdCepstrumNcN(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeAugmentedModGdCepstrumNcN : Computes the augmentedCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeAugmentedModGdCepstrumNcN(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeModGdCepstrumNcN(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeDeltaModGdCepstrumNcN(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeDeltaDeltaModGdCepstrumNcN(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}
  
/*----------------------------------------------------------------------------
 FrameComputeModGdLogSmthCepstrumNcNRaw : Computes the modified group delay cepstrum
                                     for a frame of speech.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of "numCepstrum" cepstral coefficients

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeModGdLogSmthCepstrumNcNRaw(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static int                           numFilters, windowSize;
  static int                           numCepstrum;
  static int                           fftOrder, fftSize, samplingRate;
  static int                           smthWinSize, removeLPhase, gdSign;
  static int                           startIndex, endIndex;
  static float                         minFrequency, maxFrequency;
  static int                           removeMin, medianOrder, mgdNormalize;
  static float                         gamma, alfaP, alfaN;
  static ASDF                          *prevAsdf=NULL;
  static F_VECTOR                      *waveform;
  static float                         *farray; 
  int                                  i;
  if (prevAsdf != asdf) {
    numFilters = (int) GetIAttribute(asdf,"numFilters");
    numCepstrum = (int) GetIAttribute(asdf,"numCepstrum");
    fftSize = (int) GetIAttribute(asdf, "fftSize");
    fftOrder = (int) GetIAttribute(asdf, "fftOrder");
    windowSize = (int) GetIAttribute(asdf, "windowSize");
    smthWinSize = (int) GetIAttribute(asdf, "gdSmthWinSize");
    gamma = (float) GetFAttribute(asdf, "gamma");
    alfaP = (float) GetFAttribute(asdf, "gdPosScale");
    alfaN = (float) GetFAttribute(asdf, "gdNegScale");
    mgdNormalize = (int) GetIAttribute(asdf, "mgdNormalize");
    removeLPhase = (int) GetIAttribute(asdf, "gdRemoveLPhase");
    removeMin =  (int) GetIAttribute(asdf, "removeMin");
    samplingRate = (int) GetIAttribute(asdf,"samplingRate");
    gdSign = (int) GetIAttribute(asdf,"gdSign");
    medianOrder = (int) GetIAttribute(asdf,"medianOrder");
    minFrequency = (float) GetFAttribute(asdf,"minFrequency");
    maxFrequency = (float) GetFAttribute(asdf,"maxFrequency");
    startIndex = (int) (minFrequency/samplingRate*fftSize);
    endIndex = (int) (maxFrequency/samplingRate*fftSize);
    waveform = (F_VECTOR *) AllocFVector(windowSize);
    farray = (float *) AllocFloatArray(farray, fftSize+1);
    prevAsdf = asdf;
}
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 0; i < windowSize; i++)
    farray[i+1] = waveform->array[i];
  fvect->array = (float *) ModGdLogSmthCepstrumNcN(farray, windowSize, fftSize, 
					    fftOrder, numCepstrum, 
					    smthWinSize, fvect->array, 
					    alfaP, alfaN, gamma, gdSign, 
					    removeLPhase, removeMin, 
					    mgdNormalize, medianOrder, 
					    startIndex, endIndex);
 return(fvect);

}
/*-------------------------------------------------------------------------
 *  FrameComputeModGdLogSmthCepstrumNcNMean -- Computes the Cepstrum mean of ModGd
 *                                             CepstrumNcN LogSmth for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeModGdLogSmthCepstrumNcNMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames, zeroMean;
  static F_VECTOR  *meanVector=NULL;
  int              i,j;
  int              numVoicedFrames = 0;

 numCepstrum = GetIAttribute(asdf,"numCepstrum");
 numFrames = GetIAttribute(asdf, "numFrames");
 numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
 zeroMean = GetIAttribute(asdf, "zeroMean");
 meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
 for (i = 0; i < numCepstrum; i++)
   meanVector->array[i] = 0;
 for (i = 0; i < numFrames; i++) 
   if (asdf->vU[i]){
     fvect = (F_VECTOR *) FrameComputeModGdLogSmthCepstrumNcNRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ )
       meanVector->array[j] = fvect->array[j] + meanVector->array[j];
   }
 for ( i = 0; i < numCepstrum; i++ )
   meanVector->array[i] = meanVector->array[i]/numVoicedFrames;
 return(meanVector);

}	/*  End of FrameComputeModGdLogSmthCepstrumNcNMean	*/	


/*-------------------------------------------------------------------------
 *  FrameComputeModGdLogSmthCepstrumNcNVariance -- Computes the 
 *                                              -- ModGdLogSmthCepstrumNcN Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeModGdLogSmthCepstrumNcNVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeModGdLogSmthCepstrumNcNMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeModGdLogSmthCepstrumNcNRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeModGdLogSmthCepstrumNcNVariance	*/	


/*--------------------------------------------------------------------------
  FrameComputeModGdLogSmthCepstrumNcN : Computes the modgd cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeModGdLogSmthCepstrumNcN(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static int       zeroMean, featureVarNormalize;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector, *varVector=NULL;
  static char      oldName[500];

  int              i;
  static float     sqrtValue;
 if (prevAsdf != asdf) {
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean) || (featureVarNormalize)) {
     numCepstrum = GetIAttribute(asdf,"numCepstrum");
     meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
     varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   }
   prevAsdf = asdf;
}
 if (((zeroMean) ||(featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
     meanVector = (F_VECTOR *) FrameComputeModGdLogSmthCepstrumNcNMean(asdf, 0, meanVector);
     varVector = (F_VECTOR *) FrameComputeModGdLogSmthCepstrumNcNVariance(asdf, 0, varVector);
     oldName[0] = '\0';
     strcpy(oldName, asdf->waveFileName);
 }

 fvect = (F_VECTOR *) FrameComputeModGdLogSmthCepstrumNcNRaw (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numCepstrum; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numCepstrum; i++) {
     sqrtValue = (float) sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtValue;
   }
   return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeDeltaModGdLogSmthCepstrumNcN : Computes the modgd deltaCepstrumNcN of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaModGdLogSmthCepstrumNcN(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
}
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeModGdLogSmthCepstrumNcN(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeModGdLogSmthCepstrumNcN(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
   LinearVectorScalarDivide(normalizingConst,fvect,fvect);
  return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaModGdCepstrumNcN : Computes the modgd deltaDeltaCepstrumNcN of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaCepstrumNcN returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaModGdLogSmthCepstrumNcN(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeDeltaModGdLogSmthCepstrumNcN(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeDeltaModGdLogSmthCepstrumNcN(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeAugmentedModGdLogSmthCepstrumNcN : Computes the augmentedCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeAugmentedModGdLogSmthCepstrumNcN(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeModGdLogSmthCepstrumNcN(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeDeltaModGdLogSmthCepstrumNcN(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeDeltaDeltaModGdLogSmthCepstrumNcN(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}
  
/*----------------------------------------------------------------------------
 FrameComputeModGdCepstrumDCTRaw : Computes the modified group delay cepstrum
                                     for a frame of speech.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of "numCepstrum" cepstral coefficients

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeModGdCepstrumDCTRaw(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static int                           numFilters, windowSize;
  static int                           numCepstrum;
  static int                           fftOrder, fftSize, samplingRate;
  static int                           smthWinSize, removeLPhase, gdSign;
  static int                           startIndex, endIndex;
  static float                         minFrequency, maxFrequency;
  static float                         gamma, alfaP, alfaN;
  static ASDF                          *prevAsdf=NULL;
  static F_VECTOR                      *waveform;
  static float                         *farray; 
  int                                  i;
  if (prevAsdf != asdf) {
    numFilters = (int) GetIAttribute(asdf,"numFilters");
    numCepstrum = (int) GetIAttribute(asdf,"numCepstrum");
    fftSize = (int) GetIAttribute(asdf, "fftSize");
    fftOrder = (int) GetIAttribute(asdf, "fftOrder");
    windowSize = (int) GetIAttribute(asdf, "windowSize");
    smthWinSize = (int) GetIAttribute(asdf, "gdSmthWinSize");
    gamma = (float) GetFAttribute(asdf, "gamma");
    alfaP = (float) GetFAttribute(asdf, "gdPosScale");
    alfaN = (float) GetFAttribute(asdf, "gdNegScale");
    removeLPhase = (int) GetIAttribute(asdf, "gdRemoveLPhase");
    samplingRate = (int) GetIAttribute(asdf,"samplingRate");
    gdSign = (int) GetIAttribute(asdf,"gdSign");
    minFrequency = (float) GetFAttribute(asdf,"minFrequency");
    maxFrequency = (float) GetFAttribute(asdf,"maxFrequency");
    startIndex = (int) (minFrequency/samplingRate*fftSize);
    endIndex = (int) (maxFrequency/samplingRate*fftSize);
    waveform = (F_VECTOR *) AllocFVector(windowSize);
    farray = (float *) AllocFloatArray(farray, fftSize+1);
    prevAsdf = asdf;
}
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 0; i < windowSize; i++)
    farray[i+1] = waveform->array[i];
  fvect->array = (float *) ModGdCepstrum_DCT(farray, windowSize, fftSize, 
					    fftOrder, numCepstrum, 
					    smthWinSize, fvect->array, 
					    alfaP, alfaN, gamma, gdSign, 
					    removeLPhase,  
					     startIndex, endIndex, 1.0);
 return(fvect);

}
/*-------------------------------------------------------------------------
 *  FrameComputeModGdCepstrumDCTMean -- Computes the Cepstrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeModGdCepstrumDCTMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames, zeroMean;
  static F_VECTOR  *meanVector=NULL;
 int              i,j;
 int              numVoicedFrames = 0;
 numCepstrum = GetIAttribute(asdf,"numCepstrum");
 numFrames = GetIAttribute(asdf, "numFrames");
 numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
 zeroMean = GetIAttribute(asdf, "zeroMean");
 meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
 for (i = 0; i < numCepstrum; i++)
   meanVector->array[i] = 0;
 for (i = 0; i < numFrames; i++) 
   if (asdf->vU[i]){
     fvect = (F_VECTOR *) FrameComputeModGdCepstrumDCTRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ )
       meanVector->array[j] = fvect->array[j] + meanVector->array[j];
   }
 for ( i = 0; i < numCepstrum; i++ )
   meanVector->array[i] = meanVector->array[i]/numVoicedFrames;
 return(meanVector);

}	/*  End of FrameComputeModGdCepstrumDCTMean	*/	



/*-------------------------------------------------------------------------
 *  FrameComputeModGdCepstrumDCTVariance -- Computes the 
 *                                              -- ModGdCepstrumDCT Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeModGdCepstrumDCTVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeModGdCepstrumDCTMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeModGdCepstrumDCTRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeModGdCepstrumDCTVariance	*/	




/*--------------------------------------------------------------------------
  FrameComputeModGdCepstrumDCT : Computes the modgd cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeModGdCepstrumDCT(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static int       zeroMean, featureVarNormalize;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector, *varVector;
  static char      oldName[500];
  static float     sqrtVal;
  int              i;

 if (prevAsdf != asdf) {
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean)||(featureVarNormalize)) {
     numCepstrum = GetIAttribute(asdf,"numCepstrum");
     meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
     varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   }
   prevAsdf = asdf;
}
 if (((zeroMean)||(featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
     meanVector = (F_VECTOR *) FrameComputeModGdCepstrumDCTMean(asdf, 0, meanVector);
     varVector = (F_VECTOR *) FrameComputeModGdCepstrumDCTVariance(asdf, 0, varVector);
     oldName[0] = '\0';
     strcpy(oldName, asdf->waveFileName);
 }

 fvect = (F_VECTOR *) FrameComputeModGdCepstrumDCTRaw (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numCepstrum; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numCepstrum; i++) {
     sqrtVal = (float) sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtVal;
   }
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeDeltaModGdCepstrumDCT : Computes the modgd deltaCepstrumNcN of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaModGdCepstrumDCT(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
}
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeModGdCepstrumDCT(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeModGdCepstrumDCT(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaModGdCepstrumDCT : Computes the modgd deltaDeltaCepstrumNcN of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaCepstrumNcN returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaModGdCepstrumDCT(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeDeltaModGdCepstrumDCT(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeDeltaModGdCepstrumDCT(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeAugmentedModGdCepstrumDCT : Computes the augmentedCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeAugmentedModGdCepstrumDCT(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeModGdCepstrumDCT(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeDeltaModGdCepstrumDCT(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeDeltaDeltaModGdCepstrumDCT(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}
/*----------------------------------------------------------------------------
 FrameComputeModGdCepstrumLPDCTRaw : Computes the modified group delay cepstrum
                                     for a frame of speech.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of "numCepstrum" cepstral coefficients

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeModGdCepstrumLPDCTRaw(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static int                           numFilters, windowSize;
  static int                           numCepstrum;
  static int                           fftOrder, fftSize, samplingRate;
  static int                           lpOrder, removeLPhase, gdSign;
  static int                           startIndex, endIndex;
  static float                         minFrequency, maxFrequency;
  static float                         gamma, alfaP, alfaN;
  static ASDF                          *prevAsdf=NULL;
  static F_VECTOR                      *waveform;
  static float                         *farray; 
  int                                  i;
  if (prevAsdf != asdf) {
    numFilters = (int) GetIAttribute(asdf,"numFilters");
    numCepstrum = (int) GetIAttribute(asdf,"numCepstrum");
    fftSize = (int) GetIAttribute(asdf, "fftSize");
    fftOrder = (int) GetIAttribute(asdf, "fftOrder");
    windowSize = (int) GetIAttribute(asdf, "windowSize");
    lpOrder = (int) GetIAttribute(asdf, "lpOrder");
    gamma = (float) GetFAttribute(asdf, "gamma");
    alfaP = (float) GetFAttribute(asdf, "gdPosScale");
    alfaN = (float) GetFAttribute(asdf, "gdNegScale");
    removeLPhase = (int) GetIAttribute(asdf, "gdRemoveLPhase");
    samplingRate = (int) GetIAttribute(asdf,"samplingRate");
    gdSign = (int) GetIAttribute(asdf,"gdSign");
    minFrequency = (float) GetFAttribute(asdf,"minFrequency");
    maxFrequency = (float) GetFAttribute(asdf,"maxFrequency");
    startIndex = (int) (minFrequency/samplingRate*fftSize);
    endIndex = (int) (maxFrequency/samplingRate*fftSize);
    waveform = (F_VECTOR *) AllocFVector(windowSize);
    farray = (float *) AllocFloatArray(farray, fftSize+1);
    prevAsdf = asdf;
}
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 0; i < windowSize; i++)
    farray[i+1] = waveform->array[i];
  fvect->array = (float *) ModGdCepstrum_LPDCT(farray, windowSize, fftSize, 
					    fftOrder, numCepstrum, 
					    lpOrder, fvect->array, 
					    alfaP, alfaN, gamma, gdSign, 
					    removeLPhase,  
					     startIndex, endIndex, 1.0);
 return(fvect);

}
/*-------------------------------------------------------------------------
 *  FrameComputeModGdCepstrumLPDCTMean -- Computes the Cepstrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeModGdCepstrumLPDCTMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames, zeroMean;
  static F_VECTOR  *meanVector=NULL;
 int              i,j;
 int              numVoicedFrames = 0;
 numCepstrum = GetIAttribute(asdf,"numCepstrum");
 numFrames = GetIAttribute(asdf, "numFrames");
 numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
 zeroMean = GetIAttribute(asdf, "zeroMean");
 meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
 for (i = 0; i < numCepstrum; i++)
   meanVector->array[i] = 0;
 for (i = 0; i < numFrames; i++) 
   if (asdf->vU[i]){
     fvect = (F_VECTOR *) FrameComputeModGdCepstrumLPDCTRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ )
       meanVector->array[j] = fvect->array[j] + meanVector->array[j];
   }
 for ( i = 0; i < numCepstrum; i++ )
   meanVector->array[i] = meanVector->array[i]/numVoicedFrames;
 return(meanVector);

}	/*  End of FrameComputeModGdCepstrumLPDCTMean	*/	



/*-------------------------------------------------------------------------
 *  FrameComputeModGdCepstrumLPDCTVariance -- Computes the 
 *                                              -- ModGdCepstrumDCT Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeModGdCepstrumLPDCTVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeModGdCepstrumDCTMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeModGdCepstrumLPDCTRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeModGdCepstrumLPDCTVariance	*/	




/*--------------------------------------------------------------------------
  FrameComputeModGdCepstrumDCT : Computes the modgd cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeModGdCepstrumLPDCT(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static int       zeroMean, featureVarNormalize;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector, *varVector;
  static char      oldName[500];
  static float     sqrtVal;
  int              i;

 if (prevAsdf != asdf) {
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean)||(featureVarNormalize)) {
     numCepstrum = GetIAttribute(asdf,"numCepstrum");
     meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
     varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   }
   prevAsdf = asdf;
}
 if (((zeroMean)||(featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
     meanVector = (F_VECTOR *) FrameComputeModGdCepstrumLPDCTMean(asdf, 0, meanVector);
     varVector = (F_VECTOR *) FrameComputeModGdCepstrumLPDCTVariance(asdf, 0, varVector);
     oldName[0] = '\0';
     strcpy(oldName, asdf->waveFileName);
 }

 fvect = (F_VECTOR *) FrameComputeModGdCepstrumLPDCTRaw (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numCepstrum; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numCepstrum; i++) {
     sqrtVal = (float) sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtVal;
   }
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeDeltaModGdCepstrumLPDCT : Computes the modgd deltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaModGdCepstrumLPDCT(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
}
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeModGdCepstrumLPDCT(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeModGdCepstrumLPDCT(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaModGdCepstrumLPDCT : Computes the modgd deltaDeltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaCepstrumLPDCT returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaModGdCepstrumLPDCT(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeDeltaModGdCepstrumLPDCT(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeDeltaModGdCepstrumLPDCT(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeAugmentedModGdCepstrumLPDCT : Computes the augmentedCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeAugmentedModGdCepstrumLPDCT(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeModGdCepstrumLPDCT(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeDeltaModGdCepstrumLPDCT(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeDeltaDeltaModGdCepstrumLPDCT(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}
/*----------------------------------------------------------------------------
 FrameComputeProductGDCepstrumNcNRaw : Computes the modified group delay cepstrum
                                     for a frame of speech.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of "numCepstrum" cepstral coefficients

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeProductGdCepstrumNcNRaw(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static int                           numFilters, windowSize;
  static int                           numCepstrum;
  static int                           fftOrder, fftSize, samplingRate;
  static int                           smthWinSize, removeLPhase, gdSign;
  static int                           startIndex, endIndex;
  static float                         minFrequency, maxFrequency;
  static int                           removeMin, mgdNormalize;
  static float                         gamma, alfaP, alfaN;
  static ASDF                          *prevAsdf=NULL;
  static F_VECTOR                      *waveform;
  static float                         *farray; 
  int                                  i;
  if (prevAsdf != asdf) {
    numFilters = (int) GetIAttribute(asdf,"numFilters");
    numCepstrum = (int) GetIAttribute(asdf,"numCepstrum");
    fftSize = (int) GetIAttribute(asdf, "fftSize");
    fftOrder = (int) GetIAttribute(asdf, "fftOrder");
    windowSize = (int) GetIAttribute(asdf, "windowSize");
    smthWinSize = (int) GetIAttribute(asdf, "gdSmthWinSize");
    gamma = (float) GetFAttribute(asdf, "gamma");
    alfaP = (float) GetFAttribute(asdf, "gdPosScale");
    alfaN = (float) GetFAttribute(asdf, "gdNegScale");
    mgdNormalize = (int) GetIAttribute(asdf, "mgdNormalize");
    removeLPhase = (int) GetIAttribute(asdf, "gdRemoveLPhase");
    removeMin =  (int) GetIAttribute(asdf, "removeMin");
    samplingRate = (int) GetIAttribute(asdf,"samplingRate");
    gdSign = (int) GetIAttribute(asdf,"gdSign");
    minFrequency = (float) GetFAttribute(asdf,"minFrequency");
    maxFrequency = (float) GetFAttribute(asdf,"maxFrequency");
    startIndex = (int) (minFrequency/samplingRate*fftSize);
    endIndex = (int) (maxFrequency/samplingRate*fftSize);
    waveform = (F_VECTOR *) AllocFVector(windowSize);
    farray = (float *) AllocFloatArray(farray, fftSize+1);
    prevAsdf = asdf;
}
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 0; i < windowSize; i++)
    farray[i+1] = waveform->array[i];
  fvect->array = (float *) ProductGdCepstrumNcN(farray, windowSize, fftSize, 
					    fftOrder, numCepstrum, 
					    smthWinSize, fvect->array, 
					    alfaP, alfaN, gamma, gdSign, 
					    removeLPhase, removeMin, mgdNormalize, startIndex, endIndex);
 return(fvect);

}
/*-------------------------------------------------------------------------
 *  FrameComputeProductGdCepstrumNcNMean -- Computes the Cepstrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeProductGdCepstrumNcNMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames, zeroMean;
  static F_VECTOR  *meanVector=NULL;
  int              i,j;
 int              numVoicedFrames = 0;
 numCepstrum = GetIAttribute(asdf,"numCepstrum");
 numFrames = GetIAttribute(asdf, "numFrames");
 numVoicedFrames = GetIAttribute(asdf, "numVoicedFrames");
 zeroMean = GetIAttribute(asdf, "zeroMean");
 meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
 for (i = 0; i < numCepstrum; i++)
   meanVector->array[i] = 0;
 for (i = 0; i < numFrames; i++) 
   if (asdf->vU[i]){
     fvect = (F_VECTOR *) FrameComputeProductGdCepstrumNcNRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ )
       meanVector->array[j] = fvect->array[j] + meanVector->array[j];
   }
 for ( i = 0; i < numCepstrum; i++ )
   meanVector->array[i] = meanVector->array[i]/numVoicedFrames;
 return(meanVector);

}	/*  End of FrameComputeProductGdCepstrumNcNMean	*/	




/*-------------------------------------------------------------------------
 *  FrameComputeProductGdCepstrumDCTVariance -- Computes the 
 *                                              -- ProdGdCepstrumNcN Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeProductGdCepstrumNcNVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeProductGdCepstrumNcNMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeProductGdCepstrumNcNRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeProductGdCepstrumNcNVariance	*/	



/*--------------------------------------------------------------------------
  FrameComputeProductGdCepstrumNcN : Computes the modgd cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeProductGdCepstrumNcN(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static int       zeroMean, featureVarNormalize;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector, *varVector;
  static char      oldName[500];
  static float     sqrtVal;
  int              i;

 if (prevAsdf != asdf) {
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean) ||(featureVarNormalize)) {
     numCepstrum = GetIAttribute(asdf,"numCepstrum");
     meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
     varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   }
   prevAsdf = asdf;
}
 if (((zeroMean)||(featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
     meanVector = (F_VECTOR *) FrameComputeProductGdCepstrumNcNMean(asdf, 0, meanVector);
     varVector = (F_VECTOR *) FrameComputeProductGdCepstrumNcNVariance(asdf, 0, varVector);
     oldName[0] = '\0';
     strcpy(oldName, asdf->waveFileName);
 }

 fvect = (F_VECTOR *) FrameComputeProductGdCepstrumNcNRaw (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numCepstrum; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numCepstrum; i++) {
     sqrtVal = (float) sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtVal;
   }
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeDeltaProductGdCepstrumNcN : Computes the modgd deltaCepstrumNcN of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaProductGdCepstrumNcN(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
}
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeProductGdCepstrumNcN(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeProductGdCepstrumNcN(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
   LinearVectorScalarDivide(normalizingConst,fvect,fvect);
  return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaProductGdCepstrumNcN : Computes the modgd deltaDeltaCepstrumNcN of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaCepstrumNcN returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaProductGdCepstrumNcN(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeDeltaProductGdCepstrumNcN(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeDeltaProductGdCepstrumNcN(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeAugmentedProductGdCepstrumNcN : Computes the augmentedCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeAugmentedProductGdCepstrumNcN(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeProductGdCepstrumNcN(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeDeltaProductGdCepstrumNcN(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeDeltaDeltaProductGdCepstrumNcN(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}

/*----------------------------------------------------------------------------
 FrameComputeMinGdCepstrumRaw : Computes the minimum phase group delay cepstrum
                                     for a frame of speech.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of "numCepstrum" cepstral coefficients

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMinGdCepstrumRaw(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
  static int                           windowSize;
  static int                           numCepstrum, gdSmthWinSize;
  static int                           fftOrder, fftSize, samplingRate;
  static int                           startIndex, endIndex;
  static float                         minFrequency, maxFrequency;
  static float                         gamma;
  static ASDF                          *prevAsdf=NULL;
  static F_VECTOR                      *waveform;
  static float                         *farray; 
  int                                  i;
  if (prevAsdf != asdf) {
    numCepstrum = (int) GetIAttribute(asdf,"numCepstrum");
    fftSize = (int) GetIAttribute(asdf, "fftSize");
    fftOrder = (int) GetIAttribute(asdf, "fftOrder");
    windowSize = (int) GetIAttribute(asdf, "windowSize");
    gamma = (float) GetFAttribute(asdf, "gamma");
    gdSmthWinSize = (int) GetIAttribute (asdf, "gdSmthWinSize");
    samplingRate = (int) GetIAttribute(asdf,"samplingRate");
    minFrequency = (float) GetFAttribute(asdf,"minFrequency");
    maxFrequency = (float) GetFAttribute(asdf,"maxFrequency");
    startIndex = (int) (minFrequency/samplingRate*fftSize);
    endIndex = (int) (maxFrequency/samplingRate*fftSize);
    waveform = (F_VECTOR *) AllocFVector(windowSize);
    farray = (float *) AllocFloatArray(farray, fftSize+1);
    prevAsdf = asdf;
}
  waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
  for (i = 0; i < windowSize; i++)
    farray[i+1] = waveform->array[i];
  fvect->array = (float *) MinGdCepstrum_DCT(farray, windowSize, fftSize, 
					     fftOrder, gdSmthWinSize, numCepstrum, fvect->array, gamma, startIndex, endIndex);

 return(fvect);

}
/*-------------------------------------------------------------------------
 *  FrameComputeMinGdCepstrumMean -- Computes the Cepstrum mean for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - mean of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeMinGdCepstrumMean(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames, numVoicedFrames, zeroMean;
  static F_VECTOR  *meanVector=NULL;
  int              i,j;
  numCepstrum = GetIAttribute(asdf,"numCepstrum");
  numFrames = GetIAttribute(asdf, "numFrames");
  numVoicedFrames = GetIAttribute(asdf,"numVoicedFrames");
  zeroMean = GetIAttribute(asdf, "zeroMean");
  meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
  for (i = 0; i < numCepstrum; i++)
    meanVector->array[i] = 0;
  for (i = 0; i < numFrames; i++) 
    if (asdf->vU[i]) {
      fvect = (F_VECTOR *) FrameComputeMinGdCepstrumRaw(asdf,i, fvect);
      for ( j = 0; j < numCepstrum; j++ )
	meanVector->array[j] = fvect->array[j] + meanVector->array[j];
    }
  for ( i = 0; i < numCepstrum; i++ )
    fvect->array[i] = meanVector->array[i]/numVoicedFrames;
  return(fvect);
  
}	/*  End of FrameComputeMinGdCepstrumMean	*/	



/*-------------------------------------------------------------------------
 *  FrameComputeMinGdCepstrumVariance -- Computes the 
 *                                              -- MinGdCepstrum Variance for a waveform
 *    Args: 
 *    asdf                 : front-end-parameters
 *    frameIndex           : not used
 *    Returns:	
 *    fvect        : returned as an F_VECTOR - variance of cepstra
 *    Bugs:	
 * -------------------------------------------------------------------------*/

F_VECTOR * FrameComputeMinGdCepstrumVariance(ASDF *asdf, int frameIndex, 
				    F_VECTOR *fvect) 
{
  static int      numCepstrum, numFrames;
  static F_VECTOR  *meanVector=NULL, *varVector=NULL, *diffVector=NULL;
  int              i,j;
  
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   numFrames = GetIAttribute(asdf, "numVoicedFrames");
   meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
   meanVector = (F_VECTOR *) FrameComputeMinGdCepstrumMean(asdf, 0, meanVector);
   varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   diffVector = (F_VECTOR *) AllocFVector(numCepstrum);
   for (i = 0; i < numCepstrum; i++)
     varVector->array[i] = 0;
   for (i = 0; i < numFrames; i++) {
     fvect = (F_VECTOR *) FrameComputeMinGdCepstrumRaw(asdf,i, fvect);
     for ( j = 0; j < numCepstrum; j++ ) {
       diffVector->array[j]= (fvect->array[j] - meanVector->array[j]);
       varVector->array[j] = diffVector->array[j]*diffVector->array[j] + varVector->array[j];
     }
   }
   for ( i = 0; i < numCepstrum; i++ ) {
     fvect->array[i] = varVector->array[i]/numFrames;
     if (fvect->array[i] == 0.0) {
       printf("Flooring variance to 1.0E-12 of index = %d\n", i);
       fvect->array[i] = 1.0E-12;
     }
   }
   return(fvect);
}	/*  End of FrameComputeMinGdCepstrumVariance	*/	




/*--------------------------------------------------------------------------
  FrameComputeMinGdCepstrum : Computes the mingd cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMinGdCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numCepstrum;
  static int       zeroMean, featureVarNormalize;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *meanVector, *varVector;
  static char      oldName[500];
  int              i;
  static float     sqrtVal;
 if (prevAsdf != asdf) {
   zeroMean = (int) GetIAttribute(asdf, "zeroMean");
   featureVarNormalize = (int) GetIAttribute(asdf, "featureVarNormalize");
   if ((zeroMean)||(featureVarNormalize)) {
     numCepstrum = GetIAttribute(asdf,"numCepstrum");
     meanVector = (F_VECTOR *) AllocFVector(numCepstrum);
     varVector = (F_VECTOR *) AllocFVector(numCepstrum);
   }
   prevAsdf = asdf;
}
 if (((zeroMean) || (featureVarNormalize)) && (strcmp(oldName, asdf->waveFileName) != 0)) {
     meanVector = (F_VECTOR *) FrameComputeMinGdCepstrumMean(asdf, 0, meanVector);
     varVector = (F_VECTOR *) FrameComputeMinGdCepstrumVariance(asdf, 0, varVector);
   oldName[0] = '\0';
   strcpy(oldName, asdf->waveFileName);
 }
 fvect = (F_VECTOR *) FrameComputeMinGdCepstrumRaw (asdf, frameIndex, fvect);
 if (zeroMean) {
   for (i = 0; i < numCepstrum; i++)
     fvect->array[i] = fvect->array[i] - meanVector->array[i];
 }
 if (featureVarNormalize) 
   for (i = 0; i < numCepstrum; i++) {
     sqrtVal = (float) sqrtf(varVector->array[i]);
     fvect->array[i] = fvect->array[i]/sqrtVal;
   }

 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeDeltaMinGdCepstrum : Computes the modgd deltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaCepstrum        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaMinGdCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
}
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeMinGdCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeMinGdCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeDeltaDeltaMinGdCepstrum : Computes the modgd deltaDeltaCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaCepstrum returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeDeltaDeltaMinGdCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum,
                  deltaDeltaDifference,numFrames;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
 static F_VECTOR  *prev,*next,*temp;
 int i;
 if (prevAsdf != asdf) {
   numCepstrum = GetIAttribute(asdf,"numCepstrum");
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   prev = (F_VECTOR *) AllocFVector(numCepstrum);
   next = (F_VECTOR *) AllocFVector(numCepstrum);
   temp = (F_VECTOR *) AllocFVector(numCepstrum);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numCepstrum; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeDeltaMinGdCepstrum(asdf,frameIndex-i,prev);
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeDeltaMinGdCepstrum(asdf,frameIndex+i,next);
   else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeAugmentedMinGdCepstrum : Computes the augmentedCepstrum of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeAugmentedMinGdCepstrum(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numCepstrum;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numCepstrum = GetIAttribute(asdf,"numCepstrum");
    temp = (F_VECTOR *) AllocFVector(numCepstrum);
    tempEnergy = (F_VECTOR *) AllocFVector(1);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeMinGdCepstrum(asdf,frameIndex, temp);
  for (i = 0; i < numCepstrum; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeDeltaMinGdCepstrum(asdf,frameIndex, temp);
  for (i = numCepstrum; i < 2*numCepstrum; i++)
    fvect->array[i] = temp->array[i-numCepstrum];
  temp = (F_VECTOR *) FrameComputeDeltaDeltaMinGdCepstrum(asdf,frameIndex, temp);
  for (i = 2*numCepstrum; i < 3*numCepstrum; i++)
    fvect->array[i] = temp->array[i-2*numCepstrum];
  fvect->array[3*numCepstrum] = ((F_VECTOR *) FrameComputeEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+1] = ((F_VECTOR *) FrameComputeDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numCepstrum+2] = ((F_VECTOR *) FrameComputeDeltaDeltaEnergy(asdf, frameIndex, tempEnergy))->array[0];
  return(fvect);
}

/*--------------------------------------------------------------------------
  FrameComputeMelSlope : Computes the cepstrum of a given frame 
  inputs :
     asdf                     : front-end-parameters
     frameIndex               : frameNumber 
     
  outputs :
     cepstrum                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelSlope(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int       numSlope;
  static ASDF      *prevAsdf=NULL;
  static F_VECTOR  *tempFVector=NULL;
  static int       numRegressCoeffts;
  float            intercept;
  int              i;

 if (prevAsdf != asdf) {
   numSlope = (int) (GetIAttribute(asdf,"numFilters") - 1);
   tempFVector = (F_VECTOR *) AllocFVector(numSlope+1);
   numRegressCoeffts = (int) (GetIAttribute(asdf, "numRegressCoeffts"));
   prevAsdf = asdf;
 }

tempFVector = (F_VECTOR *) FrameComputeFilterbankLogEnergy (asdf, frameIndex, 
							    tempFVector);
 for (i = 0; i < numSlope; i++) 
   LinReg (tempFVector, i, numRegressCoeffts, &intercept, &fvect->array[i]); 
return(fvect);
 }

/*--------------------------------------------------------------------------
  FrameComputeMelDeltaSlope : Computes the deltaSlope of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     deltaSlope        : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelDeltaSlope(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numSlope,
                  deltaDifference,numFrames;
  static int       numRegressCoeffts;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *prev,*next,*temp, *tempFVector;
  int              i;

 if (prevAsdf != asdf) {
   numSlope = (int) GetIAttribute(asdf,"numFilters") - 1 ;
   deltaDifference = GetIAttribute(asdf,"deltaDifference");
   numRegressCoeffts = GetIAttribute(asdf,"numRegressCoeffts");
   numFrames = GetIAttribute(asdf,"numFrames");
   prev = (F_VECTOR *) AllocFVector(numSlope);
   next = (F_VECTOR *) AllocFVector(numSlope);
   tempFVector = (F_VECTOR *) AllocFVector(numSlope);
   temp = (F_VECTOR *) AllocFVector(numSlope);
   prevAsdf = asdf;
   for (i = -deltaDifference; i <= deltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
}
 for (i = 0; i < numSlope; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i]))
     prev = (F_VECTOR *) FrameComputeMelSlope(asdf, frameIndex-i, prev);
     /*     for (i = 0; i < numSlope; i++) 
	    LinReg (tempFVector, i, numRegressCoeffts, &intercept, &prev->array[i]) ;*/ 
   else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i]))
     next = (F_VECTOR *) FrameComputeMelSlope(asdf, frameIndex+i, next);
     /*     for (i = 0; i < numSlope; i++) 
	    LinReg (tempFVector, i, numRegressCoeffts, &intercept, &next->array[i]) ;*/ 
   else
     InitFVector(next);
   LinearVectorDifference(prev, next, temp);
   LinearVectorScalarMultiply((float)(i), temp, temp);
   LinearVectorAddition(temp, fvect, fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeMelDeltaDeltaSlope : Computes the deltaDeltaSlope of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : deltaDeltaSlope returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelDeltaDeltaSlope(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numSlope,
                  deltaDeltaDifference,numFrames;
  static int       numRegressCoeffts;
  static float    normalizingConst =0;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *prev,*next,*temp;
  int              i;
  float            intercept;

 if (prevAsdf != asdf) {
   numSlope = (int) GetIAttribute(asdf,"numFilters") - 1;
   deltaDeltaDifference = GetIAttribute(asdf,"deltaDeltaDifference");
   numFrames = GetIAttribute(asdf,"numFrames");  
   numRegressCoeffts = GetIAttribute(asdf,"numRegressCoeffts");
   prev = (F_VECTOR *) AllocFVector(numSlope);
   next = (F_VECTOR *) AllocFVector(numSlope);
   temp = (F_VECTOR *) AllocFVector(numSlope+1);
   prevAsdf = asdf;
   for (i = -deltaDeltaDifference; i <= deltaDeltaDifference; i++)
     normalizingConst = normalizingConst+i*i;
 }
 for (i = 0; i < numSlope; i++)
   fvect->array[i] = 0;
 for (i = 1; i <= deltaDeltaDifference; i++){
   if (((frameIndex-i) >= 0)&& (asdf->vU[frameIndex-i])){
     temp = (F_VECTOR *) FrameComputeDeltaDeltaFilterbankLogEnergy(asdf,frameIndex-i,temp);
     for (i = 0; i < numSlope; i++) 
       LinReg (temp, i, numRegressCoeffts, &intercept, &prev->array[i]); 
   } else
     InitFVector(prev);
   if (((frameIndex+i) <=  numFrames)&& (asdf->vU[frameIndex+i])){
     temp = (F_VECTOR *) FrameComputeDeltaDeltaFilterbankLogEnergy(asdf,frameIndex+i,temp);
     for (i = 0; i < numSlope; i++) 
       LinReg (temp, i, numRegressCoeffts, &intercept, &next->array[i]); 
   } else
     InitFVector(next);
   LinearVectorDifference(prev,next,temp);
   LinearVectorScalarMultiply((float)(i),temp,temp);
   LinearVectorAddition(temp,fvect,fvect);
 }
 LinearVectorScalarDivide(normalizingConst,fvect,fvect);
 return(fvect);
}
/*--------------------------------------------------------------------------
  FrameComputeMelAugmentedSlope : Computes the augmentedSlope of a given frame of speech
  inputs :
     asdf                  : front-end-parameters
     frameIndex           : frameNumber 
     
  outputs :
     fvect                 : returned as an F_VECTOR

---------------------------------------------------------------------------*/

F_VECTOR *FrameComputeMelAugmentedSlope(ASDF *asdf, int frameIndex, F_VECTOR *fvect) 
{
  static int      numSlope;
  static ASDF     *prevAsdf=NULL;
  static F_VECTOR  *temp, *tempEnergy;
  int i;
  if (prevAsdf != asdf) {
    numSlope = (int) GetIAttribute(asdf, "numFilters") - 1;
    temp = (F_VECTOR *) AllocFVector(numSlope);
    prevAsdf = asdf;
  }
  temp = (F_VECTOR *) FrameComputeMelSlope(asdf,frameIndex, temp);
  for (i = 0; i < numSlope; i++)
    fvect->array[i] = temp->array[i];
  temp = (F_VECTOR *) FrameComputeMelDeltaSlope(asdf,frameIndex, temp);
  for (i = numSlope; i < 2*numSlope; i++)
    fvect->array[i] = temp->array[i-numSlope];
  temp = (F_VECTOR *) FrameComputeMelDeltaDeltaSlope(asdf,frameIndex, temp);
  for (i = 2*numSlope; i < 3*numSlope; i++)
    fvect->array[i] = temp->array[i-2*numSlope];
  fvect->array[3*numSlope] = ((F_VECTOR *) FrameComputeLogEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numSlope+1] = ((F_VECTOR *) FrameComputeDeltaLogEnergy(asdf, frameIndex, tempEnergy))->array[0];
  fvect->array[3*numSlope+2] = ((F_VECTOR *) FrameComputeDeltaDeltaLogEnergy(asdf, frameIndex, tempEnergy))->array[0];

  return(fvect);
}

/*----------------------------------------------------------------------

 FrameComputeLPResAutoCorrSource : Computes the source features
in a frame of speech using Residual Autocorrelation.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of source cepstral coefficients

 *---------------------------------------------------------------------*/

 F_VECTOR *FrameComputeLPResAutoCorrSource(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
   static int         windowSize;
  static ASDF        *prevAsdf=NULL;
  static F_VECTOR    *waveform;
  static float       *signal;
  static int         minPitch, maxPitch, lpOrder, frameShift, medianOrder, numPthCoefficients;
  static float       *pthAutoCorr, *pthSmthAutoCorr, average;
  int                i;

  if (prevAsdf != asdf) {
    windowSize = GetIAttribute(asdf,"windowSize");
    minPitch = GetIAttribute(asdf, "minPitch");
    maxPitch = GetIAttribute(asdf, "maxPitch");
    lpOrder = GetIAttribute(asdf, "lpOrder");
    medianOrder = GetIAttribute(asdf, "medianOrder");
    frameShift = GetIAttribute(asdf, "frameAdvanceSamples");
    waveform = (F_VECTOR *) AllocFVector((windowSize));
    signal = (float *) calloc(windowSize+1, sizeof(float));
    numPthCoefficients = maxPitch-minPitch+1;
    pthAutoCorr = (float *) calloc (numPthCoefficients, sizeof(float));
    pthSmthAutoCorr = (float *) calloc (numPthCoefficients, sizeof(float));
    prevAsdf = asdf;
  }
 waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
 for (i = 0; i < windowSize; i++)
   signal[i+1] = waveform->array[i];
 pthAutoCorr = (float *) PitchAutoCorrLP(signal, windowSize, frameShift,
					 lpOrder, minPitch, maxPitch, pthAutoCorr);
 // RemoveAverage(&pthAutoCorr[-1], numPthCoefficients, &average);
 MedianSmoothArray (pthAutoCorr, numPthCoefficients-1, medianOrder, pthSmthAutoCorr);
 //Normalize(pthSmthAutoCorr, numPthCoefficients);
 for (i = 0; i < fvect->numElements; i++)
   fvect->array[i] = pthSmthAutoCorr[i];
 return(fvect);
}

/*----------------------------------------------------------------------

 FrameComputeStdCepstrumSource : Computes the source features
in a frame of speech using Standard Cepstrum.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of source cepstral coefficients

 *---------------------------------------------------------------------*/

 F_VECTOR *FrameComputeStdCepstrumSource(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
   static int         mfft, nfft,  nfBy2, windowSize;
  static ASDF        *prevAsdf=NULL;
  static F_VECTOR    *waveform;
  static float       *signal;
  static int         minPitch, maxPitch, medianOrder, numPthCoefficients;
  static float       *pthCepstrum, *pthSmthCepstrum, average;
  int                i;

  if (prevAsdf != asdf) {
    mfft = GetIAttribute(asdf, "fftOrder");
    nfft = GetIAttribute(asdf,"fftSize");
    nfBy2 = nfft/2;
    //    Cstore(nfft);
    windowSize = GetIAttribute(asdf,"windowSize");
    minPitch = GetIAttribute(asdf, "minPitch");
    maxPitch = GetIAttribute(asdf, "maxPitch");
    medianOrder = GetIAttribute(asdf, "medianOrder");
    waveform = (F_VECTOR *) AllocFVector((windowSize));
    signal = (float *) calloc(nfft+1, sizeof(float));
    numPthCoefficients = maxPitch-minPitch+1;
    pthCepstrum = (float *) calloc (numPthCoefficients, sizeof(float));
    pthSmthCepstrum = (float *) calloc (numPthCoefficients, sizeof(float));
    prevAsdf = asdf;
  }
 waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
 for (i = 0; i < windowSize; i++)
   signal[i+1] = waveform->array[i];
 Window(&signal[1],windowSize,'M','D',1.0);
 pthCepstrum = (float *)PitchCepstrumStd(signal, windowSize, nfft, mfft, 
					 minPitch, maxPitch, pthCepstrum);
 //RemoveAverage(&pthCepstrum[-1], numPthCoefficients, &average);
 MedianSmoothArray (pthCepstrum, numPthCoefficients-1, medianOrder, pthSmthCepstrum);
 //Normalize(pthSmthCepstrum, numPthCoefficients);
 for (i = 0; i < fvect->numElements; i++)
   fvect->array[i] = pthSmthCepstrum[i];
 return(fvect);
}

/*----------------------------------------------------------------------

 FrameComputeMinGdSource : Computes the source features
in a frame of speech using MinGd.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of source cepstral coefficients

 *---------------------------------------------------------------------*/

 F_VECTOR *FrameComputeMinGdSource(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
   static int         mfft, nfft,  nfBy2, windowSize;
  static ASDF        *prevAsdf=NULL;
  static F_VECTOR    *waveform;
  static float       *signal;
  static int         minPitch, maxPitch, medianOrder, numPthCoefficients;
  static float       *pthCepstrum, *pthSmthCepstrum, average;
  static float       gamma;
  int                i;

  if (prevAsdf != asdf) {
    mfft = GetIAttribute(asdf, "fftOrder");
    nfft = GetIAttribute(asdf,"fftSize");
    nfBy2 = nfft/2;
    //    Cstore(nfft);
    windowSize = GetIAttribute(asdf,"windowSize");
    minPitch = GetIAttribute(asdf, "minPitch");
    maxPitch = GetIAttribute(asdf, "maxPitch");
    medianOrder = GetIAttribute(asdf, "medianOrder");
    gamma = GetFAttribute(asdf, "gamma");
    waveform = (F_VECTOR *) AllocFVector((windowSize));
    signal = (float *) calloc(nfft+1, sizeof(float));
    numPthCoefficients = maxPitch-minPitch+1;
    pthCepstrum = (float *) calloc (numPthCoefficients, sizeof(float));
    pthSmthCepstrum = (float *) calloc (numPthCoefficients, sizeof(float));
    prevAsdf = asdf;
  }
 waveform = (F_VECTOR *) FrameComputeWaveform(asdf, frameIndex, waveform);
 for (i = 0; i < windowSize; i++)
   signal[i+1] = waveform->array[i];
 Window(&signal[1],windowSize,'M','D',1.0);
 pthCepstrum = (float *) PitchCepstrumMinGd(signal, windowSize, nfft, mfft, 
					    minPitch, maxPitch, gamma, pthCepstrum);
 //RemoveAverage(&pthCepstrum[-1], numPthCoefficients, &average);
 MedianSmoothArray (pthCepstrum, numPthCoefficients-1, medianOrder, pthSmthCepstrum);
 //Normalize(pthSmthCepstrum, numPthCoefficients);
 for (i = 0; i < fvect->numElements; i++)
   fvect->array[i] = pthSmthCepstrum[i];
 return(fvect);
}
/*----------------------------------------------------------------------

 FrameComputeLPModGSource : Computes the source features 
in a frame of speech using ModGd.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of source cepstral coefficients

 *---------------------------------------------------------------------*/

 F_VECTOR *FrameComputeLPModGdSource(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
   static int         mfft, nfft,  nfBy2, windowSize;
  static ASDF        *prevAsdf=NULL;
  static F_VECTOR    *waveform;
  static float       *signal;
  static int         lpOrder, winLen, medianOrder;
  static float       gamma, gdPosScale, gdNegScale, winScaleFactor;
  static int         minPitch, maxPitch, numPthCoefficients;
  static float       *pthCepstrum, *pthSmthCepstrum, average;
  int                i;
  if (prevAsdf != asdf) {
    mfft = GetIAttribute(asdf, "fftOrder");
    nfft = GetIAttribute(asdf,"fftSize");
    nfBy2 = nfft/2;
    //    Cstore(nfft);
    windowSize = GetIAttribute(asdf,"windowSize");
    minPitch = GetIAttribute(asdf, "minPitch");
    maxPitch = GetIAttribute(asdf, "maxPitch");
    medianOrder = GetIAttribute(asdf, "medianOrder");
    winScaleFactor = GetFAttribute(asdf, "winScaleFactor");
    winLen = GetIAttribute(asdf, "numCepstrum");
    gamma = GetFAttribute(asdf, "gamma");
    gdPosScale = GetFAttribute(asdf, "gdPosScale");
    gdNegScale = GetFAttribute(asdf, "gdNegScale");
    lpOrder = GetIAttribute(asdf, "lpOrder");
    waveform = (F_VECTOR *) AllocFVector((windowSize));
    signal = (float *) calloc(nfft+1, sizeof(float));
    numPthCoefficients = maxPitch-minPitch+1;
    pthCepstrum = (float *) calloc (maxPitch-minPitch+1, sizeof(float));
    pthSmthCepstrum = (float *) calloc (maxPitch-minPitch+1, sizeof(float));
    prevAsdf = asdf;
  }
 waveform = (F_VECTOR *) FrameComputeWaveform(asdf,frameIndex,waveform);
 for (i = 0; i < windowSize; i++)
   signal[i+1] = waveform->array[i];
 Window(&signal[1],windowSize,'M','D',1.0);
 pthCepstrum = (float *) PitchCepstrumModifiedGdLP(signal, windowSize, nfft, mfft, 
			minPitch, maxPitch, winLen, winScaleFactor, 
			lpOrder, medianOrder, 
			gamma, gdPosScale, gdNegScale, pthCepstrum);
 //RemoveAverage(&pthCepstrum[-1], numPthCoefficients, &average);
 MedianSmoothArray (pthCepstrum, numPthCoefficients-1, medianOrder, pthSmthCepstrum);
 //Normalize(pthSmthCepstrum, numPthCoefficients);
 for (i = 0; i < fvect->numElements; i++)
   fvect->array[i] = pthSmthCepstrum[i];
 return(fvect);
}

/*----------------------------------------------------------------------

 FrameComputeModGdSource : Computes the source features 
in a frame of speech using ModGd.

 Inputs : front-end structure of type ASDF
          frameIndex
 
 Outputs : a vector of source cepstral coefficients

 *---------------------------------------------------------------------*/

 F_VECTOR *FrameComputeModGdSource(ASDF *asdf, int frameIndex, F_VECTOR *fvect) {
   static int         mfft, nfft,  nfBy2, windowSize;
  static ASDF        *prevAsdf=NULL;
  static F_VECTOR    *waveform;
  static float       *signal;
  static int         minPitch, maxPitch, winLen, gdSmthWinSize, 
                      medianOrder, numPthCoefficients;
  static float       *pthCepstrum, *pthSmthCepstrum, average;
  static float       gamma, gdPosScale, gdNegScale, winScaleFactor;
  int                i;
  if (prevAsdf != asdf) {
    mfft = GetIAttribute(asdf, "fftOrder");
    nfft = GetIAttribute(asdf,"fftSize");
    nfBy2 = nfft/2;
    //    Cstore(nfft);
    windowSize = GetIAttribute(asdf,"windowSize");
    minPitch = GetIAttribute(asdf, "minPitch");
    maxPitch = GetIAttribute(asdf, "maxPitch");
    medianOrder = GetIAttribute(asdf, "medianOrder");
    winLen = GetIAttribute(asdf, "numCepstrum");
    gdSmthWinSize = GetIAttribute(asdf, "gdSmthWinSize");
    winScaleFactor = GetFAttribute(asdf, "winScaleFactor");
    gamma = GetFAttribute(asdf, "gamma");
    gdPosScale = GetFAttribute(asdf, "gdPosScale");
    gdNegScale = GetFAttribute(asdf, "gdNegScale");
    waveform = (F_VECTOR *) AllocFVector((windowSize));
    signal = (float *) calloc(nfft+1, sizeof(float));
    numPthCoefficients = maxPitch-minPitch+1;
    pthCepstrum = (float *) calloc (numPthCoefficients, sizeof(float));
    pthSmthCepstrum = (float *) calloc (numPthCoefficients, sizeof(float));
    prevAsdf = asdf;
  }
 waveform = (F_VECTOR *) FrameComputeWaveform(asdf,frameIndex,waveform);
 for (i = 0; i < windowSize; i++)
   signal[i+1] = waveform->array[i];
 for (i = windowSize+1; i <= nfft; i++)
   signal[i] = 0;
 Window(&signal[1],windowSize,'M','D',1.0);
 pthCepstrum = (float *) PitchCepstrumModifiedGd(signal, windowSize, nfft, mfft, 
			      minPitch, maxPitch,  winLen, winScaleFactor,
                              gdSmthWinSize, medianOrder, gamma, gdPosScale,
			      gdNegScale, pthCepstrum);
 // RemoveAverage(&pthCepstrum[-1], numPthCoefficients, &average);
 MedianSmoothArray (pthCepstrum, numPthCoefficients-1, medianOrder, pthSmthCepstrum);
 //Normalize(pthSmthCepstrum, numPthCoefficients);
 for (i = 0; i < fvect->numElements; i++)
   fvect->array[i] = pthSmthCepstrum[i];
 return(fvect);
}

/*-------------------------------------------------------------------------
 * $Log: BatchProcessWaveform.c,v $
 * Revision 1.15  2011/06/08 15:44:08  hema
 * Perceptual filterbank -- modgd now operates on the
 * log filterbank energy
 *
 * Revision 1.14  2011/03/25 12:15:19  hema
 * Modified FrameComputeFilterbankEnergy to use a Perceptual Filterbank
 *
 * Revision 1.12  2011/03/24 16:55:17  hema
 * Fixed bugs in various group delay functions
 *
 * Revision 1.11  2009/11/12 08:01:23  hema
 * Fixed the bug in computation Velocity and
 * Acceleration parameters
 *
 * Revision 1.10  2007/06/14 08:20:50  hema
 * Modified the computation of Modified group delay function
 * now includes parameters for gamma, alfaP, alfaN
 *
 * Revision 1.9  2004/12/23 08:25:20  hema
 * Added new features
 * frameRootCepstrum
 * frameDeltaRootCepstrum
 * frameDeltaDeltaRootCepstrum
 * frameAugmentedRootCepstrum
 *
 * Revision 1.8  2004/12/21 10:20:31  hema
 * Revised the errors in Augmented Cepstrum Computation
 *
 * Revision 1.7  2003/04/07 09:09:32  hema
 * Added Standard Group Delay as a feature
 *
 * Revision 1.6  2003/04/01 08:50:28  hema
 * Added Mean Computation for Log Energy
 * Modified Residual Group Delay to take a different windowsize
 *
 * Revision 1.5  2003/02/24 10:22:37  hema
 * Added new features
 * LP Derived Cepstra
 * LP Residual Cepstra
 * LP Residual Group Delay
 *
 * Revision 1.4  2003/02/06 07:16:30  hema
 * Added MinGdCepstrum and its derivatives
 *
 * Revision 1.3  2002/11/25 06:04:21  hema
 * Major Changes.  Added a number of new features
 * frameLinearCepstrum, frameSlope, frameModGdCepstrum and
 * their derivatives.
 * frameAugmentedCepstrum, frameAugmentedLinearCepstrum,
 * frameAugmentedSlope, frameAugmentedModGdCepstrum
 * All features support both with and without mean subtraction.
 *
 * Revision 1.2  2002/04/23 07:29:55  hema
 * Added Cepstrum Mean Subtraction
 *
 *
 * Revision 1.1  2002/04/19 10:40:04  hema
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
 *                        End of BatchProcessWaveform.c
 -------------------------------------------------------------------------*/








