
#ifndef CHRONO_H
#define CHRONO_H

#include "utils.h"
#include "../ExternalLibs.h"


// ===========================================================================
/*
	\class 	Chrono
	\author 	Castronovo Michael
	
	\brief 	A chronometer to measure the wall time elapsed.
			The implementation depends on POSIX.
			(more specifically, it requires 'gettimeofday()' function)	
	
	\date 	2014-12-13
*/
// ===========================================================================
class utils::Chrono
{
	public:
		// =================================================================
		//	Public Constructor
		// =================================================================
		/**
			\brief	Constructor.
					The Chrono is started at its instanciation.
					(use the 'restart()' method to restart it)
		*/
		Chrono();
		
		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Restart this Chrono.
		*/
		void restart();
		
		
		/**
			\brief	Return the time elapsed (in ms) since the last call
					of 'start()'.
					
			\return	The time elapsed (in ms) since the last call of
					'start()'.
		*/
		double get();


	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		struct timeval startClock;
};

#endif

