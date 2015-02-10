
#ifndef AGENTFACTORY_H
#define AGENTFACTORY_H

#include "AgentFactoryException.h"
#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	AgentFactory
	\author 	Castronovo Michael
	
	\brief 	Interface of an AgentFactory to generate RL Agents.

	\date 	2015-01-23
*/
// ===========================================================================
class dds::AgentFactory : public Serializable
{		
	public:
		// =================================================================
		//	Public Destructor
		// =================================================================
		/**
			\brief	Destructor.
		*/
		virtual ~AgentFactory();
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "AgentFactory"; }


          /**
		     \brief    Parse the standard program options in order to
		               to instanciate an AgentFactory.
		     
		     \return   The AgentFactory found (0 if it fails).
		*/
		static AgentFactory* parse(int argc, char* argv[])
                                   throw (utils::parsing::ParsingException);

		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief			Initialize this AgentFactory with respect
							to the given MDP distribution.
			
			\param[mdpDistrib	A MDP distribution.
		*/
		virtual void init(const MDPDistribution* mdpDistrib)
									throw (AgentFactoryException) = 0;


		/**
			\brief			Return an agent parametrized by 'paramList'.
			
			\param[paramList	The parameters to use.
			
			\return			An agent parametrized by 'paramList'.
		*/
		virtual Agent* get(const std::vector<double>& paramList) const
									throw (AgentFactoryException) = 0;


          /**
               \brief    Return the number of parameters.
               
               \return   The number of parameters.
          */
          unsigned int getNbParam() const { return getBounds().size(); }


		/**
			\brief			Return the list of bounds on each parameter.
			
			\return			The list of bounds on each parameter.
		*/
		virtual std::vector<std::pair<double, double> >
				getBounds() const throw (AgentFactoryException) = 0;


		/**
			\brief			Return the list of split accuracies of each
							parameter.
			
			\return			The list of split accuracies of each
							parameter.
		*/
		virtual std::vector<double> getSplitAcc() const
									throw (AgentFactoryException) = 0;
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const
		{
		   return AgentFactory::toString();
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
};

#endif
