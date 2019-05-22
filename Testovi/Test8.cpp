#include "kernel.h"
#include <iostream.h>
#include <stdlib.h>

/*
 	 Test: Semafori bez realizacije spavanja
*/

#include <stdio.h>
#include "semaphor.h"
Semaphore *outSem = new Semaphore();

const int n = 5;
int count = 10;

Semaphore s(2);

class TestThread : public Thread
{
public:

	TestThread(): Thread(){}
	~TestThread()
	{
		waitToComplete();
	}
protected:

	void run();

};

void TestThread::run()
{
	s.wait(0);
	
	outSem->wait(0);
	cout<<"Thread "<<getId()<<" in critical section."<<endl;
	outSem->signal();
	
	for(unsigned int i=0;i<64000;i++)
		for(unsigned int j=0;j<64000;j++);
	s.signal();
}

void tick(){}

int userMain(int argc, char** argv)
{
	outSem->wait(0);
	printf("Test starts.\n");
	outSem->signal();
			
	TestThread t[n];
	int i;
	for(i=0;i<n;i++)
	{
		t[i].start();
	}
	for(i=0;i<n;i++)
	{
		t[i].waitToComplete();
	}
	
	outSem->wait(0);
	printf("Test ends.\n");
	outSem->signal();
	return 0;
}


