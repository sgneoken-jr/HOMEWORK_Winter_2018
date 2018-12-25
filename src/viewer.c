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

	}

	pthread_exit(NULL);
}

// C++ code from stack overflow for a progress indicator
// https://stackoverflow.com/questions/14539867/how-to-display-a-progress-indicator-in-pure-c-c-cout-printf
// float progress = 0.0;
// while (progress < 1.0) {
//     int barWidth = 70;
//
//     std::cout << "[";
//     int pos = barWidth * progress;
//     for (int i = 0; i < barWidth; ++i) {
//         if (i < pos) std::cout << "=";
//         else if (i == pos) std::cout << ">";
//         else std::cout << " ";
//     }
//     std::cout << "] " << int(progress * 100.0) << " %\r";
//     std::cout.flush();
//
//     progress += 0.16; // for demonstration only
// }
// std::cout << std::endl;

int updatePosBar(double pos, int barLength){
	printf("%s", "|");
	int indicator = adaptToRange(&pos, &barLength);
	for (int i = 0; i < barLength; ++i){
		if (i == indicator){
			printf("%s", "O");
		}
		else {
			printf("%s", "-");
		}
	}
	printf("%s %0.4lf\n", "|", pos);
	fflush(stdout);

  return indicator;
}

double adaptToRange(double *pos, int *barLength){
  double scaledPos;
  double realRange = MAX - MIN;
  double scaleFactor = *barLength / realRange;
  double offset = ABS(MIN);

  scaledPos = (*pos + offset) * scaleFactor;
  return scaledPos;
}

// This is a problem of the stdout stream being buffered. You have to flush it explicitly (implicit flushing occurs with a \n) using fflush(stdout) after the printf():
//
// fflush(stdout);
