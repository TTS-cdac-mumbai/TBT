#ifndef SPHERE_INTERFACE_H
#define SPHERE_INTERFACE_H
#define SUCCESS 1
#define FAILURE 0

typedef unsigned char UCHAR;
typedef short int INT;
typedef int DWORD;

typedef struct RIFFHeader
{
	UCHAR riffChunkID[4];		//0x00	 	
	DWORD riffChunkDataSize;	//0x04
	UCHAR riffType[4];		//0x08
	
	UCHAR fmtChunkID[4];		//0x0C
	DWORD fmtChunkDataSize;		//0x10
	INT compressionCode;		//0x14
	INT numberOfChannel;		//0x16
	DWORD sampleRate;		//0x18
	DWORD averageBytesPerSecond;	//0x1C
	INT blockAlign;			//0x20
	INT sigBitsPerSample;		//0x22
	
	//@ This Element is not present for Uncompressed/PCM format
	//INT numberOfExtraBytes  __attribute__ ((packed)); 	//0x24
	
	UCHAR dataChunkID[4]; 		//0x24
	DWORD dataChunkDataSize;	//0x28
		
}RIFFHeader __attribute__((packed));	
	
typedef struct RIFFWaveFile
{
	RIFFHeader *header;				//0x00 to 0x2B
  short int *data __attribute__((packed));	//0x2C to dataSize
	
}RIFFWaveFile;

short *ReadSpherePcm(char *waveFileName, long *numSamples);
short *ReadSphereUlaw(char *waveFileName, long *numSamples);
short *ReadText(char *waveFileName, long *numSamples, float scaleFactor);
float *ReadFloatText (char *waveFileName, long *numSamples);
short *ReadRaw (char *waveFileName, long *numSamples);
short *ReadRaw16Big (char *waveFileName, long *numSamples);
short *ReadRaw16Little (char *waveFileName, long *numSamples);
short *ReadRIFF( char *waveFileName, long *numSamples);
short *ReadPitchValues( char *waveFileName, int timeOrFreq, long numFrames, int samplingRate);
void PrintRIFFWaveFileInfo( FILE *file, RIFFWaveFile *riff);
int WriteRIFFWaveFile( RIFFWaveFile*, int size, FILE* );
int WriteRIFF(char *waveFileName, short int *waveform, long numSamples, long samplingRate, int bytesPerSample); 
#endif
