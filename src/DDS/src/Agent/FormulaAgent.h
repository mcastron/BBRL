
#ifndef FORMULAAGENT_H
#define FORMULAAGENT_H

#include "Agent.h"
#include "../MDP/CModel.h"
#include "../MDPDistribution/MDPDistribution.h"
#include "../MDPDistribution/DirMultiDistribution.h"
#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/**
	\class 	FormulaAgent
	\author 	Castronovo Michael
	
	\brief 	Interface of a FormulaAgent to address MDP problems
	          for:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)
				
		     A formula is used to compute a Q-function, where its variables 
		     are computed through Q-iteration on specific models
		     (updated during the interaction).
		     
		     !!! Only supports formulas with 3 variables !!!
		     (will be more flexible in the future releases)

	\date 	2014-10-14
*/
// ===========================================================================
/* final */class dds::FormulaAgent : public dds::Agent
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
               
               \param[f_ A list of formulas.
		*/
		FormulaAgent(Formula* f_);
		
		
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
		int getAction(int xt) const throw (MDPException);
		
		
		/**
			\brief	Learn from <x, u, y, r> transition (online).
			
			\param[x	The origin state.
			\param[u	The action performed.
			\param[y	The state reached.
			\param[r	The reward observed.
		*/
		void learnOnline(int x, int u, int y, double r) throw (MDPException);

		
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
               \brief    The formula to follow when interacting with an
                         MDP.
		*/
		Formula* f;


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