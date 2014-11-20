
#ifndef MDP_H
#define MDP_H

#include "../dds.h"
#include "../../ExternalLibs.h"


//	Note:
//		Update 'checkRewardType()' private method whenever this enum type
//		is modified.
enum RewardType
{
	RT_CONSTANT,
	RT_GAUSSIAN
};


// ===========================================================================
/**
	\class 	MDP
	\author 	Castronovo Michael
	
	\brief 	Interface of a RL MDP problem for:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)
			
	\date 	2013-08-13
*/
// ===========================================================================
class dds::MDP : public IMDP
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the MDP to load.
					(can either be compressed or uncompressed)
		*/
		MDP(std::istream& is);
		
		
		/**
			\brief			Constructor.
			
			\param[name		The name of this MDP.
			\param[nX_		The number of states.
			\param[nU_		The number of actions.
			\param[iniState_	The initial state. (-1: drawn randomly)
			\param[P_			The probability matrix. (size: nX * nU * nX)
			\param[rType_		The type of rewards.
			\param[R_			The list of rewards means.
							(size: nX * nU * nX)
			\param[V_			The list of rewards variances.
							(size: nX * nU * nX or 0; if 0: no variance)
		*/
		MDP(	std::string name,
			unsigned int nX_, unsigned int nU_,
			int iniState_,
			const std::vector<double>& P_,
			RewardType rType_,
			const std::vector<double>& R_, const std::vector<double>& V_);


		/**
			\brief	Destructor.
		*/
		virtual ~MDP() {}
		

		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief		Throw a MDPException if the given RewardType
						element is invalid.
					
			\param[rType_	The RewardType element to check.
		*/
		static void checkRewardType(const RewardType rType_)
											throw (MDPException);


		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "MDP"; }


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the number of states.
			
			\retur	The number of states.
		*/
		unsigned int getNbStates() const { return nX; }
		
		
		/**
			\brief	Return the number of actions.
			
			\return	The number of actions.
		*/
		unsigned int getNbActions() const { return nU; }
		
		
		/**
			\brief	Return the current state.
			
			\return	The current state.
		*/
		unsigned int getCurrentState() const { return xt; }
		
		
		/**
			\brief	Perform the given action and return the reached
					state and the corresponding reward.
					
			\param[u	The action to perform.
			\param[y	The reached state.
			\param[r	The reward gained.
		*/
		void perform(unsigned int u, unsigned int& y, double& r);
		
		
		/**
			\brief	Reset this MDP.
		*/
		void reset();
		
		
		/**
			\brief		Run Value-Iteration algorithm on this MDP.
			
			\param[gamma	The discount factor. (in ]0; 1[)
			\param[T		The horizon limit.
			\param[pV		The result of the last call of 'valueIteration()'
						(can improve convergence speed if provided).
			
			\return		The result of Value-Iteration. (size: nX)
		*/
		std::vector<double> valueIteration(
				double gamma, unsigned int T,
				std::vector<double> pV = std::vector<double>()) const
											throw (MDPException);


		/**
			\brief		Run Q-Iteration algorithm on this MDP.
			
			\param[gamma	The discount factor. (in ]0; 1[)
			\param[T		The horizon limit.
			\param[pQ		The result of the last call of 'qIteration()'
						(can improve convergence speed if provided).
			
			\return		The result of Q-Iteration. (size: nX * nU)
		*/
		std::vector<double> qIteration(double gamma, unsigned int T,
				std::vector<double> pQ = std::vector<double>()) const
											throw (MDPException);
		
		
		/**
			\brief	Return true if this MDP is 'known', false else.
			
			\return	True if this MDP is 'known', false else.
		*/
		bool isKnown() const { return known; }
		
		
		/**
			\brief	If 'known', return the initial state of this
					MDP, throw a MDPException else.
			
			\return	The initial state of this MDP.
		*/
		int getIniState() const throw (MDPException)
		{
			if (!isKnown())
			{
				std::string msg;
				msg += "Cannot retrieve the initial state of an ";
				msg += "unknown MDP!\n";
				
				throw MDPException(msg);
			}
			
			return iniState;
		}
		
		
		/**
			\brief	If 'known', return the transition matrix of this
					MDP, throw a MDPException else.
					(size: nX * nU * nX)
			
			\return	The transition matrix of this MDP. (size: nX * nU * nX)
		*/
		const std::vector<double>& getP() const throw (MDPException)
		{
			if (!isKnown())
			{
				std::string msg;
				msg += "Cannot retrieve the transition matrix of an ";
				msg += "unknown MDP!\n";
				
				throw MDPException(msg);
			}
			
			return P;
		}
		
		
		/**
			\brief	If 'known', return the type of rewards of this
					MDP, throw a MDPException else.
			
			\return	The type of rewards of this MDP.
		*/
		RewardType getRType() const throw (MDPException)
		{
			if (!isKnown())
			{
				std::string msg;
				msg += "Cannot retrieve the type of rewards of an ";
				msg += "unknown MDP!\n";
				
				throw MDPException(msg);
			}
			
			return rType;
		}
		
		
		/**
			\brief	If 'known', return the the list of rewards means of
					this	MDP, throw a MDPException else.
					(size: nX * nU * nX)
			
			\return	The list of rewards means of this MDP.
					(size: nX * nU * nX)
		*/
		const std::vector<double>& getR() const throw (MDPException)
		{
			if (!isKnown())
			{
				std::string msg;
				msg += "Cannot retrieve the list of rewards means of an ";
				msg += "unknown MDP!\n";
				
				throw MDPException(msg);
			}
			
			return R;
		}
		
		
		/**
			\brief	If 'known', return the the list of rewards variances
					of this MDP, throw a MDPException else.
					(size: nX * nU * nX)
			
			\return	The list of rewards variances of this MDP.
					(size: nX * nU * nX)
		*/
		const std::vector<double>& getV() const throw (MDPException)
		{
			if (!isKnown())
			{
				std::string msg;
				msg += "Cannot retrieve the list of rewards variances of ";
				msg += "an unknown MDP!\n";
				
				throw MDPException(msg);
			}
			
			return V;
		}


		/**
			\brief		Set a new name to this MDP.
			
			\param[name	The new name of this MDP.
		*/
		void setName(std::string name) { IMDP::setName(name); }
		
		
		/**
			\brief	Return a clone of this MDP.
			
			\return	A clone of this MDP.
		*/
		virtual MDP* clone() const
		{
			return cloneInstance<MDP>(this);
		}

		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const { return MDP::toString(); }

		
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
		MDP() : IMDP() {}
		
		
		// =================================================================
		//	Protected attributes
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
			\brief	The probability matrix. (size: nX * nU * nX)
		*/
		std::vector<double> P;
		
		
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
		
		
		/**
			\brief	The current state.
		*/
		unsigned int xt;
		
		
		/**
			\brief	The connexity map, which, for each state, maintain
					a list of reachable states.
					Used to boost Value-Iteration and Q-Iteration
					algorithms.
		*/
		std::vector<std::set<unsigned int> > connexityMap;


	private:
		// =================================================================
		//	Private attribues
		// =================================================================
		/**
			\brief	False if this MDP is unknown, prohibiting the usage
					of some functions (see the documentation).
		*/
		bool known;
		
		
		// =================================================================
		//	Private methods
		// =================================================================
		/**
			\brief	Set this MDP as 'known'.
		*/
		void setKnown()
		{
			known = true;
			
			
			//	Check integrity
			#ifndef NDEBUG
			checkIntegrity();
			#endif
		}
		
		
		/**
			\brief	Set this MDP as 'unknown'.
		*/
		void setUnknown()
		{
			known = false;
			
			
			//	Check integrity
			#ifndef NDEBUG
			checkIntegrity();
			#endif
		}

		
		/**
			\brief	Compute the connexity map from the transition
					matrix.
		*/
		static std::vector<std::set<unsigned int> >
				computeConnexityMap(
					unsigned int nX, unsigned int nU,
					const std::vector<double>& P);
		
		
		#ifndef NDEBUG
		/**
			\brief	Function to call whenever the internal data could
					be corrupted by wrong data entries
					(calls 'assert()' for each data).
		*/
		void checkIntegrity() const;
		#endif


	// ======================================================================
	//	Friendships
	// ======================================================================
	/**
		\brief	Used to give acces to 'setKnown()/setUnknown()' private
				methods to the function simulating a RL single trajectory.
	*/
	friend simulation::SimulationRecord simulation::simulate(
			Agent*, MDP*, double, unsigned int, bool);
};

#endif
