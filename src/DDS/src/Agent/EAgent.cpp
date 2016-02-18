
#include "EAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
EAgent::EAgent(std::istream& is) : Agent(), baseAgent(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


EAgent::EAgent(double epsilon_, Agent* baseAgent_) :
		Agent(), epsilon(epsilon_), baseAgent(baseAgent_)
{
     assert((epsilon >= 0.0) && (epsilon <= 1.0));
     assert(baseAgent);
	
	
	stringstream sstr;
	sstr << "e-Agent(e = ";
	sstr << setprecision(ceil(log10(epsilon) + 2)) << epsilon << ", ";
	sstr << baseAgent->getName();
	sstr << ")";
	setName(sstr.str());
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


EAgent::~EAgent()
{
     if (baseAgent) { delete baseAgent; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int EAgent::getAction(int xt) const throw (AgentException)
{	
	assert((xt >= 0) && (xt < (int) getMDP()->getNbStates()));
	
	
	unsigned int nU = getMDP()->getNbActions();
	double f = RandomGen::rand01_Uniform();
	
	//	'random' case
	if (f <= epsilon)
		return RandomGen::randIntRange_Uniform(0, (nU - 1));
	
	//	'agent' case
	else { return baseAgent->getAction(xt); }
}


void EAgent::learnOnline(int x, int u, int y, double r) throw (AgentException)
{
     baseAgent->learnOnline(x, u, y, r);
}


void EAgent::reset() throw (AgentException)
{
	baseAgent->setMDP(getMDP(), getGamma(), getT());
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void EAgent::freeData()
{
     baseAgent->freeData();
     
     
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void EAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	os << EAgent::toString() << "\n";
	os << 2 << "\n";
	os << epsilon << "\n";
	
	
	//	'agent'
	if (!baseAgent) { os << 0 << "\n"; }
	else
	{
	    os << 1 << "\n";
	    os << baseAgent->getClassName() << "\n";

	    stringstream baseAgentStream;
	    baseAgent->serialize(baseAgentStream);
	    
	    os << baseAgentStream.str().length() << "\n";
	    copy(istreambuf_iterator<char>(baseAgentStream),
			istreambuf_iterator<char>(),
			ostreambuf_iterator<char>(os));
	}
}


void EAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != EAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	
	int i = 0;
	

	//	'epsilon'
	if (!getline(is, tmp)) { throwEOFMsg("epsilon"); }
	epsilon = atof(tmp.c_str());
	++i;


	//   'baseAgent'
	if (baseAgent) { delete baseAgent; baseAgent = 0; }
	
	if (!getline(is, tmp)) { throwEOFMsg("baseAgent"); }
	bool hasBaseAgent = (tmp != "0");
	
	if (hasBaseAgent)
	{
        	if (!getline(is, tmp)) { throwEOFMsg("baseAgent"); }
        	string baseAgentClassName = tmp;
        	
        	if (!getline(is, tmp)) { throwEOFMsg("baseAgent"); }
        	unsigned int baseAgentStreamLength = atoi(tmp.c_str());
        	
        	stringstream baseAgentStream;
        	string tmp;
        	tmp.resize(baseAgentStreamLength);
        	is.read(&tmp[0], baseAgentStreamLength);
        	baseAgentStream << tmp;
        	
        	baseAgent = dynamic_cast<Agent*>(
        	      Serializable::createInstance(baseAgentClassName,
        	                                   baseAgentStream));
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
void EAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
                                                       throw (AgentException)
{
     baseAgent->learnOffline(mdpDistrib);
}


#ifndef NDEBUG
void EAgent::checkIntegrity() const
{
     assert((epsilon >= 0.0) && (epsilon <= 1.0));
     assert(baseAgent);
}
#endif
