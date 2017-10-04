/*-------------------------------------------------------------------------
 *  front-end-types.h - A set of definitions used by the speech front-end
 *  Version:	$Name:  $
 *  Module:	
 *
 *  Purpose:	
 *  See:	
 *
 *  Author:	Hema A Murthy (hema@bhairavi.iitm.ernet.in)
 *
 *  Created:        Mon 10-Sep-2001 15:33:45
 *  Last modified:  Thu 09-Oct-2014 16:13:36 by hema
 *  $Id: front-end-types.h,v 1.1 2001/10/25 12:28:05 hema Exp $
 *
 *  Bugs:	
 *
 *  Change Log:	<Date> <Author>
 *  		<Changes>
 -------------------------------------------------------------------------*/

#ifndef FRONT_END_TYPES
#define FRONT_END_TYPES
#include "stdio.h" 
typedef int INT_TYPE;


typedef float FLOAT_TYPE;

typedef struct {
  int numElements;
  float *array;
} F_VECTOR;

typedef F_VECTOR* VECTOR_OF_F_VECTORS;

typedef union {
      int ival;
      float fval;
      union uTag *next;
    } u;

typedef struct {
  char  waveFileName[500];
  short *waveform;
  VECTOR_OF_F_VECTORS *melCepstrumCosineTransform;
  VECTOR_OF_F_VECTORS *filterbankWeights;
  F_VECTOR *modgd;
  short *vU;
  short *wavePthValues;
  int *dftIndices;
  int fileChanged;
   int waveType;
   int windowSize;
  int resGdWindowSize;
   int fftSize;
   int fftOrder;
  float preemphasis;
   int preemphasisDelay;
   long numSamples;
   int frameAdvanceSamples;
   int numCepstrum;
   int numFilters;
   int numRegressCoeffts;
   int numFrames;
   int numVoicedFrames;
   int samplingRate;
   int lpOrder;
   int zeroOrder;
   int filterOrder;
   int minPitch;
   int maxPitch;
   int numFormants;
   int numAntiFormants;
   int seed;
   int deltaDifference;
   int deltaDeltaDifference;
   int gdSmthWinSize;
   int gdLifterWinSize;
   int gdRemoveLPhase;
   int removeMin;
   int gdSign;
   int mgdNormalize;
   int medianOrder;
   int zeroMean;
   int varianceNormalize;
   int featureVarNormalize;
   int percentFrames;
   int vad;
   int perceptualFilterbank;
   int perceptNumCepstrum;
   int useTrain;
   int useLog;
   int useMinGd;
   int stGauss;
   int stGaussWnd;
   int centOrFreq;
   int chromaFB;
   int chromaOverlapFB;
   int pitchSync;
   int  windowType;
   int  windowShape;
   int numPitch;
   int offset;
  int timeOrFreq;
  int   normalizeSpecFlux;
  int   uniformCentFB;
  int fftScale;
  int numDFTCoefficients;
  float gausMin;
  float warpConst;
  float trapezoidalRatio;
  float bandwidthScale;
  float gamma;
  float gdPosScale;
  float gdNegScale;
  float varianceFloor;
  float winScaleFactor;
  float probScaleFactor;
  float minFrequency, maxFrequency;
  float thresEnergy;
  float thresZero;
  float thresSpecFlatness;
  float tonic;
} ASDF;


typedef struct {
  int numElements;
  int *array;
} I_VECTOR;

typedef I_VECTOR* VECTOR_OF_I_VECTORS;

typedef struct {
  int numColumns;
  int numRows;
  float **array;
} MATRIX;
#endif



















/*-------------------------------------------------------------------------
 * $Log: front-end-types.h,v $
 * Revision 1.1  2001/10/25 12:28:05  hema
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
 *                        End of front-end-types.h
 -------------------------------------------------------------------------*/
