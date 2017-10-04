#ifndef HASHTABLE1_H
#define HASHTABLE1_H
#include<stdlib.h>

#define DEFAULT_HASH_TABLE_SIZE 200

typedef struct HashElement
{
	char* data;
	void* fnptr;
	int numFVectors;
	struct HashElement* next;
}HashElement;

typedef struct hashTable
{
        struct HashElement** element;
        int size;
}hashTable;

HashElement* GetHashElement(char* data,void* fnptr,int numFVectors);
void InsertFeaturesInfo(hashTable* ht,int numCepstrum,int numFilters,int fftSize,int windowSize,int resGdWindowSize, int numPthCoefficients);
hashTable* InitHashTable(int size);
void InsertHE(hashTable* ht,char* data,void* fnptr,int numFVectors);
int GetHashValue(int table_size,char* str);
HashElement* SearchHE(struct hashTable *ht,char* data);
void UnloadHashTable(struct hashTable* ht);
#endif
