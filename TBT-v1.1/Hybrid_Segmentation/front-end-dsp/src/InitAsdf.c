/*-------------------------------------------------------------------------
 *  InitAsdf.c - Front-end parameter processing
 *  Version:	$Name:  $
 *  Module:	
 *
 *  Purpose:	
 *  See:	
 *
 *  Author:	Hema A Murthy (hema@bhairavi.iitm.ernet.in)
 *
 *  Created:        Wed 31-Oct-2001 23:11:09
 *  Last modified:  Mon 23-Feb-2015 14:22:33 by hema
 *  $Id: InitAsdf.c,v 1.9 2011/03/24 16:56:24 hema Exp hema $
 *
 *  Bugs:	
 *
 *  Change Log:	<Date> <Author>
 *  		<Changes>
 -------------------------------------------------------------------------*/

/*****************************************************************************
  Function : Front-end parameter processing 
  Author   : Hema A Murthy
  Date     : Dec 10 1997
  Uses     : DspLibrary, sphere_interface
  Known Bugs : none to date
  ****************************************************************************/


#include "stdio.h"
#include "stdlib.h"
//#include "malloc.h"
#include "fe/FrontEndTypes.h"
#include "fe/DspLibrary.h"
#include "sp/sphere.h"
#include "fe/SphereInterface.h"
#include "fe/BatchProcessWaveform.h"
#include "fe/HashTable.h"
#include "fe/InitAsdf.h"
#include "string.h"
#include "stdlib.h"

hashTable* featuresHT = NULL;
typedef F_VECTOR* (*FP)(ASDF* ,int ,F_VECTOR* );

/*****************************************************************************
  Function : stores an integer attribute defined by attributeName
             attribute value in front-end structure asdf
  
  Inputs : asdf, attributeName, attributeValue

  ****************************************************************************/

void PutIAttribute(ASDF *asdf, char *attributeName, int attributeValue) {

if (strcmp(attributeName,"windowSize") == 0)
   asdf->windowSize = attributeValue;

if (strcmp(attributeName,"resGdWindowSize") == 0)
   asdf->resGdWindowSize = attributeValue;

else if (strcmp(attributeName,"waveType") == 0)
   asdf->waveType = attributeValue;

else if (strcmp(attributeName,"fftSize") == 0)
   asdf->fftSize = attributeValue;

else if (strcmp(attributeName,"fftOrder") == 0)
   asdf->fftOrder = attributeValue;

else if (strcmp(attributeName,"frameAdvanceSamples") == 0)
   asdf->frameAdvanceSamples = attributeValue;

else if (strcmp(attributeName,"preemphasisDelay") == 0)
   asdf->preemphasisDelay = attributeValue;

else if (strcmp(attributeName,"samplingRate") == 0)
   asdf->samplingRate = attributeValue;

else if (strcmp(attributeName,"numCepstrum") == 0)
   asdf->numCepstrum = attributeValue;

else if (strcmp(attributeName,"numFilters") == 0)
   asdf->numFilters = attributeValue;

else if (strcmp(attributeName,"numRegressCoeffts") == 0)
   asdf->numRegressCoeffts = attributeValue;

else if (strcmp(attributeName,"seed") == 0)
   asdf->seed = attributeValue;

else if (strcmp(attributeName,"deltaDifference") == 0)
   asdf->deltaDifference = attributeValue;

else if (strcmp(attributeName,"deltaDeltaDifference") == 0)
   asdf->deltaDeltaDifference = attributeValue;

else if (strcmp(attributeName,"lpOrder") == 0)
   asdf->lpOrder = attributeValue;

else if (strcmp(attributeName,"zeroOrder") == 0)
   asdf->zeroOrder = attributeValue;

else if (strcmp(attributeName,"filterOrder") == 0)
   asdf->filterOrder = attributeValue;

else if (strcmp(attributeName,"minPitch") == 0)
   asdf->minPitch = attributeValue;

else if (strcmp(attributeName,"maxPitch") == 0)
   asdf->maxPitch = attributeValue;

else if (strcmp(attributeName,"numFormants") == 0)
   asdf->numFormants = attributeValue;

else if (strcmp(attributeName,"numAntiFormants") == 0)
   asdf->numAntiFormants = attributeValue;

else if (strcmp(attributeName, "gdRemoveLPhase") == 0)
   asdf->gdRemoveLPhase = attributeValue;

else if (strcmp(attributeName, "removeMin") == 0)
   asdf->removeMin = attributeValue;

else if (strcmp(attributeName, "gdSmthWinSize") == 0)
   asdf->gdSmthWinSize = attributeValue;

 else if (strcmp(attributeName,"mgdNormalize") == 0) 
   asdf->mgdNormalize = attributeValue;

else if (strcmp(attributeName,"medianOrder") == 0) 
   asdf->medianOrder = attributeValue;
 
else if (strcmp(attributeName, "gdSign") == 0)
   asdf->gdSign = attributeValue;

else if (strcmp(attributeName, "zeroMean") == 0)
   asdf->zeroMean = attributeValue;

else if (strcmp(attributeName, "varianceNormalize") == 0)
   asdf->varianceNormalize = attributeValue;

else if (strcmp(attributeName, "featureVarNormalize") == 0)
   asdf->featureVarNormalize = attributeValue;

else if (strcmp(attributeName, "percentFrames") == 0)
   asdf->percentFrames = attributeValue;

else if (strcmp(attributeName, "vad") == 0)
   asdf->vad = attributeValue;

else if (strcmp(attributeName, "perceptualFilterbank") == 0)
   asdf->perceptualFilterbank = attributeValue;

else if (strcmp(attributeName, "useTrain") == 0)
   asdf->useTrain = attributeValue;

else if (strcmp(attributeName, "useLog") == 0)
  asdf->useLog = attributeValue;

else if (strcmp(attributeName, "useMinGd") == 0)
   asdf->useMinGd = attributeValue;

else if (strcmp(attributeName, "perceptNumCepstrum") == 0)
 asdf->perceptNumCepstrum = attributeValue;

else if (strcmp(attributeName, "numVoicedFrames") == 0)
   asdf->numVoicedFrames = attributeValue;

else if (strcmp(attributeName, "centOrFreq") == 0)
   asdf->centOrFreq = attributeValue;

else if (strcmp(attributeName, "chromaFB") == 0)
   asdf->chromaFB = attributeValue;

else if (strcmp(attributeName, "chromaOverlapFB") == 0)
   asdf->chromaOverlapFB = attributeValue;

else if (strcmp(attributeName, "pitchSync") == 0)
   asdf->pitchSync = attributeValue;

else if (strcmp(attributeName, "windowType") == 0)
   asdf->windowType = attributeValue;

else if (strcmp(attributeName, "windowShape") == 0)
   asdf->windowShape = attributeValue;

else if (strcmp(attributeName, "numPitch") == 0)
   asdf->numPitch = attributeValue;

else if (strcmp(attributeName, "timeOrFreq") == 0)
   asdf->timeOrFreq = attributeValue;

else if (strcmp(attributeName, "offset") == 0)
   asdf->offset = attributeValue;

else if (strcmp(attributeName, "normalizeSpecFlux") == 0)
   asdf->normalizeSpecFlux = attributeValue;

else if (strcmp(attributeName, "uniformCentFB") == 0)
   asdf->uniformCentFB = attributeValue;

else if (strcmp(attributeName, "fftScale") == 0)
   asdf->fftScale = attributeValue;

else if (strcmp(attributeName, "numDFTCoefficients") == 0)
   asdf->numDFTCoefficients = attributeValue;
}


/*****************************************************************************
  Function : stores a float attribute defined by attributeName
             and attribute value in front-end structure asdf
  
  Inputs : asdf, attributeName, attributeValue

  ****************************************************************************/

void PutFAttribute(ASDF *asdf, char *attributeName, float attributeValue) {

if (strcmp(attributeName,"warpConst") == 0)
   asdf->warpConst = attributeValue;

else if (strcmp(attributeName,"trapezoidalRatio") == 0)
   asdf->trapezoidalRatio = attributeValue;

else if (strcmp(attributeName,"bandwidthScale") == 0)
   asdf->bandwidthScale = attributeValue;

else if (strcmp(attributeName,"minFrequency") == 0)
   asdf->minFrequency = attributeValue;

else if (strcmp(attributeName,"maxFrequency") == 0)
   asdf->maxFrequency = attributeValue;

else if (strcmp(attributeName,"preemphasis") == 0)
   asdf->preemphasis = attributeValue;

else if (strcmp(attributeName,"gamma") == 0)
   asdf->gamma = attributeValue;

else if (strcmp(attributeName, "gdPosScale") == 0)
   asdf->gdPosScale = attributeValue;

else if (strcmp(attributeName, "gdNegScale") == 0)
  asdf->gdNegScale = attributeValue;

else if (strcmp(attributeName, "probScaleFactor") == 0)
  asdf->probScaleFactor = attributeValue;

else if (strcmp(attributeName, "winScaleFactor") == 0)
   asdf->winScaleFactor = attributeValue;

else if (strcmp(attributeName, "varianceFloor") == 0)
   asdf->varianceFloor = attributeValue;

else if (strcmp(attributeName, "thresEnergy") == 0)
  asdf->thresEnergy = attributeValue;

else if (strcmp(attributeName, "thresZero") == 0)
  asdf->thresZero = attributeValue;

else if (strcmp(attributeName, "thresSpecFlatness") == 0)
  asdf->thresSpecFlatness = attributeValue;

else if (strcmp(attributeName, "tonic") == 0)
  asdf->tonic = attributeValue;

}

/*****************************************************************************
  Function : reads an integer attribute defined by 
             attribute name from front-end structure asdf
  
  Inputs : asdf, attributeName

  ****************************************************************************/

int GetIAttribute(ASDF *asdf, char *string) {

 if (strcmp(string, "windowSize") == 0)
   return(asdf->windowSize);

 if (strcmp(string, "resGdWindowSize") == 0)
   return(asdf->resGdWindowSize);

 else if (strcmp(string, "waveType") == 0)
   return(asdf->waveType);

 else if (strcmp(string, "fftSize") == 0)
    return(asdf->fftSize);

 else if (strcmp(string, "fftOrder") == 0)
    return(asdf->fftOrder);

 else if (strcmp(string, "numSamples") == 0)
   return(asdf->numSamples);   

 else if (strcmp(string, "numFilters") == 0)
   return(asdf->numFilters);

  else if (strcmp(string, "numCepstrum") == 0)
    return(asdf->numCepstrum);

  else if (strcmp(string, "numRegressCoeffts") == 0)
    return(asdf->numRegressCoeffts);

  else if (strcmp(string, "frameAdvanceSamples") == 0)
    return(asdf->frameAdvanceSamples);

  else if (strcmp(string, "samplingRate") == 0)
    return(asdf->samplingRate);

  else if (strcmp(string, "seed") == 0)
    return(asdf->seed);

  else if (strcmp(string, "deltaDifference") == 0)
    return(asdf->deltaDifference);

  else if (strcmp(string, "deltaDeltaDifference") == 0)
    return(asdf->deltaDeltaDifference);

 else if (strcmp(string, "numFrames") == 0)
   return(asdf->numFrames);   

 else if (strcmp(string, "numVoicedFrames") == 0)
   return(asdf->numVoicedFrames);   

 else if (strcmp(string, "lpOrder") == 0)
   return(asdf->lpOrder);   

 else if (strcmp(string, "zeroOrder") == 0)
   return(asdf->zeroOrder);   

 else if (strcmp(string, "filterOrder") == 0)
   return(asdf->filterOrder);   

else if (strcmp(string, "minPitch") == 0)
   return(asdf->minPitch);

 else if (strcmp(string, "maxPitch") == 0)
   return(asdf->maxPitch);

 else if (strcmp(string, "numFormants") == 0)
   return(asdf->numFormants);

 else if (strcmp(string, "numAntiFormants") == 0)
   return(asdf->numAntiFormants);

else if (strcmp(string, "removeLPhase") == 0)
   return(asdf->gdRemoveLPhase);

else if (strcmp(string, "removeMin") == 0)
   return(asdf->removeMin);

else if (strcmp(string, "gdSmthWinSize") == 0)
   return(asdf->gdSmthWinSize);

else if (strcmp(string, "gdSign") == 0)
   return(asdf->gdSign);

else if (strcmp(string, "mgdNormalize") == 0)
   return(asdf->mgdNormalize);

else if (strcmp(string, "medianOrder") == 0)
   return(asdf->medianOrder);

else if (strcmp(string, "gdLifterWinSize") == 0)
   return(asdf->gdLifterWinSize);

else if (strcmp(string, "zeroMean") == 0)
   return(asdf->zeroMean);

 else if (strcmp(string, "preemphasisDelay") == 0)
    return(asdf->preemphasisDelay);

 else if (strcmp(string, "featureVarNormalize") == 0)
   return(asdf->featureVarNormalize);

 else if (strcmp(string, "percentFrames") == 0)
   return(asdf->percentFrames);

else if (strcmp(string, "vad") == 0)
   return(asdf->vad);

 else if (strcmp(string, "perceptualFilterbank") == 0)
   return(asdf->perceptualFilterbank);

 else if (strcmp(string, "useTrain") == 0)
   return(asdf->useTrain);

 else if (strcmp(string, "useLog") == 0)
   return(asdf->useLog);

 else if (strcmp(string, "useMinGd") == 0)
   return(asdf->useMinGd);

 else if (strcmp(string, "perceptNumCepstrum") == 0)
 return(asdf->perceptNumCepstrum);

 else if (strcmp (string, "centOrFreq") == 0)
   return(asdf->centOrFreq);

 else if (strcmp (string, "chromaFB") == 0)
   return(asdf->chromaFB);

 else if (strcmp (string, "chromaOverlapFB") == 0)
   return(asdf->chromaOverlapFB);

 else if (strcmp (string, "pitchSync") == 0)
   return(asdf->pitchSync);

 else if (strcmp (string, "windowType") == 0)
   return(asdf->windowType);
 
 else if (strcmp (string, "windowShape") == 0)
   return(asdf->windowShape);

 else if (strcmp (string, "numPitch") == 0)
   return(asdf->numPitch);
 
 else if (strcmp (string, "timeOrFreq") == 0)
   return(asdf->timeOrFreq);
 
 else if (strcmp (string, "normalizeSpecFlux") == 0)
   return(asdf->normalizeSpecFlux);
  
 else if (strcmp(string,"offset") == 0) 
   return(asdf->offset);

 else if (strcmp (string, "uniformCentFB") == 0)
   return(asdf->uniformCentFB);
 
 else if (strcmp (string, "fftScale") == 0)
   return(asdf->fftScale);
 
 else if (strcmp (string, "numDFTCoefficients") == 0)
   return(asdf->numDFTCoefficients);
 
  else return(0);
}

/*****************************************************************************
  Function : reads a float  attribute defined by 
             attribute name from front-end structure asdf
  
  Inputs : asdf, attributeName

  ****************************************************************************/

float GetFAttribute(ASDF *asdf, char *string) {

  if (strcmp(string, "warpConst") == 0)
    return(asdf->warpConst);
  
  else if (strcmp(string, "bandwidthScale") == 0)
    return(asdf->bandwidthScale);
  
  else if (strcmp(string, "trapezoidalRatio") == 0)
    return(asdf->trapezoidalRatio);
  
  else if (strcmp(string, "gamma") == 0)
    return(asdf->gamma);   
  
  else if (strcmp(string, "minFrequency") == 0)
    return(asdf->minFrequency);
  
  else if (strcmp(string, "maxFrequency") == 0)
    return(asdf->maxFrequency);
  
  else if (strcmp(string, "preemphasis") == 0)
    return(asdf->preemphasis);
    
  else if (strcmp(string, "gdPosScale") == 0)
    return(asdf->gdPosScale);
  
  else if (strcmp(string, "gdNegScale") == 0)
    return(asdf->gdPosScale);

 else if (strcmp(string, "probScaleFactor") == 0)
    return(asdf->probScaleFactor);

else if (strcmp(string, "winScaleFactor") == 0)
   return(asdf->winScaleFactor);

 else if (strcmp(string, "varianceFloor") == 0)
   return(asdf->varianceFloor);

 else if (strcmp(string, "thresEnergy") == 0)
    return(asdf->thresEnergy);

 else if (strcmp(string, "thresZero") == 0)
    return(asdf->thresZero);

 else if (strcmp(string, "thresSpecFlatness") == 0)
    return(asdf->thresSpecFlatness);

 else if (strcmp(string, "winScaleFactor") == 0)
    return(asdf->winScaleFactor);

 else if (strcmp(string, "tonic") == 0)
    return(asdf->tonic);
 else
   return(0);
}

/*****************************************************************************
  Function : reads a pointer attribute defined by 
             attribute name from front-end structure asdf
  
  Inputs : asdf, attributeName

  ****************************************************************************/

void *GetPtrAttribute(ASDF *asdf, char *string) {
 if (strcmp(string, "filterbankWeights") == 0)
   return(asdf->filterbankWeights);
 else  if (strcmp(string, "melCepstrumCosineTransform") == 0)
   return(asdf->melCepstrumCosineTransform);
 else  if (strcmp(string, "dftIndices") == 0)
   return(asdf->dftIndices);
 else  if (strcmp(string, "waveform") == 0)
   return(asdf->waveform);
 else return(0);

}

/*****************************************************************************
  Function : Outputs all the front-end parameters in structure asdf
  
  Inputs : asdf

  ****************************************************************************/


void PrintFrontEndParameters (ASDF *asdf) {

   printf ("numFilters = %d\n", asdf->numFilters);
   printf("wavefile = %s\n",asdf->waveFileName);
   printf("windowSize = %d\n",asdf->windowSize);
   printf("resGdWindowSize = %d\n",asdf->resGdWindowSize);
   printf ("numCepstrum = %d\n", asdf->numCepstrum);
   printf ("deltaDifference = %d\n", asdf->deltaDifference);
   printf ("deltaDeltaDifference = %d\n", asdf->deltaDeltaDifference);
   printf ("seed = %d\n", asdf->seed);
   printf("fftSize = %d\n",asdf->fftSize);
   printf("fftOrder = %d\n",asdf->fftOrder);
   printf("preemphasis = %f\n",(float) GetFAttribute(asdf,"preemphasis"));
   printf("preemphasisDelay = %d\n",asdf->preemphasisDelay);
   printf("numSamples = %ld\n",asdf->numSamples);
   printf("numFrames = %d\n",asdf->numFrames);
   printf("samplingRate = %d\n",asdf->samplingRate);
   printf("lpOrder = %d\n",asdf->lpOrder);
   printf("zeroOrder = %d\n",asdf->zeroOrder);
   printf("seed = %d\n",asdf->seed);
   printf("warpConst = %f\n",GetFAttribute(asdf,"warpConst"));
   printf("trapRatio = %f\n",asdf->trapezoidalRatio);
   printf("bwScale = %f\n",asdf->bandwidthScale);
   printf("gamma = %f\n",GetFAttribute(asdf, "gamma"));
   printf("gdPosScale = %f\n",GetFAttribute(asdf, "gdPosScale"));
   printf("gdNegScale = %f\n",GetFAttribute(asdf, "gdNegScale"));
   printf("gdSign = %d\n",asdf->gdSign);
   printf("mgdNormalize = %d\n",asdf->mgdNormalize);
   printf("medianOrder = %d\n",asdf->medianOrder);
   printf("gdRemoveLPhase = %d\n",asdf->gdRemoveLPhase);
   printf("removeMin = %d\n",asdf->removeMin);
   printf("minFreq = %f maxFreq = %f\n",GetFAttribute(asdf, "minFrequency"), 
          GetFAttribute(asdf, "maxFrequency"));   
   printf("varianceNormalize = %d\n",asdf->varianceNormalize);   
   printf("varianceFloor = %f\n",asdf->varianceFloor);   
   printf("probScaleFactor = %f\n",asdf->probScaleFactor);   
   printf("thresEnergy = %f\n",asdf->thresEnergy);   
   printf("thresZero = %f\n",asdf->thresZero);   
   printf("thresSpecFlatness = %f\n",asdf->thresSpecFlatness);   
   printf("percentFrames = %d\n",asdf->percentFrames);   
   printf("winScaleFactor = %f\n",asdf->winScaleFactor);    
   printf("minPitch = %d maxPitch = %d\n",asdf->minPitch, asdf->maxPitch);
   printf("numFormants = %d\n",asdf->numFormants);
   printf("numAntiFormants = %d\n",asdf->numAntiFormants);
   printf ("perceptualFilterbank = %d\n", asdf->perceptualFilterbank);
   printf ("useTrain = %d\n", asdf->useTrain);
   printf ("useLog = %d\n", asdf->useLog);
   printf ("useMinGd = %d\n", asdf->useMinGd);
   printf ("perceptNumCepstrum = %d\n", asdf->perceptNumCepstrum);
   printf ("tonicFrequency=%f\n", asdf->tonic);
   printf ("centOrFreq=%d\n", asdf->centOrFreq);
   printf ("chromaFB=%d\n", asdf->chromaFB);
   printf ("chromaOverlapFB = %d\n", asdf->chromaOverlapFB);
   printf ("pitchSync = %d\n", asdf->pitchSync);
   printf ("normalizeSpecFlux=%d\n", asdf->normalizeSpecFlux);
   printf ("uniformCentFB=%d\n", asdf->uniformCentFB);
   printf ("fftScale=%d\n", asdf->fftScale);
   printf ("numDFTCoefficients = %d\n", asdf->numDFTCoefficients);
}   



/*****************************************************************************
  Function : Initialise the front structure asdf
             asdf
  
  Inputs : asdf

  ****************************************************************************/
void InitializeASDF (ASDF *asdf) {
  asdf->waveFileName[0] ='\0' ;
  //*(asdf->waveform) = NULL;
  asdf->fileChanged = 0;
  asdf->waveType = 0;
  asdf->windowSize = 256;
  asdf->resGdWindowSize = 64;
  asdf->fftSize = 512;
  asdf->fftOrder = 9;
  asdf->preemphasis = 0.0;
  asdf->preemphasisDelay = 0;
  asdf->numSamples = 0;
  asdf->frameAdvanceSamples = 64;
  asdf->numCepstrum = 0;
  asdf->numFilters = 0;
  asdf->numRegressCoeffts = 2;
  asdf->numFrames = 0;
  asdf->samplingRate = 16000;
  asdf->lpOrder = 0;
  asdf->zeroOrder = 0;
  asdf->filterOrder = 0;
  asdf->minPitch = 40;
  asdf->maxPitch = 160;
  asdf->numFormants = 20;
  asdf->numAntiFormants = 0;
  asdf->seed = 13457;
  asdf->deltaDifference = 1;
  asdf->deltaDeltaDifference = 1;
  asdf->warpConst = 0;
  asdf->trapezoidalRatio = 1;
  asdf->bandwidthScale = 0.0;
  asdf->gamma = 1.0;
  asdf->gdPosScale = 1.0;
  asdf->gdNegScale = 1.0;
  asdf->gdSmthWinSize = 6;
  asdf->gdLifterWinSize = 10;
  asdf->gdRemoveLPhase = 0;
  asdf->removeMin = 0;
  asdf->gdSign = 1;
  asdf->mgdNormalize = 1;
  asdf->medianOrder = 0;
  asdf->zeroMean = 0;
  asdf->varianceNormalize = 0;
  asdf->varianceFloor = 1.0E-20;
  asdf->featureVarNormalize = 0;
  asdf->winScaleFactor = 6;
  asdf->probScaleFactor = 1;
  asdf->minFrequency = 0;
  asdf->maxFrequency = 8000;
  asdf->thresEnergy = 0.3;
  asdf->thresSpecFlatness = 0.3;
  asdf->thresZero = 0.3;
  asdf->percentFrames = 1;
  asdf->vad = 0;
  asdf->perceptNumCepstrum = 15;
  asdf->perceptualFilterbank = 0;
  asdf->useTrain = 0;
  asdf->useLog = 0;
  asdf->useMinGd = 1;
  asdf->tonic = 0;
  asdf->centOrFreq = 0;
  asdf->chromaFB = 0;
  asdf->chromaOverlapFB = 0;
  asdf->pitchSync = 0;
  asdf->numPitch = 0;
  asdf->timeOrFreq = 0;
  asdf->windowType = 82;
  asdf->windowShape = 68;
  asdf->uniformCentFB = 1;
  asdf->offset = 2;
  asdf->normalizeSpecFlux = 1;
  asdf->fftScale = 0;
  asdf->numDFTCoefficients = asdf->fftSize;
  //*asdf->melCepstrumCosineTransform = NULL;
  //*asdf->melCepstrumInvTransform = NULL;
  //*asdf->filterbankWeights = NULL;
  //*asdf->dftIndices = NULL;
}
/*****************************************************************************
  Function : associates the front-end file fp  with the front-end structure
             asdf
  
  Inputs : asdf, fp

  ****************************************************************************/



void InitializeStandardFrontEnd(ASDF *asdf,FILE *fp) {

  char                    line[500];
  char                    attributeName[40], attributeType[40];
  char                    attributeString[40],*name = NULL;
  char                    blankString[40];
  float                   attributeValue = 0;
  float                   minFrequency, maxFrequency, dftLineFrequency;
  int                     fftOrder=0, fftSize=0, numCepstrum=0, 
                          numFilters=0, resGdWindowSize, windowSize, 
                          numPthCoefficients, numDFTCoefficients;
  int i;
 
  InitializeASDF(asdf);
  for (i = 0; i < 39; i++)
    blankString[i] =' ';
  blankString[39] = '\0';
  while (fgets(line,500,fp) != NULL) {
    if (line[0] != '#') {
      sscanf(line,"%s %s %s", attributeName, attributeType, attributeString);
      //      printf("%s %s %s\n",attributeName, attributeType, attributeString);
      //fflush(stdout); 
      if (*attributeName != *blankString){
	if (attributeString[0] == '$'){
	  name = (char *) getenv(&attributeString[1]);
	  sscanf(name,"%f",&attributeValue);
	} else
	  sscanf(attributeString,"%f",&attributeValue);
	//	printf("attributeValue %f\n",attributeValue); 
	if (strcmp(attributeName,"windowSize") == 0) {
	  PutIAttribute(asdf, "windowSize",(int) attributeValue);

	} else if (strcmp(attributeName,"resGdWindowSize") == 0) {
	  PutIAttribute(asdf, "resGdWindowSize",(int) attributeValue);

	} else if (strcmp(attributeName,"waveType") == 0) {
	  PutIAttribute(asdf, "waveType",(int) attributeValue);

	} else if (strcmp(attributeName,"fftSize") == 0) {
	  PutIAttribute(asdf, "fftSize",(int) attributeValue);
	  fftOrder = (int) (log(attributeValue)/log(2) +0.5);
	  PutIAttribute(asdf,"fftOrder",(int) fftOrder);

	}  else if (strcmp(attributeName,"samplingRate") == 0) {
	  PutIAttribute(asdf, "samplingRate",(int) attributeValue);

	}  else if (strcmp(attributeName,"lpOrder") == 0) {
	  PutIAttribute(asdf, "lpOrder",(int) attributeValue);

	}  else if (strcmp(attributeName,"zeroOrder") == 0) {
	  PutIAttribute(asdf, "zeroOrder",(int) attributeValue);

	}  else if (strcmp(attributeName,"filterOrder") == 0) {
	  PutIAttribute(asdf, "filterOrder",(int) attributeValue);

	}  else if (strcmp(attributeName,"frameAdvanceSamples") == 0) {
	  PutIAttribute(asdf, "frameAdvanceSamples",(int) attributeValue);

	}  else if (strcmp(attributeName,"preemphasis") == 0) {
	  PutFAttribute(asdf, "preemphasis",attributeValue);

	}  else if (strcmp(attributeName,"preemphasisDelay") == 0) {
	  PutIAttribute(asdf, "preemphasisDelay",(int) attributeValue);

	}  else if (strcmp(attributeName,"numCepstrum") == 0) {
	  PutIAttribute(asdf, "numCepstrum",(int) attributeValue);

	}  else if (strcmp(attributeName,"numFilters") == 0) {
	  PutIAttribute(asdf, "numFilters",(int) attributeValue); 

	}  else if (strcmp(attributeName,"numRegressCoeffts") == 0) {
	  PutIAttribute(asdf, "numRegressCoeffts",(int) attributeValue); 

	}  else if (strcmp(attributeName,"seed") == 0) {
	  PutIAttribute(asdf, "seed",(int) attributeValue);

	}  else if (strcmp(attributeName,"deltaDifference") == 0) {
	  PutIAttribute(asdf, "deltaDifference",(int) attributeValue);

	}  else if (strcmp(attributeName,"deltaDeltaDifference") == 0) {
	  PutIAttribute(asdf, "deltaDeltaDifference",(int) attributeValue);

	}  else if (strcmp(attributeName,"minPitch") == 0) {
	  PutIAttribute(asdf, "minPitch",(int) attributeValue);

	}  else if (strcmp(attributeName,"maxPitch") == 0) {
	  PutIAttribute(asdf, "maxPitch",(int) attributeValue);

	}  else if (strcmp(attributeName,"numFormants") == 0) {
	  PutIAttribute(asdf, "numFormants",(int) attributeValue);

	}  else if (strcmp(attributeName,"numAntiFormants") == 0) {
	  PutIAttribute(asdf, "numAntiFormants",(int) attributeValue);

	}  else if (strcmp(attributeName,"removeLPhase") == 0) {
	  PutIAttribute(asdf, "removeLPhase",(int) attributeValue);

	}  else if (strcmp(attributeName,"removeMin") == 0) {
	  PutIAttribute(asdf, "removeMin",(int) attributeValue);

	}  else if (strcmp(attributeName,"gdSign") == 0) {
	  PutIAttribute(asdf, "gdSign",(int) attributeValue);

	}  else if (strcmp(attributeName,"gdSmthWinSize") == 0) {
	  PutIAttribute(asdf, "gdSmthWinSize",(int) attributeValue);

	}  else if (strcmp(attributeName,"mgdNormalize") == 0) {
	  PutIAttribute(asdf, "mgdNormalize",(int) attributeValue);

	}  else if (strcmp(attributeName,"medianOrder") == 0) {
	  PutIAttribute(asdf, "medianOrder",(int) attributeValue);

	}  else if (strcmp(attributeName,"gdLifterWinSize") == 0) {
	  PutIAttribute(asdf, "gdLifterWinSize",(int) attributeValue);

	}  else if (strcmp(attributeName,"zeroMean") == 0) {
	  PutIAttribute(asdf, "zeroMean",(int) attributeValue);

	}  else if (strcmp(attributeName,"varianceNormalize") == 0) {
	  PutIAttribute(asdf, "varianceNormalize",(int) attributeValue);

	}  else if (strcmp(attributeName,"featureVarNormalize") == 0) {
	  PutIAttribute(asdf, "featureVarNormalize",(int) attributeValue);

	}  else if (strcmp(attributeName,"percentFrames") == 0) {
	  PutIAttribute(asdf, "percentFrames",(int) attributeValue);

	}  else if (strcmp(attributeName,"vad") == 0) {
	  PutIAttribute(asdf, "vad",(int) attributeValue);

	}  else if (strcmp(attributeName,"perceptualFilterbank") == 0) {
	  PutIAttribute(asdf, "perceptualFilterbank",(int) attributeValue);

	}  else if (strcmp(attributeName,"useTrain") == 0) {
	  PutIAttribute(asdf, "useTrain",(int) attributeValue);

	}  else if (strcmp(attributeName,"useLog") == 0) {
	  PutIAttribute(asdf, "useLog",(int) attributeValue);

	}  else if (strcmp(attributeName,"useMinGd") == 0) {
	  PutIAttribute(asdf, "useMinGd",(int) attributeValue);

	}  else if (strcmp(attributeName,"perceptNumCepstrum") == 0) {
	  PutIAttribute(asdf, "perceptNumCepstrum",(int) attributeValue);

	}  else if (strcmp(attributeName,"centOrFreq") == 0) {
	  PutIAttribute(asdf, "centOrFreq",(int) attributeValue);

	}  else if (strcmp(attributeName,"chromaFB") == 0) {
	  PutIAttribute(asdf, "chromaFB",(int) attributeValue);

	}  else if (strcmp(attributeName,"chromaOverlapFB") == 0) {
	  PutIAttribute(asdf, "chromaOverlapFB",(int) attributeValue);

	}  else if (strcmp(attributeName,"pitchSync") == 0) {
	  PutIAttribute(asdf, "pitchSync",(int) attributeValue);

	}  else if (strcmp(attributeName,"windowType") == 0) {
	  PutIAttribute(asdf, "windowType",(int) (attributeString[0]));

	}  else if (strcmp(attributeName,"windowShape") == 0) {
	  PutIAttribute(asdf, "windowShape",(int) (attributeString[0]));

	}  else if (strcmp(attributeName,"numPitch") == 0) {
	  PutIAttribute(asdf, "numPitch",(int) attributeValue);

	}  else if (strcmp(attributeName,"timeOrFreq") == 0) {
	  PutIAttribute(asdf, "timeOrFreq",(int) attributeValue);

	}  else if (strcmp(attributeName,"offset") == 0) {
	  PutIAttribute(asdf, "offset",(int) attributeValue);

	}  else if (strcmp(attributeName,"uniformCentFB") == 0) {
	  PutIAttribute(asdf, "uniformCentFB",(int) attributeValue);
	}  else if (strcmp(attributeName,"fftScale") == 0) {
	  PutIAttribute(asdf, "fftScale",(int) attributeValue);

	}  else if (strcmp(attributeName,"normalizeSpecFlux") == 0) {
	  PutIAttribute(asdf, "normalizeSpecFlux",(int) attributeValue);
}  else if (strcmp(attributeName,"numDFTCoefficients") == 0) {
	  PutIAttribute(asdf, "numDFTCoefficients",(int) attributeValue);

	}  else if (strcmp(attributeName,"minFrequency") == 0) {
	  PutFAttribute(asdf, "minFrequency",attributeValue);

	}  else if (strcmp(attributeName,"winScaleFactor") == 0) {
	  PutFAttribute(asdf, "winScaleFactor",(float) attributeValue);

	}  else if (strcmp(attributeName,"maxFrequency") == 0) {
	  PutFAttribute(asdf, "maxFrequency",attributeValue);

	}  else if (strcmp(attributeName,"tonic") == 0) {
	  PutFAttribute(asdf, "tonic",attributeValue);

	}  else if (strcmp(attributeName,"warpConst") == 0) {
	  PutFAttribute(asdf, "warpConst",attributeValue);

	}  else if (strcmp(attributeName,"trapezoidalRatio") == 0) {
	  PutFAttribute(asdf, "trapezoidalRatio",attributeValue);

	}  else if (strcmp(attributeName,"bandwidthScale") == 0) {
	  PutFAttribute(asdf, "bandwidthScale",attributeValue);

	}  else if (strcmp(attributeName,"gamma") == 0) {
	  PutFAttribute(asdf, "gamma",attributeValue);

	}  else if (strcmp(attributeName,"gdPosScale") == 0) {
	  PutFAttribute(asdf, "gdPosScale",attributeValue);

	}  else if (strcmp(attributeName,"gdNegScale") == 0) {
	  PutFAttribute(asdf, "gdNegScale",attributeValue);

	}  else if (strcmp(attributeName,"probScaleFactor") == 0) {
	  PutFAttribute(asdf, "probScaleFactor",attributeValue);

	}  else if (strcmp(attributeName,"thresEnergy") == 0) {
	  PutFAttribute(asdf, "thresEnergy",attributeValue);

	}  else if (strcmp(attributeName,"thresZero") == 0) {
	  PutFAttribute(asdf, "thresZero",attributeValue);

	}  else if (strcmp(attributeName,"thresSpecFlatness") == 0) {
	  PutFAttribute(asdf, "thresSpecFlatness",attributeValue);

	}  else if (strcmp(attributeName,"varianceFloor") == 0) {
	  PutFAttribute(asdf, "varianceFloor",attributeValue);
     
	}  else if (strcmp(attributeName,"gausMin") == 0) {
	  PutFAttribute(asdf, "gausMin",attributeValue);
     
	}
      }
    }
  }
 numCepstrum = GetIAttribute(asdf,"numCepstrum");
 numFilters =  GetIAttribute(asdf,"numFilters");
 windowSize =  GetIAttribute(asdf,"windowSize");
 resGdWindowSize =  GetIAttribute(asdf,"resGdWindowSize");
 numPthCoefficients = ((int) GetIAttribute(asdf, "maxPitch")) - ((int) GetIAttribute(asdf, "minPitch"))+1;
 fftSize = GetIAttribute(asdf, "fftSize");
 printf("numCepstrum = %d numFilters = %d\n", numCepstrum, numFilters);
 if (fftSize != 0)
   Cstore(fftSize);
if ((numCepstrum != 0) && (numFilters != 0)) {
   asdf->melCepstrumCosineTransform = (VECTOR_OF_F_VECTORS *)
     GeneratePseudoDct(asdf->offset, numCepstrum, numFilters);
   if ((asdf->centOrFreq == 1) && (asdf->chromaFB == 1))
       GenerateChromaFilters(asdf);
   else if ((asdf->centOrFreq == 1) && (asdf->uniformCentFB))
       GenerateUniformCentFilters(asdf);
   else if ((asdf->centOrFreq == 1) && !(asdf->uniformCentFB))
       GenerateNonUniformCentFilters(asdf);
   else
     GenerateMelFilters(asdf);
 }
  if (asdf->perceptualFilterbank) 
   asdf->modgd = (F_VECTOR *) AllocFVector(fftSize/2);
 PrintFrontEndParameters(asdf); 
 featuresHT = InitHashTable(DEFAULT_HASH_TABLE_SIZE);
 if (fftSize != 0) {
 dftLineFrequency = (int) asdf->samplingRate/fftSize;
 if (asdf->centOrFreq == 1) {
   maxFrequency = ConvertCentToFreq(asdf->maxFrequency, asdf->tonic);
   minFrequency = ConvertCentToFreq(asdf->minFrequency, asdf->tonic);
 } else {
   maxFrequency = asdf->maxFrequency;
   minFrequency = asdf->minFrequency;
 }
 numDFTCoefficients = (int) (maxFrequency/dftLineFrequency) - (int)  (minFrequency/dftLineFrequency) + 1;
 if (numDFTCoefficients > fftSize/2)
   numDFTCoefficients = fftSize/2;
 asdf->numDFTCoefficients = numDFTCoefficients;
 }
 InsertFeaturesInfo(featuresHT, numCepstrum, numFilters, numDFTCoefficients, 
		     windowSize, resGdWindowSize, numPthCoefficients);

}


/*****************************************************************************
  Function : associates a wavefile filename   with the front-end structure
             asdf
  
  Inputs : asdf, filename

  ****************************************************************************/



void GsfOpen(ASDF *asdf, char *filename) {

  long                  numSamples; 
  int                   i, j, samples_per_channel, num_channels=1;
  short                 *buff;
  int                   lntFileName;
  char                  pthFileName[500];

  asdf->waveFileName[0] = '\0';
  strcpy(asdf->waveFileName,filename);
  printf("filename = %s\n",filename);
  /*  if (asdf->waveform != NULL) 
      free(asdf->waveform); */
  if (asdf->waveType == 0)
    asdf->waveform = (short *) ReadSpherePcm(filename,&numSamples);
  else if (asdf->waveType == 1)
    asdf->waveform = (short *) ReadSphereUlaw(filename,&numSamples);
  else if (asdf->waveType == 2)
    asdf->waveform = (short *) ReadText(filename,&numSamples, 16000);
  else if (asdf->waveType == 3)
    asdf->waveform = (short *) ReadRaw(filename,&numSamples);
  else if (asdf->waveType == 4)
    asdf->waveform = (short *) ReadRaw16Big(filename,&numSamples);
  else if (asdf->waveType == 5)
    asdf->waveform = (short *) ReadRaw16Little(filename,&numSamples);
  else if (asdf->waveType == 6) 
    asdf->waveform = (short *) ReadRIFF(filename,&numSamples);
  if (num_channels == 2) {
      samples_per_channel = numSamples / num_channels;
      buff = (short *) malloc (sizeof(short) * samples_per_channel);       
      if(select_channel == 0) j = 0;
      else j = 1;
      for(i = 0; i < samples_per_channel; i++) {
            buff[i] = asdf->waveform[j];
            j += 2;
      }
      free(asdf->waveform);
      asdf->waveform = buff;
      numSamples = samples_per_channel;
  }
  else if (num_channels != 1) exit(1);
//    temp = (short *) ReadRIFF(asdf->waveFileName,&numSamples);
  //  asdf->waveform = temp;
      

  /* for (i = 0; i < numSamples-1; i++)
    asdf->waveform[i] = asdf->waveform[i] - 
      asdf->preemphasis*asdf->waveform[i+1]; 
      asdf->waveform[numSamples-1] = asdf->waveform[numSamples-2]; */
  //printf("wavename = %s number of samples = %d\n", filename, numSamples); 
  asdf->numSamples = numSamples;
  asdf->numFrames = (int) (ceilf((float) (numSamples)/(float) (asdf->frameAdvanceSamples)));
  asdf->numVoicedFrames = asdf->numFrames;
  asdf->vU = (short *) malloc (asdf->numFrames*sizeof(short));
  if (asdf->vad == 1) {
    VoicedUnvoiced(asdf->waveform, numSamples, asdf->vU, 
		   asdf->frameAdvanceSamples, asdf->windowSize, asdf->lpOrder,
		   asdf->thresZero, asdf->thresEnergy, 
		   asdf->thresSpecFlatness, asdf->percentFrames);                          
    asdf->numVoicedFrames = 0; 
    for (i = 0; i < asdf->numFrames; i++)
      if (asdf->vU[i])
	asdf->numVoicedFrames++;
  }
  else 
    for (i = 0; i < asdf->numFrames; i++)
      asdf->vU[i] = 1;
  fflush(stdout);
  if (asdf->pitchSync == 1) {
    strcpy(pthFileName,filename);
    lntFileName = strlen(pthFileName);
    pthFileName[lntFileName-4] = '\0';
    strcat (pthFileName, ".pth");
    asdf->wavePthValues = (short *) ReadPitchValues(pthFileName, asdf->timeOrFreq, asdf->numFrames, asdf->samplingRate);
    if (asdf->wavePthValues == NULL) 
      exit(-1);
  }
  printf("exit GsfOpen\n");
}

/*****************************************************************************
  Function : dissociates wavefile from the front-end structure
             asdf
  
  Inputs : asdf

  ****************************************************************************/


void GsfClose(ASDF *asdf) {
  free(asdf->waveform);
  free(asdf->vU);
  /*spClose(asdf->waveFile);
  printf("file closed\n");
  fflush(stdout); */
 
}

/*****************************************************************************
  Function : gets the feature vector featureName for frameIndex of
             wavefile
  
  Inputs : asdf, frameIndex featureName
  Outputs : fvect - feature vector
  Problems : Dangerous allocates space for vector and does not free!
  ****************************************************************************/



F_VECTOR *GsfRead(ASDF *asdf, int frameIndex, char *featureName) {

  F_VECTOR                          *fvect;
  HashElement*                      he=NULL;
  int                               totalFVect=0;
  char                             *featptr=NULL;
  char*                             features=NULL;
  F_VECTOR                          *tmp=NULL;
  if (asdf->vU[frameIndex]) {
    features = (char *)calloc(strlen(featureName)+1, sizeof(char));
    features[0] = '\0';
    //bzero(features,strlen(featureName));
    strcpy(features,featureName);
    featptr = (char *) strtok(features,"+");
    he = SearchHE(featuresHT,featptr);
    fvect = (F_VECTOR*)AllocFVector(he->numFVectors);
    fvect = (F_VECTOR* )((FP)(he->fnptr))(asdf, frameIndex, fvect);
    totalFVect = he->numFVectors;
    featptr = strtok(NULL,"+");
    while(featptr!=NULL)
      {
	he = SearchHE(featuresHT,featptr);
	if(he == NULL)
	  {
	    printf("feature %s not found",featptr);
	    exit(-1);
	  }
	  
	totalFVect = totalFVect+he->numFVectors;
	tmp = (F_VECTOR*)AllocFVector(he->numFVectors);
	tmp = (F_VECTOR* )((FP)(he->fnptr))(asdf,frameIndex,tmp);
	fvect = (F_VECTOR* )ReallocFVector(fvect,totalFVect);
	fvect = (F_VECTOR* )MergeFVectors(fvect,tmp);
	//	  fvect->numElements = totalFVect;
	free(tmp->array);
	free(tmp);
	tmp = NULL;
	//        *featptr = NULL;
	featptr = strtok(NULL,"+");
      }
    free(features);
    return(fvect);
  } else
    return(NULL);
}


/*-------------------------------------------------------------------------
 * $Log: InitAsdf.c,v $
 * Revision 1.9  2011/03/24 16:56:24  hema
 * fixed bugs in GsfRead
 *
 * Revision 1.10 2008/04/04 11:34:21  chaitanya
 * Modified GsfRead function to extract multiple features given
 * feature names are seperated by "+" character
 *
 * Revision 1.9  2007/12/26 08:25:53  chaitanya
 * Added HashTable search capability for features
 *
 * Revision 1.8  2007/06/14 08:23:56  hema
 * Added a function to initialise parameters in the ASDF structure
 * Modified PutIAttribute to accomodate comments in the control file
 * fixed some bugs in initialisation when parameters are absent
 *
 * Revision 1.7  2005/01/21 04:11:06  hema
 * Modified GsfOpen- preemphasis x(n+1) - preemp*x(n) rather than x(n) - preemp*x(n)
 *
 * Revision 1.6  2004/12/23 08:25:53  hema
 * Added new features to GsfRead
 * frameRootCepstrum, frameDeltaRootCepstrumm, frameDeltaDeltaRootCepstrum,
 * frameAugmentedRootCepstrum
 *
 * Revision 1.5  2003/04/01 09:04:54  hema
 * fixed some bugs in InitialiseStandardFrontEnd
 *
 * Revision 1.4  2003/04/01 08:51:07  hema
 * Added new features to GsfRead
 * LinearCepstrum, LPCepstrum
 * added new parameter for residual group delay
 *
 * Revision 1.3  2002/11/25 06:06:01  hema
 * Added a number of front-end parameters for processing
 * modified group delay functions.
 * Included a new parameter zeroMean, zeroMean = 1 ==> mean
 * subtraction is invoked for any feature.
 * zeroMean = 0 ==> no mean subtraction
 *
 * Revision 1.2  2002/04/23 07:30:39  hema
 * Modified GSFRead to accomodate for Cepstrum Mean Subtraction
 *
 * Revision 1.1  2001/10/31 17:41:32  hema
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
 *                        End of InitAsdf.c
 -------------------------------------------------------------------------*/
