#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>


#include "constants.h"
#include "interface.h"
#include "model.h"
#include "controller.h"
#include "viewer.h"
#include "list.h"
#include "myTypes.h"
#include "myFunctions.h"
#include "globVar.h"

// global variables in file globVar.c e globVar.h

int main(int argc, char **argv){
	
	InputPar cmdLinePar;
	int status;
	char fileName[MAX_SIZE_FILE_NAME];
	
	// Initialization of global variables
	DeviceInput = NULL;
	DevicePosition = NULL;
	
	
	// Input parsing
	status = parseInput(&cmdLinePar, argc, argv, fileName);
	if (status != 0){
		printf("Something's wrong with the command line parsing\n");
		exit(EXIT_FAILURE);
	}
	
	#ifdef DEBUG
	printf("Reading from file: %s\n", fileName);
	#endif
	
	// Condition variables initialization
	if (pthread_cond_init(&condDevIn, NULL) != 0){
			printf("Condition Variable initialization: condDevIn\n");
			exit(EXIT_FAILURE);
	}	
	
	if (pthread_cond_init(&condDevPos, NULL) != 0){
			printf("Condition Variable initialization: condDevPos\n");
			exit(EXIT_FAILURE);
	}
	
	
	// Thread create
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
	
	if (pthread_create(&viewerID, NULL, (void*)viewer, (void *)&cmdLinePar) != 0){
			printf("Thread create: viewer\n");
			exit(EXIT_FAILURE);
	}	
	
	
	// Thread join
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
