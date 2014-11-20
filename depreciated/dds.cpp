
#include "Agent/Guez/utils/guez_utils.h"

#include "Agent/Agent.h"
#include "Agent/RandomAgent.h"
#include "Agent/OptimalAgent.h"
#include "Agent/EGreedyAgent.h"
#include "Agent/SoftMaxAgent.h"
#include "Agent/VDBEEGreedyAgent.h"

#include "AgentFactory/AgentFactory.h"

#include "MDP/MDP.h"
#include "MDP/CModel.h"

#include "MDPDistribution/DirMultiDistribution.h"

#include "Experiment/Experiment.h"

#include "dds.h"

using namespace std;


// ===========================================================================
//	Functions
// ===========================================================================
void dds::init()
{
	//	Initialize the RNGs
	utils::RandomGen::setSeed(time(0));
	guez_utils::setSeed(utils::RandomGen::randIntRange_Uniform(0, INT_MAX));
	
	
	//	Agents
	Serializable::checkIn<RandomAgent>(
			&Serializable::createInstance<RandomAgent>);

	Serializable::checkIn<OptimalAgent>(
			&Serializable::createInstance<OptimalAgent>);

	Serializable::checkIn<EGreedyAgent>(
			&Serializable::createInstance<EGreedyAgent>);

	Serializable::checkIn<SoftMaxAgent>(
			&Serializable::createInstance<SoftMaxAgent>);

	Serializable::checkIn<VDBEEGreedyAgent>(
			&Serializable::createInstance<VDBEEGreedyAgent>);


	//	MDP
	Serializable::checkIn<MDP>(
			&Serializable::createInstance<MDP>);

	
	//	Models
	Serializable::checkIn<CModel>(
			&Serializable::createInstance<CModel>);


	//	Distributions
	Serializable::checkIn<DirMultiDistribution>(
			&Serializable::createInstance<DirMultiDistribution>);


	//	Experiments
	Serializable::checkIn<Experiment>(
			&Serializable::createInstance<Experiment>);
}


// ---------------------------------------------------------------------------
//	'simulation' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//	Functions
// ===========================================================================
dds::simulation::SimulationRecord dds::simulation::simulate(
		dds::Agent* agent, dds::MDP* mdp,
		double gamma, unsigned int T,
		bool safeSim)
{
	assert(agent);
	assert(mdp);
	assert((gamma > 0.0) && (gamma <= 1.0));


	//	Create a SimulationRecord
	dds::simulation::SimulationRecord simRec(gamma);
	
	
	//	Reset the agent and the MDP
	mdp->reset();
	
	
	//	Set the MDP as 'unknown'
	if (safeSim) { mdp->setUnknown(); }
	
	
	vector<double> rewardList;
	
	//	Initialization	
	unsigned int x = mdp->getCurrentState();
	agent->setMDP(mdp, gamma, T);
	
	
	//	Simulation
	double cGamma = 1.0;
	for (unsigned int t = 0; t < T; ++t)
	{
		//	Retrieve the action to perform
		unsigned int u = agent->getAction(x);
		
		
		//	Perform the selected action
		unsigned int y;
		double r;
		mdp->perform(u, y, r);
		
		
		//	Store the Transition observed
		simRec.add(Transition(x, u, y, r));
		
		
		//	Online learning of the agent
		agent->learnOnline(x, u, y, r);
		
		
		//	Update of the data
		x = y;
		rewardList.push_back(cGamma * r);
		
		
		//	Update the current discount factor
		cGamma *= gamma;
	}
	
	
	//	Set the MDP as 'known'
	if (safeSim) { mdp->setKnown(); }
	
	
	//	Return
	return simRec;
}


// ---------------------------------------------------------------------------
//	'opps' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//	Details (to ignore)
// ===========================================================================
dds::opps::details::OPPSUCB1::OPPSUCB1(
		double c,
		const vector<Agent*>& strategyList_,
		const MDPDistribution* mdpDistrib_,
		double gamma_, unsigned int T_) :
				utils::UCB1(strategyList_.size(), c),
				strategyList(strategyList_), mdpDistrib(mdpDistrib_),
				gamma(gamma_), T(T_)
{
	assert(!strategyList.empty());
	assert(mdpDistrib);
	assert((gamma > 0.0) && (gamma <= 1.0));
}


double dds::opps::details::OPPSUCB1::drawArm(unsigned int i) const
{
	MDP* mdp = mdpDistrib->draw();
	
	dds::simulation::SimulationRecord simRec;
	simRec = dds::simulation::simulate(strategyList[i], mdp, gamma, T);
	
	delete mdp;
	
	return simRec.computeDSR();
}


dds::opps::details::OPPSUCT::OPPSUCT(
		double c,
		AgentFactory* agentFactory_,
		const MDPDistribution* mdpDistrib_,
		double gamma_, unsigned int T_) :
				utils::UCT(	c,
							agentFactory_->getBounds(),
							agentFactory_->getSplitAcc()),
				agentFactory(agentFactory_), mdpDistrib(mdpDistrib_),
				gamma(gamma_), T(T_)
{
	assert(mdpDistrib);
	assert((gamma > 0.0) && (gamma <= 1.0));
}


double dds::opps::details::OPPSUCT::drawArm(
		const vector<double>& paramList) const
{
	Agent* agent = agentFactory->get(paramList);
	MDP* mdp = mdpDistrib->draw();
		
	dds::simulation::SimulationRecord simRec;
	simRec = dds::simulation::simulate(agent, mdp, gamma, T);
	
	delete agent;
	delete mdp;
	
	return simRec.computeDSR();
}


// ===========================================================================
//	Functions
// ===========================================================================
/*dds::Agent* dds::opps::discreteSearch(
		unsigned int n, double c,
		const vector<dds::Agent*>& strategyList,
		const MDPDistribution* mdpDistrib,
		double gamma, unsigned int T,
		unsigned int refreshFreq, unsigned int backupFreq,
		string outputFile)
{
	double offlineTime;
	double iniN = n;
	dds::opps::details::OPPSUCB1* oppsUCB1;
	oppsUCB1 = new dds::opps::details::OPPSUCB1(
			c, strategyList, mdpDistrib, gamma, T);

	unsigned int s;
	if (refreshFreq == 0)
	{
		s = oppsUCB1->run(n);
		delete oppsUCB1;
	}
	
	else
	{
		utils::Chrono chrono, refreshChrono, backupChrono;
		while (n-- > 0)
		{
			s = oppsUCB1->run(1);

			
			//	Print out a summary
			if ((refreshFreq > 0) && (refreshChrono.get() >= refreshFreq))
			{
				vector<unsigned int> top = oppsUCB1->getTop(10);
				cout << "\tProgression: ";
				cout << (100.0 - (100.0*(n / iniN))) << "%\n";
				cout << "\tUCB1 - Top 10\n";
				
				for (unsigned int i = 0; i < top.size(); ++i)
				{
					cout << "\t\t[" << setw(2) << (i + 1) << "]:\t";
					cout << "n = \t";
					cout << setw(9) << oppsUCB1->getNDraws(top[i]);
					cout << ";\tmu = ";
					cout << oppsUCB1->getMu(top[i]) << "\n";
				}
				cout << "\n";
				
				refreshChrono.restart();
			}
			
			
			//	Create a backup if needed
			if ((backupFreq > 0)
					&& (backupChrono.get() >= backupFreq))
			{
				utils::Chrono backupTime;
			
				cout << "\tCreate a backup..." << flush;
	
				//	Generate a random file name
				string tmpStr
					= outputFile.substr(0, outputFile.find_last_of("/"));
				tmpStr += "/";
				const char alphanum[] =
					"0123456789"
					"!@#$%^&*"
					"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					"abcdefghijklmnopqrstuvwxyz";
				for (int l = 0; l < 42; ++l)
				{ tmpStr += alphanum[rand() % (sizeof(alphanum) - 1)]; }
	
	
//				if (compressOutput)
//				{	
//					ofstream os(tmpStr.c_str());
//					exp->zSerialize(os, Z_BEST_SPEED);
//					os.close();
//	
//					rename(tmpStr.c_str(),
//							(outputFile + ".bak.zz").c_str());
//				}
//	
//				else
//				{
					Agent* agent = strategyList[s]->clone();
					agent->setName("OPPS-UCB1(" + agent->getName() + ")");
					agent->offlineTime = chrono.get();
	
					ofstream os(tmpStr.c_str());
					agent->serialize(os);
					os.close();
					rename(tmpStr.c_str(), (outputFile + ".bak").c_str());
					
					delete agent;					
//				}
				
				cout << "done! (in " << backupTime.get() << "ms)\n\n";
				
				backupChrono.restart();
			}
		}

		offlineTime = chrono.get();
		delete oppsUCB1;
	}
	

	Agent* agent = strategyList[s]->clone();
	agent->setName("OPPS-UCB1(" + agent->getName() + ")");
	agent->offlineTime = offlineTime;

	return agent;
}


dds::Agent* dds::opps::continuousSearch(
		unsigned int n, double c,
		AgentFactory* agentFactory, const MDPDistribution* mdpDistrib,
		double gamma, unsigned int T,
		unsigned int refreshFreq, unsigned int backupFreq,
		string outputFile)
{
	double offlineTime;
	double iniN = n;
	dds::opps::details::OPPSUCT* oppsUCT;
	oppsUCT = new dds::opps::details::
			OPPSUCT(c, agentFactory, mdpDistrib, gamma, T);


	vector<double> s;
	if (refreshFreq == 0)
	{
		s = oppsUCT->run(n);
		delete oppsUCT;
	}
	
	else
	{
		utils::Chrono chrono, refreshChrono, backupChrono;
		while (n-- > 0)
		{
			vector<pair<double, double> > armBoundsList;
			s = oppsUCT->run(1, &armBoundsList);
			
			
			//	Print out a summary
			if ((refreshFreq > 0) && (refreshChrono.get() >= refreshFreq))
			{
				cout << "\tProgression: ";
				cout << (100.0 - (100.0*(n / iniN))) << "%\n";
				
				vector<pair<double, double> > boundsList;
				boundsList = agentFactory->getBounds();
				double regret = 0.0;
				unsigned int nBounds = 0;
				for (unsigned int i = 0; i < armBoundsList.size(); ++i)
				{
					if (boundsList[i].first == boundsList[i].second)
						continue;
					
					double cRegret = 0.0;
					cRegret = (armBoundsList[i].second
							- armBoundsList[i].first
							- agentFactory->getSplitAcc()[i]);
					
					if (cRegret < 0) { cRegret = 0.0; }
					
					cRegret /= (boundsList[i].second
							- boundsList[i].first);
					
					regret += cRegret;
					++nBounds;
				}
				if (nBounds > 0) { regret /= nBounds; }
				
				
				cout << "\tUCT - Top regret = ";
				cout << setw(9) << regret << "\n\n";
				
				refreshChrono.restart();
			}
			
			
			//	Create a backup if needed
			if ((backupFreq > 0) && (backupChrono.get() >= backupFreq))
			{
				utils::Chrono backupTime;
			
				cout << "\tCreate a backup..." << flush;
	
				//	Generate a random file name
				string tmpStr
					= outputFile.substr(0, outputFile.find_last_of("/"));
				tmpStr += "/";
				const char alphanum[] =
					"0123456789"
					"!@#$%^&*"
					"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					"abcdefghijklmnopqrstuvwxyz";
				for (int l = 0; l < 42; ++l)
				{ tmpStr += alphanum[rand() % (sizeof(alphanum) - 1)]; }
	
	
//				if (compressOutput)
//				{	
//					ofstream os(tmpStr.c_str());
//					exp->zSerialize(os, Z_BEST_SPEED);
//					os.close();
//	
//					rename(tmpStr.c_str(),
//							(outputFile + ".bak.zz").c_str());
//				}
//	
//				else
//				{
					Agent* agent = agentFactory->get(s);
					agent->setName("OPPS-UCT(" + agent->getName() + ")");
					agent->offlineTime = chrono.get();
	
					ofstream os(tmpStr.c_str());
					agent->serialize(os);
					os.close();	
					rename(tmpStr.c_str(), (outputFile + ".bak").c_str());
					
					delete agent;
//				}
				
				cout << "done! (in " << backupTime.get() << "ms)\n\n";
				
				backupChrono.restart();
			}
		}
		
		offlineTime = chrono.get();
		delete oppsUCT;
	}
	

	Agent* agent = agentFactory->get(s);
	agent->setName("OPPS-UCT(" + agent->getName() + ")");
	agent->offlineTime = offlineTime;
	
	return agent;
}
*/