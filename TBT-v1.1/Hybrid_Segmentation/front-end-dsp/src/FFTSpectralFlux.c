#include "stdio.h"
#include "math.h"
#include "malloc.h"
/**************************************************************************
 *  $Id: FFTSpectralFlux.c,v 1.1 2000/07/24 02:07:57 hema Exp hema $
 *  File:	FFTSpectralFlux.c - Computes the SpectralFlux of the LPSpectrum (framewise)
 *              of a given signal.
 *
 *  Purpose:	To compute FFTSpectralFlux for music processing
 *
 *  Author:	Hema A Murthy,BSB-307,445-8342,445-9342
 *
 *  Created:    Wed 09-Feb-2000 09:12:15
 *
 *  Last modified:  Tue 11-Oct-2011 10:58:55 by hema
 *
 *  Bugs:	
 *
 *  Change Log:	<Date> <Author>
 *  		<Changes>
 *
 **************************************************************************/
#include "stdio.h"
#include "sp/sphere.h"
#include "fe/FrontEndDefs.h"
#include "fe/FrontEndTypes.h"
#include "fe/DspLibrary.h"
#include "fe/InitAsdf.h"
#include "fe/SphereInterface.h"
/*******************************************************************************
* 	the Following program computes the Spectral Flux
        from frameStart to frameEnd from a given utterance of speech 
*	a file. 
*	Inputs :
*	controlFile, waveFile, specFile, frameStart, frameEnd
*	Output :
*       Average of the LP Spectrum  of data are written to a file 

*******************************************************************************/       
void Usage() {
           printf("Usage : FFTSpectralFlux ctrlFile waveFile sigFile(output) SpecFile SpecChangeFile Avsmth/MedSmth(0/1) windowType normalizeSpecFlux specFluxThreshold (%% average) frameStart frameEnd\n");
}
/*****************************************************************************/

int  main (int argc, char *argv[])
{ 

        float           *signal=NULL;
	int  	        i, fftOrder,fftSize,fftSizeBy2, 
	                frameSize, numFrames,
	                frameNum, medianOrder, frameShift, normalizeSpecFlux,
                        frameStart, frameEnd;
        int             samplingRate, smthType, changeDet, flag=0;
        float           *ax=NULL, *ay=NULL, *FFTSpecCurrent=NULL,*FFTSpecPrev=NULL, *FFTPhase=NULL;
        float           *specFlux=NULL, *specFluxSmooth=NULL, specFluxThreshold, specFluxAverage, specAverage;
	char            windowType = 'R';
    	F_VECTOR        *waveform=NULL;
        FILE            *sigFile=NULL, *specFile=NULL, *controlFile=NULL, *specChangeFile=NULL;
        static ASDF     *asdf;
	float           timeStart, timeEnd, value, specFluxMax;
	char            line[500];
/******************************************************************************/
       if (argc != 12) {
         Usage();
         exit(-1);
       }
       
       controlFile = fopen(argv[1], "r");
       asdf = (ASDF *) malloc(1*sizeof(ASDF));
       InitializeStandardFrontEnd(asdf, controlFile);
       GsfOpen(asdf,argv[2]);
       sigFile = fopen(argv[3], "w");
       specFile = fopen(argv[4],"w");
       specChangeFile = fopen(argv[5], "w");
       sscanf(argv[6], "%d", &smthType);
       sscanf(argv[7], "%c", &windowType);
       sscanf(argv[8], "%d", &normalizeSpecFlux);
       sscanf(argv[9], "%f", &specFluxThreshold);
       sscanf(argv[10], "%d", &frameStart);
       sscanf(argv[11], "%d", &frameEnd);
       frameSize = (int) GetIAttribute(asdf, "windowSize");
       fftOrder  = (int) GetIAttribute(asdf, "fftOrder");
       fftSize  = (int) GetIAttribute(asdf, "fftSize");
       medianOrder   = (int) GetIAttribute(asdf, "medianOrder");
       frameShift = (int) GetIAttribute(asdf, "frameAdvanceSamples");
       numFrames = (int) GetIAttribute(asdf, "numFrames");
       samplingRate = (int) GetIAttribute(asdf, "samplingRate");
       printf ("frameSize = %d fftOrder = %d fftSize = %d frameShift = %d numFrames=%d\n", 
	       frameSize, fftOrder, fftSize, frameShift, numFrames);
       fftSizeBy2 = fftSize/2;
       Cstore(fftSize);
       signal = (float *) AllocFloatArray(signal, fftSize+1);
       FFTSpecCurrent  = (float *) AllocFloatArray(FFTSpecCurrent, fftSize+1);
       FFTSpecPrev  = (float *) AllocFloatArray(FFTSpecPrev, fftSize+1);
       ax       = (float *) AllocFloatArray(ax, fftSize+1);
       ay       = (float *) AllocFloatArray(ay, fftSize+1);
       FFTPhase  = (float *) AllocFloatArray(FFTPhase, fftSize+1);
       specFlux  = (float *) AllocFloatArray(specFlux, numFrames);
       specFluxSmooth  = (float *) AllocFloatArray(specFluxSmooth, numFrames);

       if ((frameStart == -1) && (frameEnd == -1 )){
         frameStart = 0;
	 frameEnd = numFrames - 1;
       }
       waveform = (F_VECTOR *) GsfRead(asdf, frameStart, "frameWaveform");
       for (i = 1; i <= frameSize; i++){
	 //	   printf("waveform %d = %f\n", i-1, waveform->array[i-1]);
	 signal[i] = waveform->array[i-1]*HamDw(i,frameSize);
	 // printf("signal %d = %f\n", i, signal[i]);
       }
       free(waveform->array);
       free(waveform);
       for (i = frameSize+1; i <= fftSize; i++)
	 signal[i] = 0;
       Rfft(signal,ax, ay, fftOrder, fftSize, -1);
       SpectrumReal(fftSize, ax, ay, FFTSpecPrev, FFTPhase);
	 if (normalizeSpecFlux == 1) {
	   specAverage = 0;
	   for (i = 1; i <= fftSizeBy2; i++)
             specAverage = specAverage + FFTSpecPrev[i]*FFTSpecPrev[i];
	   for (i = 1; i <= fftSizeBy2; i++)
             FFTSpecPrev[i] = FFTSpecPrev[i]*FFTSpecPrev[i]/specAverage;
	 } else 
	 if (normalizeSpecFlux == 2) {
	   specAverage = FFTSpecPrev[ImaxActual(FFTSpecPrev, fftSizeBy2)];
	   specAverage = specAverage*specAverage;
	   for (i = 1; i <= fftSizeBy2; i++)
             FFTSpecPrev[i] = FFTSpecPrev[i]*FFTSpecPrev[i]/specAverage;
	 }
       for (frameNum = frameStart+1; frameNum < frameEnd; frameNum++) {
	 waveform = (F_VECTOR *) GsfRead(asdf, frameNum, "frameWaveform");
	 for (i = 1; i <= frameSize; i++){
	   //	   printf("waveform %d = %f\n", i-1, waveform->array[i-1]);
	   signal[i] = waveform->array[i-1]*HamDw(i,frameSize);
	   fprintf (sigFile, "%f %f\n", (float)((frameNum)*frameShift+i)/(float)samplingRate, waveform->array[i-1]);
	 
	   // printf("signal %d = %f\n", i, signal[i]);
	 }
	 free(waveform->array);
	 free(waveform);
         for (i = frameSize+1; i <= fftSize; i++)
           signal[i] = 0;
	 Rfft(signal,ax, ay, fftOrder, fftSize, -1);
	 SpectrumReal(fftSize, ax, ay, FFTSpecCurrent, FFTPhase);
	 if (normalizeSpecFlux == 1) {
	   specAverage = 0;
	   for (i = 1; i <= fftSizeBy2; i++)
             specAverage = specAverage + FFTSpecCurrent[i]*FFTSpecCurrent[i];
	   for (i = 1; i <= fftSizeBy2; i++)
             FFTSpecCurrent[i] = FFTSpecCurrent[i]*FFTSpecCurrent[i]/specAverage;
	 } else
	 if (normalizeSpecFlux == 2) {
	   specAverage = FFTSpecCurrent[ImaxActual(FFTSpecCurrent, fftSizeBy2)];
	   specAverage = specAverage*specAverage;
	   for (i = 1; i <= fftSizeBy2; i++)
             FFTSpecCurrent[i] = FFTSpecCurrent[i]*FFTSpecCurrent[i]/specAverage;
	 }

	 specFlux[frameNum-frameStart] = 0;
	 for (i = 1; i <= fftSizeBy2; i++)
	   specFlux[frameNum-frameStart] = specFlux[frameNum-frameStart] + (FFTSpecCurrent[i]- FFTSpecPrev[i])*(FFTSpecCurrent[i]- FFTSpecPrev[i]);	   
	 for (i = 1; i <= fftSizeBy2; i++)
           FFTSpecPrev[i] = FFTSpecCurrent[i];
	 specFlux[frameNum-frameStart] = specFlux[frameNum-frameStart]/fftSizeBy2;
	 specFluxAverage = specFluxAverage + specFlux[frameNum-frameStart];
	 printf("specFluxAverage = %e specFlux %d = %e \n", specFluxAverage, frameNum-frameStart, specFlux[frameNum-frameStart]);
	 fflush(stdout);
       }
       specFluxMax = specFlux[Imax(specFlux, frameEnd-frameStart+1)];
       for (i = 0; i <= frameEnd-frameStart; i++)
	 specFlux[i] = specFlux[i]/specFluxMax;
       specFluxAverage = specFluxAverage/(frameEnd-frameStart+1)/specFluxMax;
       printf ("specFluxAverage = %f specFluxThreshold = %f\n", specFluxAverage, specFluxThreshold*specFluxAverage);
       if (smthType == 0) {
	 AverageSmoothArray(&specFlux[-1], frameEnd-frameStart, medianOrder, windowType, &specFluxSmooth[-1]);
	 AverageSmoothArray(&specFluxSmooth[-1], frameEnd-frameStart, medianOrder, windowType, &specFlux[-1]);
	 AverageSmoothArray(&specFlux[-1], frameEnd-frameStart, medianOrder, windowType, &specFluxSmooth[-1]);
       } else
	 MedianSmoothArray(&specFlux[-1], frameEnd-frameStart, medianOrder, &specFluxSmooth[-1]);
       for (i = 0; i <= frameEnd-frameStart; i++)
	 if (specFluxSmooth[i] >= specFluxAverage*specFluxThreshold) 
	   fprintf(specFile, "%e %e 0 \n", (float)i*frameShift/(float) samplingRate, specFluxSmooth[i]);
	 else
	   fprintf(specFile, "%e %e 1 \n", (float) i*frameShift/(float) samplingRate, specFluxSmooth[i]);
       fclose(specFile);
       specFile = fopen (argv[3], "r");
       fgets (line, 500, specFile);
       sscanf(line, "%e %e %d", &timeStart, &value, &changeDet);
       if (changeDet == 0) {
	 fprintf (specChangeFile, "%e ", timeStart);
	 flag = 1;
       }
       timeEnd = timeStart;
       while (!(feof(specFile))) {
	 while ((fgets (line, 500, specFile) != NULL) && (changeDet == 0)) {
		timeEnd = timeStart;
		sscanf(line, "%e %e %d", &timeStart, &value, &changeDet);
	 }
	 if (flag) {
	   fprintf (specChangeFile,"%e \n", timeEnd);
	   flag = 0;
	 }
	 while ((fgets (line, 500, specFile) != NULL) && (changeDet == 1)) {
	     sscanf(line, "%e %e %d", &timeStart, &value, &changeDet);
	 }
	 if (!flag) {
	   fprintf (specChangeFile,"%e ", timeStart);
	   flag = 1;
	 }
       }
       if (flag) 
	 fprintf(specChangeFile, "%e ", timeStart);
       fclose(specFile);
       fclose(specChangeFile);
       free(ax);
       free(ay);
       free(FFTPhase);
       free(signal);
       free(FFTSpecCurrent);
       free(FFTSpecPrev);
       free(specFlux);
       free(specFluxSmooth);
       return(0);
}
/**************************************************************************
 * $Log: FFTSpectralFlux.c,v $
 * Revision 1.1  2000/07/24 02:07:57  hema
 * Initial revision
 *
 *
 * Local Variables:
 * time-stamp-active: t
 * time-stamp-line-limit: 20
 * time-stamp-start: "Last modified:[ 	]+"
 * time-stamp-format: "%3a %:d-%3b-%:y %H:%M:%S by %u"
 * time-stamp-end: "$"
 * End:
 *                        End of FFTSpectralFlux.c
 **************************************************************************/



