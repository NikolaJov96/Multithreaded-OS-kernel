// File: prog03.h

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
class EventThread : public Thread 
{
public:
	EventThread(StackSize ss, Time ts, Semaphore *s) : 
		Thread(ss, ts), sem(s) {}
	EventThread(StackSize ss, Semaphore *s) : 
		Thread(ss), sem(s) {}
	EventThread(Semaphore *s) : 
		Thread(), sem(s) {}
	
protected:
	virtual void run ();
	
private:
	Semaphore *sem;
	
}

#endif

