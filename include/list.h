#ifndef list_h
#define list_h

#include "myTypes.h"

typedef enum {timeType, spaceType} searchType;

struct searchThis{
	searchType type;
	union {
		int i;
		double d;
	} val;
};

struct node{
	Coordinate value;
	struct node *next;
};
typedef struct node Node;

/*============================================================================*/

/* insert a node at the beginning of a linked list */
Node *addToList(Node *list, Coordinate *coord);

/* search an element in a linked list */
Node *searchList(Node *list, struct searchThis *v);

/* delete a node from a linked list */
Node *deleteFromList(Node *list, int n);
  
/* print list */
int printList(Node *list, char *listName);

/* free list */
Node *freeList(Node *list); 

#endif
