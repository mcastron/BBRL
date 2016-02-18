
#include "AgentFactory.h"
#include "EGreedyAgentFactory.h"
#include "SoftMaxAgentFactory.h"
#include "VDBEEGreedyAgentFactory.h"
#include "FormulaAgentFactory.h"
#include "SLAgentFactory/ANNAgentFactory.h"

using namespace utils;


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
dds::AgentFactory::~AgentFactory() {}


// ===========================================================================
//	Public static methods
// ===========================================================================
dds::AgentFactory* dds::AgentFactory::parse(
          int argc, char* argv[],
          bool fromFile,
          bool fromParameters) throw (parsing::ParsingException)
{
     //   Get 'agentFactoryClassName'
     string agentFactoryClassName =
               utils::parsing::getValue(argc, argv, "--agent_factory");


     //   'agentFactoryFile' provided
     try
     {
          if (!fromFile)
               throw parsing::ParsingException("--agent_factory"); 
     
     
          string agentFactoryFile =
                    parsing::getValue(argc, argv, "--agent_factory_file");

          ifstream is(agentFactoryFile.c_str());
          if (is.fail()) // Unable to open the file
               throw parsing::ParsingException("--agent_factory_file");
          
          return dynamic_cast<AgentFactory*>(
                    Serializable::createInstance(agentFactoryClassName, is));
     }
     
     
     //   'agentFile' not provided
     catch (parsing::ParsingException& e)
     {
          if (!fromParameters)
               throw parsing::ParsingException("--agent_factory");
     
     
          //   Get 'agentFactory'
          if (agentFactoryClassName == "EGreedyAgentFactory")
          {
               //   Get 'minEps' and 'maxEps'
               vector<string> tmp =
                         parsing::getValues(argc, argv, "--epsilon", 2);
               double minEps = atof(tmp[0].c_str());
               double maxEps = atof(tmp[1].c_str());
          
               //   Return
               return new EGreedyAgentFactory(minEps, maxEps);
          }
     
          if (agentFactoryClassName == "SoftMaxAgentFactory")
          {
               //   Get 'minTau' and 'maxTau'
               vector<string> tmp =
                         parsing::getValues(argc, argv, "--tau", 2);
               double minTau = atof(tmp[0].c_str());
               double maxTau = atof(tmp[1].c_str());
          
               //   Return
               return new SoftMaxAgentFactory(minTau, maxTau);
          }
     
          if (agentFactoryClassName == "VDBEEGreedyAgentFactory")
          {
               //   Get 'minSigma' and 'maxSigma'
               vector<string> tmp =
                         parsing::getValues(argc, argv, "--sigma", 2);
               double minSigma = atof(tmp[0].c_str());
               double maxSigma = atof(tmp[1].c_str());
          
               //   Get 'minDelta' and 'maxDelta'
               tmp = parsing::getValues(argc, argv, "--delta", 2);
               double minDelta = atof(tmp[0].c_str());
               double maxDelta = atof(tmp[1].c_str());
          
               //   Get 'minEps' and 'maxEps'
               tmp = parsing::getValues(argc, argv, "--ini_epsilon", 2);
               double minIniEps = atof(tmp[0].c_str());
               double maxIniEps = atof(tmp[1].c_str());
          
               //   Return
               return new VDBEEGreedyAgentFactory(
                         minSigma, maxSigma, minDelta, maxDelta,
                         minIniEps, maxIniEps);
          }
     
          if (agentFactoryClassName == "FormulaAgentFactory")
          {
               //   Get 'minW' & 'maxW'
               vector<string> tmp =
                         parsing::getValues(argc, argv, "--weights", 2);
               double minW = atof(tmp[0].c_str());
               double maxW = atof(tmp[1].c_str());
          
               //   Get 'varNameList'
               string tmp2 = parsing::getValue(argc, argv, "--variables");
               unsigned int nVar = atoi(tmp2.c_str());
          
               vector<string> varNameList
                         = parsing::getValues(argc, argv, "--variables", (nVar + 1));                                              
               varNameList.erase(varNameList.begin());
          
          
               //   Return
               return new FormulaAgentFactory(minW, maxW, 0.01, varNameList);
          }
     
          if (agentFactoryClassName == "ANNAgentFactory")
          {
               //   Get 'hiddenLayers'
               string tmp = parsing::getValue(
                    argc, argv, "--hidden_layers");
               unsigned int nLayers = atoi(tmp.c_str());
               
               vector<string> values = parsing::getValues(
                    argc, argv, "--hidden_layers", nLayers + 1);
               
               vector<unsigned int> hiddenLayers;
               for (unsigned int i = 1; i < values.size(); ++i)
                    hiddenLayers.push_back(atoi(values[i].c_str()));


               //   Get 'learningRate'
               tmp = parsing::getValue(argc, argv, "--learning_rate");
               double learningRate = atof(tmp.c_str());

               
               //   Get 'decreasingLearningRate'
               bool decreasingLearningRate =
                    parsing::hasFlag(argc, argv, "--decreasing_learning_rate");

               
               //   Get 'maxEpoch'
               tmp = parsing::getValue(argc, argv, "--max_epoch");
               unsigned int maxEpoch = atoi(tmp.c_str());


               //   Get 'epochRange'
               tmp = parsing::getValue(argc, argv, "--epoch_range");
               unsigned int epochRange = atoi(tmp.c_str());
          
          
               //   Get 'base agent factory'
               string agentFactoryClass =
                         parsing::getValue(argc, argv,
                                           "--base_agent_factory");

               string agentFactoryFileName =
                         parsing::getValue(argc, argv,
                                           "--base_agent_factory_file");

               ifstream is(agentFactoryFileName.c_str());
               if (!is.is_open())
                    throw parsing::ParsingException("--agent_factory");
          
               AgentFactory* baseAgentFactory =
                         dynamic_cast<AgentFactory*>(
                              Serializable::createInstance(
                                   agentFactoryClass, is));
               is.close();
               
               if (dynamic_cast<SLAgentFactory*>(baseAgentFactory) != 0)
               {
                    delete baseAgentFactory;

                    throw parsing::ParsingException(
                              "--agent_factory \
                              (base agent cannot be a SLAgentFactory!)");
               }
          
     
               //   Get 'nbOfMDPs'
               tmp = parsing::getValue(argc, argv, "--n_mdps");
               unsigned int nbOfMDPs = atoi(tmp.c_str());
          
          
               //   Get 'simGamma'
               tmp = parsing::getValue(argc, argv, "--discount_factor");
               double simGamma = atof(tmp.c_str());
          
          
               //   Get 'T'
               tmp = parsing::getValue(argc, argv, "--horizon_limit");
               unsigned int T = atoi(tmp.c_str());
          
          
               //   Get 'SLModelFileName'
               string SLModelFileName =
                         parsing::getValue(argc, argv, "--model_file");
          
               //   Return
               return new ANNAgentFactory(
                         hiddenLayers,
                         learningRate, decreasingLearningRate,
                         maxEpoch, epochRange,
                         baseAgentFactory,
                         nbOfMDPs, simGamma, T, SLModelFileName);
          }
     }

     throw parsing::ParsingException("--agent_factory");
}


// ===========================================================================
//	Public methods
// ===========================================================================
void dds::AgentFactory::serialize(ostream& os) const
{
	Serializable::serialize(os);
	
	
	os << AgentFactory::toString() << "\n";
	os << 0 << "\n";
}


void dds::AgentFactory::deserialize(istream& is) throw (SerializableException)
{
	Serializable::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != AgentFactory::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
