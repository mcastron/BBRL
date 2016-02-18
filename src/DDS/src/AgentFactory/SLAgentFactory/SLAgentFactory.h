
#ifndef SLAGENTFACTORY_H
#define SLAGENTFACTORY_H

#include "../AgentFactory.h"

#include "../../Agent/SLAgent/SLAgent.h"
#include "../../MDPDistribution/DirMultiDistribution.h"

#include "../../dds.h"
#include "../../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	SLAgentFactory
	\author 	Castronovo Michael
	
	\brief 	An AgentFactory which generates SLAgent's.
	               (i)  The SLAgentFactory use another AgentFactory for
                         generating the Agent corresponding to the given
                         parameters.
                    (ii) The generated Agent is copied by a SLAgent, and
                         the SLAgent is returned.
               	
	          'init()' must be called after this AgentFactory has been
	          deserialized.

	\date 	2015-11-30
*/
// ===========================================================================
class dds::SLAgentFactory : public dds::AgentFactory
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
		     \brief    Constructor.
          */
          SLAgentFactory();
		
		
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the SLAgentFactory to load.
					(can either be compressed or uncompressed)
		*/
		SLAgentFactory(std::istream& is);
		
		
		/**
			\brief		          Constructor.

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
		SLAgentFactory(AgentFactory* agentFactory_,
		               unsigned int nbOfMDPs_,
		               double simGamma_,
		               unsigned int T_,
		               std::string SLModelFileName_);
		
		
		/**
               \brief    Destructor.
		*/
		~SLAgentFactory();
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "SLAgentFactory"; }

		
		// =================================================================
		//	Public methods
		// =================================================================
          /**
               \brief    Return the name of this AgentFactory.
               
               \return   The name of this AgentFactory.
		*/
		virtual std::string getName() const
		{
		     return agentFactory->getName() + " (through SL)";
          }


		/**
			\brief			Initialize this AgentFactory with respect
							to the given MDP distribution.
							
							Must be called after this AgentFactory has
							been deserialized.
			
			\param[mdpDistrib	A MDP distribution.
		*/
		void init(const MDPDistribution* mdpDistrib)
									throw (AgentFactoryException);


		/**
			\brief			Return an agent parametrized by 'paraSList'.
			
			\param[paraSList	The parameters to use.
			
			\return			An agent parametrized by 'paraSList'.
		*/
		Agent* get(const std::vector<double>& paraSList) const
									throw (AgentFactoryException);

		
		/**
			\brief			Return the list of bounds on each parameter.
			
			\return			The list of bounds on each parameter.
		*/
		std::vector<std::pair<double, double> > getBounds() const
									throw (AgentFactoryException)
		{
			return agentFactory->getBounds();
		}


		/**
			\brief			Return the list of split accuracies of each
							parameter.
			
			\return			The list of split accuracies of each
							parameter.
		*/
		std::vector<double> getSplitAcc() const throw (AgentFactoryException)
		{
			return agentFactory->getSplitAcc();
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const
		{
		   return SLAgentFactory::toString();
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
          //   Protected methods
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
          virtual SLAgent* getSLAgent(Agent* agent,
                                      unsigned int nbOfMDPs,
                                      double simGamma,
                                      unsigned int T,
                                      std::string SLModelFileName) const = 0;


	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
		     \brief    The AgentFactory whose agent will be copied.
		*/
		AgentFactory* agentFactory;
		
		
          /**
               \brief    The number of MDPs on which the agent to copy will
                         be tested. Each trajectory (1 per MDP) will then
                         be used to build SL samples.
          */
          unsigned int nbOfMDPs;
          
          
          /**
               \brief    The discount factor to use when the agent to copy
                         will be tested in order to generate SL samples.
          */
          double simGamma;
          
          
          /**
               \brief    The horizon limit to use when the agent to copy
                         will be tested in order to generate SL samples.
          */
          unsigned int T;
          

          /**
               \brief    The name of the file in which the current SL model
                         is stored.
          */
          std::string SLModelFileName;
          
          
          /**
               \brief    The MDP distribution used for initializing this
                         FormulaAgentFactory.
		*/
		const MDPDistribution* mdpDistrib;
};

#endif
