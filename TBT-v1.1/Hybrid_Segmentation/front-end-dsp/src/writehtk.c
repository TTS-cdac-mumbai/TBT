#include "fe/FrontEndDefs.h"
#include "fe/FrontEndTypes.h"
#include "fe/writehtk.h"

void WriteFloat (FILE *f, float *x, int n)
{
   int j;
   float *p;
   for(p = x,j = 0; j < n; p++, j++) {
      SwapInt32((int32*)p);  
      //   fwrite(x, sizeof(float), n, f);
      fwrite(p, 4, 1, f);
   }
}

void SwapInt32(int32 *p)
{
   char temp,*q;
   
   q = (char*) p;
   temp = *q; *q = *(q+3); *(q+3) = temp;
   temp = *(q+1); *(q+1) = *(q+2); *(q+2) = temp;
}

/* SwapShort: swap byte order of short data value *p */
void SwapShort(short *p)
{
   char temp,*q;
   
   q = (char*) p;
   temp = *q; *q = *(q+1); *(q+1) = temp;
}

void WriteHTKHeader(FILE *f, long nSamp, long sampP, short sampS, 
                    short kind)
{
   HTKhdr hdr;
   int n = sizeof hdr;
   
   hdr.nSamples   = nSamp;
   hdr.sampSize   = sampS;
   hdr.sampKind   = kind;
   hdr.sampPeriod = sampP;
   SwapInt32(&hdr.nSamples); 
   SwapInt32(&hdr.sampPeriod);
   SwapShort(&hdr.sampSize);
   SwapShort(&hdr.sampKind);
   if (fwrite(&hdr, 1, n, f) != n)
      fprintf(stderr,"WriteHTKHeader: Cannot write HTK format header"); 
   else 
      fprintf(stderr,"Wrote header\n");
}

void WriteToFile (char *fileName, F_VECTOR **vfv, long numVectors,
                 int frameAdvanceSamples, int samplingRate) {
    int i;
    FILE *outputFile=NULL;
    outputFile = fopen (fileName, "wb");
    WriteHTKHeader(outputFile, numVectors,
                   (int) (frameAdvanceSamples * 10E6 / samplingRate),
                   4*vfv[0]->numElements,
                   9);
   for(i = 0; i < numVectors; i++) {
         WriteFloat(outputFile, vfv[i]->array, vfv[i]->numElements);
   }  
   fclose(outputFile);
}
