#include "kernel.h"

/*
 	 Test: idleThread
*/

#include <stdio.h>
#include "semaphor.h"
Semaphore *outSem = new Semaphore();

void tick(){}

class TestThread : public Thread
{
private:
	TestThread *t;

public:

	TestThread(TestThread *thread): Thread(), t(thread){}
	~TestThread()
	{
		waitToComplete();
	}
protected:

	void run();

};

void TestThread::run()
{
	t->waitToComplete();
}


int userMain(int argc, char** argv)
{
	outSem->wait(0);
	printf("Test starts.\n");
	outSem->signal();
		
	TestThread *t1,*t2;
	t1 = new TestThread(t2);
	t2 = new TestThread(t1);
	t1->start();
	t2->start();
	t1->waitToComplete();
	// IdleThread running forever
	delete t1;
	delete t2;
	
	outSem->wait(0);
	printf("Test ends.\n");
	outSem->signal();
	return 0;
}


