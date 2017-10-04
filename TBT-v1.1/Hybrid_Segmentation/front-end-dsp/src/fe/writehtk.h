#ifndef WRITE_HTK
#define WRITE_HTK
typedef int int32;
typedef struct {              /* HTK File Header */
   int32 nSamples;
   int32 sampPeriod;
   short sampSize;
   short sampKind;
} HTKhdr;

void WriteHTKHeader(FILE *f, long nSamp, long sampP, short sampS, 
                    short kind);
void SwapShort(short *p);
void SwapInt32(int32 *p);
void WriteFloat (FILE *f, float *x, int n);
void WriteToFile (char *fileName, F_VECTOR **vfv, long numVectors,
		  int frameAdvanceSamples, int samplingRate);
#endif
