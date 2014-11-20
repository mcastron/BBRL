
#ifndef CONSTANT_H
#define CONSTANT_H

#include "Token.h"
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	Constant
	\author 	Castronovo Michael
	
	\brief 	A constant value.
	
	\date 	2014-10-12
*/
// ===========================================================================
/* final */class Constant : public Token
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief		 Constructor.
			
			\param[value_   The value of this constant.
		*/
		Constant(double value_) : Token(0), value(value_) {}


		// =================================================================
		//	Public methods
		// =================================================================
          /**
               \brief          Return the value of this Token.
               
               \param[operands The operands of this Token in reverse order
                               (operands[0] is the last operand).
               
               \return         The value of this Token.
          */
          double operator()(const std::vector<double>& operands
                                             = std::vector<double>()) const
                                                       throw (TokenException)
          {
               assert(operands.size() == getArity());
               
               
               return value;
          }
          
          
          /**
               \brief    Return the symbol representing this Token.
          */
          std::string getSymbol() const
          {
               std::stringstream sstr;
               sstr << value;
               
               return sstr.str();
          }


     private:
          // =================================================================
		//	Private attributes
		// =================================================================
		/**
               \brief    The value of this constant.
		*/
		double value;
};

#endif
