// File: thread.h

#ifndef _thread_h_
#define _thread_h_

typedef unsigned long StackSize;			/*!< Type of the variables representing the stack size */
const StackSize defaultStackSize = 4096;	/*!< Default size of the stack (when not specified in constrictor) */

typedef unsigned int Time; 					/*!< Type of the variables representing time in units of minTimeSlice ms */
const Time defaultTimeSlice = 2; 			/*!< Default parameter of the thread constructor - time slice asigned to the thread */

typedef int ID;		/*!< Type of the variable representing unique thread ID */

typedef void (*SignalHandler)();	/*!< Type of the signal handler function */
typedef unsigned SignalId;			/*!< Type of signal ID variables */
const int NUMBER_OF_SIGNALS = 16;	/*!< Number of different signals from 0 to NUMBER_OF_SIGNALS - 1 */

class PCB;		/*!< Process Control Block structure forward declaradion, see PCB.h */

/*!
 * class Thread
 * Class representing single runnable hread in the OS
 */
class Thread 
{	
public:
	void start();					/*!< Starts created thread (calls overrided run() method) */
	void sleep(Time timeToSleep);	/*!< Suspends thread for desired time */
	void waitToComplete();			/*!< Pauses calling thread until current is finished */
	virtual ~Thread();				/*!< Wait to complete and free used memory */
	
	ID getId();				/*!< Returns ID of the thread */
	
	static ID getRunningId();				/*!< Static method returning ID of the running thread */
	static Thread *getThreadById(ID id);	/*!< Static method returning Thread* with provided ID */
	
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
	
	static void pause();		/*!< Puts thread to sleep until it receives non - masked signal */

protected:
	friend class PCB;			/*!< See PCB.h */
	friend class IdleThread;	/*!< See ithread.h */ 
	friend class Kernel;		/*!< See kernel.h */
	
	Thread(StackSize stackSize = defaultStackSize, 
		Time timeSlice = defaultTimeSlice);	/*!< Constructor with provided default parameters */
	virtual void run() {}	/*!< Function to be overrided by programmer */

private:
	PCB* myPCB;		/*!< PCB structure of the thread, see PCB.h */
	
};

void dispatch();	/*!< Wrapper for Kernel::dispatch(), see kernel.h */

#endif

