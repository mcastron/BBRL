
#ifndef VARIABLE_H
#define VARIABLE_H

#include "Token.h"
#include "../utils.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	Variable
	\author 	Castronovo Michael
	
	\brief 	A Token representing a variable value.
	
	\date 	2014-12-13
*/
// ===========================================================================
/* final */class utils::formula::Variable : public utils::formula::Token
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief		 Constructor.

			\param[rank_    The rank of this Variable.
		*/
		Variable(unsigned int rank_) :
                    Token(0), rank(rank_), hasValue(false) {}


		// =================================================================
		//	Public methods
		// =================================================================
		/**
               \brief    Return the rank of this variable.
               
               \return   The rank of this variable.
		*/
		unsigned int getRank() const { return rank; }		
		
		
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
               
               
               if (!hasValue)
               {
                    std::string msg;
                    msg += "This variable has no value! (token evaluation)\n";

                    throw TokenException(msg);
               }
               
               return value;
          }
          
          
          /**
               \brief    Return the symbol representing this Token.
          */
          std::string getSymbol() const
          {
               std::stringstream sstr;
               sstr << "X" << rank;

               return sstr.str();
          }
          
          
          /**
               \brief         Set a new value to this variable.
               
               \param[value_  The new value of this variable.
          */
          void set(double value_) { value = value_; hasValue = true; }
          
          
          /**
               \brief    Unset this variable.
          */
          void unset() { hasValue = false; }


     private:
          // =================================================================
		//	Private attributes
		// =================================================================
		/**
               \brief    The rank of this variable.
		*/
		unsigned int rank;


		/**
               \brief    True if this variable is has a value, false else.
		*/
		bool hasValue;
		
		
		/**
               \brief    The value of this variable.
		*/
		double value;
};

#endif
