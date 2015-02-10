
#ifndef EGREEDYAGENTFACTORY_H
#define EGREEDYAGENTFACTORY_H

#include "AgentFactory.h"

#include "../Agent/EGreedyAgent.h"
#include "../MDP/CModel.h"
#include "../MDPDistribution/DirMultiDistribution.h"

#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	EGreedyAgentFactory
	\author 	Castronovo Michael
	
	\brief 	An AgentFactory which generates EGreedyAgent's.
	
	          'init()' must be called after this AgentFactory has been
	          deserialized.

	\date 	2015-01-20
*/
// ===========================================================================
/* final */ class dds::EGreedyAgentFactory : public dds::AgentFactory
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the EGreedyAgentFactory to load.
					(can either be compressed or uncompressed)
		*/
		EGreedyAgentFactory(std::istream& is);
		
		
		/**
			\brief		Constructor.
			
			\param[minEps_	The minimal value of epsilon.
			\param[maxEps_	The maximal value of epsilon.
		*/
		EGreedyAgentFactory(double minEps_, double maxEps_);
		
		
		/**
               \brief    Destructor.
		*/
		~EGreedyAgentFactory();
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "EGreedyAgentFactory"; }

		
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
		   return EGreedyAgentFactory::toString();
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
			\brief	The minimal and maximal values of epsilon
					respectively.
		*/
		double minEps, maxEps;

		
		/**
               \brief    The initial model used by the EGreedyAgent generated.
		*/
		CModel* iniModel;
		
		
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
