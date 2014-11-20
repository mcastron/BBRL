
#ifndef OPPSDSAGENT_H
#define OPPSDSAGENT_H

#include "Agent.h"
#include "../AgentFactory/AgentFactory.h"
#include "../MDP/MDP.h"
#include "../MDP/CModel.h"
#include "../MDPDistribution/MDPDistribution.h"
#include "../MDPDistribution/DirMultiDistribution.h"
#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/**
	\class 	OPPSDSAgent
	\author 	Castronovo Michael
	
	\brief 	Interface of an e-greedy Agent to address MDP problems for:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)
			
			For Offline learning, does not support another distribution
			than 'DirMultiDistribution'.

	\date 	2014-10-13
*/
// ===========================================================================
/* final */ class dds::OPPSDSAgent : public dds::Agent
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the OPPSDSAgent to load.
					(can either be compressed or uncompressed)
		*/
		OPPSDSAgent(std::istream& is);
		
		
		/**
			\brief			 Constructor.
			
			\param[n_            The number of draws of the UCB1 during the
			                     offline learning.
			\param[c_            The constant used in the UCB1
                                        formula:
                                        I_t(i) = mu_i + c * sqrt(ln(n_t) / n_i)
               \param[strategyList_ The list of strategies.
               \param[gamma_        The discount factor.
               \param[T_            The horizon limit.
		*/
		OPPSDSAgent(   unsigned int n, double c_,
                         const vector<Agent*>& strategyList_,
					double gamma_, unsigned int T_);
		
		
		/**
			\brief	Destructor.
			          'strategyList' is freed.
		*/
		~OPPSDSAgent();


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "OPPSDSAgent"; }


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the action to perform on the currently
					associated MDP from state 'x'.
			
			\param[xt	The state from which a decision has to be made.
			
			\return	The action to perform.
		*/
		int getAction(int xt) const throw (MDPException);
		
		
		/**
			\brief	Learn from <x, u, y, r> transition (online).
			
			\param[x	The origin state.
			\param[u	The action performed.
			\param[y	The state reached.
			\param[r	The reward observed.
		*/
		void learnOnline(int x, int u, int y, double r) throw (MDPException);

		
		/**
			\brief	Reset this agent.
					(called when this Agent is associated to a new MDP).
		*/
		void reset() throw (MDPException);


		/**
			\brief	Return a clone of this Agent.
			
			\return	A clone of this Agent.
		*/
		Agent* clone() const
		{
			return cloneInstance<OPPSDSAgent>(this);
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const { return OPPSDSAgent::toString(); }

		
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
		     \brief    The number of draws of the UCB1 during the offline
		               learning.
		*/
		unsigned int n;
		
		
		/**
		     \brief    The constant used in the UCB1 formula:
                              I_t(i) = mu_i + c * sqrt(ln(n_t) / n_i)
		*/
		double c;
		
		
		/**
               \brief    The list of strategies.
          */
		vector<Agent*> strategyList;
		
		
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
