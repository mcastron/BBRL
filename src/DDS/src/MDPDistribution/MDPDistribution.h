
#ifndef MDPDISTRIBUTION_H
#define MDPDISTRIBUTION_H

#include "../dds.h"
#include "../MDP/MDP.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/**
	\class 	MDPDistribution
	\author 	Castronovo Michael
	
	\brief 	Interface of an MDP distribution generating MDP problems for:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)

	\date 	2014-10-03
*/
// ===========================================================================
class dds::MDPDistribution : public IMDPDistribution
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief		     Constructor.

			\param[name	     The name of this MDPDistribution.
			\param[shortName	The short name of this IMDPDistribution.
		*/
		MDPDistribution(std::string name, std::string shortName);
		
		
		/**
			\brief	Destructor.
		*/
		virtual ~MDPDistribution() {}


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "MDPDistribution"; }
		
          
          /**
		     \brief    Parse the standard program options in order to
		               to instanciate an MDPDistribution.
		     
		     \return   The Agent found (0 if it fails).
		*/
		static MDPDistribution* parse(int argc, char* argv[])
		                            throw (utils::parsing::ParsingException);


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Draw and return a MDP from this MDP distribution.
			
			\return	A MDP drawn from this MDP distribution.
		*/
		virtual MDP* draw() const = 0;
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const
		{
			return MDPDistribution::toString();
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
		MDPDistribution() : IMDPDistribution() {}
};

#endif
