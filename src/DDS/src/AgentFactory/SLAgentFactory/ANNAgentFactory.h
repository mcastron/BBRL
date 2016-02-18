
#ifndef ANNAGENTFACTORY_H
#define ANNAGENTFACTORY_H

#include "SLAgentFactory.h"

#include "../../Agent/SLAgent/ANNAgent.h"
#include "../../MDPDistribution/DirMultiDistribution.h"

#include "../../dds.h"
#include "../../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	ANNAgentFactory
	\author 	Castronovo Michael
	
	\brief 	An AgentFactory which generates ANNAgent's.
	               (i)  The ANNAgentFactory use another AgentFactory for
                         generating the Agent corresponding to the given
                         parameters.
                    (ii) The generated Agent is copied by a ANNAgent, and
                         the ANNAgent is returned.
               	
	          'init()' must be called after this AgentFactory has been
	          deserialized.

	\date 	2015-12-18
*/
// ===========================================================================
/* final */ class dds::ANNAgentFactory : public dds::SLAgentFactory
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the ANNAgentFactory to load.
					(can either be compressed or uncompressed)
		*/
		ANNAgentFactory(std::istream& is);
		
		
		/**
			\brief		          Constructor.

               //   TODO

		     \param[agentFactory_     The AgentFactory whose agent will be
		                              copied.
               \param[nbOfMDPs_         The number of MDPs on which the agent
                                        to copy will be tested.
                                        Each trajectory (1 per MDP) will
                                        then be used to build SL samples.
               \param[simGamma_         The discount factor to use when the
                                        agent to copy will be tested in order
                                        to generate SL samples.
               \param[T_                The horizon limit to use when the
                                        agent to copy will be tested in order
                                        to generate SL samples.
               \param[SLModelFileName_  The name of the file where to save
                                        the SL model.
		*/
		ANNAgentFactory(const std::vector<unsigned int>& hiddenLayers_,
		                double learningRate_,
		                bool decreasingLearningRate_,
		                unsigned int maxEpoch_,
		                unsigned int epochRange_,
		                AgentFactory* agentFactory_,
		                unsigned int nbOfMDPs_,
		                double simGamma_,
		                unsigned int T_,
		                std::string SLModelFileName_);


		/**
               \brief    Destructor.
		*/
		~ANNAgentFactory();
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "ANNAgentFactory"; }

		
		// =================================================================
		//	Public methods
		// =================================================================
          /**
               \brief    Return the name of this AgentFactory.
               
               \return   The name of this AgentFactory.
		*/
		std::string getName() const
		{
		     return SLAgentFactory::getName() + " (DNN)";
          }
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const
		{
		   return ANNAgentFactory::toString();
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
          //   Private attributes
          // =================================================================
           /* TODO */
          vector<unsigned int> hiddenLayers;
          double learningRate;
          bool decreasingLearningRate;
          unsigned int maxEpoch;
          unsigned int epochRange;

     
          // =================================================================
          //   Private methods
          // =================================================================
          /**
               \brief                   Return a RL agent copying another
                                        agent using a SL algorithm.

               \param[agent_            The agent to copy.
               \param[nbOfMDPs_         The number of MDPs on which the agent
                                        to copy will be tested.
                                        Each trajectory (1 per MDP) will
                                        then be used to build SL samples.
               \param[simGamma_         The discount factor to use when the
                                        agent to copy will be tested in order
                                        to generate SL samples.
               \param[T                 The horizon limit to use when the
                                        agent to copy will be tested in order
                                        to generate SL samples.
               \param[SLModelFileName_  The name of the file where to save
                                        the SL model.
          */
          ANNAgent* getSLAgent(Agent* agent,
                               unsigned int nbOfMDPs,
                               double simGamma,
                               unsigned int T,
                               std::string SLModelFileName) const;
};

#endif
