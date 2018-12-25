#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "viewer.h"
#include "constants.h"
#include "globVar.h"
#include "myTypes.h"


void *viewer(void *inputParameters){
	#ifdef DEBUG
	printf("Viewer thread lauched...\n");
	#endif
/*	int width = 104; //characters*/
/*	int height = 50; //characters*/
/*	int xPos = 100; //pixels*/
/*	int yPos = 20; //pixels*/
/*	char newWindow[80];*/
/*	sprintf(newWindow, "gnome-terminal --geometry=%dx%d+%d+%d",*/
/*		width, height, xPos, yPos);*/
/*	system(newWindow);*/

	while (!gracefulDegradation){
		#ifdef DEBUG
		printf("%s\n", "I'm the viewer");
		#endif
	}

	pthread_exit(NULL);
}
