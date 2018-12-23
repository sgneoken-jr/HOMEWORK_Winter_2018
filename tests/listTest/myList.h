#ifndef MYLIST_H
#define MYLIST_H

typedef enum {timeType, spaceType} searchType;

struct searchThis{
	searchType type;
	union {
		int i;
		double d;
	} val;
};

struct coordinate{
	int time;
	double space;
};
typedef struct coordinate Coordinate;

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
void printList(Node *list);

/* free list */
void freeList(Node *list);


#endif
