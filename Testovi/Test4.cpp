#include "kernel.h"

/*
 	 Test: razlicita vremena izvrsavanja i velicine steka
*/

#include <stdio.h>
#include "semaphor.h"
Semaphore *outSem = new Semaphore();

volatile Time ts;

void tick()
{
	outSem->wait(0);
	printf("timeSlice=%d\n",ts);
	outSem->signal();
}

class TestThread : public Thread
{
private:
	Time myTimeSlice;
public:

	TestThread(StackSize stackSize, Time timeSlice): Thread(stackSize,timeSlice), myTimeSlice(timeSlice) {};
	~TestThread()
	{
		waitToComplete();
	}
protected:

	void run();

};

void TestThread::run()
{
	for(unsigned i=0;i<32000;i++)
	{
		for(unsigned int j=0;j<32000;j++)
		{
			ts = myTimeSlice;
		}
	}
}


int userMain(int argc, char** argv)
{
	outSem->wait(0);
	printf("Test starts.\n");
	outSem->signal();
	
	TestThread t1(64,1), t2(4096,32), t3(1024,16), t4(4096,0);
	t1.start();
	t2.start();
	t3.start();
	t4.start();
	t1.waitToComplete();
	t2.waitToComplete();
	t3.waitToComplete();
	t4.waitToComplete();
	
	outSem->wait(0);
	printf("Test ends.\n");
	outSem->signal();
	return 0;
}


