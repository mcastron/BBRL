
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>

#include "DDS/DDSLib.h"
#include "Utils/UtilsLib.h"

using namespace std;
using namespace dds;
using namespace simulation;
using namespace utils;
using namespace utils::formula;


// ===========================================================================
/**	
	\brief	A simple command-line program for BBRL library
	
	\author	Castronovo Michael

	\date	2015-12-21
*/
// ===========================================================================
// ---------------------------------------------------------------------------
//   Extra functions
// ---------------------------------------------------------------------------
inline bool fileExists(const std::string& name)
{
     struct stat buffer;   
     return (stat (name.c_str(), &buffer) == 0);
}

void offlineLearning(int argc, char* argv[]) throw (AgentException,
                                                    MDPException,
                                                    parsing::ParsingException);
void newExperiment(int argc, char* argv[])   throw (parsing::ParsingException);
void runExperiment(int argc, char* argv[])   throw (AgentException,
                                                    MDPException,
                                                    parsing::ParsingException);

void mdpDistribGen(int argc, char* argv[]) throw (MDPException,
                                                  parsing::ParsingException);
void formulaSetGen(int argc, char* argv[]) throw (parsing::ParsingException);
void slSamplesGen(int argc, char* argv[])  throw (bad_cast,
                                                  parsing::ParsingException);
void newAgentFactory(int argc, char* argv[]) throw (SerializableException,
                                                    parsing::ParsingException);

void help();


// ---------------------------------------------------------------------------
//	Main function
// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	//   Parsing and execution
	    //   0.   Get seed (if specified) in order to initialize 'DDS'.
     bool hasSeed = parsing::hasFlag(argc, argv, "--seed");
     if (hasSeed)
     {
          int seed = atoi(parsing::getValue(argc, argv, "--seed").c_str());
          init((seed > 0) ? seed : 0);
     }
     else { init(); }

	
	    //   1.   Get the right mode
     bool modeIsOfflineLearning =
          parsing::hasFlag(argc, argv, "--offline_learning");

     bool modeIsNewExperiment =
          parsing::hasFlag(argc, argv, "--new_experiment");

     bool modeIsRunExperiment =
          parsing::hasFlag(argc, argv, "--run_experiment");

     bool modeIsMDPDistribGen =
          parsing::hasFlag(argc, argv, "--mdp_distrib_generation");
          
     bool modeIsFormulaSetGen =
          parsing::hasFlag(argc, argv, "--formula_set_generation");

     bool modeIsSLSamplesGen =
          parsing::hasFlag(argc, argv, "--sl_samples_generation");

     bool modeIsNewAgentFactory =
          parsing::hasFlag(argc, argv, "--new_agent_factory");

     bool modeIsHelp =
	     parsing::hasFlag(argc, argv, "--help");
    
	    
	    //   2.   Launch the selected mode
     try
     {
          if      (modeIsOfflineLearning) { offlineLearning(argc, argv);   }
          else if (modeIsNewExperiment)   { newExperiment(argc, argv);     }
          else if (modeIsRunExperiment)   { runExperiment(argc, argv);     }
          else if (modeIsMDPDistribGen)   { mdpDistribGen(argc, argv);     }
          else if (modeIsFormulaSetGen)   { formulaSetGen(argc, argv);     }
          else if (modeIsSLSamplesGen)    { slSamplesGen(argc, argv);      }
          else if (modeIsNewAgentFactory) { newAgentFactory(argc, argv);   }
          else if (modeIsHelp)            { help();                        }
          else             { cout << "No mode selected! (see '--help')\n"; }
     }
     
     catch (AgentException& e)
     {
          cout << "\nagent exception: " << e.what();
          cout << "\n\n";
     }
     
     catch (MDPException& e)
     {
          cout << "\nMDP exception: " << e.what();
          cout << "\n\n";
     }
     
     catch (parsing::ParsingException& e)
     {
          cout << "\nparsing exception: " << e.what();
          cout << "\n(see '--help')\n\n";
     }
     
     catch (std::exception& e)
     {
          cout << "\nexception: " << e.what();
          cout << "\n\n";
     }
}


// ---------------------------------------------------------------------------
//	Extra functions implementation
// ---------------------------------------------------------------------------
void offlineLearning(int argc, char* argv[]) throw (AgentException,
                                                    MDPException,
                                                    parsing::ParsingException)
{
     //   1.   Get 'agent'
     Agent* agent = Agent::parse(argc, argv, false);
     assert(agent);


     //   2.   Get 'mdpDistrib'
     MDPDistribution* mdpDistrib = MDPDistribution::parse(argc, argv);
     assert(mdpDistrib);

     
     //   3.   Get 'compressOutput', 'output'     
     bool compressOutput = parsing::hasFlag(argc, argv, "--compress_output");
     string output = parsing::getValue(argc, argv, "--output");     
     assert(output != "");
     
     if (fileExists(output + (compressOutput ? ".zz" : "")))
     {
          cout << "\n\tThe file \"";
          cout << (output + (compressOutput ? ".zz" : ""));
          cout << "\" already exists!\n\n";
          return;
     }
     
     
     //   4.   Run
	cout << "\n";
	cout << "\tOffline learning..." << flush;
	agent->learnOffline(mdpDistrib);
	cout << "done! (in " << agent->getOfflineTime() << "ms)\n";
	
	Chrono exportTime;
	cout << "\tExport the results..." << flush;
	
	if (compressOutput)
	{	
		ofstream os((output + ".zz").c_str());
		agent->zSerialize(os, Z_BEST_COMPRESSION);
		os.close();
	}
	
	else
	{
		ofstream os(output.c_str());
		agent->serialize(os);
		os.close();
	}
	
	cout << "done! (in " << exportTime.get() << "ms)\n\n";
	
	
	//  5.   Free data
	delete agent;
	delete mdpDistrib;
}

void newExperiment(int argc, char* argv[]) throw (parsing::ParsingException)
{
     //   1.   Get 'name'
     string name = parsing::getValue(argc, argv, "--name");
     
     
     //   2.   Get 'mdpDistrib'
     MDPDistribution* mdpDistrib = MDPDistribution::parse(argc, argv);
     assert(mdpDistrib);
     
     
     //   3.   Get 'n_mdps', 'n_simulations_per_mdp', 'discount_factor',
     //        'horizon_limit', 'safe_simulations', 'compress_output', 'output'     
     string tmp = parsing::getValue(argc, argv, "--n_mdps");
     unsigned int nMdps = atoi(tmp.c_str());
     
     tmp = parsing::getValue(argc, argv, "--n_simulations_per_mdp");
     unsigned int nSimulationsPerMdp = atoi(tmp.c_str());
     
     tmp = parsing::getValue(argc, argv, "--discount_factor");
     double discountFactor = atof(tmp.c_str());
     
     tmp = parsing::getValue(argc, argv, "--horizon_limit");
     unsigned int horizonLimit = atoi(tmp.c_str());

     bool saveTraj = parsing::hasFlag(argc, argv, "--save_trajectories");
     bool safeSimulations = parsing::hasFlag(argc, argv, "--safe_simulations");
     bool compressOutput = parsing::hasFlag(argc, argv, "--compress_output");
     
     string output = parsing::getValue(argc, argv, "--output");
     
     if (fileExists(output + (compressOutput ? ".zz" : "")))
     {
          cout << "\n\tThe file \"";
          cout << (output + (compressOutput ? ".zz" : ""));
          cout << "\" already exists!\n\n";
          return;
     }
     
     
     assert(nMdps > 0);
     assert(nSimulationsPerMdp > 0);
     assert((discountFactor > 0.0) && (discountFactor <= 1.0));
     assert(horizonLimit > 0);
     assert(output != "");
     
     
     //   4.   Run
	Chrono newTime;
	cout << "\n";
	cout << "\tCreate the Experiment..." << flush;
	
	vector<MDP*> mdpList;
	for (unsigned int j = 0; j < nMdps; ++j)
		mdpList.push_back(mdpDistrib->draw());
	
	Experiment* experiment(
		new Experiment(name, mdpList, nSimulationsPerMdp,
		discountFactor, horizonLimit, safeSimulations, saveTraj));
		
	cout << "done! (in " << newTime.get() << "ms)\n";
	
	
	    //	Export the Experiment
	Chrono exportTime;
	cout << "\tExport the Experiment..." << flush;
	
	if (compressOutput)
	{
		ofstream os((output + ".zz").c_str());
		experiment->zSerialize(os, Z_BEST_COMPRESSION);
		os.close();
	}
	
	else
	{
		ofstream os(output.c_str());
		experiment->serialize(os);
		os.close();
	}
	
	cout << "done! (in " << exportTime.get() << "ms)\n\n";
	
	
	//  5.   Free data
	delete experiment;
	delete mdpDistrib;
}

void runExperiment(int argc, char* argv[]) throw (AgentException,
                                                  MDPException,
                                                  parsing::ParsingException)
{
     //   1.   Get 'experiment'
     Experiment* experiment = 0;
     
          //   Backup case
     bool compressOutput = parsing::hasFlag(argc, argv, "--compress_output");     
     string output = parsing::getValue(argc, argv, "--output");
     
     if (fileExists(output + ".bak" + (compressOutput ? ".zz" : "")))
     {
          int argcBis = 4;
          char* argvBis[4];
          string str;
          
          argvBis[0] = argv[0];
          
          str = "--experiment";
          argvBis[1] = new char[str.size() + 1];
          std::copy(str.begin(), str.end(), argvBis[1]);
          argvBis[1][str.size()] = '\0';
          
          str = "--experiment_file";
          argvBis[2] = new char[str.size() + 1];
          std::copy(str.begin(), str.end(), argvBis[2]);
          argvBis[2][str.size()] = '\0';
          
          str = (output + ".bak" + (compressOutput ? ".zz" : ""));
          argvBis[3] = new char[str.size() + 1];
          std::copy(str.begin(), str.end(), argvBis[3]);
          argvBis[3][str.size()] = '\0';

          experiment = Experiment::parse(argcBis, argvBis);
     }

          //   Other case
     else { experiment = Experiment::parse(argc, argv); }

     assert(experiment);
     
     
     //   2.   Get 'agent'
     Agent* agent = Agent::parse(argc, argv, true, false);
     assert(agent);
     
     
     //   3.   Get 'n_threads', 'compress_output', 'output',
     //        'refresh_frequency', 'backup_frequency'     
     string tmp = parsing::getValue(argc, argv, "--n_threads");
     unsigned int nThreads = atoi(tmp.c_str());

     tmp = parsing::getValue(argc, argv, "--refresh_frequency");
     unsigned int refreshFrequency = (1000.0 * atoi(tmp.c_str()));
     
     tmp = parsing::getValue(argc, argv, "--backup_frequency");
     unsigned int backupFrequency = (1000.0 * atoi(tmp.c_str()));


     assert(nThreads > 0);
     assert(output != "");
     
     if (fileExists(output + (compressOutput ? ".zz" : "")))
     {
          cout << "\n\tThe file \"";
          cout << (output + (compressOutput ? ".zz" : ""));
          cout << "\" already exists!\n\n";
          return;
     }
     
     
     //   Special case for agents based on A. Guez implementation
     if (((agent->getClassName() == BAMCPAgent::toString())
               || (agent->getClassName() == BFS3Agent::toString())
               || (agent->getClassName() == SBOSSAgent::toString())
               || (agent->getClassName() == BEBAgent::toString()))
               && (nThreads > 1))
     {
          cout << "\n\tCannot use this agent with more than one thread!\n";
          return;
     }          
     //
     
     
     //   4.   Run
     Chrono backupChrono;
	while (experiment->getProgress() < 1.0)
	{
		//	Run the Experiment for a certain amount of time
		experiment->run(agent, refreshFrequency, nThreads);


		//	Print out a summary of the Experiment
		vector<double> dsrList = experiment->computeDSRList();
		pair<double, double> CI95 = statistics::computeCI95<double>(dsrList);

		cout << "\tProgression: " << 100*experiment->getProgress() << "%\n";
		cout << "\t\tOffline time:                 ";
		cout << agent->getOfflineTime() << "ms\n";
		cout << "\t\tOnline time (per trajectory): ";
		cout << (experiment->getTimeElapsed() / dsrList.size()) << "ms\n";
		cout << "\t\tExpected return in            ";
		cout << "[" << CI95.first << "; " << CI95.second;
		cout << "] (with 95% confidence)\n";
		cout << "\n";
		
		
		//	Create a bakcup if needed
		if ((backupFrequency > 0) && (backupChrono.get() >= backupFrequency))
		{
			Chrono backupTime;
			
			cout << "\tCreate a backup..." << flush;

			//	Generate a random file name
			string tmpStr
					= output.substr(0, output.find_last_of("/"));
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
				experiment->zSerialize(os, Z_BEST_SPEED);
				os.close();

				rename(tmpStr.c_str(), (output + ".bak.zz").c_str());
			}

			else
			{
				ofstream os(tmpStr.c_str());
				experiment->serialize(os);
				os.close();

				rename(tmpStr.c_str(), (output + ".bak").c_str());
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
		ofstream os((output + ".zz").c_str());
		experiment->zSerialize(os, Z_BEST_COMPRESSION);
		os.close();
		
		
		//	Remove backup
		remove((output + ".bak.zz").c_str());
	}
	
	else
	{
		ofstream os(output.c_str());
		experiment->serialize(os);
		os.close();
		
		//	Remove backup
		remove((output + ".bak").c_str());
	}
	
	cout << "done! (in " << exportTime.get() << "ms)\n\n";
	
	
	//  Free
	delete agent;
	delete experiment;
}


void mdpDistribGen(int argc, char* argv[]) throw (MDPException,
                                                  parsing::ParsingException)
{
     //   Create a Dirichlet MDP distribution
          //   The name to give to the distribution
     string name      = parsing::getValue(argc, argv, "--name");
     string shortName = parsing::getValue(argc, argv, "--short_name");


          //   The number of states/actions
     unsigned int nX = atoi(
               parsing::getValue(argc, argv, "--n_states").c_str());
     unsigned int nU = atoi(
               parsing::getValue(argc, argv, "--n_actions").c_str());


          //   The initial state
     int iniState = atoi(parsing::getValue(argc, argv, "--ini_state").c_str());


          //   The vector representing the counters of the Dirichlet
          //   distribution, where 'theta[nU*nX*x + nX*u + y]' represents
          //   the counter associated to the transition '(x, u, y)'.
     vector<double> theta;


          //   The vectors representing the reward function, where
          //   'R[nU*nX*x + nX*u + y]' and 'V[nU*nX*x + nX*u + y]' are the
          //   mean and the variance of the rewards associated to the
          //   transition '(x, u, y)' respectively.
          //
          //   If the rewards are constant, you do not have to fill the
          //   'V' vector (but you have to provide an empty vector).
     vector<double> R, V;


          //   Fill 'theta'
     vector<string> weights
               = parsing::getValues(argc, argv, "--transition_weights", nX*nU*nX);
     for (unsigned int i = 0; i < weights.size(); ++i)
          theta.push_back(atof(weights[i].c_str()));


     //   The type of rewards
     RewardType rType = MDP::getRewardType(
               parsing::getValue(argc, argv, "--reward_type"));


     //   Fill 'R' and 'V'
     vector<string> rewardsMeans
               = parsing::getValues(argc, argv, "--reward_means", nX*nU*nX);
     for (unsigned int i = 0; i < rewardsMeans.size(); ++i)
          R.push_back(atof(rewardsMeans[i].c_str()));

     vector<string> rewardsVariances;
     try
     {
          rewardsVariances = parsing::getValues(
                    argc, argv, "--reward_variances", nX*nU*nX);

          for (unsigned int i = 0; i < rewardsVariances.size(); ++i)
               V.push_back(atof(rewardsVariances[i].c_str()));
     }
     catch (parsing::ParsingException&) {}


     //   Instantiation
     DirMultiDistribution distrib(
               name, shortName, nX, nU, iniState, theta, rType, R, V);


     //   Export
     string output = parsing::getValue(argc, argv, "--output");
     ofstream os(output.c_str());
     distrib.serialize(os);
     os.close();
}


void formulaSetGen(int argc, char* argv[]) throw (parsing::ParsingException)
{
     //   1.   Get 'nVar', 'nTokens', 'tokenListStr' and 'maxSize'
     unsigned int nVar = atoi(
               (parsing::getValue(argc, argv, "--n_variables")).c_str());

     unsigned int nbTokens = atoi(
               (parsing::getValue(argc, argv, "--tokens")).c_str());

     vector<string> tokenListStr =
               parsing::getValues(argc, argv, "--tokens", (nbTokens + 1));
     tokenListStr.erase(tokenListStr.begin());          

     unsigned int maxSize = atoi(
               (parsing::getValue(argc, argv, "--max_size")).c_str());


     //   2.   Get 'nPoints', 'minV', 'maxV' and 'reduce'
     bool toReduce = parsing::hasFlag(argc, argv, "--reduce");
     
     unsigned int nPoints;
     vector<string> range;
     double minV, maxV;
     
     if (toReduce)
     {
          nPoints = atoi((parsing::getValue(argc, argv, "--n_points")).c_str());
          range = parsing::getValues(argc, argv, "--points_range", 2);
          minV = atof(range[0].c_str());
          maxV = atof(range[1].c_str());
     }


     //   3.   Get 'compressOutput' and 'output'
     bool compressOutput = parsing::hasFlag(argc, argv, "--compress_output");
     string output = parsing::getValue(argc, argv, "--output");
          
     if (fileExists(output + (compressOutput ? ".zz" : "")))
     {
          cout << "\n\tThe file \"";
          cout << (output + (compressOutput ? ".zz" : ""));
          cout << "\" already exists!\n\n";
          return;
     }

     
     //   4.   Build the set of tokens
     set<Token*, Token::pComp> tokenSet;
     try
     {          
          //   Variables
          for (unsigned int i = 0; i < nVar; ++i)
               tokenSet.insert(new Variable(i));
               
          //   Other tokens
          for (unsigned int i = 0; i < tokenListStr.size(); ++i)
               tokenSet.insert(Token::getToken(tokenListStr[i]));
     }
     catch (TokenException& e) { throw parsing::ParsingException(e.what()); }
     
     
     //   5.   Produce the set of points
     vector<vector<double> >pointList;
     for (unsigned int i = 0; i < nPoints; ++i)
     {
          pointList.push_back(vector<double>());
          for (unsigned int j = 0; j < nVar; ++j)
          {
               pointList.back().push_back(
                         RandomGen::randRange_Uniform(minV, maxV));
          }
     }
     
     
     //   6.   Run
     Chrono time;
	cout << "\n";
	cout << "\tGenerate the set of formulas..." << flush;
     FormulaVector formulaList = Formula::generate(tokenSet, maxSize);
     cout << "done! (in " << time.get() << "ms)\n";

     if (toReduce)
     {
          time.restart();
	     cout << "\tReduce the set of formulas..." << flush;
          formulaList = Formula::reduce(formulaList, pointList);
          cout << "done! (in " << time.get() << "ms)\n";
     }


     //   7.   Export     
     Chrono exportTime;
	cout << "\tExport the results..." << flush;
	
	if (compressOutput)
	{	
		ofstream os((output + ".zz").c_str());
		formulaList.zSerialize(os, Z_BEST_COMPRESSION);
		os.close();
	}
	
	else
	{
		ofstream os(output.c_str());
		formulaList.serialize(os);
		os.close();
	}
	
	cout << "done! (in " << exportTime.get() << "ms)\n\n";


     //   8.   Free
     set<Token*, Token::pComp>::iterator it  = tokenSet.begin();
     set<Token*, Token::pComp>::iterator end = tokenSet.end();
     for (; it != end; ++it) { if (*it) { delete (*it); } }
     
     for (unsigned int i = 0; i < formulaList.size(); ++i)
          delete formulaList[i];
}


void slSamplesGen(int argc, char* argv[]) throw (bad_cast,
                                                 parsing::ParsingException)
{
     //   1.   Get 'agent' (if specified)
     Agent* agent = 0;
     try { agent = Agent::parse(argc, argv); }
     catch (parsing::ParsingException e) {}


     //   2.   Get 'dirDistrib'
     MDPDistribution* mdpDistrib = MDPDistribution::parse(argc, argv);
     assert(mdpDistrib);
     
     const DirMultiDistribution* dirDistrib = 
				dynamic_cast<const DirMultiDistribution*>(mdpDistrib);
     
     
     //   3.   Get 'nbOfMDPs'
     string tmp = parsing::getValue(argc, argv, "--n_mdps");
     unsigned int nbOfMDPs = atoi(tmp.c_str());
     
     
     //   4.   Get 'simGamma'
     tmp = parsing::getValue(argc, argv, "--discount_factor");
     double simGamma = atof(tmp.c_str());
     
     
     //   5.   Get 'T'
     tmp = parsing::getValue(argc, argv, "--horizon_limit");
     unsigned int T = atoi(tmp.c_str());
     
     
     //   6.   Generate the SL samples
     Chrono time;
	cout << "\tGenerate the SL samples..." << flush;
	
	vector<vector<double> > inputs, outputs;
	vector<double> weights;
	vector<unsigned int> indexes;
     SLAgent::generateSLSamples(agent, dirDistrib, nbOfMDPs, simGamma, T,
                                inputs, outputs, weights, indexes);
	
	cout << "done! (in " << time.get() << "ms)\n\n";
     
     
     //   7.   Get 'output'
     string output = parsing::getValue(argc, argv, "--output");
     
     
     //   8.   Save the ML samples into a file
	Chrono exportTime;
	cout << "\tExport the results..." << flush;
	
	ofstream os(output.c_str());
	SLAgent::saveSLSamples(inputs, outputs, weights, indexes, os);
	os.close();
	
	cout << "done! (in " << exportTime.get() << "ms)\n\n";

     
     //   9.   Free
     delete mdpDistrib;
     if (agent) { delete agent; }
}


void newAgentFactory(int argc, char* argv[]) throw (SerializableException,
                                                    parsing::ParsingException)
{
     //   1.   Get 'agentFactory'
     Chrono time;
     cout << "\n";
	cout << "\tCreate the AgentFactory..." << flush;
	
     AgentFactory* agentFactory =
               AgentFactory::parse(argc, argv, false);
     assert(agentFactory);
     
     cout << "done! (in " << time.get() << "ms)\n\n";
     
     
     //   2.   Get 'output'
     string output = parsing::getValue(argc, argv, "--output");


     //   3.   Serialize 'agentFactory'
     Chrono exportTime;
	cout << "\tExport the results..." << flush;
	
     ofstream os(output.c_str());
     agentFactory->serialize(os);
     os.close();
     
     cout << "done! (in " << exportTime.get() << "ms)\n\n";
}


void help()
{
     ifstream is("doc/command-line manual (BBRL-DDS).txt");
     for (string tmp; getline(is, tmp);) { cout << tmp << "\n"; }
}
