// File: event.cpp

#include "event.h"

#include "kevent.h"

Event::Event(IVTNo ivtNo)
{
	myImpl = new KernelEv(ivtNo, this);
}

Event::~Event()
{
	delete myImpl;
}

void Event::wait()
{
	if (myImpl) myImpl->wait();
}

void Event::signal()
{
	if (myImpl) myImpl->signal();
}

