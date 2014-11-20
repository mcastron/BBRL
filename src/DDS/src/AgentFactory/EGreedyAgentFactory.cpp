
#include "EGreedyAgentFactory.h"

using namespace std;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
dds::EGreedyAgentFactory::EGreedyAgentFactory(std::istream& is) :
          AgentFactory(), iniModel(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


dds::EGreedyAgentFactory::EGreedyAgentFactory(double minEps_, double maxEps_) :
          minEps(minEps_), maxEps(maxEps_), iniModel(0)
{
	assert((minEps >= 0.0) && (minEps <= 1.0) && (minEps < maxEps));
	assert((maxEps >= 0.0) && (maxEps <= 1.0) && (maxEps > minEps));
}


dds::EGreedyAgentFactory::~EGreedyAgentFactory()
{
     if (iniModel) { delete iniModel; }
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
		
		boundsList.clear();
		splitAccList.clear();
		
		boundsList.push_back(pair<double, double>(minEps, maxEps));
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


dds::Agent* dds::EGreedyAgentFactory::get(const vector<double>& paramList)
									const throw (AgentFactoryException)
{
	assert(paramList.size() == 1);
	

	return (new EGreedyAgent(paramList[0], iniModel->clone()));
}


void dds::EGreedyAgentFactory::serialize(ostream& os) const
{
	AgentFactory::serialize(os);
	
	
	os << EGreedyAgentFactory::toString() << "\n";
	os << 3 << "\n";
	
	
	//  'minEps'
	os << minEps << "\n";
	
	
	//  'maxEps'
	os << maxEps << "\n";
	
	
	//  'iniModel'
	stringstream iniModelStream;
	iniModel->serialize(iniModelStream);
	
	os << iniModelStream.str().length() << "\n";
	copy(istreambuf_iterator<char>(iniModelStream),
			istreambuf_iterator<char>(),
			ostreambuf_iterator<char>(os));
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
	
	
	//  'iniModel'
	if (!getline(is, tmp)) { throwEOFMsg("iniModel"); }
	unsigned int iniModelStreamLength = atoi(tmp.c_str());
	
	stringstream iniModelStream;
	tmp.resize(iniModelStreamLength);
	is.read(&tmp[0], iniModelStreamLength);
	iniModelStream << tmp;
	
	iniModel = dynamic_cast<CModel*>(
			Serializable::createInstance<CModel>(iniModelStream));
	++i;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
