
#include "ANNAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
ANNAgent::ANNAgent(std::istream& is)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


ANNAgent::ANNAgent(const vector<unsigned int>& hiddenLayers_,
                   double learningRate_,
		         bool decreasingLR_,
		         unsigned int maxEpoch_,
		         unsigned int epochRange_,
                   Agent* agent,
		         unsigned int nbOfMDPs, double simGamma, unsigned int T,
		         string SLModelFileName) :
		               SLAgent("",
		                       agent,
		                       nbOfMDPs, simGamma, T, SLModelFileName),
		               hiddenLayers(hiddenLayers_),
		               learningRate(learningRate_),
		               decreasingLR(decreasingLR_),
		               maxEpoch(maxEpoch_), epochRange(epochRange_)
{
     stringstream sstr;
     sstr << "ANN (";
     
     if (!hiddenLayers.empty())
     {
          sstr << hiddenLayers[0];
          for (unsigned int i = 1; i < hiddenLayers.size(); ++i)
               sstr << "-" << hiddenLayers[i];
     }
     else { sstr << "0"; }
     
     sstr << ", " << learningRate;
     sstr << ", " << epochRange;
     sstr << ", " << nbOfMDPs;
     sstr << ")";

     setName(sstr.str());


     #ifndef NDEBUG
     checkIntegrity();
     #endif
}


// ===========================================================================
//	Public methods
// ===========================================================================
void ANNAgent::serialize(ostream& os) const
{
	SLAgent::serialize(os);
	
	
	os << ANNAgent::toString() << "\n";
	os << 7 << "\n";
	
	
	//   'hiddenLayers'
	os << hiddenLayers.size() << "\n";
	for (unsigned int i = 0; i < hiddenLayers.size(); ++i)
	     os << hiddenLayers[i] << "\n";
	

     //   'learningRate'
     os << learningRate << "\n";


     //   'decreasingLearningRate'
     os << (decreasingLR ? 1 : 0) << "\n";
     
     
     //   'maxEpoch'
     os << maxEpoch << "\n";


     //   'epochRange'
     os << epochRange << "\n";


     //   'meansList' and 'stDevsList'
     os << meansList.size() << "\n";
     for (unsigned int i = 0; i < meansList.size(); ++i)
     {
          //   'means'
          const std::vector<double>& means = meansList[i];
          os << means.size() << "\n";
          for (unsigned int j = 0; j < means.size(); ++j)
               os << means[j] << "\n";


          //   'stDevs'
          const std::vector<double>& stDevs = stDevsList[i];
          os << stDevs.size() << "\n";
          for (unsigned int j = 0; j < stDevs.size(); ++j)
               os << stDevs[j] << "\n";
     }
}


void ANNAgent::deserialize(istream& is) throw (SerializableException)
{
	SLAgent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != ANNAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;


     //   'hiddenLayers'
     hiddenLayers.clear();
     if (!getline(is, tmp)) { throwEOFMsg("hiddenLayers"); }
     unsigned int hiddenLayersSize = atoi(tmp.c_str());
     for (unsigned int i = 0; i < hiddenLayersSize; ++i)
     {
          if (!getline(is, tmp)) { throwEOFMsg("hiddenLayers"); }
          hiddenLayers.push_back(atoi(tmp.c_str()));
     }
     ++i;


     //   'learningRate'
     if (!getline(is, tmp)) { throwEOFMsg("learningRate"); }
     learningRate = atof(tmp.c_str());
     ++i;
     
     
     //   'decreasingLR'
     if (!getline(is, tmp)) { throwEOFMsg("decreasingLR"); }
     decreasingLR = (atoi(tmp.c_str()) != 0);
     ++i;
     
     
     //   'maxEpoch'
     if (!getline(is, tmp)) { throwEOFMsg("maxEpoch"); }
     maxEpoch = atoi(tmp.c_str());
     ++i;
     
     
     //   'epochRange'
     if (!getline(is, tmp)) { throwEOFMsg("epochRange"); }
     epochRange = atoi(tmp.c_str());
     ++i;
     

     //   'meansList' and 'stDevsList'
     meansList.clear();
     stDevsList.clear();
     
     if (!getline(is, tmp)) { throwEOFMsg("meansList & stDevsList"); }
     unsigned int meansListSize = atoi(tmp.c_str());
     for (unsigned int i = 0; i < meansListSize; ++i)
     {
          //   'means'
          meansList.push_back(vector<double>());
          std::vector<double>& means = meansList.back();

          if (!getline(is, tmp)) { throwEOFMsg("means"); }
          unsigned int meansSize = atoi(tmp.c_str());
          for (unsigned int j = 0; j < meansSize; ++j)
          {
               if (!getline(is, tmp)) { throwEOFMsg("means"); }
               means.push_back(atof(tmp.c_str()));
          }

          //   'stDevs'
          stDevsList.push_back(vector<double>());
          std::vector<double>& stDevs = stDevsList.back();

          if (!getline(is, tmp)) { throwEOFMsg("stDevs"); }
          unsigned int stDevsSize = atoi(tmp.c_str());
          for (unsigned int j = 0; j < stDevsSize; ++j)
          {
               if (!getline(is, tmp)) { throwEOFMsg("stDevs"); }
               stDevs.push_back(atof(tmp.c_str()));
          }
     }
     i += 2;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
	
	
	#ifndef NDEBUG
     checkIntegrity();
     #endif
}


// ===========================================================================
//	Private static methods
// ===========================================================================
void ANNAgent::splitSamples(const vector<vector<double> >& inputs,
                            const vector<vector<double> >& outputs,
                            const vector<double>& weights,
                            const vector<
                                   vector<unsigned int> >& packedIndexes,
                            double pLS, double pVS, double pTS,
                            vector<unsigned int>& ls,
                            vector<unsigned int>& vs,
                            vector<unsigned int>& ts)
{
     ls.clear();
     vs.clear();
     ts.clear();

     
     //   Create a bag containing all the indexes
     set<unsigned int> bagS;
     for (unsigned int i = 0; i < packedIndexes.size(); ++i)
     {
          for (unsigned int j = 0; j < packedIndexes[i].size(); ++j)
               bagS.insert(packedIndexes[i][j]);
     }
     vector<unsigned int> bag(bagS.begin(), bagS.end());
     bagS.clear();


     //   Shuffle the bag
     std::random_shuffle(bag.begin(), bag.end());


     //   Distribute the samples among 'ls', 'vs' and 'ts'
     //   (do not split the samples with the same index)
     double sumP = (pLS + pVS + pTS);
     pLS /= sumP, pVS /= sumP, pTS /= sumP;
     for (unsigned int i = 0; i < bag.size(); ++i)
     {
          //   Determine where to put the current trajectory
          vector<unsigned int>* s;
          if      (ls.size() < pLS*inputs.size()) { s = &ls; }
          else if (vs.size() < pVS*inputs.size()) { s = &vs; }
          else                                    { s = &ts; }

          //   Put the selected trajectory in the right set
          for (unsigned int j = 0; j < inputs.size(); ++j)
          {
               //   Search for 'bag[j]' in 'packedIndexes[k]'
               unsigned int k;
               for (k = 0; k < packedIndexes[j].size(); ++k)
                    if (packedIndexes[j][k] == bag[i]) { break; }
           
               //   Found case
               if (k < packedIndexes[j].size()) { s->push_back(j); }
          }
     }
}


vector<unsigned int> ANNAgent::buildPartition(unsigned int partitionSize,
                                              vector<unsigned int>& s)
{
     //   Create a bag containing all the elements IDs
     set<unsigned int> bagS;
     for (unsigned int i = 0; i < s.size(); ++i) { bagS.insert(s[i]); }
     vector<unsigned int> bag(bagS.begin(), bagS.end());
     bagS.clear();


     //   Shuffle the bag
     std::random_shuffle(bag.begin(), bag.end());


     //   Build the partition
     partitionSize = (partitionSize < s.size()) ? partitionSize : s.size();
     vector<unsigned int> partition;
     for (unsigned int i = 0; i < partitionSize; ++i)
          partition.push_back(bag[i]);
     
     return partition;
}


void ANNAgent::loadFANNData(const vector<vector<double> >& inputs,
                            const vector<vector<double> >& outputs,
                            const vector<double>& weights,
                            const vector<unsigned int>& s,
                            fann_train_data*& data, double*& dataWeights)
{
     if (data)        { fann_destroy_train(data); }
     if (dataWeights) { delete[] dataWeights;     }


     //   Gather the LS samples
     vector<vector<double > > sInputs, sOutputs;
     vector<double> sWeights;
     for (unsigned int i = 0; i < s.size(); ++i)
     {
          sInputs.push_back(inputs[s[i]]);
          sOutputs.push_back(outputs[s[i]]);
          sWeights.push_back(weights[s[i]]);
     }


     //   Save the LS samples into a temporary file
     char tmpFileName[] = "data/agents/tmp.XXXXXX";
     close(mkstemp(tmpFileName));

     ofstream os(tmpFileName);
     saveSLSamples(sInputs, sOutputs, sWeights,
                   vector<unsigned int>(), os);
     os.close();


     //   Load the LS samples from the temporary file
     data = fann_read_train_from_file(tmpFileName);
     unlink(tmpFileName);


     //   Load the weights into a C array
     dataWeights = new double[sWeights.size()];
     for (unsigned int i = 0; i < sWeights.size(); ++i)
          dataWeights[i] = sWeights[i];
}


double ANNAgent::computeER(const struct fann* ann,
                           const vector<vector<double> >& inputs,
                           const vector<vector<double> >& outputs,
                           const vector<double>& weights,
                           const vector<unsigned int>& s)
{
     double sumWeights = 0.0;
     double sER = 0.0;
     for (unsigned int i = 0; i < s.size(); ++i)
     {
          //   Retrieve the input and the output
          const vector<double>& input  = inputs[ s[i]];
          const vector<double>& output = outputs[s[i]];


          //   Build the predicted output
          const vector<double>& pOutput = predict(ann, input);
          

          //   Update 'sASE'
               //   Retrieve the list of 'optimal actions' according to
               //   'output'
          vector<vector<double>::const_iterator> outputMaxList;
          outputMaxList = utils::search::max<vector<double> >(
                    output.begin(), output.end());
          
               //   Retrieve the list of 'optimal actions' according to
               //   'pOutput'
          vector<vector<double>::const_iterator> pOutputMaxList;
          pOutputMaxList = utils::search::max<vector<double> >(
                    pOutput.begin(), pOutput.end());

               //   Update 'sASE'
          double er = 1.0;
          for (unsigned int j = 0; j < pOutputMaxList.size(); ++j)
          {
               unsigned int pOutU = pOutputMaxList[j] - pOutput.begin();
               for (unsigned int k = 0; k < outputMaxList.size(); ++k)
               {
                    unsigned int outU = outputMaxList[j] - output.begin();
                    if (pOutU == outU)
                    {
                         er -= (1.0 / pOutputMaxList.size());
                         break;
                    }
               }
          }

          sER += (weights[s[i]] * er);
          sumWeights += weights[s[i]];
     }
     sER /= sumWeights;


     //   Return
     return sER;
}


double ANNAgent::computeMSE(const struct fann* ann,
                            const vector<vector<double> >& inputs,
                            const vector<vector<double> >& outputs,
                            const vector<double>& weights,
                            const vector<unsigned int>& s)
{
     double sumWeights = 0.0;
     double sMSE = 0.0;
     for (unsigned int i = 0; i < s.size(); ++i)
     {
          //   Retrieve the input and the output
          const vector<double>& input  = inputs[ s[i]];
          const vector<double>& output = outputs[s[i]];


          //   Build the predicted output
          const vector<double>& pOutput = predict(ann, input);
               
                         
          //   Update 'sMSE'
          double mse = 0.0;
          for (unsigned int j = 0; j < output.size(); ++j)
          {
               double diff = (pOutput[j] - output[j]);
               mse += ((diff * diff) / output.size());
          }

          sMSE += (weights[s[i]] * mse);
          sumWeights += weights[s[i]];
     }
     sMSE /= sumWeights;
     
     
     //   Return
     return sMSE;
}


vector<double> ANNAgent::predict(
          const struct fann* ann, const vector<double>& input)
{
     //   Build FANN input
     fann_type* inputFANN = new fann_type[input.size()];
     for (unsigned int i = 0; i < input.size(); ++i)
               inputFANN[i] = input[i];
     
     //   Compute FANN output
     fann_type* outputFANN = fann_run(ann, inputFANN);
     delete inputFANN;
     
     //   Store the FANN output into a vector
     vector<double> output(fann_get_num_output(ann));
     for (unsigned int i = 0; i < fann_get_num_output(ann); ++i)
          output[i] = outputFANN[i];

     /*
          TODO - Modify the output?
               ex. 1:
                    output[i] > 0 ?  1
                    output[i] < 0 ? -1

               ex. 2:
                    output[i] >  threshold ?  1
                    output[i] < -threshold ? -1
                    else                      0
     */
         

     //   Return
     return output;
}


// ===========================================================================
//	Private methods
// ===========================================================================
void ANNAgent::train(vector<vector<double> >& inputs,
                     vector<vector<double> >& outputs,
                     vector<double>& weights,
                     vector<unsigned int>& indexes)
{
     //   Parameters
     unsigned int nANN             = 1;
     bool verbose                  = true;
     double accuracy               = 1e-3;

     
     //   Free previous NNs
     for (unsigned int i = 0; i < annList.size(); ++i)
          if (annList[i] != 0) { fann_destroy(annList[i]); }
     
     annList.clear();
     meansList.clear();
     stDevsList.clear();


     //   Create new NNs
     for (unsigned int n = 0; n < nANN; ++n)
     {
          annList.push_back(0);
          struct fann*& ann = annList.back();


          //   Pack the samples (remove duplicates)
          vector<vector<unsigned int> > packedIndexes;
          packSamples(inputs, outputs, weights, indexes, packedIndexes);


          //   Split the samples in LS, VS and TS
          double pLS = 0.5, pVS = 0.25, pTS = 0.25;
          vector<unsigned int> ls, vs, ts;
          splitSamples(inputs, outputs, weights, packedIndexes,
                       pLS, pVS, pTS, ls,  vs,  ts);


          //   Compute the standardization of the features
          computeStandardizationParameters(n, inputs, ls);


          //   Standardize the inputs
          vector<vector<double> > nInputs;
          for (unsigned int i = 0; i < inputs.size(); ++i)
               nInputs.push_back(standardizeInput(n, inputs[i]));


          //   Load the LS samples          
          fann_train_data* data = 0;
          double* dataWeights   = 0;
          loadFANNData(nInputs, outputs, weights, ls, data, dataWeights);


          //   Build a NN
          unsigned int s = RandomGen::randIntRange_Uniform(0, INT_MAX);

               //   Layers
          unsigned int* layers = new unsigned int[2 + hiddenLayers.size()];
          layers[0] = inputs[0].size();
          for (unsigned int j = 0; j < hiddenLayers.size(); ++j)
               layers[j + 1] = hiddenLayers[j];     
          layers[2 + hiddenLayers.size() - 1] = outputs[0].size();
     
               //   NN
          struct fann* cAnn = fann_create_standard_array(
               2 + hiddenLayers.size(), layers);
          delete[] layers;
          
          RandomGen::setSeed(s);
               
               //   Initialize the weights
          s = RandomGen::randIntRange_Uniform(0, INT_MAX);
          fann_init_weights(cAnn, data);
          
          RandomGen::setSeed(s);


          //   Set the activation functions
          fann_set_activation_function_hidden(cAnn, FANN_SIGMOID_SYMMETRIC);
          fann_set_activation_function_output(cAnn, FANN_SIGMOID_SYMMETRIC);


          //   Set the training algorithm
          fann_set_training_algorithm(cAnn, FANN_TRAIN_INCREMENTAL);
          

          //   Train on LS and minimize the error on VS
          if (verbose)
          {
               cout << "\n";
               cout << setprecision(3) << fixed;
          }

          unsigned int epochPerStep = (epochRange / 5);
          epochPerStep = (epochPerStep == 0 ? 1 : epochPerStep);
          
          unsigned int minEpoch;
          double minVsMSE = 0.0;
          for (unsigned int epoch = 0; epoch <= maxEpoch; ++epoch)
          {
               if (epoch % epochPerStep == 0)
               {
                    //   Evaluate performance on VS
                    double vsMSE =
                              computeMSE(cAnn, nInputs, outputs, weights, vs);

               
                    //   Check if better than the current best one
                    if (ann == 0 ||
                         ((vsMSE < minVsMSE)
                              && fabs(vsMSE - minVsMSE) > accuracy))
                    {                    
                         //   Save the current NN into a temporary file
                         char tmpFileName[] = "data/agents/tmp.XXXXXX";
                         close(mkstemp(tmpFileName));
                         fann_save(cAnn, tmpFileName);
                    
                         //   Copy the NN
                         if (ann != 0) { fann_destroy(ann); }
                         ann = fann_create_from_file(tmpFileName);
                         unlink(tmpFileName);

                         //   Update 'max info'
                         minEpoch = epoch;
                         minVsMSE = vsMSE;
                    }
                    
                    else if (epoch != minEpoch) { learningRate /= 10.0; }
               
               
                    //   Output the current results
                    if (verbose)
                    {
                         double lsMSE =
                              computeMSE(cAnn, nInputs, outputs, weights, ls);
               
                         cout << "\t\t[" << epoch;
                         cout << "]\tLS (mse = " << lsMSE << ")";
                         cout <<  "\tVS (mse = " << vsMSE << ")";
                         if (ann) { cout << "\t(*" << minEpoch << ")"; }
                         
                         cout << "\n";
                    }
               
               
                    //   Stopping condition
                    if (ann != 0
                              && (epoch-minEpoch+epochPerStep) >= epochRange)
                         break;
               }


               //   Train
               fann_shuffle_train_data_w(data, dataWeights);
               
               fann_set_learning_rate(cAnn,
                         decreasingLR ?
                              learningRate / sqrt(epoch + 1) :
                              learningRate);

               if (fann_train_epoch_w(cAnn, data, dataWeights) == -1)
                    break;
          }


          //   Free
          fann_destroy(cAnn);
          delete[] dataWeights;


          /*
               TODO
                    - If we use a threshold for modifying the output
                      (see 'predict()'), we could consider it to be equal to 0.0
                      (no threshold) for the training phase and optimize it
                      right here (dichotomic search should be enough).
          */


          //   Output the performance of the selected ANN on LS, VS and TS
          if (verbose)
          {
               double lsER  = computeER( ann, nInputs, outputs, weights, ls);
               double lsMSE = computeMSE(ann, nInputs, outputs, weights, ls);

               double vsER  = computeER( ann, nInputs, outputs, weights, vs);
               double vsMSE = computeMSE(ann, nInputs, outputs, weights, vs);

               double tsER  = computeER( ann, nInputs, outputs, weights, ts);
               double tsMSE = computeMSE(ann, nInputs, outputs, weights, ts);

               cout << "\n";
               cout << "\t\t[F]";
               cout << "\tLS (";
               cout << "error rate = " << 100*lsER << "%, ";
               cout << "mse = " << lsMSE << ")";
               cout << " VS (";
               cout << "error rate = " << 100*vsER << "%, ";
               cout << "mse = " << vsMSE << ")";
               cout << " TS (";
               cout << "error rate = " << 100*tsER << "%, ";
               cout << "mse = " << tsMSE << ")";
               cout << "\n\t";
          }
     }
}


void ANNAgent::saveSLModel(string fileName) const
{
     ofstream os(fileName.c_str());
     os << annList.size() << "\n";
     for (unsigned int i = 0; i < annList.size(); ++i)
     {
          struct fann * const& ann = annList[i];
          
          if (!ann) { os << 0 << "\n"; }
          
          stringstream sstr;
          sstr << fileName.substr(0, fileName.find_last_of("."));
          sstr << "(" << i << ")";
          sstr << fileName.substr(fileName.find_last_of("."), string::npos);
          fann_save(ann, sstr.str().c_str());
          
          os << sstr.str() << "\n";
     }
     os.close();
}


void ANNAgent::loadSLModel(string fileName)
{
     string tmp;
     ifstream is(fileName.c_str());
     if (!getline(is, tmp)) { throwEOFMsg("ann models"); }


     //   Free previous NNs
     for (unsigned int i = 0; i < annList.size(); ++i)
          if (annList[i] != 0) { fann_destroy(annList[i]); }
     annList.clear();

     unsigned int annListSize = atoi(tmp.c_str());
     for (unsigned int i = 0; i < annListSize; ++i)
     {
          annList.push_back(0);
          struct fann*& ann = annList[i];

          if (!getline(is, tmp)) { throwEOFMsg("ann model"); }
          ann = fann_create_from_file(tmp.c_str());
     }
     is.close();
}


void ANNAgent::computeStandardizationParameters(
          unsigned int n,
          const vector<vector<double> >& inputs,
          const vector<unsigned int>& s)
{
     meansList.resize( n < meansList.size()  ? meansList.size()  : (n + 1));
     stDevsList.resize(n < stDevsList.size() ? stDevsList.size() : (n + 1));
     
     for (unsigned int k = 0; k < inputs[0].size(); ++k)
     {
          vector <double> kV;
          for (unsigned int i = 0; i < s.size(); ++i)
               kV.push_back(inputs[s[i]][k]);

          meansList[n].push_back(statistics::computeMean<double>(kV));
          stDevsList[n].push_back(
               statistics::computeStandardDeviation<double>(
                    meansList[n].back(), kV));
     }
}
  
          
vector<double> ANNAgent::predict(const vector<double>& input) const
{
     //   Predict and return
     vector<double> output = predict(
               annList[0], standardizeInput(0, input));
     
     for (unsigned int i = 1; i < annList.size(); ++i)
     {
          //   Standardization of the features
          vector<double> nInput = standardizeInput(i, input);
     
          vector<double> tmp = predict(annList[i], nInput);
          for (unsigned int j = 0; j < output.size(); ++j)
               output[j] += tmp[j];
     }
     
     return output;
}


vector<double> ANNAgent::standardizeInput(unsigned int n,
                                          const vector<double>& input) const
{
     vector<double> nInput(input.size());
     for (unsigned int k = 0; k < nInput.size(); ++k)
     {
          if (stDevsList[n][k] == 0.0) { nInput[k] = 0.0; continue; }
          nInput[k] = ((input[k] - meansList[n][k]) / stDevsList[n][k]);
     } 
     
     return nInput;
}


#ifndef NDEBUG
void ANNAgent::checkIntegrity() const
{
     SLAgent::checkIntegrity();
     
     for (unsigned int i = 0; i < neuronsPerLayer.size(); ++i)
          assert(neuronsPerLayer[i] > 0);
     
     assert(learningRate > 0.0);
     assert(iniMinWeight <= iniMaxWeight);
}
#endif