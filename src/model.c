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
	#ifdef DEBUG
	printf("Model thread lauched...\n");
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
	printf("%s\n", "Model telling Interface it is ready");
	#endif
	pthread_cond_signal(&condModelReady);


	while (!gracefulDegradation){ // EDIT HERE FOR GRACEFUL DEGRADATION
		// when it's time, take the data from DeviceInput:
		// order of execution problem: no signalC before creating the thread

		#ifdef DEBUG
		printf("%s\n", "Model waiting to read from buffer");
		#endif
		pthread_cond_wait(&condDevIn, &mtxDevIn);

		// data acquired
		incr = DeviceInput->value.space;
		currTime = DeviceInput->value.time;

		// update position (with hysteresis)
		currPos = updatePosition(currPos, incr, lowerLimit, upperLimit);

		// fill the list DevicePosition
		newPosCoord.space = currPos;
		newPosCoord.time = currTime;


		// append to DevicePosition list
		DevicePosition = addToList(DevicePosition, &newPosCoord);
/*		pthread_cond_signal(&condDevPos, &mtxDevPos); // must be timed*/

		printList(DevicePosition, getName(DevicePosition));

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
