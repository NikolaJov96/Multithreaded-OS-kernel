// File: prog04.h

#ifndef PROG01_H_
#define PROG01_H_

#include "thread.h"
#include "semaphor.h"
#include "event.h"

/*!
 * class InfiniteThread
 * Thread with inifinite run method and
 * variable stack size
 */
class SignalThread : public Thread 
{
public:
	SignalThread(StackSize ss, Time ts, Semaphore *s) : 
		Thread(ss, ts), sem(s) {}
	SignalThread(StackSize ss, Semaphore *s) : 
		Thread(ss), sem(s) {}
	SignalThread(Semaphore *s) : 
		Thread(), sem(s) {}
	
protected:
	virtual void run ();
	
private:
	static int flag;
	Semaphore *sem;
	
}

#endif

