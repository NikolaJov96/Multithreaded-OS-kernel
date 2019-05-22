// File: PCB.h

#ifndef PCB_H_
#define PCB_H_

#include <stdlib.h>

#include "thread.h"
#include "waitable.h"

typedef enum {NEW, READY, BLOCKED, OVER} State;		/*!< Enum with diferent thread states */

class Queue;			/*!< Queue structure, see queue.h */
class Waitable;			/*!< see waitable.h */

struct SQueueNode 
{
	SignalId id;
	SQueueNode *next;
	SQueueNode(SignalId i) : id(i), next(NULL) {}
};

/*!
 * class PCB
 * Process Control Block structure, containing the state of the thread
 */
class PCB {
	friend class Kernel;		/*!< see kernel.h */
	
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
	static Thread *getThreadById(ID id);	/*!< Static method returning Thread* with provided ID */
	
	ID threadID;					/*!< Unique thread ID */
	Thread *thread;					/*!< Associated thread */
	ID parentID;					/*!< Parent thread ID - used to send signal 1 */
	StackSize PCBStackSize;			/*!< Size of the stack */
	
	Time PCBTimeSlice;				/*!< Max time thread has, before preemptive context switch */
	volatile Time timePassed;		/*!< Time that has passed since thread got CPU */
	
	volatile Time timeToSleep;		/*!< Remaining nunmber of time quants to sleep */
	
	unsigned char *pcbSP;			/*!< Pointer to the thread's stack */
	unsigned int sp, ss;			/*!< Thread's PC */
	
	Queue *waitQueue;				/*!< Queue of threads waiting on this to complete */
	volatile State state;			/*!< Currend thread state */
	
	volatile Waitable *waiting;	/*!< Semaphore (or Event - no need), thread is currently waiting on */
	volatile int signalled;			/*!< Flags that thread was awaken by signal (instead of timeot) */
	
	static Queue *identificationQueue;	/*!< Global queue of all of the user threads (all except main and idle thread) */
	static Queue *sleepQueue;			/*!< Global queue of the sleeping threads */
	
	// Signals
	void signal(SignalId signal);		/*!< Sends signal to the thread */
	
	void registerHandler(SignalId signal, SignalHandler handler);	/*!< Sets handler for the specific signal */
	SignalHandler getHandler(SignalId signal);		/*!< Returns handler of the desired signal */
	
	void maskSignal(SignalId signal);					/*!< Tells thread to ignore specific signal */
	static void maskSignalGlobally(SignalId signal);	/*!< Tells all threads to ignore specific signal */
	void unmaskSignal(SignalId signal);					/*!< Tells thread to respond to specific signal */
	static void unmaskSignalGlobally(SignalId signal);	/*!< Tells all threads to respond to specific signal (except where explicitly masked) */
	
	void blockSignal(SignalId signal);					/*!< Tells thread to remember received signal for later processing */
	static void blockSignalGlobally(SignalId signal);	/*!< Tells all threads to remember received signals for later processing */
	void unblockSignal(SignalId signal);				/*!< Tells thread to process received signal */
	static void unblockSignalGlobally(SignalId signal);	/*!< Tells all threads to process received signals (except where explicitly blocked) */
	
	SignalHandler signalHandlers[NUMBER_OF_SIGNALS];	/*!< Functions that should be called when signal is received */
	static void defaultHandler();			/*!< Default handler for 0 signal */
	
	volatile unsigned maskedSignals;				/*!< Signals masked on this PCB */
	volatile unsigned blockedSignals;				/*!< Signals blocked on this PCB */
	volatile static unsigned gMaskedSignals;		/*!< Signals masked globally */
	volatile static unsigned gBlockedSignals;		/*!< Signals blocked globally */
	static Queue *pausedQueue;			/*!< Queue of threads waiting on unmasked signal do wake up */
	
	static void pause();		/*!< Puts thread to sleep until it receives non - masked signal */
	
private:
	static ID nextID;			/*!< Static counter for generating new unique thread ID's */
	
	SQueueNode *firstS, *lastS;	/*!< First and last element od received signals queue */
	
};


#endif

