// File: kobject.h

#ifndef _KROBJECT_H_
#define _KROBJECT_H_

#include "PCB.h"

/*!
 * class KernelObject
 * Parrent class for KernelSem and KernelEvent
 */
class KernelObject
{
public:
	virtual int iCall(PCB *pcb) /*!< ? */
	{ return 0; }
};

#endif

