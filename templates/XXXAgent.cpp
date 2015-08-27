
/* TODO - replace "XXXAgent" the agent class name */


#include "XXXAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
XXXAgent::XXXAgent(std::istream& is) : Agent()
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


XXXAgent::~XXXAgent()
{
     /* TODO - free all allocated data */
}


// ===========================================================================
//	Public methods
// ===========================================================================
int XXXAgent::getAction(int xt) const throw (MDPException)
{
	/* TODO - to implement */
}


void XXXAgent::learnOnline(int x, int u, int y, double r)
											throw (MDPException)
{
	/* TODO - to implement (if needed) */
}


void XXXAgent::reset() throw (MDPException)
{
	/* TODO - to implement (if needed) */
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void XXXAgent::freeData()
{
	/* TODO - to implement (if needed) */
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void XXXAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << XXXAgent::toString() << "\n";
	os << /* TODO - insert the number of parameters */ << "\n";
	
	
	/* TODO - write in 'os' the parameters to store */
}


void XXXAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != /* TODO - insert class name here */::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
	/* TODO - retrieve the parameters from 'is', and increment 'i' for each */
		
	
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
void XXXAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
											throw (AgentException)
{
	/* TODO - To implement (if needed) */
		
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


#ifndef NDEBUG
void XXXAgent::checkIntegrity() const
{
	/* TODO - To implement (if needed) */
}
#endif
