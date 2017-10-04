/**************************************************************************
 *  $Id: ComputeFeature.c,v 1.1 2000/05/26 10:00:58 hema Exp hema $
 *  Release $Name:  $
 *
 *  File:	ComputeFeature.c - Computes features from a given
 *              utterance and writes it to a file.
 *
 *  Purpose:	Feature Extraction
 *
 *  Author:	Hema A Murthy,BSB 307,8342,9342
 *
 *  Created:    Wed 08-Mar-2000 21:51:19
 *
 *  Last modified:  Mon 22-Apr-2013 19:13:36 by hema
 *
 *  Bugs:	
 *
 *  Change Log:	<Date> <Author>
 *  		<Changes>
 *
 **************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "fe/constants.h"
#include "fe/FrontEndDefs.h"
#include "fe/FrontEndTypes.h"
#include "fe/DspLibrary.h"
#include "fe/InitAsdf.h"
//#include "malloc.h"
void Usage () {
  printf ("ComputeFeature controlFile waveFile featureFile featureName startFrame endFrame maxValues(0/1)\n");
  fflush(stdout);
}

/*-------------------------------------------------------------------------
 *  FindOnes -- Determines if a given index is within deltaDifference of a 1
 *    Args: randPhs -- array with zeroes and 1, numPts -- size of array	
 *    Returns:	int
 *    Throws:	
 *    See:	
 *    Bugs:	
 -------------------------------------------------------------------------*/
int FindOnes(int *randPhs, int deltaDifference, int index, int numPts) {
  int flag=0;
  int i = index - deltaDifference;
  while  ((i <= index+deltaDifference) && !(flag)) {
    if ((i > 0) && (i <= numPts)) {
      if (randPhs[i] == 1) 
        flag = 1;
      else
        i++;
    } else i++;
  }
  return (flag);
}	/*  End of FindOnes		End of FindOnes   */

  int main(int argc, char *argv[])
  {
    FILE                  *c_file = NULL, *out_file = NULL;
    char                  *wavname =NULL, 
                          *cname = NULL, *cepname = NULL, *feature_name = NULL;
    ASDF                  *asdf;
    int                   i, j, k;
    int                   startFrame, endFrame, numVoicedFrames =0, 
                          samplingRate, frameShift, 
                          maxValues, numPeaks, numFormants, deltaDifference;
    F_VECTOR              *fvect;
    int                   *randPhs;
    if(argc != 8) {
      Usage();
      exit(-1);
    }
    cname = argv[1];
    wavname = argv[2];
    cepname = argv[3];
    out_file = fopen(cepname ,"w");
    feature_name = argv[4];
    sscanf(argv[5], "%d", &startFrame);
    sscanf(argv[6], "%d", &endFrame);
    sscanf(argv[7], "%d", &maxValues);
    c_file = fopen(cname,"r");
    asdf = (ASDF *) malloc(1*sizeof(ASDF));
    InitializeASDF(asdf);
    InitializeStandardFrontEnd(asdf,c_file);
    //    Cstore(asdf->fftSize);
    printf("testfile name = %s\n",wavname);
    fflush(stdout);
    GsfOpen(asdf,wavname);
    printf("Total Number of frames = %d\n", asdf->numFrames);
    samplingRate = (int) GetIAttribute(asdf, "samplingRate");
    frameShift = (int) GetIAttribute(asdf, "frameAdvanceSamples");
    if (maxValues == 1) {
      numPeaks = (int) GetIAttribute (asdf, "numFormants");
      deltaDifference = (int) GetIAttribute(asdf, "deltaDifference");
    }
    if (startFrame < 0) { 
      startFrame = 0;
    }
    if (endFrame == -1) { 
      endFrame = asdf->numFrames;
    }
    if (endFrame > asdf->numFrames) 
      endFrame = asdf->numFrames;
    if (maxValues == 1) {
      fvect = (F_VECTOR*) GsfRead(asdf, 0, feature_name);
       randPhs = (int *) malloc ((fvect->numElements+1)*sizeof(int));
       free(fvect->array);
       free(fvect);
    }
    for (i = startFrame; i < endFrame; i++) {
      //      if (asdf->vU[i])
	fvect = (F_VECTOR *) GsfRead(asdf, i, feature_name);
      /*      if(fvect == NULL) {
	      printf("problems fvect\n");
	      fflush(stdout);
	      exit(-1);
	}*/
      if ((fvect != NULL) && (fvect->numElements != 0)){
	numVoicedFrames++;
	if (maxValues == 1)
          FindPeaks(&(fvect->array[-1]), randPhs, fvect->numElements); 
	for (k = 0; k < fvect->numElements; k++)
	  if (maxValues == 1) {
	    if (FindOnes (randPhs, deltaDifference, k+1, fvect->numElements))
	      fprintf(out_file,"%e ", /*(float)(i*(frameShift))/samplingRate, k,*/ fvect->array[k]);
	    else   
	      fprintf(out_file,"%e ", /*(float)(i*(frameShift))/samplingRate, k,*/ fvect->array[Imin0(fvect->array, fvect->numElements)]);
	  } else
	    fprintf(out_file,"%e ", /*(float)(i*(frameShift))/samplingRate, k,*/ fvect->array[k]);
	fprintf(out_file,"\n");
	free(fvect->array);
	free(fvect);
      }
    }
    GsfClose(asdf);
    printf(" %d vectors processed \n", numVoicedFrames);
    fclose(out_file);
    return(0);
  }














/**************************************************************************
 * $Log: ComputeFeature.c,v $
 * Revision 1.1  2000/05/26 10:00:58  hema
 * Initial revision
 *
 * Revision 1.1  2000/04/25 11:43:29  hema
 * Initial revision
 *
 *
 * Local Variables:
 * time-stamp-active: t
 * time-stamp-line-limit: 20
 * time-stamp-start: "Last modified:[ 	]+"
 * time-stamp-format: "%3a %:d-%3b-%:y %02H:%02M:%02S by %u"
 * time-stamp-end: "$"
 * End:
 *                        End of ComputeFeature.c
 **************************************************************************/
