#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H

/*============================================================================*/
/* Input Parsing */

int checkInput(int argc, char **argv);

int parseInput(InputPar *par, int argc, char **argv, char *fileName);

void printInputPar(InputPar *par);

/*============================================================================*/
void printHappy(char *msg);

#endif
