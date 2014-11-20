
#ifndef FORMULA_H
#define FORMULA_H

#include "Token.h"
#include "Variable.h"
#include "FormulaException.h"
#include "../utils.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/**
	\class 	Formula
	\author 	Castronovo Michael
	
	\brief 	Defines a Formula through a set of tokens, interpreted in
	          RPN notation.
	          
	          e.g.:    '1 2 ln +'               ==> (1 + ln(2))
	                   '3 5 * 2 / 6 sqrt -'     ==> ((3 * 5) / 2) - sqrt(6)

	\date 	2014-10-16
*/
// ===========================================================================
/* final */ class utils::formula::Formula
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief		   Constructor.
			                  There must be a single instance of each token
			                  used (if a token is used twice or more in the
			                  formula, the pointers have to point to the
			                  same token instance).
			
			                  Throw an exception if the Formula is invalid.
			
			\param[tokenList_ The list of tokens composing this Formula.
		*/
		Formula(std::vector<Token*>& tokenList_) throw (FormulaException);
		
		
		/**
               \brief         Constructor.
               
               \param[str     A string representing a formula in RPN notation.
		*/
		Formula(std::string str) throw (FormulaException);
		
		
		/**
               \brief    Destructor.
                         'tokenList' is freed.
		*/
		~Formula();	


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
               \brief              Given a set of tokens, return the list of
                                   all valid formulas of size at most 's'.
               
               \param[tokenSet     A set of tokens.
               
               \return             The list of all valid formulas of size at
                                   most 's'.
		*/
		static std::vector<Formula*> generate(
                    std::set<Token*, Token::pComp> tokenSet,
                    unsigned int s);


          /**
               \brief              Return a minimal list of formulas by
                                   discarding the formulas of 'formulaList'
                                   which are similar.
                                   
                                   A formula 'F1' is similar to formula 'F2'
                                   according to a set of points iff they
                                   order this set of points in the same way.
               
               \param[formulaList  The list of formulas to reduce.
               \param[pointList    The points according to which to do the
                                   reduction.
          */
          static std::vector<Formula*> reduce(
                    std::vector<Formula*> formulaList,
                    const std::vector<std::vector<double> >& pointList);


		// =================================================================
		//	Public methods
		// =================================================================	
          /**
               \brief     Return the value of this Formula.
                          If a variable appears several times in 'x', the last
                          assignation will be effective.

               \param[val The value assignate to each variable
                          (x[i] := val[i]).
               
               \return    The value of this Formula.
          */
          double operator()(const std::vector<double>& val) const
                                                  throw (FormulaException);


          /**
               \brief    Return the number of tokens composing this formula.
               
               \return   The number of tokens composing this formula.
          */
          unsigned int size() const { return tokenList.size(); }


          /**
               \brief    Return a string representing this Formula in RPN
                         notation.
               
               \return   A string representing this FOrmula in RPN notation .
          */
          std::string getRPNStr() const;
          
          
          /**
               \brief    Return true if variable at rank 'rank_' is used
                         by this formula, false else.

               \return   True if variable at rank 'rank_' is used by this
                         formula, false else.
          */
          bool isVarUsed(unsigned int rank_) const
          {
               return (varMap.find(rank_) != varMap.end());
          }


	private:	
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
               \brief    The list of tokens composing this Formula.
		*/
		std::vector<Token*> tokenList;
		
		
		/**
               \brief    The set of all variables used by this Formula.
		*/
		std::map<unsigned int, Variable*> varMap;


		// =================================================================
		//	Private methods
		// =================================================================
		/**
               \brief         Return the list of tokens corresponding to this
                              string (do not instance twice or more times the
                              same token).

               \param[str     The string representing a list of tokens.

               \return        The list of tokens corresponding to this string.
		*/
		std::vector<Token*> getTokensFromStr(std::string str) const
		                                           throw (TokenException);
		
		
		/**
               \brief    Build 'varMap' from 'tokenList'.
		*/
		void buildVarMap() throw (FormulaException);
		
		
		/**
               \brief    Delete the tokens of 'tokenList'.
                         (take care of duplicates)
		*/
		void deleteTokens();
		
		
		/**
               \brief    Return true if this Formula is valid, false else.
                         
               \return   True if this Formula is valid, false else.
		*/
		bool isValid();
		
		
		/**
               \brief           Compute and return the key of this formula
                                according to the points given (a point is an
                                assigment for all variables).
                              
                                Two formulas sharing the same key are formulas
                                which order the given points in the same way.

               \param[pointList A list of points.
               
               \return          The key of this formula according to the points
                                given.
          */
          std::vector<unsigned int> getKey(
                    const std::vector<std::vector<double> >& pointList) const
                                                  throw (FormulaException);
		
		
		// =================================================================
          //   Private Classes
		// =================================================================
		/**
               \brief    Represents a pair between a rank and a value of
                         a point, which are sorted only by their value.
		*/
		class RankValuePair
		{
               public:
                    unsigned int rank;
                    double value;
               
                    RankValuePair(unsigned int rank_ = 0,
                                  double value_ = 0.0) : rank(rank_),
                                                         value(value_) {}

                    bool operator<(const RankValuePair& a) const
                    {
                         return (value < a.value);
                    }
		                          
		};
};

#endif
