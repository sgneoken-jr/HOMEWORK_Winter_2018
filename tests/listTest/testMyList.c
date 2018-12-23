/* gcc -Wall -o test testMyList.c myList.c */
#include <stdlib.h>
#include <stdio.h>
#include "myList.h"

#define NUM		6
#define MUL		1.00
#define DEBUG

int main(int argc, char **argv) {
    
    Node *first = NULL;
    
	Coordinate coord[NUM];
	
    for (int i = 0; i < NUM; ++i){
    	coord[i].time = i;
    	coord[i].space = MUL * (double)i;
		#ifdef DEBUG
		printf("%d, %lf\n", coord[i].time, coord[i].space);
		#endif
    	first = addToList(first, &coord[i]);
    }
    
    printList(first); 
    
    struct searchThis findThis;
	findThis.type = timeType;
	findThis.val.i = 2;
    /* search an element in the middle of the list */
    if (searchList(first, &findThis) == NULL) 
      printf("Element time 2 not found\n"); 
    else
      printf("Element time 2 found\n");
    
    /* search an element at the beginning of the list */
    findThis.val.i = 0;
    if (searchList(first, &findThis) == NULL) 
      printf("Element time 0 not found\n"); 
    else
      printf("Element time 0 found\n");
    
    /* search an element at teh end of the list */
    findThis.val.i = NUM-1;
    if (searchList(first, &findThis) == NULL) 
      printf("Last element not found\n"); 
    else
      printf("Last element found\n");
   
    /* search an element not in the list` */
    findThis.val.i = NUM;
    if (searchList(first, &findThis) == NULL) 
      printf("Element not existent not found\n"); 
    else
      printf("Element not existent found\n");
      
    Node *nodeFound;
    
  	findThis.type = spaceType;
  	findThis.val.d = 10.00;
  	nodeFound = searchList(first, &findThis);
  	if (nodeFound == NULL) 
      printf("Space %lf not found\n", findThis.val.d); 
    else
      printf("Space %lf found at time: %d\n", findThis.val.d, nodeFound->value.time);
      
	findThis.type = timeType;
    
    
    first = deleteFromList(first, 5);
    printf("Deleted 5, List is:\n "); 
    printList(first); 
    
    first = deleteFromList(first, 1);
    printf("Deleted 1, List is:\n "); 
    printList(first); 
    
    first = deleteFromList(first, 2);
    printf("Deleted 2, List is:\n "); 
    printList(first); 
    
    first = deleteFromList(first, NUM+1);
    printf("Tried to delete not existent, List is still:\n "); 
    printList(first); 
    
    exit(EXIT_SUCCESS); 
}
