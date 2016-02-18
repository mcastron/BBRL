
#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "../dds.h"
#include "../Agent/Agent.h"
#include "../MDP/MDP.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	Experiment
	\author 	Castronovo Michael
	
	\brief 	Represent a RL experiment.
	          It has been designed to address:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)

	\date 	2015-12-18
*/
// ===========================================================================
/* final */ class dds::Experiment :
		public IExperiment<Agent, MDP, simulation::SimulationRecord>
{		
	public:
		// =================================================================
		//	Public Constructors/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the Experiment to load.
					(can either be compressed or uncompressed)
		*/
		Experiment(std::istream& is);

	
		/**
			\brief			Constructor.
							The responsibility to delete the MDPs
							is transfered to this Experiment.
							
			\param[name		The name of this Experiment.
			\param[mdpList		The test MDPs.
			\param[nbSimPerMDP	The number of simulations per MDP.
			\param[simGamma_	The discount factor to use for the
							simulations.
			\param[T_			The horizon limit.
			\param[safeSim_	If true, the MDP is 'unknown', preventing
							the agent to access MDP data
							(e.g.: the transition matrix).
               \param[saveTraj_	True if the complete trajectories must be
			                    saved.
		*/
		Experiment(	std::string name,
					std::vector<MDP*>& mdpList, unsigned int nbSimPerMDP,
					double simGamma_, unsigned int T_,
					bool safeSim_ = false, bool saveTraj = false);
		
		
		/**
			\brief	Destructor.
			          'mdpList' is freed.
		*/
		~Experiment() {}


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "Experiment"; }
		
		
		/**
		     \brief    Parse the standard program options in order to
		               to instanciate an Experiment.
		     
		     \return   The Experiment found (0 if it fails).
		*/
		static Experiment* parse(int argc, char* argv[])
		                            throw (utils::parsing::ParsingException);


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const { return Experiment::toString(); }


          /**
               \brief    Return the discount factor used during the
                         simulations.

               \return   The discount factor used during the simulations.
		*/
		double getSimGamma() const { return simGamma; }
		
		
		/**
               \brief    Return the horizon limit used during the
                         simulations.

               \return   The horizon limit used during the simulations.
		*/
		unsigned int getT() const { return T; }

		
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


	private:
		// =================================================================
		//	Private attributes
		// =================================================================	
		/**
			\brief	The discount factor used for the simulations.
		*/
		double simGamma;
		
		
		/**
			\brief	The horizon limit.
		*/
		unsigned int T;
		
		
		/**
			\brief	If true, the MDP is 'unknown', preventing the agent
					to access MDP data (e.g.: the transition matrix).
		*/
		bool safeSim;

		
		// =================================================================
		//	Private methods
		// =================================================================
		/**
			\brief		Perform a single simulation.
			
			\param[agent	The agent interacting with the MDP.
			\param[mdp	The MDP representing the environment.
			
			\return		A SimulationRecord storing all the data
						about the simulation performed.
		*/
		simulation::SimulationRecord simulate(Agent* agent, MDP* mdp) const
		{
			return simulation::simulate(agent, mdp, simGamma, T, safeSim);
		}

		
		
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
