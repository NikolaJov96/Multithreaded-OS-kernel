// File: ksem.h

#ifndef KSEM_H_
#define KSEM_H_

#include "queue.h"
#include "semaphor.h"
#include "kobject.h"

/*!
 * class KernelSem
 * Class responsible for thread synchronization
 * (when accessing critical code segment)
 */
class KernelSem : public KernelObject {
	friend class Semaphore;		/*!< See semaphor.h */
	
public:
	KernelSem (int val) : value(val) {};

	int wait(Time time);	/*!< Signal that thread is entering critical code segment */
	void signal();			/*!< Signal that thread has completed critical code segment */

	int val() const;		/*!< Returns the current value of the semaphore  */

	virtual int iCall(PCB *pcb);	/*!< ? */

private:
	int value;			/*!< Semaphore value */
	Queue waitQueue;	/*!< Queue of threads waiting on this semaphore */

};


#endif

