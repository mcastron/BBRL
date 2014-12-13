
#ifndef MODEL_H
#define MODEL_H

#include "MDP.h"


// ===========================================================================
/*
	\class 	Model
	\author 	Castronovo Michael
	
	\brief 	Interface of a model of a MDP.
	          It has been designed to address:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)
			
	\date 	2013-12-13
*/
// ===========================================================================
class dds::Model : public MDP
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================	
		/**
			\brief			Constructor.
			
			\param[name		The name of this MDP.
			\param[nX			The number of states.
			\param[nU			The number of actions.
			\param[iniState	The initial state (-1: drawn randomly)
			\param[P			The probability matrix. (size: nX * nU * nX)
			\param[rType		The type of rewards.
			\param[R			The list of rewards means.
							(size: nX * nU * nX)
			\param[V			The list of rewards variances
							(size: nX * nU * nX or 0;
							       if 0: no variance).
		*/
		Model(	std::string name,
				unsigned int nX, unsigned int nU,
				int iniState,
				const std::vector<double>& P,
				RewardType rType,
				const std::vector<double>& R,
				const std::vector<double>& V);


		/**
			\brief	Destructor.
		*/
		virtual ~Model() {}


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "Model"; }


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Update the Model with respect to transition
					<x, u, y, r>.
			
			\param[x	An origin state.
			\param[u	An action.
			\param[y	A destination state.
			\param[r	The reward observed.
		*/
		virtual void update(
			unsigned int x, unsigned int u, unsigned int y, double r) = 0;
		
		
		/**
			\brief	Return a clone of this Model.
			
			\return	A clone of this Model.
		*/
		virtual Model* clone() const = 0;
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const
		{
			return Model::toString();
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
		Model() : MDP() {}
};

#endif
