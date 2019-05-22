#include "kernel.h"

/*
	Test: asinhrono preuzimanje
*/

#include <stdio.h>
#include "semaphor.h"
Semaphore *outSem = new Semaphore();

class TestThread : public Thread
{
public:

	TestThread(): Thread() {};
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
	printf("Thread %d: loop1 starts\n", getId());
	outSem->signal();

	for(int i=0;i<32000;i++)
	{
		for (int j = 0; j < 32000; j++);
	}

	outSem->wait(0);
	printf("Thread %d: loop1 ends, dispatch\n",getId());
	outSem->signal();

	dispatch();

	outSem->wait(0);
	printf("Thread %d: loop2 starts\n",getId());
	outSem->signal();

	for (int k = 0; k < 20000; k++);

	outSem->wait(0);
	printf("Thread %d: loop2 ends\n",getId());
	outSem->signal();

}



void tick(){}

int userMain(int argc, char** argv)
{
	outSem->wait(0);
	printf("User main starts\n");
	outSem->signal();
	
	TestThread t1,t2;
	t1.start();
	t2.start();
	
	outSem->wait(0);
	printf("User main ends\n");
	outSem->signal();
	
	return 16;
}


