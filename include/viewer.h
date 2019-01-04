#ifndef VIEWER_H
#define VIEWER_H

// since the machine precision would make the indicator go over the higher threshold,
// a delta is necessary to visualize the upper limit

void *viewer(void *inPar);
void printBar(int *indicator, int *barLength, int *zeroInd);
double adaptToRange(double *pos, double *lowerLimit, double *upperLimit, int *barLength);
void printCoord(int *time, double *pos);
void printHeader(int *barLength, int *zeroInd);
int viewPos(double *pos, int *time, double *lowerLimit, double *upperLimit, int *barLength, int *zeroInd);

#endif
