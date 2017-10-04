#ifndef DSP_LIBRARY_H
#define DSP_LIBRARY_H
#include "FrontEndDefs.h"
#include "FrontEndTypes.h"
float HanW(int i,int npts);
float HanDw(int i,int npts); 
float HamW(int i,int npts);
float HamDw(int i,int npts) ;
float *AutoCorr(float *a, int npts);
float BartW(int i,int npts) ;
float LogAdd (float x, float y);
void Window(float *sig,int npts,char hw,char sw,float gausmin);
void TrapeziumWindow(float sig[],int npts,char taperWin,int winSep);
int Imin(float sig[],int npts);
int Imax(float sig[],int npts);
int Imin0(float sig[],int npts);
int Imax0(float sig[],int npts);
int IminShort0(short sig[],int npts);
int ImaxShort0(short sig[],int npts);
int Imax0Actual(float sig[],int npts);
int Imin0Actual(float sig[],int npts);
int ImaxActual(float sig[],int npts);
int IminActual(float sig[],int npts);
void ComputeHistogram (float *array, int numPts, float minValue, float maxValue, int numBins, float *hist);
int FindIndex(int *array, int npts, int index);
int MatchFVectors (F_VECTOR *fvect1, F_VECTOR *fvect2);
int ZeroFVector (F_VECTOR *fvect);
void InitFVector (F_VECTOR *fvect);
int FindMatch(VECTOR_OF_F_VECTORS *vfv, int numVectors, int *array, int npts, int index);
float Median(float *array, int npts);
short MedianShort(short *array, int npts);
float Average(float *array, int npts);
short * MedianSmoothShortArray(short *inArray, int npts, int medianOrder, short *smthArray);
float * MedianSmoothArray(float *inArray, int npts, int medianOrder, float *smthArray);
float * AverageSmoothArray(float *inArray, int npts, int medianOrder, char windowType, float *smthArray);
void LinReg (F_VECTOR *fvect, int startIdx, int numPts, float *intercept, float *slope);
void FindPeaks(float *spectr,int *randPhs,int nfft);
float *FrameCompCepstrum(float *signal, int numPts, float *cepstrum, int numCepstrum, int mfft, int nfft);
void Durbin(float *R,int M,float *A,float *AK,float *E);
void LpAnal(float *Signal,float *E,int Npts, int frameAdvanceSamples, float *A,int Order,float *Gain);
void MASignal(float *residual, float *MASignal, int numSamples, float *coefZero,int orderZ);
void LogSpectrum(float spectrum[],int npts);
float warp (float fin, float minFrequency, float maxFrequency, float warpConstant);
void Cstore(int n);
float *AllocFloatArray(float *array, int npts);
int *AllocIntArray(int *array, int npts);
void Cfft(complex a[],complex b[],int m,int ,int nsign);
void Rfft(float *sig,float *ax,float *ay,int mfft,int nfft,int nsign);
void SpectrumReal(int nfft,float *ax,float *ay,float *amag,float *phase);
void SpectrumComplex(int nfft,complex csig[],float *amag,float *phase);
 void RemoveAverage(float *derv,int nfft,float *ave);
void Normalize(float *signal,int npts);
void LPSpectrum(float *a,int order,float *mag,float *phase,int npts,int mexp,float gain);
void LPCepstrum(float *a, int order,float *cepstrum, int numCepstrum, float gain);
void LogSpectrum(float spectrum[],int npts);
void CepSmooth(float *amag,float *smthAmag,int mfft,int nfft,int winlen,float *co, float gamma);
void LogCepSmooth(float *amag,float *smthAmag,int mfft,int nfft,int winlen,float *c0, float gamma);
VECTOR_OF_F_VECTORS *GeneratePseudoDct(int offset, int numRows, int numColumns);
float Warp (float fin, float minFrequency, float maxFrequency, float warpConstant); 
float TrapezoidalFilter(float startFrequency, float centerFrequency,float fin, float trapRatio);
double *TrinomialMult( int n, double *b, double *c );
float  *BandPassFilter( float minFrequency, float maxFrequency, int samplingRate, int filterOrder);
void ButterWorthFilter( float minFrequency, float maxFrequency, int samplingRate, int filterOrder, float *numerator, float *denominator);
F_VECTOR *AllocFVector( int npts);
F_VECTOR* ReallocFVector(F_VECTOR*fvect,int npts);
F_VECTOR* MergeFVectors(F_VECTOR* fvect,F_VECTOR* temp);
F_VECTOR *FilterbankEnergyIntegration(ASDF *asdf, float *Spectrum,
				      F_VECTOR *fvect);
F_VECTOR *PerceptualFilterbankEnergyIntegration(ASDF *asdf, float *Spectrum, float *modGd, int useLog,
				      F_VECTOR *fvect);

float *FrameCompWaveform (short *waveform,float *array, int frameIndex,int frameShift, int frameSize, long samples);
int GetActualFrameIndex (ASDF *asdf, int frameIndex);
float ComputeEnergy (float *array, int frameSize, float gamma);
void ComputeAverage (float *array, int frameSize, float *ave);
void VoicedUnvoiced(short *waveform, long samples, short *vU, int frameShift, int frameSize, int LPOrder, float thresZero, float thresEnergy, float thresSpecFlatness, int percentFrames); 
float ComputeThresholdEnergy(ASDF *asdf, float threshold, int percentFrames);
float ComputeThresholdZeroCrossing(ASDF *asdf, float threshold, int percentFrames);
float ComputeThresholdSpectralFlatness(ASDF *asdf, float threshold, int percentFrames);
float ComputeZeroCrossing(float *array, int frameSize);
float ComputeSpectralFlatness(float *array, float *residual, int order, int frameSize, int frameShift);
int GradientFrames(float *frame1, float *frame2, int numPts, int frameAdvanceSamples, float thresEnergy, 
		   float thresZero, float thresSpecFlatness);
void GenerateMelFilters(ASDF *asdf);
void GenerateUniformCentFilters(ASDF *asdf);
void GenerateNonUniformCentFilters(ASDF *asdf);
void GenerateChromaFilters(ASDF *asdf);
float ConvertCentToFreq(float cent, float tonic);
float ConvertFreqToCent(float freq, float tonic);
void LinearTransformationOfFVector (F_VECTOR *inVect, VECTOR_OF_F_VECTORS *melCepstrumCosineTransform, int numRows, int numCols, F_VECTOR *outVect);
void LinearVectorDifference (F_VECTOR *fvect1, F_VECTOR *fvect2, F_VECTOR *fvect); 
void LinearVectorAddition (F_VECTOR *fvect1, F_VECTOR *fvect2, F_VECTOR *fvect);
void LinearVectorScalarDivide (float scalar, F_VECTOR *fvect1, F_VECTOR *fvect); 
void LinearVectorScalarMultiply (float scalar, F_VECTOR *fvect1, F_VECTOR *fvect); 
float *GroupDelayCepstrum(float *signal,int npts, int nfft, int mfft, 
			  int winlen, float *groupDelayCepstrum);
float *GroupDelay(float *signal,int npts, int nfft, int mfft, int winlen, float *groupDelay);
float *MinGdCepstrum(float *signal,int npts, int nfft, int mfft, 
		     int winlen, float *minGdCepstrum, float gamma);
float *MinGdCepstrum_DCT(float *signal,int npts, int nfft, int mfft, 
			 int gdSmthWinSize, int numCepstrum, float *minGd, float gamma, int startIndex, int endIndex);
float *MinGd(float *signal,int npts, int nfft, int mfft, 
	     int winlen, float *minGd, float gamma);
float *LPGd(float *signal,int npts, int frameShift, int nfft, int mfft, 
	    int lpOrder, float *LPGdelay);
void StdGroupDelay(float *signal,int npts, int nfft, int mfft, 
		   float *groupDelay);
void LPResGroupDelay(float *signal,int npts, int frameShift, int nfft, int mfft,  
		     int order, float *groupDelay);
float *SmoothMagSpectrum(float *signal, int npts, int nfft, int mfft, 
			 int smthWinSize, float *smthMag);
void NormalizeMgd(float *modGd, float *smthMag, float *modGdCepstrum, int nfft, int mfft);
float *ModGdCepstrumNcN(float *signal,int npts,int nfft,int mfft, 
			int winlen, int smthWinSize, float *modGdCepstrum, 
			float alfaP, float alfaN,  
			float gamma, int gdsign, int removeLPhase, 
                        int removeMin, int mgdNormalize,
			int medOrder,
			int startIndex, int endIndex);
float *ModGdLogSmthCepstrumNcN(float *signal,int npts,int nfft,int mfft, 
			int winlen, int smthWinSize, float *modGdCepstrum, 
			float alfaP, float alfaN,  
			float gamma, int gdsign, int removeLPhase, 
                        int removeMin, int mgdNormalize,
			int medOrder,
			int startIndex, int endIndex);
float *ProductGdCepstrumNcN(float *signal,int npts,int nfft,int mfft, 
			int winlen, int smthWinSize, float *modGdCepstrum, 
			float alfaP, float alfaN,  
			float gamma, int gdsign, int removeLPhase, 
                        int removeMin, int mgdNormalize,
			int startIndex, int endIndex);
float *ModGdCepstrum(float *signal,int npts,int nfft,int mfft, 
		     int winlen, int smthWinSize, float *modGdCepstrum, 
		     float alfaP, float alfaN, float gamma, int gdsign, 
		     int removeLPhase, int removeMin, int startIndex, int endIndex);
float *ModGdCepstrum_DCT(float *signal,int npts,int nfft,int mfft, 
			 int winlen, int smthWinSize, float *modGdCepstrum, 
			 float alfaP, float alfaN, float gamma, int gdsign, 
			 int removeLPhase, int startIndex, int endIndex, 
			 float scaleDCT);
float *ModGdCepstrum_LPDCT(float *signal,int npts,int nfft,int mfft, 
			 int winlen, int lpOrder, float *modGdCepstrum, 
			 float alfaP, float alfaN, float gamma, int gdsign, 
			 int removeLPhase, int startIndex, int endIndex, 
			 float scaleDCT);
float *ModGd_DCT(float *signal,int npts,int nfft,int mfft, 
		 int winlen, int smthWinSize, float *modGd, float alfaP, 
		 float alfaN, float gamma, int gdsign, int removeLPhase,
		 int startIndex, int endIndex, float scaleDCT);
float *SmoothModGd(float *signal,int npts,int nfft,int mfft, 
		   int winlen, int smthWinSize, float gamma, float gdPosScale,
                   float gdNegScale, float *modGd);
float *StandardModGd(float *signal,int npts,int nfft,int mfft, 
		     int smthWinSize, float gamma, float gdPosScale,
                     float gdNegScale, int medianOrder, float *modGd);
float *StandardModGdLP(float *signal,int npts,int nfft,int mfft, 
		     int lpOrder, float gamma, float gdPosScale,
                     float gdNegScale, int medianOrder, float *modGd);
float *ModGdCepstrumFromGD(float *modGd,int nfft, int winlen, 
			   float *modGdCepstrum, float alfaP, float alfaN,  
			   float gamma, int gdsign, int removeLPhase, 
			   int startIndex, int endIndex, float scaleDCT);
float *ModGdCepstrumFromStdCepstra(float *inModGdCepstrum,int nfft, int mfft, 
				   int winlen, float *modGdCepstrum, 
				   float alfaP, float alfaN,  
				   float gamma, int gdsign, int removeLPhase, 
				   int startIndex, int endIndex, float scaleDCT);
float *PitchCepstrumMinGd(float *signal,int npts,int nfft,int mfft, 
			  int pthLow, int pthHgh,float alfa, float *pthCepstrum);
float 	*PitchCepstrumStd(float *signal,int npts,int nfft,int mfft, int pthLow, int pthHgh, float *pthCepstrum);
 float *PitchCepstrumModifiedGdLP(float *signal,int npts,int nfft,int mfft, 
			int pthLow,int pthHgh, int winLen, float winScaleFactor, 
			int lpOrder, int medOrder, 
			float gamma, float gdPosScale,
				 float gdNegScale, float *pthCepstrum);
float *PitchCepstrumModifiedGd(float *signal,int npts,int nfft,int mfft, 
			      int pthLow,int pthHgh, int winLen, float winScaleFactor,
                              int gdSmthWinSize, int medOrder, float gamma, float gdPosScale,
				       float gdNegScale, float *pthCepstrum);

float 	*PitchAutoCorrLP(float *signal,int npts, int frameShift,int lpOrder, int pthLow, int pthHgh, float *pthAutoCorr);
#endif









