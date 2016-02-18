
#include "Agent.h"
#include "RandomAgent.h"
#include "OptimalAgent.h"
#include "EAgent.h"
#include "EGreedyAgent.h"
#include "SoftMaxAgent.h"
#include "VDBEEGreedyAgent.h"
#include "FormulaAgent/FormulaAgent.h"
#include "FormulaAgent/FormulaVector.h"
#include "BAMCPAgent.h"
#include "BFS3Agent.h"
#include "SBOSSAgent.h"
#include "BEBAgent.h"
#include "OPPSDSAgent.h"
#include "OPPSCSAgent.h"
#include "SLAgent/ANNAgent.h"


using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
Agent::Agent(string name) : IAgent(name), mdp(0), offlineTime(0.0)
{
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


// ===========================================================================
//	Public static methods
// ===========================================================================
Agent* Agent::parse(int argc, char* argv[],
                    bool fromFile,
                    bool fromParameters) throw (parsing::ParsingException)
{     
      //   Get 'agentClassName'
     string agentClassName = parsing::getValue(argc, argv, "--agent");
     
     //   'agentFile' provided
     try
     {
          if (!fromFile) { throw parsing::ParsingException("--agent"); }


          string agentFile = parsing::getValue(argc, argv, "--agent_file");

          ifstream is(agentFile.c_str());
          if (is.fail()) // Unable to open the file
               throw parsing::ParsingException("--agent_file");
          
          return dynamic_cast<Agent*>(
                    Serializable::createInstance(agentClassName, is));
     }
     
     
     //   'agentFile' not provided
     catch (parsing::ParsingException& e)
     {
          if (!fromParameters) { throw parsing::ParsingException("--agent"); }

     
          //   Get 'agent'
          if (agentClassName == RandomAgent::toString())
               return new RandomAgent();
     
          if (agentClassName == OptimalAgent::toString())
               return new OptimalAgent();        
          
          if (agentClassName == EAgent::toString())
          {
               //   Get 'epsilon'         
               string tmp = parsing::getValue(argc, argv, "--epsilon");
               double epsilon = atof(tmp.c_str());
               
               
               //   Get 'base agent'
               int argcBase;
               char** argvBase;
               for (unsigned int i = 1; i < argc; ++i)
               {
                    //   '--base_agent' not found
                    if (string(argv[i]) != "--base_agent") { continue; }
                    
                    
                    //   '--base_agent' found
                    argvBase = new char*[2 + (argc - i - 1)];
                    argvBase[0] = argv[0];
                    char str[] = "--agent";
                    argvBase[1] = str;
                    
                    argcBase = 2;
                    for (unsigned int j = i + 1; j < argc; ++j)
                         argvBase[argcBase++] = argv[j];
                        
                    break;
               }
               Agent* baseAgent = Agent::parse(argcBase, argvBase, false);
               
               
               //   Return
               return new EAgent(epsilon, baseAgent);
          }
          
          if (agentClassName == EGreedyAgent::toString())
          {
               //   Get 'epsilon'         
               string tmp = parsing::getValue(argc, argv, "--epsilon");
               double epsilon = atof(tmp.c_str());
               
               
               //   Return
               return new EGreedyAgent(epsilon);
          }
          
          if (agentClassName == SoftMaxAgent::toString())
          {
               //   Get 'tau'
               string tmp = parsing::getValue(argc, argv, "--tau");
               double tau = atof(tmp.c_str());
               
               
               //   Return
               return new SoftMaxAgent(tau);
     
          }
          
          if (agentClassName == VDBEEGreedyAgent::toString())
          {
               //   Get 'sigma'
               string tmp = parsing::getValue(argc, argv, "--sigma");
               double sigma = atof(tmp.c_str());
               
               
               //   Get 'delta'
               tmp = parsing::getValue(argc, argv, "--delta");
               double delta = atof(tmp.c_str());
     
     
               //   Get 'iniEpsilon'
               tmp = parsing::getValue(argc, argv, "--ini_epsilon");
               double iniEpsilon = atof(tmp.c_str());
               
               
               //   Return
               return new VDBEEGreedyAgent(sigma, delta, iniEpsilon);
          }
          
          if (agentClassName == FormulaAgent::toString())
          {     
               //   Get 'f'
               string fStr = parsing::getValue(argc, argv, "--formula");
               utils::formula::Formula* f = new utils::formula::Formula(fStr);
               
               
               //   Get 'varNameList'
               string tmp = parsing::getValue(argc, argv, "--variables");
               unsigned int nVar = atoi(tmp.c_str());
               
               vector<string> varNameList
                         = parsing::getValues(argc, argv,
                                              "--variables", (nVar + 1));                                              
               varNameList.erase(varNameList.begin());
               
               
               //   Return
               return new FormulaAgent(f, varNameList);
          }
          
          if (agentClassName == BAMCPAgent::toString())
          {     
               //   Get 'K'
               string tmp = parsing::getValue(argc, argv, "--K");
               unsigned int K = atoi(tmp.c_str());
               
               try
               {
                    //   Get 'D'
                    tmp = parsing::getValue(argc, argv, "--D");
                    unsigned int D = atoi(tmp.c_str());               


                    //   Return
                    return new BAMCPAgent(K, D);
               }

               catch (parsing::ParsingException e)
               {
                    return new BAMCPAgent(K);
               }     
          }
          
          if (agentClassName == BFS3Agent::toString())
          {     
               //   Get 'K'
               string tmp = parsing::getValue(argc, argv, "--K");
               unsigned int K = atoi(tmp.c_str());


               //   Get 'C'
               tmp = parsing::getValue(argc, argv, "--C");
               unsigned int C = atoi(tmp.c_str());


               try
               {
                    //   Get 'D'
                    tmp = parsing::getValue(argc, argv, "--D");
                    unsigned int D = atoi(tmp.c_str());               


                    //   Return
                    return new BFS3Agent(K, C, D);
               }

               catch (parsing::ParsingException e)
               {
                    return new BFS3Agent(K, C);
               }
          }
          
          if (agentClassName == SBOSSAgent::toString())
          {     
               //   Get 'K'
               string tmp = parsing::getValue(argc, argv, "--epsilon");
               double epsilon = atof(tmp.c_str());


               //   Get 'delta'
               tmp = parsing::getValue(argc, argv, "--delta");
               double delta = atof(tmp.c_str());


               //   Return
               return new SBOSSAgent(epsilon, delta);
          }
          
          if (agentClassName == BEBAgent::toString())
          {     
               //   Get 'beta'
               string tmp = parsing::getValue(argc, argv, "--beta");
               double beta = atof(tmp.c_str());


               //   Return
               return new BEBAgent(beta);
          }
          
          if (agentClassName == OPPSDSAgent::toString())
          {
               //   Get 'nDraws'
               string tmp = parsing::getValue(argc, argv, "--n_draws");
               unsigned int nDraws = atoi(tmp.c_str());
               
               
               //   Get 'c'
               tmp = parsing::getValue(argc, argv, "--c");
               double c = atof(tmp.c_str());
               
               
               //   Get 'formulaVector'
               FormulaVector* formulaVector = FormulaVector::parse(argc, argv);
               assert(formulaVector);
               
               
               //   Get 'varNameList'
               tmp = parsing::getValue(argc, argv, "--variables");
               unsigned int nVar = atoi(tmp.c_str());
               
               vector<string> varNameList
                         = parsing::getValues(argc, argv,
                                              "--variables", (nVar + 1));                                              
               varNameList.erase(varNameList.begin());
               
               
               //   Get 'gamma'
               tmp = parsing::getValue(argc, argv, "--discount_factor");
               double gamma = atof(tmp.c_str());
               
               
               //   Get 'T'
               tmp = parsing::getValue(argc, argv, "--horizon_limit");
               unsigned int T = atoi(tmp.c_str());
               
               
               //   Build 'strategyList'
                    //   Get 'mdpDistrib'
               MDPDistribution* mdpDistrib = MDPDistribution::parse(argc, argv);
               assert(mdpDistrib);
               
                    //   Generate and store the strategies
               vector<Agent*> strategyList;
               for (unsigned int i = 0; i < formulaVector->size(); ++i)
                    strategyList.push_back(
                              new FormulaAgent((*formulaVector)[i],
                                               varNameList));
                    
                    //   Free 'formulaVector'
               delete formulaVector;
               
               
               //   Return
               return new OPPSDSAgent(nDraws, c, strategyList, gamma, T);
          }
          
          if (agentClassName == OPPSCSAgent::toString())
          {
               //   Get 'n'
               string tmp = parsing::getValue(argc, argv, "--n_eval");
               unsigned int nEval = atoi(tmp.c_str());


               //   Get 'K'
               tmp = parsing::getValue(argc, argv, "--K");
               unsigned int K = atoi(tmp.c_str());


               //   Get 'agentFactory'
               AgentFactory* agentFactory =
                         AgentFactory::parse(argc, argv, true, false);
               assert(agentFactory);
               
               
               //   Get 'gamma'
               tmp = parsing::getValue(argc, argv, "--discount_factor");
               double gamma = atof(tmp.c_str());
               
               
               //   Get 'T'
               tmp = parsing::getValue(argc, argv, "--horizon_limit");
               unsigned int T = atoi(tmp.c_str());
               
               
               //   Check if 'k', 'hMax' & delta are specified
               bool hasSmallK = parsing::hasFlag(argc, argv, "--k");
               bool hasHMax   = parsing::hasFlag(argc, argv, "--h_max");
               bool hasDelta  = parsing::hasFlag(argc, argv, "--delta");
               
               
               //   Case 1:   'k', 'hMax' & 'delta' are specified
               if (hasSmallK && hasHMax && hasDelta)
               {
                    tmp = parsing::getValue(argc, argv, "--k");
                    unsigned int k = atoi(tmp.c_str());

                    tmp = parsing::getValue(argc, argv, "--h_max");
                    unsigned int hMax = atoi(tmp.c_str());

                    tmp = parsing::getValue(argc, argv, "--delta");
                    double delta = atof(tmp.c_str());
                    
                    return new OPPSCSAgent(nEval, K, k, hMax, delta,
                                           agentFactory, gamma, T);
               }


               //  Case 2:    'k', 'hMax' & 'delta' are not specified 
               else if (!hasSmallK && !hasHMax && !hasDelta)
                    return new OPPSCSAgent(nEval, K, agentFactory, gamma, T);
               
               
               //   Case 3:   Among 'k', 'hMax' & 'delta', at least one is
               //             specified and one is not specified
               else
               {
                    tmp = "Cannot define 'k', 'h_max' or 'delta' without ";
                    tmp += "defining the others!\n";
                    throw parsing::ParsingException(tmp);
               }               
          }
          
          if (agentClassName == ANNAgent::toString())
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
          
          
               //   Get 'base agent'
               int argcBase;
               char** argvBase;
               for (unsigned int i = 1; i < argc; ++i)
               {
                    //   '--base_agent' not found
                    if (string(argv[i]) != "--base_agent") { continue; }
                    
                    
                    //   '--base_agent' found
                    argvBase = new char*[2 + (argc - i - 1)];
                    argvBase[0] = argv[0];
                    char str[] = "--agent";
                    argvBase[1] = str;
                    
                    argcBase = 2;
                    for (unsigned int j = i + 1; j < argc; ++j)
                         argvBase[argcBase++] = argv[j];
                        
                    break;
               }
               Agent* baseAgent = Agent::parse(argcBase, argvBase, false);
               
               if (dynamic_cast<SLAgent*>(baseAgent) != 0)
               {
                    delete baseAgent;
                    throw parsing::ParsingException( "--base_agent");
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
               return new ANNAgent(
                         hiddenLayers, learningRate, decreasingLearningRate,
                         maxEpoch, epochRange,
                         baseAgent, nbOfMDPs, simGamma, T, SLModelFileName);
          }
     }
     
     throw parsing::ParsingException("--agent");
}


// ===========================================================================
//	Public methods
// ===========================================================================
void Agent::learnOffline(const MDPDistribution* mdpDistrib)
											throw (AgentException)
{
	assert(mdpDistrib);

	
	utils::Chrono chrono;
	learnOffline_aux(mdpDistrib);
	offlineTime = chrono.get();
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void Agent::setMDP(const MDP* mdp_, double gamma_, unsigned int T_)
											throw (AgentException)
{
	//	Parameters check
	assert(mdp_);
	assert((gamma_ > 0.0) && (gamma_ <= 1.0));
	
	
	//	Associate the new MDP
	mdp = mdp_;
	gamma = gamma_;
	T = T_;


	//	Reset the agent
	reset();
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}

void Agent::serialize(ostream& os) const
{
	IAgent::serialize(os);
	
	
	os << Agent::toString() << "\n";
	os << 1 << "\n";


	//	'offlineTime'
	os << offlineTime << "\n";
}


void Agent::deserialize(istream& is) throw (SerializableException)
{
	IAgent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != Agent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	
	int i = 0;
	
	
	//	'offlineTime'
	if (!getline(is, tmp)) { throwEOFMsg("offlineTimeElapsed"); }
	offlineTime = atof(tmp.c_str());
	++i;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


// ===========================================================================
//	Private methods
// ===========================================================================
#ifndef NDEBUG
void Agent::checkIntegrity() const { assert(offlineTime >= 0.0); }
#endif
