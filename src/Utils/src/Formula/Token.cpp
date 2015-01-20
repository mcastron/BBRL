
#include "Token.h"
#include "Operators.h"
#include "Constant.h"
#include "Variable.h"

using namespace utils::formula;


// ============================================================================
//	Public static methods
// ============================================================================
Token* Token::getToken(std::string str) throw (TokenException)
{
     //   Operator case
          //   Unary & Binary
     if (str == AbsOp().getSymbol())  { return new AbsOp();  }
     if (str == LnOp().getSymbol())   { return new LnOp();   }
     if (str == SqrtOp().getSymbol()) { return new SqrtOp(); }
     if (str == InvOp().getSymbol())  { return new InvOp();  }
     if (str == OppOp().getSymbol())  { return new OppOp();  }
     if (str == SubOp().getSymbol())  { return new SubOp();  }
     if (str == DivOp().getSymbol())  { return new DivOp();  }
     
          //   n-ary
     std::string tmp;
     size_t p = str.find_first_of("0123456789");
     if (p < std::string::npos){ tmp = str.substr(p, std::string::npos); }
     
     unsigned int arity = atoi(tmp.c_str());
     if (arity > 1)
     {
          if (str == AddOp(arity).getSymbol()) { return new AddOp(arity); }
          if (str == MulOp(arity).getSymbol()) { return new MulOp(arity); }
          if (str == MinOp(arity).getSymbol()) { return new MinOp(arity); }
          if (str == MaxOp(arity).getSymbol()) { return new MaxOp(arity); }
          if (str == AvgOp(arity).getSymbol()) { return new AvgOp(arity); }
     }
     
     
     //   Variable
     unsigned int rank = arity;
     if (str == Variable(rank).getSymbol()) { return new Variable(rank); }
     
     
     //   Constant
     unsigned int i = 0;
     for (; ((i < str.size())
               && ((std::isdigit(str[i]) || (str[i] == '.')
                    || (str[i] == '-') || (str[i] == 'e')))); ++i);
     
     if (i == str.size()) { return new Constant(atof(str.c_str())); }
     
     
     //   Error
     std::string msg;
     msg += "Unable to convert \"" + str + "\" into a Token!\n";
     
     throw TokenException(msg);
}
