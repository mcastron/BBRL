
#ifndef FORMULAEXCEPTION_H
#define FORMULAEXCEPTION_H

#include "../utils.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/**
	\class 	FormulaException
	\author 	Castronovo Michael
	
	\brief 	Exception.
	\date 	2014-10-12
*/
// ===========================================================================
class utils::formula::FormulaException: public std::exception
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief 		Constructor.
			
			\param[msg	The error message to transmit.
		*/
		FormulaException(std::string msg_);
		
		
		virtual ~FormulaException() throw();


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
