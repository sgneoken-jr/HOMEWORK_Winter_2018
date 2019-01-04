#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "model.h"
#include "config.h"
#include "list.h"
#include "globVar.h"
#include "myTypes.h"
#include "myFunctions.h"

void *model(void* inPar){
	int status;
	#ifdef DEBUG
	printf("[Model] Launched...\n");
	#endif

	// Declaring and initializing a few things before cycling
	InputPar *myPar = (InputPar *)inPar;
	double lowerLimit, upperLimit;
	lowerLimit = myPar->posMin;
	upperLimit = myPar->posMax;

	double incr = 0.0;
	double currPos = 0.0;
	int currTime = 0;
	Coordinate newPosCoord;
	//double newPos; // maybe just currPos is sufficient


	// The data will be in reversed order, since extracted from a list.
	// I can put them in that order into the next list, so that the viewer and
	// the controller will extract them in the correct order.
	#ifdef DEBUG
	printf("%s\n", "[Model] Hey Interface, I'm ready!");
	#endif
	modelReady = true;
	if((status = pthread_cond_signal(&condModelReady)) != 0){
		printf("[Model] Error %d in signaling\n", status);
	}

	while (!gracefulDegradation){

		#ifdef DEBUG
		printf("%s\n", "[Model] Waiting to read from DeviceInput...");
		#endif
		// if gracefulDegradation is set to true while model is waiting and a signal does not come it is a problem
		if((status = pthread_cond_wait(&condDevIn, &mtxDevIn)) != 0){
			printf("[Model] Error %d in waiting\n", status);
		}
		if (gracefulDegradation){ // avoid deadlock in gracefulDegradation
			break;
		}

		#ifdef DEBUG
		printf("%s\n", "[Model]: I got up!");
		#endif
		#ifdef DEBUG
		printf("%s\n", "[Model]");
		printList(DeviceInput, getName(DeviceInput));
		#endif

		// data acquired
		incr = DeviceInput->value.space;
		currTime = DeviceInput->value.time;

		// update position (with hysteresis)
		currPos = updatePosition(currPos, incr, lowerLimit, upperLimit);

		// fill the struct coord to append to DevicePosition
		newPosCoord.space = currPos;
		newPosCoord.time = currTime;

		// append to DevicePosition list
		DevicePosition = addToList(DevicePosition, &newPosCoord);

		// clear the DeviceInput list


		// #ifdef DEBUG
		// printList(DevicePosition, getName(DevicePosition));
		// #endif
	}

	// Releasing mutexes
	if((status = pthread_mutex_unlock(&mtxDevIn)) != 0){
		printf("[Model] Error %d in unlocking mutex\n", status);
	}
	if((status = pthread_mutex_unlock(&mtxDevPos)) != 0){
		printf("[Model] Error %d in unlocking mutex\n", status);
	}

	pthread_exit(NULL);
}


double updatePosition(double currPos, double incr, double lowerLimit, double upperLimit){
	if (currPos + incr > upperLimit){
		currPos = upperLimit;
	}
	else if (currPos + incr < lowerLimit){
		currPos = lowerLimit;
	}
	else {
		currPos += incr;
	}
	return currPos;
}
