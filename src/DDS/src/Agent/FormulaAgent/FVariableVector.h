
#ifndef FVariableVector_H
#define FVariableVector_H

#include "FVariable.h"
#include "../../dds.h"
#include "../../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	FVariableVector
	\author 	Castronovo Michael
	
	\brief 	Defines a std::vector of FVariables which also add
	          serialization.

	\date 	2015-06-08
*/
// ===========================================================================
/* final */class dds::FVariableVector : public Serializable,
                                 public std::vector<dds::FVariable*>
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the FVariableVector to load.
					(can either be compressed or uncompressed)
		*/
		FVariableVector(std::istream& is);
		
		
		/**
			\brief    Constructor.
		*/
		FVariableVector() {}
		
		
		/**
               \brief    Constructor.
               
               \param[v  A vector containing FVariables.
		*/
		FVariableVector(std::vector<dds::FVariable*> v);


          // =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "FVariableVector"; }
		
		
		/**
		     \brief    Parse the standard program options in order to
		               to instanciate a FVariableVector.
		     
		     \return   The FVariableVector found (0 if it fails).
		*/
		static FVariableVector* parse(int argc, char* argv[])
                                   throw (utils::parsing::ParsingException);


		// =================================================================
		//	Public methods
		// =================================================================	
          /**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const
		{
		   return FVariableVector::toString();
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
};

#endif
