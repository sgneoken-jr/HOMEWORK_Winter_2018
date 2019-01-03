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
	if (fileRead((char*)fileName) != 0){
		printf("Couldn't read the file!");
		exit(EXIT_FAILURE);
	}

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



    while ( fgets(line, sizeof(line), device_file) ) {
        sscanf(line, "%d %lf", &time, &change);
        newCoord.time = time;
        newCoord.space = change;

				#ifdef DEBUG
				printf("%s\n", "[Interface]: Waiting for model to be ready");
				#endif
				pthread_cond_wait(&condModelReady, &mtxModelReady); //order of execution

        // Append data to DeviceInput list
				pthread_cond_wait(&condWakeInterface, &mtxWakeInterface); // Timing
				#ifdef DEBUG
				printf("%s\n", "[Interface]: I got up!");
				#endif
        DeviceInput = addToList(DeviceInput, &newCoord); // a new node is created

        // signal the data was appended to Model
        pthread_cond_signal(&condDevIn);

				#ifdef DEBUG
		    printList(DeviceInput, getName(DeviceInput));
		    #endif
        #ifdef PRINT_ALL
        printf("At time: %d change of position: %lf\n", time, change);
        #endif
    }


/*    #ifdef TESTING*/
/*    DeviceInput = freeList(DeviceInput);*/
/*    printList(DeviceInput, getName(DeviceInput));*/
/*    #endif*/

    fclose(device_file);

    #ifdef EASTER_EGGS
    char *str = "The file is wholly read!";
    printHappy(str);
    #endif

    return(0);
}
