
#include "SLAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor
// ===========================================================================
SLAgent::SLAgent() : agent(0) {}


SLAgent::SLAgent(string name,
                 Agent* agent_,
                 unsigned int nbOfMDPs_, double simGamma_, unsigned int T_,
                 std::string slModelFileName_) :
                         Agent(name),
                         agent(agent_),
                         nbOfMDPs(nbOfMDPs_), simGamma(simGamma_), T(T_),
                         slModelFileName(slModelFileName_)
{
     assert(agent);
     assert(nbOfMDPs > 0);
     assert(simGamma > 0.0 && simGamma < 1.0);
     assert(T > 0);
     assert(slModelFileName.empty());
}


SLAgent::~SLAgent()
{
     if (agent) { delete agent; }

     for (unsigned int i = 0; i < varList.size(); ++i)
          if (varList[i]) { delete varList[i]; }
}


// ===========================================================================
//	Public static methods
// ===========================================================================
void SLAgent::generateSLSamples(
          const DirMultiDistribution* dirDistrib,
          unsigned int nbOfMDPs, double simGamma, unsigned int T,
          vector<vector<double> >& inputs,
          vector<vector<double> >& outputs,
          vector<double>& weights,
          vector<unsigned int>& indexes)
{
     generateSLSamples(0, dirDistrib, nbOfMDPs, simGamma, T,
                       inputs, outputs, weights, indexes);
}


void SLAgent::generateSLSamples(
          Agent* agent,
          const DirMultiDistribution* dirDistrib,
          unsigned int nbOfMDPs, double simGamma, unsigned int T,
          vector<vector<double> >& inputs,
          vector<vector<double> >& outputs,
          vector<double>& weights,
          vector<unsigned int>& indexes)
{
     //   Draw the trajectories
     vector<MDP*> mdpList;
     vector<simulation::SimulationRecord> simRecList;
     for (unsigned int i = 0; i < nbOfMDPs; ++i)
     {
          //   Generate new samples
               //   Agent
          double epsilon = i / (double) (nbOfMDPs - 1);
          Agent* eAgent  = new EAgent(epsilon, new OptimalAgent());
          eAgent->learnOffline(dirDistrib);

               //   MDP
          mdpList.push_back(dirDistrib->draw());
          
               //   Simulation
          simRecList.push_back(
               simulation::simulate(eAgent, mdpList[i], simGamma, T,
                                    false));

               //   Free
          delete eAgent;
     }


     //   Initialize 'agent'
     if (agent) { agent->learnOffline(dirDistrib); }


     //   Build a set of variables
     vector<FVariable*> varList = buildVarList(dirDistrib);


     //   Build the samples
     inputs.clear();
     outputs.clear();
     double sumWeights = 0.0;
     for (unsigned int i = 0; i < simRecList.size(); ++i)
     {
          //   Retrieve the list of transitions from this trajectory
          vector<simulation::Transition> transitionList;
          transitionList = simRecList[i].getTransitionList();

          //   Reset 'agent'
          if (agent) { agent->setMDP(mdpList[i], simGamma, T); }

          //   Reset the variables of 'varList'
          for (unsigned int j = 0; j < varList.size(); ++j)
               varList[j]->reset(mdpList[i], simGamma, T);

          //   Browse the trajectory
          for (unsigned int j = 0; j < transitionList.size(); ++j)
          {
               //   Retrieve the current transition
               simulation::Transition t = transitionList[j];
               unsigned int uP = t.u;
               
               //   Replace the action in this transition by the one that would
               //   have been chosen by 'agent'
               if (agent) { uP = agent->getAction(t.x); }
               
               //   Build <input, output> pair
               vector<double> input;
               input = buildInput(
                         j, varList, dirDistrib->getNbActions(), t.x);
               
               vector<double> output;
               for (int u = 0; u < dirDistrib->getNbActions(); ++u)
               {
                    if (u == uP) { output.push_back( 1.0); }
                    else         { output.push_back(-1.0); }
               }
               
               inputs.push_back(input);
               outputs.push_back(output);
               weights.push_back(pow(simGamma, j));               
               indexes.push_back(i);

               sumWeights += weights.back();

               //   Update 'agent'
               if (agent) { agent->learnOnline(t.x, t.u, t.y, t.r); }

               //   Update the variables
               for (unsigned int i = 0; i < varList.size(); ++i)
                    varList[i]->update(t.x, t.u, t.y, t.r);
          }
     }
     
     
     //   Adjust the weights
     for (unsigned int i = 0; i < weights.size(); ++i)
          weights[i] *= (inputs.size() / sumWeights);
     

     //   Free
     for (unsigned int i = 0; i < varList.size(); ++i)
          delete varList[i];
     
     for (unsigned int i = 0; i < mdpList.size(); ++i)
          delete mdpList[i];
}


void SLAgent::saveSLSamples(const vector<vector<double> >& inputs,
                            const vector<vector<double> >& outputs,
                            const vector<double>& weights,
                            const vector<unsigned int>& indexes,
                            ostream& os)
{    
     //   No sample case
     if (inputs.empty())
     {
          os << "0 0 0\n";
//           os.close();
          return;
     }


     //   Samples case
     os << inputs.size();
     os << " " << inputs[0].size() << " " << outputs[0].size() << "\n";
     for (unsigned int i = 0; i < inputs.size(); ++i)
     {
          os << inputs[i][0];
          for (unsigned int j = 1; j < inputs[i].size(); ++j)
               os << " " << inputs[i][j];
          os << "\n";

          os << outputs[i][0];
          for (unsigned int j = 1; j < outputs[i].size(); ++j)
               os << " " << outputs[i][j];
          os << "\n";
     }
     
     
     //   'weights'
     for (unsigned int i = 0; i < weights.size(); ++i)
          os << weights[i] << "\n";


     //   'indexes'
     for (unsigned int i = 0; i < indexes.size(); ++i)
          os << indexes[i] << "\n";
     
     
//      os.close();
}


void SLAgent::loadSLSamples(istream& is,
                            vector<vector<double> >& inputs,
                            vector<vector<double> >& outputs,
                            vector<double>& weights,
                            vector<unsigned int>& indexes)
{
     //   Clear 'inputs', 'outputs', 'weights' and 'indexes'
     inputs.clear();
     outputs.clear();
     weights.clear();
     indexes.clear();

     
     //   Load samples
     string line, tmp;
     getline(is, line);
     
     tmp = line.substr(0, line.find_first_of(" "));
     unsigned int nbSamples = atoi(tmp.c_str());
     line = line.substr(line.find_first_of(" ") + 1);
     
     tmp = line.substr(0, line.find_first_of(" "));
     unsigned int nbInputs = atoi(tmp.c_str());
     line = line.substr(line.find_first_of(" ") + 1);
     
     tmp = line.substr(0, line.find_first_of(" "));
     unsigned int nbOutputs = atoi(tmp.c_str());
     
     unsigned int i = 0;
     while (i < nbSamples && getline(is, line))
     {
          //   'input'
          vector<double> input;
          for (unsigned int j = 0; j < nbInputs; ++j)
          {
               tmp = line.substr(0, line.find_first_of(" "));
               input.push_back(atof(tmp.c_str()));
               
               line = line.substr(line.find_first_of(" ") + 1);
          }
          inputs.push_back(input);


          //   'output'
          getline(is, line);
          vector<double> output;
          for (unsigned int j = 0; j < nbOutputs; ++j)
          {
               tmp = line.substr(0, line.find_first_of(" "));
               output.push_back(atof(tmp.c_str()));
               
               line = line.substr(line.find_first_of(" ") + 1);
          }
          outputs.push_back(output);

          ++i;
     }
     
     
     //   Load 'weights'
     while (i++ < 2*nbSamples && getline(is, line))
          weights.push_back(atof(line.c_str()));
     
     
     //   Load 'indexes'
     while (i++ < 3*nbSamples && getline(is, line))
          indexes.push_back(atoi(line.c_str()));
     
     
//      is.close();
}


// ===========================================================================
//	Public methods
// ===========================================================================
int SLAgent::getAction(int xt) const throw (AgentException)
{
     assert(isSLModelLoaded());


     //   Build the input
     vector<double> input = buildInput(xt);
     

     //   Make a prediction of the output
     vector<double> output = predict(input);
     assert(output.size() == getMDP()->getNbActions());

     
     //   Select the best action according to the SLAgent
     vector<vector<double>::const_iterator> maxList;
	maxList =
	     utils::search::max<vector<double> >(output.begin(), output.end());
	
	assert(!maxList.empty());

	int maxI = RandomGen::randIntRange_Uniform(0, (maxList.size() - 1));
	return (maxList[maxI] - output.begin());
}


void SLAgent::learnOnline(int x, int u, int y, double r)
                                                  throw (AgentException)
{
     for (unsigned int i = 0; i < varList.size(); ++i)
          varList[i]->update(x, u, y, r);
     ++t;
}


void SLAgent::reset() throw (AgentException)
{
     for (unsigned int i = 0; i < varList.size(); ++i)
          varList[i]->reset(getMDP(), simGamma, getT());
     t = 0;


	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void SLAgent::freeData()
{
     if (agent) { agent->freeData(); }

     for (unsigned int i = 0; i < varList.size(); ++i)
          if (varList[i]) { varList[i]->freeData(); }


     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void SLAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	os << SLAgent::toString() << "\n";
	os << 6 << "\n";
     

     //   'varList'
     os << varList.size() << "\n";
     for (unsigned int i = 0; i < varList.size(); ++i)
     {
          if (!varList[i]) { os << 0 << "\n"; }
          else
          {
               os << 1 << "\n";
               os << varList[i]->getClassName() << "\n";
               
               stringstream varStream;
               varList[i]->serialize(varStream);
               
               os << varStream.str().length() << "\n";
               copy(istreambuf_iterator<char>(varStream),
                    istreambuf_iterator<char>(),
             		ostreambuf_iterator<char>(os));
          }
     }

     
     //	'agent'
	if (!agent) { os << 0 << "\n"; }
	else
	{
	    os << 1 << "\n";
	    os << agent->getClassName() << "\n";

	    stringstream agentStream;
	    agent->serialize(agentStream);
	    
	    os << agentStream.str().length() << "\n";
	    copy(istreambuf_iterator<char>(agentStream),
			istreambuf_iterator<char>(),
			ostreambuf_iterator<char>(os));
	}
     
     
     //   'nbOfMDPs'
     os << nbOfMDPs << "\n";
     
     
     //   'simGamma'
     os << simGamma << "\n";
     
     
     //   'T'
     os << T << "\n";
     

     //   'slModelFileName'
     os << slModelFileName << "\n";

     
     //   Save the current SL model
     saveSLModel();
}


void SLAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != SLAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	
	int i = 0;
	
	
	//  'varList'
	for (unsigned int i = 0; i < varList.size(); ++i)
	    if (varList[i]) { delete varList[i]; }
     varList.clear();

	if (!getline(is, tmp)) { throwEOFMsg("varList"); }
     unsigned int varListSize = atoi(tmp.c_str());
     
	for (unsigned int i = 0; i < varListSize; ++i)
	{
	     if (!getline(is, tmp)) { throwEOFMsg("varList"); }
	     bool hasVar = (atoi(tmp.c_str()) != 0);
	     if (!hasVar) { varList.push_back(0); continue; }
	     
	     string varClassStr;
	     if (!getline(is, varClassStr)) { throwEOFMsg("varList"); }
	     
          if (!getline(is, tmp)) { throwEOFMsg("varList"); }
          unsigned int varStreaSLength = atoi(tmp.c_str());
          
          stringstream varStream;
          tmp.resize(varStreaSLength);
          is.read(&tmp[0], varStreaSLength);
          varStream << tmp;
          
          varList.push_back(dynamic_cast<FVariable*>(
                    Serializable::createInstance(varClassStr, varStream)));
	}
	++i;
	
	
	//   'agent'
     if (agent) { delete agent; agent = 0; }
	
	if (!getline(is, tmp)) { throwEOFMsg("agent"); }
	bool hasAgent = (tmp != "0");
	
	if (hasAgent)
	{
        	if (!getline(is, tmp)) { throwEOFMsg("agent"); }
        	string agentClassName = tmp;
        	
        	if (!getline(is, tmp)) { throwEOFMsg("agent"); }
        	unsigned int agentStreaSLength = atoi(tmp.c_str());
        	
        	stringstream agentStream;
        	string tmp;
        	tmp.resize(agentStreaSLength);
        	is.read(&tmp[0], agentStreaSLength);
        	agentStream << tmp;
        	
        	agent = dynamic_cast<Agent*>(
        	      Serializable::createInstance(agentClassName, agentStream));
	}
	++i;


	//   'nbOfMDPs'
	if (!getline(is, tmp)) { throwEOFMsg("nbOfMDPs"); }
	nbOfMDPs = atoi(tmp.c_str());
	++i;


	//   'simGamma'
	if (!getline(is, tmp)) { throwEOFMsg("simGamma"); }
	simGamma = atof(tmp.c_str());
	++i;


	//   'T'
	if (!getline(is, tmp)) { throwEOFMsg("T"); }
	T = atoi(tmp.c_str());
	++i;

	
	//   'slModelFileName'
	if (!getline(is, tmp)) { throwEOFMsg("slModelFileName"); }
	slModelFileName = tmp;
	++i;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
	
	
	//   Load the current SL model
	loadSLModel();
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void SLAgent::saveSLModel() const { saveSLModel(slModelFileName); }


void SLAgent::loadSLModel()       { loadSLModel(slModelFileName); }


// ===========================================================================
//	Protected static methods
// ===========================================================================
void SLAgent::insertSamples(
          vector<vector<double> >& inputs1,
          vector<vector<double> >& outputs1,
          vector<double>& weights1,
          vector<unsigned int>& indexes1,
          const vector<vector<double> >& inputs2,
          const vector<vector<double> >& outputs2,
          const vector<double>& weights2,
          const vector<unsigned int>& indexes2)
{
     for (unsigned int i = 0; i < inputs2.size(); ++i)
          inputs1.push_back(inputs2[i]);
     
     for (unsigned int i = 0; i < outputs2.size(); ++i)
          outputs1.push_back(outputs2[i]);
     
     for (unsigned int i = 0; i < weights2.size(); ++i)
          weights1.push_back(weights2[i]);
     
     for (unsigned int i = 0; i < indexes2.size(); ++i)
          indexes1.push_back(indexes2[i]);
}


void SLAgent::packSamples(vector<vector<double> >& inputs,
                          vector<vector<double> >& outputs,
                          vector<double>& weights,
                          const vector<unsigned int>& indexes,
                          vector<vector<unsigned int> >& packedIndexes)
{
     packedIndexes.clear();
     if (inputs.empty()) { return; }


     //   Sort the data
     vector<pair<vector<double>, int> > pairedInputs;
     for (unsigned int i = 0; i < inputs.size(); ++i)
     {
          pairedInputs.push_back(
               pair<vector<double>, int>(inputs[i], i));
     }
     sort(pairedInputs.begin(), pairedInputs.end());


     //   Create a pack for consecutive identical elements
     vector<vector<double> > nInputs, nOutputs;
     vector<double> nWeights;
     vector<unsigned int> pack;
     for (unsigned int i = 0; i <= pairedInputs.size(); ++i)
     {
          //   Create a pack if necessary
          int m = i < pairedInputs.size() ? pairedInputs[i].second : -1;         
          if (!pack.empty()
                    && ((m == -1) || (inputs[m] != inputs[pack.back()])))
          {
               unsigned int nElem = pack.size();
          
               //   'nInputs'
               nInputs.push_back(inputs[pack[0]]);               
               
               //   'nOutputs'
               nOutputs.push_back(outputs[pack[0]]);
               for (unsigned int j = 1; j < pack.size(); ++j)
               {
                    nOutputs.back() = combine(
                              nOutputs.back(), outputs[pack[j]],
                              j / (j + nElem), 1.0 / (j + nElem));
               }
               
               //   'nWeights'
               nWeights.push_back(weights[pack[0]]);
               for (unsigned int j = 1; j < pack.size(); ++j)
               {
                    nWeights.back() =
                              (j / (j + nElem))*nWeights.back()
                                   + (1.0 / (j + nElem))*weights[pack[j]];
               }

               //   'nIndexes'
               packedIndexes.push_back(vector<unsigned int>());
               for (unsigned int j = 0; j < pack.size(); ++j)
                    packedIndexes.back().push_back(indexes[pack[j]]);
               
               pack.clear();
          }
          
          if (m != 1) { pack.push_back(m); }
     }

     inputs  = nInputs;
     outputs = nOutputs;
     weights = nWeights;
}


// ===========================================================================
//	Private static methods
// ===========================================================================
 vector<double> SLAgent::combine(const vector<double>& a,
                                 const vector<double>& b,
                                 double aWeight,
                                 double bWeight)
{
     vector<double> c;
     for (unsigned int i = 0; i < a.size(); ++i)
          c.push_back(aWeight*a[i] + bWeight*b[i]);
     
     return c;
}


vector<FVariable*> SLAgent::buildVarList(const MDPDistribution* mdpDistrib)
{
     vector<FVariable*> varList;
     varList.push_back(new QMean());

     for (unsigned int i = 0; i < varList.size(); ++i)
          varList[i]->init(mdpDistrib);

     return varList;
}


vector<double> SLAgent::buildInput(
          unsigned int t,
          const vector<FVariable*>& varList,
          unsigned int nU,
          unsigned int x)
{
     vector<double> input;
     
     
     //   'varList'
     for (unsigned int i = 0; i < varList.size(); ++i)
     {
          for (unsigned int u = 0; u < nU; ++u)
               input.push_back((*varList[i])(x, u));
     }


     //   Return
     return input;
}


// ===========================================================================
//	Private methods
// ===========================================================================
void SLAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
											throw (AgentException)
{
	//	'DirMultiDistribution' case
	try
	{
          const DirMultiDistribution* dirDistrib = 
				dynamic_cast<const DirMultiDistribution*>(mdpDistrib);


          //   Initialize the variables
          varList = buildVarList(dirDistrib);


          //   Generate the samples
          vector<vector<double> > inputs, outputs;
          vector<double> weights;
          vector<unsigned int> indexes;
          generateSLSamples(agent, dirDistrib, nbOfMDPs, simGamma, T,
                            inputs, outputs, weights, indexes);

          
          //   Save the samples
          stringstream sstr;
          sstr << "data/sl_samples/LS_[" << getName() << "].dat";
          
          ofstream os(sstr.str().c_str());
          saveSLSamples(inputs, outputs, weights, indexes, os);
          os.close();

     
          //   Train on the samples
          train(inputs, outputs, weights, indexes);
     }

	
	//	Other cases
	catch (bad_cast)
	{
		string msg;
		msg += "Unsupported MDPDistribution for offline learning!\n";
		
		throw AgentException(msg);
	}
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


vector<double> SLAgent::buildInput(unsigned int x) const
{
     return buildInput(t, varList, getMDP()->getNbActions(), x);
}


#ifndef NDEBUG
void SLAgent::checkIntegrity() const
{
	assert((simGamma > 0.0) && (simGamma <= 1.0));
     assert(T > 0);
}
#endif
