
#include "OPPSCSAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
OPPSCSAgent::OPPSCSAgent(std::istream& is) : Agent(), agent(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


OPPSCSAgent::OPPSCSAgent(
		unsigned int n_, double c_,
		AgentFactory* agentFactory_,
		double gamma_, unsigned int T_) :
			Agent(),
			agent(0), n(n_), c(c_), agentFactory(agentFactory_),
			gamma(gamma_), T(T_)
{
	stringstream sstr;
	sstr << "OPPS-CS(no learning)";
	setName(sstr.str());


	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


OPPSCSAgent::~OPPSCSAgent()
{
	if (agent) { delete agent; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int OPPSCSAgent::getAction(int xt) const throw (MDPException)
{
	assert(agent);
	return agent->getAction(xt);
}


void OPPSCSAgent::learnOnline(int x, int u, int y, double r)
											throw (MDPException)
{
	assert(agent);
	agent->learnOnline(x, u, y, r);
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void OPPSCSAgent::reset() throw (MDPException)
{
	assert(agent);	
	agent->reset();
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void OPPSCSAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << getClassName() << "\n";
	os << 1 << "\n";
	
	
	//	'agent'
	if (agent) { os << 0 << "\n"; }
	
	else
	{
		os << 1 << "\n";
		os << agent->getClassName() << "\n";
		stringstream agentStream;
		agent->serialize(agentStream);
		
		os << agentStream.str().length() << "\n";
		copy(istreambuf_iterator<char>(agentStream),
			istreambuf_iterator<char>(),
			ostreambuf_iterator<char>(os));
	}
}


void OPPSCSAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != getClassName())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;


	//	'agent'
	if (!getline(is, tmp)) { throwEOFMsg("agent"); }
	bool hasAgent = atoi(tmp.c_str());
	
	if (hasAgent)
	{
		if (!getline(is, tmp)) { throwEOFMsg("agent"); }
		unsigned int agentStreamLength = atoi(tmp.c_str());
	
		stringstream agentStream;
		string tmp;
		tmp.resize(agentStreamLength);
		is.read(&tmp[0], agentStreamLength);
		mdpStream << tmp;
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
void OPPSCSAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
											throw (AgentException)
{	
	agentFactory->init(mdpDistrib);
	dds::opps::details::OPPSCS* OPPSCS;
	OPPSCS = new dds::opps::details::
			OPPSCS(c, agentFactory, mdpDistrib, gamma, T);

	vector<double> s = OPPSCS->run(n);
	agent = agentFactory->get(s);
	
	stringstream sstr;
	sstr << "OPPS-CS(" << mdpDistrib->getShortName() << ", ";
	sstr << n << ", " << c << ")";
	sstr << " (" << agent->getName() << ")"; 
	setName(sstr.str());

	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


#ifndef NDEBUG
void OPPSCSAgent::checkIntegrity() const
{
	assert(!agent && (n > 0));
	assert(!agent && (c >= 0.0));
	assert(!agent && agentFactory);
	assert(!agent && (gamma > 0.0) && (gamma <= 1.0));
	assert(!agent && (T > 0));
}
#endif
