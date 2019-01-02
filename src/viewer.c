#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "viewer.h"
#include "config.h"
#include "globVar.h"
#include "myTypes.h"
#include "myFunctions.h"
#include "colors.h"



void *viewer(void *inPar){
	#ifdef DEBUG
	printf("Viewer thread lauched...\n");
	#endif

	InputPar *myPar = (InputPar *)inPar;
	double lowerLimit, upperLimit;
	lowerLimit = myPar->posMin;
	upperLimit = myPar->posMax;
	int barLength = BAR_LENGTH;
	int ind;

	// int newTerminalStatus = system("gnome-terminal");

	double pos = 0.0;
	int time = 0;
	int zeroInd = (int)adaptToRange(&pos, &lowerLimit, &upperLimit, &barLength);

	printHeader(&barLength, &zeroInd);

	pos = 10.0;
	ind = viewPos(&pos, &time, &lowerLimit, &upperLimit, &barLength, &zeroInd);


	pos = 0.0;
	time = 1000;
	ind = viewPos(&pos, &time, &lowerLimit, &upperLimit, &barLength, &zeroInd);

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


void printBar(int *indicator, int *barLength, int *zeroInd){
	printf("%s", "|");
	for (int i = 0; i < *barLength; ++i){
		if (i == *indicator){
			printf("%s", "X");
		}
		else if (i < *zeroInd && i != *indicator){
			printf(NEG_COLOR"%s"RESET, "-");
		}
    else if (i > *zeroInd  && i != *indicator){
      printf(POS_COLOR"%s"RESET, "-");
    }
    else {
      printf(ZERO_COLOR"%s"RESET, "-");
    }
	}
	printf("%s", "|");
}

void printCoord(int *time, double *pos){
  printf("%9d %+15.6lf\n", *time, *pos);
}

double adaptToRange(double *pos, double *lowerLimit, double *upperLimit, int *barLength){
  double scaledPos;
  double realRange = *upperLimit - *lowerLimit;
  double scaleFactor = *barLength / realRange;
  double offset = ABS(*lowerLimit);

  scaledPos = (*pos + offset) * scaleFactor;
  return scaledPos;
}

void printHeader(int *barLength, int *zeroInd){
	printf(BOLDDEFAULT"%s"RESET, "|");
	for (int i = 0; i < *barLength; ++i){
    if (i < *zeroInd){
      printf(BOLD_NEG_COLOR"%s"RESET, "=");
    }
    else if (i == *zeroInd){
      printf(BOLD_ZERO_COLOR"%s"RESET, "0");
    }
    else {
      printf(BOLD_POS_COLOR"%s"RESET, "=");
    }

	}
	printf(BOLDDEFAULT"%s"RESET, "|");
	printf(BOLDDEFAULT"%9s %15.9s\n"RESET, "Time", "Position" );
}


int viewPos(double *pos, int *time, double *lowerLimit, double *upperLimit, int *barLength, int *zeroInd){
  int indicator = (int)adaptToRange(pos, lowerLimit, upperLimit, barLength);
  printBar(&indicator, barLength, zeroInd);
  printCoord(time, pos);

  return indicator;
}
// This is a problem of the stdout stream being buffered. You have to flush it explicitly (implicit flushing occurs with a \n) using fflush(stdout) after the printf():
//
// fflush(stdout);
