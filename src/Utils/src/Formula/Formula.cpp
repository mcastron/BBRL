
#include "Formula.h"

using namespace std;
using namespace utils::formula;


// ============================================================================
//	Public Constructor/Destructor
// ============================================================================
Formula::Formula(std::vector<Token*>& tokenList_) throw (FormulaException) :
          tokenList(tokenList_) { buildVarMap(); }


Formula::Formula(std::string str) throw (FormulaException)
{
     try                       { tokenList = getTokensFromStr(str); }
     catch (TokenException& e) { throw FormulaException(e.what());  }
     buildVarMap();
}


Formula::~Formula() { deleteTokens(); }


// ============================================================================
//	Public static methods
// ============================================================================
std::vector<Formula*> Formula::generate(
          std::set<Token*, Token::pComp> tokenSet,
          unsigned int s)
{
     //   For each formula size...
     std::vector<Formula*> formulaList;
     for (unsigned int cS = 1; cS <= s; ++cS)
     {
          //   Initialize an iterator for 'space'
          std::vector<std::set<Token*>::iterator> itList;
          for (unsigned int i = 0; i < cS; ++i)
               itList.push_back(tokenSet.begin());
     
          //   While the first iterator is valid...
          while (itList[0] != tokenSet.end())
          {
               //   Push the tokens pointed by the iterators
               std::map<std::string, Token*> tokenMap;
               std::vector<Token*> tokenList;
               for (unsigned int i = 0; i < cS; ++i)
               {
                    //   Generate the token to add
                    Token* token;
                    std::string symb = (*itList[i])->getSymbol();
                    
                         //   Search for the token symbol in 'tokenMap'
                    std::map<std::string, Token*>::iterator it;
                    
                         //   The token has already been generated during this
                         //   loop
                    it = tokenMap.find(symb);
                    if (it != tokenMap.end()) { token = it->second; }
                    
                         //   The token has to be generated and added to the
                         //   map
                    else
                    {
                         token = Token::getToken(symb);
                         tokenMap[symb] = token;
                    }


                    //   Add the token
                    tokenList.push_back(token);
               }
               
               
               //   Try to build the corresponding formula and store it
               //   if it worked, discard the formula else
               try
               {
                    Formula* f = new Formula(tokenList);
                    formulaList.push_back(f);
               }
               catch (FormulaException& f) {}
     
               
               //   Advance in the sequence for the last iterator.
               //   If it reaches the end, restart it and proceed as well with
               //   the previous one.
               ++(itList.back());
               for (unsigned int j = (itList.size() - 1);
                         ((j > 0) && (itList[j] == tokenSet.end())); --j)
               {
                    itList[j] = tokenSet.begin();
                    ++(itList[j - 1]);
               }
          }
     }

     //   Return
     return formulaList;
}


std::vector<Formula*> Formula::reduce(
          std::vector<Formula*> formulaList,
          const std::vector<std::vector<double> >& pointList)
{
     //   Fill a map in which all formulas are discriminated by their key.
     //   For a each key, we keep a single formula (the smallest).
     std::vector<Formula*> rFormulaList;
     std::map<std::vector<unsigned int>, Formula*> rMap;
     for (unsigned int i = 0; i < formulaList.size(); ++i)
     {
          std::vector<unsigned int> key;
          
          try { key = formulaList[i]->getKey(pointList); }
          catch (FormulaException& e) { continue; }
          
          std::map<std::vector<unsigned int>, Formula*>::iterator it;
          it = rMap.find(key);
          if ((it == rMap.end())
                    || (formulaList[i]->size() < (it->second)->size()))
          {
               rMap[key] = formulaList[i];
          }
     }
     
     
     //   Retrieve the formulas of 'rMap' and store them in 'rFormulaList'
     //   before returning it
     std::map<std::vector<unsigned int>, Formula*>::iterator it, end;
     for (it = rMap.begin(), end = rMap.end(); it != end; ++it)
          rFormulaList.push_back(it->second);          
     return rFormulaList;
}


// ============================================================================
//	Public methods
// ============================================================================
double Formula::operator()(const std::vector<double>& val) const
                                                       throw (FormulaException)
{
     //   Unset all variables and assign new values
     std::map<unsigned int, Variable*>::const_iterator it  = varMap.begin();
     std::map<unsigned int, Variable*>::const_iterator end = varMap.end();
     for (; it != end; ++it)
     {
          //   Retrieve the rank of the current variable
          unsigned int r = (it->second)->getRank();
          
          //   Unset the variable
          (it->second)->unset();
          
          //   Assignment
          if (r < val.size()) { (it->second)->set(val[r]); }
     }


     //   Evaluate the formula
     std::vector<double> stack;
     for (unsigned int i = 0; i < tokenList.size(); ++i)
     {
          //   Retrieve the operands from the stack
          std::vector<double> operands;
          for (unsigned int j = 0; j < tokenList[i]->getArity(); ++j)
          {
               operands.push_back(stack.back());
               stack.pop_back();
          }
          
          
          //   Evaluate the token and push its value on the stack
          try { stack.push_back((*tokenList[i])(operands)); }
          catch (TokenException& e) { throw FormulaException(e.what()); }
     }


     //   Return
     assert(stack.size() == 1);
     return stack[0];
}


std::string Formula::getRPNStr() const
{
     if (tokenList.empty()) { return ""; }
     
     
     std::string str;
     str += tokenList[0]->getSymbol();
     for (unsigned int i = 1; i < tokenList.size(); ++i)
          str += " " + tokenList[i]->getSymbol();
     
     return str;
}


// ============================================================================
//	Private methods
// ============================================================================
std::vector<Token*> Formula::getTokensFromStr(std::string str) const
                                                       throw (TokenException)
{
     std::vector<Token*> tokenList;
     std::map<std::string, Token*> tokenMap;
     for (unsigned int i = 0; i < str.size(); ++i)
     {
          //   Skip white spaces and new line
          while (((str[i] == ' ') || (str[i] == '\n')) && (i < str.size()))
               ++i;
          
          //   Get string
          std::string symb;
          while ((str[i] != ' ') && (str[i] != '\n') && (i < str.size()))
               symb += str[i++];
          
          //   Load 'tmp' token if not empty
          if (symb != "")
          {
               Token* token;
               
               //   Search for the token symbol in 'tokenMap'
               std::map<std::string, Token*>::iterator it;
               it = tokenMap.find(symb);
               
               //   The token has already been generated during this loop
               if (it != tokenMap.end()) { token = it->second; }
               
               //   The token has to be generated and added to the map
               else
               {
                    token = Token::getToken(symb);
                    tokenMap[symb] = token;
               }
               
               //   Add the token
               tokenList.push_back(token);
          }
     }
     
     return tokenList;
}


void Formula::buildVarMap() throw (FormulaException)
{
     #ifndef NDEBUG
     for (unsigned int i = 0; i < tokenList.size(); ++i)
          assert(tokenList[i] != 0);
     #endif


     if (!isValid())
     {
          std::string msg;
          msg += "This formula is not valid!\n";
          
          throw FormulaException(msg);
     }
     
     
     //   Identify the set of Variables in 'tokenList'
     varMap.clear();
     for (unsigned int i = 0; i < tokenList.size(); ++i)
     {
          //   Variable case
          Variable* var = dynamic_cast<Variable*>(tokenList[i]);
          if (var != NULL) { varMap[var->getRank()] = var; }
     }
}


void Formula::deleteTokens()
{
     std::set<Token*> tokenSet;
     for (unsigned int i = 0; i < tokenList.size(); ++i)
          if (tokenList[i]) { tokenSet.insert(tokenList[i]); }
          
     std::set<Token*>::iterator it  = tokenSet.begin();
     std::set<Token*>::iterator end = tokenSet.end();
     for (; it != end; ++it) { delete *it; }
     
     tokenList.clear();
}


bool Formula::isValid()
{
     //   Ensure that tokens sharing the same symbol are sharing the same
     //   token instance
     std::map<std::string, Token*> symbMap;
     for (unsigned int i = 0; i < tokenList.size(); ++i)
     {
          std::map<std::string, Token*>::iterator it;
          it = symbMap.find(tokenList[i]->getSymbol());

          if (it == symbMap.end())
               symbMap[tokenList[i]->getSymbol()] = tokenList[i];

          else if (it->second != tokenList[i]) { return false; }               
     }
     
     
     //   Ensure that the number of outputs is equal to one 
     unsigned int nbOutputs = 0;
     for (unsigned int i = 0; i < tokenList.size(); ++i)
     {
          nbOutputs += (1 - tokenList[i]->getArity());
          if (nbOutputs < 1) { return false; }
     }
     
     return (nbOutputs == 1);
}


std::vector<unsigned int> Formula::getKey(
          const std::vector<std::vector<double> >& pointList) const
                                                       throw (FormulaException)
{
     //   Order the points with respect of this formula and retaining their
     //   rank     
     std::vector<RankValuePair> list;
     for (unsigned int i = 0; i < pointList.size(); ++i)
          list.push_back(RankValuePair(i, (*this)(pointList[i])));
     std::stable_sort(list.begin(), list.end());
     
     
     //   Build the key as the rank of each point
     std::vector<unsigned int> key;
     for (unsigned int i = 0; i < list.size(); ++i)
          key.push_back(list[i].rank);


     //   Return
     return key;          
}
