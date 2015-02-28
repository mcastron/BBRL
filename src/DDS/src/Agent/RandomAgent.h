
#ifndef RANDOMAGENT_H
#define RANDOMAGENT_H

#include "Agent.h"
#include "../MDP/MDP.h"
#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	RandomAgent
	\author 	Castronovo Michael
	
	\brief 	A RL agent taking random decisions.
	          It has been designed to address:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)

	\date 	2015-02-27
*/
// ===========================================================================
/* final */ class dds::RandomAgent : public dds::Agent
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the RandomAgent to load.
					(can either be compressed or uncompressed)
		*/
		RandomAgent(std::istream& is);
		
		
		/**
			\brief	Constructor.
		*/
		RandomAgent();
		
		
		/**
			\brief	Destructor.
		*/
		~RandomAgent() {}


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "RandomAgent"; }


		// =================================================================
		//	Public methods
		// =================================================================	
		/**
			\brief	Return the action to perform on the currently
					associated MDP from state 'x'.
					Consists to draw a random action from the action
					space of the currently associated MDP.
			
			\return	The action to perform.
		*/
		int getAction(int) const throw (AgentException);

		
		/**
			\brief	Return a clone of this Agent.
			
			\return	A clone of this Agent.
		*/
		Agent* clone() const
		{
			return cloneInstance<RandomAgent>(this);
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const { return RandomAgent::toString(); }


          /**
               \brief    Return the name of the class of this object, formatted
                         for export files.

               \return   The name of the class of this object, formatted for
                         export files.
		*/
		std::string getExportClassName() const { return "Random"; }

		
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
};

#endif
