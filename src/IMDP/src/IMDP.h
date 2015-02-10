
#ifndef IMDP_H
#define IMDP_H

#include "MDPException.h"
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	IMDP
	\author 	Castronovo Michael
	
	\brief 	Interface of a RL MDP.
			
	\date 	2013-10-13
*/
// ===========================================================================
class IMDP : public Serializable
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief		Constructor.
			
			\param[name_	The name of this IMDP.
		*/
		IMDP(std::string name_ = "no name") : Serializable(), name(name_) {}
		
		
		/**
			\brief	Destructor.
		*/
		virtual ~IMDP() {}
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "IMDP"; }
		
		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the name of this IMDP.
			
			\return	The name of this IMDP.
		*/
		std::string getName() const { return name; }

		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const { return IMDP::toString(); }
		
		
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
		//	Protected methods
		// =================================================================
		/**
			\brief		Set a new name for this IMDP.
			
			\param[name_	The new name for this IMDP.
		*/
		void setName(std::string name_) { name = name_; }


	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The name of this IMDP.
		*/
		std::string name;
};

#endif
