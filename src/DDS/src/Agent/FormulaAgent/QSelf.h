
#ifndef QSELF_H
#define QSELF_H

#include "QVar.h"
#include "../../dds.h"
#include "../../MDPDistribution/DirMultiDistribution.h"


// ===========================================================================
/*
	\class 	QSelf
	\author 	Castronovo Michael
	
	\brief 	A variable for the FormulaAgent's, which represents a
	          Q-function based on the 'self' model of a prior distribution.

	\date 	2014-12-13
*/
// ===========================================================================
/* final */ class dds::QSelf : public QVar
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
          /**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the QSelf to load.
					(can either be compressed or uncompressed)
		*/
		QSelf(std::istream& is);
		
		
		/**
               \brief    Constructor.
		*/
		QSelf();

          
          /**
			\brief	Destructor.
		*/
		~QSelf();
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "QSelf"; }


		// =================================================================
		//	Public methods
		// =================================================================
		/**
               \brief    Initialization.
                         (used to initialize 'iniModel')
		*/
		void init(const dds::MDPDistribution* mdpDistrib)
                                                  throw (FVariableException);


          /**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const
		{
		   return QSelf::toString();
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
