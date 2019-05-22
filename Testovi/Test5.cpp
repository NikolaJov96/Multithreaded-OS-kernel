#include "kernel.h"

/*
 	 Test: frekventni dispatch
*/

#include <stdio.h>
#include "semaphor.h"
Semaphore *outSem = new Semaphore();

const int n = 10;

void tick(){}

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
	for(int i=0;i<32000;i++)
	{
		for(int j=0;j<16;j++){
		dispatch();
		
		outSem->wait(0);
		printf(".%d.", this->getId());
		outSem->signal();
		}
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


