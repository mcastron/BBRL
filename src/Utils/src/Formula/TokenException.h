
#ifndef TOKENEXCEPTION_H
#define TOKENEXCEPTION_H

#include "../utils.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	TokenException
	\author 	Castronovo Michael
	
	\brief 	Exception.
	\date 	2014-12-13
*/
// ===========================================================================
class utils::formula::TokenException: public std::exception
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief 		Constructor.
			
			\param[msg	The error message to transmit.
		*/
		TokenException(std::string msg_);
		
		
		virtual ~TokenException() throw();


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
