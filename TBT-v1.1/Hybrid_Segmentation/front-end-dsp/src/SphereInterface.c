/*-------------------------------------------------------------------------
 *  SphereInterface.c - Library of functions to read different wavefile formats
 *  Version:	$Name:  $
 *  Module:	
 *
 *  Purpose:	
 *  See:	
 *
 *  Author:	Hema Murthy (hema@bhairavi.iitm.ernet.in)
 *
 *  Created:        Thu 01-Mar-2007 12:19:44
 *  Last modified:  Fri 26-Apr-2013 12:49:44 by hema
 *  $Id: SphereInterface.c,v 1.4 2007/06/14 08:25:49 hema Exp hema $
 *
 *  Bugs:	
 *
 *  Change Log:	<Date> <Author>
 *  		<Changes>
 -------------------------------------------------------------------------*/

#include "sp/sphere.h"
#include "fe/DspLibrary.h"
#include "stdlib.h"
#include "fe/SphereInterface.h"
#include <stdio.h>
#include <string.h>


/*****************************************************************************
  function : ReadSpherePcm - reads sphere headered pcm files
  Inputs   : wavefilename
  Outputs  : short waveform, numSamples
  **************************************************************************/
  


short *ReadSpherePcm(char *wavname, long *numSamples) {
  SP_FILE             *waveFile;
  unsigned long               samples = 0;  
//SP_INTEGER          samples = 0;
  long                numSamp = 0;
  short               *waveform;
  //  SP_INTEGER          success;
  unsigned short            success;
  //int                 i;

  waveFile = sp_open(wavname,"r");
  success = sp_h_get_field(waveFile,"sample_count",T_INTEGER, &samples);
  printf("samples = %ld\n",samples);
  if ((waveform = (short *) sp_data_alloc(waveFile,-1)) == NULL) {
    printf("unable to allocate space for waveform\n");
    exit(-1);
  }
  sp_rewind(waveFile);
  numSamp = sp_read_data(waveform, samples, waveFile);
  printf("numSamp = %ld\n",numSamp);
  if (numSamp != samples) {  
    printf("numSamp = %ld error reading waveform \n", numSamp);
    exit(-1);
  }
  *numSamples = numSamp;
/*for (i = 0; i < numSamp; i++)
  printf("waveform %d = %d \n", i, waveform[i]);*/
  sp_close(waveFile);
  return(waveform);
}
/*****************************************************************************
  function : ReadRaw16Little - Read Raw binary files 
             (2 bytes/sample, Little endian format)
  Inputs   : wavefilename
  Outputs  : short waveform, numSamples
  **************************************************************************/
  


short *ReadRaw16Little(char *wavname, long *numSamples) {
  FILE                *waveFile;
  short               *waveform;
  int                 i;
  unsigned            value;
  int                 c1, c2;

  waveFile = fopen(wavname,"r");
  i = 0;
  while ((c1 = fgetc(waveFile)) != EOF) {
    i++;
  }
  *numSamples = i/2;
  printf ("numSamples = %ld \n", *numSamples);
  waveform = (short *) malloc (*numSamples*sizeof(short));
  i = 0;
  rewind (waveFile);
  while ((c1 = fgetc(waveFile)) != EOF) {
    c2 = fgetc(waveFile);
    value = (unsigned)(c2*256+c1);
    if (value < 32768) 
      waveform[i] = value;
    else
      waveform[i] = -65536+value;
    i++;
  }
  fclose(waveFile);
  return(waveform);
}
/*****************************************************************************
  function : ReadRaw16Big - Read Raw binary files
             (2 bytes/sample, Big endian format)
  Inputs   : wavefilename
  Outputs  : short waveform, numSamples
  **************************************************************************/
  


short *ReadRaw16Big(char *wavname, long *numSamples) {
  FILE             *waveFile;
  short               *waveform;
  int                 i;
  int                 c1, c2;
  unsigned            value;
  waveFile = fopen(wavname,"r");
  i = 0;
  while ((c1 = fgetc(waveFile)) != EOF) {
    i++;
  }
  *numSamples = i/2;
  printf ("numSamples = %ld \n", *numSamples);
  waveform = (short *) malloc (*numSamples*sizeof(short));
  i = 0;
  rewind (waveFile);
  while ((c1 = fgetc(waveFile)) != EOF) {
    c2 = fgetc(waveFile);
    value = (unsigned)(c1*256+c2);
    if (value < 32768)
      waveform[i] = value;
    else
      waveform[i] = -65536+value;
    i++;
  }
  fclose(waveFile);
  return(waveform);
}
/*****************************************************************************
  function : ReadRaw - Read Raw binary files
  Inputs   : wavefilename
  Outputs  : short waveform, numSamples
  **************************************************************************/
  


short *ReadRaw(char *wavname, long *numSamples) {
  FILE             *waveFile;
  short               *waveform;
  int                 i;
  int                 c;

  waveFile = fopen(wavname,"r");
  i = 0;
  while ((c = fgetc(waveFile)) != EOF) {
    i++;
  }
  *numSamples = i;
  printf ("numSamples = %ld \n", *numSamples);
  waveform = (short *) malloc (*numSamples*sizeof(short));
  i = 0;
  rewind (waveFile);
  while ((c = fgetc(waveFile)) != EOF) {
    waveform[i] = c;
    i++;
  }
  fclose(waveFile);
  return(waveform);
}
/*****************************************************************************
  function : ReadSphereUlaw - reads sphere headered ulaw files
  Inputs   : wavefilename
  Outputs  : short waveform, numSamples
  **************************************************************************/

short *ReadSphereUlaw(char *wavename,long *numSamples) {
  SP_FILE              *sp;
  SP_INTEGER            channelCount, sampleNBytes, sampleCount;
  //int                 channelCount, sampleNBytes, sampleCount;
  SP_INTEGER            mode;
  short                 *waveform;
  SP_INTEGER            waveByteSize,
                        totalSamples,
                        samplesRead;

    sp = sp_open(wavename,"r");

    mode = (SP_INTEGER) sp_set_data_mode(sp, "SE-PCM-2");
    if (mode  > 0){
      sp_print_return_status(stdout);
      sp_close(sp);
      exit(-1);
    }
    mode = (SP_INTEGER) sp_h_get_field(sp,"channel_count",T_INTEGER,&channelCount);
    if (mode > 0 ){
      fprintf(stderr,"Error: Unable to get the '%s' field\n",
	      "channelCount");
      sp_close(sp);
      exit(-1);
    }
    /*  When the sd_set_data_mode() function is called to convert the file */
    /*  from ulaw to pcm, the sampleNBytes in the header is automatically*/
    /*  changed to 2                                                       */

    mode = (SP_INTEGER) sp_h_get_field(sp,"sample_n_bytes",T_INTEGER,&sampleNBytes);
    if (mode > 0){
      fprintf(stderr,"Error: Unable to get the '%s' field\n",
	      "sampleNBytes");
      sp_close(sp);
      exit(-1);
    }
    mode = (SP_INTEGER) sp_h_get_field(sp,"sample_count",T_INTEGER,&sampleCount);
    if (mode > 0){
      fprintf(stderr,"Error: Unable to get the '%s' field\n","sampleCount");
      sp_close(sp);
      exit(-1);
    }

    totalSamples=sampleCount * channelCount;
    waveByteSize=sampleNBytes * totalSamples;
    
    printf("---- Example 2: Expected channelCount=1,      Actually=%ld\n",
		channelCount);
    printf("---- Example 2: Expected sampleNBytes=2,     Actually=%ld\n",
		sampleNBytes);
    printf("---- Example 2: Expected sampleCount=16000,   Actually=%ld\n",
		sampleCount);
    printf("---- Example 2: Expected totalSamples=16000,  Actually=%ld\n",
		totalSamples);
    printf("---- Example 2: Expected waveByteSize=32000, Actually=%ld\n",
	waveByteSize);

    waveform=(short *)malloc(waveByteSize); 
    if (waveform == (short *)0){
      fprintf(stderr,"Error: Unable to allocate %d bytes for the waveform\n",
	      waveByteSize);
      sp_close(sp);
      exit(-1);
    }
    
    samplesRead= (SP_INTEGER) sp_read_data(waveform, totalSamples, sp); 
    if (samplesRead != totalSamples){
      fprintf(stderr,"Error: Unable to read speech waveform, ");
      fprintf(stderr,"%d samples read\n",samplesRead);
      sp_print_return_status(stderr);
      sp_close(sp);
      exit(-1);
    }
    printf("numSamp = %d\n",samplesRead);
    fflush(stdout);
    *numSamples=samplesRead;
    sp_close(sp);
    return(waveform);
    
}


/**********************************************************************

 ReadText reads speech data from a text_file

 Inputs :
        wavename : name of the text file
 Outputs : 
        waveform : array of short integers
	num_samples : number of samples read

***********************************************************************/
float *ReadFloatText(char *wavename, long *numSamples)
{
  FILE *waveFile;
  long numSamp = 0;
  float *waveFloat;
  float rmax;
  char line[500];
  int i;


  waveFile = fopen(wavename,"r");
  while (fgets(line,254,waveFile) != NULL) {
     numSamp++;
  }
  waveFloat =(float *) malloc(numSamp*sizeof(float));
  rewind(waveFile);
  if (waveFloat == NULL) {
    printf("unable to allocate space for waveform \n");
    exit(-1);
  }
  numSamp=0;
  while (fgets(line,254,waveFile)!= NULL) {
    sscanf(line,"%f",&waveFloat[numSamp]);
    numSamp++;
  }
  *numSamples = numSamp;
  /*  rmax = fabs((float) waveFloat[Imax0(waveFloat, numSamp)]);
  for (i = 0; i < numSamp; i++)
    if (scaleFactor != 0)
      waveform[i] = (short) (waveFloat[i]/rmax*scaleFactor);
    else 
      waveform[i] = (short) (waveFloat[i]);
  */
  printf("number of samples read = %ld\n",numSamp);
  fflush(stdout);
  fclose(waveFile);  
  return(waveFloat);
}

/**********************************************************************

 ReadPitchValues reads pitch values in samples from a pitch File

 Inputs :
        pitchFile : name of the text file
	numFrames : number of frames to be read
 Outputs : 
        waveform : array of short integers

***********************************************************************/
short *ReadPitchValues(char *pthFileName, int timeOrFreq, long numFrames, int samplingRate)
{
  FILE *pthFile;
  short *wavePthValues;
  long numSamp;
  float pthVal;
  short *pthValues=NULL;
  char line[254];


  pthFile = fopen(pthFileName,"r");
  numSamp = 0;
  while (fgets(line,254,pthFile) != NULL) {
     numSamp++;
  }
  if (numSamp != numFrames) {
    printf("Wave file size and pitch file sizes do not match\n");
    exit(-1);
  }
  pthValues =(short *) malloc(numSamp*sizeof(float));
  rewind(pthFile);
  if (pthValues == NULL) {
    printf("unable to allocate space for waveform \n");
    exit(-1);
  }
  numSamp=0;
  while (fgets(line,254,pthFile)!= NULL) {
    sscanf(line,"%f",&pthVal);
    if (pthVal != 0) {
      if (timeOrFreq == 1)
        pthVal = 1.0/pthVal;
      pthValues[numSamp] = (int) (ceilf(pthVal*samplingRate));
    }
    else
      pthValues[numSamp] = 0;
    numSamp++;
  }
  /*  rmax = fabs((float) pthValues[Imax0(pthValues, numSamp)]);
  for (i = 0; i < numSamp; i++)
    if (scaleFactor != 0)
      waveform[i] = (short) (pthValues[i]/rmax*scaleFactor);
    else 
      waveform[i] = (short) (pthValues[i]);
  */
  printf("number of frames read = %ld\n",numSamp);
  fflush(stdout);
  fclose(pthFile);  
  return(pthValues);
}



/**********************************************************************

 ReadText reads speech data from a text_file

 Inputs :
        wavename : name of the text file
 Outputs : 
        waveform : array of short integers
	num_samples : number of samples read

***********************************************************************/
short *ReadText(char *wavename, long *numSamples, float scaleFactor)
{
  FILE *waveFile;
  long numSamp = 0;
  short *waveform;
  float *waveFloat;
  float rmax;
  char line[500];
  int i;


  waveFile = fopen(wavename,"r");
  while (fgets(line,200,waveFile) != NULL) {
     numSamp++;
  }
  waveform =(short *) malloc(numSamp*sizeof(short));
  waveFloat =(float *) malloc(numSamp*sizeof(float));
  rewind(waveFile);
  if (waveform == NULL) {
    printf("unable to allocate space for waveform \n");
    exit(-1);
  }
  numSamp=0;
  while (fgets(line,200,waveFile)!= NULL) {
    sscanf(line,"%f",&waveFloat[numSamp]);
    numSamp++;
  }
  *numSamples = numSamp;
  rmax = fabs((float) waveFloat[Imax0(waveFloat, numSamp)]);
  for (i = 0; i < numSamp; i++)
    if (scaleFactor != 0)
      waveform[i] = (short) (waveFloat[i]/rmax*scaleFactor);
    else 
      waveform[i] = (short) (waveFloat[i]);
  printf("number of samples read = %ld\n",numSamp);
  fflush(stdout);
  fclose(waveFile);  
  free(waveFloat);
  return(waveform);
}

/*****************************************************************************
  function : WriteRIFF - write to a file in RIFF format assumes
             data is sampled at 16KHz and quantised to 16bits/sample
  Inputs   : wavefilename
  Outputs  : short waveform, numSamples
  **************************************************************************/

//=================== WriteWaveFile ===================//
int WriteRIFF( char *waveFileName, short *waveform, long  numsamples, long samplingRate, int bytesPerSample)
{
        FILE *file;
	RIFFWaveFile *myRIFF = NULL;
	int i = 0;
	int size = 0;
	int result = SUCCESS;
	
        file = fopen(waveFileName, "w");
       	myRIFF = (RIFFWaveFile *)malloc(sizeof(RIFFWaveFile));
	myRIFF->header = (RIFFHeader *)malloc(sizeof(RIFFHeader));

	strcpy(myRIFF->header->riffChunkID, "RIFF");
	myRIFF->header->riffChunkDataSize = numsamples*2 + 36;
	strcpy(myRIFF->header->riffType, "WAVE");
	

	strcpy(myRIFF->header->fmtChunkID,"fmt ");
	myRIFF->header->fmtChunkDataSize = 16;
	myRIFF->header->compressionCode = 1;
	myRIFF->header->numberOfChannel = 1;
	myRIFF->header->sampleRate = samplingRate;
	myRIFF->header->averageBytesPerSecond = bytesPerSample*samplingRate;
	myRIFF->header->blockAlign = 2;
	myRIFF->header->sigBitsPerSample = bytesPerSample*8;

	strcpy(myRIFF->header->dataChunkID,"data");
	myRIFF->header->dataChunkDataSize = numsamples*2;
	
	size = myRIFF->header->dataChunkDataSize/2;
	myRIFF->data = (short *)malloc(sizeof(short) * (size+100));
	for(i=0; i <size; i++)
	{
		myRIFF->data[i] = waveform[i];
		if ((i % 10000) == 0) printf("i=%d\n", i);
	}

	result = WriteRIFFWaveFile(myRIFF, size*2, file);

        fclose(file);
	        free(myRIFF->data);
		free(myRIFF->header);
		free(myRIFF);
	return result;
}

//=================== WriteRIFFWaveFile ===================//
int WriteRIFFWaveFile( RIFFWaveFile *riff, int size, FILE *file)
	{
	int result = SUCCESS;
	
		if( fwrite( riff->header, 44, 1, file) != 1) 
	{
		result = FAILURE;
	 }
        
	 if( fwrite( riff->data, size, 1, file ) != 1)
	    {
	      result = FAILURE;
	    }

	return result;
	}

//=================== ReadRIFFWaveFile ===================//
/*****************************************************************************
  function : ReadRIFF - read a RIFF Wavefile and store in a short array
             assumes that the data is utmost coded using 16bit/sample
  Inputs   : wavefilename
  Outputs  : short waveform, numSamples
  **************************************************************************/
short  *ReadRIFF( char *waveFileName, long *numSamples) 
	{
	  RIFFWaveFile *riff=0;
	  int result = FAILURE;
	  int size = 0;
	  FILE *file=NULL;
          short *waveform=0;
	  file = fopen (waveFileName, "r");
	  fseek( file, 0L, SEEK_SET );

	 //-------- Added on 8/2/2008 -----------------//
	riff = (RIFFWaveFile *)malloc(sizeof(RIFFWaveFile));
        riff->header = (RIFFHeader *)malloc(sizeof(RIFFHeader));	
 
	//-------- Read RIFF Header Info ---------------//
	if( fread( riff->header, 44, 1, file ) != 1 )
		{
  		fprintf( stderr,"Error Reading the File\n" );	
		exit(1);
		}
	
	if ( memcmp( riff->header->riffChunkID, "RIFF", 4 ) != 0 ) 
		{
		  fprintf(stderr, "Error Reading the File \n");
                  exit(1);
		}

	if ( memcmp( riff->header->dataChunkID, "data", 4 ) != 0 ) 
		{
                  fprintf(stderr, "Error Reading the File \n");
                  exit(1);
		}
		
	//-------- Read Data --------------------------//
	size = riff->header->dataChunkDataSize;
	waveform = (short int*)malloc( size );
	
	if( fread( waveform, size, 1, file ) != 1 ) 
		{
 		fprintf( stderr,"Error Reading the File\n" );
		exit(1);
		}
        *numSamples = size/2;
	result = SUCCESS;
        fclose(file);
	return waveform;
	}

//=================== PrintRIFFWaveFile ==================//
void PrintRIFFWaveFileInfo( FILE *oFile, RIFFWaveFile *riff )
	{
	int i = 0;
	
	printf("----------------------------------------\n");
	printf("RIFF Chunk\n");
	printf("----------------------------------------\n");
	printf("ID\t\t\t\t");
	for( i = 0; i < 4; i++ )
		printf("%c",riff->header->riffChunkID[i]);
	printf("\n");	
	printf("Data Size\t\t\t");
	printf("%ld\n",riff->header->riffChunkDataSize);
	printf("Type\t\t\t\t");
	for( i = 0; i < 4; i++ )
		printf("%c",riff->header->riffType[i]);
	printf("\n");
	
	printf("----------------------------------------\n");
	printf("Format Chunk\n");
	printf("----------------------------------------\n");
	printf("ID\t\t\t\t");
	for( i = 0; i < 4; i++ )
		printf("%c",riff->header->fmtChunkID[i]);
	printf("\n");
	printf("Data Size\t\t\t");
	printf("%ld\n",riff->header->fmtChunkDataSize);
	printf("Compression Code\t\t");
	printf("%d\n",riff->header->compressionCode);
	printf("Number Of Channels\t\t");
	printf("%d\n",riff->header->numberOfChannel);
	printf("Sample Rate\t\t\t");
	printf("%ld\n",riff->header->sampleRate);
	printf("Average Bytes Per Second\t");
	printf("%ld\n",riff->header->averageBytesPerSecond);
	printf("Block Align\t\t\t");	
	printf("%d\n",riff->header->blockAlign);
	printf("Significant Bytes Per Sample\t");
	printf("%d\n",riff->header->sigBitsPerSample);
	
	printf("----------------------------------------\n");
	printf("Data Chunk\n");
	printf("----------------------------------------\n");
	printf("ID\t\t\t\t");
	for( i = 0; i < 4; i++ )
		printf("%c",riff->header->dataChunkID[i]);
	printf("\n");
	printf("Data Size\t\t\t");
	printf("%ld\n",riff->header->dataChunkDataSize);

	for( i = 0; i < riff->header->dataChunkDataSize/2;)
		{
		fprintf( oFile,"%d\n",riff->data[i] );
		i++;
		}
	return;
	}

//xxxxxxxxxxxxxxxxxxxxxxxx CUT HERE xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

//Appendix A

//****************** RIFF FORMAT CHUNK *********************

//Chunk Id
//Always end with the space character (0x20).

//Compression Code Description
//Code 		Description
//0 (0x0000)	UnKnown
//1 (0x0001)	PCM/Uncompressed
//2 (0x0002)	Microsoft ADPCM
//6 (0x0006)	ITU G.711 a-law
//7 (0x0007)	ITU G.711 mu-law
//17 (0x0011)	IMA ADPCM
//20 (0x0016)	ITU G.723 ADPCM (Yamaha)
//49 (0x0031)	GSM 6.10
//64 (0x0040)	ITU G.721 ADPCM
//80 (0x0050)	MPEG
//   (0xFFFF)	Experimental 

//Number of Channels
//1 - mono signal, 2 - stereo signal.

//Average Bytes Per Second
//AverageBytesPerSecond = SampleRate * BlockAlign.

//BlockAlign
//BlockAlign = SignificantBitsPerSample / 8 * NumChannels.

//Significant Bits Per Sample
//Its usually 8,16,24, or 32. Must be aligned to nearest byte (multiples of 8).

//ExtraFormatBytes
//Its not there for Uncompressed PCM file. If present, must be word aligned (multiples of 2).


//Appendix B

//****************** RIFF DATA CHUNK ************************

//If the compression code is 1, then the wave data contains raw sample values.

//xxxxxxxxxxxxxxxxxxxxxxxx CUT TILL HERE xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


/*-------------------------------------------------------------------------
 * $Log: SphereInterface.c,v $
 * Revision 1.4  2007/06/14 08:25:49  hema
 * Added functions to read Raw, Raw16-BigEndian, Raw16-LittleEndian
 * RIFF format wavefiles
 *
 * Revision 1.3  2007/05/24 12:12:14  hema
 * Supports Raw, LittleEndian and BigEndian data formats
 *
 *
 * Local Variables:
 * time-stamp-active: t
 * time-stamp-line-limit: 20
 * time-stamp-start: "Last modified:[ 	]+"
 * time-stamp-format: "%3a %02d-%3b-%:y %02H:%02M:%02S by %u"
 * time-stamp-end: "$"
 * End:
 *                        End of SphereInterface.c
 -------------------------------------------------------------------------*/
