
#ifndef SOFTMAXAGENT_H
#define SOFTMAXAGENT_H

#include "Agent.h"
#include "../MDP/MDP.h"
#include "../MDP/CModel.h"
#include "../MDPDistribution/MDPDistribution.h"
#include "../MDPDistribution/DirMultiDistribution.h"
#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	SoftMaxAgent
	\author 	Castronovo Michael
	
	\brief 	A RL Agent following a soft-max policy.
	          It has been designed to address:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)
			
			For Offline learning, does not support another distribution
			than 'DirMultiDistribution'.

	\date 	2014-12-13
*/
// ===========================================================================
/* final */ class dds::SoftMaxAgent : public dds::Agent
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the SoftMaxAgent to load.
					(can either be compressed or uncompressed)
		*/
		SoftMaxAgent(std::istream& is);
		
		
		/**
			\brief			Constructor.

			\param[tau_		Controls the probability to select an action
							with respect to its Q-value according to the
							current model.
							(0: random; +inf: random)
			
			\param[iniModel_	The initial model on which the Agent based
							its 'best' decisions
							(updated during the interaction, learned
							offline if not provided).
		*/
		SoftMaxAgent(double tau_, CModel* iniModel_ = 0);
		
		
		/**
			\brief	Destructor.
			          'iniModel' is freed.
		*/
		~SoftMaxAgent();


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "SoftMaxAgent"; }


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
			return cloneInstance<SoftMaxAgent>(this);
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const { return SoftMaxAgent::toString(); }


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
			\brief	Controls the probability to select an action with
					respect to its Q-value according to the current
					model.
					(0: random; +inf: random)
		*/
		double tau;


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
