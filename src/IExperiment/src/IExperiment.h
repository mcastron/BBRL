
#ifndef IEXPERIMENT_H
#define IEXPERIMENT_H

#include "ExperimentException.h"
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	IExperiment
	\author 	Castronovo Michael
	
	\brief 	Interface of a RL Experiment.
			
	\date 	2013-08-13
*/
// ===========================================================================
template<typename AgentType, typename MDPType, typename SimulationRecordType>
class IExperiment : public Serializable
{
	public:
		// =================================================================
		//	Public Constructors/Destructors
		// =================================================================
		/**
			\brief		Constructor.
			
			\param[name_	The name of this IExperiment.
		*/
		IExperiment(std::string name_ = "") : Serializable(), name(name_) {}
		
		
		/**
			\brief			Constructor.
							The responsibility to delete the MDPs
							is transfered to this Experiment.
							
			\param[name_		The name of this Experiment.
			\param[mdpList_	The test MDPs.
			\param[nbSimPerMDP_	The number of simulations per MDP.
		*/
		IExperiment(	std::string name_,
					std::vector<MDPType*>& mdpList_,
					unsigned int nbSimPerMDP_);
		
		
		/**
			\brief	Destructor.
		*/
		virtual ~IExperiment();


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "IExperiment"; }


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief			Run the experiment for a certain amount
							of time 'timeLimit'.
							Continue from when it stops since the last
							call of 'run()'.
			
			\param[agent		The Agent to test.
			\param[timeLimit	The time limit (in ms)
							(0: perform all simulations without stopping)
			\param[nThreads	The number of threads to use to perform
							the simulations.
		*/
		void run(AgentType* agent,
				double timeLimit_ = 0.0, unsigned int nbThreads = 1);
		
		
		/**
			\brief	Delete all previous results computed so far.
		*/
		void clear();
		
		
		/**
			\brief	Return the number of test MDPs used in this
					Experiment.
			
			\return	The number of test MDPs used in this Experiment.
		*/
		unsigned int getNbOfMDPs() const { return mdpList.size(); }


		/**
			\brief	Return the progress of this Experiment (in [0; 1]).
			
			\return	The progress of this Experiment (in [0; 1]).
		*/
		double getProgress() const
		{
			return (nextMDP / ((double) mdpList.size()));
		}

	
		/**
			\brief	Return the time elapsed during this Experiment
					(in ms).
			
			\return	The time elapsed during this Experiment (in ms).
		*/
		double getTimeElapsed() const { return timeElapsed; }
		
		
		/**
			\brief	Compute and return the list of discounted sum of
					rewards of the simulations discounted by 'gamma'
					(in ]0; 1]).
					If 'gamma' is not provided (or invalid), the discount
					factor used during the simulations will be used
					instead.
			
			\return	The list of discounted sum of rewards of the
					simulations, discounted by 'gamma' (in ]0; 1]).
		*/
		std::vector<double> computeDSRList(double gamma = -1.0) const;
		
		
		/**
			\brief	Return the list of SimulationRecords from the
					last call of 'run()'.
					
			\return	The list of Simulation Records from the last call of 
					'run()'.
		*/
		std::vector<SimulationRecordType*> getSimRecList() const
		{
			return simRecList;
		}

		
		/**
			\brief	Return the name of this IExperiment.
			
			\return	The name of this IExperiment.
		*/
		std::string getName() const { return name; }

		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const
		{
			return IExperiment::toString();
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
		//	Protected attributes
		// =================================================================
		/**
			\brief	The test MDPs.
		*/
		std::vector<MDPType*> mdpList;
		
		
		/**
			\brief	The number of simulations per MDP.
		*/
		unsigned int nbSimPerMDP;
		
		
		/**
			\brief	The time elapsed during this Experiment (in ms).
		*/
		double timeElapsed;
		
		
		/**
			\brief	The list of SimulationRecords osbserved.
		*/
		std::vector<SimulationRecordType*> simRecList;
		
		
		/**
			\brief	The index of the next MDP to simulate.
		*/
		unsigned int nextMDP;


		/**
			\brief	A mutex on 'nextMDP'
					(only for multi-thread).
		*/
		pthread_mutex_t m_nextMDP;
		
		
		/**
			\brief	The list of copies of 'agent' associated to each
					SimulationThread
					(only for multi-thread).
		*/
		std::vector<AgentType*> copyAgentList;
		
		
		/**
			\brief	When 'run()' is called, represents the chronometer
					measuring the time elapsed since 'run()' has been
					called.
		*/
		utils::Chrono chrono;
		
		
		/**
			\brief	When 'run()' is called, represents the time limit
					to perform as much simulations as possible.
					(only for multi-thread).
		*/
		double timeLimit;
	
	
		// =================================================================
		//	Protected methods
		// =================================================================
		/**
			\brief		Perform a single simulation.
			
			\param[agent	The agent interacting with the MDP.
			\param[mdp	The MDP representing the environment.
			
			\return		A SimulationRecord storing all the data
						about the simulation performed.
		*/
		virtual SimulationRecordType
				simulate(AgentType* agent, MDPType* mdp) const = 0;


	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The name of this IExperiment.
		*/
		std::string name;
		
		
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
		
		
		// =================================================================
		//	Private Classes
		// =================================================================
		/**
		*/
		class SimulationThread : public utils::Thread
		{			
			public:
				// =======================================================
				//	Public Constructors
				// =======================================================
				/**
					\brief	Constructor.
					
					\param[ID_	The ID of this SimulationThread.
					\param[exp_	The Experiment from which to perform
								the simulations.
				*/
				SimulationThread(unsigned int ID_, IExperiment* exp_) :
						Thread(),
						ID(ID_), exp(exp_) {}
			
				
				// =======================================================
				//	Public methods
				// =======================================================
				/**
					\brief	Run the SimuationThread.
				*/
				void* run();


			private:				
				// =======================================================
				//	Private attributes
				// =======================================================
				/**
					\brief	The ID of this SimulationThread.
				*/
				unsigned int ID;
				
				
				/**
					\brief	The Experiment from which to perform
							the simulations.
				*/
				IExperiment* exp;
		};


		// =================================================================
		//	Friendships
		// =================================================================
		/**
			\brief	Allow the SimulationThreads to access the mutexes and
					various data structures in order to perform the
					requested simulations.
		*/
		friend class SimulationThread;
};


#include "IExperiment.tpp"

#endif
