
#include "Serializable.h"

using namespace std;
using namespace utils::compression;


// ===========================================================================
//	Public methods
// ===========================================================================
void Serializable::serialize(ostream& os) const
{
	os.precision(numeric_limits<double>::digits10);

	os << Serializable::toString() << "\n";
	os << 0 << "\n";
}


void Serializable::zSerialize(ostream& os, int compressionLevel) const
										throw (SerializableException)
{
	string cStr;
	
	try
	{
		stringstream sstr;
		serialize(sstr);
		cStr = compressStr(sstr.str(), compressionLevel);
	}
	
	catch (runtime_error e)
	{		
		string msg;
		msg += "An error occured during the compresssion! (";
		msg += e.what();
		msg += ")\n";
		
		throw SerializableException(msg);
	}
	
	os << cStr;
}


void Serializable::deserialize(istream& is) throw (SerializableException)
{
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != Serializable::toString())
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


void Serializable::dDeserialize(istream& is) throw (SerializableException)
{
	string dStr;
	
	try
	{
		istreambuf_iterator<char> eos;
		string cStr(istreambuf_iterator<char>(is), eos);
		dStr = decompressStr(cStr);
	}
	
	catch (runtime_error e)
	{
		is.seekg (0, is.beg);
		
		string msg;
		msg += "An error occured during the decompresssion! (";
		msg += e.what();
		msg += ")\n";
		
		throw SerializableException(msg);
	}
	
	stringstream sstr;
	sstr << dStr;
	deserialize(sstr);
}


// ===========================================================================
//	Private state attributes initialization
// ===========================================================================
map<string, Serializable*(*)(istream&)> Serializable::newMap;
