
#include "Model.h"

using namespace std;
using namespace dds;


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
Model::Model(	string name,
			unsigned int nX, unsigned int nU,
			int iniState,
			const vector<double>& P,
			RewardType rType,
			const vector<double>& R, const vector<double>& V) :
				MDP(name, nX, nU, iniState, P, rType, R, V) {}


// ===========================================================================
//	Public methods
// ===========================================================================
void Model::serialize(ostream& os) const
{
	MDP::serialize(os);
	
	
	os << Model::toString() << "\n";
	os << 0 << "\n";
}


void Model::deserialize(istream& is) throw (SerializableException)
{
	MDP::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != Model::toString())
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
