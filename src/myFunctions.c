#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "myTypes.h"
#include "myFunctions.h"
#include "constants.h"

/*============================================================================*/
/* Input Parsing */

int checkInput(int argc, char **argv){
	if (argc != 5){
		if (argc == 7 && strcmp(argv[5], "--file")==0){
			return FILE_INPUT_MODE;
		}
		printf("\n"
			"*** Correct usage: \n"
			"./haptic [posMin] [posMax] [ctrlPer] [viewPer]"
			"\n\n"
			"*** Alternatively, for a non-default input file: \n"
			"./haptic [posMin] [posMax] [ctrlPer] [viewPer] --file [fileName]"
			"\n\n"
		);
		exit(EXIT_FAILURE);
	}
	
	return 0;
}


int parseInput(InputPar *par, int argc, char **argv, char *fileName){
	int status;
	status = checkInput(argc, argv);
	if (status != 0 && status != FILE_INPUT_MODE){
		printf("Something's wrong with the command line");
		exit(EXIT_FAILURE); // it's a redundant control
	}
	// The input was checked, so the variables can be assigned
	par->posMin = strtod(argv[1], NULL);
	par->posMax = strtod(argv[2], NULL);
	par->ctrlPer = atoi(argv[3]);
	par->viewPer = atoi(argv[4]);
	
	if (status == FILE_INPUT_MODE){
		strcpy(fileName, argv[6]);
	}
	else {
		strcpy(fileName, DEFAULT_INPUT_FILE);
	}
	
	return 0;
}


void printInputPar(InputPar *par){
	printf(
		"Min\t\t%0.3lf\nMax\t\t%0.3lf\nT_ctrl\t\t%d\nT_view\t\t%d\n",
		par->posMin, par->posMax, par->ctrlPer, par->viewPer
	);
}

/*============================================================================*/

#ifdef EASTER_EGGS
void printHappy(char *msg){
	char buf[80];
	sprintf(buf,"cowsay -e ^^ 'Yeah, we did it! '%s", msg);
	system(buf);
}
#endif
