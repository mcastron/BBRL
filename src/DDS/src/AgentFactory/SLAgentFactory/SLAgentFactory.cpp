
#include "SLAgentFactory.h"

using namespace std;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
dds::SLAgentFactory::SLAgentFactory() : agentFactory(0) {}


dds::SLAgentFactory::SLAgentFactory(std::istream& is) : agentFactory(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


dds::SLAgentFactory::SLAgentFactory(
          AgentFactory* agentFactory_,
          unsigned int nbOfMDPs_,
          double simGamma_,
          unsigned int T_,
          std::string SLModelFileName_) :
               AgentFactory(),
               agentFactory(agentFactory_),
               nbOfMDPs(nbOfMDPs_), simGamma(simGamma_), T(T_),
               SLModelFileName(SLModelFileName_)
{
     assert(agentFactory);
     assert(nbOfMDPs > 0);
     assert(simGamma > 0.0 && simGamma < 1.0);
     assert(T > 0);
     assert(SLModelFileName.empty());
}


dds::SLAgentFactory::~SLAgentFactory()
{
     if (agentFactory) { delete agentFactory; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
void dds::SLAgentFactory::init(const dds::MDPDistribution* mdpDistrib_)
									throw (AgentFactoryException)
{
     mdpDistrib = mdpDistrib_;
	agentFactory->init(mdpDistrib);
}


dds::Agent* dds::SLAgentFactory::get(const vector<double>& paraSList)
									const throw (AgentFactoryException)
{
     SLAgent* SLAgent;
     try
     {
          SLAgent = getSLAgent(agentFactory->get(paraSList),
                               nbOfMDPs, simGamma, T, SLModelFileName);

          SLAgent->learnOffline(mdpDistrib);
     }
     catch (AgentException& e) { throw AgentFactoryException(e.what()); }


     return SLAgent;
}


void dds::SLAgentFactory::serialize(ostream& os) const
{
	AgentFactory::serialize(os);
	
	
	os << SLAgentFactory::toString() << "\n";
	os << 5 << "\n";
	
	
     //  'agentFactory'
	os << agentFactory->getClassName() << "\n";

     stringstream agentFactoryStream;
     agentFactory->serialize(agentFactoryStream);

     os << agentFactoryStream.str().length() << "\n";
     copy(istreambuf_iterator<char>(agentFactoryStream),
	         istreambuf_iterator<char>(),
	         ostreambuf_iterator<char>(os));


	//  'nbOfMDPs'
	os << nbOfMDPs << "\n";
	
     //  'simGamma'
	os << simGamma << "\n";
	
	//  'T'
	os << T << "\n";
		
	//  'SLModelFileName'
	os << SLModelFileName << "\n";
}


void dds::SLAgentFactory::deserialize(istream& is)
                                                  throw (SerializableException)
{
	AgentFactory::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != SLAgentFactory::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
	//  'agentFactory'
	if (agentFactory) { delete agentFactory; agentFactory = 0; }
	
	if (!getline(is, tmp)) { throwEOFMsg("agentFactory"); }
     string agentFactoryClassName = tmp;
        	
     if (!getline(is, tmp)) { throwEOFMsg("agentFactory"); }
     unsigned int agentFactoryStreaSLength = atoi(tmp.c_str());
        	
     stringstream agentFactoryStream;
     tmp.resize(agentFactoryStreaSLength);
     is.read(&tmp[0], agentFactoryStreaSLength);
     agentFactoryStream << tmp;
        	
     agentFactory = dynamic_cast<AgentFactory*>(
               Serializable::createInstance(
                         agentFactoryClassName, agentFactoryStream));
	++i;

	
     //	'nbOfMDPs'
	if (!getline(is, tmp)) { throwEOFMsg("nbOfMDPs"); }
	nbOfMDPs = atoi(tmp.c_str());
	++i;


     //	'simGamma'
	if (!getline(is, tmp)) { throwEOFMsg("simGamma"); }
	simGamma = atof(tmp.c_str());
	++i;


     //	'T'
	if (!getline(is, tmp)) { throwEOFMsg("T"); }
	T = atoi(tmp.c_str());
	++i;


     //	'SLModelFileName'
	if (!getline(is, tmp)) { throwEOFMsg("SLModelFileName"); }
	SLModelFileName = tmp;
	++i;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
