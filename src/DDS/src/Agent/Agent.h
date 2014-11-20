
#ifndef AGENT_H
#define AGENT_H

#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/**
	\class 	Agent
	\author 	Castronovo Michael
	
	\brief 	Interface of an Agent to address MDP problems for:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)

	\date 	2014-10-16
*/
// ===========================================================================
class dds::Agent : public IAgent
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief		Constructor.

			\param[name	The name of this Agent.
		*/
		Agent(std::string name = "");
		
		
		/**
			\brief	Destructor.
		*/
		virtual ~Agent() {}


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "Agent"; }
		
		
		/**
		     \brief    Parse the standard program options in order to
		               to instanciate an Agent.
		     
		     \return   The Agent found (0 if it fails).
		*/
		static Agent* parse(int argc, char* argv[])
                                   throw (utils::parsing::ParsingException);


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief			Learn from a MDP distribution offline
							and save the time elapsed (in ms).
			
			\param[mdpDistrib	A MDP distribution.
		*/
		void learnOffline(const MDPDistribution* mdpDistrib)
											throw (AgentException);
		
		
		/**
			\brief	Return the time elapsed during the last call of
					'learnOffline()' (in ms).
					
			\return	The time elapsed during the last call of
					'learnOffline()' (in ms).
		*/
		double getOfflineTime() const { return offlineTime; }
		
		
		/**
			\brief		Associate this Agent to a MDP.
			
			\param[mdp_	A MDP.
			\param[gamma	The discount factor.
			\param[T		The horizon limit.
		*/
		void setMDP(const MDP* mdp_, double gamma_, unsigned int T_)
											throw (AgentException);
		
		
		/**
			\brief	Return the action to perform on the currently
					associated MDP from state 'x'.
			
			\param[xt	The state from which a decision has to be made.
			
			\return	The action to perform.
		*/
		virtual int getAction(int xt) const throw (AgentException) = 0;
		
		
		/**
			\brief	Learn from <x, u, y, r> transition (online).
			
			\param[x	The origin state.
			\param[u	The action performed.
			\param[y	The state reached.
			\param[r	The reward observed.
		*/
		virtual void learnOnline(int, int, int, double)
										throw (AgentException) {}
		
		
		/**
			\brief	Reset this agent.
					(called when this Agent is associated to a new MDP).
		*/
		virtual void reset() throw (AgentException) {}
		
		
		/**
               \brief    Free unnecessary data.
                         (called when this Agent has finished to interact
                         with the current MDP).
		*/
		virtual void freeData() {}
		
		
		/**
			\brief	Return a clone of this Agent.
			
			\return	A clone of this Agent.
		*/
		virtual Agent* clone() const = 0;
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const
		{
			return Agent::toString();
		}

		
		/**
			\brief	Serialize this Object.
					If overloaded, the new 'serialize()' method should
					call the 'serialize()' method of the base class
					before doing anything else.
		*/
		virtual void serialize(std::ostream& os) const;
		
		
		/**
			\brief	Deserialize this Object.
					If overloaded, the new 'deserialize()' method should
					call the 'deserialize()' method of the base class
					before doing anything else.
		*/
		virtual void deserialize(std::istream& is)
									throw (SerializableException);
	
	
	protected:	
		// =================================================================
		//	Protected methods
		// =================================================================
		/**
			\brief			Learn from a MDP distribution offline.
			
			\param[mdpDistrib	A MDP distribution.
		*/
		virtual void learnOffline_aux(const MDPDistribution*)
										throw (AgentException) {}

		
		/**
			\brief	Return a pointer to the MDP associated to this
					Agent.
					Throw an AgentException if no MDP is associated.
					
			\return	A pointer to the MDP associated to this Agent.
		*/
		const MDP* getMDP() const throw (AgentException)
		{
			if (!mdp)
				throw AgentException("No MDP associated to this Agent!\n");

			return mdp;
		}


		/**
			\brief	Return the discount factor which will be used during
					the interaction with associated MDP.
					Throw an AgentException if no MDP is associated.
					
			\return	The discount factor.
		*/
		double getGamma() const throw (AgentException)
		{
			if (!mdp)
				throw AgentException("No MDP associated to this Agent!\n");

			return gamma;
		}


		/**
			\brief	Return the horizon limit which will be used during
					the interaction with associated MDP.
					Throw an AgentException if no MDP is associated.
					
			\return	The horizon limit.
		*/
		unsigned int getT() const throw (AgentException)
		{
			if (!mdp)
				throw AgentException("No MDP associated to this Agent!\n");

			return T;
		}
	

	private:	
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The MDP associated to this Agent.
		*/
		const MDP* mdp;
		
		
		/**
			\brief	The discount factor.
		*/
		double gamma;
		
		
		/**
			\brief	The horizon limit.
		*/
		unsigned int T;
		
		
		/**
			\brief	The time elapsed during the last call of
					'learnOffline()' (in ms).
		*/
		double offlineTime;
		
		
		// =================================================================
		//	Private methods
		// =================================================================
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
