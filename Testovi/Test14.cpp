#include "kernel.h"

/*
	Test: brojac sekundi
*/

#include <stdio.h>
#include "semaphor.h"
Semaphore *outSem = new Semaphore();

unsigned t=18;
unsigned seconds = 5;

void secondPast()
{
	if(seconds){
		outSem->wait(0);
		printf("%d\n",seconds);
		outSem->signal();
	}
	seconds--;
}

void tick()
{
	t--;
	if(t==0){
		t = 18;
		secondPast();
	}
}



int userMain(int argc, char** argv)
{
	outSem->wait(0);
	printf("Starting test\n");
	outSem->signal();
	
	while(seconds);
	
	outSem->wait(0);
	printf("Test ends\n");
	outSem->signal();
	return 0;
}
