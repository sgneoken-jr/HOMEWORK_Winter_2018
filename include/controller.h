#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "list.h"

void *controller(void* inputParameters);
void cleanBuffer(Node *freeFromHere, struct searchThis *whatToFreeFrom);

#endif
