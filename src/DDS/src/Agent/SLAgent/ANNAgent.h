
#ifndef ANNAGENT_H
#define ANNAGENT_H

#include "SLAgent.h"
#include "../../MDP/MDP.h"
#include "../../dds.h"
#include "../../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	ANNAgent
	\author 	Castronovo Michael
	
	\brief 	A RL agent copying another agent using a Deep neural network.
	          It has been designed to address:
				- DISCRETE state space (int)
				- DISCRETE action space (int)
				- SINGLE reward (double)

	\date 	2015-12-21
*/
// ===========================================================================
/* final */ class dds::ANNAgent : public dds::SLAgent
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the ANNAgent to load.
					(can either be compressed or uncompressed)
		*/
		ANNAgent(std::istream& is);
		
		
		/**
		     \brief                   Constructor.

               \param[hiddenLayers_     The number of neurons for each
                                        hidden layer.
               
               \param[learningRate_     The learning rate.
               \param[decreasingLR_     If true, decreases the learning rate
                                        at each epoch such that :
                                             LR(epoch) = ini_LR/sqrt(epoch+1)
               
               \param[maxEpoch_         The maximal number of iterations.
               \param[epochRange_       The maximal number of iterations for
                                        which the VS error can increase.
               
               \param[agent             The agent to copy.
               \param[nbOfMDPs          The number of MDPs on which the agent
                                        to copy will be tested.
                                        Each trajectory (1 per MDP) will
                                        then be used to build SL samples.
               \param[simGamma          The discount factor to use when the
                                        agent to copy will be tested in order
                                        to generate SL samples.
               \param[T                 The horizon limit to use when the
                                        agent to copy will be tested in order
                                        to generate SL samples.
               \param[SLModelFileName   The name of the file where to save
                                        the SL model.
		*/
		ANNAgent(const std::vector<unsigned int>& hiddenLayers_,
		         double learningRate_,
		         bool decreasingLR_,
		         unsigned int maxEpoch_,
		         unsigned int epochRange_,
		         Agent* agent,
		         unsigned int nbOfMDPs, double simGamma, unsigned int T,
		         std::string SLModelFileName);
		
		
		/**
			\brief	Destructor.
		*/
		~ANNAgent() {}


		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "ANNAgent"; }


		// =================================================================
		//	Public methods
		// =================================================================		
		/**
			\brief	Return a clone of this Agent.
			
			\return	A clone of this Agent.
		*/
		Agent* clone() const
		{
			return cloneInstance<ANNAgent>(this);
		}
		
		
		/**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const { return ANNAgent::toString(); }


          /**
               \brief    Return the name of the class of this object, formatted
                         for export files.

               \return   The name of the class of this object, formatted for
                         export files.
		*/
		std::string getExportClassName() const { return "ANN agent"; }

		
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
		     \brief    The neural network.
		               (internal representation of the SL model)
		*/
          std::vector<struct fann*> annList;
          
          
          /**
               \brief    The number of neurons per layer.
          */
          std::vector<unsigned int> hiddenLayers;


          /**
               \brief    The learning rate.
          */
          double learningRate;
          
          
          /**
               \brief    If true, decreases the learning rate at each epoch
                         such that :
                                   LR(epoch) = ini_LR/sqrt(epoch+1)
          */
          bool decreasingLR;
          
          
          /**
               \brief    The maximal number of iterations.
          */
          unsigned int maxEpoch;
          
          
          /**
               \brief    The maximal number of iterations for which the VS
                         error can increase.
          */
          unsigned int epochRange;

          
          /**
               \brief    The mean value of each feature of the input vectors
                         coming from the learning set.
          */
          std::vector<std::vector<double> > meansList;


          /**
               \brief    The standard deviation of each feature of the input
                         vectors coming from the learning set.
          */
          std::vector<std::vector<double> > stDevsList;
               

          // =================================================================
		//	Private static methods
		// =================================================================
		/**
               \brief    Split the samples in 3 distinct sets : LS, VS & TS.
               
               \param[inputs            The inputs  of the SL samples.
               \param[outputs           The outputs of the SL samples.
               \param[weights           The weights of the SL samples.
               \param[packedIndexes     The list of the indexes of the MDPs
                                        from which each sample is from.
               
               \param[pLS               The proportion of LS samples.
               \param[pVS               The proportion of VS samples.
               \param[pTS               The proportion of TS samples.
               
               \param[ls                The list of the indexes of the
                                        samples in LS.
               \param[vs                The list of the indexes of the
                                        samples in VS.
               \param[ts                The list of the indexes of the
                                        samples in TS.
          */
          static void splitSamples(const std::vector<
                                        std::vector<double> >& inputs,
                                   const std::vector<
                                        std::vector<double> >& outputs,
                                   const std::vector<double>& weights,
                                   const std::vector<
                                        std::vector<unsigned int> >&
                                             packedIndexes,
                                   double pLS, double pVS, double pTS,
                                   std::vector<unsigned int>& ls,
                                   std::vector<unsigned int>& vs,
                                   std::vector<unsigned int>& ts);


          /**
               \brief                   Build a partition of 's' with
                                        'partitionSize' elements of 's'
                                        (no repetition).
                         
                                        If there is not enough elements
                                        in 's', it only takes each element
                                        once.
               
               \param[partitionSize     The number of elements to take.
               \param[s                 The set from which to create a
                                        partition.
               
               \return                  A partition of 's'.
          */
          static std::vector<unsigned int> buildPartition(
                         unsigned int partitionSize,
                         std::vector<unsigned int>& s);


          /**
               \brief              Load the samples into a 'fann_train_data'
                                   structure.
          
               \param[inputs       The vector in which to store inputs 
                                   of the SL samples.
               \param[outputs      The vector in which to store outputs 
                                   of the SL samples.
               \param[weights      The weights of the SL samples.
               \param[s            The list of the samples in 'inputs' &
                                   'outputs' which have to be tested.
               
               \param[data         The SL samples, stored in a
                                   'fann_train_data' structure.

               \param[dataWeights  The weights of the SL samples, stored in
                                   a C array. 
          */
          static void loadFANNData(
                    const std::vector<std::vector<double> >& inputs,
                    const std::vector<std::vector<double> >& outputs,
                    const std::vector<double>& weights,
                    const vector<unsigned int>& s,
                    fann_train_data*& data, double*& dataWeights);


          /**
               \brief         Predict the action to perform w.r.t. to the
                              given output by associating a score to each
                              action using the given neural network.
                              
                              The action which obtained the highest score
                              will be played. In case of a tie, the action
                              will be selected randoSLy among those in tie.

               \param[ann     A neural network.
               \param[input   A vector where each column corresponds to an
                              input feature.

               \return        A vector where each column corresponds to a
                              prediction an output feature.
          */
          static std::vector<double> predict(
                    const struct fann* ann,
                    const std::vector<double>& input);


          /**
               \brief              For the given neural network, compute the
                                   ER (error rate) on the subset of samples
                                   defined by 's'.

               \param[ann          A neural network.
               \param[inputs       The vector in which to store inputs 
                                   of the SL samples.
               \param[outputs      The vector in which to store outputs 
                                   of the SL samples.
               \param[weights      The weights of the SL samples.
               \param[s            The list of the samples in 'inputs' &
                                   'outputs' which have to be tested.

               \return             The ER on 's'.                         
          */
          static double computeER(const struct fann* ann,
                                  const std::vector<
                                             std::vector<double> >& inputs,
                                  const std::vector<
                                             std::vector<double> >& outputs,
                                  const std::vector<double>& weights,
                                  const std::vector<unsigned int>& s);


          /**
               \brief              For the given neural network, compute the
                                   MSE (mean squared error) on the subset
                                   of samples defined by 's'.

               \param[ann          A neural network.
               \param[inputs       The vector in which to store inputs 
                                   of the SL samples.
               \param[outputs      The vector in which to store outputs 
                                   of the SL samples.
               \param[weights      The weights of the SL samples.
               \param[s            The list of the samples in 'inputs' &
                                   'outputs' which have to be tested.

               \return             The MSE on 's'.                         
          */
          static double computeMSE(const struct fann* ann,
                                   const std::vector<
                                             std::vector<double> >& inputs,
                                   const std::vector<
                                             std::vector<double> >& outputs,
                                   const std::vector<double>& weights,
                                   const std::vector<unsigned int>& s);


          // =================================================================
		//	Private methods
		// =================================================================
          /**
               \brief              Learn a SL model by exploiting the given
                                   SL samples.

               \param[inputs       The inputs  of the SL samples.
               \param[outputs      The outputs of the SL samples.
               \param[weights      The weights of the SL samples.
               \param[indexes      The indexes of the MDPs from which each
                                   sample is from.
          */
          void train(std::vector<std::vector<double> >& inputs,
                     std::vector<std::vector<double> >& outputs,
                     std::vector<double>& weights,
                     std::vector<unsigned int>& indexes);


          /**
               \brief              Save the current SL model into a file.
               
               \param[fileName     The name of the file in which to save
                                   the current SL model.
          */
          void saveSLModel(std::string fileName) const;
          
          
          /**
               \brief              Load the current SL model from a file.
                                   Does nothing if the file does not exist.
               
               \param[fileName     The name of the file from which to load
                                   the current SL model.
          */
          void loadSLModel(std::string fileName);


          /**
               \brief              Compute the standardization parameters
                                   with respect the samples.
               
               \param[n            The index of the NN for which to compute
                                   the standardization parameters.
          
               \param[inputs       The vector in which to store inputs 
                                   of the SL samples.
               \param[s            The list of the samples in 'inputs'
                                   which have to be tested.
          */
          void computeStandardizationParameters(
                         unsigned int n,
                         const std::vector<std::vector<double> >& inputs,
                         const std::vector<unsigned int>& s);


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
          std::vector<double> predict(
                    const std::vector<double>& input) const;


          /**
               \brief         Standardize the input vector w.r.t. 'means'
                              and 'stDevs'.
               
               \param[n       The index of the ANN for which we want to
                              standardize 'input'.

               \param[input   The input vector to standardize.
               
               \return        The standardized input vector.
          */
          std::vector<double> standardizeInput(
                    unsigned int n,
                    const std::vector<double>& input) const;


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
