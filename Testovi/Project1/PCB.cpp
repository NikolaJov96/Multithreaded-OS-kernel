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
volatile unsigned PCB::gMaskedSignals = 0;
volatile unsigned PCB::gBlockedSignals = 0;
Queue *PCB::pausedQueue = new Queue();

PCB::PCB(Thread* thread, StackSize stackSize, Time timeSlice)
{
	Kernel::PBDepth++;		// Mind closing

	threadID = nextID++;			// Get next unique ID
	this->thread = thread;
	parentID = Kernel::running->threadID;
	PCBStackSize = stackSize;
	PCBTimeSlice = timeSlice;		// Max time thread has before preemptive context switch
	
	timePassed = 0;
	timeToSleep = 0;
	pcbSP = NULL;
	waitQueue = new Queue();
	state = NEW;
	waiting = NULL;
	signalled = 0;
	
	for (int i = 1; i < NUMBER_OF_SIGNALS; i++) 
		if (Kernel::running)
			signalHandlers[i] = Kernel::running->signalHandlers[i];
		else signalHandlers[i] = NULL;
	signalHandlers[0] = defaultHandler;
	
	if (Kernel::running) maskedSignals = Kernel::running->maskedSignals;
	else maskedSignals = 0;
	if (Kernel::running) blockedSignals = Kernel::running->blockedSignals;
	else blockedSignals = 0;
	firstS = lastS = NULL;

	if (threadID > 0) identificationQueue->add(this);	// Add user defined thread to identification queue

	Kernel::PBDepth--;
}

PCB::~PCB() 
{
	Kernel::PBDepth++;		// Mind closing
	
	if (pcbSP) delete pcbSP;
	if (waitQueue) delete waitQueue;
	while (firstS) 
	{
		SQueueNode *del = firstS;
		firstS = firstS->next;
		delete del;
	}
	
	Kernel::PBDepth--;
}

void PCB::start()
{
	asm cli;
	Kernel::PBDepth++;		// Mind closing
	
	if (state == NEW)
	{
		state = READY;			// Add PCB to the queue of ready threads
		createStack();			// Create stack
		Kernel::enqueue(this);	// Add PCB to Scheduler pool
	}
	
	Kernel::PBDepth--;
	asm sti;
}

void PCB::sleep(Time SleepTime)
{
	Kernel::PBDepth++;		// Mind closing
	
	asm cli;				// Guard sleepQueue from timer interference
	sleepQueue->add(this);
	timeToSleep = SleepTime;
	state = BLOCKED;
	asm sti;
	
	Kernel::dispatch();
	
	Kernel::PBDepth--;		// Dispatch needs to be called on the same preemption block layer
}

void PCB::waitToComplete()
{
	Kernel::PBDepth++;		// Mind closing
	
	// Don't wait for
	if (state == OVER ||			// finished thread
		state == NEW ||				// not started thread
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
	// asm cli; // set outside
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
	// asm sti;		// set outside
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
		Kernel::enqueue(pcb);
	}
	
	// Using getter because thread might not exist
	Thread *thr = Thread::getThreadById(Kernel::running->parentID);
	if (thr) thr->signal(1);		// Signal to parent that child has finihed 
	if (!((Kernel::running->blockedSignals) & (1 << 2)) &&		// handling signal 2
		!((Kernel::running->gBlockedSignals) & (1 << 2)) &&
		!((Kernel::running->maskedSignals) & (1 << 2)) &&	
		!((Kernel::running->gMaskedSignals) & (1 << 2)) &&
		Kernel::running->signalHandlers[2]) 
		Kernel::running->signalHandlers[2]();
	
	Kernel::dispatch();		// change context to another thread
	
	Kernel::PBDepth--;
}

ID PCB::getRunningId()
{
	return Kernel::running->threadID;
}

Thread *PCB::getThreadById(ID id)
{
	if (id == -1) return Kernel::starting->thread;
	if (id == 0) return Kernel::idle->thread;
	return identificationQueue->findThreadWithID(id);
}

void PCB::signal(SignalId signal)
{
	Kernel::PBDepth++;		// Mind closing
	
	// Ignore invalid and masked signals
	if (signal < NUMBER_OF_SIGNALS && 
		!(maskedSignals & (1 << signal)) &&
		!(gMaskedSignals & (1 << signal)))
	{
		if (firstS) lastS = lastS->next = new SQueueNode(signal);	// Record signal
		else firstS = lastS = new SQueueNode(signal);
		
		if (state == BLOCKED && waiting == Kernel::signalMonitor)		// Wake thread up
		{
			state = READY;
			waiting = NULL;
			Kernel::enqueue(this);
		}
	}
	
	Kernel::PBDepth--;
}
	
void PCB::registerHandler(SignalId signal, SignalHandler handler)
{
	Kernel::PBDepth++;		// Mind closing
	if (signal < NUMBER_OF_SIGNALS)
		signalHandlers[signal] = handler;
	Kernel::PBDepth--;
}

SignalHandler PCB::getHandler(SignalId signal)
{
	Kernel::PBDepth++;		// Mind closing
	SignalHandler handler = NULL;
	if (signal < NUMBER_OF_SIGNALS)
		signalHandlers[signal] = handler;
	Kernel::PBDepth--;
	return handler;
}

void PCB::maskSignal(SignalId signal)
{
	Kernel::PBDepth++;		// Mind closing
	if (signal < NUMBER_OF_SIGNALS)
		maskedSignals |= (unsigned)(1u << signal);
	Kernel::PBDepth--;
}
void PCB::maskSignalGlobally(SignalId signal)
{
	Kernel::PBDepth++;		// Mind closing
	if (signal < NUMBER_OF_SIGNALS)
		gMaskedSignals |= 1 << signal;
	Kernel::PBDepth--;
}

void PCB::unmaskSignal(SignalId signal)
{
	Kernel::PBDepth++;		// Mind closing
	if (signal < NUMBER_OF_SIGNALS)
		maskedSignals &= ~(1 << signal);
	Kernel::PBDepth--;
}
void PCB::unmaskSignalGlobally(SignalId signal)
{
	Kernel::PBDepth++;		// Mind closing
	if (signal < NUMBER_OF_SIGNALS)
		gMaskedSignals &= ~(1 << signal);
	Kernel::PBDepth--;
}

void PCB::blockSignal(SignalId signal)
{
	Kernel::PBDepth++;		// Mind closing
	if (signal < NUMBER_OF_SIGNALS)
		blockedSignals |= 1 << signal;
	Kernel::PBDepth--;
}
void PCB::blockSignalGlobally(SignalId signal)
{
	Kernel::PBDepth++;		// Mind closing
	if (signal < NUMBER_OF_SIGNALS)
		gBlockedSignals |= 1 << signal;
	Kernel::PBDepth--;
}

void PCB::unblockSignal(SignalId signal)
{
	Kernel::PBDepth++;		// Mind closing
	if (signal < NUMBER_OF_SIGNALS)
		blockedSignals &= ~(1 << signal);
	Kernel::PBDepth--;
}
void PCB::unblockSignalGlobally(SignalId signal)
{
	Kernel::PBDepth++;		// Mind closing
	if (signal < NUMBER_OF_SIGNALS)
		gBlockedSignals &= ~(1 << signal);
	Kernel::PBDepth--;
}

void PCB::defaultHandler()
{
	Kernel::PBDepth++;		// Mind closing
	
	PCB::identificationQueue->removePCB(Kernel::running);	// Remove from user threads
	Kernel::running->state = OVER;							// Set state to OVER
	
	// Continue execution of threads waiting on the current to complete
	while (!Kernel::running->waitQueue->isEmpty())
	{
		PCB *pcb = Kernel::running->waitQueue->get();
		pcb->state = READY;
		Kernel::enqueue(pcb);
	}
	
	// Using getter because parent thread might not exist
	Thread *thr = Thread::getThreadById(Kernel::running->parentID);
	if (thr) thr->signal(1);		// Signal to parent that child has finihed 
	
	Kernel::dispatch();		// change context to another thread
	
	Kernel::PBDepth--;
}

void PCB::pause()
{
	Kernel::PBDepth++;		// Mind closing
	
	Kernel::running->state = BLOCKED;
	Kernel::running->waiting = Kernel::signalMonitor;
	pausedQueue->add(Kernel::running);
	
	Kernel::dispatch();
	
	Kernel::PBDepth--;
}

