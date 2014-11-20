
#include "Thread.h"


// ===========================================================================
//	Extra function
// ===========================================================================
/**
	\brief	Used by Thread to launch a pthread.
*/
static void* runThread(void* arg) { return ((utils::Thread*) arg)->run(); }


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
utils::Thread::Thread() : m_tid(0), started(false), joined(false) {}


utils::Thread::~Thread()
{
	if (!started)		{ return;		}
	if (!joined)		{ join();		}
}


// ===========================================================================
//	Public methods
// ===========================================================================
int utils::Thread::start() throw (utils::ThreadException)
{
	//	Error case
	if (started)
	{
		std::string msg = "This Thread has already be started!\n";
		throw utils::ThreadException(msg);
	}
	
	
	started = true;
	return pthread_create(&m_tid, NULL, runThread, this);

}


int utils::Thread::join() throw (utils::ThreadException)
{
	//	Error case
	if (!started)
	{
		std::string msg = "This Thread must be started first!\n";
		throw utils::ThreadException(msg);
	}


	joined = true;
	return pthread_join(m_tid, NULL);

}

