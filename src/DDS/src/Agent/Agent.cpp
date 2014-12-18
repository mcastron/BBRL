
#include "Agent.h"
#include "RandomAgent.h"
#include "OptimalAgent.h"
#include "EGreedyAgent.h"
#include "SoftMaxAgent.h"
#include "VDBEEGreedyAgent.h"
#include "FormulaAgent/FormulaAgent.h"
#include "FormulaAgent/FormulaVector.h"
#include "BAMCPAgent.h"
#include "BFS3Agent.h"
#include "SBOSSAgent.h"
#include "OPPSDSAgent.h"
#include "OPPSCSAgent.h"


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
Agent* Agent::parse(int argc, char* argv[]) throw (parsing::ParsingException)
{     
      //   Get 'agentClassName'
     string agentClassName = parsing::getValue(argc, argv, "--agent");
     
     //   'agentFile' provided
     try
     {
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
          //   Get 'agent'
          if (agentClassName == RandomAgent::toString())
               return new RandomAgent();
     
          if (agentClassName == OptimalAgent::toString())
               return new OptimalAgent();        
          
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
               //   Get 'K'
               string fStr = parsing::getValue(argc, argv, "--formula");
               
               
               //   Return
               return new FormulaAgent(new formula::Formula(fStr));
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
               string tmp = parsing::getValue(argc, argv, "--K");
               unsigned int K = atoi(tmp.c_str());


               //   Get 'delta'
               tmp = parsing::getValue(argc, argv, "--C");
               double delta = atof(tmp.c_str());


               //   Return
               return new SBOSSAgent(K, delta);
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
                              new FormulaAgent((*formulaVector)[i]));
                    
                    //   Free 'formulaVector'
               delete formulaVector;
               
               
               //   Return
               return new OPPSDSAgent(nDraws, c, strategyList, gamma, T);
          }
          
          if (agentClassName == OPPSCSAgent::toString())
          {
               //   Get 'nDraws'
               string tmp = parsing::getValue(argc, argv, "--n_draws");
               unsigned int nDraws = atoi(tmp.c_str());
               
               
               //   Get 'c'
               tmp = parsing::getValue(argc, argv, "--c");
               double c = atof(tmp.c_str());
               
               
               //   Get 'agentFactory'
               AgentFactory* agentFactory = AgentFactory::parse(argc, argv);
               assert(agentFactory);
               
               
               //   Get 'gamma'
               tmp = parsing::getValue(argc, argv, "--discount_factor");
               double gamma = atof(tmp.c_str());
               
               
               //   Get 'T'
               tmp = parsing::getValue(argc, argv, "--horizon_limit");
               unsigned int T = atoi(tmp.c_str());
               
               
               //   Return
               return new OPPSCSAgent(nDraws, c, agentFactory, gamma, T);
               
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
