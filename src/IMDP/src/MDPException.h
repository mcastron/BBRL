
#ifndef MDPEXCEPTION_H
#define MDPEXCEPTION_H

#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	MDPException
	\author 	Castronovo Michael
	
	\brief 	Exception.
	\date 	2014-03-24
*/
// ===========================================================================
class MDPException: public std::exception
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief 		Constructor.
			
			\param[msg	The error message to transmit.
		*/
		MDPException(std::string msg_);
		
		
		virtual ~MDPException() throw();


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
