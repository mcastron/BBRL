
#ifndef IMDPDISTRIBUTION_H
#define IMDPDISTRIBUTION_H

#include "MDPDistributionException.h"
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	IMDPDistribution
	\author 	Castronovo Michael
	
	\brief 	Interface of a RL MDP distribution.
			
	\date 	2013-08-13
*/
// ===========================================================================
class IMDPDistribution : public Serializable
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief			Constructor.
			
			\param[name_		The name of this IMDPDistribution.
			\param[shortName_	The short name of this IMDPDistribution.
		*/
		IMDPDistribution(
				std::string name_ = "",
				std::string shortName_ = "") :
					Serializable(), name(name_), shortName(shortName_) {}
		
		
		/**
			\brief	Destructor.
		*/
		virtual ~IMDPDistribution() {}
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "IMDPDistribution"; }
		
		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the name of this IMDPDistribution.
			
			\return	The name of this IMDPDistribution.
		*/
		std::string getName() const { return name; }


		/**
			\brief	Return the short name of this IMDPDistribution.
			
			\return	The short name of this IMDPDistribution.
		*/
		std::string getShortName() const { return shortName; }

		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const
		{
			return IMDPDistribution::toString();
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
	
	
	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The name of this IMDPDistribution.
		*/
		std::string name;


		/**
			\brief	The short name of this IMDPDistribution.
		*/
		std::string shortName;
};

#endif
