
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "DDS/DDSLib.h"
#include "Utils/UtilsLib.h"

using namespace std;
using namespace dds;
using namespace dds::simulation;
using namespace utils;


int main(int, char* argv[])
{
     dds::init();
     
     
     /* YOUR CODE HERE */
          //   MDP
     ifstream is1("data/MDPs/double-loop.dat");
     MDP* mdp = new MDP(is1);
     is1.close();


          //   Agent
     Agent* agent = new BAMCPAgent(500, 89);

     
          //   Prior
     unsigned int nX = mdp->getNbStates();
     unsigned int nU = mdp->getNbActions();
     
     vector<double> theta;
     for (unsigned int i = 0; i < nX*nU*nX; ++i) { theta.push_back(1.0); }
     
     MDPDistribution* prior = new DirMultiDistribution(
               "uniform", "U", nX, nU, mdp->getIniState(), theta,
               mdp->getRType(), mdp->getR(), mdp->getV());

          
          //   Offline learning
     Chrono chrono;
     agent->learnOffline(prior);
     double offlineTime = chrono.get();

     
          //   Test
     unsigned int n = atoi(argv[1]);
     double gamma = 0.95;
     unsigned int T = 1000;
     
     chrono.restart();
     vector<SimulationRecord> simRecList;
     for (unsigned int i = 0; i < n; ++i)
          simRecList.push_back(simulate(agent, mdp, gamma, T));
      double onlineTime = chrono.get();
     
     vector<double> dsrList;
     for (unsigned int i = 0; i < n; ++i)
          dsrList.push_back(simRecList[i].computeDSR(1.0));
     pair<double, double> p = statistics::computeCI95<double>(dsrList);
     
     
          //   Output
     cout << "\n";
     cout << "\toffline time:\t" << offlineTime << "ms\n";
     cout << "\tonline time:\t" << onlineTime << "ms\n";
     cout << "\tprior:\t\t" << prior->getName() << "\n";
     cout << "\tagent:\t\t" << agent->getName() << "\n";
     cout << "\tmdp:\t\t" << mdp->getName() << "\n";
     cout << "\tscore:\t\t[" << p.first << "; " << p.second << "]\n";
     cout << "\n";
     
     delete mdp;
     delete agent;
     delete prior;
     
     //   Return
     return 0;
}
