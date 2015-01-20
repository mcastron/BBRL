
#include "VDBEEGreedyAgentFactory.h"

using namespace std;


// ===========================================================================
//	Public Constructor
// ===========================================================================
dds::VDBEEGreedyAgentFactory::VDBEEGreedyAgentFactory(std::istream& is) :
          AgentFactory(), iniModel(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


dds::VDBEEGreedyAgentFactory::VDBEEGreedyAgentFactory(
		double minSigma_, double maxSigma_,
		double minDelta_, double maxDelta_,
		double minIniEps_, double maxIniEps_) :
			minSigma(minSigma_), maxSigma(maxSigma_),
			minDelta(minDelta_), maxDelta(maxDelta_),
			minIniEps(minIniEps_), maxIniEps(maxIniEps_),
			iniModel(0)
{
	assert((minSigma > 0.0) && (minSigma < maxSigma));
	assert((maxSigma > 0.0) && (maxSigma > minSigma));

	assert((minDelta >= 0.0) && (minDelta < 1.0) && (minDelta < maxDelta));
	assert((maxDelta >= 0.0) && (maxDelta < 1.0) && (maxDelta > minDelta));

	assert((minIniEps >= 0) && (minIniEps <= 1.0) && (minIniEps < maxIniEps));
	assert((maxIniEps >= 0) && (maxIniEps <= 1.0) && (maxIniEps > minIniEps));
}


dds::VDBEEGreedyAgentFactory::~VDBEEGreedyAgentFactory()
{
     if (iniModel) { delete iniModel; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
void dds::VDBEEGreedyAgentFactory::init(const dds::MDPDistribution* mdpDistrib)
									throw (AgentFactoryException)
{
	assert(mdpDistrib);
	
	
	try
	{
		const dds::DirMultiDistribution* dirDistrib = 
				dynamic_cast<const DirMultiDistribution*>(mdpDistrib);
		
		boundsList.clear();
		splitAccList.clear();
		
		boundsList.push_back(pair<double, double>(minSigma, maxSigma));
		splitAccList.push_back(0.01);
		
		boundsList.push_back(pair<double, double>(minDelta, maxDelta));
		splitAccList.push_back(0.01);
		
		boundsList.push_back(pair<double, double>(minIniEps, maxIniEps));
		splitAccList.push_back(0.01);


		std::string shortDistribName = dirDistrib->getShortName();		
		unsigned int nX = dirDistrib->getNbStates();
		unsigned int nU = dirDistrib->getNbActions();
		int iniState = dirDistrib->getIniState();
		vector<double> N = dirDistrib->getTheta();
		RewardType rType = dirDistrib->getRType();
		vector<double> R = dirDistrib->getR();
		vector<double> V = dirDistrib->getV();

          if (iniModel) { delete iniModel; }
          iniModel = new CModel(
                    shortDistribName, nX, nU, iniState, N, rType, R, V);
	}
	
	
	//	Other cases
	catch (bad_cast)
	{
		string msg;
		msg += "Unsupported MDPDistribution for initialization!\n";
		
		throw AgentFactoryException(msg);
	}	
}


dds::Agent* dds::VDBEEGreedyAgentFactory::get(const vector<double>& paramList)
									const throw (AgentFactoryException)
{
	assert(paramList.size() == 3);
	
	
	double sigma = paramList[0];
	double delta = paramList[1];
	double iniEps = paramList[2];
	
	return (new VDBEEGreedyAgent(sigma, delta, iniEps, iniModel->clone()));
}


void dds::VDBEEGreedyAgentFactory::serialize(ostream& os) const
{
	AgentFactory::serialize(os);
	
	
	os << VDBEEGreedyAgentFactory::toString() << "\n";
	os << 6 << "\n";
	
	
	//  'minSigma'
	os << minSigma << "\n";
	
	
	//  'maxSigma'
	os << maxSigma << "\n";
	
	
	//  'minDelta'
	os << minDelta << "\n";
	
	
	//  'maxDelta'
	os << maxDelta << "\n";
	
	
	//  'minIniEps'
	os << minIniEps << "\n";
	
	
	//  'maxIniEps'
	os << maxIniEps << "\n";
}


void dds::VDBEEGreedyAgentFactory::deserialize(istream& is)
                                                  throw (SerializableException)
{
	AgentFactory::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != VDBEEGreedyAgentFactory::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
     //	'minSigma'
	if (!getline(is, tmp)) { throwEOFMsg("minSigma"); }
	minSigma = atof(tmp.c_str());
	++i;


     //	'maxSigma'
	if (!getline(is, tmp)) { throwEOFMsg("maxSigma"); }
	maxSigma = atof(tmp.c_str());
	++i;


     //	'minDelta'
	if (!getline(is, tmp)) { throwEOFMsg("minDelta"); }
	minDelta = atof(tmp.c_str());
	++i;


     //	'maxDelta'
	if (!getline(is, tmp)) { throwEOFMsg("maxDelta"); }
	maxDelta = atof(tmp.c_str());
	++i;


     //	'minIniEps'
	if (!getline(is, tmp)) { throwEOFMsg("minIniEps"); }
	minIniEps = atof(tmp.c_str());
	++i;


     //	'maxIniEps'
	if (!getline(is, tmp)) { throwEOFMsg("maxIniEps"); }
	maxIniEps = atof(tmp.c_str());
	++i;


     //  'iniModel'
	if (iniModel) { delete iniModel; iniModel = 0; }
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
