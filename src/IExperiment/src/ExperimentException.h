
#ifndef EXPERIMENTEXCEPTION_H
#define EXPERIMENTEXCEPTION_H

#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	ExperimentException
	\author 	Castronovo Michael
	
	\brief 	Exception.
	\date 	2014-03-24
*/
// ===========================================================================
class ExperimentException: public std::exception
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief 		Constructor.
			
			\param[msg	The error message to transmit.
		*/
		ExperimentException(std::string msg_);
		
		
		virtual ~ExperimentException() throw();


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief		Returns the error message.
			
			\return		The error message.
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
