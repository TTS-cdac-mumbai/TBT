#ifndef PTH_LIBRARY_H
#define PTH_LIBRARY_H
float PitchMinGd(float *Signal,int Npts,int Nfft,int Mfft, int PthLow,int PthHgh,float Alfa);
float 	PitchCepstrum(float *Signal,int Npts,int Nfft,int Mfft, int PthLow, int PthHgh);
float PitchModifiedGd(float *Signal,int Npts,int Nfft,int Mfft, int PthLow, int PthHgh, int WinLen, float winScaleFactor, int gdSmthWinSize, int medOrder, float gamma, float gdPosScale, float gdNegScale);
float PitchModifiedGdLP(float *Signal,int Npts,int Nfft,int Mfft, int PthLow, int PthHgh, int WinLen, float winScaleFactor, int lpOrder, 
			int medianOrder, float gamma, float gdPosScale, float gdNegScale);
float 	PitchLP(float *Signal,int Npts, int frameShift, int PthLow, int PthHgh);
#endif
