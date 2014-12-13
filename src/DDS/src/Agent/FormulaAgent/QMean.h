
#ifndef QMEAN_H
#define QMEAN_H

#include "QVar.h"
#include "../../dds.h"
#include "../../MDPDistribution/DirMultiDistribution.h"


// ===========================================================================
/*
	\class 	QMean
	\author 	Castronovo Michael
	
	\brief 	A variable for the FormulaAgent's, which represents a
	          Q-function based on the mean model of a prior distribution.

	\date 	2014-12-13
*/
// ===========================================================================
/* final */ class dds::QMean : public QVar
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
          /**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the QMean to load.
					(can either be compressed or uncompressed)
		*/
		QMean(std::istream& is);
		
		
		/**
               \brief    Constructor.
		*/
		QMean();

          
          /**
			\brief	Destructor.
		*/
		~QMean();
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "QMean"; }


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
		   return QMean::toString();
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
