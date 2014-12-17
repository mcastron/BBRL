
#ifndef DDS_H
#define DDS_H

#include "../ExternalLibs.h"


// ===========================================================================
/**
	\namespace	dds
	\author 		Castronovo Michael
	
	\brief		A namespace gathering the tools and classes related to
	               RL problems with Discrete state space, Discrete action
	               space and Single reward.
					
				This namespace gathers the Agents, MDPs, MDPDistributions
				and Experiments in this particular setting.
				
	\date		2014-12-17
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
	/**
          \class    Agent
          \brief    Interface of an RL agent to address MDPs.
	*/
	class Agent;
	
	
	/**
          \class    RandomAgent
          \brief    A RL agent taking random decisions.
	*/
	class RandomAgent;
	
	
	/**
          \class    OptimalAgent
          \brief 	A RL agent taking optimal decisions.
	*/
	class OptimalAgent;


	/**
	     \class    EGreedyAgent
	     \brief 	A RL Agent following an e-greedy policy.
	*/
	class EGreedyAgent;


     /**
          \class    SoftMaxAgent
          \brief 	A RL Agent following a soft-max policy.
     */
	class SoftMaxAgent;
	
	
	/**
          \class    VDBEEGreedyAgent
          \brief 	A RL Agent following an vdbe e-greedy policy.
	*/
	class VDBEEGreedyAgent;
	
	
	/**
	     \class    FormulaAgent
          \brief 	A RL Agent following a policy parametrized by a formula.
	*/
	class FormulaAgent;
	
	
	/**
          \class    FormulaVector
          \brief 	Defines a std::vector of formulas which also add
                    serialization.
	*/
	class FormulaVector;
	
	
	/**
          \class    BAMCPAgent
          \brief 	A RL Agent following a BAMCP policy.
	*/
	class BAMCPAgent;
	
	
	/**
          \class    BFS3Agent
          \brief 	A RL Agent following a BFS3 policy.
	*/
	class BFS3Agent;
	
	
	/**
	     \class    OPPSDSAgent
	     \brief 	A RL agent which applies the OPPS meta-learning algorithm
	               during its offline phase (discrete case).
	*/
	class OPPSDSAgent;
	
	
	/**
	     \class    OPPSCSAgent
	     \brief 	A RL agent which applies the OPPS meta-learning algorithm
	               during its offline phase (continuous case).
	*/
	class OPPSCSAgent;


     /**
          \class    FVariable
          \brief 	Represent a variable for the FormulaAgent's.
     */
     class FVariable;
     
     
     /**
          \class    FVariableException
          \brief    Exception.
     */
     class FVariableException;
     
     
     /**
          \class    QVar
          \brief 	A variable for the FormulaAgent's, which represents a
	               Q-function model-based variable for FormulaAgent.
     */
     class QVar;
     
     
     /**
          \class    QMean
          \brief 	A variable for the FormulaAgent's, which represents a
	               Q-function based on the mean model of a prior distribution.
     */
	class QMean;
	
	
	/**
          \class    QSelf
          \brief 	A variable for the FormulaAgent's, which represents a
	               Q-function based on the 'self' model of a prior
	               distribution.
     */
	class QSelf;
	
	
	/**
          \class    QUniform
          \brief 	A variable for the FormulaAgent's, which represents a
	               Q-function based on the 'uniform' model of a prior
	               distribution.
     */
	class QUniform;
	
	
	/**
          \class    AgentFactory
          \brief    Interface of an AgentFactory to generate RL Agents.
	*/
	class AgentFactory;
	
	
	/**
          \class    AgentFactoryException
          \brief    Exception.
	*/
	class AgentFactoryException;
	
	
	/**
          \class    EGreedyAgentFactory
          \brief 	An AgentFactory which generates EGreedyAgent's.
	*/
	class EGreedyAgentFactory;
	
	
	/**
          \class    SoftMaxAgentFactory
          \brief 	An AgentFactory which generates SoftMaxAgent's.
	*/
	class SoftMaxAgentFactory;
	
	
	/**
          \class    VDBEEGreedyAgentFactory
          \brief 	An AgentFactory which generates VDBEEGreedyAgent's.
	*/
	class VDBEEGreedyAgentFactory;
	
	
	/**
          \class    MDP
          \brief 	Represent a MDP problem.
	*/
	class MDP;
	
	
	/**
          \class    MDPDistribution
          \brief 	Interface of a MDP distribution.
	*/
	class MDPDistribution;
	
	
	/**
          \class    DirMultiDistribution
          \brief 	A MDP distribution represented by a Dirichlet Multinomial
	               distribution.
	*/
	class DirMultiDistribution;
	
	
	/**
          \class    Model
          \brief 	Interface of a model of a MDP.
	*/
	class Model;
	
	
	/**
          \class    CModel
          \brief 	A Model represented by a set of counters.
	*/
	class CModel;

     
     /**
          \class    Experiment
          \brief 	Represent a RL experiment.
     */
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
				double drawArm(unsigned int i) const
                                                       throw (std::exception);
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
