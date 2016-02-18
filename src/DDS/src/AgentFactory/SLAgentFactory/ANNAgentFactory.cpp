
#include "ANNAgentFactory.h"

using namespace std;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
dds::ANNAgentFactory::ANNAgentFactory(std::istream& is)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


dds::ANNAgentFactory::ANNAgentFactory(
          const std::vector<unsigned int>& hiddenLayers_,
          double learningRate_,
          bool decreasingLearningRate_,
          unsigned int maxEpoch_,
          unsigned int epochRange_,
          AgentFactory* agentFactory_,
          unsigned int nbOfMDPs_,
          double simGamma_,
          unsigned int T_,
          std::string SLModelFileName_) :
               SLAgentFactory(agentFactory_,
                              nbOfMDPs_, simGamma_, T_,
                              SLModelFileName_) {}


dds::ANNAgentFactory::~ANNAgentFactory() {}


// ===========================================================================
//	Public methods
// ===========================================================================
void dds::ANNAgentFactory::serialize(ostream& os) const
{
	SLAgentFactory::serialize(os);
	
	
	os << ANNAgentFactory::toString() << "\n";
	os << 5 << "\n";
	
	
	//   'hiddenLayers'
	os << hiddenLayers.size() << "\n";
	for (unsigned int i = 0; i < hiddenLayers.size(); ++i)
	     os << hiddenLayers[i] << "\n";
	

     //   'learningRate'
     os << learningRate << "\n";


     //   'decreasingLearningRate'
     os << (decreasingLearningRate ? 1 : 0) << "\n";
     
     
     //   'maxEpoch'
     os << maxEpoch << "\n";


     //   'epochRange'
     os << epochRange << "\n";
}


void dds::ANNAgentFactory::deserialize(istream& is)
                                                  throw (SerializableException)
{
	SLAgentFactory::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != ANNAgentFactory::toString())
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
     
     
     //   'decreasingLearningRate'
     if (!getline(is, tmp)) { throwEOFMsg("decreasingLearningRate"); }
     decreasingLearningRate = (atoi(tmp.c_str()) != 0);
     ++i;
     
     
     //   'maxEpoch'
     if (!getline(is, tmp)) { throwEOFMsg("maxEpoch"); }
     maxEpoch = atoi(tmp.c_str());
     ++i;
     
     
     //   'epochRange'
     if (!getline(is, tmp)) { throwEOFMsg("epochRange"); }
     epochRange = atoi(tmp.c_str());
     ++i;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}


// ===========================================================================
//   Private methods
// ===========================================================================
dds::ANNAgent* dds::ANNAgentFactory::getSLAgent(Agent* agent,
                                                unsigned int nbOfMDPs,
                                                double simGamma,
                                                unsigned int T,
                                                string SLModelFileName) const
{
     return new dds::ANNAgent(hiddenLayers,
                              learningRate, decreasingLearningRate,
                              maxEpoch, epochRange,
                              agent, nbOfMDPs, simGamma, T, SLModelFileName);
}
