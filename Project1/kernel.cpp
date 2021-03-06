// File: kernel.cpp

#include "kernel.h"

#include <stdio.h>
#include <dos.h>

#include "schedule.h"
#include "queue.h"

extern void tick();
void interrupt (*Kernel::oldRoutine)() = NULL;

PCB *Kernel::starting = NULL;
PCB *Kernel::idle = NULL;
PCB *Kernel::running = NULL;
Waitable *Kernel::signalMonitor = new Waitable();
volatile Action Kernel::action = CLOCK;
volatile int Kernel::PBDepth = 0;
IVTEntry *Kernel::entries[256] = { NULL };
Semaphore *Kernel::outSem = new Semaphore();

void Kernel::initialize() {
	Kernel::PBDepth++;		// Mind closing
	
	starting = new Thread(initialSS, defaultTimeSlice)->myPCB;	// Initialize kernel thread (which executes userMain - user program)
																// it already started, so no need to start it explicitly
	starting->state = READY;
	running = starting;				// Set it as starting (main) thread

	idle = (new IdleThread())->myPCB;	// Init idle thread, used to be assigned to the CPU when none of the other threads are ready

	idle->start();		// Same as if idle was thread itself
	
	oldRoutine = initializeInterrupt(0X08, timer);				// Set Kernel::timer() as default interrupt routine
	
	Kernel::PBDepth--;
}

void Kernel::terminate()
{
	Kernel::PBDepth++;		// Mind closing
	
	if (running != starting) return;	// Make sure terminate is called by the main thread

	delete PCB::identificationQueue;			// Wait to complete and free all user threads
	delete PCB::sleepQueue;						// Clear sleepQueue (should be empty)
	delete PCB::pausedQueue;					// Clear pausedQueue (should be empty)
	
	for (int i = 0; i < 256; i++)				// Retrive old routines changed by PREPAREENTRY
	{
		if (entries[i]) 
		{
			initializeInterrupt(i, entries[i]->oldRoutine);
			delete entries[i];
		}
	}

	initializeInterrupt(0X08, oldRoutine);		// Retrive old default tmer interrupt routine

	delete idle;								// Free idle and main thread
	delete starting;
	delete signalMonitor;
	
	delete outSem;
	
	Kernel::PBDepth--;
}

void Kernel::dispatch()
{
	Kernel::PBDepth++;		// Mind closing
	
	action = EXPLICIT;				// Flag that timer() is called by explicit dispatch
	int depth = Kernel::PBDepth;
	Kernel::PBDepth = 0;
	timer();						// Explicit timer call (condext switch)
	Kernel::PBDepth = depth;
	action = CLOCK;					// Should be done by timer()
	
	Kernel::PBDepth--;
}

void Kernel::enqueue(PCB *pcb)
{
	if (pcb) Scheduler::put(pcb);
}

InterruptRoutine Kernel::initializeInterrupt(
	IVTNoInt ivtNo, InterruptRoutine newRoutine)
{
	asm cli;

	unsigned int tempseg = FP_SEG(newRoutine);
	unsigned int tempoff = FP_OFF(newRoutine);

	unsigned int oldseg, oldoff;

	ivtNo *= 4;

	asm {

		push es
		push ax
		push bx

		mov ax, 0
		mov es, ax
		mov bx, word ptr ivtNo

		mov ax, word ptr es:bx+2
		mov word ptr oldseg, ax
		mov ax, word ptr es:bx
		mov word ptr oldoff, ax

		mov ax, word ptr tempseg
		mov word ptr es:bx+2, ax
		mov ax, word ptr tempoff
		mov word ptr es:bx, ax

		pop bx
		pop ax
		pop es
	}

	InterruptRoutine old = (InterruptRoutine) MK_FP(oldseg, oldoff);
	
	asm sti;
	return old;
}

void interrupt Kernel::timer()
{
	static volatile unsigned int tempSP, tempSS;
	static PCB *newThread;

	PBDepth++;		// Mind closings
	
	if (action == CLOCK) 	// if dispatch is not called explicitly, call default interrupt routine
	{
		tick();		// User specified function used to make user application aware of time
		asm cli;	// Old interrupt routine expects interrupts to be disabled
		(*oldRoutine)();
		asm sti;
	}

	// If timer is called by CPU timer interrupt, update sleeping threads
	if ((action != EXPLICIT) && !(PCB::sleepQueue->isEmpty()))
		PCB::sleepQueue->update();
	
	// Only if dispatch is not explicitly called and thread has limited time slice, 
	// incrise time passed since thread got CPU for one minimal time slice
	if (action != EXPLICIT && running->PCBTimeSlice != 0)
		running->timePassed++;
	
	// If PBDepth is more than 1 (it was just increased by 1), 
	// kernel routine is running, so it can't be interrupted
	if (PBDepth > 1) 
	{
		PBDepth--;
		return;
	}
	
	// Only if dispatch is not explicitly called and thread still has CPU time left,
	// return (do not change context)
	if ((action != EXPLICIT) && 
		(running->timePassed < running->PCBTimeSlice || running->PCBTimeSlice == 0))
	{
		PBDepth--;
		return;
	}
	
	asm cli;
	
	// Reset flag, set when dispatch was explicitly called
	if (action == EXPLICIT) action = CLOCK;

	//ukoliko je nit spremna a nije idle ili starting vracamo je u red spremnih
	// If thread is READY (not BLOCKED) and it is not idle, return it to the Scheduler pool
	if (running->state == READY && running != idle)	enqueue(running);
	
	while (1)
	{
		newThread = Scheduler::get();
		if (newThread == NULL) newThread = idle;	// Scheduler pool is empty
		if (newThread->state == READY) break;
		// If pooled thread is not READY (sleeping), leave it out of Scheduler and get next
	}
	
	// Context switch:
	
	asm {
		mov tempSP, sp		// Beckup current SP
		mov tempSS, ss
	}

	running->sp = tempSP;	// Apply new SP
	running->ss = tempSS;

	running = newThread;


	tempSP = running->sp;	// Prepare backed up SP for storing
	tempSS = running->ss;

	asm {
		mov sp, tempSP		// Store old SP
		mov ss, tempSS
	}
	
	running->timePassed = 0;	// Reset time passed since (new) thread got CPU
	
	asm sti;
	
	remarkSignals();	// Process signals selected thread has received
	
	PBDepth--;
}

void Kernel::remarkSignals()
{
	PCB *pcb = Kernel::running;
	SQueueNode *prev = NULL, *curr = pcb->firstS;
	while (curr)	// No invalid or masked signals exist in the queue
	{
		if (pcb->blockedSignals & (1 << curr->id) ||
			PCB::gBlockedSignals & (1 << curr->id)) // if curr->id (signal) is blocked - skip
		{
			prev = curr;
			curr = curr->next;
			continue;
		}
		
		if (pcb->signalHandlers[curr->id]) pcb->signalHandlers[curr->id]();
		
		if (pcb->lastS == curr) pcb->lastS = prev;
		if (prev) 
		{
			prev->next = curr->next;
			delete curr;
			curr = prev->next;
		}
		else 
		{
			pcb->firstS = curr->next;
			delete curr;
			curr = pcb->firstS;
		}
		
	}
}

