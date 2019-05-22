// File: thread.cpp

#include "thread.h"

#include "PCB.h"
#include "kernel.h"
#include "schedule.h"
#include "queue.h"
	
void Thread::start()
{
	myPCB->start();
}

void Thread::sleep(Time timeToSleep)
{
	myPCB->sleep(timeToSleep);
}

void Thread::waitToComplete()
{
	myPCB->waitToComplete();
}

Thread::~Thread()
{
	myPCB->waitToComplete();
	delete myPCB;
}

ID Thread::getId()
{
	return myPCB->threadID;
}

ID Thread::getRunningId()
{
	return PCB::getRunningId();
}

Thread *Thread::getThreadById(ID id)
{
	return PCB::identificationQueue->findThreadWithID(id);
}

Thread::Thread(StackSize stackSize, Time timeSlice)
{
	myPCB = new PCB(this, stackSize, timeSlice);
}

void dispatch()
{
	Kernel::dispatch();		// call wrapped Kernel::dispatch() method
}

