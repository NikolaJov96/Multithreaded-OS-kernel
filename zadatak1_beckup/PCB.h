// File: PCB.h

#ifndef PCB_H_
#define PCB_H_

#include "thread.h"
#include "kobject.h"

typedef enum {NEW, READY, BLOCKED, OVER} State;		/*!< Enum with diferent thread states */

class Queue;			/*!< Queue structure, see queue.h */
class KernelObject;		/*!< see kobject.h */

/*!
 * class PCB
 * Process Control Block structure, containing the state of the thread
 */
class PCB {
public:
	PCB(Thread *thread, StackSize stackSize, Time timeSlice);
	~PCB();
	
	// void Interrupt();
	void start();					/*!< Starts created thread (calls overrided run() method) */
	void sleep(Time SleepTime);		/*!< Pause thread for provided number of time quants */
	void waitToComplete();			/*!< Pauses calling thread until current is finished */
	
	void createStack();				/*!< Alocating statck for the thread, called when starting the thread */
	static void wrapper();			/*!< Method wrapping user thread run(), resposible for cleanup */
	
	static ID getRunningId();		/*!< Returns ID if the running thread */
	
	ID threadID;					/*!< Unique thread ID */
	Thread *thread;					/*!< Associated thread */
	StackSize PCBStackSize;			/*!< Size of the stack */
	
	Time PCBTimeSlice;				/*!< Max time thread has, before preemptive context switch */
	volatile Time timePassed;		/*!< Time that has passed since thread got CPU */
	
	Time timeToSleep;				/*!< Remaining nunmber of time queants to sleep */
	
	unsigned char *pcbSP;			/*!< Pointer to the thread's stack */
	unsigned int sp, ss;			/*!< ? */
	
	Queue *waitQueue;				/*!< Queue of threads waiting on this to complete */
	State state;					/*!< Currend thread state */
	
	KernelObject *waiting;			/*!< Semaphore or Event thread is currently waiting on */
	int signalled;					/*!< Flags that thread was awaken by signal (instead of timeot) */
	
	static Queue *identificationQueue;	/*!< Global queue of all of the user threads (all except main and idle thread) */
	static Queue *sleepQueue;			/*!< Global queue of the sleeping threads */
	
private:
	static ID nextID;				/*!< Static counter for generating new unique thread ID's */
	
};


#endif

