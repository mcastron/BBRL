
#ifndef FVARIABLE_H
#define FVARIABLE_H

#include "FVariableException.h"
#include "../../dds.h"
#include "../../MDP/MDP.h"
#include "../../MDPDistribution/MDPDistribution.h"
#include "../../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	FVariable
	\author 	Castronovo Michael
	
	\brief 	Represent a variable for the FormulaAgent's.

	\date 	2014-12-13
*/
// ===========================================================================
class dds::FVariable : public Serializable
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
		*/
		FVariable() {}


		/**
			\brief	Destructor.
		*/
		virtual ~FVariable() {}


          // =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "FVariable"; }


		// =================================================================
		//	Public methods
		// =================================================================
		/**
               \brief    Initialization.
		*/
		virtual void init(const dds::MDPDistribution*)
                                                  throw (FVariableException){}


		/**
               \brief    Reset.  
		*/
		virtual void reset(const dds::MDP*, double, unsigned int) {}
		
		
		/**
               \brief    Free unnecessary data between two MDP trajectories.
		*/
		virtual void freeData() {}
		
		
		/**
               \brief    Update.
		*/
		virtual void update(
                    unsigned int, unsigned int, unsigned int, double) {}


          /**
               \brief    Evaluate the given <state, action> pair.
          */
		virtual double operator()(const unsigned int&, const unsigned int&)
                                        const throw (FVariableException) = 0;


          /**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const
		{
		   return FVariable::toString();
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
