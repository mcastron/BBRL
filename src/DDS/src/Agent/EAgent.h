
#ifndef EAGENT_H
#define EAGENT_H

#include "Agent.h"
#include "../MDP/MDP.h"
#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	EAgent
	\author 	Castronovo Michael
	
	\brief 	A RL agent which adds a probability of 'epsilon' to take
	          a random decision to an Agent.
	          It has been designed to address:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)

	\date 	2015-12-18
*/
// ===========================================================================
/* final */ class dds::EAgent : public dds::Agent
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the EAgent to load.
					(can either be compressed or uncompressed)
		*/
		EAgent(std::istream& is);

		
		/**
			\brief		     Constructor.
			
			\param[epsilon_     The probability to select an action
							randomly.
							
			\param[baseAgent_   The agent taking the decisions with a
			                    probability of 'epsilon - 1'.
		*/
		EAgent(double epsilon_, Agent* baseAgent_);
		
		
		/**
			\brief	Destructor.
		*/
		~EAgent();


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "EAgent"; }


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the action to perform on the currently
					associated MDP from state 'x'.
					Consists to draw a Optimal action from the action
					space of the currently associated MDP.
			
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
			return cloneInstance<EAgent>(this);
		}


		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const { return EAgent::toString(); }


          /**
               \brief    Return the name of the class of this object, formatted
                         for export files.

               \return   The name of the class of this object, formatted for
                         export files.
		*/
		std::string getExportClassName() const
		{
		     return "e-" + baseAgent->getExportClassName();
		}


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
			\brief	The agent taking the decisions with a probability
			          of 'epsilon - 1'.
		*/
		Agent* baseAgent;
		
		
		// =================================================================
		//	Private methods
		// =================================================================
		/**
			\brief			Learn from a MDP distribution offline.
			
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
