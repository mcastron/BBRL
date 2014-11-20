
#ifndef OPTIMALAGENT_H
#define OPTIMALAGENT_H

#include "Agent.h"
#include "../MDP/MDP.h"
#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/**
	\class 	OptimalAgent
	\author 	Castronovo Michael
	
	\brief 	Interface of an Optimal Agent to address MDP problems for:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)

	\date 	2014-08-13
*/
// ===========================================================================
/* final */ class dds::OptimalAgent : public dds::Agent
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the OptimalAgent to load.
					(can either be compressed or uncompressed)
		*/
		OptimalAgent(std::istream& is);

		
		/**
			\brief		Constructor.
			
			\param[uGamma	The discount factor to use for undiscounted
						trajectories. (in ]0; 1[);
		*/
		OptimalAgent(double uGamma_ = 0.95);
		
		
		/**
			\brief	Destructor.
		*/
		~OptimalAgent() {}


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "OptimalAgent"; }


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
		int getAction(int xt) const throw (MDPException);

		
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
			return cloneInstance<OptimalAgent>(this);
		}


		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const { return OptimalAgent::toString(); }


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
			\brief	The discount factor to use for undiscounted
					trajectories.
		*/
		double uGamma;
		
		
		/**
			\brief	The optimal Q-Function of the currently associated
					MDP.
		*/
		std::vector<double> Q;
		
		
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
