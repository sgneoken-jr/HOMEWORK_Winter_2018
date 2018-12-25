#ifndef GLOBVAR_H
#define GLOBVAR_H

#include <pthread.h>
#include "myTypes.h"
#include "list.h"

extern bool gracefulDegradation;

extern Node* DeviceInput;
extern Node *DevicePosition;

extern pthread_t interfaceID;
extern pthread_t modelID;
extern pthread_t controllerID;
extern pthread_t viewerID;
extern pthread_t timingID;

// Mutual exclusion on buffers
extern pthread_mutex_t mtxDevIn;	// mutex for monitoring DeviceInput
extern pthread_mutex_t mtxDevPos;	// mutex for monitoring DevicePosition

extern pthread_cond_t condDevIn;	// cond for monitoring DeviceInput
extern pthread_cond_t condDevPos;	// cond for monitoring DevicePosition

// Synchronization
extern pthread_cond_t condModelReady;
extern pthread_mutex_t mtxModelReady;

#endif