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
	return PCB::getThreadById(id);
}

void Thread::signal(SignalId signal)
{
	myPCB->signal(signal);
}
	
void Thread::registerHandler(SignalId signal, SignalHandler handler)
{
	myPCB->registerHandler(signal, handler);
}
SignalHandler Thread::getHandler(SignalId signal)
{
	return myPCB->getHandler(signal);
}

// Signals
void Thread::maskSignal(SignalId signal)
{
	myPCB->maskSignal(signal);
}
void Thread::maskSignalGlobally(SignalId signal)
{
	PCB::maskSignalGlobally(signal);
}

void Thread::unmaskSignal(SignalId signal)
{
	myPCB->unmaskSignal(signal);
}
void Thread::unmaskSignalGlobally(SignalId signal)
{
	PCB::unmaskSignalGlobally(signal);
}

void Thread::blockSignal(SignalId signal)
{
	myPCB->blockSignal(signal);
}
void Thread::blockSignalGlobally(SignalId signal)
{
	PCB::blockSignalGlobally(signal);
}

void Thread::unblockSignal(SignalId signal)
{
	myPCB->unblockSignal(signal);
}
void Thread::unblockSignalGlobally(SignalId signal)
{
	PCB::unblockSignalGlobally(signal);
}

void Thread::pause()
{
	PCB::pause();	// get current thread and pause it
}

Thread::Thread(StackSize stackSize, Time timeSlice)
{
	myPCB = new PCB(this, stackSize, timeSlice);
}

void dispatch()
{
	Kernel::dispatch();		// call wrapped Kernel::dispatch() method
}

