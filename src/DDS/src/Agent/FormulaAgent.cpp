
#include "FormulaAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
FormulaAgent::FormulaAgent(std::istream& is) : Agent(), f(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


FormulaAgent::FormulaAgent(Formula* f_) : f(f_)
{
     assert(f_);
     
     
     setName("Formula agent(" + f->getRPNStr() +")");     
     
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


FormulaAgent::~FormulaAgent()
{
     if (f) { delete f; }
     
     for (unsigned int i = 0; i < modelList.size(); ++i)
          if (modelList[i]) { delete modelList[i]; }
          
     for (unsigned int i = 0; i < iniModelList.size(); ++i)
          if (iniModelList[i]) { delete iniModelList[i]; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int FormulaAgent::getAction(int xt) const throw (MDPException)
{
     unsigned int nU = getMDP()->getNbActions();
     
     
     //   Compute the Q-function with formula 'f'
	std::vector<double> Qxt;
	for (unsigned int u = 0; u < nU; ++u)
	{
	    std::vector<double> v;
	    for (unsigned int i = 0; i < QList.size(); ++i)
	    {
	         if (QList[i].empty()) { v.push_back(0.0);                 }
	         else                  { v.push_back(QList[i][nU*xt + u]); }
	    }
	    
	    Qxt.push_back((*f)(v));
	}
	
	
	//  Select the action maximizing the computed Q-function
	vector<vector<double>::const_iterator> maxList;
	maxList = utils::search::max<vector<double> >(Qxt.begin(), Qxt.end());
	
	assert(!maxList.empty());

	int maxI = RandomGen::randIntRange_Uniform(0, (maxList.size() - 1));
	return (maxList[maxI] - Qxt.begin());
}


void FormulaAgent::learnOnline(int x, int u, int y, double r)
											throw (MDPException)
{
	for (unsigned int i = 0; i < modelList.size(); ++i)
	{
	    if (modelList[i])
	    {
	         modelList[i]->update(x, u, y, r);
	         QList[i] = modelList[i]->qIteration(getGamma(), getT(), QList[i]);
	    }
	}	    
}


void FormulaAgent::reset() throw (MDPException)
{     
     //   Reset the models and Q-functions
	modelList.clear();
	QList.clear();
	for (unsigned int i = 0; i < iniModelList.size(); ++i)
	{
	    if (!iniModelList[i])
	    {
	         modelList.push_back(0);
	         QList.push_back(std::vector<double>());
	         continue;
	    }
	    
	    modelList.push_back(iniModelList[i]->clone());
	    QList.push_back((modelList.back())->qIteration(getGamma(), getT()));
	}

	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void FormulaAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << FormulaAgent::toString() << "\n";
	os << 2 << "\n";
	
	
     //   'f'
	stringstream fStream;
	f->serialize(fStream);
	
	os << fStream.str().length() << "\n";
	copy(istreambuf_iterator<char>(fStream),
          istreambuf_iterator<char>(),
		ostreambuf_iterator<char>(os));


     //   'iniModelList'
     os << iniModelList.size() << "\n";
     for (unsigned int i = 0; i < iniModelList.size(); ++i)
     {
          if (!iniModelList[i]) { os << 0 << "\n"; continue; }
          
          os << 1 << "\n";
          stringstream iniModelStream;
        	iniModelList[i]->serialize(iniModelStream);
        	
        	os << iniModelStream.str().length() << "\n";
        	copy(istreambuf_iterator<char>(iniModelStream),
               istreambuf_iterator<char>(),
        		ostreambuf_iterator<char>(os));
     }
}


void FormulaAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != FormulaAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
	//  'f'
	if (f) { delete f; }
	
	if (!getline(is, tmp)) { throwEOFMsg("f"); }
	unsigned int fStreamLength = atoi(tmp.c_str());
	
	stringstream fStream;
	tmp.resize(fStreamLength);
	is.read(&tmp[0], fStreamLength);
	fStream << tmp;
	
	f = dynamic_cast<Formula*>(
	         Serializable::createInstance<Formula>(fStream));
	++i;
	
	
	//  'iniModelList'
	for (unsigned int i = 0; i < iniModelList.size(); ++i)
	    if (iniModelList[i]) { delete iniModelList[i]; }
     iniModelList.clear();

	if (!getline(is, tmp)) { throwEOFMsg("iniModelList"); }
     unsigned int iniModelListSize = atoi(tmp.c_str());
     
	for (unsigned int i = 0; i < iniModelListSize; ++i)
	{
	     if (!getline(is, tmp)) { throwEOFMsg("iniModel"); }
	     bool hasIniModel = (atoi(tmp.c_str()) != 0);
	     if (!hasIniModel) { iniModelList.push_back(0); continue; }
	     
          if (!getline(is, tmp)) { throwEOFMsg("iniModel"); }
          unsigned int iniModelStreamLength = atoi(tmp.c_str());
          
          stringstream iniModelStream;
          tmp.resize(iniModelStreamLength);
          is.read(&tmp[0], iniModelStreamLength);
          iniModelStream << tmp;
          
          iniModelList.push_back(dynamic_cast<CModel*>(
          		Serializable::createInstance<CModel>(iniModelStream)));
	}
	++i;
		
	
	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


// ===========================================================================
//	Private methods
// ===========================================================================
void FormulaAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
											throw (AgentException)
{
	const DirMultiDistribution* dirDistrib = 
			dynamic_cast<const DirMultiDistribution*>(mdpDistrib);
	
	
	unsigned int nX = dirDistrib->getNbStates();
	unsigned int nU = dirDistrib->getNbActions();
	int iniState = dirDistrib->getIniState();
	RewardType rType = dirDistrib->getRType();
	const vector<double>& R = dirDistrib->getR();
	const vector<double>& V = dirDistrib->getV();
	
	
     //   Mean model
     const vector<double>& NMean = dirDistrib->getTheta();
	iniModelList.push_back(
          new CModel("mean(" + dirDistrib->getShortName() + ")",
                     nX, nU, iniState, NMean, rType, R, V));


     //   Uniform model
     vector<double> NUniform(nX*nU*nX);
     for (unsigned int i = 0; i < NUniform.size(); ++i)
          NUniform.push_back(1.0);
     
	iniModelList.push_back(
          new CModel("uniform", nX, nU, iniState,
                     NUniform, rType, R, V));


     //   Self model
     vector<double> NSelf(nX*nU*nX);
     for (unsigned int x = 0; x < nX; ++x)
          for (unsigned int u = 0; u < nU; ++u)
               NSelf[nX*nU*x + nX*u + x] = 1.0;

     iniModelList.push_back(
          new CModel("self", nX, nU, iniState,
                     NSelf, rType, R, V));
     

	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


#ifndef NDEBUG
void FormulaAgent::checkIntegrity() const { assert(f); }
#endif
