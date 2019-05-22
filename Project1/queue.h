// File: queue.h

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdlib.h>

#include "PCB.h"
#include "thread.h"

/*!
 * class Queue
 * Class used for queues of PCB structures inside kernel
 */
class Queue {
public:
	Queue() : first(NULL), last(NULL), co(0) {}
	~Queue();								/*!< Free queue contents */
	
	int check();
	void add(PCB *pcb);						/*!< Add PCB to queue */
	PCB *get();								/*!< Remove PCV from queue */
	int isEmpty() const;					/*!< Check if queue is empty */
	int removePCB(PCB *pcb);				/*!< Remove PCB form queue by it's address */
	void update();							/*!< Update timer on sleeping thread */
	Thread* findThreadWithID(ID idToFind); 	/*!< Get thread with specific ID from the queue */
	
private:
	/*!
	 * struct Node
	 * Structure that holds one PCB inside of queue
	 */
	struct Node 
	{
		PCB *pcb;
		Node *next;
		Node(PCB *p, Node *n = NULL) : pcb(p), next(n) {}
	};
	Node *first, *last;
	int co;
	
};


#endif

