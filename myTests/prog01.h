// File: program.h

#ifndef PROG01_H_
#define PROG01_H_

#include "thread.h"

/*!
 * class ThreadExample
 * First example thread
 */
class ThreadExample : public Thread 
{
public:
	ThreadExample(int n);
	
	int getNum();
	
protected:
	virtual void run ();
	
private:
	int num;
}

#endif

