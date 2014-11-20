
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
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

	\date	2014-07-28
*/
// ===========================================================================
// ---------------------------------------------------------------------------
//	Extra functions header (see below for documentation)
// ---------------------------------------------------------------------------
string parsingErr(string);
int getMode(string);
void help();

Agent* getAgent(vector<string>);
Experiment* getExperiment(vector<string>);

void latex(vector<Agent*>&, vector<Experiment*>&, string);
void matlab(vector<Agent*>&, vector<Experiment*>&, string);


// ---------------------------------------------------------------------------
//	Main function
// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{	
	//	'DDS' package initilization
	init();


	//	Retrieve the request mode
	//		0:	Display help
	//		1:	Create a Latex table
	//		2:	Create a Matlab script for a bargraph
	if (argc < 2)
	{
		string msg;
		msg += "No argument provided!";
		cout << parsingErr(msg) << "\n";
		
		return -1;
	}
	
	int mode = getMode(argv[1]);
	int i = 2;	//	Index of the current command-line parameter
	vector<Agent*> agentList;
	vector<Experiment*> expList;
	
	
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
		//	'latex_table' case
		// ------------------------------------------------------------------
		case 1:
		{
			// -------------------------------------------------------------
			//	Get 'agentList' and 'expList'
			// -------------------------------------------------------------
			while ((i < argc)
					&& (string(argv[i]) != "--output")
					&& (string(argv[i]) != "-o"))
			{
				// --------------------------------------------------------
				//	Get 'agent'
				// --------------------------------------------------------
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
						&& (string(argv[i]) != "--experiment")
						&& (string(argv[i]) != "-e"))
				{
					agentArg.push_back(argv[i++]);
				}
				
					//	Instanciation
				Agent* agent = getAgent(agentArg);
				if (!agent)
				{
					string msg;
					msg += "Unable to initialize the ";
					
					stringstream sstr;
					sstr << (agentList.size() + 1);
					
					msg += sstr.str();
					msg += "th 'Agent' object!!";
					cout << parsingErr(msg) << "\n";
					
					return -1;
				}
				
					//	Add 'agent' to 'agentList'
				agentList.push_back(agent);
				
				
				// --------------------------------------------------------
				//	Get 'exp'
				// --------------------------------------------------------
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
						&& (string(argv[i]) != "-a")
						&& (string(argv[i]) != "--output")
						&& (string(argv[i]) != "-o"))
				{
					expArg.push_back(argv[i++]);
				}
				
					//	Instanciation
				Experiment* exp = getExperiment(expArg);
				if (!exp)
				{
					string msg;
					msg += "Unable to initialize the ";
					
					stringstream sstr;
					sstr << (expList.size() + 1);
					
					msg += sstr.str();
					msg += "th 'Experiment' object!!";
					cout << parsingErr(msg) << "\n";
					
					return -1;
				}
				
					//	Add 'exp' to 'expList'
				expList.push_back(exp);
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
			latex(agentList, expList, outputFile);
			
			
			break;
		}
		
		
		// ------------------------------------------------------------------
		//	'matlab_bargraph_script' case
		// ------------------------------------------------------------------
		case 2:
		{
			// -------------------------------------------------------------
			//	Get 'agentList' and 'expList'
			// -------------------------------------------------------------
			while ((i < argc)
					&& (string(argv[i]) != "--output")
					&& (string(argv[i]) != "-o"))
			{
				// --------------------------------------------------------
				//	Get 'agent'
				// --------------------------------------------------------
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
						&& (string(argv[i]) != "--experiment")
						&& (string(argv[i]) != "-e"))
				{
					agentArg.push_back(argv[i++]);
				}
				
					//	Instanciation
				Agent* agent = getAgent(agentArg);
				if (!agent)
				{
					string msg;
					msg += "Unable to initialize the ";
					
					stringstream sstr;
					sstr << (agentList.size() + 1);
					
					msg += sstr.str();
					msg += "th 'Agent' object!!";
					cout << parsingErr(msg) << "\n";
					
					return -1;
				}
				
					//	Add 'agent' to 'agentList'
				agentList.push_back(agent);
				
				
				// --------------------------------------------------------
				//	Get 'exp'
				// --------------------------------------------------------
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
						&& (string(argv[i]) != "-a")
						&& (string(argv[i]) != "--output")
						&& (string(argv[i]) != "-o"))
				{
					expArg.push_back(argv[i++]);
				}
				
					//	Instanciation
				Experiment* exp = getExperiment(expArg);
				if (!exp)
				{
					string msg;
					msg += "Unable to initialize the ";
					
					stringstream sstr;
					sstr << (expList.size() + 1);
					
					msg += sstr.str();
					msg += "th 'Experiment' object!!";
					cout << parsingErr(msg) << "\n";
					
					return -1;
				}
				
					//	Add 'exp' to 'expList'
				expList.push_back(exp);
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
			matlab(agentList, expList, outputFile);
			
			
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
	while (!agentList.empty())
	{
		delete agentList.back();
		agentList.pop_back();
	}

	while (!expList.empty())
	{
		delete expList.back();
		expList.pop_back();
	}
}

// ----------------------------------------------------------------------------
//	Extra functions implementation
// ----------------------------------------------------------------------------
string parsingErr(string msg)
{
	return ("\n\t[parsing error]: " + msg + \
			" (see --help for more details)\n");
}


int getMode(string modeStr)
{
	if (modeStr == "--help")
		return 0;

	if ((modeStr == "--latex") || (modeStr == "-l"))
		return 1;

	if ((modeStr == "--matlab") || (modeStr == "-m"))
		return 2;

	else { return -1; }
}


void help()
{
	ifstream is("doc/command-line manual (export).txt");
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


void latex(vector<Agent*>& agentList, vector<Experiment*>& expList,
		string outputFile)
{
	ofstream os(outputFile.c_str());
		
	while (!expList.empty())
	{
		string expName = expList[0]->getName();
		vector<Agent*> tAgentList;
		vector<Experiment*> tExpList;
		
		for (unsigned int i = 0; i < expList.size(); ++i)
		{
			if (expList[i]->getName() == expName)
			{
				tAgentList.push_back(agentList[i]);
				tExpList.push_back(expList[i]);
				
				for (unsigned int j = (i + 1); j < agentList.size(); ++j)
				{
					agentList[j - 1] = agentList[j];
					expList[j - 1] = expList[j];
				}
				
				agentList.pop_back();
				expList.pop_back();
				
				--i;
			}
		}
		
		os << "\\begin{table}\n";
		os << "\t\\centering\n";
		os << "\t\\begin{tabular}{l|c|c|c}\n";
		os << "\t\tAgent & Offline time & Online time & Mean score\\\\\n";
		os << "\t\t\\hline\n";
		
		for (unsigned int i = 0; i < tExpList.size(); ++i)
		{
			os << "\t\t";
			
			os << tAgentList[i]->getName();
			
			int offlineTime = round(tAgentList[i]->getOfflineTime());
			if (offlineTime < 1)
				os << " & - ";
			else
			{
				os << setprecision(ceil(log10(offlineTime)));
				os << " & $" << offlineTime << "$ ms";
			}
			
			double onlineTime = (tExpList[i]->getTimeElapsed()
					/ (double) tExpList[i]->getNbOfMDPs());
			os << setprecision(ceil(log10(onlineTime) + 2));
			os << " & $" << onlineTime << "$ ms";
			
			vector<double> dsrList = tExpList[i]->computeDSRList();
			pair<double, double> CI95
					= statistics::computeCI95<double>(dsrList);

			double mean = ((CI95.first + CI95.second) / 2.0);
			os << setprecision(ceil(log10(mean) + 2));
			os << " & $" << mean << " \\pm ";
			
			double gap = (CI95.second - mean);
			os << setprecision(ceil(log10(gap) + 2));
			os << gap << "$";
			
			os << "\\\\\n";
		}
		os << "\t\\end{tabular}\n";
		os << "\t\\caption{" << tExpList[0]->getName() << "}\n";
		os << "\\end{table}\n\n";
	}
	os.close();
}


void matlab(vector<Agent*>& agentList, vector<Experiment*>& expList,
		string outputFile)
{
     string tmp = outputFile.substr(outputFile.find_last_of('/') + 1);
	string functionName = tmp.substr(0, tmp.find_last_of('.'));	
	
	ofstream os(outputFile.c_str());
	
	os << "\n";
	os << "function " << functionName << "()\n";
	os << "\tY =\t[\n\t\t";
	vector<string> agentNameList;
	vector<string> expNameList;
	while (!expList.empty())
	{
		string expName = expList[0]->getName();
		expNameList.push_back(expName);
		
		vector<Agent*> tAgentList;
		vector<Experiment*> tExpList;
		
		for (unsigned int i = 0; i < expList.size(); ++i)
		{
			if (expList[i]->getName() == expName)
			{
				tAgentList.push_back(agentList[i]);
				tExpList.push_back(expList[i]);
				
				agentList[i] = agentList.back();
				expList[i] = expList.back();
				
				agentList.pop_back();
				expList.pop_back();
				
				--i;
			}
		}

		os << "\t";
		for (unsigned int i = 0; i < tExpList.size(); ++i)
		{
			if (expNameList.size() == 1)
				agentNameList.push_back(tAgentList[i]->getName());
				
			else if ((i >= agentNameList.size())
					|| (agentNameList[i] != tAgentList[i]->getName()))
			{
				string msg;
				msg += "The agents used on the different experiments ";
				msg += "are not the same or are not provided in the same ";
				msg += "order!";
				
				parsingErr(msg);
			}
			
			vector<double> dsrList = tExpList[i]->computeDSRList();
			pair<double, double> CI95
					= statistics::computeCI95<double>(dsrList);

			double mean = ((CI95.first + CI95.second) / 2.0);

			os << " " << mean;
		}
		os << "\n\t\t";
	}

	//	1 group case
	if (expNameList.size() == 1)
	{
		os << "\t";
		for (int i = 0; i < (int) agentNameList.size(); ++i) { os << " 0"; }
		os << "\n\t\t";
	}
	
	os << "];\n\n";

	os << "\tLabels = {";
	for (unsigned int i = 0; i < (expNameList.size() - 1); ++i)
		os << "\'" << expNameList[i] << "\', ";
	os << "\'" << expNameList.back() << "\'";
	
	//	1 group case
	if (expNameList.size() == 1) { os << ", \' \'"; }
	
	os << "};\n\n";

	os << "\tbar(Y, 'group');\n";
	os << "\thold on;\n";
	os << "\tset(gca, 'XTick', 1:";
	
	//	1 group case
	if (expNameList.size() == 1)	{ os << 2;				}
	else						{ os << expNameList.size();	}
	
	os << ", 'XTickLabel', Labels);\n";
	
	//	1 group case
	if (expNameList.size() == 1) { os << "\txlim([0, 2]);\n"; }
	
	os << "\tylabel(\'Mean score\');\n";

	os << "\tlegend(";
	for (unsigned int i = 0; i < (agentNameList.size() - 1); ++i)
		os << "\'" << agentNameList[i] << "\', ";
	os << "\'" << agentNameList.back() << "\');\n";
	os << "\tlegend(\'Location\', \'NorthEastOutside\');\n";

	os.close();
}
