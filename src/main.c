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
void releaseMutex(void); // for safety, before destroying them
void destroyMutex(void);
void destroyCondVar(void);

//============================================================================//
// Main Thread
int main(int argc, char **argv){
	InputPar cmdLinePar;
	int status;
	char fileName[MAX_SIZE_FILE_NAME];

	// Initialization of global variables
	DeviceInput = NULL;
	DevicePosition = NULL;
	gracefulDegradation = false;
	modelReady = false;
	// inputFileLastTime = -1; // Initializing it as negative is safer (time will always be a positive int)


	// Input parsing
	if ((status = parseInput(&cmdLinePar, argc, argv, fileName)) != 0){
		printf("[Main] Something's wrong with the command line parsing\n");
		exit(EXIT_FAILURE);
	}

	// Before creating the threads, let's make a signal mask that will be inherited
	// by all the threads
	sigset_t protMask;
	sigfillset(&protMask);
	if ((status = pthread_sigmask(SIG_BLOCK, &protMask, NULL)) != 0){
		printf("[Main] Error %d in setting the process mask\n", status);
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
	// releaseMutex();
	destroyMutex();
	destroyCondVar();

	#ifdef DEBUG
	printf("%s\n", "[Main] Exiting successfully...");
	#endif
	exit(EXIT_SUCCESS);
}
//============================================================================//
// Creating and joining threads
void createThreads(InputPar *cmdLinePar, char* fileName){
	int status;
	if ((status = pthread_create(&timingID, NULL, (void*)timing, (void *)cmdLinePar)) != 0){
			printf("[Main] Error %d in thread create: timing\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_create(&interfaceID, NULL, (void*)interface, (void *)fileName)) != 0){
			printf("[Main] Error %d in thread create: interface\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_create(&modelID, NULL, (void*)model, (void *)cmdLinePar)) != 0){
			printf("[Main] Error %d in thread create: model\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_create(&controllerID, NULL, (void*)controller, NULL)) != 0){
			printf("[Main] Error %d in thread create: controller\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_create(&viewerID, NULL, (void*)viewer, (void *)cmdLinePar)) != 0){
			printf("[Main] Error %d in thread create: viewer\n", status);
			exit(EXIT_FAILURE);
	}
}


void joinThreads(void){
	int status;
	// at first, let's close threads the user sees
	if ((status = pthread_join(controllerID, NULL)) != 0){
			printf("[Main] Error %d in thread join: controller\n", status);
			exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	printf("[Main] Controller joined\n");
	#endif

	if ((status = pthread_join(viewerID, NULL)) != 0){
			printf("[Main] Error %d in thread join: viewer\n", status);
			exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	printf("[Main] Viewer joined\n");
	#endif

	// then, let's close the "hidden work"
	if ((status = pthread_join(interfaceID, NULL)) != 0){
			printf("[Main] Error %d in thread join: interface\n", status);
			exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	printf("[Main] Interface joined\n");
	#endif

	// To prevent deadlock of Model if the file is completely read
	if((status = pthread_cond_signal(&condDevIn)) != 0){
		printf("[Main] Error %d in signaling condDevIn\n", status);
	} // Otherwise Model could be in a deadlock


	if ((status = pthread_join(modelID, NULL)) != 0){
			printf("[Main] Error %d in thread join: model\n", status);
			exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	printf("[Main] Model joined\n");
	#endif

	everyoneJoined = true;  // allows timing to join, otherwise it keeps on waiting for signals

	if ((status = pthread_join(timingID, NULL)) != 0){ // shall be the last, since is the one receiving signals
			printf("[Main] Error %d in thread join: timing\n", status);
			exit(EXIT_FAILURE);
	}
	#ifdef DEBUG
	printf("[Main] Timing joined\n");
	#endif
}
//----------------------------------------------------------------------------//
// Creation, clean-up and destruction of mutexes and condition variables
void initMutex(void){
	int status;
	if ((status = pthread_mutex_init(&mtxDevIn, NULL)) != 0){
			printf("[Main] Error %d in mutex initialization: mtxDevIn\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_mutex_init(&mtxDevPos, NULL)) != 0){
			printf("[Main] Error %d in mutex initialization: mtxDevPos\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_mutex_init(&mtxModelReady, NULL)) != 0){
			printf("[Main] Error %d in mutex initialization: mtxModelReady\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_mutex_init(&mtxWakeInterface, NULL)) != 0){
			printf("[Main] Error %d in mutex initialization: mtxWakeInterface\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_mutex_init(&mtxWakeController, NULL)) != 0){
			printf("[Main] Error %d in mutex initialization: mtxWakeController\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_mutex_init(&mtxWakeViewer, NULL)) != 0){
			printf("[Main] Error %d in mutex initialization: mtxWakeViewer\n", status);
			exit(EXIT_FAILURE);
	}
}

void initCondVar(void){
	int status;
	if ((status = pthread_cond_init(&condDevIn, NULL)) != 0){
			printf("[Main] Error %d in condition variable initialization: condDevIn\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_cond_init(&condDevPos, NULL)) != 0){
			printf("[Main] Error %d in condition variable initialization: condDevPos\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_cond_init(&condModelReady, NULL)) != 0){
			printf("[Main] Error %d in condition variable initialization: condModelReady\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_cond_init(&condWakeInterface, NULL)) != 0){
			printf("[Main] Error %d in condition variable initialization: condWakeInterface\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_cond_init(&condWakeController, NULL)) != 0){
			printf("[Main] Error %d in condition variable initialization: condWakeController\n", status);
			exit(EXIT_FAILURE);
	}

	if ((status = pthread_cond_init(&condWakeViewer, NULL)) != 0){
			printf("[Main] Error %d in condition variable initialization: condWakeViewer\n", status);
			exit(EXIT_FAILURE);
	}
}

void releaseMutex(void){ // for safety, before destroying them
	int status;
	// // To avoid EBUSY error, I must lock them and then unlock them
	//
	// if((status = pthread_mutex_lock(&mtxDevIn)) != 0){
	// 	printf("[Main] Error %d in mutex locking: mtxDevIn\n", status);
	// }
	//
	// if((status = pthread_mutex_lock(&mtxDevPos)) != 0){
	// 	printf("[Main] Error %d in mutex locking: mtxDevPos\n", status);
	// }
	//
	// if((status = pthread_mutex_lock(&mtxModelReady)) != 0){
	// 	printf("[Main] Error %d in mutex locking: mtxModelReady\n", status);
	// }
	//
	// if((status = pthread_mutex_lock(&mtxWakeController)) != 0){
	// 	printf("[Main] Error %d in mutex locking: mtxWakeInterface\n", status);
	// }
	//
	// if((status = pthread_mutex_lock(&mtxWakeInterface)) != 0){
	// 	printf("[Main] Error %d in mutex locking: mtxWakeController\n", status);
	// }
	//
	// if((status = pthread_mutex_lock(&mtxWakeViewer)) != 0){
	// 	printf("[Main] Error %d in mutex locking: mtxWakeViewer\n", status);
	// }
	//

	if((status = pthread_mutex_unlock(&mtxDevIn)) != 0){
		printf("[Main] Error %d in mutex unlocking: mtxDevIn\n", status);
	}

	if((status = pthread_mutex_unlock(&mtxDevPos)) != 0){
		printf("[Main] Error %d in mutex unlocking: mtxDevPos\n", status);
	}

	if((status = pthread_mutex_unlock(&mtxModelReady)) != 0){
		printf("[Main] Error %d in mutex unlocking: mtxModelReady\n", status);
	}

	if((status = pthread_mutex_unlock(&mtxWakeController)) != 0){
		printf("[Main] Error %d in mutex unlocking: mtxWakeInterface\n", status);
	}

	if((status = pthread_mutex_unlock(&mtxWakeInterface)) != 0){
		printf("[Main] Error %d in mutex unlocking: mtxWakeController\n", status);
	}

	if((status = pthread_mutex_unlock(&mtxWakeViewer)) != 0){
		printf("[Main] Error %d in mutex unlocking: mtxWakeViewer\n", status);
	}

}

void destroyMutex(void){
	int status;
	if ((status = pthread_mutex_destroy(&mtxDevIn)) != 0){
			printf("[Main] Error %d in mutex destruction: mtxDevIn\n", status);
	}

	if ((status = pthread_mutex_destroy(&mtxDevPos)) != 0){
			printf("[Main] Error %d in mutex destruction: mtxDevPos\n", status);
	}

	if ((status = pthread_mutex_destroy(&mtxModelReady)) != 0){
			printf("[Main] Error %d in mutex destruction: mtxModelReady\n", status);
	}

	if ((status = pthread_mutex_destroy(&mtxWakeInterface)) != 0){
			printf("[Main] Error %d in mutex destruction: mtxWakeInterface\n", status);
	}

	if ((status = pthread_mutex_destroy(&mtxWakeController)) != 0){
			printf("[Main] Error %d in mutex destruction: mtxWakeController\n", status);
	}

	if ((status = pthread_mutex_destroy(&mtxWakeViewer)) != 0){
			printf("[Main] Error %d in mutex destruction: mtxWakeViewer\n", status);
	}
}

void destroyCondVar(void){
	int status;
	if ((status = pthread_cond_destroy(&condDevIn)) != 0){
			printf("[Main] Error %d in condition variable destruction: condDevIn\n", status);
	}

	if ((status = pthread_cond_destroy(&condDevPos)) != 0){
			printf("[Main] Error %d in condition variable destruction: condDevPos\n", status);
	}

	if ((status = pthread_cond_destroy(&condModelReady)) != 0){
			printf("[Main] Error %d in condition variable destruction: condModelReady\n", status);
	}

	if ((status = pthread_cond_destroy(&condWakeInterface)) != 0){
			printf("[Main] Error %d in condition variable destruction: condWakeInterface\n", status);
	}

	if ((status = pthread_cond_destroy(&condWakeController)) != 0){
			printf("[Main] Error %d in condition variable destruction: condWakeController\n", status);
	}

	if ((status = pthread_cond_destroy(&condWakeViewer)) != 0){
			printf("[Main] Error %d in condition variable destruction: condWakeViewer\n", status);
	}
}
