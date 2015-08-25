// in 'custom.cpp'
//   (mu == 1.0, sigma == 1.0)
//        'alpha = 1.0' & 'beta'  =   0.19 --> Ini distance ~= Update distance (first iteration only)
//        'alpha + beta' == 22.5  --> Score ~= Min distance (first iteration)


#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

#include "DDS/DDSLib.h"
#include "Utils/UtilsLib.h"
#include "Xport/XportLib.h"

using namespace std;
using namespace dds;
using namespace utils;
using namespace xport;


int main(int argc, char* argv[])
{
     bool hasSeed = parsing::hasFlag(argc, argv, "--seed");
     if (hasSeed)
     {
          int seed = atoi(parsing::getValue(argc, argv, "--seed").c_str());
          init((seed > 0) ? seed : 0);
     }
     else { init(); }


     /* YOUR CODE HERE */

     //   Loading
     ifstream is1("data/distributions/UGrid-distrib.dat");
     DirMultiDistribution* prior = new DirMultiDistribution(is1);
     is1.close();


     //   Variables generation
     unsigned int nX    = prior->getNbStates();
     unsigned int nU    = prior->getNbActions();
     unsigned int N     = atoi(parsing::getValue(argc, argv, "--N").c_str());
     double gamma       = atof(parsing::getValue(argc, argv, "--gamma").c_str());
     unsigned int T     = atoi(parsing::getValue(argc, argv, "--T").c_str());
     double mu          = atof(parsing::getValue(argc, argv, "--mu").c_str());
     double sigma       = atof(parsing::getValue(argc, argv, "--sigma").c_str());
     unsigned int b     = atoi(parsing::getValue(argc, argv, "--b").c_str());
     unsigned int np    = atoi(parsing::getValue(argc, argv, "--np").c_str());
     double acc         = atof(parsing::getValue(argc, argv, "--accuracy").c_str());
     double alpha       = atof(parsing::getValue(argc, argv, "--alpha").c_str());
     double beta        = atof(parsing::getValue(argc, argv, "--beta").c_str());

     vector<QCounterVar*> qcVarList;
     qcVarList.push_back(new QCounterVar(nX, nU, prior->getTheta()));
     for (unsigned int i = 0; i < 1; ++i)
     {
          QCounterVar* qcVar;
          qcVar = QCounterVar::generate(nX, nU, prior, N, gamma, T,
                                        mu, sigma, b, np, acc,
                                        alpha, beta, qcVarList);

          qcVarList.push_back(qcVar);
     }
     FVariableVector varList;
     for (unsigned int i = 0; i < qcVarList.size(); ++i)
          varList.push_back(qcVarList[i]);
     
     
     //   Evaluation of the variables
     vector<MDP*> mdps;
     for (unsigned int i = 0; i < 100; ++i) { mdps.push_back(prior->draw()); }
     
     vector<double> scores;
     for (unsigned int i = 0; i < qcVarList.size(); ++i)
     {
          //   Evaluate
               //   Agent
          vector<FVariable*> tmpVarList;
          tmpVarList.push_back(qcVarList[i]);
          Agent* agent = new FormulaAgent(
                    new formula::Formula("X0"), tmpVarList);
          agent->learnOffline(prior);
          
               //   Simulation
          vector<double> dsrList;
          for (unsigned int j = 0; j < mdps.size(); ++j)
          {
               simulation::SimulationRecord simRec
                         = simulation ::simulate(agent, mdps[j], gamma, T);
     
               dsrList.push_back(simRec.computeDSR());
          }
          
               //   Compute the score
          scores.push_back(statistics::computeMean<double>(dsrList));               
          

          //   Free data
          delete agent;
     }
     
     
     
     //   Output
     stringstream sstr;
     sstr << "data/variables/";
     
     sstr << "[" << "(" << qcVarList.size() << ")";
     for (unsigned int i = 0; i < qcVarList.size(); ++i)
     {
          sstr << scores[i];
          if (i < qcVarList.size() - 1) { sstr << ","; }
     }
     sstr << "]";
     
     sstr << "[" << b << "]" << "[" << np << "]";
     sstr << "[" << alpha << "]" << "[" << beta << "]";
     sstr << ".dat";
     
     ofstream os(sstr.str().c_str());
     varList.serialize(os);
     os.close();

     
     /* YOUR CODE HERE */


     //   Return
     return 0;
}
