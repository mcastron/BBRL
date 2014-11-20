
#ifndef SERIALIZABLEEXCEPTION_H
#define SERIALIZABLEEXCEPTION_H

#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	SerializableException
	\author 	Castronovo Michael
	
	\brief 	Exception.
	\date 	2014-05-15
*/
// ===========================================================================
class SerializableException: public std::exception
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief 		Constructor.
			
			\param[msg	The error message to transmit.
		*/
		SerializableException(std::string msg_);
		
		
		virtual ~SerializableException() throw();


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
