
#ifndef THREAD_H
#define THREAD_H

#include "utils.h"
#include "ThreadException.h"
#include "../ExternalLibs.h"


// ===========================================================================
/*
	\class 	Thread
	\author 	Castronovo Michael
	
	\brief 	A C++ handler for pthreads.	

	\date 	2014-12-13
*/
// ===========================================================================
class utils::Thread
{
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
					
					This Thread is not started at its instanciation
					(use 'start()').
		*/
		Thread();
		
		
		/**
			\brief	Destructor.
			
					If this Thread has been started, wait until its job
					is done (join).
		*/
		virtual ~Thread();
		
		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Start this Thread.
					It throws an exception if it has already been started. 
			
			\return	See the return of 'pthread_start()' function.
		*/
		int start() throw (utils::ThreadException);
		
		
		/**
			\brief	Wait for this Thread to finish.
					It throws an exception if it has not been started.
					
			\return	See the return of 'pthread_join()' function.
		*/
		int join() throw (utils::ThreadException);


		/**
			\brief	Defines the job to be executed.
		*/
		virtual void* run() = 0;


	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The pthread handled.
		*/
		pthread_t  m_tid;

		
		/**
			\brief	True if this Thread has been started yet.
		*/
		bool started;
		
		
		/**
			\brief	True if this Thread has been joined yet.
		*/
		bool joined;
};

#endif
