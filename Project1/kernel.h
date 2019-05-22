// File: kernel.h

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdlib.h>

#include "PCB.h"
#include "thread.h"
#include "ithread.h"
#include "event.h"
#include "semaphor.h"

typedef enum {CLOCK, EXPLICIT} Action; 	/*!< Enum with diferent ways context switch can be initialized */

typedef void (interrupt *InterruptRoutine)();	/*!< Type representing interrupt routine functions */
typedef unsigned int IVTNoInt;		/*!< Type of the variable containing index of desired interrupt routine in IVT */

static Event *boundedEvents[256] = { NULL }; 

const StackSize initialSS = 128;

/*!
 * class Kernel
 * Class containing initializing and terminating
 * methods and current OS state
 */
class Kernel 
{
	friend class PCB;			/*!< See PCB.h */
	friend class IdleThread;	/*!< See ithread.h */
	friend class KernelSem;		/*!< See ksem.h */
	friend class KernelEv;		/*!< See kevent.h */
	
public:
	static void initialize();	/*!< Static method initializing new interrupt routine */
	static void terminate();	/*!< Static method returning old interrupt routine */
	static void dispatch();		/*!< Explicit call of interrupt function for context change */
	static void enqueue(PCB *pcb);	/*!< Calls Scheduler::put(pcb) */
	static InterruptRoutine initializeInterrupt(
		IVTNoInt ivtNo, InterruptRoutine newRoutine);	/*!< Sets provided function as interrupt routine in IVT on index n */
	static IVTEntry *entries[256];		/*!< IVTEntry objects */
	static Semaphore *outSem;
	
private:
	static void interrupt timer();				/*!< Method that is set as new interrupt routine */
	static InterruptRoutine oldRoutine;			/*!< Method calling old interrupt routine ? */
	static void remarkSignals();				/*!< Calls handler functions for received signals */

	static PCB *starting;	/*!< Main user application thread */
	static PCB *idle;		/*!< Idle thread, ready to take CPU if no other thread is "READY" */
	static PCB *running;	/*!< Currently running thread */
	static Waitable *signalMonitor;	/*!< Object ond which all paused threads wait */
	static volatile Action action;	/*!< Determins if timer is called by interrupt or explicitly */
	static volatile int PBDepth;	/*!< Preemption block depth, increased when entering kernel code */
	
};

#endif

