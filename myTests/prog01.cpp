// File: program.cpp

#include "prog01.h"

#include <iostream.h>

ThreadExample::ThreadExample(int n) : 
	Thread(), num(n) {}

int ThreadExample::getNum()
{
	return num;
}

void ThreadExample::run()
{
	cout << "Hello World!" << endl;	
	for (int i = 0; i < 100; i++) num++;
}

int userMain(int argc, char *argv[])
{
	ThreadExample *thread = new ThreadExample(0);
	thread->start();
	thread->waitToComplete();
	cout << thread->getNum() << endl;
	delete thread;
	return 1;
}

void tick() {}

