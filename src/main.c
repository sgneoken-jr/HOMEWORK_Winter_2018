#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>


#include "config.h"
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
void createThreads(InputPar *cmdLinePar, char* fileName);
void joinThreads(void);

void initMutex(void);
void initCondVar(void);
void unlockMutex(void); // for safety, before destroying them
void destroyMutex(void);
void destroyCondVar(void);


int main(int argc, char **argv){

	InputPar cmdLinePar;
	int status;
	char fileName[MAX_SIZE_FILE_NAME];

	// Initialization of global variables
	DeviceInput = NULL;
	DevicePosition = NULL;
	gracefulDegradation = false;
	modelReady = false;


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
	createThreads(&cmdLinePar, fileName);

	// Thread join
	// Each thread must release on its own the mutexes it uses BEFORE joining
	joinThreads();

	// Release and clean up all the mutex
	unlockMutex();
	destroyMutex();
	destroyCondVar();

	#ifdef DEBUG
	printf("%s\n", "Exiting successfully...");
	#endif
	exit(EXIT_SUCCESS);
}
//============================================================================//

void createThreads(InputPar *cmdLinePar, char* fileName){

	if (pthread_create(&timingID, NULL, (void*)timing, (void *)cmdLinePar) != 0){
			printf("Thread create: timing\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_create(&interfaceID, NULL, (void*)interface, (void *)fileName) != 0){
			printf("Thread create: interface\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_create(&modelID, NULL, (void*)model, (void *)cmdLinePar) != 0){
			printf("Thread create: model\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_create(&controllerID, NULL, (void*)controller, NULL) != 0){
			printf("Thread create: controller\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_create(&viewerID, NULL, (void*)viewer, (void *)cmdLinePar) != 0){
			printf("Thread create: viewer\n");
			exit(EXIT_FAILURE);
	}
}


void joinThreads(void){

	// at first, let's close threads the user sees
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

	// then, let's close the "hidden work"
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

	if (pthread_join(timingID, NULL) != 0){ // shall be the last, since is the one receiving signals
			printf("Thread join: timing\n");
			exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	printf("Timing joined\n");
	#endif
}

void initMutex(void){
	if (pthread_mutex_init(&mtxDevIn, NULL) != 0){
			printf("Mutex initialization: mtxDevIn\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_mutex_init(&mtxDevPos, NULL) != 0){
			printf("Mutex initialization: mtxDevPos\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_mutex_init(&mtxModelReady, NULL) != 0){
			printf("Mutex initialization: mtxModelReady\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_mutex_init(&mtxWakeInterface, NULL) != 0){
			printf("Mutex initialization: mtxWakeInterface\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_mutex_init(&mtxWakeController, NULL) != 0){
			printf("Mutex initialization: mtxWakeController\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_mutex_init(&mtxWakeViewer, NULL) != 0){
			printf("Mutex initialization: mtxWakeViewer\n");
			exit(EXIT_FAILURE);
	}
}

void initCondVar(void){
	if (pthread_cond_init(&condDevIn, NULL) != 0){
			printf("Condition Variable initialization: condDevIn\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&condDevPos, NULL) != 0){
			printf("Condition Variable initialization: condDevPos\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&condModelReady, NULL) != 0){
			printf("Condition Variable initialization: condModelReady\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&condWakeInterface, NULL) != 0){
			printf("Condition Variable initialization: condWakeInterface\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&condWakeController, NULL) != 0){
			printf("Condition Variable initialization: condWakeController\n");
			exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&condWakeViewer, NULL) != 0){
			printf("Condition Variable initialization: condWakeViewer\n");
			exit(EXIT_FAILURE);
	}
}

void unlockMutex(void){ // for safety, before destroying them
	int s;
	if((s = pthread_mutex_unlock(&mtxDevIn)) != 0){
		printf("Mutex unlocking: mtxDevIn\n");
		#ifdef DEBUG
		printf("Error: %d\n", s);
		#endif
	}

	if((s = pthread_mutex_unlock(&mtxDevPos)) != 0){
		printf("Mutex unlocking: mtxDevPos\n");
		#ifdef DEBUG
		printf("Error: %d\n", s);
		#endif
	}

	if((s = pthread_mutex_unlock(&mtxModelReady)) != 0){
		printf("Mutex unlocking: mtxModelReady\n");
		#ifdef DEBUG
		printf("Error: %d\n", s);
		#endif
	}

	if((s = pthread_mutex_unlock(&mtxWakeController)) != 0){
		printf("Mutex unlocking: mtxWakeInterface\n");
		#ifdef DEBUG
		printf("Error: %d\n", s);
		#endif
	}

	if((s = pthread_mutex_unlock(&mtxWakeInterface)) != 0){
		printf("Mutex unlocking: mtxWakeController\n");
		#ifdef DEBUG
		printf("Error: %d\n", s);
		#endif
	}

	if((s = pthread_mutex_unlock(&mtxWakeViewer)) != 0){
		printf("Mutex unlocking: mtxWakeViewer\n");
		#ifdef DEBUG
		printf("Error: %d\n", s);
		#endif
	}

}

void destroyMutex(void){
	int s;
	if ((s = pthread_mutex_destroy(&mtxDevIn)) != 0){
			printf("Mutex destruction: mtxDevIn\n");
			#ifdef DEBUG
			printf("Error: %d\n", s);
			#endif
	}

	if ((s =pthread_mutex_destroy(&mtxDevPos)) != 0){
			printf("Mutex destruction: mtxDevPos\n");
			#ifdef DEBUG
			printf("Error: %d\n", s);
			#endif
	}

	if ((s =pthread_mutex_destroy(&mtxModelReady)) != 0){
			printf("Mutex destruction: mtxModelReady\n");
			#ifdef DEBUG
			printf("Error: %d\n", s);
			#endif
	}

	if ((s =pthread_mutex_destroy(&mtxWakeInterface)) != 0){
			printf("Mutex destruction: mtxWakeInterface\n");
			#ifdef DEBUG
			printf("Error: %d\n", s);
			#endif
	}

	if ((s =pthread_mutex_destroy(&mtxWakeController)) != 0){
			printf("Mutex destruction: mtxWakeController\n");
			#ifdef DEBUG
			printf("Error: %d\n", s);
			#endif
	}

	if ((s =pthread_mutex_destroy(&mtxWakeViewer)) != 0){
			printf("Mutex destruction: mtxWakeViewer\n");
			#ifdef DEBUG
			printf("Error: %d\n", s);
			#endif
	}
}

void destroyCondVar(void){
	if (pthread_cond_destroy(&condDevIn) != 0){
			printf("Condition Variable destruction: condDevIn\n");
	}

	if (pthread_cond_destroy(&condDevPos) != 0){
			printf("Condition Variable destruction: condDevPos\n");
	}

	if (pthread_cond_destroy(&condModelReady) != 0){
			printf("Condition Variable destruction: condModelReady\n");
	}

	if (pthread_cond_destroy(&condWakeInterface) != 0){
			printf("Condition Variable destruction: condWakeInterface\n");
	}

	if (pthread_cond_destroy(&condWakeController) != 0){
			printf("Condition Variable destruction: condWakeController\n");
	}

	if (pthread_cond_destroy(&condWakeViewer) != 0){
			printf("Condition Variable destruction: condWakeViewer\n");
	}
}
