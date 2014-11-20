
#include "FVariable.h"

using namespace std;
using namespace dds;


// ===========================================================================
//	Public methods
// ===========================================================================
void FVariable::serialize(ostream& os) const
{
     Serializable::serialize(os);


	os << FVariable::toString() << "\n";
	os << 0 << "\n";
}


void FVariable::deserialize(istream& is) throw (SerializableException)
{
     Serializable::deserialize(is);
     
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != FVariable::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters check
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	if (n != 0)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
