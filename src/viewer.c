#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "viewer.h"
#include "list.h"
#include "config.h"
#include "globVar.h"
#include "myTypes.h"
#include "myFunctions.h"
#include "colors.h"


void *viewer(void *inPar){
	int status;
	#ifdef DEBUG
	printf("[Viewer] Launched...\n");
	#endif

	InputPar *myPar = (InputPar *)inPar;
	double lowerLimit, upperLimit;
	lowerLimit = myPar->posMin;
	upperLimit = myPar->posMax;
	int barLength = BAR_LENGTH;

	// Finding the index of the origin
	double pos = 0.0;
	int zeroInd = (int)adaptToRange(&pos, &lowerLimit, &upperLimit, &barLength);

	printHeader(&barLength, &zeroInd);

	// pos = 0.0;
	// time = 1000;
	// ind = viewPos(&pos, &time, &lowerLimit, &upperLimit, &barLength, &zeroInd);
	//
	Node *currNode, *previousNode, *correctOrderList; // saving in local
	Coordinate correctOrderCoord;
	previousNode = NULL;
	correctOrderList = NULL;
	#ifdef PRINT_BAR_INDEX
	int ind; // The index in the bar can possibly be displayed if necessary
	#endif

	while (!gracefulDegradation){

		// Waiting to be awaken by the timing thread
		if ((status = pthread_cond_wait(&condWakeViewer, &mtxWakeViewer)) != 0){
			printf("[Viewer] Error %d in waiting\n", status);
		}
		//------------------------------------------------------------------------//
		// CRITICAL SECTION on DevicePosition
		if((status = pthread_mutex_unlock(&mtxDevPos)) != 0){
			printf("[Viewer] Error %d in unlocking mutex\n", status);
		}

		// Getting the current Node pointer
		currNode = DevicePosition;

		if((status = pthread_mutex_unlock(&mtxDevPos)) != 0){
			printf("[Viewer] Error %d in unlocking mutex\n", status);
		}
		//------------------------------------------------------------------------//
		if (currNode != NULL){
			for (Node *p = currNode; (p != previousNode) && (p != NULL); p = p->next){
				// Coordinates are extracted in reverse order
				correctOrderCoord.space = p->value.space;
				correctOrderCoord.time = p->value.time;
				correctOrderList = addToList(correctOrderList, &correctOrderCoord);
				// reverse order in local auxiliary list
			}

			lastView = currNode->value.time; // last time considered by viewer
			previousNode = currNode;

			for (Node *p = correctOrderList; p != NULL; p = p->next){
				#ifndef PRINT_BAR_INDEX
				viewPos(&(p->value.space), &(p->value.time), &lowerLimit, &upperLimit, &barLength, &zeroInd);
				#else
				ind = viewPos(&(p->value.space), &(p->value.time), &lowerLimit, &upperLimit, &barLength, &zeroInd);
				printf("Index = %d\n", ind);
				#endif
			}

			correctOrderList = freeList(correctOrderList);
		}
	}

	// Release mutex
	if((status = pthread_mutex_unlock(&mtxWakeViewer)) != 0){
		printf("[Viewer] Error %d in unlocking mutex\n", status);
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
