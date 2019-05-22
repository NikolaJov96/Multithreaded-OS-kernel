// File: PCB.cpp

#include "PCB.h"

#include <dos.h>
#include <stdlib.h>

#include "kernel.h"
#include "schedule.h"
#include "queue.h"

int PCB::nextID = -1;
// Initial (main) thread ID: -1
// Idle thread ID: 0

Queue *PCB::identificationQueue = new Queue();
Queue *PCB::sleepQueue = new Queue();

PCB::PCB(Thread* thread, StackSize stackSize, Time timeSlice)
{
	Kernel::PBDepth++;		// Mind closing

	threadID = nextID++;			// Get next unique ID
	this->thread = thread;
	PCBStackSize = stackSize;
	PCBTimeSlice = timeSlice;		// Max time thread has before preemptive context switch
	
	timePassed = 0;
	timeToSleep = 0;
	pcbSP = NULL;
	waitQueue = new Queue();
	state = NEW;
	waiting = NULL;
	signalled = 0;

	if (threadID > 0) identificationQueue->add(this);	// Add user defined thread to identification queue

	Kernel::PBDepth--;
}

PCB::~PCB() 
{
	Kernel::PBDepth++;		// Mind closing
	
	if (pcbSP) delete pcbSP;
	
	Kernel::PBDepth--;
}

/*void PCB::Interrupt()
{
	asm cli;
	if (state == BLOCKED){
		if(waiting->iCall(this)){
			BlockingFlag = 1;
			state = READY;
			Scheduler::add(this);
		}
	}
	asm sti;
}*/

void PCB::start()
{
	Kernel::PBDepth++;		// Mind closing
	
	if (state == NEW)
	{
		state = READY;			// Add PCB to the queue of ready threads
		createStack();			// Create stack
		Scheduler::put(this);	// Add PCB to Scheduler pool
	}
	
	Kernel::PBDepth--;
}

void PCB::sleep(Time SleepTime)
{
	Kernel::PBDepth++;		// Mind closing
	
	sleepQueue->add(this);
	timeToSleep = SleepTime;
	state = BLOCKED;
	
	Kernel::dispatch();
	
	Kernel::PBDepth--;		// Dispatch needs to be called on the same preemption block layer
}

void PCB::waitToComplete()
{
	Kernel::PBDepth++;		// Mind closing
	
	// Don't wait for
	if (state == OVER ||			// finished thread
		this == Kernel::running ||	// yourself
		this == Kernel::idle ||		// idle thread
		this == Kernel::starting)	// main thread
	{
		Kernel::PBDepth--;
		return;
	}

	Kernel::running->state = BLOCKED;			// Set current thread state to BLOCKED
	waitQueue->add(Kernel::running);			// Add caller thread (currently running) to the list of threads waiting for me

	Kernel::dispatch();							// Current thread is on the waiting list, so get another thread to execute
	
	Kernel::PBDepth--;
}

void PCB::createStack()
{
	Kernel::PBDepth++;		// Mind closing

	static volatile unsigned newSP, newSS, newIP, newCS, oldSP, oldSS, tax;

	pcbSP = new unsigned char[PCBStackSize];	// Allocate memory for the stack

	newSP = sp = FP_OFF(pcbSP + PCBStackSize);	// Get stack address offset
	newSS = ss = FP_SEG(pcbSP + PCBStackSize);	// Get stack address segment

	newIP = FP_OFF(&wrapper);		// Running function is actualy kernel wrapper of the user provided
	newCS = FP_SEG(&wrapper);		// run() method, so Program Counter is set to point on it

	asm {
		
		mov tax, ax
		mov oldSS, ss
		mov oldSP, sp

		mov ss, newSS
		mov sp, newSP

		pushf					//postavljamo PSW na stek
		pop ax					//skidamo PSW sa steka
		or ax, 1000000000b		//setujemo bit I reci PSW kako bi omogucili prekide
		push ax					//vracamo PSW na stack

		mov ax, newCS			//stavljamo CS na stek
		push ax

		mov ax, newIP			//stavljamo IP na stek
		push ax

		//stavljamo na stek ostale programski dostupne registre
		mov ax, 0
		push ax
		push bx
		push cx
		push dx
		push es
		push ds
		push si
		push di
		push bp

		mov newSP, sp
		mov sp, oldSP
		mov ss, oldSS
	}

	this->sp = newSP;			//azuriramo SP

	Kernel::PBDepth--;
}

void PCB::wrapper()
{
	Kernel::running->thread->run();		// Run thread
	// When run function finishes, notify sistem that thread has finished
	
	Kernel::PBDepth++;		// Mind closing
	
	PCB::identificationQueue->removePCB(Kernel::running);	// Remove from user threads
	Kernel::running->state = OVER;							// Set state to OVER
	
	// Continue execution of threads waiting on the current to complete
	while (!Kernel::running->waitQueue->isEmpty())
	{
		PCB *pcb = Kernel::running->waitQueue->get();
		pcb->state = READY;
		Scheduler::put(pcb);
	}

	Kernel::dispatch();		// change context to another thread
	
	Kernel::PBDepth--;
}

ID PCB::getRunningId()
{
	return Kernel::running->threadID;
}

