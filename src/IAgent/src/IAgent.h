
#ifndef IAGENT_H
#define IAGENT_H

#include "AgentException.h"
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	IAgent
	\author 	Castronovo Michael
	
	\brief 	Interface of a RL agent.
			
	\date 	2013-08-13
*/
// ===========================================================================
class IAgent : public Serializable
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief		Constructor.
			
			\param[name_	The name of this IAgent.
		*/
		IAgent(std::string name_) : Serializable(), name(name_) {}
		
		
		/**
			\brief	Destructor.
		*/
		virtual ~IAgent() {}
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "IAgent"; }
		
		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the name of this IAgent.
			
			\return	The name of this IAgent.
		*/
		std::string getName() const { return name; }
		
		
		/**
			\brief		Set a new name for this IAgent.
			
			\param[name_	The new name for this IAgent.
		*/
		void setName(std::string name_) { name = name_; }


		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const
		{
			return IAgent::toString();
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
		//	Protected Constructor
		// =================================================================
		/**
			\brief	Constructor.
		*/
		IAgent() : Serializable(), name("") {}


	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The name of this IAgent.
		*/
		std::string name;
};

#endif
