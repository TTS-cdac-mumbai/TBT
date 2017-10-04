/*-------------------------------------------------------------------------
 *  HashTable.c - HashTable Operations
 *  Version:    $Name:  $
 *  Module:
 *
 *  Purpose:
 *  See:
 *
 *  Author:     chaitanya (chaitanya@lantana.tenet.res.in)
 *
 *  Created:        Wed  09:12:2007
 *  
 *  $Id: chaitanya.c,v 1.8 2007/06/14 08:23:56 chaitanya Exp chaitanya $
 *
 *  Bugs:
 *
 *  Change Log: <Date> <Author>
 *              <Changes>
 -------------------------------------------------------------------------*/

#include<strings.h>
#include<string.h>
#include "fe/HashTable.h"
#include "malloc.h"

/*****************************************************************************
  Function : Allocate Memory for HashElement and initialize the 
	     values of the attributes of hash elements

  Inputs : data contains name of function, pointer to the function and
           the number of feature vectors

  ****************************************************************************/


 HashElement* GetHashElement(char* data,void* fnptr,int numFVectors)
{
   struct HashElement* temp;
   temp = (HashElement*)malloc(1*sizeof(HashElement));
   temp->data = (char*)malloc(sizeof(char)*(strlen(data)+1));
   strcpy(temp->data,data);
   temp->fnptr = fnptr;
   temp->numFVectors = numFVectors;
   temp->next=NULL;
   return(temp);
}


/*****************************************************************************
  Function : Initializes the hash table with the specified size of hashelements

  Inputs : size(Number of hash elements)

  ****************************************************************************/



hashTable* InitHashTable(int size)
{
   struct hashTable* ht;
   int i;
   
   ht = (hashTable*)malloc(1*sizeof(hashTable));
   bzero(ht,sizeof(hashTable));
   ht->size=size;
   ht->element = (HashElement**)malloc(sizeof(HashElement*)*size);
   for(i=0;i<size;i++)
   {
	ht->element[i]=NULL;
   }

return ht;

}



/*****************************************************************************
  Function : Calculates the hashvalue of given string and returns

  Inputs : size of hash table,string

  ****************************************************************************/

int GetHashValue(int table_size,char* str)
{
        int hash = 0 ;
        int c=0 ;
	c=*str;
        while (c !='\0' && c!='\n' )
	{
        hash = hash + c ;
	c= *(str++);
	}
        return((hash%(table_size)));       
       
}

/*****************************************************************************
  Function : Inserts hash element into the hash table

  Inputs : hashtable,data,function pointer, number of feature vectors

  ****************************************************************************/


void InsertHE(hashTable* ht, char* data, void* fnptr,int numFVectors)
{
  int hashvalue=0;
  struct HashElement* temp; 
  hashvalue=GetHashValue(ht->size,data);
  temp=ht->element[hashvalue];
  if(temp == NULL)
  {
	ht->element[hashvalue]=GetHashElement(data,fnptr,numFVectors);
        return;
  }
  while(temp->next != NULL)
  {
	temp=temp->next;

  }
  
  temp->next=(HashElement *) GetHashElement(data,fnptr,numFVectors);
  temp->next->next = NULL;
  return;
	
}

/*****************************************************************************
  Function : searches the hash element in a hash table

  Inputs : hashtable,data 

  ****************************************************************************/

HashElement* SearchHE(hashTable *ht,char* data)
{
  int hashvalue;
  HashElement* temp;
  int strlength;
 hashvalue=ht->size; 
 
  hashvalue=GetHashValue(ht->size,data);
  temp=ht->element[hashvalue];
  strlength = (int) strlen(data);
   while(temp!=NULL)
  {    
	
	if(!strncmp(temp->data,data,strlength))
	{
		return(temp);
	}
	else
	{
        temp=temp->next;
	}
  }


  return NULL;

}


/*****************************************************************************
  Function : Unloads the hashtable

  Inputs : hashtable that has to be unloaded

  ****************************************************************************/

void UnloadHashTable(hashTable* ht)
{
  
 HashElement* trav1;
 HashElement* trav2;
 int i;
 for(i=0;i<ht->size;i++)
 {
   trav1=ht->element[i];
   trav2=ht->element[i]->next;	
  	while(trav1!=NULL && trav2!=NULL)
  	{
	 	free(trav1->data);	
	 	free(trav1);
		trav1=trav2;
		trav1=trav1->next;
	
	}
 }

free(ht);
}



/*-------------------------------------------------------------------------
 * $Log: HashTable.c,v $

*
 * Local Variables:
 * time-stamp-active: t
 * time-stamp-line-limit: 20
 * time-stamp-start: "Last modified:[   ]+"
 * time-stamp-format: "%3a %02d-%3b-%:y %02H:%02M:%02S by %u"
 * time-stamp-end: "$"
 * End:
 *                        End of HashTable.c
 -------------------------------------------------------------------------*/



