
#ifndef IAGENT_H
#define IAGENT_H

#include "AgentException.h"
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	IAgent
	\author 	Castronovo Michael
	
	\brief 	Interface of a RL agent.
			
	\date 	2015-02-27
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
               \brief    Return the name of this IAgent, formatted for
                         export files.
                         If not overloaded, has the same return as 'getName()'.

               \return   The name of this IAgent, formatted for export files.
		*/
		virtual std::string getExportName() const { return getName(); }
		
		
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
               \brief    Return the name of the class of this object, formatted
                         for export files.
                         If not overloaded, has the same return as
                         'getClassName()'.

               \return   The name of the class of this object, formatted for
                         export files.
		*/
		virtual std::string getExportClassName() const
		{
		   return getClassName();
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
