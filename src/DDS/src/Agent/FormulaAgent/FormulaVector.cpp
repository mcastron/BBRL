
#include "FormulaVector.h"

using namespace std;
using namespace utils;
using namespace utils::formula;


// ============================================================================
//	Public Constructor/Destructor
// ============================================================================
FormulaVector::FormulaVector(std::istream& is) : Serializable()
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


FormulaVector::FormulaVector(vector<Formula*> v) : std::vector<Formula*>(v) {}


// ===========================================================================
//	Public static methods
// ===========================================================================
FormulaVector* FormulaVector::parse(int argc, char* argv[])
                                             throw (parsing::ParsingException)
{
     assert(parsing::hasFlag(argc, argv, "--formula_set"));
     

     string formulaVectorFile
               = parsing::getValue(argc, argv, "--formula_set_file");

     ifstream is(formulaVectorFile.c_str());
     if (is.fail()) // Unable to open the file
          throw parsing::ParsingException("--formula_set_file");
     
     return dynamic_cast<FormulaVector*>(
               Serializable::createInstance<FormulaVector>(is));
}


// ============================================================================
//	Public methods
// ============================================================================
void FormulaVector::serialize(std::ostream& os) const
{
     Serializable::serialize(os);
     
     
     os << FormulaVector::toString() << "\n";
     os << 1 << "\n";
     

     //   'formulaVector'
     os << size() << "\n";
     for (unsigned int i = 0; i < size(); ++i)
          os << at(i)->getRPNStr() << "\n";     
}
		

void FormulaVector::deserialize(std::istream& is) throw (SerializableException)
{
     Serializable::deserialize(is);
     
     string tmp;
     
     
     //	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != FormulaVector::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
	//  'formulaVector'
	clear();
	if (!getline(is, tmp)) { throwEOFMsg("formulaVector"); }
	unsigned int formulaVectorSize = atoi(tmp.c_str());
	
	for (unsigned int j = 0; j < formulaVectorSize; ++j)
	{
	    if (!getline(is, tmp)) { throwEOFMsg("formulaVector"); }
	    push_back(new Formula(tmp));
	}
	++i;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}
