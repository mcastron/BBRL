
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
				
	\date		2014-11-18
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
	class FormulaAgent;
	class BAMCPAgent;
	class OPPSDSAgent;
	class OPPSCSAgent;

     class FVariable;
     class QVar;
	class QMean;
	class QSelf;
	class QUniform;
	
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
						bool safeSim = true)
                                        throw (AgentException, MDPException);
	}
	
	
	/**
		\brief	This namespace gathers OPPS algorithms:
					-	OPPS-UCB1	(for discrete strategy spaces)
					-	OPPS-UCT	(for continuous strategy spaces)
	*/
	namespace opps
	{
          /**
               \brief    A UCB1 instance where drawing an arm consists to play
                         the associated strategy an a MDP of the given MDP
                         distribution.
          */	
		class UCB1 : public utils::UCB1
		{
			public:
			     // =======================================================
			     //     Public Constructor.
			     // =======================================================
                    /**
                         \brief               Constructor.
                         
                         \param[c             The constant used in the UCB1
                                              formula:
                                                 I_t(i) =
                                                 mu_i + c * sqrt(ln(n_t) / n_i)
                         \param[strategyList_ The list of strategies.
                         \param[mdpDistrib_   The MDP distribution from to
                                              learn.
                         \param[gamma_        The discount factor.
                         \param[T_            The horizon limit.
                    */
				UCB1(double c,
					const vector<Agent*>& strategyList_,
					const MDPDistribution* mdpDistrib_,
					double gamma_, unsigned int T_);

			private:
			     // =======================================================
			     //     Private attributes.
			     // =======================================================
                    /**
                         \brief    The list of strategies.
                    */
				const vector<Agent*>& strategyList;
			
				
				/**
				     \brief     The MDP distribution from to learn.
				*/
				const MDPDistribution* mdpDistrib;
				
				
				/**
				     \brief     The discount factor.
				*/
				double gamma;
				
				
				/**
				     \brief     The horizon limit.
				*/
				unsigned int T;
				
				
				// =======================================================
				//    Private methods.
				// =======================================================
				/**
          			\brief	Draw the arm 'i' and return its score.
          			
          			\param[i	The arm to draw.
          		*/
				double drawArm(unsigned int i) const throw (std::exception);
		};

          
          /**
		     \brief    A UCT instance where drawing an arm from a node
		               consists to draw a strategy from the part of the
		               strategy space defined by this node, and play it on
		               a MDP of the given MDP distribution.
		*/
		class UCT : public utils::UCT
		{
			public:
			     // =======================================================
			     //     Public Constructor.
			     // =======================================================
                    /**
                         \brief               Constructor.
                         
                         \param[c             The constant used in the UCB1
                                              formula:
                                                 I_t(i) =
                                                 mu_i + c * sqrt(ln(n_t) / n_i)
                         \param[agentFactory_ The AgentFactory to use for
                                              bulding the strategies.
                         \param[mdpDistrib_   The MDP distribution from to
                                              learn.
                         \param[gamma_        The discount factor.
                         \param[T_            The horizon limit.
                    */
				UCT(	double c,
					AgentFactory* agentFactory_,
					const MDPDistribution* mdpDistrib_,
					double gamma_, unsigned int T_);

			private:
			     // =======================================================
			     //     Private attributes.
			     // =======================================================
			     /**
                         \brief    The AgentFactory to use for bulding the
                                   strategies.
			     */
				AgentFactory* agentFactory;
				
				
				/**
				     \brief     The MDP distribution from to learn.
				*/
				const MDPDistribution* mdpDistrib;
				
				
				/**
				     \brief     The discount factor.
				*/
				double gamma;
				
				
				/**
				     \brief     The horizon limit.
				*/
				unsigned int T;
				
				
				// =======================================================
			     //     Private methods.
			     // =======================================================
				/**
          			\brief			Draw the arm defined by
          			                    'paramList' and return its score.
          			
          			\param[paramList	The parameters defining the arm to
          			                    draw.
          			
          			\return			The score of the arm defined by
          			                    'paramList'.
          		*/
				double drawArm(const vector<double>& paramList) const;
		};
	}
}

#endif
