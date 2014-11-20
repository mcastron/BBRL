
#ifndef DirMultiDISTRIBUTION_H
#define DirMultiDISTRIBUTION_H

#include "MDPDistribution.h"


// ===========================================================================
/**
	\class 	DirMultiDistribution
	\author 	Castronovo Michael
	
	\brief 	Interface of a Dirichlet Multinomial distribution generating
			MDP problems for:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)
			
			The state space, the action space, the reward function and
			the initial state are identical for all the MDPs composing the
			support of this distribution.
			They only differ by their transition matrix, which is drawn
			from a Dirichlet Multinomial distribution. It defines
			a Dirichlet distribution for each state-action pair.
			The DM distribution can be updated. It consists to increment
			the observation counters.

	\date 	2014-08-13
*/
// ===========================================================================
/* final */ class dds::DirMultiDistribution : public dds::MDPDistribution
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'istream' containting the data representing
					the DirMultiDistribution to load.
					(can either be compressed or uncompressed)
		*/
		DirMultiDistribution(std::istream& is);


		/**
			\brief			Constructor.

			\param[name		The name of this DirMultiDistribution.
			\param[nX_		The number of states of the MDPs to
							generate.
			\param[nU_		The number of actions of the MDPs to
							generate.
			\param[iniState_	The initial state of the MDPs to generate.
							(-1: drawn randomly)
			\param[theta_	The list of prior observations used to
							initialize the Dirichlet counters.
							(size: nX * nU * nX)
			\param[rType_		The type of rewards.
			\param[R_			The list of rewards means.
							(size: nX * nU * nX)
			\param[V_			The list of rewards variances.
							(size: nX * nU * nX or 0; if 0: no variance)
		*/
		DirMultiDistribution(
				std::string name,
				unsigned int nX_, unsigned int nU_,
				int iniState_,
				const std::vector<double>& theta_,
				RewardType rType_,
				const std::vector<double>& R_,
				const std::vector<double>& V_);


		/**
			\brief	Destructor.
		*/
		~DirMultiDistribution() {}


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "DirMultiDistribution"; }
		

		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Draw and return a MDP from this MDP distribution.
			
			\return	A MDP drawn from this MDP distribution.
		*/
		MDP* draw() const;
		
		
		/**
			\brief		Update the observation counter of the given
						transition.
			
			\param[x		The origin state.
			\param[u		The action performed.
			\param[y		The reached state.
			\param[inc	The increment of the counter.
		*/
		void update(	unsigned int x, unsigned int u, unsigned int y,
					double inc = 1.0);
		
		
		/**
			\brief	Return the number of states of the MDPs generated
					by this distribution.
			
			\retur	The number of states.
		*/
		unsigned int getNbStates() const { return nX; }
		
		
		/**
			\brief	Return the number of actions of the MDPs generated
					by this distribution.
			
			\return	The number of actions.
		*/
		unsigned int getNbActions() const { return nU; }
		
		
		/**
			\brief	Return the initial state of the MDPs generated
					by this distribution.
					(-1: drawn randomly)
			
			\return	The number of actions.
		*/
		unsigned int getIniState() const { return iniState; }
		
		
		/**
			\brief	Return the observation counters defining this
					distribution.
					(size: nX * nU * nX)
			
			\return	The observation counters defining this distribution.
					(size: nX * nU * nX)
		*/
		const std::vector<double>& getTheta() const throw (MDPException)
		{
			return theta;
		}
		
		
		/**
			\brief	Return the type of rewards of the MDPs generated
					by this distribution.
			
			\return	The type of rewards.
		*/
		RewardType getRType() const throw (MDPException) { return rType; }
		
		
		/**
			\brief	Return the list of rewards means of the MDPs generated
					by this distribution. (size: nX * nU * nX)
			
			\return	The list of rewards means. (size: nX * nU * nX)
		*/
		const std::vector<double>& getR() const throw (MDPException)
		{
			return R;
		}
		
		
		/**
			\brief	Return the list of rewards variances of the MDPs
					generated	by this distribution. (size: nX * nU * nX)
			
			\return	The list of rewards variances. (size: nX * nU * nX)
		*/
		const std::vector<double>& getV() const
									throw (MDPDistributionException)
		{
			return V;
		}

		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const
		{
			return DirMultiDistribution::toString();
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
		void deserialize(std::istream& is)
									throw (SerializableException);


	private:	
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The number of states.
		*/
		unsigned int nX;
		
		
		/**
			\brief	The number of actions.
		*/
		unsigned int nU;
		
		
		/**
			\brief	The initial state.
		*/
		int iniState;
		
		
		/**
			\brief	The list of current observations defining the
					Dirichlet counters. (size: nX * nU * nX)
		*/
		std::vector<double> theta;
		
		
		/**
			\brief	The type of rewards.
		*/
		RewardType rType;
		
		
		/**
			\brief	The list of rewards means. (size: nX * nU * nX)
		*/
		std::vector<double> R;
		
		
		/**
			\brief	The list of rewards variances
					(size: nX * nU * nX or 0; if 0: no variance).
		*/
		std::vector<double> V;
		
		
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
