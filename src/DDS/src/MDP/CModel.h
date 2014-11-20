
#ifndef CMODEL_H
#define CMODEL_H

#include "Model.h"


// ===========================================================================
/**
	\class 	CModel
	\author 	Castronovo Michael
	
	\brief 	A RL Model of a MDP for:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)
			based on counters.
			The reward function remains unchanged.
			
	\date 	2013-08-13
*/
// ===========================================================================
/* final */ class dds::CModel : public dds::Model
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the CModel to load.
					(can either be compressed or uncompressed)
		*/
		CModel(std::istream& is);


		/**
			\brief			Constructor.
			
			\param[name		The name of this MDP.
			\param[nX			The number of states.
			\param[nU			The number of actions.
			\param[iniState	The initial state (-1: drawn randomly)
			\param[N_			The counters matrix. (size: nX * nU * nX)
			\param[rType		The type of rewards.
			\param[R			The list of rewards means.
							(size: nX * nU * nX)
			\param[V			The list of rewards variances.
							(size: nX * nU * nX or 0;
							       if 0: no variance).
		*/
		CModel(	std::string name,
				unsigned int nX, unsigned int nU,
				int iniState,
				const std::vector<double>& N_,
				RewardType rType,
				const std::vector<double>& R,
				const std::vector<double>& V);


		/**
			\brief	Destructor
		*/
		~CModel() {}


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "CModel"; }


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the list counters for each transition.
			
			\return	The list of counters for each transition.
		*/
		const std::vector<double>& getN() const { return N; }


		/**
			\brief	Return the list of counters for each <state, action>
					pair.
					
			\return	The list of counters for each <state, action> pair.
		*/
		const std::vector<double>& getNp() const { return Np; }
		
		
		/**
			\brief	Update the Model with respect to transition
					<x, u, y, r>.
			
			\param[x	An origin state.
			\param[u	An action.
			\param[y	A destination state.
			\param[r	Not used.
		*/
		void update(unsigned int x, unsigned int u, unsigned int y, double);
		
		
		/**
			\brief	Update the counters matrices with respect to
					transition <x, u, y>.
					Notice that the transition matrix remains unchanged.
			
			\param[x	An origin state.
			\param[u	An action.
			\param[y	A destination state.
		*/
		void updateN(unsigned int x, unsigned int u, unsigned int y);
		
		
		/**
			\brief	Update the transition matrix of this Model with
					respect to the counters matrices.
		*/
		void updateP();
		
		
		/**
			\brief	Update the transition matrix of this Model with
					respect to the counters matrices for the state-action
					pair <x, u>.
					
			\param[x	An origin state.
			\param[u	An action.	
		*/
		void updateP(unsigned int x, unsigned int u);
		
		
		/**
			\brief	Return a clone of this CModel.
			
			\return	A clone of this CModel.
		*/
		CModel* clone() const
		{
			return cloneInstance<CModel>(this);
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const { return CModel::toString(); }

		
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
		void deserialize(std::istream& is)
									throw (SerializableException);
		
		
	protected:
		// =================================================================
		//	Protected attributes
		// =================================================================
		/**
			\brief	The counters matrix. (size: nX * nU * nX)
		*/
		std::vector<double> N;
		
		
		/**
			\brief	The sum of counters values of 'N' for each
					state-action pair. (size: nX * nU)
		*/
		std::vector<double> Np;


	private:
		// =================================================================
		//	Private static methods
		// =================================================================
		/**
			\brief	Compute the transition matrix from the counters
					matrix.
		*/
		static std::vector<double> computeP(
				unsigned int nX, unsigned int nU,
				const std::vector<double>& N);

	
		// =================================================================
		//	Private methods
		// =================================================================
		#ifndef NDEBUG
		/**
			\brief	Function to call whenever the internal data could
					be corrupted by wrong data entries
					(calls 'assert()' for each data).
		*/
		void checkIntegrity() const;
		#endif
};

#endif
