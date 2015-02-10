
#ifndef VDBEEGREEDYAGENT_H
#define VDBEEGREEDYAGENT_H

#include "Agent.h"
#include "../MDP/MDP.h"
#include "../MDP/CModel.h"
#include "../MDPDistribution/MDPDistribution.h"
#include "../MDPDistribution/DirMultiDistribution.h"
#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	VDBE-EGreedyAgent
	\author 	Castronovo Michael
	
	\brief 	A RL Agent following an vdbe e-greedy policy.
	          It has been designed to address:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)
			
			For Offline learning, does not support another distribution
			than 'DirMultiDistribution'.

	\date 	2014-12-13
*/
// ===========================================================================
/* final */ class dds::VDBEEGreedyAgent : public dds::Agent
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the VDBE-EGreedyAgent to load.
					(can either be compressed or uncompressed)
		*/
		VDBEEGreedyAgent(std::istream& is);
		
		
		/**
			\brief			Constructor.
			
			\param[sigma_		The inverse sensitivity.
			\param[delta_		Determine the influence of the selected
							action on the state-dependent explortation
							probability.
							(in [0; 1[).
			\param[iniEpsilon_	The initial probability to select an action
							randomly from each state.
			\param[iniModel_	The initial model on which the Agent based
							its 'best' decisions
							(updated during the interaction, learned
							offline if not provided).
		*/
		VDBEEGreedyAgent(double sigma_, double delta_, double iniEpsilon_,
				CModel* iniModel_ = 0);
		
		
		/**
			\brief	Destructor.
			          'iniModel' is freed.
		*/
		~VDBEEGreedyAgent();


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "VDBEEGreedyAgent"; }


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the action to perform on the currently
					associated MDP from state 'x'.
			
			\param[xt	The state from which a decision has to be made.
			
			\return	The action to perform.
		*/
		int getAction(int xt) const throw (AgentException);
		
		
		/**
			\brief	Learn from <x, u, y, r> transition (online).
			
			\param[x	The origin state.
			\param[u	The action performed.
			\param[y	The state reached.
			\param[r	The reward observed.
		*/
		void learnOnline(int x, int u, int y, double r)
                                                       throw (AgentException);

		
		/**
			\brief	Reset this agent.
					(called when this Agent is associated to a new MDP).
		*/
		void reset() throw (AgentException);


          /**
               \brief    Free unnecessary data.
                         (called when this Agent has finished to interact
                         with the current MDP).
		*/
		void freeData();


		/**
			\brief	Return a clone of this Agent.
			
			\return	A clone of this Agent.
		*/
		Agent* clone() const
		{
			return cloneInstance<VDBEEGreedyAgent>(this);
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const
		{
			return VDBEEGreedyAgent::toString();
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


	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The inverse sensitivity.
		*/
		double sigma;
		
		
		/**
			\brief	Determine the influence of the selected action on the
					state-dependent explortation probability.
					(in [0; 1[).
		*/
		double delta;
		
		
		/**
			\brief	The initial probability to select an action randomly
					from each state.
		*/
		double iniEpsilon;
		
		
		/**
			\brief	The current value of 'epsilon' for each state.
		*/
		std::vector<double> epsilonList;


		/**
			\brief	The result of the last call of 'qIteration()' on
					'cModel'.
		*/
		vector<double> Q;
		
		
		/**
			\brief	The initial model.
		*/
		CModel* iniModel;
		
		
		/**
			\brief	The model defining the 'best' action perform for
					the currently associated MDP.
		*/
		CModel* cModel;
		
		
		/**
			\brief	The number of states of the currently associated
					MDP.
		*/
		unsigned int nX;
		
		
		/**
			\brief	The number of actions of the currently associated
					MDP.
		*/
		unsigned int nU;
		
		
		// =================================================================
		//	Private methods
		// =================================================================
		/**
			\brief	Used to update the epsilon's with respect to the
					value differences between the current and previous
					Q-functions.
		*/
		double f(	double x, double u, double sigma,
				std::vector<double>& pQ, std::vector<double>& Q)
		{
			double eDiff = exp(-fabs(Q[nU*x + u] - pQ[nU*x + u]) / sigma);
			return ((1.0 - eDiff) / (1.0 + eDiff));
		}


		/**
			\brief			Learn from a MDP distribution offline.

							Consist to learn an appropriate initial
							model.
			
			\param[mdpDistrib	A MDP distribution.
		*/
		void learnOffline_aux(const MDPDistribution* mdpDistrib)
											throw (AgentException);


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
