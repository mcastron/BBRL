
#include "IAgent.h"

using namespace std;


// ===========================================================================
//	Public methods
// ===========================================================================
void IAgent::serialize(ostream& os) const
{
	Serializable::serialize(os);
	
	
	os << IAgent::toString() << "\n";
	os << 1 << "\n";
	os << name << "\n";
}


void IAgent::deserialize(istream& is) throw (SerializableException)
{
	Serializable::deserialize(is);
	
	string tmp;
	
	
	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != IAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	
	int i = 0;
	
	
	//	'name'
	if (!getline(is, tmp)) { throwEOFMsg("name"); }
	name = tmp;
	++i;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
