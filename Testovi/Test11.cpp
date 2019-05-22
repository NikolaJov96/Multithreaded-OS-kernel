#include "kernel.h"
#include <iostream.h>
#include <stdlib.h>

/*
 	 Test: Semafori sa spavanjem 3
*/

#include <stdio.h>
#include "semaphor.h"
Semaphore *outSem = new Semaphore();

int t=-1;

void tick()
{
	/*t++;
	if(t){
		outSem->wait(0);
		printf("%d\n",t);
		outSem->signal();
	}*/
}


#include <iostream.h>

Semaphore* mutex = 0;

class Znak : public Thread
{
public:
	Znak(char znak, int n) : Thread(), znak(znak), n(n) {}
	virtual ~Znak() { /*waitToComplete();*/ }

	void run()
	{
		outSem->wait(0);
		cout << znak;
		outSem->signal();
		
		for (long i = 0; i < 100000; i++)
		{
			if (mutex->wait(n)) {
				outSem->wait(0);
				cout << znak;
				outSem->signal();
				
				mutex->signal();
			}

		}

		if (mutex->wait(n)) {
			outSem->wait(0);
			cout << endl << znak << " finished" << endl;
			outSem->signal();
	
			mutex->signal();
		}
	}

private:
	char znak;
	int n;

};


int userMain(int argc, char* argv[]) {
	mutex = new Semaphore(1);

	Znak* a = new Znak('a', 10);
	Znak* b = new Znak('b', 15);
	Znak* c = new Znak('c', 20);

	a->start();
	b->start();
	c->start();

	//a->waitToComplete();
	//b->waitToComplete();
	//c->waitToComplete();
	delete a;
	delete b;
	delete c;

	if (mutex->wait(1)) {
		outSem->wait(0);
		cout << endl << "userMain finished" << endl;
		outSem->signal();
		
		mutex->signal();
	}

	delete mutex;



	return 0;
}




