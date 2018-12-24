#include <pthread.h>
#include "myTypes.h"
#include "list.h"
#include "globVar.h"

// Buffers as linked lists
Node *DeviceInput;	// The maximum size should be 2; no need of lists
Node *DevicePosition;

pthread_t interfaceID;
pthread_t modelID;
pthread_t controllerID;
pthread_t viewerID;

pthread_mutex_t mtxDevIn;	// mutex for monitoring DeviceInput
pthread_mutex_t mtxDevPos;	// mutex for monitoring DevicePosition

pthread_cond_t condDevIn;	// cond for monitoring DeviceInput
pthread_cond_t condDevPos;	// cond for monitoring DevicePosition

// Synchronization
pthread_cond_t condModelReady;
pthread_mutex_t mtxModelReady;
