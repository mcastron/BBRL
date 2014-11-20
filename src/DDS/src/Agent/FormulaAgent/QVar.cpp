
#include "QVar.h"

using namespace std;
using namespace dds;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
QVar::QVar() : iniModel(0), model(0) {}


QVar::~QVar()
{
     if (iniModel) { delete iniModel; }
     if (model)    { delete model;    }
}


// ===========================================================================
//	Public methods
// ===========================================================================
void QVar::reset(const dds::MDP*, double gamma_, unsigned int T_)
{
     assert(iniModel);
   
   
     if (model) { delete model; model = 0; }
     gamma = gamma_;
     T = T_;
     
     model = iniModel->clone();
     Q = model->qIteration(gamma, T);
}


void QVar::freeData()
{
     if (model) { delete model; model = 0; }
     Q.clear();
}


void QVar::update(
          unsigned int x, unsigned int u, unsigned int y, double r)
{
     assert(model);
     assert(x < model->getNbStates());
     assert(u < model->getNbActions());
     assert(y < model->getNbStates());


     model->update(x, u, y, r);
     Q = model->qIteration(gamma, T);
}


double QVar::operator()(const unsigned int& x, const unsigned int& u) const
                                                  throw (FVariableException)
{
     assert(model);
     assert(x < model->getNbStates());
     assert(u < model->getNbActions());
     assert((model->getNbActions()*x + u) < Q.size());


     return Q[model->getNbActions()*x + u];
}


void QVar::serialize(ostream& os) const
{
     FVariable::serialize(os);


	os << QVar::toString() << "\n";
	os << 1 << "\n";

	
	//  'iniModel'
	if (!iniModel) { os << 0 << "\n"; }
     else
     {
          os << 1 << "\n";
          stringstream iniModelStream;
          iniModel->serialize(iniModelStream);
          
          os << iniModelStream.str().length() << "\n";
          copy(istreambuf_iterator<char>(iniModelStream),
               istreambuf_iterator<char>(),
               ostreambuf_iterator<char>(os));
     }
}


void QVar::deserialize(istream& is) throw (SerializableException)
{
     FVariable::deserialize(is);
     
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != QVar::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	
	int i = 0;
	
	
	//  'iniModel'
	if (iniModel) { delete iniModel; iniModel = 0; }
	
	if (!getline(is, tmp)) { throwEOFMsg("iniModel"); }
     bool hasIniModel = (atoi(tmp.c_str()) != 0);
     if (!hasIniModel) { iniModel = 0; }
     
     else
     {
          if (!getline(is, tmp)) { throwEOFMsg("iniModel"); }
          unsigned int iniModelStreamLength = atoi(tmp.c_str());
          
          stringstream iniModelStream;
          tmp.resize(iniModelStreamLength);
          is.read(&tmp[0], iniModelStreamLength);
          iniModelStream << tmp;
          
          iniModel = dynamic_cast<CModel*>(
                    Serializable::createInstance<CModel>(iniModelStream));
     }
     ++i;
     
     
     //   'model'
     model = 0;
	
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
	
	
	model = 0;
	Q.clear();
}
