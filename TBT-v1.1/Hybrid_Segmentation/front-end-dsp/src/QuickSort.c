/*-------------------------------------------------------------------------
 *  QuickSort.c - QuickSort an array of N floating point numbers
 *  Version:	$Name$
 *  Module:	
 *
 *  Purpose:	
 *  See:	
 *
 *  Author:	Hema A Murthy (hema@localhost.localdomain)
 *
 *  Created:        Sat 10-Mar-2001 19:36:25
 *  Last modified:  Wed 20-Jul-2011 14:39:23 by hema
 *  $Id$
 *
 *  Bugs:	
 *
 *  Change Log:	<Date> <Author>
 *  		<Changes>
 -------------------------------------------------------------------------*/

#include "stdio.h"
#include "string.h"

/*-------------------------------------------------------------------------
 *  Swap -- Swaps two float elements
 *    Args:	int, int
 *    Returns:	int
 *    Throws:	
 *    See:	
 *    Bugs:	
 -------------------------------------------------------------------------*/
void Swap (float *a, float *b) {
  float temp; 
  temp = *a;
  *a = *b;
  *b = temp;
  
}	/*  End of Swap		End of Swap   */

/*-------------------------------------------------------------------------
 *  QuickSort -- Recursive sort using CAR Hoare's Algorithm
 *    Args:	floa*, int, int
 *    Returns:	Nothing
 *    Throws:	
 *    See:	
 *    Bugs:	
 -------------------------------------------------------------------------*/

void QuickSort(float *v, int left, int right) {
  int   i,j,k;
  float x, w;
  i = left;
  j = right;
  //  printf("i = %d right = %d left = %d j = %d\n", i, right, left, j
  //);
  x = v[(left+right)/2];
  do {
    while (v[i] < x)
      i++;
    while (x < v[j]) 
      j--;
    //   printf("swap ? i = %d %f j = %d %f\n", i, v[i], j, v[j]); 
     if (i <= j) {
       // printf("before swapping i = %d %f j = %d %f\n", i, v[i], j, v[j]); 
       Swap(&v[i],&v[j]);
       // printf("after swapping i = %d %f j = %d %f\n", i, v[i], j, v[j]); 
       i++;
       j--;
       //printf("new vals of i and j i = %d j = %d\n", i, j);
       /*       for (k = 1; k <= 8; k++)
	 printf("%f ", v[k]);
	 printf("\n");*/
     }
 }   while (i < j);
  /* for(k = left; k<= right; k++)
     printf("%d ", v[k]); */
//printf("\n");

  if (left < j) 
    QuickSort(v,left,j);
  if (i < right)
    QuickSort(v,i,right);
  

}	/*  End of QuickSort		End of QuickSort   */


/*-------------------------------------------------------------------------
 * $Log$
 *
 * Local Variables:
 * time-stamp-active: t
 * time-stamp-line-limit: 20
 * time-stamp-start: "Last modified:[ 	]+"
 * time-stamp-format: "%3a %02d-%3b-%:y %02H:%02M:%02S by %u"
 * time-stamp-end: "$"
 * End:
 *                        End of QuickSort.c
 -------------------------------------------------------------------------*/







