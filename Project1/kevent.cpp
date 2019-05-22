// File: kevent.cpp

#include "kevent.h"
#include "event.h"

#include "schedule.h"

KernelEv::KernelEv(IVTNo ivtNo, Event *event) :
	entry(ivtNo)
{
	Kernel::PBDepth++;		// Mind closing
	
	parentThread = Kernel::running;
	waiting = 0;
	boundedEvents[entry] = event;
	
	Kernel::PBDepth--;
}

KernelEv::~KernelEv()
{
	Kernel::PBDepth++;		// Mind closing
	boundedEvents[entry] = NULL;
	Kernel::PBDepth--;
}

void KernelEv::wait()
{
	asm cli;
	
	if (Kernel::running != parentThread) 
	{
		Kernel::PBDepth--;
		return;
	}
	
	Kernel::running->state = BLOCKED;
	Kernel::running->waiting = this;
	waiting = 1;

	asm sti;
	
	Kernel::dispatch();
}

void KernelEv::signal()		// called by interrupt routine
{
	asm cli;
	//Kernel::PBDepth++;		// Mind closing
	
	if (waiting) 
	{
		parentThread->state = READY;
		parentThread->waiting = NULL;
		waiting = 0;
		Kernel::enqueue(parentThread);
	}
	Kernel::dispatch();
	
	//Kernel::PBDepth--;
	asm sti;
}

void setupRoutine(int id, InterruptRoutine routine, IVTEntry *entry)
{
	Kernel::entries[id] = entry;
	Kernel::entries[id]->oldRoutine = Kernel::initializeInterrupt(id, routine);
}

void IVTEntry::handle(int id, int oldRoutine)
{
	if (oldRoutine) Kernel::entries[id]->oldRoutine();
	if (boundedEvents[id]) boundedEvents[id]->signal();
}

	