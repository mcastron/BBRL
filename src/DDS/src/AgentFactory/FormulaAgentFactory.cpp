
#include "FormulaAgentFactory.h"

using namespace std;
using namespace utils::formula;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
dds::FormulaAgentFactory::FormulaAgentFactory(std::istream& is) :
          AgentFactory(), mdpDistrib(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


dds::FormulaAgentFactory::FormulaAgentFactory(
          double minW_, double maxW_, double accW_,
          vector<string>& varNameList_) :
               minW(minW_), maxW(maxW_), accW(accW_),
               varNameList(varNameList_), mdpDistrib(0)
{
	assert((minW < maxW) && (acc > 0.0) && !varNameList.empty());
}


// ===========================================================================
//	Public methods
// ===========================================================================
void dds::FormulaAgentFactory::init(const dds::MDPDistribution* mdpDistrib_)
									throw (AgentFactoryException)
{
     boundsList.clear();
     splitAccList.clear();
     
     for (unsigned int i = 0; i < varNameList.size(); ++i)
     {
          boundsList.push_back(pair<double, double>(minW, maxW));
          splitAccList.push_back(accW);
     }

     mdpDistrib = mdpDistrib_;
}


dds::Agent* dds::FormulaAgentFactory::get(const vector<double>& paramList)
									const throw (AgentFactoryException)
{
	assert(paramList.size() == varNameList.size());
	

     //   Build the formula
     stringstream sstr;
     sstr << paramList[0] << " X0 MUL2";
     for (unsigned int i = 1; i < paramList.size(); ++i)
          sstr << " " << paramList[i] << " X" << i << " MUL2";
     sstr << " ADD" << paramList.size() << "\n";
     
     cout << sstr.str() << "\n";
     
     FormulaAgent* agent;
     try
     {
          agent = new FormulaAgent(new Formula(sstr.str()), varNameList);
          agent->learnOffline(mdpDistrib);
     }     
     catch (FormulaException& e) { throw AgentFactoryException(e.what()); }
     catch (AgentException& e)   { throw AgentFactoryException(e.what()); }


	return agent;
}


void dds::FormulaAgentFactory::serialize(ostream& os) const
{
	AgentFactory::serialize(os);
	
	
	os << FormulaAgentFactory::toString() << "\n";
	os << 4 << "\n";
	
	
	//  'minW', 'maxW' & 'accW'
	os << minW << "\n";
	os << maxW << "\n";
	os << accW << "\n";
	
	
	//  'varNameList'
	os << varNameList.size() << "\n";
	for (unsigned int i = 0; i < varNameList.size(); ++i)
          os << varNameList[i] << "\n";
}


void dds::FormulaAgentFactory::deserialize(istream& is)
                                                  throw (SerializableException)
{
	AgentFactory::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != FormulaAgentFactory::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
     //	'minW'
	if (!getline(is, tmp)) { throwEOFMsg("minW"); }
	minW = atof(tmp.c_str());
	++i;


     //	'maxW'
	if (!getline(is, tmp)) { throwEOFMsg("maxW"); }
	maxW = atof(tmp.c_str());
	++i;


     //	'accW'
	if (!getline(is, tmp)) { throwEOFMsg("accW"); }
	accW = atof(tmp.c_str());
	++i;
	
	
	//  'varNameList'
	varNameList.clear();
	if (!getline(is, tmp)) { throwEOFMsg("varNameList"); }
	unsigned int varNameListSize = atoi(tmp.c_str());
	
	for (unsigned int i = 0; i < varNameList.size(); ++i)
	{
	    if (!getline(is, tmp)) { throwEOFMsg("varNameList"); }
	    varNameList.push_back(tmp);
	}
	++i;
	
	
	//  'mdpDistrib'
	mdpDistrib = 0;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
