#include "kernel.h"
#include <iostream.h>
#include <stdlib.h>

/*
 	 Test: Semafori sa spavanjem 2
*/

#include <stdio.h>
#include "semaphor.h"
Semaphore *outSem = new Semaphore();

int t=-1;

Semaphore s(0);

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
	
	TestThread t1(15),t2(10),t3(30);
	t1.start();
	t2.start();
	t3.start();
	s.wait(5);
	s.signal();
	s.wait(0);
	s.signal();
	
	outSem->wait(0);
	printf("Test ends.\n");
	outSem->signal();
	return 0;
}


