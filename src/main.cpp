
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>

#include "DDS/DDSLib.h"
#include "Utils/UtilsLib.h"

using namespace std;
using namespace dds;
using namespace simulation;
using namespace utils;
using namespace utils::formula;


// ===========================================================================
/**	
	\brief	A simple command-line program for TinyBRL library
	
	\author	Castronovo Michael

	\date	2014-12-13
*/
// ===========================================================================
// ---------------------------------------------------------------------------
//   Extra functions
// ---------------------------------------------------------------------------
void help();
void formulaVGen(int argc, char* argv[])     throw (parsing::ParsingException);
void offlineLearning(int argc, char* argv[]) throw (parsing::ParsingException);
void newExperiment(int argc, char* argv[])   throw (parsing::ParsingException);
void runExperiment(int argc, char* argv[])   throw (parsing::ParsingException);


// ---------------------------------------------------------------------------
//	Main function
// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
     //   'DDS' package initilization
	init();
	
	
	//   Parsing and execution
	    //   1.   Get the right mode
	bool modeIsHelp =
	     parsing::hasFlag(argc, argv, "--help");

     bool modeIsFormulaVGen =
          parsing::hasFlag(argc, argv, "--formula_set_generation");

     bool modeIsOfflineLearning =
          parsing::hasFlag(argc, argv, "--offline_learning");

     bool modeIsNewExperiment =
          parsing::hasFlag(argc, argv, "--new_experiment");

     bool modeIsRunExperiment =
          parsing::hasFlag(argc, argv, "--run_experiment");
    
	    
	    //   2.   Launch the selected mode
     try
     {
          if (modeIsHelp)                { help();                        }
          else if (modeIsFormulaVGen)    { formulaVGen(argc, argv);       }
          else if (modeIsOfflineLearning){ offlineLearning(argc, argv);   }
          else if (modeIsNewExperiment)  { newExperiment(argc, argv);     }
          else if (modeIsRunExperiment)  { runExperiment(argc, argv);     }
          else            { cout << "No mode selected! (see '--help')\n"; }
     }
     
     catch (parsing::ParsingException e)
     {
          cout << "\nparsing exception: " << e.what();
          cout << "\n(see '--help')\n\n";
     }
}


// ---------------------------------------------------------------------------
//	Extra functions implementation
// ---------------------------------------------------------------------------
void help()
{
     ifstream is("doc/command-line manual (TinyBRL-DDS).txt");
     for (string tmp; getline(is, tmp);) { cout << tmp << "\n"; }
}


void formulaVGen(int argc, char* argv[]) throw (parsing::ParsingException)
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


void offlineLearning(int argc, char* argv[]) throw (parsing::ParsingException)
{
     //   1.   Get 'agent'
     Agent* agent = Agent::parse(argc, argv);
     assert(agent);


     //   2.   Get 'mdpDistrib'
     MDPDistribution* mdpDistrib = MDPDistribution::parse(argc, argv);
     assert(mdpDistrib);

     
     //   3.   Get 'compressOutput', 'output'     
     bool compressOutput = parsing::hasFlag(argc, argv, "--compress_output");
     string output = parsing::getValue(argc, argv, "--output");     
     assert(output != "");
     
     
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

     bool safeSimulations = parsing::hasFlag(argc, argv, "--safe_simulations");
     bool compressOutput = parsing::hasFlag(argc, argv, "--compress_output");
     
     string output = parsing::getValue(argc, argv, "--output");
     
     
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
		discountFactor, horizonLimit, safeSimulations));
		
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

void runExperiment(int argc, char* argv[]) throw (parsing::ParsingException)
{
     //   1.   Get 'experiment'
     Experiment* experiment = Experiment::parse(argc, argv);
     assert(experiment);
     
     
     //   2.   Get 'agent'
     Agent* agent = Agent::parse(argc, argv);
     assert(agent);
     
     
     //   3.   Get 'n_threads', 'compress_output', 'output',
     //        'refresh_frequency', 'backup_frequency'     
     string tmp = parsing::getValue(argc, argv, "--n_threads");
     unsigned int nThreads = atoi(tmp.c_str());
     
     bool compressOutput = parsing::hasFlag(argc, argv, "--compress_output");     
     string output = parsing::getValue(argc, argv, "--output");

     tmp = parsing::getValue(argc, argv, "--refresh_frequency");
     unsigned int refreshFrequency = (1000.0 * atoi(tmp.c_str()));
     
     tmp = parsing::getValue(argc, argv, "--backup_frequency");
     unsigned int backupFrequency = (1000.0 * atoi(tmp.c_str()));


     assert(nThreads > 0);
     assert(output != "");
     
     
     //   Special case for BAMCPAgent
     if ((agent->getClassName() == BAMCPAgent::toString())
               && (nThreads > 1))
     {
          cout << "\n\tCannot use a '" << BAMCPAgent::toString();
          cout << "' agent with more than one thread!\n";
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
}
