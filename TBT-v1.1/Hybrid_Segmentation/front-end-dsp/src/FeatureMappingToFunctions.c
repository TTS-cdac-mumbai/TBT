/*-------------------------------------------------------------------------
 *  FeatureMappingToFunctions.c - Provision to add features to hash table 
 *  Version:    $Name:  $
 *  Module:
 *
 *  Purpose:
 *  See:
 *
 *  Author:    chaitanya (chaitanya@saaranga.iitm.ernet.in)
 *
 *  Created:        Some Time in 1996
 *  Last modified:  Thu 09-Oct-2014 19:46:00 by hema
 * $Id:FeatureMappingToFunctions.c,v1.02008/04/1009:05:22chaitanya Exp chaitanya $
 *
 *  Bugs:
 *
 *  Change Log: <Date> <Author>
 *              <Changes>
 -------------------------------------------------------------------------*/


#include "fe/DspLibrary.h"
#include "fe/BatchProcessWaveform.h"
#include "fe/HashTable.h"

/*****************************************************************************
  Function : InsertFeaturesInfo
  Inputs   : ht,numCepstrum,numFilters,fftSize,windowSize,resGdWindowSize
  Author   : chaitanya
  Date     : Apr 10 2008
 
  ****************************************************************************/


void InsertFeaturesInfo(hashTable* ht,int numCepstrum,int numFilters,int numDFTCoefficients,int windowSize,int resGdWindowSize, int numPthCoefficients)
{

  InsertHE(ht, "frameCepstrum", &FrameComputeMelCepstrum, numCepstrum);

  InsertHE(ht,  "frameCepstrumMean", &FrameComputeMelCepstrumMean, numCepstrum);

  InsertHE(ht, "frameDeltaCepstrum", &FrameComputeMelDeltaCepstrum, numCepstrum);  

  InsertHE(ht, "frameDeltaDeltaCepstrum", &FrameComputeMelDeltaDeltaCepstrum, numCepstrum);

  InsertHE(ht, "frameAugmentedCepstrum", &FrameComputeMelAugmentedCepstrum, 3*numCepstrum+3);

  InsertHE(ht, "frameAugmentedCepstrum", &FrameComputeMelAugmentedCepstrum, 3*numCepstrum+3);

  InsertHE(ht, "frameRootCepstrum", &FrameComputeMelRootCepstrum, numCepstrum);

  InsertHE(ht, "frameRootCepstrumMean", &FrameComputeMelRootCepstrumMean, numCepstrum);

  InsertHE(ht, "frameRootDeltaCepstrum", &FrameComputeMelRootDeltaCepstrum, numCepstrum);

  InsertHE(ht, "frameRootDeltaDeltaCepstrum", &FrameComputeMelRootDeltaDeltaCepstrum, numCepstrum);

  InsertHE(ht, "frameAugmentedRootCepstrum", &FrameComputeMelRootAugmentedCepstrum, 3*numCepstrum+3);

  InsertHE(ht, "frameLinearCepstrum", &FrameComputeLinearCepstrum, numCepstrum);

  InsertHE(ht, "frameLinearCepstrumMean", &FrameComputeLinearCepstrumMean, numCepstrum);  
  InsertHE(ht, "frameLinearDeltaCepstrum", &FrameComputeLinearDeltaCepstrum, numCepstrum); 
  
  InsertHE(ht, "frameLinearDeltaDeltaCepstrum", &FrameComputeLinearDeltaDeltaCepstrum, numCepstrum);

  InsertHE(ht, "frameAugmentedLinearCepstrum", &FrameComputeLinearAugmentedCepstrum, 3*numCepstrum+3);
  
  InsertHE(ht, "frameResGdCepstrum", &FrameComputeMelResGdCepstrum, numCepstrum);

  InsertHE(ht, "frameMelResGdCepstrumMean", &FrameComputeMelResGdCepstrumMean, numCepstrum);
  
  InsertHE(ht, "frameResGdDeltaCepstrum", &FrameComputeMelResGdDeltaCepstrum, numCepstrum);

  InsertHE(ht, "frameResGdDeltaDeltaCepstrum", &FrameComputeMelResGdDeltaDeltaCepstrum, numCepstrum);
 
  InsertHE(ht, "frameAugmentedResGdCepstrum", &FrameComputeMelResGdAugmentedCepstrum, 3*numCepstrum+3);
 
  InsertHE(ht, "frameResidualGDelay", &FrameComputeResidualGDelay, windowSize);

  InsertHE(ht, "frameHEResidualGDelay", &FrameComputeHilbertEnvelopeResidualGDelay, windowSize);
 
  InsertHE(ht, "frameGDelay", &FrameComputeGDelay, numDFTCoefficients);

  InsertHE(ht, "frameMinGDelay", &FrameComputeMinGDelay, numDFTCoefficients);

  InsertHE(ht, "frameLPGDelay", &FrameComputeLPGDelay, numDFTCoefficients);

  InsertHE(ht, "frameModGDelay", &FrameComputeModGDelay, numDFTCoefficients);

  InsertHE(ht, "frameFlatSpectrumLC", &FrameComputeFlatSpectrumLogCepstrum, numDFTCoefficients);

  InsertHE(ht, "frameFlatSpectrumRC", &FrameComputeFlatSpectrumRootCepstrum, numDFTCoefficients);

  InsertHE(ht, "frameSmoothSpectrumRC", &FrameComputeSmthSpectrumRootCepstrum, numDFTCoefficients);

  InsertHE(ht, "frameSmoothSpectrumLC", &FrameComputeSmthSpectrumLogCepstrum, numDFTCoefficients);

  InsertHE(ht, "frameSourceModGDelay", &FrameComputeSourceModGDelay, numDFTCoefficients);

  InsertHE(ht, "frameSourceLPModGDelay", &FrameComputeSourceLPModGDelay, numDFTCoefficients);

  InsertHE(ht, "frameModGDelayMean", &FrameComputeModGDelayMean, numDFTCoefficients);

  InsertHE(ht, "frameModGDelaySmooth", &FrameComputeModGDelaySmooth, numDFTCoefficients); 

  InsertHE(ht, "frameModGDelayLP", &FrameComputeModGDelayLP, numDFTCoefficients);

  InsertHE(ht, "frameModGDelayLPSmooth", &FrameComputeModGDelayLPSmooth, numDFTCoefficients); 

  InsertHE(ht, "frameResModGdCepstrum", &FrameComputeMelResModGdCepstrum, numCepstrum);

  InsertHE(ht, "frameResModGdCepstrumMean", &FrameComputeMelResModGdCepstrumMean, numCepstrum);

  InsertHE(ht, "frameResModGdDeltaCepstrum", &FrameComputeMelResModGdDeltaCepstrum, numCepstrum);

  InsertHE(ht, "frameResModGdDeltaDeltaCepstrum", &FrameComputeMelResModGdDeltaDeltaCepstrum, numCepstrum);

  InsertHE(ht, "frameAugmentedResModGdCepstrum", &FrameComputeMelResModGdAugmentedCepstrum, 3*numCepstrum+3);

  InsertHE(ht, "frameResidualModGDelay", &FrameComputeResidualModGDelay, resGdWindowSize);

  InsertHE(ht, "frameLPSpectrum", &FrameComputeLPSpectrum, numDFTCoefficients); 

  InsertHE(ht, "frameLPCepstrumFFT", &FrameComputeLPCepstrumFFT, numCepstrum);

  InsertHE(ht, "frameLPCepstrumMeanFFT", &FrameComputeLPCepstrumMeanFFT, numCepstrum);

  InsertHE(ht, "frameDeltaLPCepstrumFFT", &FrameComputeDeltaLPCepstrumFFT, numCepstrum);
  
  InsertHE(ht, "frameDeltaDeltaLPCepstrumFFT", &FrameComputeDeltaDeltaLPCepstrumFFT, numCepstrum);

  InsertHE(ht, "frameAugmentedLPCepstrumFFT", &FrameComputeAugmentedLPCepstrumFFT, 3*numCepstrum+3);

    InsertHE(ht, "frameFFTCepstrumMean", &FrameComputeFFTCepstrumMean, numCepstrum);

  InsertHE(ht, "frameDeltaFFTCepstrum", &FrameComputeDeltaFFTCepstrum, numCepstrum);
  
  InsertHE(ht, "frameDeltaDeltaFFTCepstrum", &FrameComputeDeltaDeltaFFTCepstrum, numCepstrum);

  InsertHE(ht, "frameAugmentedFFTCepstrum", &FrameComputeAugmentedFFTCepstrum, 3*numCepstrum+3);

  InsertHE(ht, "frameFFTSpectrum", &FrameComputeFFTSpectrum, numDFTCoefficients);

  InsertHE(ht, "frameFFTSpectrumMean", &FrameComputeFFTSpectrumMean, numDFTCoefficients);

  InsertHE(ht, "frameDeltaFFTSpectrum", &FrameComputeDeltaFFTSpectrum, numDFTCoefficients);
  
  InsertHE(ht, "frameDeltaDeltaFFTSpectrum", &FrameComputeDeltaDeltaFFTSpectrum, numDFTCoefficients);

  InsertHE(ht, "frameAugmentedFFTSpectrum", &FrameComputeAugmentedFFTSpectrum, 3*numDFTCoefficients+3);

  InsertHE(ht, "frameLPCepstrum", &FrameComputeLPCepstrum, numCepstrum);
  
  InsertHE(ht, "frameLPCepstrumMean", &FrameComputeLPCepstrumMean, numCepstrum);
 
  InsertHE(ht, "frameDeltaLPCepstrum", &FrameComputeDeltaLPCepstrum, numCepstrum);
     
  InsertHE(ht, "frameDeltaDeltaLPCepstrum", &FrameComputeDeltaDeltaLPCepstrum, numCepstrum);

  InsertHE(ht, "frameAugmentedLPCepstrum", &FrameComputeAugmentedLPCepstrum, 3*numCepstrum+3);

  InsertHE(ht, "frameModGdCepstrumNcNMean", &FrameComputeModGdCepstrumNcNMean, numCepstrum);

  InsertHE(ht, "frameModGdCepstrumNcN", &FrameComputeModGdCepstrumNcN, numCepstrum);
 
  InsertHE(ht, "frameModGdDeltaCepstrumNcN", &FrameComputeDeltaModGdCepstrumNcN, numCepstrum);

  InsertHE(ht, "frameModGdDeltaDeltaCepstrumNcN", &FrameComputeDeltaDeltaModGdCepstrumNcN, numCepstrum);

  InsertHE(ht, "frameAugmentedModGdCepstrumNcN", &FrameComputeAugmentedModGdCepstrumNcN, 3*numCepstrum+3);

  InsertHE(ht, "frameModGdLogSmthCepstrumNcNMean", &FrameComputeModGdCepstrumNcNMean, numCepstrum);

  InsertHE(ht, "frameModGdLogSmthCepstrumNcN", &FrameComputeModGdLogSmthCepstrumNcN, numCepstrum);
 
  InsertHE(ht, "frameModGdDeltaLogSmthCepstrumNcN", &FrameComputeDeltaModGdLogSmthCepstrumNcN, numCepstrum);

  InsertHE(ht, "frameModGdDeltaDeltaLogSmthCepstrumNcN", &FrameComputeDeltaDeltaModGdLogSmthCepstrumNcN, numCepstrum);

  InsertHE(ht, "frameAugmentedModGdLogSmthCepstrumNcN", &FrameComputeAugmentedModGdLogSmthCepstrumNcN, 3*numCepstrum+3);

  InsertHE(ht, "frameModGdCepstrumLPDCTMean", &FrameComputeModGdCepstrumLPDCTMean, numCepstrum);

  InsertHE(ht, "frameModGdCepstrumLPDCT", &FrameComputeModGdCepstrumLPDCT, numCepstrum);
 
  InsertHE(ht, "frameModGdDeltaCepstrumLPDCT", &FrameComputeDeltaModGdCepstrumLPDCT, numCepstrum);

  InsertHE(ht, "frameModGdDeltaDeltaCepstrumLPDCT", &FrameComputeDeltaDeltaModGdCepstrumLPDCT, numCepstrum);

  InsertHE(ht, "frameAugmentedModGdCepstrumLPDCT", &FrameComputeAugmentedModGdCepstrumLPDCT, 3*numCepstrum+3);

  InsertHE(ht, "frameModGdCepstrumDCTMean", &FrameComputeModGdCepstrumDCTMean, numCepstrum);

  InsertHE(ht, "frameModGdCepstrumDCT", &FrameComputeModGdCepstrumDCT, numCepstrum);
 
  InsertHE(ht, "frameModGdDeltaCepstrumDCT", &FrameComputeDeltaModGdCepstrumDCT, numCepstrum);

  InsertHE(ht, "frameModGdDeltaDeltaCepstrumDCT", &FrameComputeDeltaDeltaModGdCepstrumDCT, numCepstrum);

  InsertHE(ht, "frameAugmentedModGdCepstrumDCT", &FrameComputeAugmentedModGdCepstrumDCT, 3*numCepstrum+3);

  InsertHE(ht, "frameMinGdCepstrumMean", &FrameComputeMinGdCepstrumMean, numCepstrum);

  InsertHE(ht, "frameMinGdCepstrum", &FrameComputeMinGdCepstrum, numCepstrum);

  InsertHE(ht, "frameMinGdDeltaCepstrum", &FrameComputeDeltaMinGdCepstrum, numCepstrum);

  InsertHE(ht, "frameMinGdDeltaDeltaCepstrum", &FrameComputeDeltaDeltaMinGdCepstrum, numCepstrum);

  InsertHE(ht, "frameAugmentedMinGdCepstrum", &FrameComputeAugmentedMinGdCepstrum, 3*numCepstrum+3);

  InsertHE(ht, "frameFilterbankEnergy", &FrameComputeFilterbankEnergy, numFilters);

  InsertHE(ht, "frameFilterbankLogEgyMean", &FrameComputeFilterbankLogEnergyMean, numFilters);

  InsertHE(ht, "frameFilterbankLogEgy", &FrameComputeFilterbankLogEnergy, numFilters);
  InsertHE(ht, "frameFilterbankDeltaLogEgy", &FrameComputeDeltaFilterbankLogEnergy, numFilters);
  InsertHE(ht, "frameFilterbankDeltaDeltaLogEgy", &FrameComputeDeltaDeltaFilterbankLogEnergy, numFilters);
InsertHE(ht, "frameAugmentedFilterbankLogEgy", &FrameComputeAugmentedFilterbankLogEnergy, numFilters);

  InsertHE(ht, "frameWaveform", &FrameComputeWaveform, windowSize);

  InsertHE(ht, "frameSlope", &FrameComputeMelSlope, numFilters-1); 
   
  InsertHE(ht, "frameDeltaSlope", &FrameComputeMelDeltaSlope, numFilters-1);

  InsertHE(ht, "frameDeltaDeltaSlope", &FrameComputeMelDeltaDeltaSlope, numFilters-1); 

  InsertHE(ht, "frameAugmentedSlope", &FrameComputeMelAugmentedSlope, 3*(numFilters-1)+3);

  InsertHE(ht, "frameLPResAutoCorrSource", &FrameComputeLPResAutoCorrSource, numPthCoefficients);

  InsertHE(ht, "frameStdCepstrumSource", &FrameComputeStdCepstrumSource, numPthCoefficients);

  InsertHE(ht, "frameMinGdSource", &FrameComputeMinGdSource, numPthCoefficients);

  InsertHE(ht, "frameModGdSource", &FrameComputeModGdSource, numPthCoefficients);

  InsertHE(ht, "frameLPModGdSource", &FrameComputeLPModGdSource, numPthCoefficients);
  
  InsertHE(ht, "frameZeroCrossing", &FrameComputeZeroCrossing, 1);

  InsertHE(ht, "frameSpectralFlatness", &FrameComputeSpectralFlatness, 1);

  InsertHE(ht, "frameSpectralFlux", &FrameComputeSpectralFlux, 1);

  InsertHE(ht, "significantChange", &FrameComputeSignificantChange, 1);

  InsertHE(ht, "frameEnergy", &FrameComputeEnergy, 1);

  InsertHE(ht, "frameDeltaEnergy", &FrameComputeDeltaEnergy, 1);
  
  InsertHE(ht, "frameDeltaDeltaEnergy", &FrameComputeDeltaDeltaEnergy, 1);

  InsertHE(ht, "frameLogEnergy", &FrameComputeLogEnergy, 1);

  InsertHE(ht, "frameDeltaLogEnergy", &FrameComputeDeltaLogEnergy, 1);
  
  InsertHE(ht, "frameDeltaDeltaLogEnergy", &FrameComputeDeltaDeltaLogEnergy, 1);
 
  return;

}

/*-------------------------------------------------------------------------
 * $Log: FeatureMappingToFunctions.cv $

*
 * Local Variables:
 * time-stamp-active: t
 * time-stamp-line-limit: 20
 * time-stamp-start: "Last modified:[   ]+"
 * time-stamp-format: "%3a %02d-%3b-%:y %02H:%02M:%02S by %u"
 * time-stamp-end: "$"
 * End:
 *                        End of FeatureMappingToFunctions.c
 -------------------------------------------------------------------------*/

