
#ifndef OPPSCSAGENT_H
#define OPPSCSAGENT_H

#include "Agent.h"
#include "../AgentFactory/AgentFactory.h"
#include "../MDP/MDP.h"
#include "../MDP/CModel.h"
#include "../MDPDistribution/MDPDistribution.h"
#include "../MDPDistribution/DirMultiDistribution.h"
#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	OPPSCSAgent
	\author 	Castronovo Michael
	
	\brief 	A RL agent which applies the OPPS meta-learning algorithm
	          during its offline phase (continuous case).
	          It has been designed to address:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)
			
			For Offline learning, does not support another distribution
			than 'DirMultiDistribution'.

	\date 	2015-02-27
*/
// ===========================================================================
/* final */ class dds::OPPSCSAgent : public dds::Agent
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the OPPSCSAgent to load.
					(can either be compressed or uncompressed)
		*/
		OPPSCSAgent(std::istream& is);
		
		
		/**
			\brief			 Constructor.
			                     ('K' = 3, 'k', 'hMax' and 'delta' are
			                     chosen appropriatly with respect to 'n').

               \param[n_            The number of evaluations performed during
                                    the offline learning.
               \param[K_            The arity of the tree to develop
                                    (odd number).
               \param[agentFactory_ The AgentFactory to use for bulding the
                                    strategies.
               \param[gamma_        The discount factor.
               \param[T_            The horizon limit.
		*/
		OPPSCSAgent(   unsigned int n_, unsigned int K_,
		               AgentFactory* agentFactory_,
					double gamma_, unsigned int T_);


		/**
			\brief			 Constructor.

               \param[n_            The number of evaluations performed during
                                    the offline learning.
               \param[K_            The arity of the tree to develop
                                    (odd number).
	          \param[k_            The maximal number of evaluations
	                               per node.
	          \param[hMax_         The maximal depth of the tree to
	                               develop.
	          \param[delta_        The confidence parameter.
               \param[agentFactory_ The AgentFactory to use for bulding the
                                    strategies.
               \param[gamma_        The discount factor.
               \param[T_            The horizon limit.
		*/
		OPPSCSAgent(   unsigned int n_, unsigned int K_,
                         unsigned int k_, unsigned int hMax_, double delta_,
		               AgentFactory* agentFactory_,
					double gamma_, unsigned int T_);
		
		
		/**
			\brief	Destructor.
			          'agentFactory' is freed.
		*/
		~OPPSCSAgent();


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "OPPSCSAgent"; }


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the action to perform on the currently
					associated MDP from state 'x'.
			
			\param[xt	The state from which a decision has to be made.
			
			\return	The action to perform.
		*/
		int getAction(int xt) const throw (AgentException);
		
		
		/**
			\brief	Learn from <x, u, y, r> transition (online).
			
			\param[x	The origin state.
			\param[u	The action performed.
			\param[y	The state reached.
			\param[r	The reward observed.
		*/
		void learnOnline(int x, int u, int y, double r)
                                                       throw (AgentException);

		
		/**
			\brief	Reset this agent.
					(called when this Agent is associated to a new MDP).
		*/
		void reset() throw (AgentException);


          /**
               \brief    Free unnecessary data.
                         (called when this Agent has finished to interact
                         with the current MDP).
		*/
		void freeData();
		

		/**
			\brief	Return a clone of this Agent.
			
			\return	A clone of this Agent.
		*/
		Agent* clone() const
		{
			return cloneInstance<OPPSCSAgent>(this);
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const { return OPPSCSAgent::toString(); }


          /**
               \brief    Return the name of the class of this object, formatted
                         for export files.

               \return   The name of the class of this object, formatted for
                         export files.
		*/
		std::string getExportClassName() const { return "OPPS-CS"; }


		/**
			\brief	Serialize this Object.
					If overloaded, the new 'serialize()' method should
					call the 'serialize()' method of the base class
					before doing anything else.
		*/
		void serialize(std::ostream& os) const;
		
		
		/**
			\brief	Deserialize this Object.
					If overloaded, the new 'deserialize()' method should
					call the 'deserialize()' method of the base class
					before doing anything else.
		*/
		void deserialize(std::istream& is) throw (SerializableException);


	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
               \brief    The strategy learned offline.
		*/
		Agent* agent;
		
		
		/**
               \brief    The number of evaluations performed during the
                         offline learning.
		*/
		unsigned int n;

		
		/**
               \brief    The arity of the tree to develop (odd number).
          */
          unsigned int K;
                                    
          /**
               \brief    The maximal number of evaluations per node.
          */
          unsigned int k;
          
          
          /**
               \brief    The maximal depth of the tree to develop.
          */
	     unsigned int hMax;
	     
	     
	     /**
               \brief    The confidence parameter.
	     */     
          double delta;        
		
		
		/**
               \brief    The AgentFactory to use for bulding the
                         strategies.
	     */
		AgentFactory* agentFactory;

		
		/**
		     \brief     The discount factor.
		*/
		double gamma;
		
		
		/**
		     \brief     The horizon limit.
		*/
		unsigned int T;
			
		
		// =================================================================
		//	Private methods
		// =================================================================
		/**
			\brief			Learn from a MDP distribution offline.

							Consist to learn an appropriate initial
							model.
			
			\param[mdpDistrib	A MDP distribution.
		*/
		void learnOffline_aux(const MDPDistribution* mdpDistrib)
											throw (AgentException);


          /**
               \brief    Initialize this OPPSCSAgent.
          */
          void init();

		
		#ifndef NDEBUG
		/**
			\brief	Function to call whenever the internal data could
					be corrupted by wrong data entries
					(calls 'assert()' for each data).
		*/
		void checkIntegrity() const;
		#endif
};

#endif
