
#include "FormulaAgent.h"

using namespace std;
using namespace dds;
using namespace utils;
using namespace utils::formula;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
FormulaAgent::FormulaAgent(std::istream& is) : Agent(), f(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


FormulaAgent::FormulaAgent(Formula* f_) : f(f_)
{
     assert(f_);
     
     
     setName("Formula agent(" + f->getRPNStr() +")");     
     
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


FormulaAgent::~FormulaAgent()
{
     if (f) { delete f; }
     
     for (unsigned int i = 0; i < varList.size(); ++i)
          if (varList[i]) { delete varList[i]; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int FormulaAgent::getAction(int xt) const throw (AgentException)
{
     unsigned int nU = getMDP()->getNbActions();
     
     
     //   Compute the Q-function with formula 'f'
	std::vector<double> Qxt;
	for (unsigned int u = 0; u < nU; ++u)
	{
	     std::vector<double> v;
	     for (unsigned int i = 0; i < varList.size(); ++i)
	     {
               if (varList[i]) { v.push_back((*varList[i])(xt, u)); }
               else            { v.push_back(0.0);                  }
	     }


	     try                         { Qxt.push_back((*f)(v));         }
	     catch (FormulaException& e) { throw AgentException(e.what()); }
	}
	
	
	//  Select the action maximizing the computed Q-function
	vector<vector<double>::const_iterator> maxList;
	maxList = utils::search::max<vector<double> >(Qxt.begin(), Qxt.end());
	
	assert(!maxList.empty());

	int maxI = RandomGen::randIntRange_Uniform(0, (maxList.size() - 1));
	return (maxList[maxI] - Qxt.begin());
}


void FormulaAgent::learnOnline(int x, int u, int y, double r)
											throw (AgentException)
{
     for (unsigned int i = 0; i < varList.size(); ++i)
          if (varList[i]) { varList[i]->update(x, u, y, r); }	    
}


void FormulaAgent::reset() throw (AgentException)
{
     //   Reset the FVariables
     for (unsigned int i = 0; i < varList.size(); ++i)
          if (varList[i]) { varList[i]->reset(getMDP(), getGamma(), getT()); }

	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void FormulaAgent::freeData()
{
     for (unsigned int i = 0; i < varList.size(); ++i)
          if (varList[i]) { varList[i]->freeData(); }
     
     
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void FormulaAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << FormulaAgent::toString() << "\n";
	os << 2 << "\n";
	
	
     //   'f'
	os << f->getRPNStr() << "\n";
     
     
     //   'varList'
     os << varList.size() << "\n";
     for (unsigned int i = 0; i < varList.size(); ++i)
     {
          if (!varList[i]) { os << 0 << "\n"; }
          else
          {
               os << 1 << "\n";
               os << varList[i]->getClassName() << "\n";
               
               stringstream varStream;
               varList[i]->serialize(varStream);
               
               os << varStream.str().length() << "\n";
               copy(istreambuf_iterator<char>(varStream),
                    istreambuf_iterator<char>(),
             		ostreambuf_iterator<char>(os));
          }
     }
}


void FormulaAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != FormulaAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
	//  'f'
	if (f) { delete f; }
	
	if (!getline(is, tmp)) { throwEOFMsg("f"); }	
	f = new Formula(tmp);
	++i;
	
	
	//  'varList'
	for (unsigned int i = 0; i < varList.size(); ++i)
	    if (varList[i]) { delete varList[i]; }
     varList.clear();

	if (!getline(is, tmp)) { throwEOFMsg("varList"); }
     unsigned int varListSize = atoi(tmp.c_str());
     
	for (unsigned int i = 0; i < varListSize; ++i)
	{
	     if (!getline(is, tmp)) { throwEOFMsg("var"); }
	     bool hasVar = (atoi(tmp.c_str()) != 0);
	     if (!hasVar) { varList.push_back(0); continue; }
	     
	     string varClassStr;
	     if (!getline(is, varClassStr)) { throwEOFMsg("var"); }
	     
          if (!getline(is, tmp)) { throwEOFMsg("var"); }
          unsigned int varStreamLength = atoi(tmp.c_str());
          
          stringstream varStream;
          tmp.resize(varStreamLength);
          is.read(&tmp[0], varStreamLength);
          varStream << tmp;
          
          varList.push_back(dynamic_cast<FVariable*>(
                    Serializable::createInstance(varClassStr, varStream)));
	}
	++i;
		
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


// ===========================================================================
//	Private methods
// ===========================================================================
void FormulaAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
											throw (AgentException)
{
     if (f && f->isVarUsed(0)) { varList.push_back(new QMean());    }
     else                      { varList.push_back(0);              }
     
     if (f && f->isVarUsed(1)) { varList.push_back(new QUniform()); }
     else                      { varList.push_back(0);              }

     if (f && f->isVarUsed(2)) { varList.push_back(new QSelf());    }
     else                      { varList.push_back(0);              }


     for (unsigned int i = 0; i < varList.size(); ++i)
          if (varList[i]) { varList[i]->init(mdpDistrib); }


	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


#ifndef NDEBUG
void FormulaAgent::checkIntegrity() const { assert(f); }
#endif
