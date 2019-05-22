// File: prog03.cpp

#include "prog03.h"

#include <iostream.h>

#include "Semaphor.h"

Semaphore *outSem03;
PREPAREENTRY(9,1);

void EventThread::run()
{
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++);
	sem->signal();
	Event *ev = new Event(9);
	for (i = 0; i < 3; i++)
	{
		outSem03->wait(0);
		cout << "Press key" << endl;
		cout.flush();
		outSem03->signal();
		ev->wait();
	}
	delete ev;
}

int userMain(int argc, char *argv[])
{
	EventThread *it;
	Semaphore *sem = new Semaphore(0);
	outSem03 = new Semaphore();
	//it = new EventThread(128, 0, sem); // 0 - only explicite dispatch
	//it = new EventThread(46, sem);
	it = new EventThread(sem);
	it->start();
	sem->wait(0);	// wait X x 55ms, if no signal recieved proceed
	outSem03->wait(0);
	cout << "main1 " << it->getId() << endl;
	cout.flush();
	outSem03->signal();
	
	//it->waitToComplete();
	
	delete it;
	delete sem;
	delete outSem03;
	return 1;
}

void tick() {}

