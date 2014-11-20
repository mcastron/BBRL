
#ifndef BAMCPAGENT_H
#define BAMCPAGENT_H

#include "Agent.h"
#include "Guez/MDPSimulator.h"
#include "Guez/PCSamplerFactory.h"
#include "Guez/planners/mcp/bamcp/bamcp.h"
#include "Guez/samplers/FDMsamplerFactory.h"
#include "../MDPDistribution/DirMultiDistribution.h"
#include "../dds.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/**
	\class 	BAMCPAgent
	\author 	Castronovo Michael
	
	\brief 	Interface of a BAMCP Agent to address MDP problems for:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)

               (based on Guez work)

	\date 	2014-10-11
*/
// ===========================================================================
/* final */ class dds::BAMCPAgent : public dds::Agent
{		
	public:
		// =================================================================
		//	Public Constructors/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the BAMCPAgent to load.
					(can either be compressed or uncompressed)
		*/
		BAMCPAgent(std::istream& is);


		/**
			\brief           Constructor.
			
			\param[K_        The number of nodes to insert to the BAMCP
			                 tree at each time-step.
		*/
		BAMCPAgent(unsigned int K_);
		
		
		/**
			\brief	Destructor.
		*/
		~BAMCPAgent();


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString()
		{
		   return "BAMCPAgent";
		}


		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Return the action to perform on the currently
					associated MDP from state 'x'.
			
			\param[xt	The state from which a decision has to be made.
			
			\return	The action to perform.
		*/
		int getAction(int xt) const throw (MDPException);
		
		
		/**
			\brief	Learn from <x, u, y, r> transition (online).
			
			\param[x	The origin state.
			\param[u	The action performed.
			\param[y	The state reached.
			\param[r	The reward observed.
		*/
		void learnOnline(int x, int u, int y, double r) throw (MDPException);

		
		/**
			\brief	Reset this agent.
					(called when this Agent is associated to a new MDP).
		*/
		void reset() throw (MDPException);


		/**
			\brief	Return a clone of this Agent.
			
			\return	A clone of this Agent.
		*/
		Agent* clone() const
		{
			return cloneInstance<BAMCPAgent>(this);
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const { return BAMCPAgent::toString(); }

		
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
		   \brief    The number of nodes to insert to the BAMCP tree
		             at each time-step.
		*/
		unsigned int K;
		
		
		/**
		   \brief    The BAMCP algorithm (implemented by Guez).
		*/
		BAMCP* bamcp;
		
		
		/**
		   \brief    A Simulator (required by 'BAMCP').
		*/
		SIMULATOR* simulator;


		/**
		   \brief    The current SamplerFactory (required by 'BAMCP').
		*/
		SamplerFactory* samplerFact;
		
		
		/**
		   \brief    The number of states of the MDPs to be played.
		*/
		unsigned int nX;
		
		
		/**
		   \brief    The number of actions of the MDPs to be played.
		*/
		unsigned int nU;
		
		
		/**
		   \brief    The reward function of the MDPs to be played.
		*/
		std::vector<double> R;
		
		
		/**
		   \brief    The list of prior observations for each transition.
		*/
		vector<double> priorcountList;
		
		
		// =================================================================
		//	Private methods
		// =================================================================
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
