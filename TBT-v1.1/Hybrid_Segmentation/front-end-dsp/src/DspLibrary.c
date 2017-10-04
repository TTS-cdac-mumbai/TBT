 /*-------------------------------------------------------------------------
 *  DspLibrary.c - A Library of Signal Processing Programs
 *  Version:	$Name:  $
 *  Module:	
 *
 *  Purpose:	
 *  See:	
 *
 *  Author:	Hema A Murthy (hema@bhairavi.iitm.ernet.in)
 *
 *  Created:        Some Time in 1996
 *  Last modified:  Thu 09-Oct-2014 16:45:09 by hema
 *  $Id: DspLibrary.c,v 1.8 2011/06/08 15:42:26 hema Exp hema $
 *
 *  Bugs:	
 *
 *  Change Log:	<Date> <Author>
 *  		<Changes>
 -------------------------------------------------------------------------*/
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "math.h"
//#include "malloc.h"
#include "fe/constants.h"
#include "fe/FrontEndDefs.h"
#include "fe/FrontEndTypes.h"
#include "fe/QuickSort.h"
#include "fe/InitAsdf.h"
#include "fe/DspLibrary.h"

/*-------------------------------------------------------------------------
 *  AllocFloatArray -- Allocates an array of floats
 *    Args:	Array, size of array
 *    Returns:	allocated array
 *    Bugs:	
 * -------------------------------------------------------------------------*/

float * AllocFloatArray(float *array, int npts)
{
  int i;
  array = (float *) calloc (npts, sizeof(float));
  for (i = 0; i < npts; i++)
    array[i] = 0;
  if (array == NULL) {
    printf("unable to allocate Float array \n");
    exit(-1);
  }
  return(array);
}	/*  End of AllocFloatArray */	

/*-------------------------------------------------------------------------
 *  AllocIntArray -- Allocates an array of Ints
 *    Args:	Array, size of array
 *    Returns:	allocated array
 *    Bugs:	
 * -------------------------------------------------------------------------*/

int *AllocIntArray(int *array, int npts)
{
  array = (int *) calloc(npts,sizeof(int));
  if (array == NULL) {
    printf("unable to allocate Int array \n");
    _exit(-1);
  }
  return(array);
}	/*  End of AllocIntArray  */


/* Global definitions for fft computation  */
static     int *iBit=NULL;
static     float *twiddleReal=NULL, *twiddleImag=NULL;


float HanW(int i, int npts) { 
 return(0.5+0.5*cosf(PI*((i)-1)/((npts)-1)));
}

float HanDw(int i, int npts){ 
  static float            *window = NULL;
  int                     k;
  if (window == NULL) {
    window = (float *) AllocFloatArray(window, npts+1);
    for (k = 1; k <= npts; k++)
      window[k] = (float)(0.5-0.5*cos((double)PI2*((k)-1)/(npts-1)));
  }
  return(window[i]);
}
float HamW(int i, int npts) {
  return(0.54+0.46*cos((double) PI*((i)-1)/((npts)-1)));
}
float HamDw(int i, int npts){ 
  static float            *window = NULL;
  int                     k;
  if (window == NULL) {
    window = (float *) AllocFloatArray(window, npts+1);
    for (k = 1; k <= npts; k++)
      window[k] = (float)(0.54-0.46*cos((double)PI2*((k)-1)/(npts-1)));
  }
  return(window[i]);
  }

/*float HamDw(int i,int npts) {
  return((float) (0.54-0.46*cos((double) PI2*((i)-1)/((npts)-1))) );
  }
*/
float BartW(int i,int npts) {
  return (((float)((double) (npts)-(i)))/((npts)-1));
}

/*--------------------------------------------------------------------------- 
  Window applies a specified window to a signal[]  

  inputs - 
            signal - array of floating point no's
            npts  -  size of signal array
            hw - 'M' - Hamming window
                 'N' - Hann window
                 'B' - Bartlett window
                 'G' - Gaussian window 
                 'R' - Rectangular window 
     All other values invalid and taken as 'rectangular' by default.

            sw - 'S' - single sided
                 'D' - double sided
                       all other values invalid   
            gausmin - for Gaussian window only: the least value at the 
                  window ends i.e 1st and last values.The parameter 
                  'min' is ignored for other windows.
 --------------------------------------------------------------------------*/

 void Window(float *sig, int npts, char hw, char sw, float gausmin)
 {
   int i;
   float midpoint,sigma2,tempval,temp;

   if ((hw=='M') && (sw=='S')) /* Single sided ham Window */
      for(i=0; i< npts; i++ ) sig[i]=sig[i]*HamW(i,npts);
   
   else if ((hw=='M') && (sw=='D')) /* Double sided ham Window */
      for(i = 0; i < npts; i++ ) sig[i]=sig[i]*HamDw(i+1,npts); 

   else if ((hw=='N') && (sw=='S')) /* Single sided han Window */
      for(i = 0; i < npts; i++ ) sig[i]=sig[i]*HanW(i,npts);

   else if ((hw=='N') && (sw=='D')) /* Double sided han Window */
      for(i = 0; i < npts; i++ ) sig[i]=sig[i]*HanDw(i+1,npts);
   
   else if ((hw=='B') && (sw=='S')) /* Single sided Bartlett Window */
      for(i=1;i<=npts;i++) sig[i]=sig[i]*BartW(i,npts);

   else if ((hw=='B') && (sw=='D')){ /* Double sided Bartlett Window */
      midpoint=((float)(npts)+1.0)/2.0;
      for(i=1;i<=npts;i++){
        tempval=(2.0*(i-1))/(float)(npts-1);
        sig[i]*=(((float)(i))<midpoint?tempval:(2.0-tempval));
      }
   } 
   else if (hw == 'G'){
     if(gausmin < 1.0){
        tempval = sqrt((double)(-2.0*log(gausmin)));
        if(sw =='S'){
          sigma2 = ((float)npts-1.0)/tempval; /* Standard Dev. */
          sigma2 = -2.0*sigma2*sigma2; 
          for(i = 0; i < npts; i++){
             temp = exp((double)((i)*(i)/sigma2));
             sig[i]*= temp;
          }
        }
        else if(sw == 'D'){
          midpoint = ((float)(npts)+1.0)/2.0;
          sigma2 = (midpoint-1.0)/tempval;  /* Standard Dev. */
          sigma2 = -2.0*sigma2*sigma2;
          for(i = 0;i < npts; i++){
            temp = exp((double)(((float)i-midpoint)*((float)i-midpoint)/sigma2));
            sig[i]*= temp;
          }
        }
        else printf("\n ERROR from Window: Bad Win. Symmetry.\n");
     }
     else printf("\n Error from Gaus.win.:'min' cannot be < 1 .");
   }
   else if (hw != 'R') {
      printf("Warning from subroutine Window:\n"); 
      printf("\t\t error in Window specification\n");
      printf("\t\t Rectangular Window is used.\n");
   }
 }


/*--------------------------------------------------------------------------
    The subroutine TrapeziumWindow() Windows the passed signal with
    a Window which is rectangular in the middle and has a taper
    at the ends of the specified type,ex:Hamming,Hann,Bartlett etc.
    Inputs -
           sig[] - (float)array of signal to be Windowed.
           npts - (int) total Window length = winSep + 2*winLen
           taperWin - (char) type of Window used as taper at ends.
           winSep - (int)the length of the central portion in which it
                     is rectangular.
    Output -
           sig[] - the passed signal array is Windowed and passed
                   back.
    Note: All arrays are indexed from 1 and not zero.
 --------------------------------------------------------------------------*/

 void TrapeziumWindow(float sig[], int npts, char taperWin, int winSep)
 {
    int n,dummy,endWinLen;
    float temp[256+1];

    /* get taper Window(one sided) into temp[]. */
    endWinLen=(npts-winSep)/2;  /* this is length of endWindows. */
    for(n=1;n<=endWinLen;temp[n++]=1.0);
    Window(temp,endWinLen,taperWin,'S',0.01);

    for(n=1;n<=endWinLen;n++)
       sig[n]*=temp[endWinLen-n+1];

    dummy=endWinLen+winSep;
    for(n=dummy+1;n<=npts;n++)
       sig[n]*=temp[n-dummy];
 }
/**********************************************************************
 *  LogAdd               - Adds two log values x and y 
 *                       - Useful for adding small probabilities
 *  inputs               - two values log x and log y
 *  outputs              - log (x + y)
 **********************************************************************/
float LogAdd(float x, float y)
{
	float temp, diff, z;
        float value;

	if (x < y) {
		temp = x;
		x = y;
		y = temp;
	}
	diff = y - x;

	if (diff < LOG_ZERO)
		return (x < LOG_SMALL) ? LOG_ZERO : x;
	else {
		z = expf(diff);
		return x + log(1.0 + z);
	}
return(value);
}

/*--------------------------------------------------------------------------
  Imin returns the location of the minimum (absolute value) of an array signal of 
         size npts.   Assumes that array starts from index 1.                         
 -------------------------------------------------------------------------*/

 int Imin(float sig[], int npts)
 {
   int i,lmin;
   lmin = 1;
   for ( i=2; i<=npts; i++ ) 
     if (fabs(sig[i]) < fabs(sig[lmin])) lmin = i;
   return (lmin);
 }

/*--------------------------------------------------------------------------
    Imax returns the location of the maximum (absolute value) of 
    an array signal of size npts. Assume that array starts from index 1
 -------------------------------------------------------------------------*/
 int Imax(float sig[], int npts)
 {
   int i,lmax;
   lmax = 1;
   for ( i=2; i<=npts; i++ ) 
     if (fabsf(sig[i]) > fabsf(sig[lmax])) lmax = i;
   return (lmax);
 }
/*--------------------------------------------------------------------------
  Imin returns the location of the minimum (absolute value) of an array signal of 
         size npts(indices start from 0)                          
 -------------------------------------------------------------------------*/

 int Imin0(float sig[], int npts)
 {
   int i,lmin;
   lmin = 0;
   for ( i=1; i<npts; i++ ) 
     if (fabs(sig[i]) < fabs(sig[lmin])) lmin = i;
   return (lmin);
 }

/*--------------------------------------------------------------------------
    Imax returns the location of the maximum (absolute value) of 
    an array signal of size npts (indices start from 0) 
 -------------------------------------------------------------------------*/
 int Imax0(float sig[], int npts)
 {
   int i,lmax;
   lmax = 0;
   for ( i=1; i<npts; i++ ) 
     if (fabs(sig[i]) > fabs(sig[lmax])) lmax = i;
   return (lmax);
 }
/*--------------------------------------------------------------------------
  Imin returns the location of the minimum (absolute value) of a short array signal of 
         size npts(indices start from 0)                          
 -------------------------------------------------------------------------*/

 int IminShort0(short sig[], int npts)
 {
   int i,lmin;
   lmin = 0;
   for ( i=1; i<npts; i++ ) 
     if (fabs(sig[i]) < fabs(sig[lmin])) lmin = i;
   return (lmin);
 }

/*--------------------------------------------------------------------------
    ImaxShort0 returns the location of the maximum (absolute value) of 
    an integer array signal of size npts (indices start from 0) 
 -------------------------------------------------------------------------*/
 int ImaxShort0(short sig[], int npts)
 {
   int i,lmax;
   lmax = 0;
   for ( i=1; i<npts; i++ ) 
     if (fabs(sig[i]) > fabs(sig[lmax])) lmax = i;
   return (lmax);
 }
/*--------------------------------------------------------------------------
  Imin returns the location of the minimum of an array signal of 
         size npts.                          
 -------------------------------------------------------------------------*/

 int IminActual(float sig[], int npts)
 {
   int i,lmin;
   lmin = 1;
   for ( i=2; i<=npts; i++ ) 
     if (sig[i] < sig[lmin]) lmin = i;
   return (lmin);
 }

/*--------------------------------------------------------------------------
    Imax returns the location of the maximum of 
    an array signal of size npts. Assumes index starts from 1
 -------------------------------------------------------------------------*/
 int ImaxActual(float sig[], int npts)
 {
   int i,lmax;
   lmax = 1;
   for ( i=2; i<=npts; i++ ) 
     if (sig[i] > sig[lmax]) lmax = i;
   return (lmax);
 }

/*--------------------------------------------------------------------------
  Imin returns the location of the minimum of an array signal of 
         size npts. Assumes index starts from 0                         
 -------------------------------------------------------------------------*/

 int Imin0Actual(float sig[], int npts)
 {
   int i,lmin;
   lmin = 0;
   for ( i=1; i<npts; i++ ) 
     if (sig[i] < sig[lmin]) lmin = i;
   return (lmin);
 }


/*--------------------------------------------------------------------------
    Imax returns the location of the maximum of 
    an array signal of size npts. 
 -------------------------------------------------------------------------*/
 int Imax0Actual(float sig[], int npts)
 {
   int i,lmax;
   lmax = 0;
   for ( i = 1; i < npts; i++ ) 
     if (sig[i] > sig[lmax]) lmax = i;
   return (lmax);
 }


/*--------------------------------------------------------------------------
    FindIndex checks whether a given element is found in a 
    an array of ints of size npts
 -------------------------------------------------------------------------*/
 int FindIndex(int *array, int npts, int index)
 {
   int flag = 0;
   int i = 0;
   while ((i <= npts) && (!flag)) 
     if (array[i] == index)
       flag = 1;
     else 
       i++;
   return(flag);
 }
/*--------------------------------------------------------------------------
    Compare two F_VECTORs for equality 
    ---------------------------------------------------------------------------*/
int MatchFVectors (F_VECTOR *fvect1, F_VECTOR *fvect2) {
  int i = 0;
  int flag = 1;
  while ((i < fvect1->numElements) && (flag)) {
    flag = (fvect1->array[i] == fvect2->array[i]);
    i++;
  }
  return(flag);
}

/*---------------------------------------------------------------------------
Computes the histogram of a given array of elements
-----------------------------------------------------------------------------*/
void ComputeHistogram (float *array, int numPts, float minValue, float maxValue, int numBins, float *hist) {
  //  float minValue, maxValue;
  float binWidth, value;
  int i, index; 
  //minValue = array[Imin0(array, numPts)];
  //maxValue = array[Imax0(array, numPts)];
  binWidth = (maxValue - minValue)/numBins;
  for (i = 0; i <= numBins; i++)
    hist[i] = 0;
  for (i = 0; i < numPts; i++)
    if ((array[i] >= minValue) && (array[i] <= maxValue)) {
      value  = (array[i]-minValue)/binWidth;
      index = (int) (value);
      printf ("value=%f index = %d \n", value, index);
      if (index > numBins+1)
	printf ("index = %d\n", index);
      hist[index]++;
    }
}
/*--------------------------------------------------------------------------
    Set elements in an F_VECTOR to zero 
    ---------------------------------------------------------------------------*/
void InitFVector (F_VECTOR *fvect) {
  int i;
  for (i = 0; i < fvect->numElements; i++) 
    fvect->array[i] = 0;
}
/*--------------------------------------------------------------------------
    Determine if most elements in an F_VECTOR are zero 
    ---------------------------------------------------------------------------*/
int ZeroFVector (F_VECTOR *fvect) {
  int i = 0;
  int flag = 0;
  int cnt = 0;
  int numElem = ceilf((float)fvect->numElements/3.0);
  while ((i < fvect->numElements) && (cnt < numElem)) {
    flag = (fvect->array[i] == 0);
    i++;
    if (flag)
      cnt++;
  }
  flag = cnt >= numElem;
  return(flag);
}
/*--------------------------------------------------------------------------
    FindMatch checks whether a given element already exists 
    an array of ints of size npts
 -------------------------------------------------------------------------*/
 int FindMatch(VECTOR_OF_F_VECTORS *vfv, int numVectors, int *array, int npts, int index)
 {
   F_VECTOR *fvect1, *fvect2;
   fvect1 = vfv[index];
   int flag = 0;
   int i = 0;
   flag = ZeroFVector(fvect1);
   if(!flag)
     while ((i <= npts) && (!flag)) {
       fvect2 = vfv[array[i]];
       flag = MatchFVectors(fvect1, fvect2); 
       i++;
     }
   return(flag);
 }
/*--------------------------------------------------------------------------
    Performs Linear regression on a set of elements of F_VECTOR
    Returns the slope intercept of the line.  
    Inputs: 
          fvect of type F_VECTOR
          startIdx -- start index of element in F_VECTOR
          numPts -- number of points used for interpolation.
    ---------------------------------------------------------------------------*/


void LinReg (F_VECTOR *fvect, int startIdx, int numPts, float *intercept, float *slope)
{
  float               sumX = 0.0, sumY = 0.0, sqSumX = 0.0, sqSumY = 0.0, sqSumXY = 0.0;
  float               m, c;  // y = mx + c
  float               norm;
  int                 i, N;
  N = startIdx+numPts < fvect->numElements ? numPts : fvect->numElements - startIdx;
  for (i = startIdx; i < (startIdx+numPts) && i < fvect->numElements; i++)
    {
	     sumY += fvect->array[i];
		  sqSumY += (fvect->array[i] * fvect->array[i]);
		  sqSumXY += (fvect->array[i] * (i+1));		  
		  sumX += (i+1);
		  sqSumX += ((i+1)*(i+1));
	 }
  norm = (N * sqSumX) - (sumX * sumX);
  if (norm != 0) {
    c = ((sqSumX * sumY) - (sumX * sqSumXY))/norm;
    m = ((N * sqSumXY) - (sumX * sumY))/norm; 
  } else {
    m = 0;
    c = 0;
  }
  //  if (isnan(m))
  //  printf ("slope is Nan\n");
// the following code computes variances of the slope and intercept
// and is not required for frameSlope calculation
/*
  varc = sqSumX / norm;
  varm  = N / norm;

  chiSquared = 0.0;
  for (i = startIdx; i < (startIdx+numPts) && i < fvect->numElements; i++)
    {
	   estimate = (startIdx+1) - (c + m*fvect->array[i]);
		estimate = estimate * estimate;
		chiSquared += estimate;
	 }

  if (N > 2)       
	 {
	   correctionFactor = sqrt (chiSquared / (N-2));
	   varc *= correctionFactor;
		varm *= correctionFactor;
	 }
*/	 
  
  *intercept = c;
  *slope = m;
}
  
/*-------------------------------------------------------------------------
MedianShort returns the location of the middle value 
in an array signal of size npts.  Assumes index starts from 1
--------------------------------------------------------------------------*/
 short MedianShort(short sig[], int npts)
 {
  short            *sigcopy;
  short             temp, medianValue;
  int               min,i,j;
  sigcopy = (short *) calloc(npts+1, sizeof(short));
  for (i = 1; i <= npts; i++)
    sigcopy[i] = sig[i];
  for (i = 1; i <= npts; i++) {
    min = i;
    for (j = i+1; j <= npts; j++)
      if (sigcopy[j] < sigcopy[min]) 
        min = j;
    temp = sigcopy[i];
    sigcopy[i] = sigcopy[min];
    sigcopy[min] = temp;
  }
  medianValue = sigcopy[(npts+1)/2];
  free(sigcopy);
  return(medianValue);
 }

/*-------------------------------------------------------------------------
median returns the location of the middle value 
in an array signal of size npts.  Assumes index starts from 1
--------------------------------------------------------------------------*/
 float Median(float sig[], int npts)
 {
  float *sigcopy=NULL;
  float             temp, medianValue;
  int               min,i,j;
  
  sigcopy = (float *) AllocFloatArray(sigcopy, npts+1);
  for (i = 1; i <= npts; i++)
    sigcopy[i] = sig[i];
  for (i = 1; i <= npts; i++) {
    min = i;
    for (j = i+1; j <= npts; j++)
      if (sigcopy[j] < sigcopy[min]) 
        min = j;
    temp = sigcopy[i];
    sigcopy[i] = sigcopy[min];
    sigcopy[min] = temp;
  }
  medianValue = sigcopy[(npts+1)/2];
  free(sigcopy);
  return(medianValue);
 }

/*-------------------------------------------------------------------------
 *  MedianSmoothShortArray -- median smooth a short array
 *    Args:	array, npts, medianOrder
 *    Returns:	median smoothed array
 *    Bugs:	
 * -------------------------------------------------------------------------*/

short *MedianSmoothShortArray(short *inArray, int npts, int medianOrder, short *smthArray)
{
  short           *medArray;
    int                    i, j;
    medArray = (short *) malloc ((medianOrder+1)*sizeof(short));
  for (i = 1; i <= npts; i++) {
    if ((i > medianOrder/2) && (i < npts-medianOrder/2))
      for (j = 1; j <= medianOrder; j++)
	medArray[j] = inArray[i-medianOrder/2+j];
    else if( i <= medianOrder/2){ 
      for (j = 1; j <= medianOrder/2; j++)
	medArray[j] = inArray[i];
      for (j = medianOrder/2+1; j <= medianOrder; j++)
	medArray[j] = inArray[i-medianOrder/2+j];
    } else 
      if (i >= npts-medianOrder/2) {
      for (j = 1; j <= medianOrder/2; j++)
	medArray[j] = inArray[i-medianOrder/2+j];
      for (j = medianOrder/2+1; j <= medianOrder; j++)
	medArray[j] = inArray[i];
    }
    smthArray[i] = (short) MedianShort (medArray,medianOrder);
  }
  free(medArray);
  return(smthArray);
  
}	/*  End of MedianSmoothShortArray		End of MedianSmoothShortArray   */


/*-------------------------------------------------------------------------
 *  AverageSmoothArray -- median smooth an array
 *    Args:	array, npts, medianOrder
 *    Returns:	median smoothed array
 *    Bugs:	
 * -------------------------------------------------------------------------*/

float * AverageSmoothArray(float *inArray, int npts, int averageOrder, char windowType, float *smthArray)
{
  static float           *aveArray;
  int                    i, j;
  aveArray = (float *) malloc ((averageOrder+1)*sizeof(float));
  for (i = 1; i <= npts; i++) {
    if ((i > averageOrder/2) && (i < npts-averageOrder/2))
      for (j = 1; j <= averageOrder; j++)
	aveArray[j] = inArray[i-averageOrder/2+j];
    else if( i <= averageOrder/2){ 
      for (j = 1; j <= averageOrder/2; j++)
	aveArray[j] = inArray[i];
      for (j = averageOrder/2+1; j <= averageOrder; j++)
	aveArray[j] = inArray[i-averageOrder/2+j];
    } else 
      if (i >= npts-averageOrder/2) {
      for (j = 1; j <= averageOrder/2; j++)
	aveArray[j] = inArray[i-averageOrder/2+j];
      for (j = averageOrder/2+1; j <= averageOrder; j++)
	aveArray[j] = inArray[i];
    }
    if (windowType != 'R')
      Window(aveArray, averageOrder, windowType, 'D', 1.0E-10);
    smthArray[i] = (float) Average (aveArray,averageOrder);
  }
  free(aveArray);
  return(smthArray);
  
}	/*  End of AverageSmoothArray		End of AverageSmoothArray   */

/*-------------------------------------------------------------------------
 *  MedianSmoothArray -- median smooth an array
 *    Args:	array, npts, medianOrder
 *    Returns:	median smoothed array
 *    Bugs:	
 * -------------------------------------------------------------------------*/

float * MedianSmoothArray(float *inArray, int npts, int medianOrder, float *smthArray)
{
  float           *medArray=NULL;
  int                    i, j;
  medArray = (float *) malloc ((medianOrder+1)*sizeof(float));
  /*  for (i = 1; i <= npts; i++) {
    if ((i >= medianOrder/2+1) && (i <= npts-(medianOrder/2+1)))
      for (j = 1; j <= medianOrder; j++)
	medArray[j] = inArray[i-medianOrder/2+j-2];
    else if( i <= medianOrder/2){ 
      for (j = 1; j <= medianOrder/2; j++)
	medArray[j] = inArray[1];
      for (j = medianOrder/2+1; j <= medianOrder; j++)
	medArray[j] = inArray[i-medianOrder/2+j-2];
    }else if (i >= npts-medianOrder/2) {
      for (j = 1; j <= medianOrder/2; j++)
	medArray[j] = inArray[i-medianOrder/2+j-2];
      for (j = medianOrder/2+1; j <= medianOrder; j++)
	medArray[j] = inArray[npts];
    }
    smthArray[i] = (float) Median (medArray,medianOrder);
  }
  */
  for (i = 1; i <= npts; i++) {
    if ((i > medianOrder/2) && (i < npts-medianOrder/2))
      for (j = 1; j <= medianOrder; j++)
	medArray[j] = inArray[i-medianOrder/2+j];
    else if( i <= medianOrder/2){ 
      for (j = 1; j <= medianOrder/2; j++)
	medArray[j] = inArray[1];
      for (j = medianOrder/2+1; j <= medianOrder; j++)
	medArray[j] = inArray[i-medianOrder/2+j];
    } else {
      for (j = 1; j <= medianOrder/2; j++)
	medArray[j] = inArray[i-medianOrder/2+j];
      for (j = medianOrder/2+1; j <= medianOrder; j++)
	medArray[j] = inArray[npts];
    }
    smthArray[i] = (float) Median (medArray,medianOrder);
  }
  free(medArray);
  return(smthArray);
  
}	/*  End of MedianSmoothArray		End of MedianSmoothArray   */

/*------------------------------------------------------------------------- 
   FindPeaks finds the peaks in the given array of data.
   Inputs-
           Spectrum[] - array of length nfft
           nfft - no of points in the input array

   Output-
            randPhs[]  -  array of nfft integers, the location of the
                          of peaks is set to 1

 ------------------------------------------------------------------------*/
void FindPeaks(float *Spectrum, int *randPhs, int npts)
 {
   int  i;
   randPhs[1] = 0;
   randPhs[npts] = 0; 
   for (i = 2; i < npts; i++) {
      if ((Spectrum[i-1] < Spectrum[i] )&&(Spectrum[i] > Spectrum[i+1]))  {
       randPhs[i] = 1;
      }
      else randPhs[i] = 0; 
   }
 }


/*--------------------------------------------------------------------------
  Cstore computes the twiddle factors used in FFT computation.
          The twiddle factors are stored in the global arrays
          IBIT, twiddleReal, twiddleImag.                                    
 -------------------------------------------------------------------------*/

 void Cstore(int n)
 /*  int n;*/        /* FFT order */
 {
   int  nv2,nm1,ix,ix1,j,i,k;
   float pi2byn;
   nv2 = n/2;
   if (iBit == NULL) {
     iBit = (int *) AllocIntArray(iBit,n+1);
     twiddleReal = (float *) AllocFloatArray(twiddleReal,nv2+1);
     twiddleImag = (float *) AllocFloatArray(twiddleImag,nv2+1);
   }
   nm1 = n-1;
   iBit[1] = 1;
   iBit[n] = n;
   ix = 0;
   for (i=2; i <= nm1; i++){
     j = 0;
     k = nv2;
     ix1 = ix;     
     while (ix1 >= k) { j = j+k; ix1 = ix1-k; k = k/2; };
     ix = ix + k - j;
     iBit[i] = ix + 1;
   };
   pi2byn = (float)(8.0*atan((double)1.0)/(double)n);
   for (i=1; i <= nv2; i++) {
     k = i-1;
     twiddleReal[i] = (float)cos((double)(pi2byn * k));
     twiddleImag[i] = (float)sin((double)(pi2byn * k));
   }
 }


/* ----------------------------------------------------------------------------


Cfft computes the FT of a complex signal.
        inputs - 
                a - complex signal of length n
                  n - FFT order
                m - m such that 2**m = n
                nsign -  -1  forward
                          1  inverse
        
        outputs - 
                 b - complex array of length n            

-----------------------------------------------------------------------------*/

void FFTReal(a, b, m, n, nsign)
     int                   m,n,nsign;
     complex               a[],b[];
{
  int                       nv2,nm1,i,j,ip,k,le,le1,le2,l;
  static                    float log2; 
  static                    int flag = 0;
  complex                   u,t;

  if ((int)pow(2,m)!=n){
    printf("ERROR from Cfft: 2**m != n\n");
    _exit(1);
  }
  if (flag == 0) {
    log2 = log((double)2.0);
    flag = 1;
  } 
  nv2 = n/2;
  nm1 = n-1;
  for ( i=1; i<=n; i++ ) b[iBit[i]] = a[i]; 
  
  for ( i=1; i<=n/2; i+=2 )
    {
      ip = i+1;
      t = b[ip];
      csub( b[ip],b[i],t ); /* b[ip] = b[i] - t  */
      b[iBit[ip]].im = -b[ip].im;
      cadd( b[i],b[i],t );  /* b[i] = b[i] + t   */
      b[iBit[i]].im = -b[i].im;
    };
  
  for( i=1; i<=n/2; i+=4 )
    {
      ip = i+2;
      t = b[ip];
      csub( b[ip],b[i],t ); /* b[ip] = b[i] - t  */
      b[iBit[ip]].im = -b[ip].im;
      cadd( b[i],b[i],t );  /* b[i] = b[i] + t   */
      b[iBit[i]].im = -b[i].im;
   };

 for( i=2; i<=n/2; i+=4 )
    {
     ip = i+2;
     t.re = -nsign * b[ip].im;
     t.im =  nsign * b[ip].re;
     csub( b[ip],b[i],t ); /* b[ip] = b[i] - t  */
      b[iBit[ip]].im = -b[ip].im;
     cadd( b[i],b[i],t );  /* b[i] = b[i] + t   */
      b[iBit[i]].im = -b[i].im;
    };

 for( l=3; l<=m; l++ )
    {
     le2 = (int) (exp(log((double)2.0)*(m-l)) + 
		  (double)0.5);  /* le2 = 2**(m-l) */
     le = (int) (exp(log2*(double)l)+(double)0.5);   /* le = 2**l */
     le1 = le/2;
     for ( j=1; j<=le1; j++ )
        {
         k = (j-1)*le2+1;
         u.re = twiddleReal[k];
         u.im = nsign*twiddleImag[k];
         for ( i=j; i<=n/2; i+=le )
            {
             ip = i+le1;
             cmul(t,b[ip],u);   /*  t = b[ip]*u  */
             csub( b[ip],b[i],t ); /* b[ip] = b[i] - t  */
	     b[iBit[ip]].im = -b[ip].im;
             cadd( b[i],b[i],t );  /* b[i] = b[i] + t   */
             b[iBit[i]].im = -b[i].im;

            };
        };
    };
 if(nsign==1) for ( i=1; i<=n; i++ ) { b[i].re=b[i].re/(float)n;
                                        b[i].im=b[i].im/(float)n; };
}


/* ----------------------------------------------------------------------------


Cfft computes the FT of a complex signal.
        inputs - 
                a - complex signal of length n
                  n - FFT order
                m - m such that 2**m = n
                nsign -  -1  forward
                          1  inverse
        
        outputs - 
                 b - complex array of length n            

-----------------------------------------------------------------------------*/

void Cfft(a, b, m, n, nsign)
     int                   m,n,nsign;
     complex               a[],b[];
{
  int                       nv2,nm1,i,j,ip,k,le,le1,le2,l;
  static                    float log2; 
  static                    int flag = 0;
  complex                   u,t;

  if ((int)pow(2,m)!=n){
    printf("ERROR from Cfft: 2**m != n\n");
    _exit(1);
  }
  if (flag == 0) {
    log2 = log((double)2.0);
    flag = 1;
  } 
  nv2 = n/2;
  nm1 = n-1;
  for ( i=1; i<=n; i++ ) b[iBit[i]] = a[i]; 
  
  for ( i=1; i<=n; i+=2 )
    {
      ip = i+1;
      t = b[ip];
      csub( b[ip],b[i],t ); /* b[ip] = b[i] - t  */
      cadd( b[i],b[i],t );  /* b[i] = b[i] + t   */
    };
  
  for( i=1; i<=n; i+=4 )
    {
      ip = i+2;
      t = b[ip];
      csub( b[ip],b[i],t ); /* b[ip] = b[i] - t  */
      cadd( b[i],b[i],t );  /* b[i] = b[i] + t   */
    };

 for( i=2; i<=n; i+=4 )
    {
     ip = i+2;
     t.re = -nsign * b[ip].im;
     t.im =  nsign * b[ip].re;
     csub( b[ip],b[i],t ); /* b[ip] = b[i] - t  */
     cadd( b[i],b[i],t );  /* b[i] = b[i] + t   */
    };

 for( l=3; l<=m; l++ )
    {
     le2 = (int) (exp(log((double)2.0)*(m-l)) + 
		  (double)0.5);  /* le2 = 2**(m-l) */
     le = (int) (exp(log2*(double)l)+(double)0.5);   /* le = 2**l */
     le1 = le/2;
     for ( j=1; j<=le1; j++ )
        {
         k = (j-1)*le2+1;
         u.re = twiddleReal[k];
         u.im = nsign*twiddleImag[k];
         for ( i=j; i<=n; i+=le )
            {
             ip = i+le1;
             cmul(t,b[ip],u);   /*  t = b[ip]*u  */
             csub( b[ip],b[i],t ); /* b[ip] = b[i] - t  */
             cadd( b[i],b[i],t );  /* b[i] = b[i] + t   */
            };
        };
    };
 if(nsign==1) for ( i=1; i<=n; i++ ) { b[i].re=b[i].re/(float)n;
                                        b[i].im=b[i].im/(float)n; };
}



/*---------------------------------------------------------------------------  

    Rfft computes the FT of a real signal   
    inputs - 
             signal - real array of length nfft
             mfft  -    2**mfft = nfft
             nfft  -  order of fft
             nsign -  -1  forward
		       1  inverse
    outputs -
	     ax - array of real part of  FT
	     ay - array of imaginary part of FT             

-----------------------------------------------------------------------------*/


void Rfft(sig, ax, ay, mfft, nfft, nsign)
 float  sig[], ax[], ay[];
 int  mfft,nfft,nsign;
{
  static                complex *a, *b;
  int i;
  static                int flag = 0;
 if (flag == 0) {
   a = (complex *) malloc((nfft+1)*sizeof(complex));
   b = (complex *) malloc((nfft+1)*sizeof(complex));
   if ((a == NULL) || (b == NULL)) {
     printf("unable to allocate complex array \n");
     _exit(-1);
   }
   flag = 1;
 }
 for (i=1; i<=nfft; i++ ) { a[i].re = sig[i];
                            a[i].im = 0.0;       };
 
 // FFTReal(a,b,mfft,nfft,nsign);
 Cfft(a,b,mfft,nfft,nsign);
 for ( i=1; i<=nfft; i++ ) { ax[i] = b[i].re; ay[i] = b[i].im; };
}


/*---------------------------------------------------------------------------

  
    SpectrumReal  computes the magnitude and phase from real and imaginary
           parts of FT
    inputs - 
              ax - array of length nfft - real part
              ay - array of length nfft - imaginary part
    outputs -
              amag - array of length nfft  -  magnitude Spectrum
             phase - "        "            -  phase Spectrum

----------------------------------------------------------------------------*/

void SpectrumReal(nfft, ax, ay, amag, phase)
 int             nfft;
 float           ax[],ay[],amag[],phase[];
{
 int             i;
 for ( i = 1; i <= nfft; i++ )
    {
     amag[i] = sqrt(ax[i]*ax[i] + ay[i]*ay[i]);
     if ((ay[i]==ax[i])&&(ax[i]==0.0))
       phase[i] = atan2((double)ay[i],1.0);
     else
       phase[i] = atan2((double)ay[i],(double)ax[i]);
    }
}


/*---------------------------------------------------------------------------  

  SpectrumComplex computes the magnitude and phase of a complex FT   

----------------------------------------------------------------------------*/

void SpectrumComplex(nfft, csig, amag, phase)
 int                nfft;
 complex            csig[];
 float              amag[],phase[];
{
 int                i;
 for (i=1; i<=nfft; i++ ) 
    {
     amag[i] = csig[i].re*csig[i].re + csig[i].im*csig[i].im;
     if (csig[i].re == 0.0) phase[i] = PI/2.0;
     else phase[i] = atan2(csig[i].im,csig[i].re);
    }
}
/*--------------------------------------------------------------------------
  FrameCompCepstrum : Computes the cepstrum of a given frame of speech

  inputs :
     signal, numPts : signal is an array of numPts
     numCepstrum    : number of cepstral coeffts reqd.
     mfft, nfft      : 2**mfft = nfft
     
  outputs :
     cepstrum        : an array of numCepstrum cepstral coeffts

---------------------------------------------------------------------------*/
float *FrameCompCepstrum(float *signal, int numPts, float *cepstrum, int numCepstrum, int mfft, int nfft)
{
 float resAx[1025],resAy[1025],amag[1025],phase[1025];
 int i;
 for (i = numPts+1; i <= nfft; i++)
   signal[i] = 0;
 Rfft(signal, resAx, resAy, mfft, nfft, -1);
 SpectrumReal(nfft, resAx, resAy, amag, phase);
 for ( i=1; i<=nfft; i++ ) amag[i] = log(amag[i]);
 Rfft(amag,resAx,resAy,mfft,nfft,1);
  for ( i=1; i<=numCepstrum; i++ )
     cepstrum[i-1] = resAx[i+1];
  return(cepstrum);
}

/******************************************************************************
   AllocFVector : Allocate space for a variable of type F_VECTOR with an 
   array size of numPts.
   inputs : npts
   outputs : an F_VECTOR of size npts
******************************************************************************/

F_VECTOR *AllocFVector(int npts) {
   F_VECTOR *fVect;
   fVect = (F_VECTOR *) malloc(1*sizeof(F_VECTOR));
   if (fVect == NULL) {
     printf("unable to allocate FVector \n");
     _exit(-1);
   }
   fVect->numElements = npts;
   fVect->array = (float *) calloc(npts, sizeof(float));
   if (fVect->array == NULL) {
     printf("unable to allocate FVector array \n");
     _exit(-1);
   } else
    return(fVect);
 }

/******************************************************************************
   ReallocFVector : Changes the memory block pointed by fvect structure member
                  array to the size of npts
   inputs : fvect,npts
   outputs : an F_VECTOR of size npts
******************************************************************************/

F_VECTOR* ReallocFVector(F_VECTOR* fvect,int npts){
  //fvect->numElements = npts;
  fvect->array = (float*)realloc(fvect->array,sizeof(float)*(npts+1));
  if(fvect->array==NULL) {
        printf("unable to allocate FVector array \n");
        _exit(-1);
  } else
  return(fvect);
}


/******************************************************************************
   MergeFVectors : Merges the member array of fvect and temp to array member of
                  fvect
   inputs : fvect,temp
   outputs : an F_VECTOR
******************************************************************************/

F_VECTOR* MergeFVectors(F_VECTOR* fvect,F_VECTOR* temp)
{
  int i=0;
  for(i=0;i<temp->numElements;i++)
  {
        fvect->array[(fvect->numElements)+i] = temp->array[i];
  }

  fvect->numElements = (fvect->numElements)+(temp->numElements);
  return fvect;

}



/*------------------------------------------------------------------------
  LinearVectorDifference : Computers the vectorial difference between two F_VECTORs
  inputs :
    fvect1,fvect2 : two F_VECTORs of identical size
  outputs :
    fvect         : vectorial difference fvect1 - fvect2
-------------------------------------------------------------------------*/   
void LinearVectorDifference (F_VECTOR *fvect1, F_VECTOR *fvect2, F_VECTOR *fvect) {
  int                numElements;
  int                i;

  numElements = fvect1->numElements;
  for (i = 0; i < numElements; i++)
    fvect->array[i] = fvect1->array[i] - fvect2->array[i];
}
/*------------------------------------------------------------------------
  LinearVectorAddition : Computers the vectorial addition between two F_VECTORs
  inputs :
    fvect1,fvect2 : two F_VECTORs of identical size
  outputs :
    fvect         : vectorial addition fvect1 + fvect2
-------------------------------------------------------------------------*/   
void LinearVectorAddition (F_VECTOR *fvect1, F_VECTOR *fvect2, F_VECTOR *fvect) {
  int                numElements;
  int                i;

  numElements = fvect1->numElements;
  for (i = 0; i < numElements; i++)
    fvect->array[i] = fvect1->array[i] + fvect2->array[i];
}

/*------------------------------------------------------------------------
  LinearVectorScalarMultiply : Multiplies a vector by a scalar constant
  inputs :
  scalar        : float
  fvect1        : F_VECTOR 
  outputs :
  fvect         : result fvect1*constant
  -------------------------------------------------------------------------*/   
void LinearVectorScalarMultiply (float scalar, F_VECTOR *fvect1, F_VECTOR *fvect) {
  int                 numElements;/*length of feature vector*/
  int                i;/*temporary variable*/
  numElements = fvect1->numElements;
   //printf("%d\n",numElements);
  for (i = 0; i < numElements; i++)
    fvect->array[i] = fvect1->array[i]*scalar;
}

/*------------------------------------------------------------------------
  LinearVectorScalarDivide : Divides a vector by a scalar constant
  inputs :
    scalar        : float
    fvect1        : F_VECTOR 
  outputs :
    fvect         : result fvect1/constant
-------------------------------------------------------------------------*/   
void LinearVectorScalarDivide (float scalar, F_VECTOR *fvect1, F_VECTOR *fvect) {
  int                numElements;
  int                i;

  numElements = fvect1->numElements;
  for (i = 0; i < numElements; i++)
    fvect->array[i] = fvect1->array[i]/scalar;
}

/*******************************************************************************
AutoCorr : Compute Autocorrelation function of a signal
Inputs :
    signal        : float *
    npts          : integer
Outputs :
    autoCorr      : float *
****************************************************************************/
float *AutoCorr (float *signal, int npts) {
  static float         *autoCorr;
  int                  i, lim, j;
  static  int          flag = 0;
  if (flag == 0){ 
    autoCorr =  (float *)AllocFloatArray(autoCorr, npts+1);
    flag = 1;
  }
  for (i = 1; i <= npts; i++){
    autoCorr[i] = 0.0;
    lim = npts-i+1;
    for (j = 1; j <= lim ; j++)
      if (((j+i-1) >= 1) && ((j+i-1) <= npts))
	autoCorr[i] = autoCorr[i]+signal[j]*signal[j+i-1];
  }
  autoCorr[npts] = autoCorr[npts-1];
  return(autoCorr);
}


/*******************************************************************************
*	Durbin computes the Auto-Regressive coefficients coef of order m and the
*	reflection coefficients refCoef also of order m 
*       Inputs -
*	autoCorr - Autocorrelation coefficients
        order        - order of the prediction filter
        Outputs -
*	resEnergy - residual energy used by LpAnal for gain computation
*       coef      - AR coefficients
*       refCoef   - Reflection Coefficients
******************************************************************************/
void Durbin(float *autoCorr, int order, float *coef, float *refCoef, float *resEnergy){
  static float 		*b;
  float 		sum;
  int     	        i,i1,j,imj,ij1;
  float                 Ecopy;
  static int            flag = 0;
  if (flag == 0) { 
    b = (float *) AllocFloatArray(b,(order+2));
    flag = 1;
  }
  Ecopy = autoCorr[1];
  refCoef[1] = -autoCorr[2]/autoCorr[1];
  coef[1] = refCoef[1];
  Ecopy = (1.0-refCoef[1]*refCoef[1])*Ecopy;
  if ((order-1) > 0){
    for(i = 2; i <= order; i++){
      i1 = i-1;
      sum = 0.0;
      for (j = 1; j <= i1; j++){
	ij1 = i-j+1;
	sum = sum+coef[j]*autoCorr[ij1];
      }	   
      refCoef[i] = -(autoCorr[i+1]+sum)/Ecopy;
      coef[i] = refCoef[i];
      for (j = 1; j <= i; j++)
	b[j] = coef[j];
      for (j = 1; j <= i1; j++) {
	imj = i-j;
	coef[j] = coef[j]+refCoef[i]*b[imj];
      }
      Ecopy = Ecopy*(1.0-refCoef[i]*refCoef[i]);
    }
  }
  Ecopy = Ecopy/autoCorr[1];
  *resEnergy = Ecopy;
}
/****************************************************************************
   Normalize  - normalises data between 0 and 1            */
 void Normalize(float *signal, int npts)
 {
   int i;
   float minValue, maxValue, range;
 minValue = signal[Imin0Actual(signal, npts)];
 maxValue = signal[Imax0Actual(signal, npts)];
 range = (maxValue - minValue);
 if ((minValue == 0.0) && (maxValue == 0.0)) {
   printf("Error: all values zero\n");
   exit (-1);
 }
  for (i = 0; i < npts; i++) {
    signal[i] = (signal[i] - minValue)/range;
  }
 }
/****************************************************************************
   RemoveAverage  - removes dc component of a signal             */

 void RemoveAverage(float *derv, int nfft, float *ave)
 {
    int i;
 
    *ave=0.0;
    for(i=1;i<=nfft;i++) *ave+=derv[i];
    *ave = *ave/(float)nfft;
    for(i=1;i<=nfft;i++) derv[i]=derv[i]-*ave;
 }
/****************************************************************************
   Average  - Returns of average value of a signal             */

 float Average(float *signal, int npts)
 {
    int i;
    float ave;

    ave=0.0;
    for (i = 1; i <= npts ; i++) {
      ave = ave + signal[i];
    }
    ave = ave/(float)npts;
    return(ave);
 }
/****************************************************************************
   ComputeAverage  - Computes the average of a signal             */

 void ComputeAverage(float *signal, int npts, float *ave)
 {
    int i;
    
    *ave=0.0;
    for (i = 1; i <= npts ; i++) {
      *ave = *ave + signal[i];
    }
    *ave = *ave/(float)npts;
 }

/*******************************************************************************
*	LpAnal performs LP analysis on a signal

*	Inputs -
*	signal - array of length npts

*       outputs - 
*	resEnergy - residual signal of length npts
*	coef - array of AR coefficients of length order
*	gain - energy
**************************************************************************** */

void LpAnal(float *signal, float *resEnergy, int npts, int frameShift, 
	      float *coef, int order, float *gain) {

    static float 	*sw=NULL, *swOrg=NULL, *refCoef=NULL, ee;
    static float        *autoCorr;
    int 	        i,k,j,lim;
    static int          flag = 0;
    static int          order1;
    if (flag == 0) {
      sw = (float *) AllocFloatArray(sw, npts+order+1);
      swOrg = (float *) AllocFloatArray(swOrg, npts+order+1);
      order1 = order+1;
      autoCorr = (float *) calloc(order1+1, sizeof(float));
      refCoef = (float *) AllocFloatArray(refCoef,order1+1);
      flag = 1;
      for (i = 1; i <= order; i++) {
        sw[i] = 0;
        swOrg[i] = 0;
      }
    }
    for (i = 1; i <= npts; i++) {
      sw[i+order] = signal[i]*HamDw(i,npts);
      swOrg[i+order] = signal[i];
    }

    for (k = 1; k <= order1; k++){
      i = k - 1;
      autoCorr[k] = 0.0;
      lim = npts-i;
      for (j = 1; j <= lim; j++)
	autoCorr[k] = autoCorr[k]+sw[j+order]*sw[j+order+i];
    }
    Durbin(autoCorr, order, coef, refCoef, &ee);
    *gain = sqrt(ee*autoCorr[1]);
    /*        printf("gain = %f\n", *Gain); */
    for (j = 1; j <= npts; j++) {
       resEnergy[j] = swOrg[j+order];
      for (k = 1; k <= order; k++)  
	resEnergy[j] = resEnergy[j] + coef[k]*swOrg[j+order-k];
    }
    for (i = 1; i <= order; i++) {
      sw[i] = sw[frameShift+i];
      swOrg[i] = swOrg[frameShift+i];
    }
  }

/*-------------------------------------------------------------------------
 *  MASignal -- Generates an MA signal
 *    Args:	residual signal,  MASignal, numSamples
 *              coefZero, orderZ 
 *    Returns:	returns the signal generated in MASignal 
 *    Bugs:	
 * -------------------------------------------------------------------------*/
void MASignal(float *residual,float *MASignal,int numSamples,
		float *coefZero,int orderZ) {
	  int             i, j;
	  float           sum1=0, sum2=0;
	  //	  printf("OrderZ= %d\n", orderZ);
	  for (i = orderZ; i < numSamples+orderZ; i++){
	    //sum2 = residual[i];
	    //	    MASignal[i+orderZ] = residual[i+orderZ]; 
	    sum1 = 0;
	    if (orderZ != 0)
	      for (j = 0; j < orderZ; j++)
	      sum1 = sum1 +coefZero[j-1]*residual[i-j];
	    MASignal[i] = sum1+sum2;
	    //MASignal[i+orderZ] = MASignal[i+orderZ] + sum1;
	    //    printf("residual %d=%f MASignal %d= %f\n",
	    //	   i+orderZ,residual[i+orderZ], 
	    //	   i, MASignal[i]); 
	  }
	}	/*  End of MAsignal		End of MAsignal   */



/*****************************************************************************
 Routine LPCepstrum computes the Cepstrum directly from the LP Coefficients
 Input a contains the coefficients, order is the LP order 
 Inputs -
         a[] - ARcoeffs of the ARmodel whose Spectrum is reqd.
         order - LPorder (AR order)
         numCepstrum - number of Cepstrum
 Outputs -
        cepstrum - array of cepstrum coefficients

******************************************************************************/
void LPCepstrum(a, order, cepstrum, numCepstrum, gain)
int         order, numCepstrum;
float       a[], cepstrum[], gain;
{ int         k,m;
  float       sum = 0;

  cepstrum[0] = log (gain*gain);
  for (m = 1; m <= numCepstrum; m++){
    if (m <= order)
      sum = a[m];
    else
      sum = 0;
    for (k = 1; k <=(m-1); k++)
      sum = sum + cepstrum[k]*a[m-k]*(float)k/(float)m;
    cepstrum[m] = sum;
  }
}
/*****************************************************************************
 Routine LPSpectrum computes the LP magnitude and phase Spectrum from the
 AR coefficients. input a contains the coefficients, order is the LP order, 
 gain is the residual gain from lp analysis and nfft and mfft are the FFT order
 parameters (nfft = 2**mfft). Outputs are obtained in mag and phase arrays.

 Inputs -
         a[] - ARcoeffs of the ARmodel whose Spectrum is reqd.
         order - LPorder (AR order)
         nfft - DFT order (nfft - LPorder = no. of zeros to be padded) 
         mfft - nfft = 2**mfft
         gain - gain of the ARmodel (constant in the numerator)
 Outputs -
         mag[] - array of DFT magnitude
         phase[] - array of DFT phase

******************************************************************************/
void LPSpectrum(a, order, mag, phase, nfft, mfft, gain)
int              nfft,mfft,order;
float            a[],mag[],phase[],gain;
{
  static float          *ac, *ax, *ay;
  int                   i,nsign;
  static int            flag=0;
  
  if (flag == 0) {
    ac = (float *) AllocFloatArray(ac, nfft+1);
    ax = (float *) AllocFloatArray(ax, nfft+1);
    ay = (float *) AllocFloatArray(ay, nfft+1);
    flag = 1;
  }
  ac[1] = 1.0;
  for (i=1; i <= order; i++) 
    ac[i+1] = a[i];
  
  for (i=order+2;i<=nfft;i++) 
    ac[i] = 0;
  nsign = -1;
  Rfft(ac,ax,ay,mfft,nfft,nsign);
  SpectrumReal(nfft, ax, ay, mag, phase);
  /*  printf("gain = %f\n", gain); */
  fflush(stdout);
  for (i=1;i<=nfft;i++) {
    if (mag[i] != 0)
      mag[i]=(gain/mag[i]);
    else
      mag[i] = 1.0E-10;
     /*printf("magaft %d = %f\n",i,mag[i]); */
  }
}

/*****************************************************************************
 Routine LogSpectrum computes the Log Spectrum 
 of the input spectral coefficients (in place).

 Inputs -
         Spectrum[] - array of spectral coefficients
         npts - number of spectral coefficients) 

Outputs - Spectrum[] - contains the log Spectrum
******************************************************************************/
void LogSpectrum(float Spectrum[],int npts)
{
   int i;
   double temp;

   for(i=1; i<=npts; i++){
      temp = ((double)Spectrum[i]*(double)Spectrum[i]);
      if(temp < 0.00000001){
         temp = 0.00000001;
         puts("\n WARNING from LogSpectrum: Squared magn < 0.00000001");
      }
      Spectrum[i] = (float)(10.0*log10(temp));
   }
}

/************************************************************************
  LogCepSmooth smoothes magnitude amag to give smthAmag using the log cepstrum
          nfft - fft order
          2**mfft = nfft. winlen is Window size.

  Inputs -
          amag[] - array of nfft spectral coefficients
          nfft - FFT order
          mfft - 2**mfft = nfft
	  winlen - cepstral Window length
  Outputs -
          smthAmag[] - smoothed Spectrum of nfft coefficients
	  co - c0 cepstral value
*************************************************************************/
void LogCepSmooth(float amag[], float smthAmag[], int mfft, int nfft,  
		  int winlen,float *c0, float gamma)
{
  static              int flag = 0;
  static float        *resAx, *resAy, *resAxcop,
                      *resAmag, *resPhase;
  int i;
  if (flag == 0) {
    resAx = (float *) AllocFloatArray(resAx, nfft+1);
    resAy = (float *) AllocFloatArray(resAy, nfft+1);
    resAxcop = (float *) AllocFloatArray(resAxcop, nfft+1);
    resAmag = (float *) AllocFloatArray(resAmag, nfft+1);
    resPhase = (float *) AllocFloatArray(resPhase, nfft+1);
    flag = 1;
  } 
 *c0 = 0;
 for ( i=1; i<=nfft; i++ ) {
   if (amag[i] <= 1E-10) 
     resAmag[i] = 1E-10;
   else
     resAmag[i] = gamma*log(amag[i]);
   *c0 = *c0 + resAmag[i]*resAmag[i];
 }
 *c0 = sqrt(*c0/nfft);
 Rfft(resAmag, resAx, resAy, mfft, nfft, 1);

 for ( i=2; i<=winlen; i++ )
    {
     resAxcop[i] = resAx[i]*HanW(i,winlen);
     resAxcop[nfft-i+2] = resAxcop[i];
    }
 resAxcop[1] = resAx[1];
 for(i=winlen+1;i<=nfft-winlen+1;i++) 
   resAxcop[i] = 0.0;
 
 Rfft(resAxcop, resAmag, resAy, mfft, nfft, -1);
 SpectrumReal(nfft, resAmag, resAy, smthAmag, resPhase);
 for (i = 1; i <= nfft; i++)
   smthAmag[i] = exp(smthAmag[i]);
}


/************************************************************************
  CepSmooth smoothes magnitude amag to give smthAmag. Order nfft.
              2**mfft = nfft. winlen is Window size.

  Inputs -
          amag[] - array of nfft spectral coefficients
          nfft - FFT order
          mfft - 2**mfft = nfft
	  winlen - cepstral Window length
  Outputs -
          smthAmag[] - smoothed Spectrum of nfft coefficients
	  co - c0 cepstral value
*************************************************************************/
void CepSmooth(float amag[], float smthAmag[], int mfft, int nfft,  
	       int winlen,float *c0, float gamma)
{
  static              int flag = 0;
  static float        *resAx, *resAy, *resAxcop,
                      *resAmag, *resPhase;
  int i;
  if (flag == 0) {
    resAx = (float *) AllocFloatArray(resAx, nfft+1);
    resAy = (float *) AllocFloatArray(resAy, nfft+1);
    resAxcop = (float *) AllocFloatArray(resAxcop, nfft+1);
    resAmag = (float *) AllocFloatArray(resAmag, nfft+1);
    resPhase = (float *) AllocFloatArray(resPhase, nfft+1);
    flag = 1;
  } 
 *c0 = 0;
 for ( i=1; i<=nfft; i++ ) {
   *c0 = *c0 + amag[i]*amag[i];
   if (amag[i] <= 0.000001) 
     resAmag[i] = 0.000001;
   else
     resAmag[i] = exp(gamma*log(amag[i]));
 }
 *c0 = sqrt(*c0/nfft);
 Rfft(resAmag, resAx, resAy, mfft, nfft, 1);

 for ( i=2; i<=winlen; i++ )
    {
     resAxcop[i] = resAx[i]*HanW(i,winlen);
     resAxcop[nfft-i+2] = resAxcop[i];
    }
 resAxcop[1] = resAx[1];
 for(i=winlen+1;i<=nfft-winlen+1;i++) 
   resAxcop[i] = 0.0;
 
 Rfft(resAxcop, resAmag, resAy, mfft, nfft, -1);
 SpectrumReal(nfft, resAmag, resAy, smthAmag, resPhase);
}

/*****************************************************************
 *  function GeneratePseudoDct generates the discrete cosine
transform 
Inputs - numRows and numColumns
outputs - VECTOR_OF_F_VECTORS melCosineTransform
*******************************************************************/

VECTOR_OF_F_VECTORS  *GeneratePseudoDct (int offset, int numRows, int numColumns)
{
  int                     i, j;
  VECTOR_OF_F_VECTORS     *vfv=NULL;
  float                   *list=NULL;
  float                    normalisingConst;
  vfv = (VECTOR_OF_F_VECTORS *) calloc(numRows, sizeof(VECTOR_OF_F_VECTORS));
  if (vfv == NULL) {
    printf("unable to allocate vfv array \n");
    fflush(stdout);
    _exit(-1);
  }
  normalisingConst = sqrt(2.0/(float)(numColumns));
  for (i = offset; i <= offset+numRows-1; i++){
    vfv[i-offset] = (F_VECTOR *) malloc(1*sizeof(F_VECTOR));
    if (vfv[i-offset] == NULL) {
      printf("unable to allocate vfv FVector\n");
      fflush(stdout);
      _exit(-1);
    }
    list = AllocFloatArray(list,numColumns);
    fflush(stdout);
    vfv[i-offset]->numElements = numColumns;
    for (j = 1; j <= numColumns; j++){
      //      vfv[i-offset]->array[j-1] = (double) cos (PI * (i - 0.5)*(j-1)/numColumns)*normalisingConst;
      list[j-1] = (double) cos (PI * (i - 0.5)*(j-1)/numColumns)*normalisingConst;
    }
    vfv[i-offset]->array = list;
  }
  return(vfv);
}
/***********************************************************************
  
  function Warp Warps an input frequency fin

  Inputs -  fin - input frequency
            minFrequency, maxFrequency - range of frequency scale
	    WarpConstant - Warping constant (0 - 1.0, 0 => no Warping
	                                               1 => heavy Warping
   Outputs - the Warped frequency
**********************************************************************/

float Warp (float fin, float minFrequency, float maxFrequency, 
	    float warpConstant)
{ 
  float                warpedFrequency;
  float                range,value;

  range = maxFrequency - minFrequency;
  value = (fin - minFrequency)*PI/range;
  warpedFrequency = fin + range *(2.0/PI)* 
    (atan(-warpConstant*sin(value)
	  /(1 + warpConstant *cos(value))));
  return(warpedFrequency); 
}

/************************************************************************
TrapezoidalFilter outputs the weights for the trapezoidal filter

     Inputs :
     startFreq - startFrequency of filter
     endFreq   - endFrequency of filter
     fin - input frequency
     trapRatio - trapezoidal ratio

***********************************************************************/

 float TrapezoidalFilter(float startFreq, float endFreq, 
			  float fin, float trapRatio) {
   
   float                  limits,slope;
   float                  weight;
   float                  leftLim, rightLim;

   float centreFreq = startFreq + fabs(endFreq - startFreq)/2.0;
   limits = (centreFreq - startFreq)*(1-trapRatio);
   if(limits != 0){
     slope = 1/limits;
     leftLim = startFreq + limits;
     rightLim = endFreq - limits;
     if (fin < leftLim) 
       weight = (fin-startFreq)*slope;
     else if (fin > rightLim)
       weight = (endFreq-fin)*slope;
     else weight = 1;
   }
   else weight = 1;
   return(weight);
 }
/**********************************************************************
  TrinomialMult - multiplies a series of trinomials together and returns
  the coefficients of the resulting polynomial.
  
  The multiplication has the following form:

  (x^2 + b[0]x + c[0])*(x^2 + b[1]x + c[1])*...*(x^2 + b[n-1]x + c[n-1])

  The b[i] and c[i] coefficients are assumed to be complex and are passed
  to the function as a pointers to arrays of doubles of length 2n. The real
  part of the coefficients are stored in the even numbered elements of the
  array and the imaginary parts are stored in the odd numbered elements.

  The resulting polynomial has the following form:
  
  x^2n + a[0]*x^2n-1 + a[1]*x^2n-2 + ... +a[2n-2]*x + a[2n-1]
  
  The a[i] coefficients can in general be complex but should in most cases
  turn out to be real. The a[i] coefficients are returned by the function as
  a pointer to an array of doubles of length 4n. The real and imaginary
  parts are stored, respectively, in the even and odd elements of the array.
  Storage for the array is allocated by the function and should be freed by
  the calling program when no longer needed.
  
  Function arguments:
  
  n  -  The number of trinomials to multiply
  b  -  Pointer to an array of doubles of length 2n.
  c  -  Pointer to an array of doubles of length 2n.
*/

double *TrinomialMult( int n, double *b, double *c )
{
  int i, j;
  double *a;

  a = (double *)calloc( 4 * n, sizeof(double) );

  a[2] = c[0];
  a[3] = c[1];
  a[0] = b[0];
  a[1] = b[1];
  
  for( i = 1; i < n; ++i )
  {
    a[2*(2*i+1)]   += c[2*i]*a[2*(2*i-1)]   - c[2*i+1]*a[2*(2*i-1)+1];
    a[2*(2*i+1)+1] += c[2*i]*a[2*(2*i-1)+1] + c[2*i+1]*a[2*(2*i-1)];

    for( j = 2*i; j > 1; --j )
    {
      a[2*j]   += b[2*i] * a[2*(j-1)]   - b[2*i+1] * a[2*(j-1)+1] + 
                  c[2*i] * a[2*(j-2)]   - c[2*i+1] * a[2*(j-2)+1];
      a[2*j+1] += b[2*i] * a[2*(j-1)+1] + b[2*i+1] * a[2*(j-1)] +
                  c[2*i] * a[2*(j-2)+1] + c[2*i+1] * a[2*(j-2)];
    }

    a[2] += b[2*i] * a[0] - b[2*i+1] * a[1] + c[2*i];
    a[3] += b[2*i] * a[1] + b[2*i+1] * a[0] + c[2*i+1];
    a[0] += b[2*i];
    a[1] += b[2*i+1];
  }

  return( a );
}


float *BandPassFilter( float lowFrequency, float highFrequency, int samplingRate, int  numCoef)
{
  int i, nc, nz;
  double d1, d2;
  double fc1, fc2, h;
  float  *filterCoefs;
  /*  if( argc < 6 )
  {
  
    printf("\ntdbpf creates a file of band pass filter coefficients.\n");
    printf("\nUsage: tdbpf outfile fc1 fc2 nc nz\n");
    printf("  outfile = output file\n");
    printf("  fc1 = lower frequency as a fraction of PI [0,1]\n");
    printf("  fc2 = upper frequency as a fraction of PI [0,1]\n");    
    printf("  nc = number of coefficients\n");
    printf("  nz = number of zeros to pad\n");
    return(-1);
    }*/

  /*fp = fopen(argv[1], "w");
  if( fp == 0 )
  {
    perror( "Unable to open outfile" );
    return(-1);
    }*/

  fc1 = 2*lowFrequency/samplingRate* M_PI;
  fc2 = 2*highFrequency/samplingRate* M_PI;
  nc = numCoef;
  nz = 0;

  d1 = ((double)nc - 1.0)/2.0;
  filterCoefs = (float *) malloc ((numCoef)*sizeof(float));
  for( i = 0; i < nc; ++i )
  {
    d2 = (double)i - d1;
    h = d2 == 0 ? (fc2 - fc1) / M_PI : (sin(fc2 * d2) - sin(fc1 * d2)) / (M_PI * d2);
    printf ("h = %f\n", h);
    filterCoefs[i] = h;
    //    fprintf( fp, "%1.15lf\n", h );
  }

  /*for( i = 0; i < nz; ++i )
    fprintf( fp, "0.0\n" );
  
    fclose( fp );*/
  return(filterCoefs);
}

void ButterWorthFilter( float minFrequency, float maxFrequency, int samplingRate, int filterOrder, float *numerator, float *denominator)
{
  int i, n;
  double fcd, fca;
  double fc1, fc2;
  double *p;
  double *d;
  double pmr, pmi;
  double ppr, ppi;
  double ppmr, ppmi;
  double d1, d2;
  double ad, bd;
  double *r, *q;
  double bpr, bpi;
  double pbpr, pbpi;
  double sf;
  n = filterOrder;
  fc1 = 2*M_PI*minFrequency/(float) samplingRate;
  fc2 = 2*M_PI*maxFrequency/(float) samplingRate;
  //numerator = (float *) malloc ((2*n+1)*sizeof(float));
  //denominator =(float *) malloc ((2*n+1)*sizeof(float));
  fcd = 0.5 * M_PI;
  fca = tan( fcd / 2.0);
  p = (double *)calloc( 2 * n, sizeof(double) );

  /* Calculate the analog Butterworth coefficients */
  for( i = 0; i < n; ++i )
  {
    p[2*i] = -fca * sin(M_PI*((double)i+0.5)/(double)n);
    p[2*i+1] = fca * cos(M_PI*((double)i+0.5)/(double)n);
  }

  ppmr = 1.0;
  ppmi = 0.0;

  for( i = 0; i < n; ++i )
  {
    pmr = p[2*i] - 1.0;
    pmi = p[2*i+1];
    ppr = p[2*i] + 1.0;
    ppi = p[2*i+1];
    
    d1 = pmr * pmr + pmi * pmi;
    p[2*i] = (ppr * pmr + ppi * pmi) / d1;
    p[2*i+1] = (ppi * pmr - ppr * pmi) / d1;

    d1 = ppmr;
    d2 = ppmi;
    ppmr = -( d1 * pmr - d2 * pmi );
    ppmi = -( d1 * pmi + d2 * pmr );
  }

  ppmr = pow( fca, (double)(2*n) ) / ppmr; /* scaling factor */

  d1 = cos( (fc2 + fc1)/2.0 ) / cos( (fc2 - fc1)/2.0 );
  d2 = tan( fcd / 2.0 ) * cos( (fc2 - fc1)/2.0 ) / sin( (fc2 - fc1)/2.0 );
  ad = 2.0 * d1 * d2 / (d2 + 1.0);
  bd = (d2 - 1.0)/(d2 + 1.0);

  r = (double *)calloc( 2 * n, sizeof(double) );
  q = (double *)calloc( 2 * n, sizeof(double) );  

  pbpr = 1.0;
  pbpi = 0.0;

  for( i = 0; i < n; ++i )
  {
    bpr = 1.0 - bd * p[2*i];
    bpi = -bd * p[2*i+1];
    pmr = bd - p[2*i];
    pmi = -p[2*i+1];
    d1 = bpr * bpr + bpi * bpi;
    r[2*i] = (pmr * bpr + pmi * bpi) / d1;
    r[2*i+1] = (pmi * bpr - pmr * bpi) / d1;
    pmr = ad * (p[2*i] - 1.0);
    pmi = ad * p[2*i+1];
    q[2*i] = (pmr * bpr + pmi * bpi) / d1;
    q[2*i+1] = (pmi * bpr - pmr * bpi) / d1;

    d1 = pbpr;
    d2 = pbpi;
    pbpr = d1 * bpr - d2 * bpi;
    pbpi = d1 * bpi + d2 * bpr;
  }

  pbpr = pow( (bd - 1.0), (double)n ) / pbpr; /* scaling factor */
  sf = ppmr * pbpr;

  d = TrinomialMult( n, q, r );
  numerator [0] = sf;
  numerator [1] = 0;
  d1 = -(double)n;
  d2 = 1.0;

  for( i = 1; i <= n; ++i )
  {
    numerator[2*i] = (float ) sf*d1/d2;
    if( i < n ) {
      numerator[2*i+1] = 0; 
    }
    d1 *= -(double)(n - i);
    d2 *= (double)(i + 1);
  }

  denominator[0] = 1;
  for( i = 0; i < 2*n; ++i )
  {
    denominator[i+1] = (float ) d[2*i];
  }
  free( p );
  free( r );
  free( q );
  free( d );
}

/***********************************************************************

GenerateMelFilters generates a set of melFilters

      Inputs : asdf - the structure containing all the relevant
               input information
      Outputs : filterbank is stored in asdf itself

**********************************************************************/

void GenerateMelFilters (ASDF *asdf)
{
 VECTOR_OF_F_VECTORS           *filterbankWeights;
 int                           *dftIndices;
 float                         frequencyInc;
 float                         dftLineFrequency;
 int                           minDftFrequency,maxDftFrequency, fftSizeBy2;
 float                         *startFrequency,*endFrequency,
                               *centreFrequency;
 int                           *startIndex,*endIndex,*centreIndex;
 int                           i,j, numElem;

 dftIndices = (int *) calloc(asdf->numFilters, sizeof(int));
 if (dftIndices == NULL) {
     printf("problems allocating dftIndices\n");
   fflush(stdout);
   _exit(-1);
 }
 frequencyInc = (asdf->maxFrequency - asdf->minFrequency)/(asdf->numFilters+1);
 dftLineFrequency = (int) asdf->samplingRate/asdf->fftSize;
 fftSizeBy2 = asdf->fftSize/2;
 minDftFrequency = (int) asdf->minFrequency/dftLineFrequency;
 maxDftFrequency = (int) asdf->maxFrequency/dftLineFrequency;
 startFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
 endFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
 centreFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
 
 startIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 endIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 centreIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 for (i = 0; i < asdf->numFilters; i++) 
   centreFrequency[i] = asdf->minFrequency + (i+1)*frequencyInc;
 
 if (asdf->bandwidthScale == 0.0)
   for (i = 0; i < asdf->numFilters; i++) {
     startFrequency[i] =centreFrequency[i] - frequencyInc;
     endFrequency[i] = centreFrequency[i] + frequencyInc;
     /*      printf("sf = %f cf = %f ef = %f\n", startFrequency[i], centreFrequency[i], endFrequency[i]); */
   } else
     for (i = 0; i < asdf->numFilters; i++) {
       if (centreFrequency[i] < 1000) {
	 startFrequency[i] = centreFrequency[i]- asdf->bandwidthScale*137.50;
	 endFrequency[i] = centreFrequency[i] + asdf->bandwidthScale*137.50;
       } else if ((centreFrequency[i] >= 1000) && 
		  (centreFrequency[i] <= 0.4*asdf->samplingRate)){
	 startFrequency[i] = centreFrequency[i] - asdf->bandwidthScale*1.11*
	   (exp(log(centreFrequency[i])*0.7));
	 endFrequency[i] = centreFrequency[i] + asdf->bandwidthScale*1.11*
	   (exp(log(centreFrequency[i])*0.7));
       } else {
	 startFrequency[i] = centreFrequency[i] - asdf->bandwidthScale*10.84*
	   (exp(log(centreFrequency[i])*0.4));
	 endFrequency[i] = centreFrequency[i] + asdf->bandwidthScale*10.84*
	   (exp(log(centreFrequency[i])*0.4));
       }
       /*      printf("sf = %f cf = %f ef = %f\n", startFrequency[i], centreFrequency[i], endFrequency[i]); */
     }
 for (i = 0; i < asdf->numFilters; i++) {
   startIndex[i] = (int) (Warp(startFrequency[i],asdf->minFrequency,
				asdf->maxFrequency,
			       asdf->warpConst)/dftLineFrequency);
   endIndex[i] = (int ) (Warp(endFrequency[i],asdf->minFrequency,
			       asdf->maxFrequency,
			      asdf->warpConst)/dftLineFrequency);
   centreIndex[i] = (int)(Warp(centreFrequency[i], asdf->minFrequency,
				asdf->maxFrequency,
			       asdf->warpConst)/dftLineFrequency);
   if (startIndex[i] > fftSizeBy2) startIndex[i] = fftSizeBy2;
   if (centreIndex[i] > fftSizeBy2) centreIndex[i] = fftSizeBy2;
   if (endIndex[i] > fftSizeBy2) endIndex[i] = fftSizeBy2;

   if (startIndex[i] < 1) startIndex[i] = 1;
   if (centreIndex[i] < 1) centreIndex[i] = 1;
   if (endIndex[i] < 1) endIndex[i] = 1;

 }
 filterbankWeights = (VECTOR_OF_F_VECTORS *) 
   malloc(asdf->numFilters*sizeof(VECTOR_OF_F_VECTORS));
 if (filterbankWeights == NULL) {
   printf("problems allocating filterbankWeights");
   _exit(-1);
 }
 for (i = 0; i < asdf->numFilters; i++) {
   numElem = endIndex[i] - startIndex[i]+1;
   filterbankWeights[i] = (F_VECTOR *) AllocFVector(numElem);
   dftIndices[i] = startIndex[i];
   for (j = startIndex[i]; j<= endIndex[i]; j++){
     filterbankWeights[i]->array[j-startIndex[i]] = 
       TrapezoidalFilter(startIndex[i], endIndex[i], j, 
			 asdf->trapezoidalRatio);
     /*     printf("fbWts %d %d = %f\n",i, 
	    j-startIndex[i],filterbankWeights[i]->array[j-startIndex[i]]); */
   }
   
 }
 asdf->filterbankWeights = filterbankWeights;
 asdf->dftIndices = dftIndices;
 free(startFrequency);
 free(endFrequency);
 free(centreFrequency);
 free(startIndex);
 free(centreIndex);
 free(endIndex);
}

/***********************************************************************

GenerateNonUniformCentFilters generates a set of centFilters

      Inputs : asdf - the structure containing all the relevant
               input information
      Outputs : filterbank is stored in asdf itself

**********************************************************************/

void GenerateNonUniformCentFilters (ASDF *asdf)
{
 VECTOR_OF_F_VECTORS           *filterbankWeights;
 int                           *dftIndices;
 float                         frequencyInc, minF, maxF;
 float                         dftLineFrequency;
 int                           minDftFrequency,maxDftFrequency, fftSizeBy2;
 float                         *startFrequency,*endFrequency,
                               *centreFrequency;
 int                           *startIndex,*endIndex,*centreIndex;
 int                           i,j, numElem;

 dftIndices = (int *) calloc(asdf->numFilters, sizeof(int));
 if (dftIndices == NULL) {
     printf("problems allocating dftIndices\n");
   fflush(stdout);
   _exit(-1);
 }
 maxF = ConvertCentToFreq(asdf->maxFrequency, asdf->tonic);
 minF = ConvertCentToFreq(asdf->minFrequency, asdf->tonic);
 frequencyInc = (maxF-minF)/(asdf->numFilters+1);
 dftLineFrequency = (int) asdf->samplingRate/asdf->fftSize;
 fftSizeBy2 = asdf->fftSize/2;
 //minDftFrequency = (int) (ConvertCentToFreq(asdf->minFrequency, asdf->tonic)/dftLineFrequency);
 //maxDftFrequency = (int) (ConvertCentToFreq(asdf->maxFrequency, asdf->tonic)/dftLineFrequency);
 startFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
 endFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
 centreFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
 
 startIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 endIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 centreIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 centreFrequency[0] = ConvertCentToFreq(asdf->minFrequency, asdf->tonic) + frequencyInc;
 for (i = 1; i < asdf->numFilters; i++) 
   centreFrequency[i] = centreFrequency[i-1] + frequencyInc;
 for (i = 0; i < asdf->numFilters; i++) {
   startFrequency[i] =centreFrequency[i] - frequencyInc;
   endFrequency[i] = centreFrequency[i] + frequencyInc;
 }
 for (i = 0; i < asdf->numFilters; i++) {
   startIndex[i] = (int) (startFrequency[i]/dftLineFrequency);
   endIndex[i] = (int ) (endFrequency[i]/dftLineFrequency);
   centreIndex[i] = (int)(centreFrequency[i]/dftLineFrequency);
   if (startIndex[i] < 1) startIndex[i] = 1;
   if (centreIndex[i] < 1) centreIndex[i] = 1;
   if (endIndex[i] < 1) endIndex[i] = 1;
   if (startIndex[i] > fftSizeBy2) startIndex[i] = fftSizeBy2;
   if (centreIndex[i] > fftSizeBy2) centreIndex[i] = fftSizeBy2;
   if (endIndex[i] > fftSizeBy2) endIndex[i] = fftSizeBy2;
   }
 filterbankWeights = (VECTOR_OF_F_VECTORS *) 
   malloc(asdf->numFilters*sizeof(VECTOR_OF_F_VECTORS));
 if (filterbankWeights == NULL) {
   printf("problems allocating filterbankWeights");
   _exit(-1);
 }
 for (i = 0; i < asdf->numFilters; i++) {
   numElem = endIndex[i] - startIndex[i]+1;
   filterbankWeights[i] = (F_VECTOR *) AllocFVector(numElem);
   dftIndices[i] = startIndex[i];
   for (j = startIndex[i]; j<= endIndex[i]; j++){
     filterbankWeights[i]->array[j-startIndex[i]] = 
       TrapezoidalFilter(startIndex[i], endIndex[i], j, 
			 asdf->trapezoidalRatio);
     /*     printf("fbWts %d %d = %f\n",i, 
	    j-startIndex[i],filterbankWeights[i]->array[j-startIndex[i]]); */
   }
   
 }
 asdf->filterbankWeights = filterbankWeights;
 asdf->dftIndices = dftIndices;
 free(startFrequency);
 free(endFrequency);
 free(centreFrequency);
 free(startIndex);
 free(centreIndex);
 free(endIndex);
}
/***********************************************************************

GenerateCentNormFFTFilters generates a set of centNormFFTFilters coefficients

      Inputs : asdf - the structure containing all the relevant
               input information
      Outputs : filterbank is stored in asdf itself -- each filter has only FFT coefficient

**********************************************************************/

void GenerateCentNormFFTFilters (ASDF *asdf)
{
 VECTOR_OF_F_VECTORS           *filterbankWeights;
 int                           *dftIndices;
 float                         frequencyInc, minF, maxF;
 float                         dftLineFrequency;
 int                           fftSizeBy2;
 float                         *centreFrequency;
 int                           *startIndex,*endIndex,*centreIndex;
 int                           i,j, numElem;

 dftIndices = (int *) calloc(asdf->fftSize, sizeof(int));
 if (dftIndices == NULL) {
     printf("problems allocating dftIndices\n");
   fflush(stdout);
   _exit(-1);
 }
 maxF = ConvertCentToFreq(asdf->maxFrequency, asdf->tonic);
 minF = ConvertCentToFreq(asdf->minFrequency, asdf->tonic);
 dftLineFrequency = (int) asdf->samplingRate/asdf->fftSize;
 asdf->numFilters = ceilf((maxF - minF)/dftLineFrequency);
 dftIndices = (int *) calloc(asdf->numFilters, sizeof(int));
 if (dftIndices == NULL) {
     printf("problems allocating dftIndices\n");
   fflush(stdout);
   _exit(-1);
 }
 frequencyInc = dftLineFrequency;
fftSizeBy2 = asdf->fftSize/2;
 //minDftFrequency = (int) (ConvertCentToFreq(asdf->minFrequency, asdf->tonic)/dftLineFrequency);
 //maxDftFrequency = (int) (ConvertCentToFreq(asdf->maxFrequency, asdf->tonic)/dftLineFrequency);
 centreFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
  centreIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 centreFrequency[0] = ConvertCentToFreq(asdf->minFrequency, asdf->tonic) + frequencyInc;
 for (i = 1; i < asdf->numFilters; i++) 
   centreFrequency[i] = centreFrequency[i-1] + frequencyInc;
 /* for (i = 0; i < asdf->numFilters; i++) {
   startFrequency[i] =centreFrequency[i] - frequencyInc;
   endFrequency[i] = centreFrequency[i] + frequencyInc;
   }*/
 for (i = 0; i < asdf->numFilters; i++) {
   centreIndex[i] = (int)(centreFrequency[i]/dftLineFrequency);
 }
 filterbankWeights = (VECTOR_OF_F_VECTORS *) 
  malloc(asdf->numFilters*sizeof(VECTOR_OF_F_VECTORS));
 if (filterbankWeights == NULL) {
   printf("problems allocating filterbankWeights");
   _exit(-1);
   }
 for (i = 0; i < asdf->numFilters; i++) {
   numElem = 1;
   dftIndices[i] = centreIndex[i];
   filterbankWeights[i] = (F_VECTOR *) AllocFVector(numElem);
   filterbankWeights[i]->array[0] = 1.0;
   /*     printf("fbWts %d %d = %f\n",i, 
	  j-startIndex[i],filterbankWeights[i]->array[j-startIndex[i]]); */
 }
   
 asdf->filterbankWeights = filterbankWeights;
 asdf->dftIndices = dftIndices;
  free(centreFrequency);
  free(centreIndex);
 }
/***********************************************************************
ConvertFreqToCent -- converts frequency to cent scale given tonic

      Inputs : frequency, tonic
      Outputs : Converted cent frequency

***********************************************************************/
float ConvertFreqToCent (float frequency, float tonic) 
{
  float value;
  value = 1200*log (frequency/tonic)/log(2.0);
  return (value);
}
/***********************************************************************
ConvertCentToFreq -- converts cent frequency to freq scale given tonic

      Inputs : cent frequency, tonic
      Outputs : Converted frequency

***********************************************************************/
float ConvertCentToFreq(float centFrequency, float tonic) 
{
  float value;
  value = exp(centFrequency*logf(2.0)/1200)*tonic;
  return(value);
  }
/***********************************************************************

GenerateUniformCentFilters generates a set of Filters in the cent scale

      Inputs : asdf - the structure containing all the relevant
               input information
      Outputs : filterbank is stored in asdf itself

**********************************************************************/

void GenerateUniformCentFilters (ASDF *asdf)
{
 VECTOR_OF_F_VECTORS           *filterbankWeights;
 int                           *dftIndices;
 float                         frequencyInc;
 float                         dftLineFrequency;
 int                           minDftFrequency,maxDftFrequency;
 float                         *startFrequency,*endFrequency,
                               *centreFrequency;
 int                           *startIndex,*endIndex,*centreIndex;
 int                           i,j, numElem;
 FILE                          *filterFreq=NULL;

 dftIndices = (int *) calloc(asdf->numFilters, sizeof(int));
 if (dftIndices == NULL) {
     printf("problems allocating dftIndices\n");
   fflush(stdout);
   _exit(-1);
 }
   filterFreq = fopen("FilterFreq.txt", "w");
 frequencyInc = (asdf->maxFrequency - asdf->minFrequency)/(asdf->numFilters+1);
 dftLineFrequency = (int) asdf->samplingRate/asdf->fftSize;
 minDftFrequency = (int) (ConvertCentToFreq(asdf->minFrequency, asdf->tonic)/dftLineFrequency);
 maxDftFrequency = (int) (ConvertCentToFreq(asdf->maxFrequency, asdf->tonic)/dftLineFrequency);
 startFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
 endFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
 centreFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
 startIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 endIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 centreIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 for (i = 0; i < asdf->numFilters; i++) 
   centreFrequency[i] = asdf->minFrequency + (i+1)*frequencyInc;
 for (i = 0; i < asdf->numFilters; i++) {
   startFrequency[i] =centreFrequency[i] - frequencyInc;
   endFrequency[i] = centreFrequency[i] + frequencyInc;
   /*      printf("sf = %f cf = %f ef = %f\n", startFrequency[i], centreFrequency[i], endFrequency[i]); */
 } 
			  
 for (i = 0; i < asdf->numFilters; i++) {
   startIndex[i] = (int) (ConvertCentToFreq(startFrequency[i], asdf->tonic)/dftLineFrequency);
   endIndex[i] = (int ) (ConvertCentToFreq(endFrequency[i],asdf->tonic)/dftLineFrequency);
   centreIndex[i] = (int)(ConvertCentToFreq(centreFrequency[i], asdf->tonic)/dftLineFrequency);
   fprintf (filterFreq, "%f %f %f %f %f %f\n",  (float) startIndex[i]/asdf->fftSize*asdf->samplingRate, startFrequency[i], 
	    (float) centreIndex[i]/asdf->fftSize*asdf->samplingRate, centreFrequency[i], 
	    (float) endIndex[i]/asdf->fftSize*asdf->samplingRate, endFrequency[i]);

 }
 fclose(filterFreq);
 filterbankWeights = (VECTOR_OF_F_VECTORS *) 
   malloc(asdf->numFilters*sizeof(VECTOR_OF_F_VECTORS));
 if (filterbankWeights == NULL) {
   printf("problems allocating filterbankWeights");
   _exit(-1);
 }
 for (i = 0; i < asdf->numFilters; i++) {
   numElem = endIndex[i] - startIndex[i]+1;
   filterbankWeights[i] = (F_VECTOR *) AllocFVector(numElem);
   dftIndices[i] = startIndex[i];
   for (j = startIndex[i]; j<= endIndex[i]; j++){
     filterbankWeights[i]->array[j-startIndex[i]] = 
       TrapezoidalFilter(startIndex[i], endIndex[i], j, 
			 asdf->trapezoidalRatio);
     /*     printf("fbWts %d %d = %f\n",i, 
	    j-startIndex[i],filterbankWeights[i]->array[j-startIndex[i]]); */
   }
   
 }
 asdf->filterbankWeights = filterbankWeights;
 asdf->dftIndices = dftIndices;
 free(startFrequency);
 free(endFrequency);
 free(centreFrequency);
 free(startIndex);
 free(centreIndex);
 free(endIndex);
}

/***********************************************************************

GenerateChromaFilters generates a set of Filters in the Chroma scale

      Inputs : asdf - the structure containing all the relevant
               input information
      Outputs : filterbank is stored in asdf itself

**********************************************************************/

void GenerateChromaFilters (ASDF *asdf)
{
 VECTOR_OF_F_VECTORS           *filterbankWeights;
 int                           *dftIndices;
 float                         frequencyInc;
 float                         dftLineFrequency;
 int                           minDftFrequency,maxDftFrequency;
 float                         *startFrequency,*endFrequency,
                               *centreFrequency;
 int                           *startIndex,*endIndex,*centreIndex, tonicIndex;
 int                           i,j, numElem;

 dftIndices = (int *) calloc(asdf->numFilters, sizeof(int));
 if (dftIndices == NULL) {
     printf("problems allocating dftIndices\n");
   fflush(stdout);
   _exit(-1);
 }
 if (asdf->chromaOverlapFB == 0)
   frequencyInc = (1200.0)/((asdf->numFilters)*2);
 else
   frequencyInc = (1200.0)/asdf->numFilters;
 dftLineFrequency = (int) asdf->samplingRate/asdf->fftSize;
 minDftFrequency = (int) (ConvertCentToFreq(0, asdf->tonic)/dftLineFrequency);
 maxDftFrequency = (int) (ConvertCentToFreq(1200, asdf->tonic)/dftLineFrequency);
 startFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
 endFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
 centreFrequency = (float *) calloc(asdf->numFilters, sizeof(float));
 startIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 endIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 centreIndex = (int *) calloc(asdf->numFilters, sizeof(int));
 tonicIndex = asdf->tonic/dftLineFrequency;
 for (i = 0; i < asdf->numFilters; i++) 
   centreFrequency[i] = (i+1)*frequencyInc;
 for (i = 0; i < asdf->numFilters; i++) {
   startFrequency[i] =centreFrequency[i] - frequencyInc;
   endFrequency[i] = centreFrequency[i] + frequencyInc;
   /*      printf("sf = %f cf = %f ef = %f\n", startFrequency[i], centreFrequency[i], endFrequency[i]); */
 } 
			  
 for (i = 0; i < asdf->numFilters; i++) {
   startIndex[i] = (int) (ConvertCentToFreq(startFrequency[i], asdf->tonic)/dftLineFrequency) - tonicIndex;
   endIndex[i] = (int ) (ConvertCentToFreq(endFrequency[i],asdf->tonic)/dftLineFrequency) - tonicIndex;
   centreIndex[i] = (int)(ConvertCentToFreq(centreFrequency[i], asdf->tonic)/dftLineFrequency) - tonicIndex;
 }
 filterbankWeights = (VECTOR_OF_F_VECTORS *) 
   malloc(asdf->numFilters*sizeof(VECTOR_OF_F_VECTORS));
 if (filterbankWeights == NULL) {
   printf("problems allocating filterbankWeights");
   _exit(-1);
 }
 for (i = 0; i < asdf->numFilters; i++) {
   numElem = endIndex[i] - startIndex[i]+1;
   filterbankWeights[i] = (F_VECTOR *) AllocFVector(numElem);
   dftIndices[i] = startIndex[i];
   for (j = startIndex[i]; j<= endIndex[i]; j++){
     filterbankWeights[i]->array[j-startIndex[i]] = 
       TrapezoidalFilter(startIndex[i], endIndex[i], j, 
			 asdf->trapezoidalRatio);
     /*     printf("fbWts %d %d = %f\n",i, 
	    j-startIndex[i],filterbankWeights[i]->array[j-startIndex[i]]); */
   }
   
 }
 asdf->filterbankWeights = filterbankWeights;
 asdf->dftIndices = dftIndices;
 free(startFrequency);
 free(endFrequency);
 free(centreFrequency);
 free(startIndex);
 free(centreIndex);
 free(endIndex);
}

/*-------------------------------------------------------------------------
 *  LinearTransformationOfFVector performs  a DCT on a F_VECTOR and
 *  outputs an F_VECTOR
 
    Inputs :
         inVect : a vector of type F_VECTOR
         melCepstrumCosineTransform : DCT
    Outputs :
         outVect : a vector of type F_VECTOR
---------------------------------------------------------------------------*/
void LinearTransformationOfFVector (F_VECTOR *inVect, VECTOR_OF_F_VECTORS 
				       *melCepstrumCosineTransform, 
				       int numRows, int numCols,
				       F_VECTOR *outVect) {

int                                    i, j;

for (i = 0; i < numRows; i++){
  outVect->array[i] = 0;
  for (j = 0; j < numCols; j++)
    outVect->array[i] = outVect->array[i] + 
      inVect->array[j]*melCepstrumCosineTransform[i]->array[j];
 }
}

/*----------------------------------------------------------------------------

  PerceptualFilterbankEnergyIntegration : integrates the given Spectrum with a perceptual comb
filter.

  Inputs :
           asdf - structure which contains the front-end-parameters
           Spectrum - Spectrum of the given frame of speech

  Outputs :
           fvect - contains the output filterbank energies.

-----------------------------------------------------------------------------*/
F_VECTOR *PerceptualFilterbankEnergyIntegration(ASDF *asdf, float *Spectrum, float *modGd, int useLog, 
				      F_VECTOR *fvect) {

  static int                 mfft, nfft, numFilters, nfby2;
  static ASDF                *prevAsdf=NULL;
  static VECTOR_OF_F_VECTORS *filterbankWeights;
  static int                 *dftIndices;
  int                        i, j;
  float                      minModGd, maxModGd, value;

  if (prevAsdf != asdf) {
    mfft = GetIAttribute(asdf, "fftOrder");
    nfft = GetIAttribute(asdf,"fftSize");
    nfby2 = nfft/2;
    numFilters = GetIAttribute(asdf,"numFilters");
    filterbankWeights = asdf->filterbankWeights;
    dftIndices = asdf->dftIndices;
    prevAsdf = asdf;
  }
  minModGd = modGd[Imin0Actual(modGd, nfby2)];
  maxModGd = modGd[Imax0Actual(modGd, nfby2)];
  value = fabs(maxModGd - minModGd);
  if ((minModGd == 0.0) && (maxModGd == 0.0)) {
    printf("We are here now \n");
    scanf("%*c");
  }
  for (i = 0; i < nfby2; i++) {
    //if (value != 0)
      modGd[i] = (modGd[i] - minModGd)/value;
  }
  fvect->numElements = numFilters;
  for (i = 0; i < numFilters; i++) {
    fvect->array[i] = 0;
    for (j = dftIndices[i]; 
	 j < dftIndices[i]+filterbankWeights[i]->numElements; j++) {
      if ((j >= 1) && (j <= nfby2)){
        if (modGd[j-1] != 0.0) 
	  /*	  if (useLog)
	    fvect->array[i] = fvect->array[i] + expf(log(Spectrum[j]*Spectrum[j])*modGd[j-1])*
	      filterbankWeights[i]->array[j-dftIndices[i]];
	      else */ 
	    fvect->array[i] = fvect->array[i] + Spectrum[j]*Spectrum[j]*
	      modGd[j-1]*filterbankWeights[i]->array[j-dftIndices[i]];
	/*          printf("fbWts %d %d  = %f spec %d = %f dftInd %d = %d\n",
     i, j-dftIndices[i], filterbankWeights[i]->array[j-dftIndices[i]], 
      j,Spectrum[j], i, dftIndices[i]);*/ 
      }
    }
    fvect->array[i] = fvect->array[i]/filterbankWeights[i]->numElements;
  }
  return(fvect);
}

/*----------------------------------------------------------------------------

  ChromaFilterbankEnergyIntegration : integrates the given Spectrum with a comb
filter of length 12 (assumes 100 cents/note, 12 notes/octave).

  Inputs :
           asdf - structure which contains the front-end-parameters
           Spectrum - Spectrum of the given frame of speech

  Outputs :
           fvect - contains the output filterbank energies.

-----------------------------------------------------------------------------
F_VECTOR *ChromaFilterbankEnergyIntegration(ASDF *asdf, float *Spectrum,
				      F_VECTOR *fvect) {

  static int                 mfft, nfft, numFilters, nfby2;
  static ASDF                *prevAsdf=NULL;
  static VECTOR_OF_F_VECTORS *filterbankWeights;
  static int                 *dftIndices, numOctaves;
  static float               dftLineFrequency, *octaveArray;
  static int                dftTonicFrequency;
  int                        i, j;

  if (prevAsdf != asdf) {
    mfft = GetIAttribute(asdf, "fftOrder");
    nfft = GetIAttribute(asdf,"fftSize");
    nfby2 = nfft/2;
    numFilters = 12;
    filterbankWeights = asdf->filterbankWeights;
    dftIndices = asdf->dftIndices;
    dftLineFrequency = asdf->samplingRate/asdf->fftSize;
    dftTonicFrequency = asdf->tonic/dftLineFrequency;
    octaveArray = (float *) AllocFloatArray(octaveArray, dftTonicFrequency);
    prevAsdf = asdf;
  }
  
  for (i = 0; i < dftTonicFrequency; i++) 
    octaveArray[i] = 0;
  for (j = 0; j < nfby2; j++)
    octaveArray[j%dftTonicFrequency] = octaveArray[j%dftTonicFrequency] + 
      Spectrum[j]*Spectrum[j];
for (i = 0; i < numFilters; i++) {
  fvect->array[i] = 0;
  for (j = dftIndices[i]; 
       j < dftIndices[i]+filterbankWeights[i]->numElements; j++) {
    if (j <= dftTonicFrequency){
      fvect->array[i] = fvect->array[i] + octaveArray[j]*octaveArray[j]* 
        filterbankWeights[i]->array[j-dftIndices[i]];
      /*          printf("fbWts %d %d  = %f spec %d = %f dftInd %d = %d\n",
     i, j-dftIndices[i], filterbankWeights[i]->array[j-dftIndices[i]], 
      j,Spectrum[j], i, dftIndices[i]); 
      }
    }
    fvect->array[i] = fvect->array[i]/filterbankWeights[i]->numElements;
  }
  return(fvect);
}
*/
/*----------------------------------------------------------------------------

  FilterbankEnergyIntegration : integrates the given Spectrum with a comb
filter.

  Inputs :
           asdf - structure which contains the front-end-parameters
           Spectrum - Spectrum of the given frame of speech

  Outputs :
           fvect - contains the output filterbank energies.

-----------------------------------------------------------------------------*/
F_VECTOR *FilterbankEnergyIntegration(ASDF *asdf, float *Spectrum,
				      F_VECTOR *fvect) {

  static int                 mfft, nfft, numFilters, nfby2;
  static ASDF                *prevAsdf=NULL;
  static VECTOR_OF_F_VECTORS *filterbankWeights;
  static int                 *dftIndices;
  static int                 dftTonicFrequency, index;
  static float               dftLineFrequency, *octaveArray;
  static float               tonicFrequency, lowestOctave;
  int                        i, j;

  if (prevAsdf != asdf) {
    mfft = GetIAttribute(asdf, "fftOrder");  
    nfft = GetIAttribute(asdf,"fftSize");
    nfby2 = nfft/2;
    numFilters = GetIAttribute(asdf,"numFilters");
    filterbankWeights = asdf->filterbankWeights;
    dftIndices = asdf->dftIndices;
    if ((asdf->chromaFB) && (asdf->centOrFreq)) {
      dftLineFrequency = asdf->samplingRate/asdf->fftSize;
      tonicFrequency = GetFAttribute(asdf, "tonic");
      dftTonicFrequency = tonicFrequency/dftLineFrequency;
      octaveArray = (float *) AllocFloatArray(octaveArray, dftTonicFrequency);
    }
    prevAsdf = asdf;
  }
  if ((asdf->chromaFB) && (asdf->centOrFreq)) {
    for (i = 0; i < dftTonicFrequency; i++) 
      octaveArray[i] = 0;
    for (j = 0; j < nfby2; j++) {
      if ((j*dftLineFrequency >= tonicFrequency/64) && (j*dftLineFrequency < tonicFrequency/32)) {
	index = (int) ((64*j)%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }      
      if ((j*dftLineFrequency >= tonicFrequency/32) && (j*dftLineFrequency < tonicFrequency/16)) {
	index = (int) ((32*j)%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }      
      if ((j*dftLineFrequency >= tonicFrequency/16) && (j*dftLineFrequency < tonicFrequency/8)) {
	index = (int) ((16*j)%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }      
      if ((j*dftLineFrequency >= tonicFrequency/8) && (j*dftLineFrequency < tonicFrequency/4)) {
	index = (int) ((8*j)%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }      
      if ((j*dftLineFrequency >= tonicFrequency/4) && (j*dftLineFrequency < tonicFrequency/2)) {
	index = (int) ((4*j)%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }      
      else if ((j*dftLineFrequency >= tonicFrequency/2) && (j*dftLineFrequency < tonicFrequency)) {
	index = (int) ((2*j)%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }
      else if ((j*dftLineFrequency >= tonicFrequency) && (j*dftLineFrequency < 2*tonicFrequency)) {
	index = (int) (j%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }
      else if ((j*dftLineFrequency >= tonicFrequency*2) && (j*dftLineFrequency < 4*tonicFrequency)){
	index = (int) ((j/2)%dftTonicFrequency);
	  octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }
      else if ((j*dftLineFrequency >= tonicFrequency*4) && (j*dftLineFrequency < 8*tonicFrequency)){
	index = (int) ((j/4)%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }
      else if ((j*dftLineFrequency >= tonicFrequency*8) && (j*dftLineFrequency < 16*tonicFrequency)){
	index = (int) ((j/8)%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }
      else if ((j*dftLineFrequency >= tonicFrequency*16) && (j*dftLineFrequency < 32*tonicFrequency)){
	index = (int) ((j/16)%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }
      else if ((j*dftLineFrequency >= tonicFrequency*32) && (j*dftLineFrequency < 64*tonicFrequency)){
	index = (int) ((j/32)%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }
      else if ((j*dftLineFrequency >= tonicFrequency*64) && (j*dftLineFrequency < 128*tonicFrequency)){
	index = (int) ((j/64)%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }  
      else if ((j*dftLineFrequency >= tonicFrequency*128) && (j*dftLineFrequency < 256*tonicFrequency)){
	index = (int) ((j/128)%dftTonicFrequency);
	octaveArray[index] = octaveArray[index] + Spectrum[j]*Spectrum[j];
      }
    }
 
  }
  fvect->numElements = numFilters;
  for (i = 0; i < numFilters; i++) {
    fvect->array[i] = 0;
    for (j = dftIndices[i]; 
	 j < dftIndices[i]+filterbankWeights[i]->numElements; j++) {
      if ((asdf->chromaFB) && (asdf->centOrFreq)){
	if (j <= tonicFrequency/dftLineFrequency)
	  fvect->array[i] = fvect->array[i] + octaveArray[j]*octaveArray[j]* 
	    filterbankWeights[i]->array[j-dftIndices[i]];
      } else {
	if (j <= nfby2)
	  fvect->array[i] = fvect->array[i] + Spectrum[j]*Spectrum[j]* 
	    filterbankWeights[i]->array[j-dftIndices[i]];
      }
	/*          printf("fbWts %d %d  = %f spec %d = %f dftInd %d = %d\n",
     i, j-dftIndices[i], filterbankWeights[i]->array[j-dftIndices[i]], 
      j,Spectrum[j], i, dftIndices[i]);*/ 
      
    }
    fvect->array[i] = fvect->array[i]/filterbankWeights[i]->numElements;
  }
  return(fvect);
}

/*---------------------------------------------------------------------------

Gets the actual frameIndex

Inputs : asdf, voiced frameIndex

Outputs : actual frameIndex

-----------------------------------------------------------------------------*/
int GetActualFrameIndex (ASDF *asdf, int voicedFrameIndex) {
  int j = 0, k =0;
  while ((j < asdf->numFrames) && (k < voicedFrameIndex))
    if (asdf->vU[j] == 1) {
      k++;
      j++;
    } else
      j++;
  return(j);
}
    
/*---------------------------------------------------------------------------

Computes the energy in an array of size frameSize

Inputs : a list array of frameSize, gamma -- the power to which energy
         should be taken computes gamma'th root of energy

Outputs : energy

-----------------------------------------------------------------------------*/


float ComputeEnergy (float *array, int frameSize, float gamma) {
int i;
float sum = 0;
for (i = 0; i < frameSize; i++){
  sum = sum + array[i]*array[i];
}
 sum = expf(log(sum)*gamma);
sum = sum/frameSize;
return(sum);
}

/*-----------------------------------------------------------------------------

Computes the zero-crossing rate in array of size of frameSize

Inputs : a list array of frameSize

Outputs : zero-crossing rate
------------------------------------------------------------------------------*/

float ComputeZeroCrossing(float *array, int frameSize) {
  static float              *signum;
  int                i;
  float              sum = 0;
  static int         flag = 0;
  if (flag == 0) {
    signum = (float*) calloc(frameSize, sizeof(float));
    flag = 1;
  }
  for (i = 0; i < frameSize; i++)
    if (array[i] > 0)
      signum[i] = 1;
    else
      signum[i] = -1;
  for(i = 1; i < frameSize; i++)
    sum = sum + fabs(signum[i] - signum[i-1]);
  sum = sum/(2.0*frameSize);
return(sum);
}
/*-----------------------------------------------------------------------------

Estimates the spectral flatness in an array of size frameSize 

Inputs : a list array of frameSize, another list residual of frameSize

Outputs : spectral flatness

------------------------------------------------------------------------------*/
float ComputeSpectralFlatness(float *array, float *residual, int order, int frameSize, int frameShift) { 
  float                  sigEnergy, resEnergy;
  float                  *coef=NULL;
  float                  gain;
  static float           flatness = 1E-10;

  sigEnergy = ComputeEnergy(array, frameSize, 1.0);
  coef = (float *) AllocFloatArray(coef, order+1);
  LpAnal(&array[-1], residual, frameSize, frameShift, coef, order, &gain);
  resEnergy = ComputeEnergy(&residual[1], frameSize, 1.0);
  free (coef);
  if ((resEnergy != 0.0) && (sigEnergy != 0.0))
    flatness = resEnergy/sigEnergy;
   return(flatness);
}


int GradientFrames(float *frame1, float *frame2, int numPts, int frameAdvanceSamples, 
		   float thresEnergy, float thresZero, float thresSpecFlatness) {

  static float              fEnergy1, fEnergy2, resEnergy1, resEnergy2;
  static float              fZero1, fZero2;
  static float              fSpecFlatness1 = 1.0E-10, fSpecFlatness2 = 1.0E-10, gain;
  static float              *residual1=NULL, *residual2=NULL;
  static float              coef[11];
  static float               egyChange, zeroChange, flatnessChange;


  if (residual1 == NULL) {
    residual1 = (float *) AllocFloatArray(residual1, numPts);
    residual2 = (float *) AllocFloatArray(residual2, numPts);
  }
  fEnergy1 = (float) ComputeEnergy(frame1, numPts,1.0);
  fEnergy2 = (float) ComputeEnergy(frame2, numPts,1.0);
  fZero1 = (float) ComputeZeroCrossing(frame1, numPts);
  fZero2 = (float) ComputeZeroCrossing(frame2, numPts);
  LpAnal(&frame1[-1], &residual1[-1], numPts, frameAdvanceSamples, coef, 10, &gain);
  resEnergy1 = ComputeEnergy(residual1, numPts,1.0);
  LpAnal(&frame2[-1], &residual2[-1],numPts, frameAdvanceSamples, coef, 10, &gain);
  resEnergy2 = ComputeEnergy(residual2, numPts,1.0);
  if ((resEnergy1 != 0.0) && (fEnergy1 != 0.0))
    fSpecFlatness1 = resEnergy1/fEnergy1;  
  if ((resEnergy2 != 0.0) && (fEnergy2 != 0.0))
    fSpecFlatness2 = resEnergy2/fEnergy2;  
  egyChange = fabs(fEnergy1 - fEnergy2)/fEnergy1;
  zeroChange = fabs(fZero1 - fZero2)/fZero1;
  flatnessChange = fabs(fSpecFlatness1 - fSpecFlatness2)/fSpecFlatness1;
  if ((egyChange >= thresEnergy) 
      && (zeroChange <= thresZero) 
      &&(flatnessChange <= thresSpecFlatness))
    return(1);
  else
    return(0);
	
}

/*------------------------------------------------------------------------------

 Classifies a given speech signal into voiced and unvoiced frames 

Input :

waveform      - given waveform
samples       - number of samples
frameShift   - shift in location of sliding Window
frameSize    - size of a frame of speech

Output : 
vU           - a binary array 
              - a 1 in a particular location indicates that the given frame of
                speech is voiced
              - a 0 in a particular location indicates that the given frame of 
                speech is unvoiced

-----------------------------------------------------------------------------*/


void VoicedUnvoiced(short *waveform, long samples, short *vU, 
		    int frameShift, int frameSize, int order, float thresZero, 
		    float thresEnergy, float thresSpecFlatness, int percentFrames) {
  float             *zeroCrossings, *energy, *specFlatness, *residual, *sortArray;
  int               numFrames,i, j, frameNumber;
  float             *array;
  float             aveZero, aveEnergy, aveSpecFlatness;
  float             zeroThres, energyThres, specThres;
  //  float             ThZero = 0.5, ThEnergy = 0.5, ThSpec = 0.1;

  numFrames = (int) (ceilf((float) (samples)/(float)(frameShift)));
  for (i = 0; i < numFrames; i++)
    vU[i] = 1;
  zeroCrossings = (float *) calloc(numFrames+1,sizeof(float));
  energy = (float *) calloc(numFrames+1,sizeof(float));
  specFlatness = (float *) calloc(numFrames+1, sizeof(float)); 
  sortArray = (float *) calloc(numFrames+1, sizeof(float)); 
  array = (float *) calloc(frameSize+1,sizeof(float));
  residual = (float *) calloc(frameSize+1,sizeof(float));
  for (i = 0; i < numFrames; i++){
    for (j = 1; j < frameSize; j++)
      if ((i*frameShift+j-1) < samples)
        array[j]  = waveform[i*frameShift+j-1];
      else
        array[j] = 0;  
  
  zeroCrossings[i] = ComputeZeroCrossing(&array[1], frameSize); 
  energy[i] = ComputeEnergy(&array[1],frameSize,1.0);
  specFlatness[i] = ComputeSpectralFlatness(array, residual, order, frameSize, frameShift);
  }
  /*  printf("zero %f energy %f flat %f \n", zeroCrossings[i], energy[i], specFlatness[i]);
      scanf("*c"); */
 if (percentFrames == 1) {
   for (i = 0; i < numFrames; i++)
     sortArray[i] = energy[i];
   QuickSort(sortArray, 0, numFrames-1);
   frameNumber = (int) (thresEnergy*(numFrames-1));
   energyThres = sortArray[frameNumber];
   if (energyThres == 0.0) {
     i = frameNumber+1;
     while ((i < numFrames) && (sortArray[i] == 0.0))
       i++;
     energyThres = sortArray[i];
   }
   for (i = 0; i < numFrames; i++)
     sortArray[i] = zeroCrossings[i];
   QuickSort(sortArray, 0, numFrames-1);
   frameNumber = (int) ((1-thresZero)*(numFrames-1));
   zeroThres = sortArray[frameNumber];
   for (i = 0; i < numFrames; i++)
     sortArray[i] = specFlatness[i];
   QuickSort(sortArray, 0, numFrames-1);
   frameNumber = (int) ((1-thresSpecFlatness)*(numFrames-1));
   specThres = sortArray[frameNumber];
 } else {
   aveZero = Average(zeroCrossings,numFrames);
   aveEnergy = Average(energy,numFrames);
   aveSpecFlatness = Average(specFlatness, numFrames);
   zeroThres = aveZero*(1-thresZero);
   energyThres = aveEnergy*thresEnergy;
   specThres = aveSpecFlatness*(1-thresSpecFlatness);
 }
    printf("numFrames %d ThZero %f ThEnergy %f ThFlat %f \n",numFrames, zeroThres, energyThres, specThres);
    //   printf("numFrames %d thzero %f thenergy %f thflat %f \n",numFrames, aveZero*thresZero, aveEnergy*thresEnergy, aveSpecFlatness*thresSpecFlatness); 
   for (i = 0; i < numFrames; i++)
     if	((energy[i] >= energyThres) && (zeroCrossings[i] <= zeroThres) &&
	 (specFlatness[i] <= specThres))
       vU[i] = 1;
     else
       vU[i] = 0;
 
   free(zeroCrossings);
   free(specFlatness);
   free(energy);
   free(array);
   free(residual);
   free(sortArray);
}

/****************************************************************************
*	The following subroutine computes the Group delay Cepstrum
*       of a given signal
*
*	inputs : signal npts 
*	mfft : fft order nfft = 2**mfft
*	winlen : number of cepstral coeffts
*
*	Outputs :
*	groupDelay(nfft) - Group delay function
*****************************************************************************
*
*/
float *GroupDelayCepstrum(float *signal,int npts, int nfft, int mfft, 
			  int winlen, float *groupDelayCepstrum){
  static float		*sigTemp;
  static float		*ax, *ay,*amag,*phase, *groupDelay;
  static int            nfBy2,flag = 0;
  static complex 	*cSig,*cfSig;
  static complex	*cfTemp1,*cfTemp2,u;
  static float          epsilon = 0.0001;
  int		        i;
  float                 Ave, c0;

  if (flag == 0) {
    nfBy2 = nfft/2;
    ax = (float *) AllocFloatArray(ax,nfft+1);
    ay = (float *) AllocFloatArray(ay,nfft+1);
    amag = (float *) AllocFloatArray(amag,nfft+1);
    phase = (float *) AllocFloatArray(phase,nfft+1);
    groupDelay = (float *) AllocFloatArray(groupDelay,nfft+1);
    sigTemp = (float *) AllocFloatArray(sigTemp,nfft+1);
    cSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp1 = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp2 = (complex *) calloc (nfft+1, sizeof(complex));
    if ((cSig == NULL) || (cfSig == NULL) || (cfTemp1 == NULL) ||
	(cfTemp2 == NULL)) {
      printf("unable to allocate complex array in Group Delay\n");
      fflush(stdout);
      _exit(-1);
    } 
    flag = 1;
  }
  for (i = 1; i <= npts; i++)
    sigTemp[i] = signal[i];
  for(i = npts+1; i<= nfft; i++)
    sigTemp[i] = 0.0;
  for (i = 1; i <= npts; i++){
    u.re = sigTemp[i];
    u.im = 0.0;
    cSig[i] = u;
    u.re = (float)(i-1);
    u.im = 0.0;
    cmul(cfSig[i],u,cSig[i]);
  }
  for (i = npts+1; i <= nfft; i++) {
    cSig[i].re = 0.0;
    cSig[i].im = 0.0;
    cfSig[i].re = 0.0;
    cfSig[i].im = 0.0;
  }
  Rfft(sigTemp,ax,ay,mfft,nfft,-1);
  SpectrumReal(nfft,ax,ay,amag,phase);
  c0 = 0;
  for (i = 1; i <= nfft; i++)
    c0 = c0 + amag[i]*amag[i];
  c0 = sqrt(c0/nfft);
  Cfft(cSig,cfTemp1,mfft,nfft,-1);
  Cfft(cfSig,cfTemp2,mfft,nfft,-1);
  for (i = 1; i <= nfft; i++){
    conjg(u,cfTemp1[i]);
    cmul(cfTemp2[i],cfTemp2[i],u);
    u.re = cfTemp2[i].re;
    u.im = cfTemp2[i].im;
    if (amag[i] > epsilon) {
      cfTemp2[i].re = u.re/(amag[i]*amag[i]);
      cfTemp2[i].im = u.im/(amag[i]*amag[i]);
    } else 
      cfTemp2[i] = cfTemp2[i-1];
    groupDelay[i] = cfTemp2[i].re;
  }
  RemoveAverage(groupDelay,nfft,&Ave); 
  Rfft(groupDelay,ax,ay,mfft,nfft,1);
  for (i = 1; i <= winlen; i++) 
    groupDelayCepstrum[i-1] = ax[i+1]*HanW(i,winlen+1);
  return(groupDelayCepstrum);
}
/****************************************************************************
*	The following subroutine computes the smoothed group delay function of signal
*
*	inputs : signal(npts) 
*	mfft : fft order  = 2**mfft
*	winlen : window length for smoothing
*
*	Outputs :
*	groupDelay(nfft) - Group delay function
*****************************************************************************
*
*/
float *GroupDelay(float *signal,int npts, int nfft, int mfft, int winlen,
		float *groupDelay){
  static float		*ax, *ay, *amag, *phase, *gdCepstrum;
  static int            nfBy2,flag = 0;
  int		        i;

  if (flag == 0) {
    nfBy2 = nfft/2;
    ax = (float *) AllocFloatArray(ax,nfft+1);
    ay = (float *) AllocFloatArray(ay,nfft+1);
    amag = (float *) AllocFloatArray(amag,nfft+1);
    phase = (float *) AllocFloatArray(phase,nfft+1);
    gdCepstrum = (float *) AllocFloatArray(gdCepstrum,winlen);
    flag = 1;
  }
  gdCepstrum = (float *) GroupDelayCepstrum(signal, npts, nfft, mfft,
					    winlen, gdCepstrum);
  ax[1] = 0.0;
  for (i = 2; i <= winlen; i++) {
    ax[i] = gdCepstrum[i-2];
    ax[nfft-i+2] = ax[i];
  }
  for (i = winlen+1; i <= nfft-(winlen-1); i++)
    ax[i] = 0.0;
  Rfft(ax,groupDelay,ay,mfft,nfft,-1);
  return(groupDelay);
}

/****************************************************************************
*	The following subroutine computes the standard group delay function of signal
*
*	inputs : DATA npts points long
*	mfft : fft stages nfft = 2**mfft
*	winlen : window length for zero spectrum
*
*	Outputs :
*	groupDelay(nfft) - Group delay function
*****************************************************************************
*
*/
	void StdGroupDelay(float *signal,int npts, int nfft, int mfft, 
		    float *groupDelay){
	static float		*sigTemp;
	static float		*ax, *ay,*amag,*phase;
        static int              nfBy2,flag = 0;
	static complex 	        *cSig,*cfSig;
	static complex		*cfTemp1,*cfTemp2,u;
	int		        i;

        if (flag == 0) {
	  nfBy2 = nfft/2;
          ax = (float *) calloc(nfft+1, sizeof(float));
          ay = (float *) calloc(nfft+1, sizeof(float));
          amag = (float *) calloc(nfft+1, sizeof(float));
          phase = (float *) calloc(nfft+1, sizeof(float));
          cSig = (complex *) calloc (nfft+1, sizeof(complex));
          cfSig = (complex *) calloc (nfft+1, sizeof(complex));
          cfTemp1 = (complex *) calloc (nfft+1, sizeof(complex));
          cfTemp2 = (complex *) calloc (nfft+1, sizeof(complex));
	  for (i = 0; i < nfft; i++) {
	    cfTemp1[i].re = 0;
	    cfTemp1[i].im = 0;
	    cfTemp2[i].re = 0;
	    cfTemp2[i].im = 0;
	  }
          sigTemp = (float *) calloc(nfft+1, sizeof(float));
          flag = 1;
	}
	for (i = 1; i <= npts; i++)
	  sigTemp[i] = signal[i];
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	for (i = 1; i < npts; i++){
	  u.re = sigTemp[i];
          u.im = 0.0;
	  cSig[i] = u;
	  u.re = (float)(i-1);
          u.im = 0.0;
	  cmul(cfSig[i],u,cSig[i]);
	}
	for (i = npts+1; i <= nfft; i++) {
	  cSig[i].re = 0.0;
          cSig[i].im = 0.0;
	  cfSig[i].re = 0.0;
          cfSig[i].im = 0.0;
	}
	Rfft(sigTemp,ax,ay,mfft,nfft,-1);
	SpectrumReal(nfft,ax,ay,amag,phase);
	Cfft(cSig,cfTemp1,mfft,nfft,-1);
	Cfft(cfSig,cfTemp2,mfft,nfft,-1);
        conjg(u,cfTemp1[1]);
	cmul(cfTemp2[1],cfTemp2[1],u);
	if (amag[1] < 1.0E-1){ 
	  cfTemp2[1].re = cfTemp2[1].re/0.0001;
	  cfTemp2[1].im = cfTemp2[1].im/0.0001;
	} else{
          cfTemp2[1].re = cfTemp2[1].re/(amag[1]*amag[1]);
          cfTemp2[1].im = cfTemp2[1].im/(amag[1]*amag[1]);
	}
	groupDelay[1] = cfTemp2[1].re;
	for (i = 2; i <= nfBy2; i++){
          conjg(u,cfTemp1[i]);
	  cmul(cfTemp2[i],cfTemp2[i],u);
	  u.re = cfTemp2[i].re;
          u.im = cfTemp2[i].im;
	  if (amag[i] < 1E-3){ 
	    cfTemp2[i].re = cfTemp2[i].re/0.0001;
	    cfTemp2[i].im = cfTemp2[i].im/0.0001;
	  } else {
	    cfTemp2[i].re = (u.re/(amag[i]*amag[i]));
	    cfTemp2[i].im = (u.im/(amag[i]*amag[i]));
	  }
	  groupDelay[i] = cfTemp2[i].re;
	  /* if (isnan(groupDelay[i])|| isinf(groupDelay[i])) {
	    groupDelay[i] = 0.0;
	    printf("Gd %d becomes nan or inf\n", i);
	    }*/
	}
	//	if (isnan(groupDelay[1])||isinf(groupDelay[1])) {
	//    groupDelay[1] = 0.0;
	//    printf("Gd %d becomes nan or inf\n", 1);
	//  }
	}

/****************************************************************************
*	The following subroutine computes the group delay function of 
*       LP Residual of a given signal.
*	inputs : DATA npts points long
*	mfft : fft stages nfft = 2**mfft
*	winlen : window length for zero spectrum
*
*	Outputs :
*	groupDelay(nfft) - Group delay function
*****************************************************************************
*
*/
void LPResGroupDelay(float *signal,int npts, int frameShift, int nfft, int mfft,  
		     int order, float *groupDelay){
  static float		*resEnergy, *coef;
  static float            gain;
  static int              flag = 0;
	
  if (flag == 0) {
    resEnergy = (float *) AllocFloatArray(resEnergy, npts+1);
    coef = (float *) AllocFloatArray(coef, order+2);
    flag = 1;
  }
  LpAnal(signal,resEnergy,npts, frameShift, coef,order, &gain);
  StdGroupDelay(resEnergy,npts, nfft, mfft, groupDelay);
}


/****************************************************************************
*	The following function computes the minimum phase 
*	group delay cepstrum for the given signal 
*
*	inputs : signal(npts)
*	mfft : fft order  nfft = 2**mfft
*	winlen : window length for minimum phase signal
*	gamma : time compression of minimum phase signal
*
*       Outputs:
*       MinGdCepstrum(winlen) - float *
*       Comment - mean removal not done as MinGd already is zero mean
*****************************************************************************
*
*/
float *MinGdCepstrum(float *signal,int npts, int nfft, int mfft, 
		     int winlen, float *minGdCepstrum, float gamma){
  static float	  *sigTemp;
  static int      flag = 0;
  static float	  *ax, *ay, *amag, *phase, *minGd;
  static int      nfBy2;
  int		  i;
  
  if (flag == 0) {
    nfBy2 = nfft/2;
    sigTemp = (float *) AllocFloatArray(sigTemp, nfft+1);
    ax =  (float *) AllocFloatArray(ax, nfft+1);
    ay = (float *) AllocFloatArray(ay,nfft+1);
    amag = (float *) AllocFloatArray(amag,nfft+1);
    phase = (float *) AllocFloatArray(phase,nfft+1);
    minGd = (float *) AllocFloatArray(minGd,nfft+1);
    flag = 1;
  }
  for (i = 1; i <= npts; i++)
    sigTemp[i] = signal[i];
  for(i = npts+1; i<= nfft; i++)
    sigTemp[i] = 0.0;
  Rfft(sigTemp, ax, ay, mfft, nfft,-1);
  SpectrumReal(nfft, ax, ay, amag, phase);
  for (i = 1; i <= nfft; i++){
    amag[i] = exp(log(amag[i])*gamma);
  }
  Rfft(amag,sigTemp,ay,mfft,nfft,1);
  for (i = 1; i <= winlen; i++)
    minGdCepstrum[i-1] = sigTemp[i]*HanW(i,winlen+1);
  return(minGdCepstrum);
}

/****************************************************************************
*	The following function computes the LP phase 
*	group delay function from the given signal 
*
*	inputs : signal(npts)
*	mfft : fft order  nfft = 2**mfft
*	lpOrder : window length for minimum phase signal
*
*       Outputs:
*       LPGd - float *
*****************************************************************************
*
*/
float *LPGd(float *signal,int npts, int frameShift, int nfft, int mfft, 
		int lpOrder, float *LPGdelay){

  static float	  *amag, *phase, *coef, *resEnergy; 
  static float    gain;
  static int      flag = 0;
  static int      nfBy2;
  int		  i;
    
  if (flag == 0) {
    nfBy2 = nfft/2;
    resEnergy = (float *) AllocFloatArray(resEnergy, npts+1);
    coef = (float *) AllocFloatArray(coef, lpOrder+1);
    amag = (float *) AllocFloatArray(amag,nfft+1);
    phase = (float *) AllocFloatArray(phase,nfft+1);
    flag = 1;
  }

  LpAnal(signal, resEnergy, npts, frameShift, coef, lpOrder, &gain);
  LPSpectrum(coef, lpOrder, amag, phase, nfft, mfft, gain);
  for ( i = 1; i < nfBy2; i++)
    LPGdelay[i] = phase[i+1] - phase[i];
  LPGdelay[nfBy2] = LPGdelay[nfBy2-1];
  //  LPGdelay[0] = 0;
  for ( i = 2; i < nfBy2; i++)
    if (fabs(LPGdelay[i]) > PI)
      LPGdelay[i] = LPGdelay[i-1];
  return(LPGdelay);
}

/****************************************************************************
*	The following function computes the Minimum phase 
*	group delay function from the given signal 
*
*	inputs : signal(npts)
*	mfft : fft order  nfft = 2**mfft
*	winlen : window length for minimum phase signal
*	alfa : compression required for magnitude spectrum
*
*       Outputs:
*       MinGd - float *
*****************************************************************************
*
*/
float *MinGdCepstrum_DCT(float *signal,int npts, int nfft, int mfft, 
			 int gdSmthWinSize, int numCepstrum, float *minGdCepstrum, float gamma, int startIndex, int endIndex){

  static float	             *ax, *ay, *amag, *phase, *minGd; 
  static float               *sigTemp, *minRootCepstrum;
  static VECTOR_OF_F_VECTORS *discreteCosineTransform;
  static F_VECTOR            *fvect, *minGdFvect;
  static int                 flag = 0, DCTSize;
  static int                 nfBy2;
  int		             i;
    
  if (flag == 0) {
    nfBy2 = nfft/2;
    sigTemp = (float *) AllocFloatArray(sigTemp,nfft+1);
    ax =  (float *) AllocFloatArray(ax,nfft+1);
    ay = (float *) AllocFloatArray(ay,nfft+1);
    amag = (float *) AllocFloatArray(amag,nfft+1);
    phase = (float *) AllocFloatArray(phase,nfft+1);
    minRootCepstrum = (float *) AllocFloatArray(minRootCepstrum, nfBy2);
    minGd = (float *) AllocFloatArray(minGd,nfft+1);   
    DCTSize = endIndex - startIndex + 1;
    discreteCosineTransform = (VECTOR_OF_F_VECTORS *) GeneratePseudoDct(1, numCepstrum, DCTSize);
    fvect = (F_VECTOR *) AllocFVector(nfBy2);
    minGdFvect = AllocFVector(numCepstrum);
    flag = 1;
  }
  minRootCepstrum = MinGdCepstrum(signal, npts, nfft, mfft, 
				  gdSmthWinSize, minRootCepstrum, gamma);
  for (i = 1; i <= gdSmthWinSize; i++) 
    sigTemp[i] = minRootCepstrum[i-1];
  for (i = gdSmthWinSize+1; i <= nfft; i++)
    sigTemp[i] = 0.0;
  Rfft(sigTemp,ax,ay,mfft,nfft,-1);
  SpectrumReal(nfft, ax, ay, amag, phase);
  for(i = 0; i <  nfBy2-1; i++) {
    minGd[i] = phase[i] - phase[i+1];
  }
  minGd[nfBy2-1] = minGd[nfBy2-2];
  for (i = startIndex; i < endIndex;i++)
    fvect->array[i- startIndex] = minGd[i - startIndex];
  LinearTransformationOfFVector(fvect, discreteCosineTransform, numCepstrum, DCTSize, minGdFvect);
  for (i = 1; i <= numCepstrum; i++) 
    minGdCepstrum[i-1] = minGdFvect->array[i-1];
  return(minGdCepstrum);  
}

/****************************************************************************
*	The following function computes the Minimum phase 
*	group delay function from the given signal 
*
*	inputs : signal(npts)
*	mfft : fft order  nfft = 2**mfft
*	winlen : window length for minimum phase signal
*	alfa : compression required for magnitude spectrum
*
*       Outputs:
*       MinGd - float *
*****************************************************************************
*
*/
float *MinGd(float *signal,int npts, int nfft, int mfft, 
		int winlen, float *minGd, float gamma){

  static float	  *ax, *ay, *amag, *phase; 
  static float    *sigTemp, *minGdCepstrum;
  static int      flag = 0;
  static int      nfBy2;
  int		  i;
    
  if (flag == 0) {
    nfBy2 = nfft/2;
    sigTemp = (float *) AllocFloatArray(sigTemp,nfft+1);
    ax =  (float *) AllocFloatArray(ax,nfft+1);
    ay = (float *) AllocFloatArray(ay,nfft+1);
    amag = (float *) AllocFloatArray(amag,nfft+1);
    phase = (float *) AllocFloatArray(phase,nfft+1);
    minGdCepstrum = (float *) AllocFloatArray(minGdCepstrum, winlen);
    flag = 1;
  }
  minGdCepstrum = MinGdCepstrum(signal, npts, nfft, mfft,
				winlen, minGdCepstrum, gamma);
  for (i = 1; i <= winlen; i++) 
    sigTemp[i] = minGdCepstrum[i-1];
  for (i = winlen+1; i <= nfft; i++)
    sigTemp[i] = 0.0;
  Rfft(sigTemp,ax,ay,mfft,nfft,-1);
  SpectrumReal(nfft, ax, ay, amag, phase);
  for(i = 0; i <  nfBy2-1; i++) {
    minGd[i] = phase[i] - phase[i+1];
  }
  minGd[nfBy2-1] = minGd[nfBy2-2];
  return(minGd);
}

/****************************************************************************
*	The following function computes  the
*	smoothed magnitude spectrum of the given signal
*       Cepstral smoothing is used.
*
*	inputs : signal - npts
*	mfft : fft order nfft = 2**mfft
*	winlen : window length for zero spectrum
*
*	Outputs :
*	modGdCepstrum(winlen) - modified group delay cepstrum
*****************************************************************************
*/
float *SmoothMagSpectrum(float *signal, int npts, int nfft, int mfft, int smthWinSize,
                         float *smthMag) {
  static float		*ax, *ay, *amag, *phase, *sigTemp;
  static float		*cepAmag, c0;
  static int            nfBy2, flag = 0, i;
  if (flag == 0) {
    nfBy2 = nfft/2;
    ax = (float *) calloc(nfft+1, sizeof(float));
    ay = (float *) calloc(nfft+1, sizeof(float));
    amag = (float *) calloc(nfft+1, sizeof(float));
    cepAmag = (float *) calloc(nfft+1, sizeof(float));
    phase = (float *) calloc(nfft+1, sizeof(float));
    sigTemp = (float *) calloc(nfft+1, sizeof(float));
    flag = 1;
  }

 
  for (i = 1; i <= npts; i++)
    sigTemp[i] = signal[i];
  for(i = npts+1; i<= nfft; i++)
    sigTemp[i] = 0.0;
 Rfft(sigTemp,ax,ay,mfft,nfft,-1);
  SpectrumReal(nfft,ax,ay,amag,phase);
  for( i = 1; i <= nfft; i++)
    ax[i] = amag[i];
  CepSmooth(ax,cepAmag,mfft,nfft, smthWinSize, &c0, 1.0);
  for (i = 0; i <= nfBy2; i++)
    smthMag[i] = cepAmag[i+1];
  return(smthMag);
  }
/****************************************************************************
*	The following function normalizes the modGd with respect
*       using the cepstrally smoothed spectrum of the signal
*	
*
*	inputs : mogGd, cepstrally smoothed spectrum
*	nfft : fftOrder 
*
*	Outputs :
*	modGdCepstrum: Normalized modGd cepstrum
*****************************************************************************
*/
void NormalizeModGd(float *modGd, float *cepAmag, float *ax, int nfft, int mfft) {
  static float        *cepstrum, *ay;
  static int          flag = 0, nfby2;
  static float        scale;
  static float        delay;
  static int          mgdMaxI, cepMaxI;
  static int          i;

  if (flag == 0) {
    cepstrum = (float *) AllocFloatArray(cepstrum, nfft);
    ay = (float *) AllocFloatArray(ay, nfft);
    nfby2 = nfft/2;
    flag = 1;
  }
  Rfft(modGd, ax, ay, mfft, nfft, 1);
  delay = ax[1];
  for (i = 2; i <= nfby2; i++){
    cepstrum[i] = ax[i]/(float)(i-1);
    cepstrum[nfft-i+2] = cepstrum[i];
  }
  cepstrum[1] = 0;
    Rfft (cepstrum, ax, ay, mfft, nfft, -1);
  mgdMaxI = (int) Imax(ax, nfft);
  cepMaxI = (int) Imax(cepAmag, nfft);
  scale = cepAmag[cepMaxI]/ax[mgdMaxI];
  for (i = 1; i <= nfby2; i++){
    ax[i] = ax[i]*scale;
    ax[nfft-i+1] = ax[i];
  }
  Rfft(ax, cepstrum, ay, mfft, nfft, 1);
  for (i = 2; i <=nfby2; i++)
    ax[i] = cepstrum[i]*(i - 1);
  ax[1] = delay*scale;
  }
/****************************************************************************
*	The following function computes  the
*	modified group delay cepstrum (ncn) for the given signal
*       The smoothing of |X(k)| is performed using LogCepSmooth
*	inputs : signal - npts
*	mfft : fft order nfft = 2**mfft
*	winlen : window length for zero spectrum
*
*	Outputs :
*	modGdCepstrum(winlen) - modified group delay cepstrum
*****************************************************************************
*/
float *ModGdLogSmthCepstrumNcN(float *signal,int npts,int nfft,int mfft, 
			int winlen, int smthWinSize, float *modGdCepstrum, 
			float alfaP, float alfaN,  
			float gamma, int gdsign, 
			int removeLPhase, int removeMin, int mgdNormalize,
			int medOrder, int startIndex, int endIndex) { 
  static float		*sigTemp;
  static float		*ax, *ay, *amag, *phase, *modGd, *modGdSmth;
  static float		*cepAmag;
  static int            nfBy2, flag = 0;
  static complex        *cSig, *cfSig;
  static complex	*cfTemp1, *cfTemp2, u;
  int		        i, sign;
  float		        c0, abscfTemp, logcfTemp;
  float                 minValue;


  if (flag == 0) {
    nfBy2 = nfft/2;
    ax = (float *) calloc(nfft+1, sizeof(float));
    ay = (float *) calloc(nfft+1, sizeof(float));
    amag = (float *) calloc(nfft+1, sizeof(float));
    cepAmag = (float *) calloc(nfft+1, sizeof(float));
    phase = (float *) calloc(nfft+1, sizeof(float));
    modGd = (float *) calloc(nfft+1, sizeof(float));
    if (medOrder != 0)
      modGdSmth = (float *) calloc(nfft+1, sizeof(float));
    cSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp1 = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp2 = (complex *) calloc (nfft+1, sizeof(complex));
    sigTemp = (float *) calloc(nfft+1, sizeof(float));
    flag = 1;
  }
  for (i = 1; i <= npts; i++)
    sigTemp[i] = signal[i];
  for(i = npts+1; i<= nfft; i++)
    sigTemp[i] = 0.0;
  for (i = 1; i < npts; i++){
    u.re = sigTemp[i];
    u.im = 0.0;
    cSig[i] = u;
    u.re = (float)(i-1);
    u.im = 0.0;
    cmul(cfSig[i],u,cSig[i]);
  }
  for (i = npts+1; i <= nfft; i++) {
    cSig[i].re = 0.0;
    cSig[i].im = 0.0;
    cfSig[i].re = 0.0;
    cfSig[i].im = 0.0;
  }
  Cfft(cSig,cfTemp1,mfft,nfft,-1);
  Cfft(cfSig,cfTemp2,mfft,nfft,-1);
  Rfft(sigTemp,ax,ay,mfft,nfft,-1);
  SpectrumReal(nfft,ax,ay,amag,phase);
  for( i = 1; i <= nfft; i++)
    ax[i] = amag[i];
  LogCepSmooth(ax,cepAmag,mfft,nfft, smthWinSize, &c0, gamma);
  for (i = 1; i <= nfBy2+1; i++){
    conjg(u,cfTemp1[i]);
    cmul(cfTemp2[i],cfTemp2[i],u);
    u.re = cfTemp2[i].re;
    u.im = cfTemp2[i].im;
    cfTemp2[i].re = (u.re/(cepAmag[i]*cepAmag[i]));
    cfTemp2[i].im = (u.im/(cepAmag[i]*cepAmag[i]));
    modGd[i] = cfTemp2[i].re;
    if (i > 1)
      modGd[nfft-i+2] = modGd[i];
  }
  /* Remove DC component of modGd */

  /* RemoveAverage(modGd,nfft,&Ave); */

  /* Reduce dynamic range of modGd if necessary */
    for (i = 1; i <= nfft; i++) {
      if (modGd[i] < 0) 
	sign = -1;
      else
	sign = 1;
      abscfTemp = fabs(modGd[i]);
      if (abscfTemp == 0)
	modGd[i] = 0;
      else {
	logcfTemp = log(abscfTemp);
	if ((alfaN == 0) && (alfaP == 0))
	  if (gdsign == 1)
	    modGd[i] = sign*logcfTemp;
	  else
	    modGd[i] = logcfTemp;
	else {
	  if (sign > 0.0)
	    modGd[i] = exp(alfaP*logcfTemp);
	  else 
	    modGd[i] = exp(alfaN*logcfTemp);
	  if (gdsign == 1)
	    modGd[i] = sign*modGd[i];
	}
      }
    }  
    /* Bandlimit modGd */
    if (removeLPhase) {
      for (i = 2; i < nfBy2; i++) {
	modGd[i] = modGd[i] - modGd[1];
	modGd[nfft - i + 2] = modGd[i];
      }
      modGd[1] = 0;
    }
    if (medOrder != 0) {
      MedianSmoothArray(modGd, nfBy2, medOrder, modGdSmth);
      for (i = 2; i <= nfBy2; i++)
	modGdSmth[nfft - i + 2] = modGdSmth[i]; 
      modGd = modGdSmth;
    }
    minValue = modGd[IminActual(modGd, nfft)];
    if (removeMin) 
      for (i = 1; i <nfft; i++) 
        modGd[i] = modGd[i] - minValue;
      
    if ((startIndex > 1) || (endIndex < nfBy2)) {
      for (i = 1; i < startIndex; i++) { 
	modGd[i] = 0.0;
	if (i > 1)
	  modGd[nfft - i+2] = modGd[i];
      }
      for (i = endIndex+1; i <= nfBy2; i++) {
	modGd[i] = 0;
	modGd[nfft - i+ 2] = 0;
      }
    }
    if (mgdNormalize == 1) {
      CepSmooth(amag, cepAmag, mfft, nfft, smthWinSize, &c0, 1.0);  
      for (i = 1; i <= nfft; i++)
        cepAmag[i] = log(cepAmag[i]);    
      NormalizeModGd(modGd, cepAmag, ax, nfft, mfft);
    } else 
      Rfft(modGd, ax, ay, mfft, nfft, 1);
    for (i = 1; i <= winlen; i++) 
      modGdCepstrum[i-1] = ax[i];
    return(modGdCepstrum);  
}
/****************************************************************************
*	The following function computes  the
*	modified group delay cepstrum (ncn) for the given signal
*
*	inputs : signal - npts
*	mfft : fft order nfft = 2**mfft
*	winlen : window length for zero spectrum
*
*	Outputs :
*	modGdCepstrum(winlen) - modified group delay cepstrum
*****************************************************************************
*/
float *ModGdCepstrumNcN(float *signal,int npts,int nfft,int mfft, 
			int winlen, int smthWinSize, float *modGdCepstrum, 
			float alfaP, float alfaN,  
			float gamma, int gdsign, 
			int removeLPhase, int removeMin, int mgdNormalize,
			int medOrder, int startIndex, int endIndex) { 
  static float		*sigTemp;
  static float		*ax, *ay, *amag, *phase, *modGd, *modGdSmth;
  static float		*cepAmag;
  static int            nfBy2, flag = 0;
  static complex        *cSig, *cfSig;
  static complex	*cfTemp1, *cfTemp2, u;
  int		        i, sign;
  float		        c0, abscfTemp, logcfTemp;
  float                 minValue;


  if (flag == 0) {
    nfBy2 = nfft/2;
    ax = (float *) calloc(nfft+1, sizeof(float));
    ay = (float *) calloc(nfft+1, sizeof(float));
    amag = (float *) calloc(nfft+1, sizeof(float));
    cepAmag = (float *) calloc(nfft+1, sizeof(float));
    phase = (float *) calloc(nfft+1, sizeof(float));
    modGd = (float *) calloc(nfft+1, sizeof(float));
    if (medOrder != 0)
      modGdSmth = (float *) calloc(nfft+1, sizeof(float));
    cSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp1 = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp2 = (complex *) calloc (nfft+1, sizeof(complex));
    sigTemp = (float *) calloc(nfft+1, sizeof(float));
    flag = 1;
  }
  for (i = 1; i <= npts; i++)
    sigTemp[i] = signal[i];
  for(i = npts+1; i<= nfft; i++)
    sigTemp[i] = 0.0;
  for (i = 1; i < npts; i++){
    u.re = sigTemp[i];
    u.im = 0.0;
    cSig[i] = u;
    u.re = (float)(i-1);
    u.im = 0.0;
    cmul(cfSig[i],u,cSig[i]);
  }
  for (i = npts+1; i <= nfft; i++) {
    cSig[i].re = 0.0;
    cSig[i].im = 0.0;
    cfSig[i].re = 0.0;
    cfSig[i].im = 0.0;
  }
  Cfft(cSig,cfTemp1,mfft,nfft,-1);
  Cfft(cfSig,cfTemp2,mfft,nfft,-1);
  Rfft(sigTemp,ax,ay,mfft,nfft,-1);
  SpectrumReal(nfft,ax,ay,amag,phase);
  for( i = 1; i <= nfft; i++)
    ax[i] = amag[i];
  CepSmooth(ax,amag,mfft,nfft, smthWinSize, &c0, 2*gamma);
  MedianSmoothArray(amag, nfft, 3, cepAmag);
  for (i = 1; i <= nfBy2+1; i++){
    conjg(u,cfTemp1[i]);
    cmul(cfTemp2[i],cfTemp2[i],u);
    u.re = cfTemp2[i].re;
    u.im = cfTemp2[i].im;
    cfTemp2[i].re = (u.re/(cepAmag[i]*cepAmag[i]));
    cfTemp2[i].im = (u.im/(cepAmag[i]*cepAmag[i]));
    modGd[i] = cfTemp2[i].re;
    if (i > 1)
      modGd[nfft-i+2] = modGd[i];
  }
  /* Remove DC component of modGd */

  /* RemoveAverage(modGd,nfft,&Ave); */

  /* Reduce dynamic range of modGd if necessary */
    for (i = 1; i <= nfft; i++) {
      if (modGd[i] < 0) 
	sign = -1;
      else
	sign = 1;
      abscfTemp = fabs(modGd[i]);
      if (abscfTemp == 0)
	modGd[i] = 0;
      else {
	logcfTemp = log(abscfTemp);
	if ((alfaN == 0) && (alfaP == 0))
	  if (gdsign == 1)
	    modGd[i] = sign*logcfTemp;
	  else
	    modGd[i] = logcfTemp;
	else {
	  if (sign > 0.0)
	    modGd[i] = exp(alfaP*logcfTemp);
	  else 
	    modGd[i] = exp(alfaN*logcfTemp);
	  if (gdsign == 1)
	    modGd[i] = sign*modGd[i];
	}
      }
    }  
    /* Bandlimit modGd */
    if (removeLPhase) {
      for (i = 2; i < nfBy2; i++) {
	modGd[i] = modGd[i] - modGd[1];
	modGd[nfft - i + 2] = modGd[i];
      }
      modGd[1] = 0;
    }
    if (medOrder != 0) {
      MedianSmoothArray(modGd, nfBy2, medOrder, modGdSmth);
      for (i = 2; i <= nfBy2; i++)
	modGdSmth[nfft - i + 2] = modGdSmth[i]; 
      modGd = modGdSmth;
    }
    minValue = modGd[IminActual(modGd, nfft)];
    if (removeMin) 
      for (i = 1; i <nfft; i++) 
        modGd[i] = modGd[i] - minValue;
      
    if ((startIndex > 1) || (endIndex < nfBy2)) {
      for (i = 1; i < startIndex; i++) { 
	modGd[i] = 0.0;
	if (i > 1)
	  modGd[nfft - i+2] = modGd[i];
      }
      for (i = endIndex+1; i <= nfBy2; i++) {
	modGd[i] = 0;
	modGd[nfft - i+ 2] = 0;
      }
    }
    if (mgdNormalize == 1) {
      CepSmooth(amag, cepAmag, mfft, nfft, smthWinSize, &c0, 1.0);  
      for (i = 1; i <= nfft; i++)
        cepAmag[i] = log(cepAmag[i]);    
      NormalizeModGd(modGd, cepAmag, ax, nfft, mfft);
    } else 
      Rfft(modGd, ax, ay, mfft, nfft, 1);
    for (i = 1; i <= winlen; i++) 
      modGdCepstrum[i-1] = ax[i];
    return(modGdCepstrum);  
}
/****************************************************************************
*	The following function computes  the
*	modified group delay cepstrum (ncn) for the given signal
*
*	inputs : signal - npts
*	mfft : fft order nfft = 2**mfft
*	winlen : window length for zero spectrum
*
*	Outputs :
*	modGdCepstrum(winlen) - modified group delay cepstrum
*****************************************************************************
*/
float *ProductGdCepstrumNcN(float *signal,int npts,int nfft,int mfft, 
			int winlen, int smthWinSize, float *prodGdCepstrum, 
			float alfaP, float alfaN,  
			float gamma, int gdsign, 
			int removeLPhase, int removeMin, int mgdNormalize,
			int startIndex, int endIndex) { 
  static float		*sigTemp;
  static float		*ax, *ay, *amag, *phase, *prodGd;
  static float		*cepAmag;
  static int            nfBy2, flag = 0;
  static complex        *cSig, *cfSig;
  static complex	*cfTemp1, *cfTemp2, u;
  int		        i, sign;
  float		        c0, abscfTemp, logcfTemp;


  if (flag == 0) {
    nfBy2 = nfft/2;
    ax = (float *) calloc(nfft+1, sizeof(float));
    ay = (float *) calloc(nfft+1, sizeof(float));
    amag = (float *) calloc(nfft+1, sizeof(float));
    cepAmag = (float *) calloc(nfft+1, sizeof(float));
    phase = (float *) calloc(nfft+1, sizeof(float));
    prodGd = (float *) calloc(nfft+1, sizeof(float));
    cSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp1 = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp2 = (complex *) calloc (nfft+1, sizeof(complex));
    sigTemp = (float *) calloc(nfft+1, sizeof(float));
    flag = 1;
  }
  for (i = 1; i <= npts; i++)
    sigTemp[i] = signal[i];
  for(i = npts+1; i<= nfft; i++)
    sigTemp[i] = 0.0;
  for (i = 1; i < npts; i++){
    u.re = sigTemp[i];
    u.im = 0.0;
    cSig[i] = u;
    u.re = (float)(i-1);
    u.im = 0.0;
    cmul(cfSig[i],u,cSig[i]);
  }
  for (i = npts+1; i <= nfft; i++) {
    cSig[i].re = 0.0;
    cSig[i].im = 0.0;
    cfSig[i].re = 0.0;
    cfSig[i].im = 0.0;
  }
  Cfft(cSig,cfTemp1,mfft,nfft,-1);
  Cfft(cfSig,cfTemp2,mfft,nfft,-1);
  Rfft(sigTemp,ax,ay,mfft,nfft,-1);
  SpectrumReal(nfft,ax,ay,amag,phase);
  for( i = 1; i <= nfft; i++)
    ax[i] = amag[i];
  CepSmooth(ax,amag,mfft,nfft, smthWinSize, &c0, 2*gamma);
  MedianSmoothArray(amag, nfft, 3, cepAmag);
  for (i = 1; i <= nfBy2+1; i++){
    conjg(u,cfTemp1[i]);
    cmul(cfTemp2[i],cfTemp2[i],u);
    u.re = cfTemp2[i].re;
    u.im = cfTemp2[i].im;
    cfTemp2[i].re = u.re;
    cfTemp2[i].im = u.im;
    prodGd[i] = cfTemp2[i].re;
    if (i > 1)
      prodGd[nfft-i+2] = prodGd[i];
  }
/* Reduce dynamic range of prodGd if necessary */
    for (i = 1; i <= nfft; i++) {
      if (prodGd[i] < 0) 
	sign = -1;
      else
	sign = 1;
      abscfTemp = fabs(prodGd[i]);
      if (abscfTemp == 0)
	prodGd[i] = 0;
      else {
	logcfTemp = log(abscfTemp);
	if ((alfaN == 0) && (alfaP == 0))
	  if (gdsign == 1)
	    prodGd[i] = sign*logcfTemp;
	  else
	    prodGd[i] = logcfTemp;
	else {
	  if (sign > 0.0)
	    prodGd[i] = exp(alfaP*logcfTemp);
	  else 
	    prodGd[i] = exp(alfaN*logcfTemp);
	  if (gdsign == 1)
	    prodGd[i] = sign*prodGd[i];
	}
      }
    }  
  /* Bandlimit the signal using minFrequency (startIndex) and maxFrequency (endIndex) */
    if ((startIndex > 1) || (endIndex < nfBy2)) {
      for (i = 1; i < startIndex; i++) { 
	prodGd[i] = 0.0;
	if (i > 1)
	  prodGd[nfft - i+2] = prodGd[i];
      }
      for (i = endIndex+1; i <= nfBy2; i++) {
	prodGd[i] = 0;
	if (i > 1)
	  prodGd[nfft - i+ 2] = 0;
      }
    }
    Rfft(prodGd, ax, ay, mfft, nfft, 1);
    for (i = 1; i <= winlen; i++) 
      prodGdCepstrum[i-1] = ax[i];
    return(prodGdCepstrum);  
}

/****************************************************************************
*	The following function computes  the
*	modified group delay cepstrum (ncn) for the given signal
*
*	inputs : signal - npts
*	mfft : fft order nfft = 2**mfft
*	winlen : window length for zero spectrum
*
*	Outputs :
*	modGdCepstrum(winlen) - modified group delay cepstrum
*****************************************************************************
*
*/
float *ModGdCepstrum(float *signal,int npts,int nfft,int mfft, 
		     int winlen, int smthWinSize, float *modGdCepstrum, float alfaP, float alfaN,  
		     float gamma, int gdsign, int removeLPhase, int removeMin, int startIndex, int endIndex) { 

static  float                     *modGdCepTemp;
static  int                       flag = 0;
int                               i;
if (flag == 0) {
  modGdCepTemp = (float *) calloc(winlen+1, sizeof(float));
  flag = 1;
}

modGdCepTemp = ModGdCepstrumNcN (signal, npts, nfft, mfft, winlen+1, smthWinSize,
				 modGdCepTemp, alfaP, alfaN, gamma, gdsign, 
                                 removeLPhase, removeMin, 0, 0, 
				 startIndex, endIndex);
for (i = 0; i < winlen; i++)
  modGdCepstrum[i] = modGdCepTemp[i+1]/(float)(i+1);
return(modGdCepstrum);
}
/****************************************************************************
*	The following function computes  the
*	modified group delay cepstrum  from signal where denominator
*       smoothing is performed using LP for the given signal using DCT
*
*	inputs : signal - npts
*	mfft : fft order nfft = 2**mfft
*	winlen : window length for zero spectrum
*
*	Outputs :
*	modGdCepstrum(winlen) - modified group delay cepstrum
*****************************************************************************
*
*/
float *ModGdCepstrum_LPDCT(float *signal,int npts,int nfft,int mfft, 
		     int winlen, int lpOrder, float *modGdCepstrum, float alfaP, float alfaN,  
		     float gamma, int gdsign, int removeLPhase, int startIndex, int endIndex, float scaleDCT) { 
  static float		     *sigTemp, *resEnergy;
  static float		     *ax, *ay, *amag, *phase, *modGd;
  static float		     *lpAmag, *coef;
  static int                 nfBy2, flag = 0;
  static int                 DCTSize;
  static complex             *cSig, *cfSig;
  static complex	     *cfTemp1, *cfTemp2, u;
  static VECTOR_OF_F_VECTORS *discreteCosineTransform;
  static F_VECTOR            *fvect, *modGdFvect;
  int		              i, sign;
  float		              abscfTemp, gain, 
                              logcfTemp, lpAmagGamma;
  
  if (flag == 0) {
    nfBy2 = nfft/2;
    ax = (float *) calloc(nfft+1, sizeof(float));
    ay = (float *) calloc(nfft+1, sizeof(float));
    amag = (float *) calloc(nfft+1, sizeof(float));
    lpAmag = (float *) calloc(nfft+1, sizeof(float));
    coef = (float *) calloc(lpOrder+1, sizeof(float));
    phase = (float *) calloc(nfft+1, sizeof(float));
    modGd = (float *) calloc(nfft+1, sizeof(float));
    cSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp1 = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp2 = (complex *) calloc (nfft+1, sizeof(complex));
    sigTemp = (float *) calloc(npts+1, sizeof(float));
    resEnergy = (float *) calloc(npts+1, sizeof(float));
    DCTSize = endIndex - startIndex + 1;
    discreteCosineTransform = (VECTOR_OF_F_VECTORS *) GeneratePseudoDct(1, winlen, DCTSize);
    fvect = (F_VECTOR *) AllocFVector(nfBy2);
    modGdFvect = AllocFVector(winlen);
    flag = 1;
  }
  for (i = 1; i <= npts; i++)
    sigTemp[i] = signal[i];
  //  for(i = npts+1; i<= nfft; i++)
  //  sigTemp[i] = 0.0;
  for (i = 1; i < npts; i++){
    u.re = sigTemp[i];
    u.im = 0.0;
    cSig[i] = u;
    u.re = (float)(i-1);
    u.im = 0.0;
    cmul(cfSig[i],u,cSig[i]);
  }
  for (i = npts+1; i <= nfft; i++) {
    cSig[i].re = 0.0;
    cSig[i].im = 0.0;
    cfSig[i].re = 0.0;
    cfSig[i].im = 0.0;
  }
  Cfft(cSig,cfTemp1,mfft,nfft,-1);
  Cfft(cfSig,cfTemp2,mfft,nfft,-1);
  //  Rfft(sigTemp,ax,ay,mfft,nfft,-1);
  //SpectrumReal(nfft,ax,ay,amag,phase);
  //for( i = 1; i <= nfft; i++)
  //  ax[i] = amag[i];
  //CepSmooth(ax,cepAmag,mfft,nfft, smthWinSize, &c0, 2*gamma);
  LpAnal(sigTemp, resEnergy, npts, 0, coef, lpOrder, &gain);
  LPSpectrum (coef, lpOrder, lpAmag, phase, nfft, mfft, gain);
  for (i = 1; i <= nfBy2+1; i++){
    conjg(u,cfTemp1[i]);
    cmul(cfTemp2[i],cfTemp2[i],u);
    u.re = cfTemp2[i].re;
    u.im = cfTemp2[i].im;
    lpAmagGamma = exp(2*gamma*log(lpAmag[i]));
    cfTemp2[i].re = u.re/lpAmagGamma;
    cfTemp2[i].im = u.im/lpAmagGamma;
    modGd[i] = cfTemp2[i].re;
    if (i > 1)
      modGd[nfft-i+2] = modGd[i];
  }
  /* Remove DC component of modGd */

  /*RemoveAverage(modGd,nfft,&Ave); */

  /* Reduce dynamic range of modGd if necessary */
    for (i = 1; i <= nfft; i++) {
      if (modGd[i] < 0) 
	sign = -1;
      else
	sign = 1;
      abscfTemp = fabs(modGd[i]);
      if (abscfTemp == 0)
	modGd[i] = 0;
      else {
	logcfTemp = log(abscfTemp);
	if ((alfaN == 0) && (alfaP == 0))
	  if (gdsign == 1)
	    modGd[i] = sign*logcfTemp;
	  else
	    modGd[i] = logcfTemp;
	else {
	  if (sign > 0.0)
	    modGd[i] = exp(alfaP*logcfTemp);
	  else 
	    modGd[i] = exp(alfaN*logcfTemp);
	  if (gdsign == 1)
	    modGd[i] = sign*modGd[i];
	}
      }
    }   
    if (removeLPhase) {
      for (i = 2; i < nfBy2; i++) {
	modGd[i] = modGd[i] - modGd[1];
	modGd[nfft - i +2] = modGd[i];
      }
      modGd[1] = 0;
    }
    /* Bandlimit modGd */

    if ((startIndex > 1) || (endIndex < nfBy2)) {
      for (i = 1; i < startIndex; i++) { 
	modGd[i] = 0.0;
	if (i > 1)
	  modGd[nfft - i+2] = modGd[i];
      }
      for (i = endIndex+1; i <= nfBy2; i++) {
	modGd[i] = 0;
	if (i > 1)
	  modGd[nfft - i+ 2] = 0;
      }
    }
  for (i = startIndex; i <= endIndex;i++)
    fvect->array[i- startIndex] = modGd[i - startIndex];
  LinearTransformationOfFVector(fvect, discreteCosineTransform, winlen, DCTSize, modGdFvect);
  for (i = 1; i <= winlen; i++) 
    modGdCepstrum[i-1] = modGdFvect->array[i-1]/(float)scaleDCT;
  return(modGdCepstrum);  
}

/****************************************************************************
*	The following function computes  the
*	modified group delay cepstrum  for the given signal using DCT
*
*	inputs : signal - npts
*	mfft : fft order nfft = 2**mfft
*	winlen : window length for zero spectrum
*
*	Outputs :
*	modGdCepstrum(winlen) - modified group delay cepstrum
*****************************************************************************
*
*/
float *ModGdCepstrum_DCT(float *signal,int npts,int nfft,int mfft, 
		     int winlen, int smthWinSize, float *modGdCepstrum, float alfaP, float alfaN,  
		     float gamma, int gdsign, int removeLPhase, int startIndex, int endIndex, float scaleDCT) { 
  static float		     *sigTemp;
  static float		     *ax, *ay, *amag, *phase, *modGd;
  static float		     *cepAmag;
  static int                 nfBy2, flag = 0;
  static int                 DCTSize;
  static complex             *cSig, *cfSig;
  static complex	     *cfTemp1, *cfTemp2, u;
  static VECTOR_OF_F_VECTORS *discreteCosineTransform;
  static F_VECTOR            *fvect, *modGdFvect;
  int		              i, sign;
  float		              c0, abscfTemp, 
                              logcfTemp;
  
  if (flag == 0) {
    nfBy2 = nfft/2;
    ax = (float *) calloc(nfft+1, sizeof(float));
    ay = (float *) calloc(nfft+1, sizeof(float));
    amag = (float *) calloc(nfft+1, sizeof(float));
    cepAmag = (float *) calloc(nfft+1, sizeof(float));
    phase = (float *) calloc(nfft+1, sizeof(float));
    modGd = (float *) calloc(nfft+1, sizeof(float));
    cSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp1 = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp2 = (complex *) calloc (nfft+1, sizeof(complex));
    sigTemp = (float *) calloc(nfft+1, sizeof(float));
    DCTSize = endIndex - startIndex + 1;
    discreteCosineTransform = (VECTOR_OF_F_VECTORS *) GeneratePseudoDct(1, winlen, DCTSize);
    fvect = (F_VECTOR *) AllocFVector(nfBy2);
    modGdFvect = AllocFVector(winlen);
    flag = 1;
  }
  for (i = 1; i <= npts; i++)
    sigTemp[i] = signal[i];
  for(i = npts+1; i<= nfft; i++)
    sigTemp[i] = 0.0;
  for (i = 1; i < npts; i++){
    u.re = sigTemp[i];
    u.im = 0.0;
    cSig[i] = u;
    u.re = (float)(i-1);
    u.im = 0.0;
    cmul(cfSig[i],u,cSig[i]);
  }
  for (i = npts+1; i <= nfft; i++) {
    cSig[i].re = 0.0;
    cSig[i].im = 0.0;
    cfSig[i].re = 0.0;
    cfSig[i].im = 0.0;
  }
  Cfft(cSig,cfTemp1,mfft,nfft,-1);
  Cfft(cfSig,cfTemp2,mfft,nfft,-1);
  Rfft(sigTemp,ax,ay,mfft,nfft,-1);
  SpectrumReal(nfft,ax,ay,amag,phase);
  for( i = 1; i <= nfft; i++)
    ax[i] = amag[i];
  CepSmooth(ax, amag,mfft,nfft, smthWinSize, &c0, 2*gamma);
  MedianSmoothArray(amag, nfBy2, 3, cepAmag);
  for (i = 1; i <= nfBy2+1; i++){
    conjg(u,cfTemp1[i]);
    cmul(cfTemp2[i],cfTemp2[i],u);
    u.re = cfTemp2[i].re;
    u.im = cfTemp2[i].im;
    cfTemp2[i].re = u.re/cepAmag[i];
    cfTemp2[i].im = u.im/cepAmag[i];
    modGd[i] = cfTemp2[i].re;
    if (i > 1)
      modGd[nfft-i+2] = modGd[i];
  }
  /* Remove DC component of modGd */

  /*RemoveAverage(modGd,nfft,&Ave); */

  /* Reduce dynamic range of modGd if necessary */
    for (i = 1; i <= nfft; i++) {
      if (modGd[i] < 0) 
	sign = -1;
      else
	sign = 1;
      abscfTemp = fabs(modGd[i]);
      if (abscfTemp == 0)
	modGd[i] = 0;
      else {
	logcfTemp = log(abscfTemp);
	if ((alfaN == 0) && (alfaP == 0))
	  if (gdsign == 1)
	    modGd[i] = sign*logcfTemp;
	  else
	    modGd[i] = logcfTemp;
	else {
	  if (sign > 0.0)
	    modGd[i] = exp(alfaP*logcfTemp);
	  else 
	    modGd[i] = exp(alfaN*logcfTemp);
	  if (gdsign == 1)
	    modGd[i] = sign*modGd[i];
	}
      }
    }   
    if (removeLPhase) {
      for (i = 2; i < nfBy2; i++) {
	modGd[i] = modGd[i] - modGd[1];
	modGd[nfft - i +2] = modGd[i];
      }
      modGd[1] = 0;
    }
    /* Bandlimit modGd */

    if ((startIndex > 1) || (endIndex < nfBy2)) {
      for (i = 1; i < startIndex; i++) { 
	modGd[i] = 0.0;
	if (i > 1)
	  modGd[nfft - i+2] = modGd[i];
      }
      for (i = endIndex+1; i <= nfBy2; i++) {
	modGd[i] = 0;
	if (i > 1)
	  modGd[nfft - i+ 2] = 0;
      }
    }
  for (i = startIndex; i < endIndex;i++)
    fvect->array[i- startIndex] = modGd[i - startIndex];
  LinearTransformationOfFVector(fvect, discreteCosineTransform, winlen, DCTSize, modGdFvect);
  for (i = 1; i <= winlen; i++) 
    modGdCepstrum[i-1] = modGdFvect->array[i-1]/(float)scaleDCT;
  return(modGdCepstrum);  
}

/****************************************************************************
*	The following function computes  the
*	modified group delay functions from ModGdCepstrum using DCT
*
*	inputs : signal - npts
*	mfft : fft stages nfft = 2**mfft
*	winlen : window length for zero spectrum and smoothing
*
*	Outputs :
*	modGd(nfft) - smoothed modified group delay function
*****************************************************************************
*
*/
float *ModGd_DCT(float *signal,int npts,int nfft,int mfft, 
	     int winlen, int smthWinSize, float *modGd, float alfaP, float alfaN, float gamma, int gdsign, int removeLPhase,
	     int startIndex, int endIndex, float scaleDCT) {
  static float		*ax, *ay, *modGdCepstrum;
  static int            flag = 0;
  int		        i;


  if (flag == 0) {
    ax = (float *) calloc(nfft+1, sizeof(float));
    ay = (float *) calloc(nfft+1, sizeof(float));
    modGdCepstrum = (float *) calloc(winlen, sizeof(float));
    flag = 1;
  }
  modGdCepstrum = (float *) ModGdCepstrum_DCT(signal, npts, nfft, 
					  mfft, winlen, smthWinSize, modGdCepstrum, alfaP, alfaN, gamma, 
					      gdsign, removeLPhase, startIndex, endIndex, scaleDCT);
  ax[1] = 0.0;
  for (i = 2; i <= winlen; i++) {
    ax[i] = modGdCepstrum[i-2]*(i-1);
    ax[nfft-i+2] = ax[i];
  }
  for (i = winlen+1; i <= nfft-(winlen-1); i++)
    ax[i] = 0.0;
  Rfft(ax,modGd,ay,mfft,nfft,-1);
  return(modGd);
}

/****************************************************************************
*	The following function computes  the standard
*	modified group delay functions for the given signal
*       The denominator is replaced by the LPSpectrum
*
*	inputs : signal - npts
*	mfft : fft stages nfft = 2**mfft
*	winlen : window length for zero spectrum and smoothing
*
*       gamma: smooth spectrum power
*       gdPosScale, gdNegScale: modgd power for positive and negative values
*	Outputs :
*	modGd(nfft) - smoothed modified group delay function
*****************************************************************************
*
*/
float *StandardModGdLP(float *signal,int npts,int nfft,int mfft, int lpOrder,
                     float gamma, float gdPosScale, float gdNegScale,  
		     int medOrder, float *modGdSmth) {
  static float		*sigTemp, *resEnergy;
  static float		*ax, *ay, *amag, *phase, *modGd;
  static float		*lpAmag, *coef, lpAmagGamma;
  static int            nfBy2, flag = 0;
  static complex        *cSig, *cfSig;
  static complex	*cfTemp1, *cfTemp2, u;
  int		        i;
  float		        tempVal, gain;
  
  if (flag == 0) {
    nfBy2 = nfft/2;
    ax = (float *) calloc(nfft+1, sizeof(float));
    ay = (float *) calloc(nfft+1, sizeof(float));
    amag = (float *) calloc(nfft+1, sizeof(float));
    lpAmag = (float *) calloc(nfft+1, sizeof(float));
    coef = (float *) calloc(lpOrder+1, sizeof(float));
    phase = (float *) calloc(nfft+1, sizeof(float));
    modGd = (float *) calloc(nfft+1, sizeof(float));
    cSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp1 = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp2 = (complex *) calloc (nfft+1, sizeof(complex));
    sigTemp = (float *) calloc(nfft+1, sizeof(float));
    resEnergy = (float *) calloc(npts+1, sizeof(float));
    flag = 1;
  }
  for (i = 1; i <= npts; i++)
    sigTemp[i] = signal[i];
  for(i = npts+1; i<= nfft; i++)
    sigTemp[i] = 0.0;
  for (i = 1; i < npts; i++){
    u.re = sigTemp[i];
    u.im = 0.0;
    cSig[i] = u;
    u.re = (float)(i-1);
    u.im = 0.0;
    cmul(cfSig[i],u,cSig[i]);
  }
  for (i = npts+1; i <= nfft; i++) {
    cSig[i].re = 0.0;
    cSig[i].im = 0.0;
    cfSig[i].re = 0.0;
    cfSig[i].im = 0.0;
  }
  Cfft(cSig,cfTemp1,mfft,nfft,-1);
  Cfft(cfSig,cfTemp2,mfft,nfft,-1);
  //  Rfft(sigTemp,ax,ay,mfft,nfft,-1);
  //SpectrumReal(nfft,ax,ay,amag,phase);
  //for( i = 1; i <= nfft; i++)
  //  ax[i] = amag[i];
  //CepSmooth(ax, cepAmag, mfft, nfft, smthWinSize, &c0, 2*gamma);
 LpAnal(sigTemp, resEnergy, npts, 0, coef, lpOrder, &gain);
  LPSpectrum (coef, lpOrder, amag, phase, nfft, mfft, gain);
  MedianSmoothArray(amag, nfft, 5, lpAmag);
  for (i = 2; i <= nfBy2; i++){
    conjg(u,cfTemp1[i]);
    cmul(cfTemp2[i],cfTemp2[i],u);
    u.re = cfTemp2[i].re;
    u.im = cfTemp2[i].im;
    lpAmagGamma = exp(2*gamma*log(lpAmag[i]));
    cfTemp2[i].re = u.re/lpAmagGamma;
    cfTemp2[i].im = u.im/lpAmagGamma;
    tempVal = fabs(cfTemp2[i].re);
    if (tempVal != 0.0) {
      cfTemp2[i].re = cfTemp2[i].re/tempVal;
      if (cfTemp2[i].re > 0)
	cfTemp2[i].re = cfTemp2[i].re*exp(gdPosScale*log(tempVal));
      else
	cfTemp2[i].re = cfTemp2[i].re*exp(gdNegScale*log(tempVal));
    } else 
      cfTemp2[i].re = 0;
    modGd[i] = cfTemp2[i].re;
  }
  conjg(u,cfTemp1[1]);
  cmul(cfTemp2[1],cfTemp2[1],u);
  if (cabs2(cfTemp1[1]) == 0.0){ 
    cfTemp2[1].re = cfTemp2[1].re/0.0001;
    cfTemp2[1].im = cfTemp2[1].im/0.0001;
  }
  else{
    cfTemp2[1].re = cfTemp2[1].re/exp(2*gamma*log(lpAmag[1]));
    cfTemp2[1].im = cfTemp2[1].im/exp(2*gamma*log(lpAmag[1]));
  }
  tempVal = fabs(cfTemp2[1].re);
  if (tempVal != 0.0) {
    cfTemp2[1].re = cfTemp2[1].re/tempVal;
    if (cfTemp2[1].re > 0)
      cfTemp2[1].re = cfTemp2[1].re*exp(gdPosScale*log(tempVal));
    else
      cfTemp2[1].re = cfTemp2[1].re*exp(gdNegScale*log(tempVal));
  } else 
    cfTemp2[1].re = 0;					    
    modGd[1] = cfTemp2[1].re;
    if (medOrder != 0) 
      MedianSmoothArray(modGd, nfft, medOrder, modGdSmth);
    else
      for (i = 1; i <= nfft; i++)
        modGdSmth[i] = modGd[i];
    return(modGdSmth);
}

/****************************************************************************
*	The following function computes  the standard
*	modified group delay functions for the given signal
*
*	inputs : signal - npts
*	mfft : fft stages nfft = 2**mfft
*	winlen : window length for zero spectrum and smoothing
*
*       gamma: smooth spectrum power
*       gdPosScale, gdNegScale: modgd power for positive and negative values
*	Outputs :
*	modGd(nfft) - smoothed modified group delay function
*****************************************************************************
*
*/
float *StandardModGd(float *signal,int npts,int nfft,int mfft, int smthWinSize,
                     float gamma, float gdPosScale, float gdNegScale,  
		     int medOrder, float *modGdSmth) {
  static float		*sigTemp;
  static float		*ax, *ay, *amag, *phase, *modGd;
  static float		*cepAmag;
  static int            nfBy2, flag = 0;
  static complex        *cSig, *cfSig;
  static complex	*cfTemp1, *cfTemp2, u;
  int		        i;
  float		        c0, tempVal;
  
  if (flag == 0) {
    nfBy2 = nfft/2;
    ax = (float *) calloc(nfft+1, sizeof(float));
    ay = (float *) calloc(nfft+1, sizeof(float));
    amag = (float *) calloc(nfft+1, sizeof(float));
    cepAmag = (float *) calloc(nfft+1, sizeof(float));
    phase = (float *) calloc(nfft+1, sizeof(float));
    modGd = (float *) calloc(nfft+1, sizeof(float));
    cSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfSig = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp1 = (complex *) calloc (nfft+1, sizeof(complex));
    cfTemp2 = (complex *) calloc (nfft+1, sizeof(complex));
    sigTemp = (float *) calloc(nfft+1, sizeof(float));
    flag = 1;
  }
  for (i = 1; i <= npts; i++)
    sigTemp[i] = signal[i];
  for(i = npts+1; i<= nfft; i++)
    sigTemp[i] = 0.0;
  for (i = 1; i < npts; i++){
    u.re = sigTemp[i];
    u.im = 0.0;
    cSig[i] = u;
    u.re = (float)(i-1);
    u.im = 0.0;
    cmul(cfSig[i],u,cSig[i]);
  }
  for (i = npts+1; i <= nfft; i++) {
    cSig[i].re = 0.0;
    cSig[i].im = 0.0;
    cfSig[i].re = 0.0;
    cfSig[i].im = 0.0;
  }
  Cfft(cSig,cfTemp1,mfft,nfft,-1);
  Cfft(cfSig,cfTemp2,mfft,nfft,-1);
  Rfft(sigTemp,ax,ay,mfft,nfft,-1);
  SpectrumReal(nfft,ax,ay,amag,phase);
  for( i = 2; i <= nfft; i++)
    ax[i] = amag[i];
  CepSmooth(ax, amag, mfft, nfft, smthWinSize, &c0, 2*gamma);
  MedianSmoothArray(amag, nfft, 5, cepAmag);
  for (i = 1; i <= nfBy2; i++){
    conjg(u,cfTemp1[i]);
    cmul(cfTemp2[i],cfTemp2[i],u);
    u.re = cfTemp2[i].re;
    u.im = cfTemp2[i].im;
    cfTemp2[i].re = u.re/cepAmag[i];
    cfTemp2[i].im = u.im/cepAmag[i];
    tempVal = fabs(cfTemp2[i].re);
    if (tempVal != 0.0) {
      cfTemp2[i].re = cfTemp2[i].re/tempVal;
      if (cfTemp2[i].re > 0)
	cfTemp2[i].re = cfTemp2[i].re*exp(gdPosScale*log(tempVal));
      else
	cfTemp2[i].re = cfTemp2[i].re*exp(gdNegScale*log(tempVal));
    } else 
      cfTemp2[i].re = 0;
    modGd[i] = cfTemp2[i].re;
  }
  conjg(u,cfTemp1[1]);
  cmul(cfTemp2[1],cfTemp2[1],u);
  if (cabs2(cfTemp1[1]) == 0.0){ 
    cfTemp2[1].re = cfTemp2[1].re/0.0001;
    cfTemp2[1].im = cfTemp2[1].im/0.0001;
  }
  else{
    cfTemp2[1].re = cfTemp2[1].re/cepAmag[1];
    cfTemp2[1].im = cfTemp2[1].im/cepAmag[1];
  }
  tempVal = fabs(cfTemp2[1].re);
  if (tempVal != 0.0) {
    cfTemp2[1].re = cfTemp2[1].re/tempVal;
    if (cfTemp2[1].re > 0)
      cfTemp2[1].re = cfTemp2[1].re*exp(gdPosScale*log(tempVal));
    else
      cfTemp2[1].re = cfTemp2[1].re*exp(gdNegScale*log(tempVal));
  } else 
    cfTemp2[1].re = 0;					    
    modGd[1] = cfTemp2[1].re;
    if (medOrder != 0) 
      MedianSmoothArray(modGd, nfBy2, medOrder, modGdSmth);
    else
      for (i = 1; i <= nfft; i++)
        modGdSmth[i] = modGd[i];
    return(modGdSmth);
}


/****************************************************************************
*	The following function computes  the ModGdCepstrum from 
*       the group delay function
*
*	inputs : signal - npts
*	mfft : fft stages nfft = 2**mfft
*	winlen : window length for zero spectrum and smoothing
*
*	Outputs :
*	modGd(nfft) - ModGd Cepstra
*****************************************************************************
*
*/
float *ModGdCepstrumFromGD(float *modGd,int nfft,
	     int winlen, float *modGdCepstrum, float alfaP, float alfaN,  
		     float gamma, int gdsign, int removeLPhase, int startIndex, int endIndex, float scaleDCT) {
  static float		     *ax, *ay;
  static int                 flag = 0, nfBy2;
  int		             i;
  int                        sign;
  float                      abscfTemp, logcfTemp;
  static VECTOR_OF_F_VECTORS *discreteCosineTransform;
  static F_VECTOR            *fvect, *modGdFvect;

  if (flag == 0) {
    nfBy2 = nfft/2;
    ax = (float *) calloc(nfft+1, sizeof(float));
    ay = (float *) calloc(nfft+1, sizeof(float));
    discreteCosineTransform = (VECTOR_OF_F_VECTORS *) GeneratePseudoDct(1, winlen, nfBy2);
    fvect = (F_VECTOR *) AllocFVector(nfBy2);
    modGdFvect = (F_VECTOR *) AllocFVector(winlen);
    flag = 1;
  }

  /* RemoveAverage(modGd,nfft,&Ave); */


  /* Reduce dynamic range of modGd if necessary */
    for (i = 0; i < nfBy2; i++) {
      if (modGd[i] < 0) 
	sign = -1;
      else
	sign = 1;
      abscfTemp = fabs(modGd[i]);
      if (abscfTemp == 0)
	modGd[i] = 0;
      else {
	logcfTemp = log(abscfTemp);
	if ((alfaN == 0) && (alfaP == 0))
	  if (gdsign == 1)
	    modGd[i] = sign*logcfTemp;
	  else
	    modGd[i] = logcfTemp;
	else {
	  if (sign > 0.0)
	    modGd[i] = exp(alfaP*logcfTemp);
	  else 
	    modGd[i] = exp(alfaN*logcfTemp);
	  if (gdsign == 1)
	    modGd[i] = sign*modGd[i];
	}
      }
    }   
    /* Bandlimit modGd */
    if (removeLPhase) {
      for (i = 2; i < nfBy2; i++) {
	modGd[i] = modGd[i] - modGd[1];
	modGd[nfft - i +2] = modGd[i];
      }
      modGd[1] = 0;
    }

    if ((startIndex > 0) || (endIndex < nfBy2-1)) {
      for (i = 1; i < startIndex; i++)  
	modGd[i] = 0.0;
      for (i = endIndex+1; i <= nfBy2; i++) 
	modGd[i] = 0;
    }
  for (i = 0; i < nfBy2;i++)
    fvect->array[i] = modGd[i];
  LinearTransformationOfFVector(fvect, discreteCosineTransform, winlen, nfBy2, modGdFvect);
  for (i = 0; i < winlen; i++) 
    modGdCepstrum[i] = modGdFvect->array[i]/(float)(i+1)/(float)scaleDCT;
  return(modGdCepstrum);  
}


/****************************************************************************
*	The following function computes  the ModGdCepstrum from 
*       the Standard modgroupdelay cepstra
*
*	inputs : signal - winlen
*	mfft : fft stages nfft = 2**mfft
*	winlen : window length for zero spectrum and smoothing
*
*	Outputs :
*	modGd(nfft) - ModGd Cepstra
*****************************************************************************
*
*/
float *ModGdCepstrumFromStdCepstra(float *inModGdCepstrum,int nfft, int mfft, 
	     int winlen, float *modGdCepstrum, float alfaP, float alfaN,  
		     float gamma, int gdsign, int removeLPhase, int startIndex, int endIndex, float scaleDCT) {
  static float		     *ax, *ay, *modGdTemp, *modGd;
  static int                 flag = 0, nfBy2;
  int		             i;
  int                        sign;
  float                      abscfTemp, logcfTemp;
  static VECTOR_OF_F_VECTORS *discreteCosineTransform;
  static F_VECTOR            *fvect, *modGdFvect;

  if (flag == 0) {
    nfBy2 = nfft/2;
    ax = (float *) calloc(nfft+1, sizeof(float));
    ay = (float *) calloc(nfft+1, sizeof(float));
    discreteCosineTransform = (VECTOR_OF_F_VECTORS *) GeneratePseudoDct(1, winlen, nfBy2);
    modGd = (float *) calloc(nfft+1, sizeof(float));
    modGdTemp = (float *) calloc(nfft+1, sizeof(float));
    fvect = (F_VECTOR *) AllocFVector(nfBy2);
    modGdFvect = (F_VECTOR *) AllocFVector(winlen);
    
    flag = 1;
  }

  ax[1] = inModGdCepstrum[0];
  for (i = 2; i <= winlen+1; i++) {
    ax[i] = inModGdCepstrum[i-1];
    ax[nfft-i+2] = ax[i];
  }
  for (i = winlen+2; i <= nfft-(winlen); i++)
    ax[i] = 0.0;
  Rfft(ax,modGdTemp,ay,mfft,nfft,-1);
  for (i = 0; i < nfBy2; i++)
    modGd[i] = modGdTemp[i+1];

  /* RemoveAverage(modGd,nfft,&Ave); */

  /* Reduce dynamic range of modGd if necessary */
    for (i = 0; i < nfBy2; i++) {
      if (modGd[i] < 0) 
	sign = -1;
      else
	sign = 1;
      abscfTemp = fabs(modGd[i]);
      if (abscfTemp == 0)
	modGd[i] = 0;
      else {
	logcfTemp = log(abscfTemp);
	if ((alfaN == 0) && (alfaP == 0))
	  if (gdsign == 1)
	    modGd[i] = sign*logcfTemp;
	  else
	    modGd[i] = logcfTemp;
	else {
	  if (sign > 0.0)
	    modGd[i] = exp(alfaP*logcfTemp);
	  else 
	    modGd[i] = exp(alfaN*logcfTemp);
	  if (gdsign == 1)
	    modGd[i] = sign*modGd[i];
	}
      }
    }   
    /* Bandlimit modGd */
    if (removeLPhase) {
      for (i = 2; i < nfBy2; i++) {
	modGd[i] = modGd[i] - modGd[1];
	modGd[nfft - i +2] = modGd[i];
      }
      modGd[1] = 0;
    }

    if ((startIndex > 0) || (endIndex < nfBy2-1)) {
      for (i = 1; i < startIndex; i++)  
	modGd[i] = 0.0;
      for (i = endIndex+1; i <= nfBy2; i++) 
	modGd[i] = 0;
    }
  for (i = 0; i < nfBy2;i++)
    fvect->array[i] = modGd[i];
  LinearTransformationOfFVector (fvect, discreteCosineTransform, winlen, nfBy2, modGdFvect);
  for (i = 0; i < winlen; i++) 
    modGdCepstrum[i] = modGdFvect->array[i]/(float)(i+1)/(float)scaleDCT;
  return(modGdCepstrum);  
}

/****************************************************************************
*	The following subroutine extracts the pitch Cepstrum from the given signal
*	using minimum phase group delay function 
*
*	inputs : DaTa (signal) npts points long
*	mfft : FFT stages nfft = 2**mfft
*	pthLow, pthHgh : Expected range of pitch values
*	alfa : compression required for magnitude SpectrumRealrum
*       fNum : frameNum upto which rootCepstrum shd be 
*       stored.
*	Outputs :
          pitchMinGgd : pitch value extracted
*****************************************************************************
*
*/
	float *PitchCepstrumMinGd(float *signal,int npts,int nfft,int mfft, 
				  int pthLow, int pthHgh,float alfa, float *pthCepstrum){
	static float	*sigTemp;
        static int      flag = 0;
	static float	*ax, *ay, *amag, *phase;
        static int      nfBy2;
	int		i;
        float           pitch;
        if (flag == 0) {
	  nfBy2 = nfft/2;
	  sigTemp = (float *) AllocFloatArray(sigTemp, nfft+1);
	  ax = (float *) AllocFloatArray(ax, nfft+1);
	  ay = (float *) AllocFloatArray(ay, nfft+1);
	  amag = (float *) AllocFloatArray(amag, nfft+1);
	  phase = (float *) AllocFloatArray(phase, nfft+1);
	  flag = 1;
        }
        fflush(stdout);
	for (i = 1; i <= npts; i++)
	  sigTemp[i] = signal[i];
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	Rfft(sigTemp, ax, ay, mfft, nfft, -1);
	SpectrumReal(nfft, ax, ay, amag, phase);
        fflush(stdout);
	for (i = 1; i <= nfft; i++){
          amag[i] = exp(log(amag[i])*alfa);
	}
	Rfft(amag,sigTemp,ay,mfft,nfft,1);
        for (i = pthLow; i < pthHgh; i++)
          pthCepstrum[i-pthLow] = sigTemp[i];
        return(pthCepstrum);
	}
	 
/****************************************************************************
*	The following subroutine extracts the pitch from the given signal
*	from the log Cepstrum of the given signal
*
*	inputs : DaTa npts points long
*	mfft : FFT stages nfft = 2**mfft
*	pthLow, pthHgh : expected range of pitch values
*
*	Outputs :
*       pitchCepstrum - extracted value of pitch
*****************************************************************************
*
*/
float 	*PitchCepstrumStd(float *signal,int npts,int nfft,int mfft, int pthLow, int pthHgh, float *pthCepstrum) {
	static float		*sigTemp, *Spectrum;
	static float		*ax, *ay,*amag,*phase;
	int		        i;
        static int              nfBy2;
        static int              flag = 0;
        float                   pitch;
        if (flag == 0) {      
          sigTemp = (float *) AllocFloatArray(sigTemp,nfft+1);
	  Spectrum = (float *) AllocFloatArray(Spectrum,nfft+1);
          ax = (float *) AllocFloatArray(ax,nfft+1);
          ay = (float *) AllocFloatArray(ay,nfft+1);
          amag = (float *) AllocFloatArray(amag,nfft+1);
          phase = (float *) AllocFloatArray(phase,nfft+1);
	  nfBy2 = nfft/2;
          flag = 1;
        }
	for (i = 1; i <= npts; i++)
	  sigTemp[i] = signal[i];
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	Rfft(sigTemp, ax, ay, mfft, nfft, -1);
	SpectrumReal(nfft, ax, ay, amag, phase);
	for (i = 1; i <= nfft; i++)
          amag[i] = log(amag[i]);
	Rfft(amag,sigTemp, ay, mfft, nfft, 1);
        for (i = pthLow; i < pthHgh; i++)
          pthCepstrum[i-pthLow] = sigTemp[i];
        return(pthCepstrum);
      }
/****************************************************************************
*	The following subroutine extracts the pitch from the given signal
*	using modified group delay functions
*
*	inputs : DaTa npts points long
*	mfft : FFT stages nfft = 2**mfft
*	WiNLEN : window length for zero SpectrumRealrum
*       pthLow, pthHgh : expected range of pitch values
*
*	Outputs :
*	pitchModgd - value of the pitch picked
*****************************************************************************
*
*/
float *PitchCepstrumModifiedGdLP(float *signal,int npts,int nfft,int mfft, 
			int pthLow,int pthHgh, int winLen, float winScaleFactor, 
			int lpOrder, int medOrder, 
			float gamma, float gdPosScale,
				float gdNegScale, float *pthCepstrum){
	static float		*derv;
        static int              flag = 0, nfBy2;
	static int              *peakArr,  numValues;
        float                   ave, pitch;
        int                     i, npeaks;
	static float		*ax, *ay, *amag, *phase, *cepAmag, *sigTemp;
        static float            c0;

        if (flag == 0) {
	  derv = (float *) AllocFloatArray(derv, nfft+1);
          peakArr = (int *) calloc (nfBy2+1, sizeof(int));
          ax = (float *) AllocFloatArray(ax, nfft+1);
          ay = (float *) AllocFloatArray(ay, nfft+1);
          amag = (float *) AllocFloatArray(amag, nfft+1);
          cepAmag = (float *) AllocFloatArray(cepAmag, nfft+1);
          phase = (float *) AllocFloatArray(phase, nfft+1);
          sigTemp = (float *) AllocFloatArray(sigTemp, nfft+1);
          flag = 1;
	  nfBy2 = nfft/2;
	  numValues = (int) (nfBy2/winScaleFactor);
	}

     	for (i = 1; i <= npts; i++)
	  sigTemp[i] = signal[i]*HamDw(i,npts);
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	Rfft(sigTemp,ax,ay,mfft,nfft,-1);
	SpectrumReal(nfft,ax,ay,amag,phase);
  	for( i = 1; i <= nfft; i++)
	  ax[i] = amag[i];
	CepSmooth(ax,cepAmag,mfft,nfft,winLen,&c0, 1.0);
        for (i = 1; i <= numValues; i++) {
          sigTemp[i] = amag[i]/cepAmag[i];
	}
	for(i = numValues+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
        MedianSmoothArray(sigTemp, numValues, medOrder, signal);
	derv = (float *) StandardModGdLP(signal, numValues, nfft, mfft, lpOrder, gamma, 
					 gdPosScale, gdNegScale, medOrder, derv);
    	Rfft(derv,ax,ay,mfft,nfft,1);
	for (i = 2; i <= winLen; i++) {
	  ax[i] = ax[i]*HanW(i,winLen);
	  ax[nfft-i+2] = ax[i];
	}
	for (i = winLen+1; i <= nfft-winLen-1; i++)
	  ax[i] = 0.0;
	  Rfft(ax,derv,ay,mfft,nfft,-1); 
	RemoveAverage(derv,nfBy2,&ave);
	for (i = pthLow; i <= pthHgh; i++)
          pthCepstrum[i-pthLow] = derv[i];
        return(pthCepstrum);
}

/****************************************************************************
*	The following subroutine extracts the pitch from the given signal
*	using modified group delay functions
*
*	inputs : DaTa npts points long
*	mfft : FFT stages nfft = 2**mfft
*	WiNLEN : window length for zero SpectrumRealrum
*       pthLow, pthHgh : expected range of pitch values
*
*	Outputs :
*	pitchModgd - value of the pitch picked
*****************************************************************************
*
*/
	float *PitchCepstrumModifiedGd(float *signal,int npts,int nfft,int mfft, 
			      int pthLow,int pthHgh, int winLen, float winScaleFactor,
                              int gdSmthWinSize, int medOrder, float gamma, float gdPosScale,
				       float gdNegScale, float *pthCepstrum){
	static float		*sigTemp;
	static float		*ax, *ay, *amag, *phase;
	static float		*cepAmag, *derv;
	static int		nfBy2;
	static complex 	        *cSig, *cFsig;
	static complex		*cfTemp1, *cfTemp2, u;
        float		        c0, ave;
        float                   pitch, tempVal;
        static int              flag = 0, numValues;
        int                     i;
        if (flag == 0) {
	  nfBy2 = nfft/2;
          ax = (float *) AllocFloatArray(ax, nfft+1);
          ay = (float *) AllocFloatArray(ay, nfft+1);
          amag = (float *) AllocFloatArray(amag, nfft+1);
          cepAmag = (float *) AllocFloatArray(cepAmag, nfft+1);
          phase = (float *) AllocFloatArray(phase, nfft+1);
          derv = (float *) AllocFloatArray(derv, nfft+1);
          cSig = (complex *) calloc (nfft+1, sizeof(complex));
          cFsig = (complex *) calloc (nfft+1, sizeof(complex));
          cfTemp1 = (complex *) calloc (nfBy2+1, sizeof(complex));
          cfTemp2 = (complex *) calloc (nfft+1, sizeof(complex));
          sigTemp = (float *) AllocFloatArray(sigTemp, nfft+1);
	  numValues = (int) (nfBy2/winScaleFactor);
	}
	for (i = 1; i <= npts; i++)
	  sigTemp[i] = signal[i]*HamDw(i, npts);
	for(i = npts+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
	Rfft(sigTemp,ax,ay,mfft,nfft,-1);
	SpectrumReal(nfft,ax,ay,amag,phase);
  	for( i = 1; i <= nfft; i++)
	  ax[i] = amag[i];
	CepSmooth(ax,cepAmag,mfft,nfft,winLen,&c0, 1.0);
        for (i = 1; i <= numValues; i++) {
          sigTemp[i] = amag[i]/cepAmag[i];
	}
	for(i = numValues+1; i<= nfft; i++)
	  sigTemp[i] = 0.0;
        derv = (float *) StandardModGd(sigTemp, numValues, nfft, mfft, gdSmthWinSize, gamma, 
				       gdPosScale, gdNegScale, medOrder, derv);
	Rfft(derv,ax,ay,mfft,nfft,1);
	if (winScaleFactor != 1) {
	  for (i = 2; i <= winLen; i++) {
	    ax[i] = ax[i]*HanW(i,winLen);
	    ax[nfft-i+2] = ax[i];
	  }
	  for (i = winLen+1; i <= nfft-winLen-1; i++)
	    ax[i] = 0.0;
	  Rfft(ax,derv,ay,mfft,nfft,-1); 
	  RemoveAverage(derv,nfBy2,&ave);
	}
	for (i = pthLow; i <= pthHgh; i++)
          pthCepstrum[i-pthLow] = derv[i];
        return(pthCepstrum);
	}

/****************************************************************************
*	The following subroutine extracts the pitch from the LP Residual
*
*	inputs : DaTa npts points long
*	mfft : FFT stages nfft = 2**mfft
*	pthLow, pthHgh : expected range of pitch values
*
*	Outputs :
*       pitchLP - extracted value of pitch
*****************************************************************************
*
*/
float 	*PitchAutoCorrLP(float *signal, int npts, int frameShift, 
			 int lpOrder, int pthLow, int pthHgh, float *pthAutoCorr) {
  static float    *residual, *coef, *resAutoCorr; 
  //  static int	  *peakArr;
  int		  i;
  float		  Gain;
  static int      flag = 0;

       
	if (flag == 0) {
	    residual = (float *) AllocFloatArray(residual, npts+1);
	    resAutoCorr = (float *) AllocFloatArray (resAutoCorr, npts+1);
	    coef = (float *) AllocFloatArray(coef,lpOrder+3);
	    flag = 1;
	}
       	LpAnal(signal, residual, npts, frameShift, coef, lpOrder, &Gain);
       	resAutoCorr  = (float *) AutoCorr(residual, npts);
      	for (i = pthLow; i <= pthHgh; i++)
          pthAutoCorr[i-pthLow] = resAutoCorr[i+1];
	return(pthAutoCorr);
      }


/**************************************************************************
 *                        End of DspLibrary.c
 **************************************************************************/







/*-------------------------------------------------------------------------
 * $Log: DspLibrary.c,v $
 * Revision 1.8  2011/06/08 15:42:26  hema
 * Fixed ModGDelaySmooth -- bug for denominator zero
 *
 * Revision 1.7  2011/06/08 02:26:21  hema
 * Fixed the cepsmooth bug in modgd computation
 * Smoothed the denominator spectrum -- should fix outliers.
 *
 * Revision 1.6  2011/03/25 12:14:46  hema
 * Implement Perceptual Filterbank based on ModGdSpectrum
 *
 * Revision 1.5  2011/03/25 06:19:16  hema
 * Removed some more memory leak bugs (as obtained) from valgrind
 *
 * Revision 1.4  2011/03/24 16:55:40  hema
 * fixed memory leaks
 *
 * Revision 1.3  2007/06/14 08:22:59  hema
 * modified LpAnal to another argument - frameShift
 * this is required for the computation of the residual on a
 * frame by frame basis
 * Added functions to bandpass filter signals
 * FIR works fine, IIR has some problem
 *
 * Revision 1.2  2004/12/23 08:28:52  hema
 * Fixed the bug SpectrumComplex
 *
 * Revision 1.1  2004/01/14 12:21:31  hema
 * Initial revision
 *
 * Revision 1.6  2002/10/16 08:53:24  hema
 * Added Modified Group Delay Cepstra
 * Min Group Delay Cepstra
 * Group Delay Cepstra
 * modified PseudoDct to include an Offset
 *
 * Revision 1.5  2002/04/23 07:26:05  hema
 * Renamed hyphenated files
 *
 * Revision 1.4  2001/10/25 12:26:32  hema
 * Modified indentation
 *
 * Revision 1.3  2000/04/28 01:57:54  hema
 * fixed group delay outlier problem
 *
 * Revision 1.2  2000/04/27 01:59:04  hema
 * modified group delay computation
 *
 * Revision 1.1  2000/04/23 02:36:13  hema
 * Initial revision
 * Local Variables:
 * time-stamp-active: t
 * time-stamp-line-limit: 20
 * time-stamp-start: "Last modified:[ 	]+"
 * time-stamp-format: "%3a %02d-%3b-%:y %02H:%02M:%02S by %u"
 * time-stamp-end: "$"
 * End:
 *                        End of DspLibrary.c
 -------------------------------------------------------------------------*/

