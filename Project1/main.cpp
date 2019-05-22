// File: main.cpp

// Required memory model: huge

#include "kernel.h"

extern int userMain (int argc, char *argv[]);

int main(int argc, char *argv[])
{
	Kernel::initialize();
  
	int res = userMain(argc, argv);

	Kernel::terminate();
  
	return res;
}

