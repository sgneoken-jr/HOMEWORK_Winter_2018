#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H

#define ABS(N) ((N<0)?(-N):(N))
#define MIN(X,Y) ((X<Y)?(X):(Y))

/*============================================================================*/
/* Input Parsing */

int checkInput(int argc, char **argv);

int parseInput(InputPar *par, int argc, char **argv, char *fileName);

void printInputPar(InputPar *par);

void upcase(char *str);

/*============================================================================*/
#ifdef EASTER_EGGS
void printHappy(char *msg);
#endif

#endif
