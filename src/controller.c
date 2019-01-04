#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "controller.h"
#include "config.h"
#include "myTypes.h"
#include "globVar.h"

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

		if((status = pthread_mutex_unlock(&mtxDevPos)) != 0){
			printf("[Controller] Error %d in unlocking mutex\n", status);
		}
		//------------------------------------------------------------------------//
		for (Node *p = currNode; (p != previousNode) && (p != NULL); p = p->next){
			// Coordinates are extracted in reverse order
			correctOrderCoord.space = p->value.space;
			correctOrderCoord.time = p->value.time;
			correctOrderList = addToList(correctOrderList, &correctOrderCoord);
			// reverse order in local auxiliary list
		}

		previousNode = currNode;

		for (Node *p = correctOrderList; p != NULL; p = p->next){
			fprintf(device_file, "%-9d %15.6lf\n", p->value.time, p->value.space);
		}
		correctOrderList = freeList(correctOrderList);
	}

	// Release mutexes
	if((status = pthread_mutex_unlock(&mtxDevPos)) != 0){
		printf("[Controller] Error %d in unlocking mutex\n", status);
	}
	if((status = pthread_mutex_unlock(&mtxWakeController)) != 0){
		printf("[Controller] Error %d in unlocking mutex\n", status);
	}


	fclose(device_file);

	pthread_exit(NULL);
}
