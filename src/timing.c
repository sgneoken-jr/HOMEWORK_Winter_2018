#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "timing.h"
#include "config.h"
#include "myTypes.h"
#include "globVar.h"

// From the Linux programming interface:
// Multithreaded programs that must deal with asynchronously gener-
// ated signals generally should not use a signal handler as the mechanism to receive
// notification of signal delivery. Instead, the preferred approach is the following:
// - All threads block all of the asynchronous signals that the process might receive.
// The simplest way to do this is to block the signals in the main thread before any
// other threads are created. Each subsequently created thread will inherit a copy
// of the main thread’s signal mask.
// - Create a single dedicated thread that accepts incoming signals using sigwaitinfo(),
// sigtimedwait(), or sigwait(). We described sigwaitinfo() and sigtimedwait()

void *timing(void *inPar){
  #ifdef DEBUG
  printf("%s\n", "Timing thread launched...");
  #endif

  struct sigevent sigx;
  struct itimerspec val;

  InputPar *myPar = (InputPar *)inPar;
  int controllerInterval = myPar->ctrlPer;
	int viewerInterval = myPar->viewPer;
  timerTag tTag;
  tTag = 0;
  #ifdef DEBUG
  printf("%s%d\n", "tTag=", (int)tTag);
  #endif
  tTag = NUM_TAGS; // timer_tag is an enum type of mine

  timer_t timerID[tTag];

  sigset_t myMask;
  sigemptyset(&myMask);
  if (pthread_sigmask(SIG_BLOCK, &myMask, NULL) != 0){
  	printf("Error in setting the process mask\n");
  	exit(EXIT_FAILURE);
  }

  sigset_t waitedSignals;
  sigemptyset(&waitedSignals);
  sigaddset(&waitedSignals, SIGUSR1);
  // sigaddset(&waitedSignals, SIGINT);
  int sig;

  sigx.sigev_notify = SIGEV_SIGNAL;
  sigx.sigev_signo = SIGUSR1;

  tTag = TIMER_NEW_DATA_TAG;
  sigx.sigev_value.sival_int = (int)tTag;
  #ifdef DEBUG
  printf("%s\n", "The timer is going to be created");
  #endif
  if (timer_create(CLOCK_REALTIME, &sigx, &timerID[tTag]) == -1) {
    printf("%s\n", "Error in creating a timer");
    exit(EXIT_FAILURE);
  }

  val.it_value.tv_sec = TIME_UNIT;
  val.it_value.tv_nsec = NTIME_UNIT;
  val.it_interval.tv_sec = TIME_UNIT;
  val.it_interval.tv_nsec = NTIME_UNIT;

  // By   default,   the    initial    expiration    time    specified    in
  //     new_value->it_value  is interpreted relative to the current time on the
  //     timer's clock at the time of the call.  This can be modified by  speci‐
  //     fying  TIMER_ABSTIME  in  flags,  in  which case new_value->it_value is
  //     interpreted as an absolute value as measured on the timer's clock; that
  //     is, the timer will expire when the clock value reaches the value speci‐
  //     fied by  new_value->it_value.   If  the  specified  absolute  time  has
  //     already  passed,  then  the  timer expires immediately, and the overrun
  //     count (see timer_getoverrun(2)) will be set correctly.

  #ifdef DEBUG
  printf("%s\n", "The timer is going to be set");
  #endif
  if (timer_settime(timerID[TIMER_NEW_DATA_TAG], 0, &val, NULL) == -1) {
    printf("%s\n", "Error in setting a timer");
    exit(EXIT_FAILURE);
  }

  if (sigwait(&waitedSignals, &sig) != 0){
      printf("%s\n", "Error in sigwait");
      //maybe I don't need to exit
  }
  if (sig == SIGUSR1){
    sigUsr1Handler();
  }


  if (timer_delete(timerID[TIMER_NEW_DATA_TAG]) == -1) {
    printf("%s\n", "Error in deleting a timer");
    return 1;
  }

  // tTag = TIMER_CTRL_TAG;
  // sigx.sigev_value.sival_int = (int)tTag;
  // if (timer_create(CLOCK_REALTIME, &sigx, &timerID) == -1) {
  //   printf("%s\n", "Error in creating a timer");
  //   exit(EXIT_FAILURE);
  // }
  //
  // tTag = TIMER_VIEW_TAG;
  // sigx.sigev_value.sival_int = (int)tTag;
  // if (timer_create(CLOCK_REALTIME, &sigx, &timerID) == -1) {
  //   printf("%s\n", "Error in creating a timer");
  //   exit(EXIT_FAILURE);
  // }



  // For graceful degradation (Extension 5.1)

  // sigset_t myMask;
	// sigemptyset(&myMask);
	// if (pthread_sigmask(SIG_BLOCK, &myMask, NULL) != 0){
	// 	printf("Error in setting the process mask\n");
	// 	exit(EXIT_FAILURE);
	// }
  //
  // sigset_t waitedSignals;
  // sigemptyset(&waitedSignals);
  // sigaddset(&waitedSignals, SIGINT);
  // int sig;
  //
  // while (!gracefulDegradation){
  //   if (sigwait(&waitedSignals, &sig) != 0){
  //     printf("%s\n", "Error in sigwait");
  //     //maybe I don't need to exit
  //   }
  //   if (sig == SIGINT){
  //     interruptHandler();
  //   }
  // }

  pthread_exit(NULL);
}

void sigUsr1Handler(void){
  printf("%s\n", "Got signal SIGUSR1");
}

void interruptHandler(void){
  gracefulDegradation = true;
}
