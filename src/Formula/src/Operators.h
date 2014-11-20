
#ifndef OPERATORS_H
#define OPERATORS_H

#include "Token.h"
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\author 	Castronovo Michael
	
	\brief 	Defines the operators to use in formulas.
	
	\date 	2014-10-12
*/
// ===========================================================================
// ---------------------------------------------------------------------------
//   Unary operators - ABS, LN, SQRT, INV, OPP
// ---------------------------------------------------------------------------
/* final */class AbsOp : public Token
{		
	public:
		AbsOp() : Token(1) {}

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const { return "ABS"; }
};

/* final */class LnOp : public Token
{		
	public:
		LnOp() : Token(1) {}

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const { return "LN"; }
};

/* final */class SqrtOp : public Token
{		
	public:
		SqrtOp() : Token(1) {}

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const { return "SQRT"; }
};

/* final */class InvOp : public Token
{		
	public:
		InvOp() : Token(1) {}

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const { return "INV"; }
};

/* final */class OppOp : public Token
{		
	public:
		OppOp() : Token(1) {}

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const { return "OPP"; }
};


// ---------------------------------------------------------------------------
//   Binary operators - SUB, DIV
// ---------------------------------------------------------------------------
/* final */class SubOp : public Token
{		
	public:
		SubOp() : Token(2) {}

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const { return "SUB"; }
};

/* final */class DivOp : public Token
{		
	public:
	     DivOp() : Token(2) {}

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const { return "DIV"; }
};


// ---------------------------------------------------------------------------
//   n-ary operators - ADD, MUL, MIN, MAX, AVG
// ---------------------------------------------------------------------------
/* final */class AddOp : public Token
{		
	public:
		AddOp(unsigned int arity) : Token(arity) { assert(arity >= 2); }

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const
          {
               std::stringstream sstr;
               sstr << "ADD" << getArity();

               return sstr.str();
          }
};

/* final */class MulOp : public Token
{		
	public:
		MulOp(unsigned int arity) : Token(arity) { assert(arity >= 2); }

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const
          {
               std::stringstream sstr;
               sstr << "MUL" << getArity();

               return sstr.str();
          }
};

/* final */class MinOp : public Token
{		
	public:
	     MinOp(unsigned int arity) : Token(arity) { assert(arity >= 2); }

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const
          {
               std::stringstream sstr;
               sstr << "MIN" << getArity();

               return sstr.str();
          }
};

/* final */class MaxOp : public Token
{		
	public:
	     MaxOp(unsigned int arity) : Token(arity) { assert(arity >= 2); }

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const
          {
               std::stringstream sstr;
               sstr << "MAX" << getArity();

               return sstr.str();
          }
};

/* final */class AvgOp : public Token
{		
	public:
	     AvgOp(unsigned int arity) : Token(arity) { assert(arity >= 2); }

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const
          {
               std::stringstream sstr;
               sstr << "AVG" << getArity();

               return sstr.str();
          }
};
#endif
