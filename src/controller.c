#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "controller.h"
#include "constants.h"

void *controller(void* inputParameters){
	#ifdef DEBUG
	printf("Controller thread lauched...\n");
	#endif


	pthread_exit(NULL);
}
