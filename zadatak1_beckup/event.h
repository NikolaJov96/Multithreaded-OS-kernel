// File: event.h

#ifndef _event_h_
#define _event_h_

typedef unsigned char IVTNo;					// Type of variables representing IVT entry IDs
typedef void (interrupt *InterruptRoutine)();	// Type of finctins representing interrupt routines

class KernelEv;		// See kevent.h

/*!
 * class Event
 * Wrapper class for KernelEv
 */
class Event
{
public:
	Event(IVTNo ivtNo);			// Constructor with provided IVT entry
	~Event();
	
	void wait();				// KernelEv::wait() wrapper - pauses execution of the thread until event

protected:
	friend class KernelEv;
	friend class IVTEntry;
	void signal(); 				// KernelEv::signal() wrapper - called on event, frees the thread

private:
	KernelEv* myImpl;			// Associated KernelEv object
	
};

/*!
 * class IVTEntry
 * Parent class for individual entry subclasses
 */
class IVTEntry
{
public:
	static void handle(int id, int oldRoutine);
	
};

void setupRoutine(int id, InterruptRoutine routine);	// Bounds prepared static methot (sets new interrupt routine)

// Defines specific IVTEntry subclass for desired IVT entry ID
// Defines execute function that will serve as interrupt routine
#define PREPAREENTRY(id,oldRoutine)\
	class IVTEntry_##id: public IVTEntry\
	{\
		public:\
			static interrupt void execute(/*...*/)\
			{\
				handle(id, oldRoutine);\
			}\
	};\
	setupRoutine(id, IVTEntry_##id::execute);

#endif

