
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
//	Extra functions header (see below for documentation)
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//	Main function
// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{	
	//	'DDS' package initilization
	init();
	
	
	try
	{
          //   Defining the command line options to be parsed
          tclap::CmdLine cmd(
                    "Offline Learning (TinyBRL library by Castronovo Michael)",
                    ' ', "1.0");
          
          
          //   Agent
          tclap::ValueArg<string> agentArg(
                     "a", "agent",
                     "The type of Agent to use.", false, "", "string");

          //   Agent file
          tclap::ValueArg<string> agentFileArg(
                     "f", "agent_file",
                     "The file containing the data defining the Agent to load.",
                     false, "", "string");

          //   Epsilon
          tclap::ValueArg<double> epsilonArg(
                     "e", "epsilon",
                     "The probability for the Agent to choose the next action "
                     "to perform randomly. (EGreedyAgent)",
                     false, -1.0, "double");          

          //   Tau
          tclap::ValueArg<double> tauArg(
                     "t", "tau",
                     "The temperature, controlling smoothly the transition "
                     "from a greedy behaviour (tau -> 0) to a random behaviour"
                     " (tau -> inf), where the action which is the best "
                     "according to the Agent is more likely to be drawn. "
                     "(SoftMaxAgent)",
                     false, -1.0, "double");       

          //   Sigma
          tclap::ValueArg<double> sigmaArg(
                     "s", "sigma", "The inverse sensitivity. (VDBEEGreedyAgent)",
                     false, -1.0, "double");      

          //   Delta
          tclap::ValueArg<double> deltaArg(
                     "", "delta",
                     "Determine the influence of the selected action on the "
                     "exploration rate. (VDBEEGreedyAgent)",
                     false, -1.0, "double");

          //   Initial epsilon
          tclap::ValueArg<double> iniEpsilonArg(
                     "", "ini_epsilon",
                     "The initial value of epsilon for each state-action pair."
                     " (VDBEEGreedyAgent)",
                     false, -1.0, "double");
          
          //   MDP distribution
          tclap::ValueArg<string> mdpDistribArg(
                     "", "mdp_distribution",
                     "The type of MDP distribution to use.",
                     false, "", "string");
          
          //   MDP distribution file
          tclap::ValueArg<string> mdpDistribFileArg(
                     "", "mdp_distribution_file",
                     "The file containing the data defining the MDP "
                     "distribution to load.",
                     false, "", "string");
          
          //   Parsing
          cmd.add(mdpDistribFileArg);
          cmd.add(mdpDistribArg);
          cmd.add(iniEpsilonArg);
          cmd.add(deltaArg);
          cmd.add(sigmaArg);
          cmd.add(tauArg);
          cmd.add(epsilonArg);
          cmd.add(agentFileArg);
          cmd.add(agentArg);
          
          
          
          cmd.parse(argc, argv);
}
	
	catch (utils::tclap::ArgException& e)
	{
	    cout << "error: " << e.error() << " for arg ";
	    cout << e.argId() << endl;
	}
}

// ----------------------------------------------------------------------------
//	Extra functions implementation
// ----------------------------------------------------------------------------
