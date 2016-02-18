
#ifndef SLAGENT_H
#define SLAGENT_H

#include "../EAgent.h"
#include "../OptimalAgent.h"
#include "../FormulaAgent/FVariable.h"
#include "../FormulaAgent/QMean.h"
#include "../FormulaAgent/QSelf.h"
#include "../FormulaAgent/QUniform.h"
#include "../Agent.h"
#include "../../MDP/MDP.h"
#include "../../MDPDistribution/DirMultiDistribution.h"
#include "../../Experiment/Experiment.h"
#include "../../dds.h"
#include "../../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	SLAgent
	\author 	Castronovo Michael
	
	\brief 	A RL agent copying another agent using a supervised learning
	          algorithm.
	          It has been designed to address:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)

	\date 	2015-12-21
*/
// ===========================================================================
class dds::SLAgent : public dds::Agent
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
               \brief    Constructor.
          */
          SLAgent();

		
		/**
		     \brief                   Constructor.

               \param[name	          The name of this Agent.
               \param[agent_            The agent to copy.
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
               \param[slModelFileName_  The name of the file where to save
                                        the SL model.
		*/
		SLAgent(std::string name,
		        Agent* agent_,
		        unsigned int nbOfMDPs_, double simGamma_, unsigned int T_,
		        std::string slModelFileName_);
		
		
		/**
			\brief	Destructor.
		*/
		virtual ~SLAgent();


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "SLAgent"; }
		
		
		/**
		     \brief              Generate SL samples from the trajectories
		                         in the given experiment.

               \param[dirDistrib   The distribution on which the agent to copy
                                   has been trained on.
               \param[nbOfMDPs     The number of MDPs to draw
                                   (1 trajectory per MDP).
               \param[simGamma	The discount factor to use for the
							simulations.
			\param[T			The horizon limit.
               \param[inputs       The inputs  of the SL samples.
               \param[outputs      The outputs of the SL samples.
               \param[weights      The weights of the SL samples.
               \param[indexes      The indexes of the MDPs from which each
                                   sample is from.
		*/
		static void generateSLSamples(const DirMultiDistribution* dirDistrib,
		                              unsigned int nbOfMDPs,
		                              double simGamma, unsigned int T,
                                        std::vector<
                                             std::vector<double> >& inputs,
                                        std::vector<
                                             std::vector<double> >& outputs,
                                        std::vector<double>& weights,
                                        std::vector<unsigned int>& indexes);
          
          /**
		     \brief              Generate SL samples from the trajectories
		                         in the given experiment.
               
               \param[agent        If specified, uses 'agent' to associate an
                                   action to each sample (the same as those
                                   that would have been taken by 'agent').
                                   
                                   If not specified, associate the optimal
                                   action.

               \param[dirDistrib   The distribution on which the agent to copy
                                   has been trained on.
               \param[nbOfMDPs     The number of MDPs to draw
                                   (1 trajectory per MDP).
               \param[simGamma	The discount factor to use for the
							simulations.
			\param[T			The horizon limit.
               \param[inputs       The inputs  of the SL samples.
               \param[outputs      The outputs of the SL samples.
               \param[weights      The weights of the SL samples.
               \param[indexes      The indexes of the MDPs from which each
                                   sample is from.
		*/
          static void generateSLSamples(Agent* agent,
		                              const DirMultiDistribution* dirDistrib,
		                              unsigned int nbOfMDPs,
		                              double simGamma, unsigned int T,
                                        std::vector<
                                             std::vector<double> >& inputs,
                                        std::vector<
                                             std::vector<double> >& outputs,
                                        std::vector<double>& weights,
                                        std::vector<unsigned int>& indexes);


          /**
               \brief              Save the inputs/outputs of the given
                                   SL samples into a file.
               
               \param[inputs       The inputs  of the SL samples.
               \param[outputs      The outputs of the SL samples. 
               \param[weights      The weights of the SL samples.
               \param[indexes      The indexes of the MDPs from which each
                                   sample is from.
               \param[os           The stream in which to save the SL samples.
          */
          static void saveSLSamples(const std::vector<
                                        std::vector<double> >& inputs,
                                    const std::vector<
                                        std::vector<double> >& outputs,
                                    const std::vector<double>& weights,
                                    const std::vector<unsigned int>& indexes,
                                    std::ostream& os);


          /**
               \brief              Load the inputs/outputs of the given
                                   SL samples from a file.

               \param[is           The stream from which to load the SL
                                   samples.
               \param[inputs       The vector in which to store inputs 
                                   of the SL samples.
               \param[outputs      The vector in which to store outputs 
                                   of the SL samples.
               \param[weights      The vector in which to store the weights
                                   of the SL samples.
               \param[indexes      The vector in which to store the indexes
                                   of the MDPs from which each sample is from.
          */
          static void loadSLSamples(std::istream& is,
                                    std::vector<
                                        std::vector<double> >& inputs,
                                    std::vector<
                                        std::vector<double> >& outputs,
                                    std::vector<double>& weights,
                                    std::vector<unsigned int>& indexes);


		// =================================================================
		//	Public methods
		// =================================================================	
		/**
			\brief	Return the action to perform on the currently
					associated MDP from state 'x'.
					Consists to draw a Optimal action from the action
					space of the currently associated MDP.
			
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
		virtual void reset() throw (AgentException);

          
          /**
               \brief    Free unnecessary data.
                         (called when this Agent has finished to interact
                         with the current MDP).
		*/
		virtual void freeData();


		/**
			\brief	Return a clone of this Agent.
			
			\return	A clone of this Agent.
		*/
		virtual Agent* clone() const = 0;


		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		virtual std::string getClassName() const { return SLAgent::toString(); }


          /**
               \brief    Return the name of the class of this object, formatted
                         for export files.

               \return   The name of the class of this object, formatted for
                         export files.
		*/
		virtual std::string getExportClassName() const { return "SL agent"; }


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


          /**
               \brief    Save the current SL model into a file.
                         Does nothing if there is no model.
          */
          void saveSLModel() const;
          
          
          /**
               \brief    Load the current SL model from a file.
                         Does nothing if the file does not exist.
          */
          void loadSLModel();


     protected:
          // =================================================================
		//	Protected static methods
		// =================================================================
		/**
		     \brief              Insert the samples of 'set 2' right after
		                         the samples of 'set 1'.
		     
		     \param[inputs1      The inputs  of the SL samples (set 1).
               \param[outputs1     The outputs of the SL samples (set 1).
               \param[weights1     The weights of the SL samples (set 1).
               \param[indexes1     The indexes of the MDPs from which each
                                   sample is from (set 1).
               
               \param[inputs2      The inputs  of the SL samples (set 2).
               \param[outputs2     The outputs of the SL samples (set 2).
               \param[weights2     The weights of the SL samples (set 2).
               \param[indexes2     The indexes of the MDPs from which each
                                   sample is from (set 2).
		*/
		static void insertSamples(std::vector<
                                        std::vector<double> >& inputs1,
                                    std::vector<
                                        std::vector<double> >& outputs1,
                                    std::vector<double>& weights1,
                                    std::vector<unsigned int>& indexes1,
                                    const std::vector<
                                        std::vector<double> >& inputs2,
                                    const std::vector<
                                        std::vector<double> >& outputs2,
                                    const std::vector<double>& weights2,
                                    const std::vector<unsigned int>& indexes2);
		
		
		/**
		     \brief              Merge the samples which have the same
		                         input. As a result, several indexes can
		                         be associated to the same sample, which
		                         are stored in 'packedIndexes'.
		     
		     
		     \param[inputs       The vector in which to store inputs 
                                   of the SL samples.
               \param[outputs      The vector in which to store outputs 
                                   of the SL samples.
               \param[weights      The vector in which to store the weights
                                   of the SL samples.
               \param[indexes      The vector in which to store the indexes
                                   of the MDPs from which each sample is from.
               \param[packedIndexes     The list of the indexes of the MDPs
                                        from which each sample is from.
		*/
		static void packSamples(
                    std::vector<
                         std::vector<double> >& inputs,
                    std::vector<
                         std::vector<double> >& outputs,
                    std::vector<double>& weights,
                    const std::vector<unsigned int>& indexes,
                    std::vector<
                         std::vector<unsigned int> >& packedIndexes);


	private:
		// =================================================================
		//	Private attributes
		// =================================================================	
		/**
               \brief    Get the current time step.
          */
          unsigned int t;


		/**
		     \brief    The list of 'FVariable's used to build the input
		               (which is used to predict the action to perform).
		*/
          std::vector<FVariable*> varList;


          /**
               \brief    The agent to copy.
          */
          Agent* agent;
          
          
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
          std::string slModelFileName;


          // =================================================================
		//	Private static methods
		// =================================================================
		/**
		     \brief         Return the linear combination of vectors
		                    'a' & 'b'.
		     
		     \param[a       A vector.
		     \param[b       A vector.
		     \param[aWeight The weight of vector 'a'.
		     \param[bWeight The weight of vector 'b'.
		     
		     \return        The linear combination of vectors 'a' & 'b'.
		*/
		static std::vector<double> combine(
		          const std::vector<double>& a,
		          const std::vector<double>& b,
		          double aWeight = 0.5,
		          double bWeight = 0.5);
		
		
		/**
		     \brief              Build the list of 'FVariable's w.r.t. to
		                         the given MDP distribution.

               \param[mdpDistrib    A MDP distribution.
		*/
		static std::vector<FVariable*> buildVarList(
		          const MDPDistribution* mdpDistrib);


          /**
               \brief         Build the input vector (for SL predictions)
                              w.r.t. the list of 'FVariable's, the number
                              of actions, and a state.
               
               \param[t       The current time-step.
               \param[varList The list of 'FVariable's.
               \param[nU      The number of actions.
               \param[x       A state.

               \return        The input vector (for SL predictions).
          */
		static std::vector<double> buildInput(
		          unsigned int t,
		          const std::vector<FVariable*>& varList,
		          unsigned int nU,
		          unsigned int x);
		
		
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

		
		/**
		     \brief    Build the input vector (for SL predictions) w.r.t.
		               the given state.

		     \param[x  A state.
		     
		     \return   The input vector (for SL predictions).
		*/
		std::vector<double> buildInput(unsigned int x) const;

          
          /**
               \brief              Learn a SL model by exploiting the given
                                   SL samples.

               \param[inputs       The inputs  of the SL samples.
               \param[outputs      The outputs of the SL samples.   
               \param[weights      The weights of the SL samples.
               \param[indexes      The indexes of the MDPs from which each
                                   sample is from.
          */
          virtual void train(std::vector<std::vector<double> >& inputs,
                             std::vector<std::vector<double> >& outputs,
                             std::vector<double>& weights,
                             std::vector<unsigned int>& indexes) = 0;


          /**
               \brief              Save the current SL model into a file.
                                   Does nothing if there is no model.
               
               \param[fileName     The name of the file in which to save
                                   the current SL model.
          */
          virtual void saveSLModel(std::string fileName) const = 0;
          
          
          /**
               \brief              Load the current SL model from a file.
                                   Does nothing if the file does not exist.
               
               \param[fileName     The name of the file from which to load
                                   the current SL model.
          */
          virtual void loadSLModel(std::string fileName) = 0;
          
          
          /**
               \brief         Predict the action to perform w.r.t. to the
                              given output by associating a score to each
                              action.
                              
                              The action which obtained the highest score
                              will be played. In case of a tie, the action
                              will be selected randoSLy among those in tie.

               \param[input   A vector where each column corresponds to an
                              input feature.

               \return        A vector where each column corresponds to a
                              prediction an output feature.
          */
          virtual std::vector<double> predict(
                    const std::vector<double>& input) const = 0;

		
		#ifndef NDEBUG
		/**
			\brief	Function to call whenever the internal data could
					be corrupted by wrong data entries
					(calls 'assert()' for each data).
		*/
		virtual void checkIntegrity() const;
		#endif
};

#endif
