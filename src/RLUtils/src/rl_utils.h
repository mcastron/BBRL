
#ifndef RLUTILS_H
#define RLUTILS_H

#include "../ExternalLibs.h"


// ===========================================================================
/**
	\namespace	rl_utils
	\author 		Castronovo Michael
	
	\brief		This namespace defines useful templates for RL elements.
								
	\date		2015-01-14
*/
// ===========================================================================
namespace rl_utils
{
	// ======================================================================
	//	Namespaces
	// ======================================================================
	/**
		\brief	This namespace gathers several templates related to
				an RL simulation.
	*/
	namespace simulation
	{
		// =================================================================
		//	Classes
		// =================================================================
		/**
			\brief	Represent a transition observed during the
					interaction between an Agent and a MDP.
					
						XType:	The type representing a state.
						UType:	The type representing an action.
						RType:	The type representing a reward.
		*/
		template<typename XType, typename UType, typename RType>
		class Transition : public Serializable
		{
			public:
				// =======================================================
				//	Public attributes
				// =======================================================
				/**
					\brief	The origin state.
				*/
				XType x;
				
				
				/**
					\brief	The action performed.
				*/
				UType u;
				
				
				/**
					\brief	The reached state.
				*/
				XType y;
				
				
				/**
					\brief	The reward observed.
				*/
				RType r;
			
				
				// =======================================================
				//	Public Constructor
				// =======================================================
				/**
					\brief	Constructor.
					
					\param[x_	The origin state.
					\param[u_	The action performed.
					\param[y_	The reached state.
					\param[r_	The reward observed.
				*/
				Transition(
					XType x_, UType u_, XType y_, RType r_) :
							x(x_), u(u_), y(y_), r(r_) {}
		};
		
		
		/**
			\brief	Represent the data gathered during the interaction
					between an Agent and a MDP over a single trajectory.
					
						XType:	The type representing a state.
						UType:	The type representing an action.
						RType:	The type representing a reward.
		*/
		template<typename XType, typename UType, typename RType>
		class SimulationRecord : public Serializable
		{
			public:				
				// =======================================================
				//	Public Constructor
				// =======================================================
				/**
					\brief	Constructor.
					
					\param[is	The 'ifstream' containting the data 
							representing the SimulationRecord to load.
				*/
				SimulationRecord(std::istream& is) : Serializable()
				{
					deserialize(is);
				}


				/**
					\brief			Construction.
					
					\param[simGamma_	The discount factor used
									during the simulation.
				*/
				SimulationRecord(double simGamma_ = 1.0);

				
				// =======================================================
				//	Public static methods
				// =======================================================
				/**
					\brief	Return the string representation of this
							class name.
			
					\return	The string representation of this class
							name.
				*/
				static std::string toString()
				{
					return "SimulationRecord";
				}
				
				
				// =======================================================
				//	Public methods
				// =======================================================
				/**
					\brief	Add Transition 't'.
					
					\param[t	The Transition to add.
				*/
				void add(const Transition<XType, UType, RType>& t)
				{
					transitionList.push_back(t);
				}
				
				
				/**
					\brief	Return the list of states crossed.
					
					\return	Return the list of states crossed.
				*/
				std::vector<XType> getCrossedStateList() const;
				
				
				/**
					\brief	Return the list of rewards observed.
					
					\return	The list of rewards observed.
				*/
				std::vector<RType> getRewardList() const;
				
				
				/**
					\brief	Return the list of transitions observed.
					
					\return	The list of transitions observed.
				*/
				std::vector<simulation::Transition<XType, UType, RType> >
						getTransitionList() const
				{
					return transitionList;
				}
				
				
				/**
					\brief	Compute and return the sum of rewards
							observed during this simulation, discounted
							by 'gamma' (in ]0; 1]).
							If 'gamma' is not provided (or invalid),
							the discount factor used during the
							simulation will be used instead.
							
					\return	The sum of reward observed during the
							simulation, discounted by 'gamma'
							(in ]0; 1]).
				*/
				double computeDSR(double gamma = -1.0) const;
				
				
				/**
					\brief	Return the name of the class of this object.
							This method should be overloaded by any
							derived class.
					
					\return	The name of the class of this object.
				*/
				std::string getClassName() const
				{
					return SimulationRecord::toString();
				}

				
				/**
					\brief	Serialize this Object.
							If overloaded, the new 'serialize()' method
							should call the 'serialize()' method of the 
							base class before doing anything else.
				*/
				void serialize(std::ostream& os) const;
				
				
				/**
					\brief	Deserialize this Object.
							If overloaded, the new 'deserialize()'
							method should call the 'deserialize()'
							method of the base class before doing
							anything else.
				*/
				void deserialize(std::istream& is)
									throw (SerializableException);
				
			
			private:	
				// =======================================================
				//	Private attributes
				// =======================================================
				/**
					\brief	The discount factor used during the
							simulation.
				*/
				double simGamma;
				
					
				/**
					\brief	The list of Transitions observed.
				*/
				std::vector<Transition<XType, UType, RType> >
						transitionList;
		};


		// =================================================================
          //   Functions
		// =================================================================
		/**
			\brief	Compute and return the sum of rewards
					observed during this simulation, discounted
					by 'gamma' (in ]0; 1]).
					
			\return	The sum of reward observed during the
					simulation, discounted by 'gamma'
					(in ]0; 1]).
		*/
		double computeDSR(const std::vector<double>& rewardList, double gamma);
	}
}

#include "rl_utils.tpp"

#endif
