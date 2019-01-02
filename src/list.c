#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myTypes.h"
#include "config.h"
#include "list.h"

#define EMPTY_LIST		1

Node *addToList(Node *list, Coordinate *coord) {
  Node *newNode; 
  
  // dynamic allocation of the memory
  newNode = malloc(sizeof(Node));
  if (newNode == NULL) {
	printf("Error: malloc failed in addToList\n");
	exit(EXIT_FAILURE);
  }
  
  // assignment of the values
  memcpy( &(newNode->value), coord, sizeof(Coordinate));
/*  newNode->value = *coord; // struct copying, hopefully it works*/
  newNode->next = list; 
  return newNode; 
  
}


Node *searchList(Node *list, struct searchThis *v){

  Node *p;
  
  switch ((int)v->type){
  	case timeType:
  		for (p = list; p != NULL; p = p->next){
			if (p->value.time == v->val.i)
				return p;
		}
		break;
  	case spaceType:
  		for (p = list; p != NULL; p = p->next){
			if (p->value.space == v->val.d)
				return p;
		}
		break;	
	default:
		printf("Invalid search type\n");
		exit(EXIT_FAILURE);
  }
  
  return NULL; 
}


Node *deleteFromList(Node *list, int n) {
  
  Node *cur, *prev;
  
  cur = list; 
  prev = NULL; 
  
  // delete only by time
  while (  (cur != NULL) && (cur->value.time != n)) {
	prev = cur; 
	cur = cur-> next;
  }
  
  if (cur == NULL)   /* n was not found */
	return list; 
  if (prev == NULL)  /* n is in the first node */
	list = list->next; 
  else               /* n is in some other node */
	prev->next = cur->next; 
  
  free(cur);
  return list; 
}


int printList(Node *list, char *listName) {
  Node *p = list;
  if (p == NULL){
  	printf("The list %s is empty!\n", listName);
	return EMPTY_LIST;
  }
  
  printf("\n%s:\n", listName);
  printf("Time\t\t\tSpace\n");
  while (p != NULL){
	  printf("%d\t\t\t%lf\n", p->value.time, p->value.space);
	  p=p->next; 
  }
  
  printf("\n"); 
  return 0;
  
}


Node* freeList(Node *list){
	Node *temp;
	
	while (list != NULL){
		temp = list;
		list = list->next;
		free(temp);
	}
	return list;
}
