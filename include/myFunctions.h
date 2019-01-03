#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H

#define ABS(N) ((N<0)?(-N):(N))

/*============================================================================*/
/* Input Parsing */

int checkInput(int argc, char **argv);

int parseInput(InputPar *par, int argc, char **argv, char *fileName);

void printInputPar(InputPar *par);

void upcase(char *str);

/*============================================================================*/
void printHappy(char *msg);

#endif
