// File: ithread.cpp

#include "ithread.h"
#include "pcb.h"
#include "kernel.h"

void IdleThread::start(){
	Kernel::PBDepth++;		// Mind closing
	
	myPCB->state = READY;
	myPCB->createStack();
	
	Kernel::PBDepth--;
}

void IdleThread::run(){
	while(1);
}

