
#include "FVariableVector.h"

using namespace dds;
using namespace std;
using namespace utils;
using namespace utils::formula;


// ============================================================================
//	Public Constructor/Destructor
// ============================================================================
FVariableVector::FVariableVector(std::istream& is) : Serializable()
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


FVariableVector::FVariableVector(vector<FVariable*> v) :
          std::vector<FVariable*>(v) {}


// ===========================================================================
//	Public static methods
// ===========================================================================
FVariableVector* FVariableVector::parse(int argc, char* argv[])
                                             throw (parsing::ParsingException)
{
     assert(parsing::hasFlag(argc, argv, "--variable_set"));
     

     string FVariableVectorFile
               = parsing::getValue(argc, argv, "--variable_set_file");

     ifstream is(FVariableVectorFile.c_str());
     if (is.fail()) // Unable to open the file
          throw parsing::ParsingException("--variable_set_file");
     
     return dynamic_cast<FVariableVector*>(
               Serializable::createInstance<FVariableVector>(is));
}


// ============================================================================
//	Public methods
// ============================================================================
void FVariableVector::serialize(std::ostream& os) const
{
     Serializable::serialize(os);
     
     
     os << FVariableVector::toString() << "\n";
     os << 1 << "\n";
     

     //   'FVariableVector'
     os << size() << "\n";
     for (unsigned int i = 0; i < size(); ++i)
     {
          if (!at(i)) { os << 0 << "\n"; }
          else
          {
               os << 1 << "\n";
               os << at(i)->getClassName() << "\n";
     
               stringstream varStream;
               at(i)->serialize(varStream);
               
               os << varStream.str().length() << "\n";
               copy(istreambuf_iterator<char>(varStream),
                    istreambuf_iterator<char>(),
               		ostreambuf_iterator<char>(os));
          }
     }
}
		

void FVariableVector::deserialize(std::istream& is) throw (SerializableException)
{
     Serializable::deserialize(is);
     
     string tmp;
     
     
     //	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != FVariableVector::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
	//  'FVariableVector'
	for (unsigned int i = 0; i < size(); ++i)
	    if (at(i)) { delete at(i); }
	clear();

	if (!getline(is, tmp)) { throwEOFMsg("FVariableVector"); }
	unsigned int vSize = atoi(tmp.c_str());
	
	for (unsigned int j = 0; j < vSize; ++j)
	{
          if (!getline(is, tmp)) { throwEOFMsg("FVariableVector"); }
          bool hasVar = (atoi(tmp.c_str()) != 0);
          if (!hasVar) { push_back(0); continue; }
          
          string varClassStr;
          if (!getline(is, varClassStr)) { throwEOFMsg("FVariableVector"); }
          
          if (!getline(is, tmp)) { throwEOFMsg("FVariableVector"); }
          unsigned int varStreamLength = atoi(tmp.c_str());
          
          stringstream varStream;
          tmp.resize(varStreamLength);
          is.read(&tmp[0], varStreamLength);
          varStream << tmp;
          
          push_back(dynamic_cast<FVariable*>(
                    Serializable::createInstance(varClassStr, varStream)));
	}
	++i;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
