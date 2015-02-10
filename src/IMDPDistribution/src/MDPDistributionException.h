
#ifndef MDPDISTRIBUTIONEXCEPTION_H
#define MDPDISTRIBUTIONEXCEPTION_H

#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	MDPDistributionException
	\author 	Castronovo Michael
	
	\brief 	Exception.
	\date 	2014-05-18
*/
// ===========================================================================
class MDPDistributionException: public std::exception
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief 		Constructor.
			
			\param[msg	The error message to transmit.
		*/
		MDPDistributionException(std::string msg_);
		
		
		virtual ~MDPDistributionException() throw();


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
