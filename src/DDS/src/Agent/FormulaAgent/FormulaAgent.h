
#ifndef FORMULAAGENT_H
#define FORMULAAGENT_H

#include "FVariable.h"
#include "QMean.h"
#include "QSelf.h"
#include "QUniform.h"
#include "../Agent.h"
#include "../../MDP/CModel.h"
#include "../../MDPDistribution/MDPDistribution.h"
#include "../../MDPDistribution/DirMultiDistribution.h"
#include "../../dds.h"
#include "../../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	FormulaAgent
	\author 	Castronovo Michael
	
	\brief 	A RL Agent following a policy parametrized by a formula.
	          It has been designed to address:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)
				
		     A formula is used to compute a Q-function, where its variables 
		     are computed through Q-iteration on specific models
		     (updated during the interaction).

	\date 	2015-02-27
*/
// ===========================================================================
/* final */ class dds::FormulaAgent : public dds::Agent
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the FormulaAgent to load.
					(can either be compressed or uncompressed)
		*/
		FormulaAgent(std::istream& is);
		
		
		/**
               \brief    Constructor.
               
               \param[f_           A formula.
               \param[varNameList_ The list of variables' names.
		*/
		FormulaAgent(utils::formula::Formula* f_,
		             const std::vector<std::string>& varNameList_);
		
		
		/**
			\brief	Destructor.
			          'f' is freed.
		*/
		~FormulaAgent();


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString()
		{
		   return "FormulaAgent";
		}


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the action to perform on the currently
					associated MDP from state 'x'.
			
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
		     /* TODO - call 'Serializable::checkIn<>()' in 'dds::init()' */
			return cloneInstance<FormulaAgent>(this);
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const { return FormulaAgent::toString(); }


          /**
               \brief    Return the name of the class of this object, formatted
                         for export files.

               \return   The name of the class of this object, formatted for
                         export files.
		*/
		std::string getExportClassName() const { return "Formula"; }

		
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
               \brief    The formula defining the Q-function to follow.
		*/
		utils::formula::Formula* f;


		/**
               \brief   The list of variables' names.
		*/
		std::vector<std::string> varNameList;


		/**
               \brief    The list of variables.
		*/
		std::vector<FVariable*> varList;


		/**
               \brief    The list of models representing the variables of
                         formula 'f'.
		*/
		std::vector<CModel*> modelList;


		/**
               \brief    The list of initial models representing the variables
                         of formula 'f'.
		*/
		std::vector<CModel*> iniModelList;


          /**
               \brief    The list of Q-functions computed from the models of
                         'modelList'.
          */
          std::vector<std::vector<double> > QList;


		// =================================================================
		//	Private methods
		// =================================================================
		/**
			\brief			Learn from a MDP distribution offline.

							Consist to learn an appropriate initial
							model.
			
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
