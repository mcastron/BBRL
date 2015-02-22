
#include "IExperiment.h"

using namespace std;


// ===========================================================================
//	Public Constructors/Destructor
// ===========================================================================
template<typename AgentType, typename MDPType, typename SimulationRecordType>
IExperiment<AgentType, MDPType, SimulationRecordType>::
		IExperiment(	string name_,
					vector<MDPType*>& mdpList_,
					unsigned int nbSimPerMDP_,
					bool saveTraj_) :
						Serializable(),
						mdpList(mdpList_), nbSimPerMDP(nbSimPerMDP_),
						timeElapsed(0.0), saveTraj(saveTraj_), nextMDP(0),
						name(name_)
{
	assert(!mdpList.empty());
	for (unsigned int i = 0; i < mdpList.size(); ++i) { assert(mdpList[i]); }
	assert(nbSimPerMDP > 0);
	
	
	//	'simRecList' & 'rList'
	if (saveTraj) { simRecList.resize(mdpList.size()*nbSimPerMDP); }
     else          { rList.resize(mdpList.size()*nbSimPerMDP);      }
	
	
	//	Clear
	clear();
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


template<typename AgentType, typename MDPType, typename SimulationRecordType>
IExperiment<AgentType, MDPType, SimulationRecordType>::~IExperiment()
{
	for (unsigned int i = 0; i < mdpList.size(); ++i)
		if (mdpList[i]) { delete mdpList[i]; }

	for (unsigned int i = 0; i < simRecList.size(); ++i)
		if (simRecList[i]) { delete simRecList[i]; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
template<typename AgentType, typename MDPType, typename SimulationRecordType>
void IExperiment<AgentType, MDPType, SimulationRecordType>::
		run(AgentType* agent, double timeLimit_, unsigned int nbThreads)
{
	//	Adjust the number of Threads with respect to the number of MDPs
	//	and set the time limit.
	nbThreads = (nbThreads > mdpList.size()) ? mdpList.size() : nbThreads;
	timeLimit = timeLimit_;

	
	//	1 Thread case
	if (nbThreads == 1)
	{
	     //	Perform the simulations
	     chrono.restart();
	     while (nextMDP < mdpList.size())
	     {
			for (unsigned int i = 0; i < nbSimPerMDP; ++i)
			{
                    //   Simulate
                    SimulationRecordType* simRec
                              = new SimulationRecordType(
								simulate(agent, mdpList[nextMDP]));

                    //   Save the results
                    if (saveTraj)
                         simRecList[nbSimPerMDP*nextMDP + i] = simRec;
                    
                    else
                    {
                         rList[nbSimPerMDP*nextMDP + i]
                                   = simRec->getRewardList();

                         delete simRec;
                    }
				
				//	Check integrity
				#ifndef NDEBUG
				checkIntegrity();
				#endif
			}
			
			
			//	Next MDP to simulate
			++nextMDP;
			
			
			//	Time limit check
			if ((timeLimit > 0) && (chrono.get() >= timeLimit)) { break; }
	     }
	     
	     //	Stop the chrono and compute the time elapsed
	     timeElapsed += chrono.get();
	}
	
	
	//	Multi-Thread case
	else
	{		
		//	Create the SimulationThreads and a unique copy of 'agent'
		//	for each one.
		vector<SimulationThread*> simThreadList;
		for (unsigned int i = 0; i < nbThreads; ++i)
		{
			simThreadList.push_back(new SimulationThread(i, this));
			copyAgentList.push_back(agent->clone());
		}


		//	Start the SimulationThreads
		chrono.restart();
		
		for (unsigned int i = 0; i < nbThreads; ++i)
			simThreadList[i]->start();
		

		//	Join all the SimulationThreads
		for (unsigned int i = 0; i < nbThreads; ++i)
			simThreadList[i]->join();
		
		
		//	Stop the chrono and compute the time elapsed
		timeElapsed += chrono.get();
		
		
		//	Delete the SimulationThreads and the copies of 'agent'
		for (unsigned int i = 0; i < nbThreads; ++i)
		{
			delete simThreadList[i];
			delete copyAgentList[i];
		}
		simThreadList.clear();
		copyAgentList.clear();
		
		
		//	Check integrity
		#ifndef NDEBUG
		checkIntegrity();
		#endif
	}
}


template<typename AgentType, typename MDPType, typename SimulationRecordType>
void IExperiment<AgentType, MDPType, SimulationRecordType>::clear()
{
	timeElapsed = 0.0;	
	
	for (unsigned int i = 0; i < simRecList.size(); ++i)
		if (simRecList[i]) { delete simRecList[i]; simRecList[i] = 0; }

     for (unsigned int i = 0; i < rList.size(); ++i) { rList[i].clear(); }

	nextMDP = 0;
	pthread_mutex_init(&m_nextMDP, NULL);
}


template<typename AgentType, typename MDPType, typename SimulationRecordType>
vector<double> IExperiment<AgentType, MDPType, SimulationRecordType>::
		computeDSRList(double gamma) const
{
	assert((gamma == -1.0) || ((gamma > 0.0) && (gamma <= 1.0)));

	
	vector<double> dsrList;
	
	if (saveTraj)
	{
     	for (unsigned int i = 0; i < simRecList.size(); ++i)
     	{
     		if (simRecList[i])
     			dsrList.push_back(simRecList[i]->computeDSR(gamma));
     	}
	}
	
	else
	{
          for (unsigned int i = 0; i < rList.size(); ++i)
          {
               if (!rList[i].empty())
               {
                    double tGamma =
                              ((gamma > 0.0) && (gamma <= 1.0)) ?
                                        gamma : getSimGamma();

                    dsrList.push_back(
                         rl_utils::simulation::computeDSR(rList[i], tGamma));
               }
          }
	}
	
	return dsrList;
}


template<typename AgentType, typename MDPType, typename SimulationRecordType>
void IExperiment<AgentType, MDPType, SimulationRecordType>::
		serialize(ostream& os) const
{
	Serializable::serialize(os);
	
	
	os << IExperiment::toString() << "\n";
	os << (8 + mdpList.size() + simRecList.size()) << "\n";
	
	
	//	'name'
	os << name << "\n";
	
	
	//	'mdpList'
	os << mdpList.size() << "\n";
	for (unsigned int i = 0; i < mdpList.size(); ++i)
	{
		stringstream mdpStream;		
		mdpList[i]->serialize(mdpStream);
		
		os << mdpStream.str().length() << "\n";
		copy(istreambuf_iterator<char>(mdpStream),
			istreambuf_iterator<char>(),
			ostreambuf_iterator<char>(os));
	}
	
	
	//	'nbSimPerMDP' and 'timeElapsed'
	os << nbSimPerMDP << "\n";
	os << timeElapsed << "\n";
	
	
	//  'saveTraj'
	os << (saveTraj ? 1 : 0) << "\n";

	
	//	'simRecList'
	os << simRecList.size() << "\n";
	for (unsigned int i = 0; i < simRecList.size(); ++i)
	{
		if (!simRecList[i]) { os << 0 << "\n"; continue; }
		stringstream simRecStream;		
		simRecList[i]->serialize(simRecStream);
		
		os << simRecStream.str().length() << "\n";
		copy(istreambuf_iterator<char>(simRecStream),
			istreambuf_iterator<char>(),
			ostreambuf_iterator<char>(os));
	}
	
	
	//  'rList'
	os << rList.size() << "\n";
	for (unsigned int i = 0; i < rList.size(); ++i)
	{
	    os << rList[i].size() << "\n";
	    for (unsigned int j = 0; j < rList[i].size(); ++j)
	         os << rList[i][j] << "\n";
	}
	
	
	//	'nextMDP'
	os << nextMDP << "\n";
}


template<typename AgentType, typename MDPType, typename SimulationRecordType>
void IExperiment<AgentType, MDPType, SimulationRecordType>::
		deserialize(istream& is) throw (SerializableException)
{
	Serializable::deserialize(is);
	
	string tmp;
	
	
	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != IExperiment::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	
	int i = 0;
	
	
	//	Clear previous data
	clear();
	
	
	//	'name'
	if (!getline(is, tmp)) { throwEOFMsg("name"); }
	name = tmp;
	++i;
	
	
	//	'mdpList'
	if (!getline(is, tmp)) { throwEOFMsg("mdpList"); }
	unsigned int mdpListSize = atoi(tmp.c_str());
	++i;
	
	for (unsigned int j = 0; j < mdpListSize; ++j)
	{
		if (!getline(is, tmp)) { throwEOFMsg("mdpList"); }
		unsigned int mdpStreamLength = atoi(tmp.c_str());
		
		stringstream mdpStream;
		string tmp;
		tmp.resize(mdpStreamLength);
		is.read(&tmp[0], mdpStreamLength);
		mdpStream << tmp;
		
		mdpList.push_back(dynamic_cast<MDPType*>(
				Serializable::createInstance<MDPType>(mdpStream)));
		++i;
	}
	
	
	//	'nbSimPerMDP'
	if (!getline(is, tmp)) { throwEOFMsg("nbSimPerMDP"); }
	nbSimPerMDP = atoi(tmp.c_str());
	++i;

	
	//	'timeElapsed'
	if (!getline(is, tmp)) { throwEOFMsg("timeElapsed"); }
	timeElapsed = atof(tmp.c_str());
	++i;


     //   'saveTraj'
     if (!getline(is, tmp)) { throwEOFMsg("saveTraj"); }
	saveTraj = (atoi(tmp.c_str()) != 0);
	++i;


	//	'simRecList'
	for (unsigned int j = 0; j < simRecList.size(); ++j)
	    if (simRecList[j]) { delete simRecList[j]; }
     simRecList.clear();
	
	if (!getline(is, tmp)) { throwEOFMsg("simRecList"); }
	unsigned int simRecListSize = atoi(tmp.c_str());
	++i;
	
	for (unsigned int j = 0; j < simRecListSize; ++j)
	{
		if (!getline(is, tmp)) { throwEOFMsg("simRecList"); }
		unsigned int simRecStreamLength = atoi(tmp.c_str());
		
		if (simRecStreamLength == 0)
		{
			simRecList.push_back(0);
			++i;
			continue;
		}
		
		stringstream simRecStream;
		string tmp;
		tmp.resize(simRecStreamLength);
		is.read(&tmp[0], simRecStreamLength);
		simRecStream << tmp;
		
		simRecList.push_back(dynamic_cast<SimulationRecordType*>(
			Serializable::createInstance<SimulationRecordType>(
					simRecStream)));
		++i;
	}


	//  'rList'	
	rList.clear();
	if (!getline(is, tmp)) { throwEOFMsg("rList"); }
	unsigned int rListSize = atoi(tmp.c_str());
	
     for (unsigned int j = 0; j < rListSize; ++j)
     {
          if (!getline(is, tmp)) { throwEOFMsg("rList"); }
          unsigned int rListJSize = atoi(tmp.c_str());
          
          rList.push_back(std::vector<double>());
          for (unsigned int k = 0; k < rListJSize; ++k)
          {
               if (!getline(is, tmp)) { throwEOFMsg("rList"); }
               double r = atof(tmp.c_str());
               
               rList.back().push_back(r);
          }
     }
	++i;
	
	
	//	'nextMDP'
	if (!getline(is, tmp)) { throwEOFMsg("nextMDP"); }
	nextMDP = atoi(tmp.c_str());
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
template<typename AgentType, typename MDPType, typename SimulationRecordType>
void IExperiment<AgentType, MDPType, SimulationRecordType>::
		checkIntegrity() const
{
	assert(!mdpList.empty());
	for (unsigned int i = 0; i < mdpList.size(); ++i) { assert(mdpList[i]); }
	
	assert(nbSimPerMDP > 0);
	assert(timeElapsed >= 0.0);
	
	assert(saveTraj  && simRecList.size() == mdpList.size()*nbSimPerMDP);
	assert(!saveTraj && rList.size()      == mdpList.size()*nbSimPerMDP);
}
#endif


// ===========================================================================
//	Classes methods
// ===========================================================================
template<typename AgentType, typename MDPType, typename SimulationRecordType>
void* IExperiment<AgentType, MDPType, SimulationRecordType>::
		SimulationThread::run()
{	
     for (;;)
     {
          //	Reserve the MDPs to simulate
          pthread_mutex_lock(&(exp->m_nextMDP));
          
          //	No more simulation to perform
          if (exp->nextMDP >= exp->mdpList.size())
          {
               pthread_mutex_unlock(&(exp->m_nextMDP));
               break;
          }
          unsigned int t = (exp->nextMDP++);
          
          pthread_mutex_unlock(&(exp->m_nextMDP));		
          
          
          //	Perform the simulations
          for (unsigned int i = 0; i < exp->nbSimPerMDP; ++i)
          {
               SimulationRecordType* simRec
                         = new SimulationRecordType(
                                   exp->simulate(exp->copyAgentList[ID],
                                   exp->mdpList[t]));
               
               if (exp->saveTraj)
                    exp->simRecList[exp->nbSimPerMDP*t + i] = simRec;
               
               else
               {
                    exp->rList[exp->nbSimPerMDP*t + i]
                              = simRec->getRewardList();

                    delete simRec;
               }
          }
          
          
          //	Time limit check
          if ((exp->timeLimit > 0)
                    && ((exp->chrono).get() >= exp->timeLimit)) { break; }
     }
     
     return 0;
}
