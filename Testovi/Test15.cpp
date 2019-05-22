#include "kernel.h"

/*
	Test: Niti maxStack velicine
*/

#include <stdio.h>
#include "semaphor.h"
Semaphore *outSem = new Semaphore();

const int n = 2;

void tick(){}

class TestThread : public Thread
{
public:

	TestThread(): Thread(32768,2) {};
	~TestThread()
	{
		waitToComplete();
	}
protected:

	void run();

};

void TestThread::run()
{

	int buffer=2;

	for(int i=0;i<32000;i++)
	{
		buffer = 4096/2048;
		for (int j = 0; j < 1024; j++)
		{
			buffer = buffer*2;
			if(buffer%2)
				buffer = 2;
		}
	}

}


int userMain(int argc, char** argv)
{
	outSem->wait(0);
	printf("Test starts: %d threads.\n",n);
	outSem->signal();
	
	int i;
	TestThread threads[n];
	for(i=0;i<n;i++)
	{
		threads[i].start();
	}
	for(i=0;i<n;i++)
	{
		threads[i].waitToComplete();
		outSem->wait(0);
		printf("%d. Done!\n",threads[i].getId());
		outSem->signal();
	}
	outSem->wait(0);
	printf("Test ends.\n");
	outSem->signal();
	return 0;
}


