
#include "OPPSDSAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
OPPSDSAgent::OPPSDSAgent(std::istream& is) : Agent(), agent(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


OPPSDSAgent::OPPSDSAgent(unsigned int n_, double c_,
                         const vector<Agent*>& strategyList_,
					double gamma_, unsigned int T_) :
					     Agent(),
					     agent(0),
					     n(n_), c(c_), strategyList(strategyList_),
					     gamma(gamma_), T(T_)
{
     assert(n > 0);
     assert(!strategyList.empty());
     assert((gamma > 0.0) && (gamma <= 1.0));
     assert(T > 0);

	
	stringstream sstr;
	sstr << "OPPS-DS (no agent)";
	setName(sstr.str());


	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


OPPSDSAgent::~OPPSDSAgent()
{
     if (agent) { delete agent; }
     for (unsigned int i = 0; i < strategyList.size(); ++i)
          if (strategyList[i]) { delete strategyList[i]; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int OPPSDSAgent::getAction(int xt) const throw (AgentException)
{
	assert(agent);


	return agent->getAction(xt);
}


void OPPSDSAgent::learnOnline(int x, int u, int y, double r)
											throw (AgentException)
{
	assert(agent);
	
	
     agent->learnOnline(x, u, y, r);
}


void OPPSDSAgent::reset() throw (AgentException)
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


void OPPSDSAgent::freeData()
{
	if (agent) { agent->freeData(); }
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void OPPSDSAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << OPPSDSAgent::toString() << "\n";
	os << 6 << "\n";
	
	
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
	
	
	//  'n'
	os << n << "\n";
	
	
	//  'c'
	os << c << "\n";
	
	
	//  'strategyList'
	os << strategyList.size() << "\n";
	for (unsigned int i = 0; i < strategyList.size(); ++i)
	{
	    os << strategyList[i]->getClassName() << "\n";
	    
	    stringstream agentStream;
	    strategyList[i]->serialize(agentStream);
	    
	    os << agentStream.str().length() << "\n";
	    copy(istreambuf_iterator<char>(agentStream),
			istreambuf_iterator<char>(),
			ostreambuf_iterator<char>(os));
	}
	
	
	//  'gamma'
	os << gamma << "\n";
	
	
	//  'T'
	os << T << "\n";
}


void OPPSDSAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != OPPSDSAgent::toString())
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
	
	
	//	'n'
	if (!getline(is, tmp)) { throwEOFMsg("n"); }
	n = atoi(tmp.c_str());
	++i;
	
	
	//	'c'
	if (!getline(is, tmp)) { throwEOFMsg("c"); }
	c = atof(tmp.c_str());
	++i;
	
	
	//  'strategyList'
	if (!getline(is, tmp)) { throwEOFMsg("strategyList"); }
	unsigned int strategyListSize = atoi(tmp.c_str());
	
	for (unsigned int j = 0; j < strategyListSize; ++j)
	{
		if (!getline(is, tmp)) { throwEOFMsg("strategyList"); }
		string strategyClassName = tmp;
		
		if (!getline(is, tmp)) { throwEOFMsg("strategyList"); }
		unsigned int strategyStreamLength = atoi(tmp.c_str());
		
		stringstream strategyStream;
		string tmp;
		tmp.resize(strategyStreamLength);
		is.read(&tmp[0], strategyStreamLength);
		strategyStream << tmp;
		
		strategyList.push_back(dynamic_cast<Agent*>(
				Serializable::createInstance(
                              strategyClassName, strategyStream)));
	}
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
void OPPSDSAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
											throw (AgentException)
{
     assert(agent == 0);


     //   Offline learning for the agents
     for (unsigned int i = 0; i < strategyList.size(); ++i)
          strategyList[i]->learnOffline(mdpDistrib);
     
     
     //   Create a UCB1
     opps::UCB1 ucb1(c, strategyList, mdpDistrib, gamma, T);
     
     
     //   Run the UCB1
     int s;
     try { s = ucb1.run(n); }
     catch (std::exception& e) { throw AgentException(e.what()); }
     
     
     //   No agent found (the UCB1 discarded all submitted strategies)
     if (s == -1)
     {
          string msg;
          msg += "Unable to find a valid Agent in the considered set!\n";
          
          throw AgentException(msg.c_str());
     }
     
     
     //   Retrieve the selected strategy
     agent = strategyList[s]->clone();
     stringstream sstr;
	sstr << "OPPS-DS (" << agent->getName() << ", ";
	sstr << mdpDistrib->getShortName() << ")";
	setName(sstr.str());
	
	
	//  Delete all unused strategies
	for (unsigned int i = 0; i < strategyList.size(); ++i)
	    if (strategyList[i]) { delete strategyList[i]; }
	strategyList.clear();

	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


#ifndef NDEBUG
void OPPSDSAgent::checkIntegrity() const
{
     assert(!strategyList.empty());
     for (unsigned int i = 0; i < strategyList.size(); ++i)
          assert(strategyList[i]);
     
     assert((gamma > 0.0) && (gamma <= 1.0));
     assert(T > 0);
}
#endif
