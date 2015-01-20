
#include "SoftMaxAgentFactory.h"

using namespace std;


// ===========================================================================
//	Public Constructor
// ===========================================================================
dds::SoftMaxAgentFactory::SoftMaxAgentFactory(std::istream& is) :
          AgentFactory(), iniModel(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


dds::SoftMaxAgentFactory::SoftMaxAgentFactory(double minTau_, double maxTau_) :
          minTau(minTau_), maxTau(maxTau_), iniModel(0)
{
	assert((minTau > 0.0) && (minTau < maxTau));
	assert((maxTau > 0.0) && (maxTau > minTau));
}


dds::SoftMaxAgentFactory::~SoftMaxAgentFactory()
{
     if (iniModel) { delete iniModel; }
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
		
		boundsList.clear();
		splitAccList.clear();
		
		boundsList.push_back(pair<double, double>(minTau, maxTau));
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


dds::Agent* dds::SoftMaxAgentFactory::get(const vector<double>& paramList)
								const throw (AgentFactoryException)
{
	assert(paramList.size() == 1);
	

	return (new SoftMaxAgent(paramList[0], iniModel->clone()));
}


void dds::SoftMaxAgentFactory::serialize(ostream& os) const
{
	AgentFactory::serialize(os);
	
	
	os << SoftMaxAgentFactory::toString() << "\n";
	os << 2 << "\n";
	
	
	//  'minTau'
	os << minTau << "\n";
	
	
	//  'maxTau'
	os << maxTau << "\n";
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
	
	
     //	'minTau'
	if (!getline(is, tmp)) { throwEOFMsg("minTau"); }
	minTau = atof(tmp.c_str());
	++i;


     //	'maxTau'
	if (!getline(is, tmp)) { throwEOFMsg("maxTau"); }
	maxTau = atof(tmp.c_str());
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
