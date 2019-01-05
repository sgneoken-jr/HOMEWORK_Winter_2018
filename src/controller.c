#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "controller.h"
#include "config.h"
#include "myTypes.h"
#include "myFunctions.h"
#include "globVar.h"
#include "list.h"

void *controller(void* inputParameters){
	int status;
	#ifdef DEBUG
	printf("[Controller] Launched...\n");
	#endif

	FILE *device_file;

	char fileName[] = "output.txt";
	device_file = fopen(fileName, "w+");

	if (device_file == NULL) {
			printf("Can't open %s\n", fileName);
			exit(EXIT_FAILURE);
	}

	fprintf(device_file, "%-9s %15.9s\n\n", "Time", "Position"); //resolution is fixed by the format

	Node *currNode, *previousNode, *correctOrderList;
	Coordinate correctOrderCoord;

	// For freeing the list DevicePosition
	Node *freeFromHere = NULL;
	struct searchThis whatToFreeFrom;
	whatToFreeFrom.type = timeType;

	while (!gracefulDegradation){

		if ((status = pthread_cond_wait(&condWakeController, &mtxWakeController)) != 0){
			printf("[Controller] Error %d in waiting\n", status);
		}
		//------------------------------------------------------------------------//
		// CRITICAL SECTION on DevicePosition
		if((status = pthread_mutex_unlock(&mtxDevPos)) != 0){
			printf("[Controller] Error %d in unlocking mutex\n", status);
		}

		// Getting the current Node pointer
		currNode = DevicePosition;
		// if (currNode == NULL){
		// 	break;
		// }

		if((status = pthread_mutex_unlock(&mtxDevPos)) != 0){
			printf("[Controller] Error %d in unlocking mutex\n", status);
		}
		//------------------------------------------------------------------------//
		if (currNode != NULL){ // if the list is not empty
			for (Node *p = currNode; (p != previousNode) && (p != NULL); p = p->next){
				// Coordinates are extracted in reverse order
				correctOrderCoord.space = p->value.space;
				correctOrderCoord.time = p->value.time;
				correctOrderList = addToList(correctOrderList, &correctOrderCoord);
				// reverse order in local auxiliary list
			}

			lastCtrl = currNode->value.time; // last time considered by controller
			previousNode = currNode;

			for (Node *p = correctOrderList; p != NULL; p = p->next){
				fprintf(device_file, "%-9d %15.6lf\n", p->value.time, p->value.space);
			}
			// Free auxiliary list
			correctOrderList = freeList(correctOrderList);

			// Clean up the buffer
			// we'll search the smallest time which has already been covered by both the controller and the viewer
			whatToFreeFrom.val.i = MIN(lastView, lastCtrl);
			cleanBuffer(freeFromHere, &whatToFreeFrom);
			#ifdef DEBUG
			printf("%s\n", "[Controller]");
			printList(DevicePosition, getName(DevicePosition));
			#endif
		}
	}

	// Release mutexes
	// if((status = pthread_mutex_unlock(&mtxDevPos)) != 0){
	// 	printf("[Controller] Error %d in unlocking mutex\n", status);
	// }
	if((status = pthread_mutex_unlock(&mtxWakeController)) != 0){
		printf("[Controller] Error %d in unlocking mutex\n", status);
	}

	fclose(device_file);

	pthread_exit(NULL);
}

void cleanBuffer(Node *freeFromHere, struct searchThis *whatToFreeFrom){
	int timeToKill;
	Node *p, *nextp;
	#ifdef DEBUG
	printf("[Controller, cleaner] time to delete from: %d\n", whatToFreeFrom->val.i);
	#endif
	if ((freeFromHere = searchList(DevicePosition, whatToFreeFrom)) != NULL){
		#ifdef DEBUG
		printf("[Controller, cleaner] entering spiral of violence from: %d\n", freeFromHere->value.time);
		#endif
		p = freeFromHere;

		while (p != NULL){
			timeToKill = p->value.time;
			nextp = p->next;
			#ifdef DEBUG
			printf("[Controller, cleaner] Time to kill: %d\n", p->value.time);
			#endif

			DevicePosition = deleteFromList(DevicePosition, timeToKill);
			p = nextp;
		}
	}
}
