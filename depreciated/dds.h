
#ifndef DDS_H
#define DDS_H

#include "../ExternalLibs.h"


// ===========================================================================
/**
	\namespace	DDS
	\author 		Castronovo Michael
	
	\brief		DDS stands for:
					- DISCRETE state space (int)
					- DISCRETE action space (int)
					- SINGLE reward (double)
					
				This namespace gathers the Agents, MDPs, MDPDistributions
				and Experiments in this particular setting.
				
	\date		2014-07-29
*/
// ===========================================================================
namespace dds
{
	// ======================================================================
	//	Constants
	// ======================================================================
	/**
		\brief	The accuracy level to use by the algorithms
				(e.g.: value-iteration / q-iteration)
	*/
	const double ACCURACY = 1e-2;
	
	
	/**
		\brief	The accuracy level of the representation of '0'.
				(e.g.: the sum of the probability values in the transition
				matrix of an MDP is not exactly '1.0' in every case)
	*/
	const double ZERO_ACCURACY = 1e-9;
	
	
	// ======================================================================
	//	Functions
	// ======================================================================
	/**
		\brief	Perfom some initialization tasks for the 'DDS' package
				and initializes the RNG's.
	*/	
	void init();

	
	// ======================================================================
	//	Classes
	// ======================================================================
	class Agent;
	class RandomAgent;
	class OptimalAgent;
	class EGreedyAgent;
	class SoftMaxAgent;
	class VDBEEGreedyAgent;
	
	class AgentFactory;
	class EGreedyAgentFactory;
	class SoftMaxAgentFactory;
	class VDBEEGreedyAgentFactory;
	
	class MDP;
	class MDPDistribution;
	class DirMultiDistribution;
	
	class Model;
	class CModel;
	
	class Experiment;


	// ======================================================================
	//	Namespaces
	// ======================================================================
	/**
		\brief	This namespace gathers several functions related to
				an RL simulation.
	*/
	namespace simulation
	{
		// =================================================================
		//	Classes
		// =================================================================
		/**
			\brief	Represent a transition observed during the
					interaction between an Agent and a MDP.
		*/
		typedef rl_utils::simulation::
			Transition<unsigned int, unsigned int, double> Transition;


		/**
			\brief	Represent the data gathered during the interaction
					between an Agent and a MDP over a single trajectory.
		*/
		typedef rl_utils::simulation::
			SimulationRecord<unsigned int, unsigned int, double>
					SimulationRecord;
		
		
		// =================================================================
		//	Functions
		// =================================================================
		/**
			\brief		Perform a single trajectory simulation.
			
			\param[agent	The agent interacting with the MDP.
			\param[mdp	The MDP representing the environment.
			\param[gamma	The discount factor.
			\param[T		The horizon limit.
			\param[safeSim	If true, the MDP is 'unknown', preventing the
						agent to access MDP data
						(e.g.: the transition matrix).
			
			\return		A SimulationRecord storing all the data
						about the simulation performed.
		*/
		simulation::SimulationRecord
				simulate(	dds::Agent* agent, dds::MDP* mdp,
						double gamma, unsigned int T,
						bool safeSim = true);
	}
	
	
	/**
		\brief	This namespace gathers OPPS algorithms:
					-	OPPS-UCB1	(for discrete strategy spaces)
					-	OPPS-UCT	(for continuous strategy spaces)
	*/
	namespace opps
	{
		// =================================================================
		//	Details (to ignore)
		// =================================================================
		namespace details
		{
			//	A UCB1 instance where drawing an arm consists to play
			//	the associated strategy an a MDP of the given MDP
			//	distribution.
			class OPPSUCB1 : public utils::UCB1
			{
				public:
					OPPSUCB1(	double c,
							const vector<Agent*>& strategyList_,
							const MDPDistribution* mdpDistrib_,
							double gamma_, unsigned int T_);

				private:
					const vector<Agent*>& strategyList;
					const MDPDistribution* mdpDistrib;
					double gamma;
					unsigned int T;
					
					double drawArm(unsigned int i) const;
			};


			//	A UCT instance where drawing an arm from a node consists
			//	to draw a strategy from the part of the strategy space
			//	defined by this node, and play it on a MDP of the given
			//	MDP distribution.
			class OPPSUCT : public utils::UCT
			{
				public:
					OPPSUCT(	double c,
							AgentFactory* agentFactory_,
							const MDPDistribution* mdpDistrib_,
							double gamma_, unsigned int T_);

				private:
					AgentFactory* agentFactory;
					const MDPDistribution* mdpDistrib;
					double gamma;
					unsigned int T;
					
					double drawArm(const vector<double>& paramList) const;
			};
		}
		
		
		// =================================================================
		//	Functions
		// =================================================================
		/**
			\brief			Create and run a UCB1 in order to identify
							the best strategy of 'strategyList' in
							average on the MDPs drawn from 'mdpDistrib'.
			
			\param[n			The number of draws of the UCB1.
			\param[c			The constant used in the UCB1 formula:
								I_t(i) = mu_i + c * sqrt(ln(n_t) / n_i)
			\param[strategyList	The list of strategies.
			\param[mdpDistrib	The MDP distribution.
			\param[gamma		The discount factor.
			\param[T			The horizon limit.
			\param[refreshFreq	The frequence of display output.
			\param[backupFreq	The frequence of the backups.
			\param[outputFile	The file name to use for the backups.
			
			\return			A clone of the best strategy of
							'strategyList'	according to the UCB1.
		*//*
		Agent* discreteSearch(
				unsigned int n, double c,
				const vector<Agent*>& strategyList,
				const MDPDistribution* mdpDistrib,
				double gamma, unsigned int T,
				unsigned int refreshFreq, unsigned int backupFreq,
				std::string outputFile);*/


		/**
			\brief			Create and run a UCT in order to identify
							the best strategy of the strategy space
							defined by 'boundsList' in average on the
							MDPs drawn from 'mdpDistrib'.
					
			\param[n			The number of draws of the UCB1.
			\param[c			The constant used in the UCB1 formula:
								I_t(i) = mu_i + c * sqrt(ln(n_t) / n_i)
			\param[agentFactory	An AgentFactory to generate the Agents of
							the strategy space.	
							'agentFactory->init()' function must
							have been called before.	
			\param[boundsList	The list of bounds defining the strategy
							space.
			\param[mdpDistrib	The MDP distribution.
			\param[gamma		The discount factor.
			\param[T			The horizon limit.
			\param[refreshFreq	The frequence of display output.
			\param[backupFreq	The frequence of the backups.
			\param[outputFile	The file name to use for the backups.
			
			\return			The best strategy of the strategy space
							defined by 'boundsList' according to the
							UCT.
		*//*
		Agent* continuousSearch(
				unsigned int n, double c,
				AgentFactory* agentFactory,
				const MDPDistribution* mdpDistrib,
				double gamma, unsigned int T,
				unsigned int refreshFreq, unsigned int backupFreq,
				std::string outputFile);*/
	}
}

#endif
