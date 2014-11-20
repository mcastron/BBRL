
#ifndef VDBEEGREEDYAGENTFACTORY_H
#define VDBEEGREEDYAGENTFACTORY_H

#include "AgentFactory.h"

#include "../Agent/VDBEEGreedyAgent.h"
#include "../MDP/CModel.h"
#include "../MDPDistribution/DirMultiDistribution.h"

#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/**
	\class 	VDBEEGreedyAgentFactory
	\author 	Castronovo Michael
	
	\brief 	Interface of an VDBEEGreedyAgentFactory to generate RL Agents.

	\date 	2014-10-03
*/
// ===========================================================================
/* final */ class dds::VDBEEGreedyAgentFactory : public dds::AgentFactory
{		
	public:
		// =================================================================
		//	Public Constructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the VDBEEGreedyAgentFactory to load.
					(can either be compressed or uncompressed)
		*/
		VDBEEGreedyAgentFactory(std::istream& is);
		
		
		/**
			\brief		Constructor.
			
			\param[minSigma_	The minimal value of sigma.
			\param[maxSigma_	The maximal value of sigma.
			\param[minDelta_	The minimal value of delta.
			\param[maxDelta_	The maximal value of delta.
			\param[minIniEps_	The minimal value of the initial epsilon.
			\param[maxIniEps_	The maximal value of the initial epsilon.
			\param[minC_		The minimal initial value of a CModel
							counter.
			\param[maxC_		The maximal initial value of a CModel
							counter.
		*/
		VDBEEGreedyAgentFactory(
				double minSigma_, double maxSigma_,
				double minDelta_, double maxDelta_,
				double minIniEps_, double maxIniEps_,
				double minC_, double maxC_);

		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "VDBEEGreedyAgentFactory"; }

		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief			Initialize this AgentFactory with respect
							to the given MDP distribution.
			
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
		   return VDBEEGreedyAgentFactory::toString();
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
               \brief    The short name of the distribution used to initial
                         this AgentFactory.
		*/
		std::string shortDistribName;

		
		/**
			\brief	The minimal and maximal values of sigma
					respectively.
		*/
		double minSigma, maxSigma;


		/**
			\brief	The minimal and maximal values of delta
					respectively.
		*/
		double minDelta, maxDelta;
		
		
		/**
			\brief	The minimal and maximal values of the initial epsilon
					respectively.
		*/
		double minIniEps, maxIniEps;
		
		
		/**
			\brief	The minimal and maximal initial values of a CModel
					counter respectively.
		*/
		double minC, maxC;
		
		
		/**
			\brief	The number of states.
		*/
		unsigned int nX;


		/**
			\brief	The number of actions.
		*/
		unsigned int nU;
		
		
		/**
			\brief	The initial state.
		*/
		int iniState;
		
		
		/**
			\brief	The type of rewards.
		*/
		RewardType rType;
		
		
		/**
			\brief	The list of rewrads means.
		*/
		std::vector<double> R;
		
		
		/**
			\brief	The list of rewards variances.
		*/
		std::vector<double> V;
		
		
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
