
#include "MDPDistribution.h"
#include "DirMultiDistribution.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
MDPDistribution::MDPDistribution(string name, string shortName) :
          IMDPDistribution(name, shortName) {}


// ===========================================================================
//	Public static methods
// ===========================================================================
MDPDistribution* MDPDistribution::parse(int argc, char* argv[])
                                             throw (parsing::ParsingException)
{
     //   Get 'mdpDistribClassName'
     string mdpDistribClassName =
               parsing::getValue(argc, argv, "--mdp_distribution");


     //   Get 'mdpDistribFile'
     string mdpDistribFile =
               parsing::getValue(argc, argv, "--mdp_distribution_file");
     
     
     //   Instanciation and return
     ifstream is(mdpDistribFile.c_str());
	if (is.fail()) //  Unable to open the file
	    throw parsing::ParsingException("--mdp_distribution_file");
	
     return dynamic_cast<MDPDistribution*>(
               Serializable::createInstance(mdpDistribClassName, is));
}


// ===========================================================================
//	Public methods
// ===========================================================================
void MDPDistribution::serialize(ostream& os) const
{
	IMDPDistribution::serialize(os);
	
	
	os << MDPDistribution::toString() << "\n";
	os << 0 << "\n";
}


void MDPDistribution::deserialize(istream& is) throw (SerializableException)
{
	IMDPDistribution::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != MDPDistribution::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	
	int i = 0;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
