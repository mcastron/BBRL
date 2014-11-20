
#ifndef OPERATORS_H
#define OPERATORS_H

#include "Token.h"
#include "../utils.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/**
	\author 	Castronovo Michael
	
	\brief 	Defines the operators to use in formulas.
	
	\date 	2014-10-16
*/
// ===========================================================================
// ---------------------------------------------------------------------------
//   Unary operators - ABS, LN, SQRT, INV, OPP
// ---------------------------------------------------------------------------
/* final */ class utils::formula::AbsOp : public utils::formula::Token
{		
	public:
		AbsOp() : Token(1) {}

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const { return "ABS"; }
};

/* final */ class utils::formula::LnOp : public utils::formula::Token
{		
	public:
		LnOp() : Token(1) {}

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const { return "LN"; }
};

/* final */ class utils::formula::SqrtOp : public utils::formula::Token
{		
	public:
		SqrtOp() : Token(1) {}

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const { return "SQRT"; }
};

/* final */ class utils::formula::InvOp : public utils::formula::Token
{		
	public:
		InvOp() : Token(1) {}

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const { return "INV"; }
};

/* final */ class utils::formula::OppOp : public utils::formula::Token
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
/* final */ class utils::formula::SubOp : public utils::formula::Token
{		
	public:
		SubOp() : Token(2) {}

          double operator()(const std::vector<double>& operands) const
                                                       throw (TokenException);
          std::string getSymbol() const { return "SUB"; }
};

/* final */ class utils::formula::DivOp : public utils::formula::Token
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
/* final */ class utils::formula::AddOp : public utils::formula::Token
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

/* final */ class utils::formula::MulOp : public utils::formula::Token
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

/* final */ class utils::formula::MinOp : public utils::formula::Token
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

/* final */ class utils::formula::MaxOp : public utils::formula::Token
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

/* final */ class utils::formula::AvgOp : public utils::formula::Token
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
