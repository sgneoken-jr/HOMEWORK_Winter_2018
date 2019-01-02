#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

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

  // Let's empty the signal mask. This thread gotta catch 'em all
  sigset_t myMask;
  sigemptyset(&myMask);
  if (pthread_sigmask(SIG_BLOCK, &myMask, NULL) != 0){
    printf("Error in setting the process mask\n");
    exit(EXIT_FAILURE);
  }

  // Definitions
  timerTag tTag;
  struct sigevent sigx;
  struct itimerspec val;
  struct sigaction act;

  // Installing the handlers
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  act.sa_sigaction = sigHandler;

  if (sigaction(SIGINT, &act, NULL) == -1) {
    printf("%s\n", "Error in sigaction interrupt");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGQUIT, &act, NULL) == -1) {
    printf("%s\n", "Error in sigaction quit");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGUSR1, &act, NULL) == -1) {
    printf("%s\n", "Error in sigaction sigusr1");
    exit(EXIT_FAILURE);
  }


  // Saving input parameters
  InputPar *myPar = (InputPar *)inPar;
  int ctrlInt = myPar->ctrlPer;
	int viewInt = myPar->viewPer;

  // Timers
  tTag = NUM_TAGS; // timer_tag is an enum type of mine
  timer_t timerID[(int)tTag];

  sigx.sigev_notify = SIGEV_SIGNAL;
  sigx.sigev_signo = SIGUSR1;

  tTag = TIMER_NEW_DATA_TAG;
  sigx.sigev_value.sival_int = (int)tTag;
  if (timer_create(CLOCK_REALTIME, &sigx, &timerID[tTag]) == -1) {
    printf("%s\n", "Error in creating a timer");
    exit(EXIT_FAILURE);
  }

  val.it_value.tv_sec = TIME_UNIT;
  val.it_value.tv_nsec = NTIME_UNIT;
  val.it_interval.tv_sec = 2*TIME_UNIT;
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


  if (timer_settime(timerID[TIMER_NEW_DATA_TAG], 0, &val, NULL) == -1) {
    printf("%s\n", "Error in setting a timer");
    exit(EXIT_FAILURE);
  }

  while (!gracefulDegradation){
    for (int i = 0; i < 5; ++i){
      pause();
    //   if (sigwait(&waitedSignals, &sig) != 0){
    //       printf("%s\n", "Error in sigwait");}
    //       //maybe I don't need to exit

    }
  }

  if (timer_delete(timerID[TIMER_NEW_DATA_TAG]) == -1) {
    printf("%s\n", "Error in deleting a timer");
  }

  pthread_exit(NULL);
}

void sigHandler(int sig, siginfo_t* evp, void* ucontext){
  time_t tim = time(0);
  printf("Timer tag: %d, signo: %d, %s \n", evp->si_value.sival_int, sig, ctime(&tim));

  unsigned int counter = 0; // This may cause overflows

  switch (sig){
    case SIGINT:
      gracefulDegradation = true;
      break;
    case SIGQUIT:
      exit(1);
      break;
    case SIGUSR1:
      #ifdef DEBUG
      printf("%s\n", "Got signal SIGUSR1");
      #endif
      counter++;
      break;
    default:
      // do nothing different from default
      break;
  }
}

void timerHandler(unsigned int counter){
  // Wake interface

  if (counter % ctrlInt == ctrlInt){
    // Wake controller
  }
  if (counter % viewInt == viewInt){
    // Wake viewer
  }


}
