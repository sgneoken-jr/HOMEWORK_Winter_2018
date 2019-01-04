#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#include "timing.h"
#include "config.h"
#include "myTypes.h"
#include "myFunctions.h"
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

// counter with visibility in this c file only
static unsigned int counter; // I have to declare it here, since I can't pass it to the handler

void *timing(void *inPar){
  int status;
  #ifdef DEBUG
  printf("%s\n", "[Timing] Launched...");
  #endif

  counter = 0; // static for limiting the visibility to this c file

  // Let's empty the signal mask. This thread gotta catch 'em all
  sigset_t myMask;
  sigemptyset(&myMask);
  if ((status = pthread_sigmask(SIG_SETMASK, &myMask, NULL)) != 0){
    printf("[Timing] Error %d in setting the process mask\n", status);
    exit(EXIT_FAILURE);
  }

  // Handlers
  struct sigaction act;

  if((status = sigemptyset(&act.sa_mask)) != 0){
    printf("[Timing] Error %d in emptying the sigset of sigaction\n", status);
    exit(EXIT_FAILURE);
  }
  act.sa_flags = SA_SIGINFO;
  act.sa_sigaction = sigHandler;

  if ((status = sigaction(SIGINT, &act, NULL)) == -1) {
    printf("[Timing] Error %d in sigaction interrupt\n", status);
    exit(EXIT_FAILURE);
  }

  if ((status = sigaction(SIGQUIT, &act, NULL)) == -1) {
    printf("[Timing] Error %d in sigaction quit\n", status);
    exit(EXIT_FAILURE);
  }

  if ((status = sigaction(SIGUSR1, &act, NULL)) == -1) {
    printf("[Timing] Error %d in sigaction sigusr1\n", status);
    exit(EXIT_FAILURE);
  }

  // Saving input parameters
  InputPar *myPar = (InputPar *)inPar;
  int ctrlInt = myPar->ctrlPer;
	int viewInt = myPar->viewPer;


  // Timers
  timerTag tTag;
  struct sigevent sigx;
  struct itimerspec val;
  tTag = NUM_TAGS; // timer_tag is an enum type of mine
  timer_t timerID[(int)tTag];

  sigx.sigev_notify = SIGEV_SIGNAL;
  sigx.sigev_signo = SIGUSR1;

  tTag = TIMER_NEW_DATA_TAG;
  sigx.sigev_value.sival_int = (int)tTag;
  if ((status = timer_create(CLOCK_REALTIME, &sigx, &timerID[(int)tTag])) == -1) {
    printf("[Timing] Error %d in creating a timer\n", status);
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


  if ((status = timer_settime(timerID[(int)tTag], 0, &val, NULL)) == -1) {
    printf("[Timing] Error %d in setting a timer\n", status);
    exit(EXIT_FAILURE);
  }

  unsigned int lastCounter = 0;

  while (!gracefulDegradation){
      pause();
      lastCounter = counterManager(&counter, &lastCounter, &ctrlInt, &viewInt);
      // #ifdef DEBUG
      // printf("%s %d\n", "The actual value of the counter is:", counter);
      // #endif
  }

  // Let everyone know of the graceful degradation
  if((status = pthread_cond_broadcast(&condDevIn)) != 0){
    printf("[Timing] Error %d in signaling\n", status);
  }
  if((status = pthread_cond_broadcast(&condDevPos)) != 0){
    printf("[Timing] Error %d in signaling\n", status);
  }
  if((status = pthread_cond_signal(&condWakeInterface)) != 0){
    printf("[Timing] Error %d in signaling\n", status);
  } // Otherwise interface could be in a deadlock

  if ((status = timer_delete(timerID[TIMER_NEW_DATA_TAG])) == -1) {
    printf("[Timing] Error %d in deleting a timer\n", status);
  }

  pthread_exit(NULL);
}

void sigHandler(int sig, siginfo_t* evp, void* ucontext){
  // time_t tim = time(0);
  // #ifdef DEBUG
  // printf("Timer tag: %d, signo: %d i.e. %s\n",
  //   evp->si_value.sival_int, sig, strsignal(sig));
  // #endif

  switch (sig){
    case SIGINT:
      gracefulDegradation = true;
      break;
    case SIGQUIT:
      exit(1);
      break;
    case SIGUSR1:
      #ifdef PRINT_ALL
      printf("%s\n", "Got signal SIGUSR1");
      #endif
      counter = (counter + 1) % UINT_MAX; // this can cause a delay when the counter resets
      break;
    default:
      // do nothing different from default
      break;
  }
}

unsigned int counterManager(
  unsigned int *counter, unsigned int *lastCounter, int *ctrlInt, int *viewInt
){
  int status;
  if (*lastCounter < *counter){ // just got SIGUSR1
    // Wake interface
    if ((status = pthread_cond_signal(&condWakeInterface)) != 0){
      printf("[Timing, counterManager] Error %d in signaling\n", status);
    }

    if (*counter % *ctrlInt == (*ctrlInt - 1)){
      // Wake controller
      if ((status = pthread_cond_signal(&condWakeController)) != 0){
        printf("[Timing, counterManager] Error %d in signaling\n", status);
      }
    }

    if (*counter % *viewInt == (*viewInt - 1)){
      // Wake viewer
      if ((status = pthread_cond_signal(&condWakeViewer)) != 0){
        printf("[Timing, counterManager] Error %d in signaling\n", status);
      }
    }

    return *counter; // last counter ought to be updated
  }
  else{ // the signal received was not SIGUSR1
    return *lastCounter;  // last counter can remain the current one
  }
}
