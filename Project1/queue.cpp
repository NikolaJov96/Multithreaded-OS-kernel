// File: queue.cpp

#include "queue.h"

#include "schedule.h"
#include "ksem.h"
#include "kernel.h"

Queue::~Queue() 
{
	Node *curr = first;
	while (curr != NULL)
	{
		Node *del = curr;
		curr = curr->next;
		delete del;
	}
}

int Queue::check()
{
	for (Node *curr = first; curr; curr = curr->next)
	{
		if (!curr->pcb)
		{
			return 0;
		}
	}
	return 1;
}

void Queue::add(PCB *pcb) 
{
	if (!first) first = last = new Node(pcb);
	else last = (last->next = new Node(pcb));
	co++;
}

PCB *Queue::get()
{
	if (!first) return NULL;
	Node* curr = first;
	if (first == last) last = NULL;
	first = first->next;
	PCB* pcb = curr->pcb;
	delete curr;
	co--;
	return pcb;
}

int Queue::isEmpty() const
{
	return (first == NULL);
}

int Queue::removePCB(PCB *pcb)
{
	if (!first) return 0;
	Node *curr = first, *prev = NULL;
	
	while ((curr != NULL) && (curr->pcb != pcb))	// Find desired PCB
	{
		prev = curr;
		curr = curr->next;
	}
	if (!curr) return 0;	// If pcb not found, return
	
	if (first == curr)
	{
		if (first == last) first = last = NULL;
		else first = first->next;
	}	 
	else
	{
		if (curr == last) last = prev;
		prev->next = curr->next;
	}
	delete curr;
	co--;
	return 1;
}

void Queue::update()	// Only applicable on semaphore queues
{
	for (Node *curr = first; curr != NULL;)
	{
		Node *next = curr->next;
		if (curr->pcb->state == READY) removePCB(curr->pcb);	// Remove PCBs that were awaken by semaphor
		else if (curr->pcb->timeToSleep <= 0)	// Remove PCBs whose sleep time has expired
		{
			curr->pcb->state = READY;			// Set PCB's state to READY to receive CPU
			curr->pcb->timeToSleep = 0;
			if (curr->pcb->waiting) ((KernelSem*)(curr->pcb->waiting))->remove(curr->pcb);	// Remove from semaphore waiting queue
			
			Kernel::enqueue(curr->pcb);			// Add PCB to Scheduler pool
			
			removePCB(curr->pcb);				// Remove PCB form queue
		}
		curr = next;
	}

	for (curr = first; curr != NULL; curr = curr->next)
		curr->pcb->timeToSleep--;			// Reduce sleep time of the remaining PCBs
}

Thread *Queue::findThreadWithID(ID id)
{
	for (Node *curr = first; curr != NULL; curr = curr->next)
		if (id == curr->pcb->threadID) 
			return curr->pcb->thread;
	return NULL;
}

