
#include "Agent/Guez/utils/guez_utils.h"

#include "Agent/Agent.h"
#include "Agent/RandomAgent.h"
#include "Agent/OptimalAgent.h"
#include "Agent/EGreedyAgent.h"
#include "Agent/SoftMaxAgent.h"
#include "Agent/VDBEEGreedyAgent.h"
#include "Agent/OPPSDSAgent.h"
#include "Agent/OPPSCSAgent.h"
#include "Agent/BAMCPAgent.h"

#include "AgentFactory/AgentFactory.h"
#include "AgentFactory/EGreedyAgentFactory.h"
#include "AgentFactory/SoftMaxAgentFactory.h"
#include "AgentFactory/VDBEEGreedyAgentFactory.h"

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

	Serializable::checkIn<OPPSDSAgent>(
			&Serializable::createInstance<OPPSDSAgent>);

	Serializable::checkIn<OPPSCSAgent>(
			&Serializable::createInstance<OPPSCSAgent>);

	Serializable::checkIn<BAMCPAgent>(
			&Serializable::createInstance<BAMCPAgent>);
			
			
	//  AgentFactorys
	Serializable::checkIn<EGreedyAgentFactory>(
			&Serializable::createInstance<EGreedyAgentFactory>);

	Serializable::checkIn<SoftMaxAgentFactory>(
			&Serializable::createInstance<SoftMaxAgentFactory>);

	Serializable::checkIn<VDBEEGreedyAgentFactory>(
			&Serializable::createInstance<VDBEEGreedyAgentFactory>);


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
dds::opps::UCB1::UCB1(
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


double dds::opps::UCB1::drawArm(unsigned int i) const
{
	MDP* mdp = mdpDistrib->draw();
	
	dds::simulation::SimulationRecord simRec;
	simRec = dds::simulation::simulate(strategyList[i], mdp, gamma, T);
	
	delete mdp;
	
	return simRec.computeDSR();
}


dds::opps::UCT::UCT(
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


double dds::opps::UCT::drawArm(
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
