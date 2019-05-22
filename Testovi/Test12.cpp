#include "kernel.h"
#include <iostream.h>
#include <stdlib.h>

/*
 	 Test: Semafori sa spavanjem 4
*/

#include <stdio.h>
#include "semaphor.h"
Semaphore *outSem = new Semaphore();

int t=-1;

const int n=15;

Semaphore s(1);

class TestThread : public Thread
{
private:
	Time waitTime;

public:

	TestThread(Time WT): Thread(), waitTime(WT){}
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
	printf("Thread %d waits for %d units of time.\n",getId(),waitTime);
	outSem->signal();
	
	int r = s.wait(waitTime);
	if(getId()%2)
		s.signal();
	
	outSem->wait(0);
	printf("Thread %d finished: r = %d\n", getId(),r);
	outSem->signal();
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
	
	TestThread* t[n];
	int i;
	for(i=0;i<n;i++)
	{
		t[i] = new TestThread(5*(i+1));
		t[i]->start();
	}
	for(i=0;i<n;i++)
	{
		t[i]->waitToComplete();
	}
	delete t;
	outSem->wait(0);
	printf("Test ends.\n");
	outSem->signal();
	return 0;
}


