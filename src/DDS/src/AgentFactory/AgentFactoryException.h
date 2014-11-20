
#ifndef AGENTFACTORYEXCEPTION_H
#define AGENTFACTORYEXCEPTION_H

#include "../../ExternalLibs.h"


// ===========================================================================
/**
	\class 	AgentFactoryException
	\author 	Castronovo Michael
	
	\brief 	Exception.
	\date 	2014-07-16
*/
// ===========================================================================
class AgentFactoryException: public std::exception
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief 		Constructor.
			
			\param[msg	The error message to transmit.
		*/
		AgentFactoryException(std::string msg_);
		
		
		virtual ~AgentFactoryException() throw();


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
