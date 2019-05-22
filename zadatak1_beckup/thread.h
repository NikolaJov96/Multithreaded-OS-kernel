// File: thread.h

#ifndef _thread_h_
#define _thread_h_

typedef unsigned long StackSize;			/*!< Type of the variables representing the stack size */
const StackSize defaultStackSize = 4096;	/*!< Default size of the stack (when not specified in constrictor) */

typedef unsigned int Time; 					/*!< Type of the variables representing time in units of minTimeSlice ms */
const Time defaultTimeSlice = 2; 			/*!< Default parameter of the thread constructor - time slice asigned to the thread */

typedef int ID;		/*!< Type of the variable representing unique thread ID */

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

