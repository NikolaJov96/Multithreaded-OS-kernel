// File: schedule.h

#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

// Required memory model: huge

class PCB;	/*!< See PCB.h */

/*!
 * class Scheduler
 * Provided code used to pick next running thread
 * all new threads are added to the Scheduler pool,
 * except for idle thread, which is picked when no 
 * other thread is ready
 */
class Scheduler 
{
public:
	static void put(PCB*);	/*!< Add PCB (thread) to Scheduler pool */
	static PCB* get();		/*!< Get some thread from Scheduler pool */
	
};

#endif

