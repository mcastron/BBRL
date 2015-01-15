
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
               \brief    Pause this Chrono.
		*/
		void pause();
		
		
		/**
               \brief    Resume this Chrono.
		*/
		void resume();


		/**
			\brief	     Restart this Chrono.
			
			\param[paused_ True if this Chrono must be paused right after
			               being restarted, false else.
		*/
		void restart(bool paused_ = false);
		
		
		/**
			\brief	Return the time elapsed (in ms).
					
			\return	The time elapsed (in ms).
		*/
		double get();


	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
               \brief    True if this Chrono is paused, false else.
		*/
		bool paused;
		
		
		/**
               \brief    The time elapsed.
		*/
		double timeElapsed;
		
		
		/**
               \brief    The time stamp of the last moment this Chrono has been
                         started for the last time.
          */
		struct timeval startClock;
		
		
		// =================================================================
          //   Private methods
		// =================================================================
		/**
               \brief    Update the time elapsed.
		*/
		void update();
};

#endif

