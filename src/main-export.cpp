
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
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

	\date	2015-02-23
*/
// ===========================================================================
// ---------------------------------------------------------------------------
//   Extra structures
// ---------------------------------------------------------------------------
class AgentData
{
     public:
          AgentData() : dsrList(vector<double>()) {}
     
          string className, name, expName;
          double offlineTime;
          double onlineTime;
          vector<double> dsrList;
          double mean;
          double gap;
};


// ---------------------------------------------------------------------------
//	Extra functions
// ---------------------------------------------------------------------------
void help();
void latex(int argc, char* argv[]) throw (parsing::ParsingException);
void wdlLatex(int argc, char* argv[]) throw (parsing::ParsingException);
void matlab(int argc, char* argv[]) throw (parsing::ParsingException);
void gnuplot(int argc, char* argv[]) throw (parsing::ParsingException);

vector<AgentData> getAgentDataList(int argc, char* argv[]);
void writeLatexTable(ostream& os,
                     string expName, vector<AgentData>& agentDataList);
void writeWDLLatexTable(ostream& os,
                        vector<AgentData>& agentDataList);
void writeMatlabFunction(ostream& os, string functionName,
                         vector<AgentData>& agentDataList);
void writeGnuplotScript(ostream& osDat, ostream& osGP,
                        vector<AgentData>& agentDataList);

void splitTime(double t,
               unsigned int& days, unsigned int& hours, unsigned int& minutes,
               unsigned int& seconds, unsigned int& milliseconds);


// ---------------------------------------------------------------------------
//	Main function
// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{	
	//	'DDS' package initilization
	init();

     
     //   Parsing and execution
	    //   1.   Get the right mode
    bool modeIsHelp     = parsing::hasFlag(argc, argv, "--help");
    bool modeIsLatex    = parsing::hasFlag(argc, argv, "--latex");
    bool modeIsWDLLatex = parsing::hasFlag(argc, argv, "--wdl-latex");
    bool modeIsMatlab   = parsing::hasFlag(argc, argv, "--matlab");
    bool modeIsGnuplot  = parsing::hasFlag(argc, argv, "--gnuplot");
	    
	    
	    //   2.   Launch the selected mode
     try
	{
	    if      (modeIsHelp)     { help();               }
	    else if (modeIsLatex)    { latex(argc, argv);    }
	    else if (modeIsWDLLatex) { wdlLatex(argc, argv); }
	    else if (modeIsMatlab)   { matlab(argc, argv);   }
	    else if (modeIsGnuplot)  { gnuplot(argc, argv);  }
	    else                     { cout << "No mode selected!\n"; }
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
     //   1.   Retrieve the 'AgentData's
     vector<AgentData> agentDataList = getAgentDataList(argc, argv);
     
     
     //   2.   Get 'output'
     string output = parsing::getValue(argc, argv, "--output");
     assert(output != "");
     
     
     //   3.   Run
	ofstream os(output.c_str());
	while (!agentDataList.empty())
	{
		string expName = agentDataList[0].expName;
		vector<AgentData> tAgentDataList;
		
		for (int i = 0; i < (int) agentDataList.size(); ++i)
		{
			if (agentDataList[i].expName == expName)
			{
				tAgentDataList.push_back(agentDataList[i]);
				
				for (int j = (i + 1); j < (int) agentDataList.size(); ++j)
					agentDataList[j - 1] = agentDataList[j];
				
				agentDataList.pop_back();				
				--i;
			}
		}
		
		writeLatexTable(os, expName, tAgentDataList);
	}
	os.close();
}


void wdlLatex(int argc, char* argv[]) throw (parsing::ParsingException)
{
     //   1.   Retrieve the 'AgentData's
     vector<AgentData> agentDataList = getAgentDataList(argc, argv);
     
     
     //   2.   Get 'output'
     string output = parsing::getValue(argc, argv, "--output");
     assert(output != "");
     
     
     //   3.   Run
	ofstream os(output.c_str());
	writeWDLLatexTable(os, agentDataList);
	os.close();
}


void matlab(int argc, char* argv[]) throw (parsing::ParsingException)
{
     //   1.   Retrieve the 'AgentData's
     vector<AgentData> agentDataList = getAgentDataList(argc, argv);
     
     
     //   2.   Get 'output'
     string output = parsing::getValue(argc, argv, "--output");
     
     
     //   3.   Run
     string tmp = output.substr(output.find_last_of('/') + 1);
	string functionName = tmp.substr(0, tmp.find_last_of('.'));	
	
	ofstream os(output.c_str());
     writeMatlabFunction(os, functionName, agentDataList);
	os.close();
}


void gnuplot(int argc, char* argv[]) throw (parsing::ParsingException)
{
     //   1.   Retrieve the 'AgentData's
     vector<AgentData> agentDataList = getAgentDataList(argc, argv);
     
     
     //   2.   Get 'output'
     string outputDat = parsing::getValue(argc, argv, "--output-dat");
     string outputGP  = parsing::getValue(argc, argv, "--output-gp");
     
     
     //   3.   Run
	ofstream osDat(outputDat.c_str());
	ofstream osGP(outputGP.c_str());
	
     writeGnuplotScript(osDat, osGP, agentDataList);
     
	osDat.close();
	osGP.close();
}


vector<AgentData> getAgentDataList(int argc, char* argv[])
{
     vector<AgentData> agentDataList;

     //   1.   Parse the Agents
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
          
          Agent* agent = Agent::parse(argcBis, argvBis);
          delete[] argvBis;
          
          AgentData agentData;
          agentData.className = agent->getExportClassName();
          agentData.name = agent->getExportName();
          agentData.offlineTime = agent->getOfflineTime();
          agentDataList.push_back(agentData);
          
          delete agent;
     }
     
     
     //   2.   Parse the Experiments
     int iExp = 0, nExp = 0;
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

          Experiment* exp = Experiment::parse(argcBis, argvBis);         
          delete[] argvBis;
          
          agentDataList[nExp].expName = (exp->getName());
          agentDataList[nExp].onlineTime = (exp->getTimeElapsed()
                    / (double) exp->getNbOfMDPs());
          
          agentDataList[nExp].dsrList = exp->computeDSRList();
		pair<double, double> CI95 =
		        statistics::computeCI95<double>(agentDataList[nExp].dsrList);
                    
          agentDataList[nExp].mean = ((CI95.first + CI95.second) / 2.0);;
          agentDataList[nExp].gap = (CI95.second - agentDataList[nExp].mean);
          ++nExp;
          
          delete exp;
     }
     
     
     //   3.   Return
     return agentDataList;
}


void writeLatexTable(ostream& os,
                     string expName, vector<AgentData>& agentDataList)
{
     os << "\n";
     os << "\\begin{table}[!h]\n";
     os << "\t\\tiny\n";
	os << "\t\\centering\n";
	os << "\t\\begin{tabular}{l|c|c|c}\n";
	os << "\t\tAgent & Offline time & Online time & Mean score\\\\\n";
	os << "\t\t\\hline\n";
	
	for (unsigned int i = 0; i < agentDataList.size(); ++i)
	{
          AgentData& agentData = agentDataList[i];
          
          //   Write 'name'
		os << "\t\t" << agentData.name;

          //   Write 'offline time'
          unsigned int days, hours, minutes, seconds, milliseconds;
          splitTime(agentData.offlineTime, days, hours, minutes,
                    seconds, milliseconds);

          os << " & $\\sim";
          if      (days    > 0) { os << (days + 1)         << "$d ";  }
          else if (hours   > 0) { os << (hours + 1)        << "$h ";  }
          else if (minutes > 0) { os << (minutes + 1)      << "$m ";  }
          else if (seconds > 0) { os << (seconds + 1)      << "$s ";  }
          else                  { os << (milliseconds + 1) << "$ms "; }
		
		
          //   Write 'online time'
          splitTime(agentData.onlineTime, days, hours, minutes,
                    seconds, milliseconds);

		os << " & $\\sim";
          if      (days    > 0) { os << (days + 1)         << "$d ";  }
          else if (hours   > 0) { os << (hours + 1)        << "$h ";  }
          else if (minutes > 0) { os << (minutes + 1)      << "$m ";  }
          else if (seconds > 0) { os << (seconds + 1)      << "$s ";  }
          else                  { os << (milliseconds + 1) << "$ms "; }
		
		
          //   Write 'score'
		os << " & $";
		os << setprecision(ceil(log10(agentData.mean) + 2));
		os << agentData.mean;
		os << " \\pm ";
		os << setprecision(ceil(log10(agentData.gap) + 2));
		os << agentData.gap << "$";
		
		os << "\\\\\n";
	}
	os << "\t\\end{tabular}\n";
	os << "\t\\caption{" << expName << "}\n";
	os << "\\end{table}\n\n";
}


void writeWDLLatexTable(ostream& os, vector<AgentData>& agentDataList)
{
     //   Build the list of agent class names
     vector<string> agentClassStrList;
     for (unsigned int i = 0; i < agentDataList.size(); ++i)
     {
          bool found = false;
          for (unsigned int j = 0; j < agentClassStrList.size(); ++j)
          {
               if (agentDataList[i].className == agentClassStrList[j])
               {
                    found = true;
                    break;
               }
          }
          
          if (!found)
               agentClassStrList.push_back(agentDataList[i].className);
     }


     //   Build the list of experiment names
     vector<string> expStrList;
     for (unsigned int i = 0; i < agentDataList.size(); ++i)
     {
          bool found = false;
          for (unsigned int j = 0; j < expStrList.size(); ++j)
          {
               if (agentDataList[i].expName == expStrList[j])
               {
                    found = true;
                    break;
               }
          }
          
          if (!found)
               expStrList.push_back(agentDataList[i].expName);
     }
     
     
     
     os << "\n";
     os << "\\begin{table}[!h]\n";
     os << "\t\\tiny\n";
	os << "\t\\centering\n";
	os << "\t\\begin{tabular}{c";
	for (unsigned int i = 0; i < agentClassStrList.size(); ++i) { os << "|c"; }
	os << "}\n";
	os << "\t\t";
	
	vector<string>::iterator itI, endI;
	itI  = agentClassStrList.begin();
     endI = agentClassStrList.end();
	for (; itI != endI; ++itI)
	{
          os << " & ";
          os << *itI;
	}
	os << "\\\\\n";
	os << "\t\t\\hline\n";
     
     
     itI  = agentClassStrList.begin();
     endI = agentClassStrList.end();
     for (; itI != endI; ++itI)
     {
          os << "\t\t" << *itI;

          vector<string>::iterator itJ;
          vector<string>::iterator endJ;
          itJ  = agentClassStrList.begin();
          endJ = agentClassStrList.end();
          for (; itJ != endJ; ++itJ)
          {
               os << "& ";
               if (*itI == *itJ) { os << "-/-/-"; continue; }
               
               unsigned int winCount = 0, lossCount = 0, drawCount = 0;
               vector<string>::iterator itK, endK;
               itK  = expStrList.begin();
               endK = expStrList.end();
               for (; itK != endK; ++itK)
               {
                    double maxMean;
                    
                    //   Retrieve the experiment file of agent of class 'i'
                    //   which obtained the best average score on this
                    //   experiment
                    int m = -1;
                    maxMean= 0.0;
                    for (unsigned int i = 0; i < agentDataList.size(); ++i)
                    {                         
                         if ((agentDataList[i].className == *itI)
                                   && (agentDataList[i].expName == *itK))
                         {
                              if ((m == -1) || (maxMean < agentDataList[i].mean))
                              {
                                   m = i;
                                   maxMean = agentDataList[i].mean;
                              }
                         }
                    }
                    assert((m >= 0) && (m < (int) agentDataList.size()));


                    //   Retrieve the experiment file of agent of class 'j'
                    //   which obtained the best average score on this
                    //   experiment
                    int n = -1;
                    maxMean = 0.0;
                    for (unsigned int i = 0; i < agentDataList.size(); ++i)
                    {
                         if ((agentDataList[i].className == *itJ)
                                   && (agentDataList[i].expName == *itK))
                         {
                              if ((n == -1) || (maxMean < agentDataList[i].mean))
                              {
                                   n = i;
                                   maxMean = agentDataList[i].mean;
                              }
                         }
                    }
                    assert((n >= 0) && (n < (int) agentDataList.size()));
                    
                    
                    //   Retrieve the two series for statistical comparison
                    vector<double>& x = agentDataList[m].dsrList;
                    vector<double>& y = agentDataList[n].dsrList;
                    
                    
                    //   Compute the statistical value 'Z' ('paired Z-test')
                    double z = utils::statistics::computePairedZ<double>(x, y);
                    
                    
                    //   Determine if agent 'i' wins/loses (or if it is a draw)
                    if (z < utils::statistics::getBilateralPairedZThreshold95())
                         ++drawCount;

                    else if (agentDataList[m].mean > agentDataList[n].mean)
                         ++winCount;

                    else if (agentDataList[m].mean < agentDataList[n].mean)
                         ++lossCount;

                    else { ++drawCount; }
               }
               
               if (winCount > lossCount) { os << "\textbf{"; }
               os << winCount << "/" << drawCount << "/" << lossCount;
               if (winCount > lossCount) { os << "}"; }
          }
          os << "\\\\\n";
     }
     
     
     os << "\t\\end{tabular}\n";
	os << "\t\\caption{Win-Draw-Loss table}\n";
	os << "\\end{table}\n\n";
}


void writeMatlabFunction(ostream& os, string functionName,
                         vector<AgentData>& agentDataList)
{
     os << "\n";
	os << "function " << functionName << "()\n";
	os << "\tY =\t[\n\t\t";
	vector<string> agentNameList;
	vector<string> expNameList;
	while (!agentDataList.empty())
	{
		string expName = agentDataList[0].expName;
		expNameList.push_back(expName);
		
		vector<AgentData> tAgentDataList;
		for (unsigned int i = 0; i < agentDataList.size(); ++i)
		{
			if (agentDataList[i].expName == expName)
			{
				tAgentDataList.push_back(agentDataList[i]);
				
				for (int j = (i + 1); j < (int) agentDataList.size(); ++j)
					agentDataList[j - 1] = agentDataList[j];
				
				agentDataList.pop_back();				
				--i;
			}
		}

		os << "\t";
		for (unsigned int i = 0; i < tAgentDataList.size(); ++i)
		{
			if (expNameList.size() == 1)
				agentNameList.push_back(tAgentDataList[i].name);

			os << " " << tAgentDataList[i].mean;
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
}


void writeGnuplotScript(ostream& osDat, ostream& osGP,
                         vector<AgentData>& agentDataList)
{
     //   Build the list of agent names
     vector<string> agentStrList;
     for (unsigned int i = 0; i < agentDataList.size(); ++i)
     {
          bool found = false;
          for (unsigned int j = 0; j < agentStrList.size(); ++j)
          {
               if (agentDataList[i].name == agentStrList[j])
               {
                    found = true;
                    break;
               }
          }
          
          if (!found)
               agentStrList.push_back(agentDataList[i].name);
     }


     //   Build the list of experiment names
     vector<string> expStrList;
     for (unsigned int i = 0; i < agentDataList.size(); ++i)
     {
          bool found = false;
          for (unsigned int j = 0; j < expStrList.size(); ++j)
          {
               if (agentDataList[i].expName == expStrList[j])
               {
                    found = true;
                    break;
               }
          }
          
          if (!found)
               expStrList.push_back(agentDataList[i].expName);
     }
	
	
	//  Data file
	osDat << "Experiments";
	vector<string>::iterator itA, endA;
	itA  = agentStrList.begin();
	endA = agentStrList.end();
	for (; itA != endA; ++itA)
	{
	    osDat << "\t\"" << *itA << "\"";
	    osDat << "\t\"" << *itA << "\"";
	    osDat << "\t\"" << *itA << "\"";
     }
	osDat << "\n";
	
	vector<string>::iterator itE, endE;
	itE  = expStrList.begin();
	endE = expStrList.end();
	for (; itE != endE; ++itE)
	{
	    osDat << "\"" << *itE << "\"";
	    
	    itA  = agentStrList.begin();
	    endA = agentStrList.end();
	    for (; itA != endA; ++itA)
	         for (unsigned int i = 0; i < agentDataList.size(); ++i)
	         {
	              string name = agentDataList[i].name;
	              string expName = agentDataList[i].expName;
	              
	              if ((name == *itA) && (expName == *itE))
	              {
	                   osDat << "\t" << agentDataList[i].mean;
	                   
	                   //
	                   double diff = (agentDataList[i].gap / 2.0);
	                   double minV, maxV;
	                   minV = (agentDataList[i].mean - diff);
	                   maxV = (agentDataList[i].mean + diff);
	                   
	                   osDat << "\t" << minV << "\t" << maxV;
	                   //
	                   break;
	              }
	         }
	    osDat << "\n";
	}
	
	
	//  GP file
	osGP << "set terminal postscript eps ";
	osGP << "enhanced solid color font 'Helvetica,10'\n";
     osGP << "set output output\n";
     osGP << "set yrange [0:*]\n";
     
     if (expStrList.size() > 1) { osGP << "set xlabel \"Experiments\"\n"; }
     else                       { osGP << "set xlabel \"\"\n";            }
     
     osGP << "set ylabel \"Score\"\n";
     osGP << "\n";
     osGP << "set xtics border nomirror\n";
     osGP << "set ytics border nomirror\n";
     osGP << "\n";
     osGP << "set palette defined (0 0.9 0.9 1, 35 0.3 0.3 1, 50 0.6 ";
     osGP << "0.15 0.4, 70 'red', 100 'yellow')\n";
     osGP << "unset colorbox\n";
     osGP << "\n";
     
     osGP << "set style fill solid 0.3\n";
     osGP << "set bars front\n";
     osGP << "set style data histogram \n";
     osGP << "set style histogram errorbars gap 1\n";
     osGP << "\n";
     osGP << "set style fill solid 1.25 border -1\n";
     osGP << "set offset -0.4,0.4,0,0\n";
     osGP << "set boxwidth 0.9 relative\n";
     osGP << "set key inside\n";
     osGP << "set key font \",6\"\n";
     osGP << "\n";
     osGP << "\n";
     osGP << "plot ";
     for (unsigned int i = 1; i <= agentStrList.size(); ++i)
     {
          if (i != 1) { osGP << ", \\\n\t"; }
          osGP << "input u " << (3*i)-1 << ":" << (3*i) << ":" << (3*i)+1;
          osGP << ":xtic(1) ";
          
          
          osGP << "title columnheader fs solid ";
          osGP << "lt palette frac ";
          osGP << ((i - 1) / (double) (agentStrList.size() - 1));
     }
     osGP << "\n";
}


void splitTime(double t,
               unsigned int& days, unsigned int& hours, unsigned int& minutes,
               unsigned int& seconds, unsigned int& milliseconds)
{   
     days = floor(t / (24*60*60*1000));
     t -= (days * (24*60*60*1000));
     
     hours = floor(t / (60*60*1000));
     t -= (hours * (60*60*1000));
     
     minutes = floor(t / (60*1000));
     t -= (minutes * (60*1000));
     
     seconds = floor(t / 1000);
     t -= (seconds * 1000);
     
     milliseconds = round(t);
}
