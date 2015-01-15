
#include "Experiment.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructors/Destructor
// ===========================================================================
Experiment::Experiment(std::istream& is) :
		IExperiment<Agent, MDP, simulation::SimulationRecord>()
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


Experiment::Experiment(	string name, vector<MDP*>& mdpList,
					unsigned int nbSimPerMDP,
					double simGamma_,
					unsigned int T_, bool safeSim_, bool saveTraj) :
						IExperiment<	Agent, MDP,
									simulation::SimulationRecord>(
										name, mdpList,
										nbSimPerMDP, saveTraj),
						simGamma(simGamma_), T(T_), safeSim(safeSim_)
{
	assert((simGamma > 0.0) && (simGamma <= 1.0));
	assert(T > 0);
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


// ===========================================================================
//	Public static methods
// ===========================================================================
Experiment* Experiment::parse(int argc, char* argv[])
                                             throw (parsing::ParsingException)
{
     assert(parsing::hasFlag(argc, argv, "--experiment"));
 
 
     //   Get 'experimentFile'    
     string experimentFile = parsing::getValue(argc, argv, "--experiment_file");
     if (experimentFile != "")
     {
          ifstream is(experimentFile.c_str());
          if (is.fail()) //   Unable to open the file
               throw parsing::ParsingException("--experiment_file");
          
          return dynamic_cast<Experiment*>(
                    Serializable::createInstance("Experiment", is));
     }

     
     throw parsing::ParsingException("--experiment");
}


// ===========================================================================
//	Public methods
// ===========================================================================
void Experiment::serialize(ostream& os) const
{
	IExperiment<Agent, MDP, simulation::SimulationRecord>::serialize(os);
	
	
	os << Experiment::toString() << "\n";
	os << 3;
	os << "\n";
	
	
	//	'simGamma', 'T', 'safeSim'
	os << simGamma << "\n";
	os << T << "\n";
	os << safeSim << "\n";
}


void Experiment::deserialize(istream& is) throw (SerializableException)
{
	IExperiment<Agent, MDP, simulation::SimulationRecord>::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != Experiment::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	
	int i = 0;
	

	//	'simGamma'
	if (!getline(is, tmp)) { throwEOFMsg("simGamma"); }
	simGamma = atof(tmp.c_str());
	++i;

	
	//	'T'
	if (!getline(is, tmp)) { throwEOFMsg("T"); }
	T = atoi(tmp.c_str());
	++i;

	
	//	'safeSim'
	if (!getline(is, tmp)) { throwEOFMsg("safeSim"); }
	safeSim = atoi(tmp.c_str());
	++i;


	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


#ifndef NDEBUG
void Experiment::checkIntegrity() const
{
	assert((simGamma > 0.0) && (simGamma <= 1.0));
	assert(T > 0);
}
#endif
