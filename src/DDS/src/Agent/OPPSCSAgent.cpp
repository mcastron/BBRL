
#include "OPPSCSAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
OPPSCSAgent::OPPSCSAgent(std::istream& is) :
          Agent(), agent(0), agentFactory(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


OPPSCSAgent::OPPSCSAgent(unsigned int n_, double c_,
                         AgentFactory* agentFactory_,
					double gamma_, unsigned int T_) :
					     Agent(),
					     agent(0),
					     n(n_), c(c_), agentFactory(agentFactory_),
					     gamma(gamma_), T(T_)
{
     assert(n > 0);
     assert(agentFactory);
     assert((gamma > 0.0) && (gamma <= 1.0));
     assert(T > 0);

	
	stringstream sstr;
	sstr << "OPPS-CS (no agent)";
	setName(sstr.str());


	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


OPPSCSAgent::~OPPSCSAgent()
{
     if (agent)        { delete agent;        }
     if (agentFactory) { delete agentFactory; }
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
}


void OPPSCSAgent::reset() throw (MDPException)
{
	if (agent)
	{
	    agent->setMDP(getMDP(), getGamma(), getT());
	    agent->reset();
	}
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void OPPSCSAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << OPPSCSAgent::toString() << "\n";
	os << 5 << "\n";
	
	
	//	'agent'
	if (!agent) { os << 0 << "\n"; }
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
	
	
	//  'c'
	os << c << "\n";
	
	
	//  'agentFactory'
	os << agentFactory->getClassName() << "\n";

     stringstream agentFactoryStream;
     agentFactory->serialize(agentFactoryStream);

     os << agentFactoryStream.str().length() << "\n";
     copy(istreambuf_iterator<char>(agentFactoryStream),
	         istreambuf_iterator<char>(),
	         ostreambuf_iterator<char>(os));
	
	
	//  'gamma'
	os << gamma << "\n";
	
	
	//  'T'
	os << T << "\n";
}


void OPPSCSAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != OPPSCSAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int ni = atoi(tmp.c_str());

	int i = 0;
	
	
	//   'agent'
	if (agent) { delete agent; agent = 0; }
	
	if (!getline(is, tmp)) { throwEOFMsg("agent"); }
	bool hasAgent = (tmp != "0");
	
	if (hasAgent)
	{
        	if (!getline(is, tmp)) { throwEOFMsg("agent"); }
        	string agentClassName = tmp;
        	
        	if (!getline(is, tmp)) { throwEOFMsg("agent"); }
        	unsigned int agentStreamLength = atoi(tmp.c_str());
        	
        	stringstream agentStream;
        	string tmp;
        	tmp.resize(agentStreamLength);
        	is.read(&tmp[0], agentStreamLength);
        	agentStream << tmp;
        	
        	agent = dynamic_cast<Agent*>(
        	      Serializable::createInstance(agentClassName, agentStream));
	}
	++i;
	
	
	//	'c'
	if (!getline(is, tmp)) { throwEOFMsg("c"); }
	c = atof(tmp.c_str());
	++i;
	
	
	//  'agentFactory'
	if (agentFactory) { delete agentFactory; agentFactory = 0; }
	
	if (!getline(is, tmp)) { throwEOFMsg("agentFactory"); }
     string agentFactoryClassName = tmp;
        	
     if (!getline(is, tmp)) { throwEOFMsg("agentFactory"); }
     unsigned int agentFactoryStreamLength = atoi(tmp.c_str());
        	
     stringstream agentFactoryStream;
     tmp.resize(agentFactoryStreamLength);
     is.read(&tmp[0], agentFactoryStreamLength);
     agentFactoryStream << tmp;
        	
     agentFactory = dynamic_cast<AgentFactory*>(
               Serializable::createInstance(
                         agentFactoryClassName, agentFactoryStream));
	++i;
	
	
	//	'gamma'
	if (!getline(is, tmp)) { throwEOFMsg("gamma"); }
	gamma = atof(tmp.c_str());
	++i;
	
	
	//	'T'
	if (!getline(is, tmp)) { throwEOFMsg("T"); }
	T = atoi(tmp.c_str());
	++i;
	
	
	//	Number of parameters check
	if (ni != i)
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
     assert(agent == 0);
     
     
     //   Create a UCT
     agentFactory->init(mdpDistrib);
     opps::UCT uct(c, agentFactory, mdpDistrib, gamma, T);
     
     
     //   Run the UCT
     vector<double> paramList = uct.run(n);
     
     
     //   Build the discovered strategy
     agent = agentFactory->get(paramList);
     stringstream sstr;
	sstr << "OPPS-CS (" << agent->getName() << ")";
	setName(sstr.str());     
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


#ifndef NDEBUG
void OPPSCSAgent::checkIntegrity() const
{
     assert(agentFactory);     
     assert((gamma > 0.0) && (gamma <= 1.0));
     assert(T > 0);
}
#endif