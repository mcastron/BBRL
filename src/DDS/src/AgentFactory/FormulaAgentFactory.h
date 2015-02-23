
#ifndef FORMULAAGENTFACTORY_H
#define FORMULAAGENTFACTORY_H

#include "AgentFactory.h"

#include "../Agent/FormulaAgent/FormulaAgent.h"

#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	FormulaAgentFactory
	\author 	Castronovo Michael
	
	\brief 	An AgentFactory which generates FormulaAgent's,
	          where the variables are combined with a polynomial.
	          (e.g.: W0*X0 + W1*X1 + ... + WN*XN)
	          
	          'init()' must be called after this AgentFactory has been
	          deserialized.

	\date 	2015-02-23
*/
// ===========================================================================
/* final */ class dds::FormulaAgentFactory : public dds::AgentFactory
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the FormulaAgentFactory to load.
					(can either be compressed or uncompressed)
		*/
		FormulaAgentFactory(std::istream& is);
		
		
		/**
			\brief		     Constructor.
			
			\param[minW_	     The minimal value of the weights.
			\param[maxW_	     The maximal value of the weights.
			\param[accW_        The accuracy of the value of the weights.
			\param[varNameList_ The list of variables' names.
		*/
		FormulaAgentFactory(double minW_, double maxW_, double accW_,
		                    std::vector<std::string>& varNameList_);
		
		
		/**
               \brief    Destructor.
		*/
		~FormulaAgentFactory() {}
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "FormulaAgentFactory"; }

		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief			Initialize this AgentFactory with respect
							to the given MDP distribution.
							
							Must be called after this AgentFactory has
							been deserialized.

               \param[mdpDistrib	A MDP distribution.
		*/
		void init(const MDPDistribution* mdpDistrib)
									throw (AgentFactoryException);


		/**
			\brief			Return an agent parametrized by 'paramList'.
			
			\param[paramList	The parameters to use.
			
			\return			An agent parametrized by 'paramList'.
		*/
		Agent* get(const std::vector<double>& paramList) const
									throw (AgentFactoryException);

		
		/**
			\brief			Return the list of bounds on each parameter.
			
			\return			The list of bounds on each parameter.
		*/
		std::vector<std::pair<double, double> > getBounds() const
									throw (AgentFactoryException)
		{
			return boundsList;
		}


		/**
			\brief			Return the list of split accuracies of each
							parameter.
			
			\return			The list of split accuracies of each
							parameter.
		*/
		std::vector<double> getSplitAcc() const throw (AgentFactoryException)
		{
			return splitAccList;
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const
		{
		   return FormulaAgentFactory::toString();
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
			\brief	The minimal and maximal values of the weights
					respectively.
		*/
		double minW, maxW;
		
		
		/**
               \brief    The accuracy on the values of the weights.
		*/
		double accW;
		
		
		/**
               \brief   The list of variables' names.
		*/
		std::vector<std::string> varNameList;
		
		
		/**
               \brief    The MDP distribution used for initializing this
                         FormulaAgentFactory.
		*/
		const MDPDistribution* mdpDistrib;
		
		
		/**
			\brief	The list of bounds on each parameter.
		*/
		std::vector<std::pair<double, double> > boundsList;
		
		
		/**
			\brief	The list of split accuracies of each parameter.
		*/
		std::vector<double> splitAccList;
};

#endif
