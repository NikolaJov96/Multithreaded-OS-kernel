// File: semaphor.cpp

#include "semaphor.h"
#include "ksem.h"

Semaphore::Semaphore(int init)
{
	myImpl = new KernelSem(init);				// Create new KernelSem object
}

Semaphore::~Semaphore()
{
	delete myImpl;
}
	
int Semaphore::wait(Time maxTimeToWait)
{
	int ret = myImpl->wait(maxTimeToWait);		// Call wrapped wait() method
	return ret;
}

void Semaphore::signal()
{
	myImpl->signal();							// Call wrapped signal() method
}
	
int Semaphore::val() const
{
	return myImpl->val();
}

