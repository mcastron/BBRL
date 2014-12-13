
#ifndef TOKEN_H
#define TOKEN_H

#include "TokenException.h"
#include "../utils.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	Token
	\author 	Castronovo Michael
	
	\brief 	Defines a Token, an element composing a Formula.

	\date 	2014-12-13
*/
// ===========================================================================
class utils::formula::Token
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief		 Constructor.
			
			\param[arity    The arity of this token. 
		*/
		Token(unsigned int arity_) : arity(arity_) {}


		/**
			\brief	Destructor.
		*/
		virtual ~Token() {}


		// =================================================================
		//	Public static methods
		// =================================================================
          /**
               \brief         Return the token represented by the given
                              string if possible, throw a TokenException else.
               
               \param[str     The string representing this token.
          */
          static Token* getToken(std::string str) throw (TokenException);


		// =================================================================
		//	Public methods
		// =================================================================
		/**
		     \brief    Return the arity of this Token.
		     
		     \return   The arity of this Token.
		*/
		unsigned int getArity() const { return arity; }
		
		
          /**
               \brief          Return the value of this Token.
               
               \param[operands The operands of this Token in reverse order
                               (operands[0] is the last operand).
               
               \return         The value of this Token.
          */
          virtual double operator()(const std::vector<double>& operands) const
                                                  throw (TokenException) = 0;


          /**
               \brief    Return the symbol representing this Token.
          */
          virtual std::string getSymbol() const = 0;
          
          
          // =================================================================
		//	Public classes
		// =================================================================
          /**
               \brief   A comparator for Token*.
          */
          struct pComp
          {
               bool operator() (
                         const Token* const& a, const Token* const& b) const
               {
                    return ((a != b) && (a->getSymbol() != b->getSymbol()));
               }
          };


	private:	
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
               \brief    The arity of this token.
		*/
		unsigned int arity;
};

#endif
