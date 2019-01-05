#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "config.h"
#include "interface.h"
#include "list.h"
#include "globVar.h"
#include "myTypes.h"
#include "myFunctions.h"


void *interface(void *fileName){
	int status;
	#ifdef DEBUG
	printf("[Interface] Launched...\n");
	#endif
	fileRead((char*)fileName);
	// #ifdef DEBUG
	// printf("[Interface] Last time read: %d\n", inputFileLastTime);
	// #endif

	// Release mutexes
	if((status = pthread_mutex_unlock(&mtxDevIn)) != 0){
		printf("[Interface] Error %d in unlocking mutex", status);
	}

	if((status = pthread_mutex_unlock(&mtxWakeInterface)) != 0){
		printf("[Interface] Error %d in unlocking mutex", status);
	}

	pthread_exit(NULL);
}

// This is the function reading from the input file.
// It returns the last time read from the input file.
int fileRead (char *fileName) {
	int status;
  FILE *device_file;
  char line[MAX_LINE_LENGTH];

  device_file = fopen(fileName, "r");

  if (device_file == NULL) {
      printf("[Interface] Can't open %s\n", fileName);
			printf("%s\n", "[Interface] Suggestion: did you write the correct relative path?");
      exit(EXIT_FAILURE);
  }

  int time;
  double change;
  Coordinate newCoord;

	#ifdef DEBUG
	printf("%s\n", "[Interface]: Waiting for model to be ready");
	#endif
	if((status = pthread_mutex_lock(&mtxModelReady)) != 0){
		printf("[Interface] Error %d in locking mutex", status);
	}

	while (!modelReady){ // to avoid busy waiting and to prevent from random awakenings of the model
		if((status = pthread_cond_wait(&condModelReady, &mtxModelReady)) != 0){
			printf("[Interface] Error %d in waiting\n", status);
		} //order of execution
	}
	if((status = pthread_mutex_unlock(&mtxModelReady)) != 0){
		printf("[Interface] Error %d in unlocking mutex", status);
	}


  while ( fgets(line, sizeof(line), device_file) && !gracefulDegradation) {
      sscanf(line, "%d %lf", &time, &change);
      newCoord.time = time;
      newCoord.space = change;

      // Append data to DeviceInput list
			// Wait for your time
			if((status = pthread_mutex_lock(&mtxWakeInterface)) != 0){
				printf("[Interface] Error %d in locking mutex", status);
			}
			if((status = pthread_cond_wait(&condWakeInterface, &mtxWakeInterface)) != 0){
				printf("[Interface] Error %d in waiting\n", status);
			} // Timing
			// Now mtxWakeInterface is locked. Unlock it!
			if((status = pthread_mutex_unlock(&mtxWakeInterface)) != 0){
				printf("[Interface] Trying to unlock on DevIn gave error: %d\n", status);
			}

			if (gracefulDegradation){ // avoid deadlock in gracefulDegradation
				break;
			}
			#ifdef DEBUG
			printf("%s\n", "[Interface]: I got up!");
			#endif

			//------------------------------------------------------------------------//
			// CRITICAL SECTION on DeviceInput
			if((status = pthread_mutex_lock(&mtxDevIn)) != 0){
				printf("[Interface] Trying to lock on DevIn gave error: %d\n", status);
			}

      DeviceInput = addToList(DeviceInput, &newCoord); // a new node is created

			if((status = pthread_mutex_unlock(&mtxDevIn)) != 0){
				printf("[Interface] Trying to unlock on DevIn gave error: %d\n", status);
			}
			//------------------------------------------------------------------------//
      // signal the data was appended to Model
      if((status = pthread_cond_signal(&condDevIn)) != 0){
				printf("[Interface] Error %d in signaling\n", status);
			}

			// #ifdef DEBUG
	    // printList(DeviceInput, getName(DeviceInput));
	    // #endif
  }

	// #ifdef DEBUG
	// printList(DeviceInput, getName(DeviceInput));
	// #endif
	#ifdef DEBUG
		#ifdef EASTER_EGGS
		if(fgets(line, sizeof(line), device_file) == NULL){
			char *str = "The file is wholly read!";
			printHappy(str);
		}
		#else
		if(fgets(line, sizeof(line), device_file) == NULL){
			printf("%s\n", "[Interface] File reading complete!");
		}
		#endif
	#endif

  fclose(device_file);

  return(time);
}
