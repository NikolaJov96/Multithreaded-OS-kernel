// File: prog02.h

#ifndef PROG01_H_
#define PROG01_H_

#include "thread.h"
#include "semaphor.h"

/*!
 * class InfiniteThread
 * Thread with inifinite run method and
 * variable stack size
 */
class InfiniteThread : public Thread 
{
public:
	InfiniteThread(StackSize ss, Time ts, Semaphore *s) : 
		Thread(ss, ts), sem(s) {}
	InfiniteThread(StackSize ss, Semaphore *s) : 
		Thread(ss), sem(s) {}
	InfiniteThread(Semaphore *s) : 
		Thread(), sem(s) {}
	
protected:
	virtual void run ();
	
private:
	Semaphore *sem;
	
}

#endif

