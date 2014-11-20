
#ifndef AGENTEXCEPTION_H
#define AGENTEXCEPTION_H

#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	AgentException
	\author 	Castronovo Michael
	
	\brief 	Exception.
	\date 	2014-03-24
*/
// ===========================================================================
class AgentException: public std::exception
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief 		Constructor.
			
			\param[msg	The error message to transmit.
		*/
		AgentException(std::string msg_);
		
		
		virtual ~AgentException() throw();


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
