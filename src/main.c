#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>


#include "constants.h"
#include "interface.h"
#include "model.h"
#include "controller.h"
#include "viewer.h"
#include "timing.h"
#include "list.h"
#include "myTypes.h"
#include "myFunctions.h"
#include "globVar.h"

// global variables in file globVar.c e globVar.h

void initMutex(void);
void initCondVar(void);

int main(int argc, char **argv){

	InputPar cmdLinePar;
	int status;
	char fileName[MAX_SIZE_FILE_NAME];

	// Initialization of global variables
	DeviceInput = NULL;
	DevicePosition = NULL;
	gracefulDegradation = false;


	// Input parsing
	status = parseInput(&cmdLinePar, argc, argv, fileName);
	if (status != 0){
		printf("Something's wrong with the command line parsing\n");
		exit(EXIT_FAILURE);
	}

	// Before creating the threads, let's make a signal mask that will be inherited
	// by all the threads
	sigset_t protMask;
	sigfillset(&protMask);
	if (pthread_sigmask(SIG_BLOCK, &protMask, NULL) != 0){
		printf("Error in setting the process mask\n");
		exit(EXIT_FAILURE);
	}
	// all the blockable signals will be blocked for the main thread and all the
	// others it will create.

	// Initialization of mutexes and condition variables (they're all global)
	initMutex();
	initCondVar();

	// Thread create

	if (pthread_create(&timingID, NULL, (void*)timing, NULL) != 0){
			printf("Thread create: timing\n");
			exit(EXIT_FAILURE);
	}

	// better have the impression that somthing is closing
	if (pthread_create(&viewerID, NULL, (void*)viewer, (void *)&cmdLinePar) != 0){
			printf("Thread create: viewer\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_create(&interfaceID, NULL, (void*)interface, (void *)fileName) != 0){
			printf("Thread create: interface\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_create(&modelID, NULL, (void*)model, (void *)&cmdLinePar) != 0){
			printf("Thread create: model\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_create(&controllerID, NULL, (void*)controller, NULL) != 0){
			printf("Thread create: controller\n");
			exit(EXIT_FAILURE);
	}



	// Thread join

	if (pthread_join(timingID, NULL) != 0){
			printf("Thread join: timing\n");
			exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	printf("Timing joined\n");
	#endif

	if (pthread_join(interfaceID, NULL) != 0){
			printf("Thread join: interface\n");
			exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	printf("Interface joined\n");
	#endif

	if (pthread_join(modelID, NULL) != 0){
			printf("Thread join: model\n");
			exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	printf("Model joined\n");
	#endif

	if (pthread_join(controllerID, NULL) != 0){
			printf("Thread join: controller\n");
			exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	printf("Controller joined\n");
	#endif

	if (pthread_join(viewerID, NULL) != 0){
			printf("Thread join: viewer\n");
			exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	printf("Viewer joined\n");
	#endif

/*	#ifdef DEBUG*/
/*	printInputPar(&cmdLinePar);*/
/*	#endif*/

	exit(EXIT_SUCCESS);
}
//============================================================================//

void initMutex(void){
	if (pthread_mutex_init(&mtxDevIn, NULL) != 0){
			printf("Mutex initialization: mtxDevIn\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_mutex_init(&mtxDevPos, NULL) != 0){
			printf("Mutex initialization: mtxDevPos\n");
			exit(EXIT_FAILURE);
	}
}

void initCondVar(void){
	// Condition variables initialization

	// Synchonization/order of execution

	// Mutual exclusion on buffers
	if (pthread_cond_init(&condDevIn, NULL) != 0){
			printf("Condition Variable initialization: condDevIn\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&condDevPos, NULL) != 0){
			printf("Condition Variable initialization: condDevPos\n");
			exit(EXIT_FAILURE);
	}
}
