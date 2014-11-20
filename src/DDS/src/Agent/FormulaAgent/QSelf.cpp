
#include "QSelf.h"

using namespace std;
using namespace dds;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
QSelf::QSelf(std::istream& is)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


QSelf::QSelf() {}


QSelf::~QSelf() {}


// ===========================================================================
//	Public methods
// ===========================================================================
void QSelf::init(const dds::MDPDistribution* mdpDistrib)
                                                  throw (FVariableException)
{
     if (iniModel) { delete(iniModel); iniModel = 0; }

     
     //   'DirMultiDistribution' case
     const dds::DirMultiDistribution* dirDistrib = 
               dynamic_cast<const dds::DirMultiDistribution*>(mdpDistrib);

     if (dirDistrib)
     {
          unsigned int nX = dirDistrib->getNbStates();
          unsigned int nU = dirDistrib->getNbActions();
          int iniState = dirDistrib->getIniState();
          RewardType rType = dirDistrib->getRType();
          const vector<double>& R = dirDistrib->getR();
          const vector<double>& V = dirDistrib->getV();


          //   Uniform model
          vector<double> N(nX*nU*nX);
          for (unsigned int x = 0; x < nX; ++x)
               for (unsigned int u = 0; u < nU; ++u)
                    N[nX*nU*x + nX*u + x] = 1.0;
               
          iniModel = new dds::CModel(
                    "self", nX, nU, iniState, N, rType, R, V);
     }


     //   Other case
     else
     {
          string msg;
          msg += "Unsupported MDPDistribution for offline ";
          msg += "learning!\n";
          
          throw FVariableException(msg);
     }
}


void QSelf::serialize(ostream& os) const
{
	QVar::serialize(os);
	
	
	os << QSelf::toString() << "\n";
	os << 0 << "\n";
}


void QSelf::deserialize(istream& is) throw (SerializableException)
{
	QVar::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != QSelf::toString())
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
