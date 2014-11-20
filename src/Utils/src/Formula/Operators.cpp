
#include "Operators.h"

using namespace utils::formula;


// ---------------------------------------------------------------------------
//   Unary operators - ABS, LN, SQRT, INV, OPP
// ---------------------------------------------------------------------------
double AbsOp::operator()(const std::vector<double>& operands) const
                                                       throw (TokenException)
{
     assert(operands.size() == getArity());
     
     
     return fabs(operands[0]);
}

double LnOp::operator()(const std::vector<double>& operands) const
                                             throw (TokenException)
{
     assert(operands.size() == getArity());
     
     
     if (operands[0] <= 0)
     {
          std::string msg;
          msg += "Cannot apply ln(.) on a number <= 0! ";
          msg += "(token evaluation)\n";

          throw TokenException(msg);
     }
     
     return log(operands[0]);
}

double SqrtOp::operator()(const std::vector<double>& operands) const
                                             throw (TokenException)
{
     assert(operands.size() == getArity());
     
     
     if (operands[0] < 0)
     {
          std::string msg;
          msg += "Cannot apply sqrt(.) on a number < 0! ";
          msg += "(token evaluation)\n";

          throw TokenException(msg);
     }

     return sqrt(operands[0]);
}

double InvOp::operator()(const std::vector<double>& operands) const
                                             throw (TokenException)
{
     assert(operands.size() == getArity());
     
     
     if (operands[0] == 0)
     {
          std::string msg;
          msg += "Cannot apply inv(.) on a number equal to 0! ";
          msg += "(token evaluation)\n";

          throw TokenException(msg);
     }

     return (1.0 / operands[0]);
}

double OppOp::operator()(const std::vector<double>& operands) const
                                             throw (TokenException)
{
     assert(operands.size() == getArity());


     return -operands[0];
}


// ---------------------------------------------------------------------------
//   Binary operators - SUB, DIV
// ---------------------------------------------------------------------------
double SubOp::operator()(const std::vector<double>& operands) const
                                                       throw (TokenException)
{
     assert(operands.size() == getArity());
     
     
     return (operands[1] - operands[0]);
}

double DivOp::operator()(const std::vector<double>& operands) const
                                             throw (TokenException)
{
     assert(operands.size() == getArity());


     if (operands[0] == 0)
     {
          std::string msg;
          msg += "Cannot divide by 0! (token evaluation)\n";

          throw TokenException(msg);
     }               
     
     return (operands[1] / operands[0]);
}


// ---------------------------------------------------------------------------
//   n-ary operators - ADD, MUL, MIN, MAX, AVG
// ---------------------------------------------------------------------------
double AddOp::operator()(const std::vector<double>& operands) const
                                                       throw (TokenException)
{
     assert(operands.size() == getArity());
     
     
     double sum = 0.0;
     for (unsigned int i = 0; i < getArity(); ++i)
          sum += operands[getArity() - 1 - i];

     return sum;
}

double MulOp::operator()(const std::vector<double>& operands) const
                                             throw (TokenException)
{
     assert(operands.size() == getArity());
     
     
     double mul = 1.0;
     for (unsigned int i = 0; i < getArity(); ++i)
          mul *= operands[getArity() - 1 - i];

     return mul;
}

double MinOp::operator()(const std::vector<double>& operands) const
                                             throw (TokenException)
{
     assert(operands.size() == getArity());


     std::vector<std::vector<double>::const_iterator> minList;
     minList = utils::search::min<std::vector<double> >(
               operands.begin(), (operands.begin() + getArity()));

     return *(minList[0]);
}

double MaxOp::operator()(const std::vector<double>& operands) const
                                             throw (TokenException)
{
     assert(operands.size() == getArity());


     std::vector<std::vector<double>::const_iterator> maxList;
     maxList = utils::search::max<std::vector<double> >(
               operands.begin(), (operands.begin() + getArity()));

     return *(maxList[0]);
}

double AvgOp::operator()(const std::vector<double>& operands) const
                                             throw (TokenException)
{
     assert(operands.size() == getArity());


     double avg = 0.0, c = (1.0 / (double) getArity());
     for (unsigned int i = 0; i < getArity(); ++i)
          avg += (c * operands[getArity() - 1 - i]);

     return avg;
}
