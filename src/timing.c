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
