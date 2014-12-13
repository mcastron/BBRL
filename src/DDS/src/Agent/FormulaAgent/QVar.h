
#ifndef QVAR_H
#define QVAR_H

#include "FVariable.h"
#include "../../dds.h"
#include "../../MDP/CModel.h"


// ===========================================================================
/*
	\class 	QVar
	\author 	Castronovo Michael
	
	\brief 	A variable for the FormulaAgent's, which represents a
	          Q-function model-based variable for FormulaAgent.

	          This base class is used to define everything to manage such
	          a variable except how the initial model is computed from a
	          MDP distribution.

	\date 	2014-12-13
*/
// ===========================================================================
class dds::QVar : public FVariable
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
               \brief    Constructor.
		*/
		QVar();
		
		
          /**
			\brief	Destructor.
		*/
		virtual ~QVar();
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "QVar"; }


		// =================================================================
		//	Public methods
		// =================================================================
		/**
               \brief    Initialization.
                         (used to initialize 'iniModel')
		*/
		virtual void init(const dds::MDPDistribution* mdpDistrib)
                                             throw (FVariableException) = 0;


		/**
               \brief         Reset.
               
               \param[gamma_  The discount factor.
               \param[T_      The horizon limit.               
		*/
		void reset(const dds::MDP*, double gamma_, unsigned int T_);


          /**
               \brief    Free unnecessary data between two MDP trajectories.
		*/
		void freeData();
		

		/**
               \brief    Update.
               
               \param[x	The origin state.
			\param[u	The action performed.
			\param[y	The state reached.
			\param[r	The reward observed.
		*/
		void update(
                    unsigned int x, unsigned int u, unsigned int y, double r);


          /**
               \brief    Evaluate the given <state, action> pair.
          */
		double operator()(const unsigned int& x, const unsigned int& u) const
                                             throw (FVariableException);


          /**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const
		{
		   return QVar::toString();
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
		//	Protected attributes
		// =================================================================
          /**
               \brief    Initial model.
		*/
		dds::CModel* iniModel;


     private:
          // =================================================================
		//	Private methods
		// =================================================================
		/**
               \brief    Current model.
		*/
		dds::CModel* model;
		

          /**
               \brief    Q-function.
          */
		std::vector<double> Q;
		
		
		/**
               \brief    The discount factor.
		*/
		double gamma;
		
		
		/**
               \brief    The horizon limit.
		*/
		unsigned int T;
};

#endif
