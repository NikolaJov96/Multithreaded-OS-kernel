// File: ksem.cpp

#include "ksem.h"

#include "kernel.h"
#include "schedule.h"

int KernelSem::wait(Time time)
{
	asm cli;				// Needed to avoid timer's interfeering with sleepQueue
	//Kernel::PBDepth++;		// Mind closing
	
	Kernel::running->signalled = 0;
	if (--value < 0)	// Other thread is already in the critical segment, so block current thread
	{
		Kernel::running->state = BLOCKED;
		Kernel::running->waiting = this;		// Blocked thread remembers on which semaphore it is waiting
		waitQueue.add(Kernel::running);			// Add thread to this semaphore's waiting queue
		
		if (time > 0)	// If time to wait is specified (time > 0), add thread to sleeping queue
		{
			Kernel::running->timeToSleep = time;	// Timeout after specified time
			PCB::sleepQueue->add(Kernel::running);
		}
		
		//Kernel::PBDepth--;
		asm sti;				// Exiting safe block
		Kernel::dispatch();						// Current thread is blocked, so find another to execute
	} 
	else
	{
		asm sti;				// Exiting safe block
		//Kernel::PBDepth--;		// Return to previous preemption block depth
		return 1;				// Return 1 if thread did not wait at all
	}
	
	return Kernel::running->signalled;
}

void KernelSem::signal() 
{
	asm cli;				// Needed to avoid timer's interfeering with sleepQueue
	//Kernel::PBDepth++;		// Mind closing
	
	if (++value <= 0) // Another thread can now enter critical segment, if any is waiting
	{
		PCB* pcb = waitQueue.get();		// Remove from semaphore queue
		pcb->state = READY;
		pcb->waiting = NULL;
		pcb->signalled = 1;
		
		pcb->timeToSleep = 0; 				// Remove from sleeping queue
		PCB::sleepQueue->removePCB(pcb);	// Thread does not have to exist in the sleeping queue

		Kernel::enqueue(pcb);
	}
	
	//Kernel::PBDepth--;
	asm sti;				// Exiting safe block
}

int KernelSem::val() const 
{
	return value;
}

void KernelSem::remove(PCB *pcb)
{
	Kernel::PBDepth++;		// Mind closing
	
	pcb->waiting = NULL;
	if (waitQueue.removePCB(pcb)) value++;
	
	Kernel::PBDepth--;
}

