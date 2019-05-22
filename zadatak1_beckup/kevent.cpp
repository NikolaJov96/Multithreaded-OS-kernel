// File: kevent.cpp

#include "kevent.h"
#include "event.h"

#include "schedule.h"

#include "iostream.h"
KernelEv::KernelEv(IVTNo ivtNo, Event *event) :
	entry(ivtNo)
{
	Kernel::PBDepth++;		// Mind closing
	
	parentThread = Kernel::running;
	waiting = 0;
	boundedEvents[entry] = event;
	// Initialize new interrupt routine (done in PREPAREENTRY())
	// oldRoutine = initializeInterrupt(ivtNo, ?);
	
	Kernel::PBDepth--;
}

KernelEv::~KernelEv()
{
	Kernel::PBDepth++;		// Mind closing
	boundedEvents[entry] = NULL;
	// Restore old routine (done at the end of the program)
	// initializeInterrupt(entry);
	Kernel::PBDepth--;
}

void KernelEv::wait()
{
	Kernel::PBDepth++;		// Mind closing
	
	if (Kernel::running != parentThread) 
	{
		Kernel::PBDepth--;
		return;
	}
	
	Kernel::running->state = BLOCKED;
	Kernel::running->waiting = this;
	waiting = 1;
	Kernel::dispatch();
	
	Kernel::PBDepth--;
}

void KernelEv::signal()		// called by interrupt routine
{
	Kernel::PBDepth++;		// Mind closing
	
	if (waiting) 
	{
		parentThread->state = READY;
		parentThread->waiting = NULL;
		waiting = 0;
		Scheduler::put(parentThread);
		Kernel::dispatch();
	}
	
	Kernel::PBDepth--;
}
	
int KernelEv::iCall(PCB *pcb)
{
	Kernel::PBDepth++;		// Mind closing
	//return waitQueue.removePCB(pcb);
	waiting = 0;
	Kernel::PBDepth--;
}

void setupRoutine(int id, InterruptRoutine routine)
{
	Kernel::oldRoutines[id] = Kernel::initializeInterrupt(id, routine);
}

void IVTEntry::handle(int id, int oldRoutine)
{
	if (oldRoutine) Kernel::oldRoutines[id]();
	if (boundedEvents[id]) boundedEvents[id]->signal();
}

	