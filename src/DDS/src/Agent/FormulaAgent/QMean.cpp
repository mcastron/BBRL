
#include "QMean.h"

using namespace std;
using namespace dds;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
QMean::QMean(std::istream& is)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


QMean::QMean() {}


QMean::~QMean() {}


// ===========================================================================
//	Public methods
// ===========================================================================
void QMean::init(const dds::MDPDistribution* mdpDistrib)
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
          const vector<double>& N = dirDistrib->getTheta();
          const vector<double>& R = dirDistrib->getR();
          const vector<double>& V = dirDistrib->getV();


          //   Uniform model
          iniModel = new dds::CModel(
                    "mean(" + dirDistrib->getShortName() + ")",
                    nX, nU, iniState, N, rType, R, V);
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


void QMean::serialize(ostream& os) const
{
	QVar::serialize(os);
	
	
	os << QMean::toString() << "\n";
	os << 0 << "\n";
}


void QMean::deserialize(istream& is) throw (SerializableException)
{
	QVar::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != QMean::toString())
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
