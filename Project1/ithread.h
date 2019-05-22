// File: ithread.h

#ifndef ITHREAD_H_
#define ITHREAD_H_

#include "thread.h"

const StackSize idleStackSize = 128;

/*!
 * class IdleThread
 * Empty loop thread, used when all other threads are blocked or waiting
 */
class IdleThread : public Thread {
public:
	IdleThread() : Thread(idleStackSize, 1) {}
	void start();
	
protected:
	virtual void run();
	
};

#endif

