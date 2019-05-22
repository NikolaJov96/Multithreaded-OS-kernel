// File: prog04.cpp

#include "prog04.h"

#include <iostream.h>

Semaphore *outSem;
SignalThread *t1, *t2, *t3;

int SignalThread::flag = 0;

void handler1()
{
	outSem->wait(0);
	cout << "Handler1" << endl;
	cout.flush();
	outSem->signal();
}

void handler2()
{
	outSem->wait(0);
	cout << "Handler2" << endl;
	cout.flush();
	outSem->signal();
}

void handler5()
{
	outSem->wait(0);
	cout << "Handler5" << endl;
	cout.flush();
	outSem->signal();
}

void SignalThread::run()
{
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++);
	sem->signal();
	switch (flag++)
	{
	case 0:
		registerHandler(1, handler1);
		
		int sigOption = 0;	// configure
		SignalId sid = 5;	// configure
		switch (sigOption)
		{
		case 1: maskSignal(sid); break;
		case 2: maskSignalGlobally(sid); break;
		case 3: blockSignal(sid); break;
		case 4: blockSignalGlobally(sid); break;
		}
	
		t2 = new SignalThread(sem);
		t2->start();
		sem->wait(0);
		
		registerHandler(2, handler2);
		 
		delete t2;
		outSem->wait(0);
		cout << "T2 deleted" << endl;
		cout.flush();
		outSem->signal();
			
		switch(sigOption)
		{
		case 1: unmaskSignal(sid); break;
		case 2: unmaskSignalGlobally(sid); break;
		case 3: unblockSignal(sid); break;
		case 4: unblockSignalGlobally(sid); break; 
		}
		
		break;
	case 1:
		t3 = new SignalThread(sem);
		registerHandler(5, handler5);
		t3->start();
		sem->wait(0);
		maskSignal(1);	// Don't wake up by chield finished signal
		blockSignal(5);
		pause();
		delete t3;
		outSem->wait(0);
		cout << "T3 deleted" << endl;
		cout.flush();
		outSem->signal();
		unblockSignal(5);
		dispatch();		// needet to remark signal 5
		break;
	case 2:
		for (int i = 3; i > 0; i--)
		{
			outSem->wait(0);
			cout << i << endl;
			cout.flush();
			outSem->signal();
			sleep(1000/55);
		}
		t2->signal(5);
		break;
	}
}

int userMain(int argc, char *argv[])
{
	Semaphore *sem = new Semaphore(0);
	outSem = new Semaphore();
	t1 = new SignalThread(sem);
	t1->start();
	sem->wait(0);
	
	delete t1;
	outSem->wait(0);
	cout << "T1	deleted" << endl;
	cout.flush();
	outSem->signal();
	delete sem;
	delete outSem;
	return 1;
}

void tick() {}

