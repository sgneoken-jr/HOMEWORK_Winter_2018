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
	#ifdef DEBUG
	printf("Interface thread lauched...\n");
	#endif
	fileRead((char*)fileName);
	// if (fileRead((char*)fileName) != 0){
	// 	printf("Couldn't read the file!");
	// 	exit(EXIT_FAILURE);
	// }

	// Release mutexes
	pthread_mutex_unlock(&mtxDevIn);
	pthread_mutex_unlock(&mtxModelReady);
	pthread_mutex_unlock(&mtxWakeInterface);

	pthread_exit(NULL);
}


int fileRead (char *fileName) {
    FILE *device_file;
    char line[MAX_LINE_LENGTH];

    device_file = fopen(fileName, "r");

    if (device_file == NULL) {
        printf("Can't open %s\n", fileName);
        exit(EXIT_FAILURE);
    }

    int time;
    double change;
    Coordinate newCoord;

		#ifdef DEBUG
		printf("%s\n", "[Interface]: Waiting for model to be ready");
		#endif
		while (!modelReady){ // to avoid busy waiting and saving from random awakenings of the model
			pthread_cond_wait(&condModelReady, &mtxModelReady); //order of execution
		}

    while ( fgets(line, sizeof(line), device_file) && !gracefulDegradation) {
        sscanf(line, "%d %lf", &time, &change);
        newCoord.time = time;
        newCoord.space = change;

        // Append data to DeviceInput list
				pthread_cond_wait(&condWakeInterface, &mtxWakeInterface); // Timing
				if (gracefulDegradation){ // avoid deadlock in gracefulDegradation
					break;
				}
				#ifdef DEBUG
				printf("%s\n", "[Interface]: I got up!");
				#endif
        DeviceInput = addToList(DeviceInput, &newCoord); // a new node is created

        // signal the data was appended to Model
        pthread_cond_signal(&condDevIn);

				// #ifdef DEBUG
		    // printList(DeviceInput, getName(DeviceInput));
		    // #endif
    }

		pthread_cond_signal(&condDevIn); // Otherwise Model could be in a deadlock


		#ifdef EASTER_EGGS
		if(fgets(line, sizeof(line), device_file) == NULL){
			char *str = "The file is wholly read!";
			printHappy(str);
		}
		#endif

    fclose(device_file);

    return(0);
}
