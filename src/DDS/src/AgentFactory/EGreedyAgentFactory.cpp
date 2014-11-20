
#include "EGreedyAgentFactory.h"

using namespace std;


// ===========================================================================
//	Public Constructor
// ===========================================================================
dds::EGreedyAgentFactory::EGreedyAgentFactory(std::istream& is) :
          AgentFactory()
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


dds::EGreedyAgentFactory::EGreedyAgentFactory(
		double minEps_, double maxEps_, double minC_, double maxC_) :
		     shortDistribName("no name"),
			minEps(minEps_), maxEps(maxEps_), minC(minC_), maxC(maxC_)
{
	assert((minEps >= 0.0) && (minEps <= 1.0) && (minEps < maxEps));
	assert((maxEps >= 0.0) && (maxEps <= 1.0) && (maxEps > minEps));

	assert((minC >= 0.0) && (minC < maxC));
	assert((maxC >= 0.0) && (maxC > minC));
}


// ===========================================================================
//	Public methods
// ===========================================================================
void dds::EGreedyAgentFactory::init(const dds::MDPDistribution* mdpDistrib)
									throw (AgentFactoryException)
{
	assert(mdpDistrib);
	
	
	try
	{
		const dds::DirMultiDistribution* dirDistrib = 
				dynamic_cast<const DirMultiDistribution*>(mdpDistrib);
		
		shortDistribName = dirDistrib->getShortName();		
		nX = dirDistrib->getNbStates();
		nU = dirDistrib->getNbActions();
		iniState = dirDistrib->getIniState();
		R = dirDistrib->getR();
		V = dirDistrib->getV();
		
		boundsList.clear();
		splitAccList.clear();
		
		boundsList.push_back(pair<double, double>(minEps, maxEps));
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


dds::Agent* dds::EGreedyAgentFactory::get(const vector<double>& paramList)
									const throw (AgentFactoryException)
{
	assert(paramList.size() == (1 + nX*nU*nX));
	
	
	double epsilon = paramList[0];
	vector<double> N;
	for (unsigned int i = 1; i < paramList.size(); ++i)
		N.push_back(round(paramList[i]));
	
	return (new EGreedyAgent(
               epsilon,
			new CModel(shortDistribName, nX, nU, iniState, N, rType, R, V)));
}


void dds::EGreedyAgentFactory::serialize(ostream& os) const
{
	AgentFactory::serialize(os);
	
	
	os << EGreedyAgentFactory::toString() << "\n";
	os << 4 << "\n";
	
	
	//  'minEps'
	os << minEps << "\n";
	
	
	//  'maxEps'
	os << maxEps << "\n";
	
	
	//  'minC'
	os << minC << "\n";
	
	
	//  'maxC'
	os << maxC << "\n";
}


void dds::EGreedyAgentFactory::deserialize(istream& is)
                                                  throw (SerializableException)
{
	AgentFactory::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != EGreedyAgentFactory::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
     //	'minEps'
	if (!getline(is, tmp)) { throwEOFMsg("minEps"); }
	minEps = atof(tmp.c_str());
	++i;


     //	'maxEps'
	if (!getline(is, tmp)) { throwEOFMsg("maxEps"); }
	maxEps = atof(tmp.c_str());
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
