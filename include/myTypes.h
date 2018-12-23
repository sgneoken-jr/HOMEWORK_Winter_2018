#ifndef MYTYPES_H
#define MYTYPES_H

/*============================================================================*/
/* Definitions */

typedef struct InputPar{
	double posMin; // Space is "continuous"
	double posMax;
	int ctrlPer; // Time is discrete
	int viewPer;
} InputPar;

struct coordinate{
	int time;
	double space;
};
typedef struct coordinate Coordinate;


#endif
