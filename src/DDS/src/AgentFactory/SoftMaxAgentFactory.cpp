
#include "SoftMaxAgentFactory.h"

using namespace std;


// ===========================================================================
//	Public Constructor
// ===========================================================================
dds::SoftMaxAgentFactory::SoftMaxAgentFactory(std::istream& is) :
          AgentFactory()
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


dds::SoftMaxAgentFactory::SoftMaxAgentFactory(
		double minTau_, double maxTau_, double minC_, double maxC_) :
			minTau(minTau_), maxTau(maxTau_),
			minC(minC_), maxC(maxC_)
{
	assert((minTau > 0.0) && (minTau < maxTau));
	assert((maxTau > 0.0) && (maxTau > minTau));

	assert((minC >= 0.0) && (minC < maxC));
	assert((maxC >= 0.0) && (maxC > minC));
}


// ===========================================================================
//	Public methods
// ===========================================================================
void dds::SoftMaxAgentFactory::init(const dds::MDPDistribution* mdpDistrib)
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
		
		boundsList.push_back(pair<double, double>(minTau, maxTau));
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


dds::Agent* dds::SoftMaxAgentFactory::get(const vector<double>& paramList)
								const throw (AgentFactoryException)
{
	assert(paramList.size() == (1 + nX*nU*nX));
	
	
	double tau = paramList[0];
	vector<double> N;
	for (unsigned int i = 1; i < paramList.size(); ++i)
		N.push_back(round(paramList[i]));
	
	return (new SoftMaxAgent(
			tau, new CModel("", nX, nU, iniState, N, rType, R, V)));
}


void dds::SoftMaxAgentFactory::serialize(ostream& os) const
{
	AgentFactory::serialize(os);
	
	
	os << SoftMaxAgentFactory::toString() << "\n";
	os << 4 << "\n";
	
	
	//  'minTau'
	os << minTau << "\n";
	
	
	//  'maxTau'
	os << maxTau << "\n";
	
	
	//  'minC'
	os << minC << "\n";
	
	
	//  'maxC'
	os << maxC << "\n";
}


void dds::SoftMaxAgentFactory::deserialize(istream& is)
                                                  throw (SerializableException)
{
	AgentFactory::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != SoftMaxAgentFactory::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
     //	'minEps'
	if (!getline(is, tmp)) { throwEOFMsg("minTau"); }
	minTau = atof(tmp.c_str());
	++i;


     //	'maxEps'
	if (!getline(is, tmp)) { throwEOFMsg("maxTau"); }
	maxTau = atof(tmp.c_str());
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
