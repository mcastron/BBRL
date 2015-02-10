
#include "OptimalAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
OptimalAgent::OptimalAgent(std::istream& is) : Agent()
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


OptimalAgent::OptimalAgent(double uGamma_) :
		Agent("Optimal Agent"), uGamma(uGamma_)
{
	assert((uGamma > 0.0) && (uGamma < 1.0));
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


// ===========================================================================
//	Public methods
// ===========================================================================
int OptimalAgent::getAction(int xt) const throw (AgentException)
{	
	assert((xt >= 0) && (xt < (int) nX));
	assert(Q.size() == (getMDP()->getNbStates() * getMDP()->getNbActions()));
	
	
	vector<vector<double>::const_iterator> maxList;
	maxList = utils::search::max<vector<double> >(
			(Q.begin() + nU*xt), (Q.begin() + nU*xt + nU));
	
	assert(!maxList.empty());

	int maxI = RandomGen::randIntRange_Uniform(0, (maxList.size() - 1));
	return (maxList[maxI] - (Q.begin() + nU*xt));
}


void OptimalAgent::reset() throw (AgentException)
{
	nX = getMDP()->getNbStates();
	nU = getMDP()->getNbActions();
	
	double gamma = getGamma();
	if (gamma == 1.0) { gamma = uGamma; }
	Q = getMDP()->qIteration(gamma, getT());
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void OptimalAgent::freeData()
{
     Q.clear();
     
     
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void OptimalAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	os << OptimalAgent::toString() << "\n";
	os << 1 << "\n";
	os << uGamma << "\n";
}


void OptimalAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != OptimalAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	
	int i = 0;
	
	
	//	'uGamma'
	if (!getline(is, tmp)) { throwEOFMsg("uGamma"); }
	uGamma = atof(tmp.c_str());
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
#ifndef NDEBUG
void OptimalAgent::checkIntegrity() const
{
	assert((uGamma > 0.0) && (uGamma < 1.0));
}
#endif
