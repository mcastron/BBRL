
#ifndef THREADEXCEPTION_H
#define THREADEXCEPTION_H

#include "utils.h"
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	ThreadException
	\author 	Castronovo Michael
	
	\brief 	Exception.
	\date 	2014-07-09
*/
// ===========================================================================
class utils::ThreadException: public std::exception
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief 		Constructor.
			
			\param[msg	The error message to transmit.
		*/
		ThreadException(std::string msg_);
		
		
		/**
		   \brief    Destructor.
		*/
		virtual ~ThreadException() throw();


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief		Returns the error message.
			
			\return		The error messIAge.
		*/
		virtual const char* what() const throw();
	
	
	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The error message.
		*/
		std::string msg;
};

#endif
