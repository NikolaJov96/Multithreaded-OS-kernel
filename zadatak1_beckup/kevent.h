// File: kevent.h

#ifndef KEVENT_H_
#define KEVENT_H_

#include "kobject.h"
#include "queue.h"
#include "PCB.h"
#include "kernel.h"
#include "event.h"

typedef unsigned char IVTNo;

/*!
 * class KernelEv
 * Class used for pausing the thread until
 * certain event is triggered
 */
class KernelEv : public KernelObject
{
public:
	KernelEv(IVTNo ivtNo, Event *event);
	virtual ~KernelEv();
	
	void wait();		/*!< Pauses thread */
	void signal();		/*!< Wakes up waiting thread */
	
	virtual int iCall(PCB *pcb);
	
private:
	PCB *parentThread;	/*!< Thread that created event (only it can wait on it) */
	IVTNo entry;		/*!< Associated IVT entry ID */
	int waiting;		/*!< Currently waiting for the event */

};

#endif

