
#include "RandomAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
RandomAgent::RandomAgent(std::istream& is) : Agent()
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


RandomAgent::RandomAgent() : Agent("Random") {}


// ===========================================================================
//	Public methods
// ===========================================================================
int RandomAgent::getAction(int) const throw (AgentException)
{
	//	Draw an random action
	unsigned int nU = getMDP()->getNbActions();
	return RandomGen::randIntRange_Uniform(0, (nU - 1));
}


void RandomAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << RandomAgent::toString() << "\n";
	os << 0 << "\n";
}


void RandomAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != RandomAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
