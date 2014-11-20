
#ifndef EGREEDYAGENT_H
#define EGREEDYAGENT_H

#include "Agent.h"
#include "../MDP/MDP.h"
#include "../MDP/CModel.h"
#include "../MDPDistribution/MDPDistribution.h"
#include "../MDPDistribution/DirMultiDistribution.h"
#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/**
	\class 	EGreedyAgent
	\author 	Castronovo Michael
	
	\brief 	Interface of an e-greedy Agent to address MDP problems for:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)
			
			For Offline learning, does not support another distribution
			than 'DirMultiDistribution'.

	\date 	2014-09-23
*/
// ===========================================================================
/* final */ class dds::EGreedyAgent : public dds::Agent
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the EGreedyAgent to load.
					(can either be compressed or uncompressed)
		*/
		EGreedyAgent(std::istream& is);
		
		
		/**
			\brief			Constructor.

			\param[epsilon_	The probability to select an action
							randomly (best one according to the current
							model).
			
			\param[iniModel_	The initial model on which the Agent based
							its 'best' decisions
							(updated during the interaction, learned
							offline if not provided).
		*/
		EGreedyAgent(double epsilon_, CModel* iniModel_ = 0);
		
		
		/**
			\brief	Destructor.
		*/
		~EGreedyAgent();


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "EGreedyAgent"; }


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
			return cloneInstance<EGreedyAgent>(this);
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const { return EGreedyAgent::toString(); }

		
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
			\brief	The probability to select an action randomly.
		*/
		double epsilon;


		/**
			\brief	The result of the last call of 'qIteration()' on
					'cModel'.
		*/
		vector<double> Q;
		
		
		/**
			\brief	The initial model.
		*/
		CModel* iniModel;
		
		
		/**
			\brief	The model defining the 'best' action perform for
					the currently associated MDP.
		*/
		CModel* cModel;
		
		
		/**
			\brief	The number of states of the currently associated
					MDP.
		*/
		unsigned int nX;
		
		
		/**
			\brief	The number of actions of the currently associated
					MDP.
		*/
		unsigned int nU;
		
		
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
