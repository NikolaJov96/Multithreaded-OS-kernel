// File: main.cpp

// Required memory model: huge

#include <iostream.h>

#include "kernel.h"

extern int userMain (int argc, char *argv[]);

int main(int argc, char *argv[])
{
	cout << "Program starting!" << endl << endl;
	
	Kernel::initialize();
  
	int res = userMain(argc, argv);

	Kernel::terminate();
  
	cout << endl << "Program finished!" << endl;
	int i; cin >> i;
  
	return res;
}

