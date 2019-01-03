#ifndef TIMING_H
#define TIMING_H

typedef enum {
  TIMER_NEW_DATA_TAG = 0,
  TIMER_CTRL_TAG,
  TIMER_VIEW_TAG,
  NUM_TAGS  // this is the number of tags. It is used to create the timerID array
} timerTag;

void *timing(void *inPar);

void sigHandler(int sig, siginfo_t* evp, void* ucontext);

unsigned int counterManager(
  unsigned int *counter, unsigned int *lastCounter, int *ctrlInt, int *viewInt
);


#endif
