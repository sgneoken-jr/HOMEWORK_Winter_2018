#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "timing.h"
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

void *timing(void *ignored){
  sigset_t myMask;
	sigemptyset(&myMask);
	if (pthread_sigmask(SIG_BLOCK, &myMask, NULL) != 0){
		printf("Error in setting the process mask\n");
		exit(EXIT_FAILURE);
	}

  sigset_t waitedSignals;
  sigemptyset(&waitedSignals);
  sigaddset(&waitedSignals, SIGINT);
  int sig;

  while (!gracefulDegradation){
    if (sigwait(&waitedSignals, &sig) != 0){
      printf("%s\n", "Error in sigwait");
      //maybe I don't need to exit
    }
    if (sig == SIGINT){
      interruptHandler();
    }
  }

  pthread_exit(NULL);
}

void interruptHandler(void){
  gracefulDegradation = true;
}
