
#ifndef QUNIFORM_H
#define QUNIFORM_H

#include "QVar.h"
#include "../../dds.h"
#include "../../MDPDistribution/DirMultiDistribution.h"


// ===========================================================================
/*
	\class 	QUniform
	\author 	Castronovo Michael
	
	\brief 	A variable for the FormulaAgent's, which represents a
	          Q-function based on the 'uniform' model of a prior distribution
	          (each state is reachable from the other states).

	\date 	2015-01-19
*/
// ===========================================================================
/* final */ class dds::QUniform : public QVar
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
          /**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the QUniform to load.
					(can either be compressed or uncompressed)
		*/
		QUniform(std::istream& is);
		
		
		/**
               \brief    Constructor.
		*/
		QUniform();

          
          /**
			\brief	Destructor.
		*/
		~QUniform();
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "QUniform"; }


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
		   return QUniform::toString();
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
