#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "controller.h"
#include "constants.h"
#include "myTypes.h"
#include "globVar.h"

void *controller(void* inputParameters){
	#ifdef DEBUG
	printf("Controller thread lauched...\n");
	#endif

	while (!gracefulDegradation){

	}

	pthread_exit(NULL);
}
