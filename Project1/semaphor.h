// File: semaphor.h

#ifndef _semaphor_h_
#define _semaphor_h_

typedef unsigned int Time;	/*!< Type of the variables Type of the variables representing time */

class KernelSem;	/*!< See ksem.h */

/*!
 * class Semaphore
 * Wrapper class for KernelSem
 */
class Semaphore 
{
public:
	Semaphore(int init = 1);	/*!< Constructor with provided initial semaphore value */
	virtual ~Semaphore();		/*!< Deletes KernelSem object */
	
	virtual int wait(Time maxTimeToWait);	/*!< KernelSem wait() wrapper */
	virtual void signal();					/*!< KernelSem signal()  wrapper */
	
	int val() const; 			/*!< KernelSem val() wrapper */

private:
	KernelSem* myImpl;	/*!< Wrapped KernelSem object */
	
};

#endif

