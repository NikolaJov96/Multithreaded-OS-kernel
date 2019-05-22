#include "kernel.h"
#include <iostream.h>
#include <stdlib.h>

/*
 	 Test: Semafori sa spavanjem
*/

#include <stdio.h>
#include "semaphor.h"
Semaphore *outSem = new Semaphore();

const int n = 1;
int t=-1;

Semaphore s(0);

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
	outSem->wait(0);
	printf("Thread waits for 10 units of time...\n");
	outSem->signal();
	
	t=0;
	s.wait(10);
	
	outSem->wait(0);
	printf("Thread finished.\n");
	outSem->signal();
	
	s.signal();
}

void tick()
{
	t++;
	if(t){
		outSem->wait(0);
		printf("%d\n",t);
		outSem->signal();
	}
}

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


