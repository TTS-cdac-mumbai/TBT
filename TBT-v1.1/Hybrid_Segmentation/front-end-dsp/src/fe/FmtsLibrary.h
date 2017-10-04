#ifndef FMTS_LIB_H
#define FMTS_LIB_H
float 	*FmtsMinGd(float *signal,int npts,int nfft,
		   int mfft, int winLen,float alfa, 
		   float *freq,int *num);
float *FmtsLpMag(float *signal,int npts, int frameShift, int nfft,
	      int mfft, int LPOrder,float *freq,int *num);

float *FmtsLpPhase(float *signal,int npts, int frameShift, int nfft,
	      int mfft, int LPOrder,float *freq,int *num);
float *FmtsCepstrum(float *signal,int npts,int nfft, 
		    int mfft, int winLen,float *freq,int *num);
float *FmtsModGd(float *signal,int npts,int nfft,
		 int mfft, int winLen, int  gdSmthWinSize, float gamma,
		 float gdPosScale, float gdNegScale,
		 float *freq,int *num);
float *FmtsModGdLP(float *signal,int npts,int nfft,
		 int mfft, int winLen, int  lpOrder, int medianOrder, float gamma,
		 float gdPosScale, float gdNegScale,
		 float *freq,int *num);
#endif



