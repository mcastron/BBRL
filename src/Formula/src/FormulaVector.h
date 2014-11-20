
#ifndef FORMULAVECTOR_H
#define FORMULAVECTOR_H

#include "Formula.h"
#include "FormulaException.h"
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	Formula
	\author 	Castronovo Michael
	
	\brief 	Defines a std::vector of formulas which also add serialization.

	\date 	2014-10-14
*/
// ===========================================================================
/* final */class FormulaVector : public Serializable,
                                 public std::vector<Formula*>
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the FormulaVector to load.
					(can either be compressed or uncompressed)
		*/
		FormulaVector(std::istream& is);
		
		
		/**
			\brief    Constructor.
		*/
		FormulaVector() {}
		
		
		/**
               \brief    Constructor.
               
               \param[v  A vector containing formulas.
		*/
		FormulaVector(std::vector<Formula*> v);


          // =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "FormulaVector"; }
		
		
		/**
		     \brief    Parse the standard program options in order to
		               to instanciate a FormulaVector.
		     
		     \return   The FormulaVector found (0 if it fails).
		*/
		static FormulaVector* parse(int argc, char* argv[])
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
		   return Serializable::toString();
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
