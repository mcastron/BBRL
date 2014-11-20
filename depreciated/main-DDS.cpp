
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

#include "DDS/DDSLib.h"
#include "Utils/UtilsLib.h"

using namespace std;
using namespace dds;
using namespace simulation;
using namespace utils;


// ===========================================================================
/**	
	\brief	Main executable TinyBRL library (DDS).
	
	\author	Castronovo Michael

	\date	2014-08-14
*/
// ===========================================================================
// ---------------------------------------------------------------------------
//	Extra functions header
// ---------------------------------------------------------------------------
string parsingErr(string);
string launchingErr(string);
int getMode(string);
void help();


Agent* getAgent(vector<string>);
AgentFactory* getAgentFactory(vector<string>);
MDPDistribution* getMDPDistribution(vector<string>);
Experiment* getExperiment(vector<string>);


void offline_learning(
		Agent*, MDPDistribution*,
		bool, string) throw (exception);

void new_experiment(
		string, MDPDistribution*, unsigned int,
		unsigned int, double, unsigned int, bool,
		bool, string) throw (exception);

void run_experiment(
	Experiment*, Agent*, unsigned int,
	bool, string,
	unsigned int, unsigned int) throw (exception);


// ---------------------------------------------------------------------------
//	Main function
// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{	
	//	'DDS' package initilization
	init();


	//	Retrieve the request mode
	//		0:	Display help
	//		1:	Offline learning
	//		2:	New Experiment
	//		3:	Run an Experiment
	if (argc < 2)
	{
		string msg;
		msg += "No argument provided!";
		cout << parsingErr(msg) << "\n";
		
		return -1;
	}
	
	int mode = getMode(argv[1]);
	int i = 2;	//	Index of the current command-line parameter
	
	Agent* agent = 0;
	AgentFactory* agentFactory = 0;
	MDPDistribution* mdpDistrib = 0;
	Experiment* exp = 0;
	
	vector<Agent*> strategyList;
	
	
	//	Switch depending on the mode
	switch (mode)
	{
		// ------------------------------------------------------------------
		//	'help' case
		// ------------------------------------------------------------------
		case 0:
			help();
			break;

		
		// ------------------------------------------------------------------
		//	'offline_learning' case
		// ------------------------------------------------------------------
		case 1:
		{
			// -------------------------------------------------------------
			//	Get 'Agent'
			// -------------------------------------------------------------
				//	'--agent'/'-a' token
			if ((i >= argc)
					|| (		(string(argv[i]) != "--agent")
						&&	(string(argv[i]) != "-a")))
			{
				string msg;
				msg += "'--agent'/'-a' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'Agent' arguments
			vector<string> agentArg;
			while ((i < argc)
					&& (string(argv[i]) != "--mdp_distribution")
					&& (string(argv[i]) != "-md"))
			{
				agentArg.push_back(argv[i++]);
			}
			
				//	Instanciation
			agent = getAgent(agentArg);
			if (!agent)
			{
				string msg;
				msg += "Unable to initialize the 'Agent' object!!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}
			
			
			// -------------------------------------------------------------
			//	Get 'MDPDistribution'
			// -------------------------------------------------------------
				//	'--mdp_distribution'/'-md' token
			if ((i >= argc)
					|| (		(string(argv[i]) != "--mdp_distribution")
						&&	(string(argv[i]) != "-md")))
			{
				string msg;
				msg += "'--mdp-distribution'/'-md' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'MDPDistribution' arguments
			vector<string> mdpDistribArg;
			while ((i < argc)
					&& (string(argv[i]) != "--output")
					&& (string(argv[i]) != "-o")
					&& (string(argv[i]) != "--compress_output")
					&& (string(argv[i]) != "-z"))
			{
				mdpDistribArg.push_back(argv[i++]);
			}
			
				//	Instanciation
			mdpDistrib = getMDPDistribution(mdpDistribArg);
			if (!mdpDistrib)
			{
				string msg;
				msg += "Unable to initialize the 'MDPDistribution' ";
				msg += "object!!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}
			
			
			// -------------------------------------------------------------
			//	Get 'compressOutput'
			// -------------------------------------------------------------
			bool compressOutput = false;
			if ((i < argc)
					&& (		(string(argv[i]) == "--compress_output")
						||	(string(argv[i]) == "-z")))
			{
				compressOutput = true;
				++i;
			}
			
			
			// -------------------------------------------------------------
			//	Get 'outputFile'
			// -------------------------------------------------------------
				//	'--output'/'-o' token
			if ((i >= argc)
					|| (		(string(argv[i]) != "--output")
						&&	(string(argv[i]) != "-o")))
			{
				string msg;
				msg += "'--output'/'-o' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'outputFile'
			if (i >= argc)
			{
				string msg;
				msg += "Missing value after '--output'/'-o' token!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}	
			string outputFile = string(argv[i++]);
			
			
			// -------------------------------------------------------------
			//	Launch
			// -------------------------------------------------------------
			try
			{
				offline_learning(agent, mdpDistrib, compressOutput,
						outputFile);
			}
			
			catch (exception& e)
			{
				cout << launchingErr(e.what()) << "\n";
				return -2;
			}
			
			
			break;
		}
		
		
		// ------------------------------------------------------------------
		//	'new_experiment' case
		// ------------------------------------------------------------------
		case 2:
		{
			// -------------------------------------------------------------
			//	Get 'name'
			// -------------------------------------------------------------
				//	'--name' token
			if ((i >= argc) || (string(argv[i]) != "--name"))
			{
				string msg;
				msg += "'--name' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'name'
			if (i >= argc)
			{
				string msg;
				msg += "Missing value after '--name' token!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}	
			string name(argv[i++]);
			
			
			// -------------------------------------------------------------
			//	Get 'MDPDistribution'
			// -------------------------------------------------------------
				//	'--mdp_distribution'/'-md' token
			if ((i >= argc)
					|| (		(string(argv[i]) != "--mdp_distribution")
						&&	(string(argv[i]) != "-md")))
			{
				string msg;
				msg += "'--mdp-distribution'/'-md' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'MDPDistribution' arguments
			vector<string> mdpDistribArg;
			while ((i < argc)
					&& (string(argv[i]) != "--n_mdps")
					&& (string(argv[i]) != "-nm"))
			{
				mdpDistribArg.push_back(argv[i++]);
			}
			
				//	Instanciation
			mdpDistrib = getMDPDistribution(mdpDistribArg);
			if (!mdpDistrib)
			{
				string msg;
				msg += "Unable to initialize the 'MDPDistribution' ";
				msg += "object!!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}
			
			
			// -------------------------------------------------------------
			//	Get 'nbMDPs'
			// -------------------------------------------------------------
				//	'--n_mdps'/'-nbm' token
			if ((i >= argc)
					|| (		(string(argv[i]) != "--n_mdps")
						&&	(string(argv[i]) != "-nm")))
			{
				string msg;
				msg += "'--n_mdps'/'-nm' token is ";
				msg += "missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'nbMDPs'
			if (i >= argc)
			{
				string msg;
				msg += "Missing value after ";
				msg += "'--n_mdps'/'-nm' token!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}	
			unsigned int nbMDPs = atoi(argv[i++]);
			
			
			// -------------------------------------------------------------
			//	Get 'nbSimPerMDP'
			// -------------------------------------------------------------
				//	'--n_simulations_per_mdp'/'-ns' token
			if ((i >= argc)
					|| (	(string(argv[i]) != "--n_simulations_per_mdp")
						&&	(string(argv[i]) != "-ns")))
			{
				string msg;
				msg += "'--n_simulations_per_mdp'/'-ns' token is ";
				msg += "missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'nbSimPerMDP'
			if (i >= argc)
			{
				string msg;
				msg += "Missing value after ";
				msg += "'--n_simulations_per_mdp'/'-ns' token!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}	
			unsigned int nbSimPerMDP = atoi(argv[i++]);
			
			
			// -------------------------------------------------------------
			//	Get 'simGamma'
			// -------------------------------------------------------------
				//	'--discount_factor'/'-gamma' token
			if ((i >= argc)
					|| (	(string(argv[i]) != "--discount_factor")
						&&	(string(argv[i]) != "-gamma")))
			{
				string msg;
				msg += "'--discount_factor'/'-gamma' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'simGamma'
			if (i >= argc)
			{
				string msg;
				msg += "Missing value after ";
				msg += "'--discount_factor'/'-gamma' token!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}	
			double simGamma = atof(argv[i++]);
			
			
			// -------------------------------------------------------------
			//	Get 'T'
			// -------------------------------------------------------------
				//	'--horizon_limit'/'-T' token
			if ((i >= argc)
					|| (	(string(argv[i]) != "--horizon_limit")
						&&	(string(argv[i]) != "-T")))
			{
				string msg;
				msg += "'--horizon_limit'/'-T' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'nbSimPerMDP'
			if (i >= argc)
			{
				string msg;
				msg += "Missing value after '--horizon_limit'/'-T' token!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}	
			unsigned int T = atoi(argv[i++]);
			
			
			// -------------------------------------------------------------
			//	Get 'safeSim'
			// -------------------------------------------------------------
				//	'--safe_simulations'/'-ss' token
			bool safeSim = false;
			if ((i < argc)
					&& (	(string(argv[i]) == "--safe_simulations")
						||	(string(argv[i]) == "-ss")))
			{
				safeSim = true;
				++i;
			}			
			
			
			// -------------------------------------------------------------
			//	Get 'compressOutput'
			// -------------------------------------------------------------
			bool compressOutput = false;
			if ((i < argc)
					&& (		(string(argv[i]) == "--compress_output")
						||	(string(argv[i]) == "-z")))
			{
				compressOutput = true;
				++i;
			}
			
			
			// -------------------------------------------------------------
			//	Get 'outputFile'
			// -------------------------------------------------------------
				//	'--output'/'-o' token
			if ((i >= argc)
					|| (		(string(argv[i]) != "--output")
						&&	(string(argv[i]) != "-o")))
			{
				string msg;
				msg += "'--output'/'-o' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'outputFile'
			if (i >= argc)
			{
				string msg;
				msg += "Missing value after '--output'/'-o' token!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}	
			string outputFile = string(argv[i++]);
			
			
			// -------------------------------------------------------------
			//	Launch
			// -------------------------------------------------------------
			try
			{
				new_experiment(
					name,
					mdpDistrib, nbMDPs,
					nbSimPerMDP, simGamma, T,
					safeSim, compressOutput, outputFile);
			}
			
			catch (exception& e)
			{
				cout << launchingErr(e.what()) << "\n";
				return -2;
			}
			
			break;
		}
		
		
		// ------------------------------------------------------------------
		//	'run_experiment' case
		// ------------------------------------------------------------------
		case 3:
		{
			// -------------------------------------------------------------
			//	Get 'Experiment'
			// -------------------------------------------------------------
				//	'--experiment'/'-e' token			
			if ((i >= argc)
					|| (		(string(argv[i]) != "--experiment")
						&&	(string(argv[i]) != "-e")))
			{
				string msg;
				msg += "'--experiment'/'-e' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}
			++i;
			
				//	'Experiment' arguments
			vector<string> expArg;
			while ((i < argc)
					&& (string(argv[i]) != "--agent")
					&& (string(argv[i]) != "-a"))
			{
				expArg.push_back(argv[i++]);
			}
			
				//	Instanciation
			exp = getExperiment(expArg);
			if (!exp)
			{
				string msg;
				msg += "Unable to initialize the 'Experiment' object!!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}
			
			
			// -------------------------------------------------------------
			//	Get 'Agent'
			// -------------------------------------------------------------
				//	'--agent'/'-a' token
			if ((i >= argc)
					|| (		(string(argv[i]) != "--agent")
						&&	(string(argv[i]) != "-a")))
			{
				string msg;
				msg += "'--agent'/'-a' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'Agent' arguments
			vector<string> agentArg;
			while ((i < argc)
					&& (string(argv[i]) != "--n_threads")
					&& (string(argv[i]) != "-nt"))
			{
				agentArg.push_back(argv[i++]);
			}
			
				//	Instanciation
			agent = getAgent(agentArg);
			if (!agent)
			{
				string msg;
				msg += "Unable to initialize the 'Agent' object!!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}
			
			
			// -------------------------------------------------------------
			//	Get 'nbThreads'
			// -------------------------------------------------------------
				//	'-threads' token
			if ((i >= argc)
					|| (		(string(argv[i]) != "--n_threads")
						&&	(string(argv[i]) != "-nt")))
			{
				string msg;
				msg += "'--n_threads'/'-nt' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}
			++i;
			
				//	'nbThreads'
			if (i >= argc)
			{
				string msg;
				msg += "Missing value after '--n_threads'/'-nt' token!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}	
			unsigned int nbThreads = atoi(argv[i++]);
			
			
			// -------------------------------------------------------------
			//	Get 'compressOutput'
			// -------------------------------------------------------------
			bool compressOutput = false;
			if ((i < argc)
					&& (		(string(argv[i]) == "--compress_output")
						||	(string(argv[i]) == "-z")))
			{
				compressOutput = true;
				++i;
			}

			
			// -------------------------------------------------------------
			//	Get 'outputFile'
			// -------------------------------------------------------------
				//	'--output'/'-o' token
			if ((i >= argc)
					|| (		(string(argv[i]) != "--output")
						&&	(string(argv[i]) != "-o")))
			{
				string msg;
				msg += "'--output'/'-o' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'outputFile'
			if (i >= argc)
			{
				string msg;
				msg += "Missing value after '--output'/'-o' token!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}	
			string outputFile = string(argv[i++]);
			
			
			// -------------------------------------------------------------
			//	Get 'refreshFreq'
			// -------------------------------------------------------------
				//	'--refresh_freq'/'-rf' token
			if ((i >= argc)
					|| (		(string(argv[i]) != "--refresh_frequency")
						&&	(string(argv[i]) != "-rf")))
			{
				string msg;
				msg += "'--refresh_freq'/'-rf' token is missing!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'refreshFreq'
			if (i >= argc)
			{
				string msg;
				msg += "Missing value after '--refresh_frequency'/'-rf' ";
				msg += "token!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}	
			unsigned int refreshFreq = atoi(argv[i++]);
			
			
			// -------------------------------------------------------------
			//	Get 'backupFreq'
			// -------------------------------------------------------------
				//	'--output'/'-o' token
			if ((i >= argc)
					|| (		(string(argv[i]) != "--backup_frequency")
						&&	(string(argv[i]) != "-bf")))
			{
				string msg;
				msg += "'--backup_frequency'/'-bf' token is missing!";
				msg += "(required for 'run_experiment' mode)";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}			
			++i;
			
				//	'backupFreq'
			if (i >= argc)
			{
				string msg;
				msg += "Missing value after '--backup_frequency'/'-bf' ";
				msg += "token!";
				cout << parsingErr(msg) << "\n";
				
				return -1;
			}	
			unsigned int backupFreq = atoi(argv[i++]);
			
			
			// -------------------------------------------------------------
			//	Launch
			// -------------------------------------------------------------
			try
			{
				run_experiment(exp, agent,
						nbThreads, compressOutput, outputFile,
						refreshFreq, backupFreq);
			}

			catch (exception& e)
			{
				cout << launchingErr(e.what()) << "\n";
				return -2;
			}
			
			break;
		}


		// ------------------------------------------------------------------
		//	Error case
		// ------------------------------------------------------------------
		default:
		{
			string msg;
			msg += "Unable to determine the mode to use!";
			cout << parsingErr(msg) << "\n";

			return -1;
		}
	}
	
	
	// -----------------------------------------------------------------------
	//	Free allocated objects
	// -----------------------------------------------------------------------
	if (agent)		{ delete agent;		}
	if (agentFactory)	{ delete agentFactory;	}
	if (mdpDistrib)	{ delete mdpDistrib;	}
	if (exp)			{ delete exp;			}
	
	for (unsigned int i = 0; i < strategyList.size(); ++i)
		delete strategyList[i];
}

// ----------------------------------------------------------------------------
//	Extra functions implementation
// ----------------------------------------------------------------------------
string parsingErr(string msg)
{
	return ("\n\t[parsing error]: " + msg + \
			" (see --help for more details)\n");
}


string launchingErr(string msg)
{
	return ("\n\t[launching error]: " + msg + "\n");
}


int getMode(string modeStr)
{
	if (modeStr == "--help")
		return 0;

	if ((modeStr == "--offline_learning") || (modeStr == "-ol"))
		return 1;

	if ((modeStr == "--new_experiment") || (modeStr == "-ne"))
		return 2;

	if ((modeStr == "--run_experiment") || (modeStr == "-re"))
		return 3;

	else { return -1; }
}


void help()
{
	ifstream is("doc/command-line manual (TinyBRL-DDS).txt");
	string line;
	cout << "\n";
	while (!is.eof()) { getline(is, line); cout << line << "\n"; }
}


Agent* getAgent(vector<string> data)
{
	//	Retrieve the agent's class
	string agentClass = data[0];
	
	
	//	Instanciate the Agent
		//	From file
	if ((data.size() == 3)
			&&	((data[1] == "--agent_file")
				|| (data[1] == "-af")))
	{
		ifstream is(data[2].c_str());
		if (is.fail()) { return 0; }	//	Unable to open the file
		
		Agent* agent = dynamic_cast<Agent*>(
				Serializable::createInstance(agentClass, is));
		is.close();
		
		return agent;
	}
	
		//	From command-line arguments
	else if ((agentClass == "RandomAgent") && (data.size() == 1))
		return (new RandomAgent());

	else if ((agentClass == "OptimalAgent") && (data.size() == 1))
		return (new OptimalAgent());
	
	else if ((agentClass == "EGreedyAgent") && (data.size() == 3)
			&& ((data[1] == "--epsilon") || (data[1] == "-eps")))
	{
		double epsilon = atof(data[2].c_str());
		return (new EGreedyAgent(epsilon));
	}
	
	else if ((agentClass == "SoftMaxAgent") && (data.size() == 3)
			&& (data[1] == "--tau"))
	{
		double tau = atof(data[2].c_str());
		return (new SoftMaxAgent(tau));
	}
	
	else if ((agentClass == "VDBEEGreedyAgent") && (data.size() == 7)
			&& (data[1] == "--sigma")
			&& (data[3] == "--delta")
			&& (data[5] == "--ini_epsilon"))
	{
		double sigma = atof(data[2].c_str());
		double delta = atof(data[4].c_str());
		double iniEpsilon = atof(data[6].c_str());
		return (new VDBEEGreedyAgent(sigma, delta, iniEpsilon));
	}

	else if ((agentClass == "OPPSDSAgent") && (data.size() >= 7)
			&& ((data[1] == "--n_draws") || (data[1] == "-nd"))
			&& (data[3] == "--c")
			&& ((data[5] == "--strategy_list") || (data[5] == "-sl"))
			&& ((data[6] == "--agent_factory") || (data[6] == "-af")))
	{	    
	     unsigned int n = atoi(data[2].c_str());
	     double c = atof(data[4].c_str());
	     
	     vector<string> dataAF;
	     for (unsigned int i = 7;
	              ((i < data.size()) && (data[i] != "--n_samples")); ++i)
	     {
	         dataAF.push_back(data[i]);
	     }
	     AgentFactory* agentFactory = getAgentFactory(dataAF);
	     if (!agentFactory) { return 0; }
	     
	     
	     int j = (7 + dataAF.size());
	     cout << j << ", " << data[j] << ", " << dataAF.size() << "\n";
	     if ((j >= (int) data.size())
	              || (data[j] != "--n_samples")) { return 0; }
	     unsigned int nSamples = atoi(data[j + 1].c_str());
	     
	     vector<pair<double, double> > boundsList
	              = agentFactory->getBounds();
	     vector<Agent*> strategyList;
	     for (unsigned int i = 0; i < nSamples; ++i)
	     {
	         vector<double> paramList;
	         for (unsigned int k = 0; k < boundsList.size(); ++k)
	         {
	              paramList.push_back(RandomGen::randRange_Uniform(
	                        boundsList[k].first, boundsList[k].second));
	         }               
	         
	         strategyList.push_back(agentFactory->get(paramList));
	     }
	     delete agentFactory;

          if (((j + 3) >= (int) data.size()) ||
                    ((data[j + 2] != "--discount_factor")
                    && (data[j + 2] != "-gamma")))
          {
               return 0;
          }
          double gamma = atof(data[j + 3].c_str());
          
	    
	    if (((j + 6) != (int) data.size()) ||
                    ((data[j + 4] != "--horizon_limit")
                    && (data[j + 4] != "-T")))
          {
               return 0;
          }
          unsigned int T = atoi(data[j + 5].c_str());
	    
	    
		return (new OPPSDSAgent(n, c, strategyList, gamma, T));
	}
	
	
	//	Error case
	else { return 0; }
}


AgentFactory* getAgentFactory(vector<string> data)
{
	//	Retrieve the agent's class
	string agentFactoryClass = data[0];
	
	
	//	Instanciate the AgentFactory
	if ((agentFactoryClass == "EGreedyAgentFactory") && (data.size() == 7))
	{
		if ((data[1] != "--epsilon") && (data[1] != "-eps")) { return 0; }
		double minEps = atof(data[2].c_str());
		double maxEps = atof(data[3].c_str());
		
		if ((data[4] != "--counters") && (data[4] != "-c")) { return 0; }
		double minC = atof(data[5].c_str());
		double maxC = atof(data[6].c_str());
	
		return (new EGreedyAgentFactory(minEps, maxEps, minC, maxC));
	}
	
	else if ((agentFactoryClass == "SoftMaxAgentFactory")
			&& (data.size() == 7))
	{
		if (data[1] != "--tau") { return 0; }
		double minTau = atof(data[2].c_str());
		double maxTau = atof(data[3].c_str());
		
		if ((data[4] != "--counters") && (data[4] != "-c")) { return 0; }
		double minC = atof(data[5].c_str());
		double maxC = atof(data[6].c_str());
	
		return (new SoftMaxAgentFactory(minTau, maxTau, minC, maxC));
	}
	
	else if ((agentFactoryClass == "VDBEEGreedyAgentFactory")
			&& (data.size() == 13))
	{
		if (data[1] != "--sigma") { return 0; }
		double minSigma = atof(data[2].c_str());
		double maxSigma = atof(data[3].c_str());

		if (data[4] != "--delta") { return 0; }
		double minDelta = atof(data[5].c_str());
		double maxDelta = atof(data[6].c_str());

		if ((data[7] != "--ini_epsilon") && (data[7] != "-ieps"))
			return 0;
		double minIniEps = atof(data[8].c_str());
		double maxIniEps = atof(data[9].c_str());
		
		if ((data[10] != "--counters") && (data[10] != "-c")) { return 0; }
		double minC = atof(data[11].c_str());
		double maxC = atof(data[12].c_str());
	
		return (new VDBEEGreedyAgentFactory(
				minSigma, maxSigma, minDelta, maxDelta,
				minIniEps, maxIniEps, minC, maxC));
	}


	//	Error case
	else { return 0; }
}


MDPDistribution* getMDPDistribution(vector<string> data)
{
	//	Retrieve the agent's class
	string mdpDistribClass = data[0];
	
	
	//	Instanciate the Agent
		//	From file
	if ((data[1] == "--mdp_distribution_file") || (data[1] == "-mdf"))
	{
		ifstream is(data[2].c_str());
		if (is.fail()) { return 0; }	//	Unable to open the file
		
		MDPDistribution* mdpDistrib = dynamic_cast<MDPDistribution*>(
				Serializable::createInstance(mdpDistribClass, is));
		is.close();
		
		return mdpDistrib;
	}
	
	
	//	Error case
	else { return 0; }	
}


Experiment* getExperiment(vector<string> data)
{
	//	Instanciate the Experiment
		//	From file
	if ((data[0] == "--experiment_file") || (data[0] == "-ef"))
	{
		ifstream is(data[1].c_str());
		if (is.fail()) { return 0; }	//	Unable to open the file
		
		
		Experiment* exp = dynamic_cast<Experiment*>(
				Serializable::createInstance("Experiment", is));
		is.close();
		
		return exp;
	}
	
	
	//	Error case
	else { return 0; }	
}


void offline_learning(Agent* agent, MDPDistribution* mdpDistrib,
		bool compressOutput, string outputFile) throw (exception)
{
	//	Offline learning
	cout << "\n";
	cout << "\tOffline learning..." << flush;
	agent->learnOffline(mdpDistrib);
	cout << "done! (in " << agent->getOfflineTime() << "ms)\n";
	
	
	//	Export the final results
	Chrono exportTime;
	cout << "\tExport the results..." << flush;
	
	if (compressOutput)
	{	
		ofstream os((outputFile + ".zz").c_str());
		agent->zSerialize(os, Z_BEST_COMPRESSION);
		os.close();
	}
	
	else
	{
		ofstream os(outputFile.c_str());
		agent->serialize(os);
		os.close();
	}
	
	cout << "done! (in " << exportTime.get() << "ms)\n\n";
}


void new_experiment(
	string name,
	MDPDistribution* mdpDistrib, unsigned int nbMDPs,
	unsigned int nbSimPerMDP, double simGamma, unsigned int T, bool safeSim,
	bool compressOutput, string outputFile) throw (exception)
{
	//	Create the Experiment
	Chrono newTime;
	cout << "\n";
	cout << "\tCreate the Experiment..." << flush;
	
	vector<MDP*> mdpList;
	for (unsigned int j = 0; j < nbMDPs; ++j)
		mdpList.push_back(mdpDistrib->draw());
	
	Experiment* exp(
		new Experiment(name, mdpList, nbSimPerMDP, simGamma, T, safeSim));
		
	cout << "done! (in " << newTime.get() << "ms)\n";
	
	
	//	Export the Experiment
	Chrono exportTime;
	cout << "\tExport the Experiment..." << flush;
	
	if (compressOutput)
	{
		ofstream os((outputFile + ".zz").c_str());
		exp->zSerialize(os, Z_BEST_COMPRESSION);
		os.close();
	}
	
	else
	{
		ofstream os(outputFile.c_str());
		exp->serialize(os);
		os.close();
	}
	
	cout << "done! (in " << exportTime.get() << "ms)\n\n";
	
	delete exp;
}


void run_experiment(Experiment* exp, Agent* agent,
		unsigned int nbThreads, bool compressOutput, string outputFile,
		unsigned int refreshFreq, unsigned int backupFreq)
											throw (exception)
{
	cout << "\n";
	
	Chrono backupChrono;
	while (exp->getProgress() < 1.0)
	{
		//	Run the Experiment for a certain amount of time
		exp->run(agent, refreshFreq, nbThreads);


		//	Print out a summary of the Experiment
		vector<double> dsrList = exp->computeDSRList();
		pair<double, double> CI95 = statistics::computeCI95<double>(dsrList);

		cout << "\tProgression: " << 100*exp->getProgress() << "%\n";
		cout << "\t\tOffline time:                 ";
		cout << agent->getOfflineTime() << "ms\n";
		cout << "\t\tOnline time (per trajectory): ";
		cout << (exp->getTimeElapsed() / dsrList.size()) << "ms\n";
		cout << "\t\tMean score in                 ";
		cout << "[" << CI95.first << "; " << CI95.second;
		cout << "] (with 95% confidence)\n";
		cout << "\n";
		
		
		//	Create a bakcup if needed
		if ((backupFreq > 0) && (backupChrono.get() >= backupFreq))
		{
			Chrono backupTime;
			
			cout << "\tCreate a backup..." << flush;

			//	Generate a random file name
			string tmpStr
					= outputFile.substr(0, outputFile.find_last_of("/"));
			tmpStr += "/";
			const char alphanum[] =
				"0123456789"
				"!@#$%^&*"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";
			for (int l = 0; l < 42; ++l)
			{ tmpStr += alphanum[rand() % (sizeof(alphanum) - 1)]; }


			if (compressOutput)
			{	
				ofstream os(tmpStr.c_str());
				exp->zSerialize(os, Z_BEST_SPEED);
				os.close();

				rename(tmpStr.c_str(), (outputFile + ".bak.zz").c_str());
			}

			else
			{
				ofstream os(tmpStr.c_str());
				exp->serialize(os);
				os.close();

				rename(tmpStr.c_str(), (outputFile + ".bak").c_str());
			}
			
			cout << "done! (in " << backupTime.get() << "ms)\n\n";
			
			backupChrono.restart();
		}
	}
	
	//	Export the final results
	Chrono exportTime;
			
	cout << "\tExport the results..." << flush;
	
	if (compressOutput)
	{
		ofstream os((outputFile + ".zz").c_str());
		exp->zSerialize(os, Z_BEST_COMPRESSION);
		os.close();
		
		
		//	Remove backup
		remove((outputFile + ".bak.zz").c_str());
	}
	
	else
	{
		ofstream os(outputFile.c_str());
		exp->serialize(os);
		os.close();
		
		//	Remove backup
		remove((outputFile + ".bak").c_str());
	}
	
	cout << "done! (in " << exportTime.get() << "ms)\n\n";
}
