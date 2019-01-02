#ifndef TIMING_H
#define TIMING_H

typedef enum {
  TIMER_NEW_DATA_TAG,
  TIMER_CTRL_TAG,
  TIMER_VIEW_TAG,
  NUM_TAGS  // this is the number of tags. It is used to create the timerID array
} timerTag;

void *timing(void *inPar);

void sigUsr1Handler(void);

void interruptHandler(void);

#endif
