
#include "VDBEEGreedyAgentFactory.h"

using namespace std;


// ===========================================================================
//	Public Constructor
// ===========================================================================
dds::VDBEEGreedyAgentFactory::VDBEEGreedyAgentFactory(std::istream& is) :
          AgentFactory()
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


dds::VDBEEGreedyAgentFactory::VDBEEGreedyAgentFactory(
		double minSigma_, double maxSigma_,
		double minDelta_, double maxDelta_,
		double minIniEps_, double maxIniEps_,
		double minC_, double maxC_) :
			minSigma(minSigma_), maxSigma(maxSigma_),
			minDelta(minDelta_), maxDelta(maxDelta_),
			minIniEps(minIniEps_), maxIniEps(maxIniEps_),
			minC(minC_), maxC(maxC_)
{
	assert((minSigma > 0.0) && (minSigma < maxSigma));
	assert((maxSigma > 0.0) && (maxSigma > minSigma));

	assert((minDelta >= 0.0) && (minDelta < 1.0) && (minDelta < maxDelta));
	assert((maxDelta >= 0.0) && (maxDelta < 1.0) && (maxDelta > minDelta));

	assert((minIniEps >= 0) && (minIniEps <= 1.0) && (minIniEps < maxIniEps));
	assert((maxIniEps >= 0) && (maxIniEps <= 1.0) && (maxIniEps > minIniEps));

	assert((minC >= 0.0) && (minC < maxC));
	assert((maxC >= 0.0) && (maxC > minC));
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
		
		
		nX = dirDistrib->getNbStates();
		nU = dirDistrib->getNbActions();
		iniState = dirDistrib->getIniState();
		R = dirDistrib->getR();
		V = dirDistrib->getV();
		
		boundsList.clear();
		splitAccList.clear();
		
		boundsList.push_back(pair<double, double>(minSigma, maxSigma));
		splitAccList.push_back(0.01);
		
		boundsList.push_back(pair<double, double>(minDelta, maxDelta));
		splitAccList.push_back(0.01);
		
		boundsList.push_back(pair<double, double>(minIniEps, maxIniEps));
		splitAccList.push_back(0.01);
		
		for (unsigned int x = 0; x < nX; ++x)
			for (unsigned int u = 0; u < nU; ++u)
				for (unsigned int y = 0; y < nX; ++y)
				{
					boundsList.push_back(
							pair<double, double>(minC, maxC));
					splitAccList.push_back(1.0);
				}
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
	assert(paramList.size() == (3 + nX*nU*nX));
	
	
	double sigma = paramList[0];
	double delta = paramList[1];
	double iniEps = paramList[2];
	
	vector<double> N;
	for (unsigned int i = 3; i < paramList.size(); ++i)
		N.push_back(round(paramList[i]));
	
	return (new VDBEEGreedyAgent(
			sigma, delta, iniEps,
			new CModel("", nX, nU, iniState, N, rType, R, V)));
}


void dds::VDBEEGreedyAgentFactory::serialize(ostream& os) const
{
	AgentFactory::serialize(os);
	
	
	os << VDBEEGreedyAgentFactory::toString() << "\n";
	os << 8 << "\n";
	
	
	//  'minSigma'
	os << minSigma << "\n";
	
	
	//  'maxSigma'
	os << maxSigma << "\n";
	
	
	//  'minDelta'
	os << minDelta << "\n";
	
	
	//  'maxDelta'
	os << maxDelta << "\n";
	
	
	//  'minEps'
	os << minIniEps << "\n";
	
	
	//  'maxEps'
	os << maxIniEps << "\n";
	
	
	//  'minC'
	os << minC << "\n";
	
	
	//  'maxC'
	os << maxC << "\n";
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


     //	'minC'
	if (!getline(is, tmp)) { throwEOFMsg("minC"); }
	minC = atof(tmp.c_str());
	++i;


     //	'maxC'
	if (!getline(is, tmp)) { throwEOFMsg("maxC"); }
	maxC = atof(tmp.c_str());
	++i;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
