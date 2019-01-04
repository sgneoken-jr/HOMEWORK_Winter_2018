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
	// int currTime;
	// double currPos;

	// #ifdef DEBUG
	// currTime = 0;
	// currPos = 0.0;
	// #endif

	char fileName[] = "output.txt";
	device_file = fopen(fileName, "w+");

	if (device_file == NULL) {
			printf("Can't open %s\n", fileName);
			exit(EXIT_FAILURE);
	}

	fprintf(device_file, "%-9s %15.9s\n\n", "Time", "Position"); //resolution is fixed by the format

	while (!gracefulDegradation){

// pthread_cond_wait(&condWakeController, &mtxWakeController);

		// currTime = ;
		// currPos = ;

		// fprintf(device_file, "%-9d %15.6lf\n", currTime, currPos);
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
