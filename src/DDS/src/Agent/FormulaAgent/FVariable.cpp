
#include "FVariable.h"
#include "QMean.h"
#include "QSelf.h"
#include "QUniform.h"

using namespace std;
using namespace dds;


// ===========================================================================
//	Public static methods
// ===========================================================================
FVariable* FVariable::getFVariable(std::string str) throw (FVariableException)
{
     //   Instantiate the correct FVariable
     if (str == "QMean")    { return new QMean();    }
     if (str == "QSelf")    { return new QSelf();    }
     if (str == "QUniform") { return new QUniform(); }


     //   Error
     std::string msg;
     msg += "Unable to convert \"" + str + "\" into a FVariable!\n";
     
     throw FVariableException(msg);
}


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
