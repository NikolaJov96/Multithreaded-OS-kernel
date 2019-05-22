// File: prog02.cpp

#include "prog02.h"

#include <iostream.h>

#include "Semaphor.h"

Semaphore *outSem;

void InfiniteThread::run()
{
	//sleep(2);
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++);
	sem->signal();
	/*asm cli;
	cout << getId() << endl;
	cout.flush();
	asm sti;*/
	while(1)
	{
		for (int i = 0; i < 1000; i++)
			for (int j = 0; j < 1000; j++);
		//sleep(20);
		/*outSem->wait(0);
		cout << getId() << endl;
		cout.flush();
		outSem->signal();*/
		//dispatch();
	}
	
}

int userMain(int argc, char *argv[])
{
	InfiniteThread *it;
	Semaphore *sem = new Semaphore(0);
	outSem = new Semaphore();
	int co = 0;
	while (co < 30)
	{
		//co++;
		//it = new InfiniteThread(128, 0, sem); // 0 - only explicite dispatch
		it = new InfiniteThread(46, sem);
		//it = new InfiniteThread(sem);
		it->start();
		sem->wait(0);	// wait X x 55ms, if no signal recieved proceed
		outSem->wait(0);
		cout << "main1 " << it->getId() << endl;
		cout.flush();
		outSem->signal();
	}
	while(1)
	{
		outSem->wait(0);
		cout << "main2" << endl;
		cout.flush();
		outSem->signal();
		for (int i = 0; i < 10000; i++)
			for (int j = 0; j < 10000; j++);
	}
	
	delete sem;
	delete outSem;
	return 1;
}

void tick() {}

