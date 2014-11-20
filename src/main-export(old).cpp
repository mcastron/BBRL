
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

	\date	2014-09-19
*/
// ===========================================================================
// ---------------------------------------------------------------------------
//	Extra functions
// ---------------------------------------------------------------------------
void help();
void latex(int argc, char* argv[]) throw (parsing::ParsingException);
void matlab(int argc, char* argv[]) throw (parsing::ParsingException);
void splitTime(double t,
               unsigned int& days, unsigned int& hours, unsigned int& minutes,
               unsigned int& seconds, double& milliseconds);

// ---------------------------------------------------------------------------
//	Main function
// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{	
	//	'DDS' package initilization
	init();

     
     //   Parsing and execution
	    //   1.   Get the right mode
    bool modeIsHelp   = parsing::hasFlag(argc, argv, "--help");
    bool modeIsLatex  = parsing::hasFlag(argc, argv, "--latex");
    bool modeIsMatlab = parsing::hasFlag(argc, argv, "--matlab");
	    
	    
	    //   2.   Launch the selected mode
     try
	{
	    if      (modeIsHelp)   { help();             }
	    else if (modeIsLatex)  { latex(argc, argv);  }
	    else if (modeIsMatlab) { matlab(argc, argv); }
	    else                           { cout << "No mode selected!\n"; }
	}
	
	catch (parsing::ParsingException e)
     {
          cout << "\nparsing exception: " << e.what();
          cout << "\n(see '--help')\n\n";
     }
}

// ----------------------------------------------------------------------------
//	Extra functions implementation
// ----------------------------------------------------------------------------
void help()
{
	ifstream is("doc/command-line manual (TinyBRL-export).txt");
	string line;
	cout << "\n";
	while (!is.eof()) { getline(is, line); cout << line << "\n"; }
}


void latex(int argc, char* argv[]) throw (parsing::ParsingException)
{
     //   1.   Parse the Agents
     vector<Agent*> agentList;
     int iAgent = 0;
     while (iAgent < (argc - 1))
     {
          while ((string(argv[iAgent]) != "--agent") && (iAgent < (argc - 1)))
               ++iAgent;
   
          int argcBis = 2;
          char** argvBis = new char*[argc];
          argvBis[0] = argv[0];
          argvBis[1] = argv[iAgent++];

          while ((string(argv[iAgent]) != "--agent") && (iAgent < (argc - 1)))
          {               
               argvBis[argcBis] = argv[iAgent];
               ++iAgent;
               ++argcBis;
          }     
          agentList.push_back(Agent::parse(argcBis, argvBis));
          
          delete[] argvBis;
     }
     
     
     
     //   2.   Parse the Experiments
     vector<Experiment*> expList;
     int iExp = 0;
     while (iExp < (argc - 1))
     {
          while ((string(argv[iExp]) != "--experiment") && (iExp < (argc - 1)))
               ++iExp;
          
          int argcBis = 2;
          char** argvBis = new char*[argc];
          argvBis[0] = argv[0];
          argvBis[1] = argv[iExp++];
          
          while ((string(argv[iExp]) != "--experiment") && (iExp < (argc - 1)))
               argvBis[argcBis++] = argv[iExp++];
          
          expList.push_back(Experiment::parse(argcBis, argvBis));          
          delete argvBis;
     }
     
     
     //   3.   Get 'output'
     string output = parsing::getValue(argc, argv, "--output");
     assert(output != "");
     
     
     //   4.   Run
	ofstream os(output.c_str());
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
                    unsigned int days, hours, minutes, seconds;
                    double milliseconds;
                    splitTime(offlineTime, days, hours, minutes,
                              seconds, milliseconds);
                    
                    os << " & ";
                    
                    if (days > 0) { os << "$" << days << "$d "; }
                    if ((days > 0) || (hours > 0))
                         os << "$" << hours << "$h ";
                    if ((days > 0) || (hours > 0) || (minutes > 0))
                         os << "$" << minutes << "$m ";
                    if ((days > 0) || (hours > 0) || (minutes > 0)
                              || (seconds > 0))
                    {          
                         os << "$" << seconds << "$s ";
                    }     
                    os << "$" << setprecision(ceil(log10(milliseconds) + 2));
                    os << milliseconds << "$ms ";
			}
			
			double onlineTime = (tExpList[i]->getTimeElapsed()
					/ (double) tExpList[i]->getNbOfMDPs());
               unsigned int days, hours, minutes, seconds;
               double milliseconds;
               splitTime(onlineTime, days, hours, minutes,
                         seconds, milliseconds);

			os << " & ";
               
               if (days > 0) { os << "$" << days << "$d "; }
               if ((days > 0) || (hours > 0)) { os << "$" << hours << "$h "; }
               if ((days > 0) || (hours > 0) || (minutes > 0))
                    os << "$" << minutes << "$m ";
               if ((days > 0) || (hours > 0) || (minutes > 0) || (seconds > 0))
                    os << "$" << seconds << "$s ";
               os << "$" << setprecision(ceil(log10(milliseconds) + 2));
               os << milliseconds << "$ms ";
			
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
	
	
	//  5.   Free data
	for (int i = 0; i < (int) agentList.size(); ++i) { delete agentList[i]; }
	for (int i = 0; i < (int) expList.size(); ++i)   { delete expList[i];   }
}


void matlab(int argc, char* argv[]) throw (parsing::ParsingException)
{
     //   1.   Parse the Agents
     vector<Agent*> agentList;
     int iAgent = 0;
     while (iAgent < (argc - 1))
     {
          while ((string(argv[iAgent]) != "--agent") && (iAgent < (argc - 1)))
               ++iAgent;
          
          int argcBis = 2;
          char** argvBis = new char*[argc];
          argvBis[0] = argv[0];
          argvBis[1] = argv[iAgent++];
          
          while ((string(argv[iAgent]) != "--agent") && (iAgent < (argc - 1)))
               argvBis[argcBis++] = argv[iAgent++];
          
          agentList.push_back(Agent::parse(argcBis, argvBis));          
          delete argvBis;
     }
     
     
     //   2.   Parse the Experiments
     vector<Experiment*> expList;
     int iExp = 0;
     while (iExp < (argc - 1))
     {
          while ((string(argv[iExp]) != "--experiment") && (iExp < (argc - 1)))
               ++iExp;
          
          int argcBis = 2;
          char** argvBis = new char*[argc];
          argvBis[0] = argv[0];
          argvBis[1] = argv[iExp++];
          
          while ((string(argv[iExp]) != "--experiment") && (iExp < (argc - 1)))
               argvBis[argcBis++] = argv[iExp++];
          
          expList.push_back(Experiment::parse(argcBis, argvBis));          
          delete[] argvBis;
     }
     
     
     //   3.   Get 'output'
     string output = parsing::getValue(argc, argv, "--output");
     
     
     //   4.   Run
     string tmp = output.substr(output.find_last_of('/') + 1);
	string functionName = tmp.substr(0, tmp.find_last_of('.'));	
	
	ofstream os(output.c_str());
	
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
	
	
	//  5.   Free data
	for (int i = 0; i < (int) agentList.size(); ++i) { delete agentList[i]; }
	for (int i = 0; i < (int) expList.size(); ++i)   { delete expList[i];   }
}

void splitTime(double t,
               unsigned int& days, unsigned int& hours, unsigned int& minutes,
               unsigned int& seconds, double& milliseconds)
{   
     days = floor(t / (24*60*60*1000));
     t -= (days * (24*60*60*1000));
     
     hours = floor(t / (60*60*1000));
     t -= (hours * (60*60*1000));
     
     minutes = floor(t / (60*1000));
     t -= (minutes * (60*1000));
     
     seconds = floor(t / 1000);
     t -= (seconds * 1000);
     
     milliseconds = t;
}
