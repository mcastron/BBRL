
#include "SoftMaxAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
SoftMaxAgent::SoftMaxAgent(std::istream& is) :
          Agent(), iniModel(0), cModel(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


SoftMaxAgent::SoftMaxAgent(double tau_, CModel* iniModel_) :
		Agent(), tau(tau_), iniModel(iniModel_), cModel(0)
{
	assert(tau > 0.0);
	
	stringstream sstr;
	sstr << "Soft-Max (" << tau << ", no model)";
	setName(sstr.str());
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


SoftMaxAgent::~SoftMaxAgent()
{
	if (iniModel)	{ delete iniModel; }
	if (cModel)	{ delete cModel; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int SoftMaxAgent::getAction(int xt) const throw (MDPException)
{
	assert((xt >= 0) && (xt < (int) nX));
	assert(Q.size() == (getMDP()->getNbStates() * getMDP()->getNbActions()));


	vector<double> smScoreList(nU);
	for (unsigned int u = 0; u < nU; ++u)
		smScoreList[u] = exp(Q[nU*xt + u] / tau);
	
	
	double f = RandomGen::rand01_Uniform();
	double cumul = 0.0;
	int u = 0;
	for (; u < (int) nU; ++u)
	{
		cumul += smScoreList[u];
		if (f <= cumul) { break; }
	}
	
	
	return u;
}


void SoftMaxAgent::learnOnline(int x, int u, int y, double r)
											throw (MDPException)
{
	assert(cModel);
	
	cModel->update(x, u, y, r);
	Q = cModel->qIteration(getGamma(), getT(), Q);
}


void SoftMaxAgent::reset() throw (MDPException)
{
	nX = getMDP()->getNbStates();
	nU = getMDP()->getNbActions();
	
	
	assert(iniModel);
	
	if (cModel) { delete cModel; }
	cModel = iniModel->clone();
	Q = cModel->qIteration(getGamma(), getT());
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void SoftMaxAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << SoftMaxAgent::toString() << "\n";
	os << 2 << "\n";
	
	
	//	'tau'
	os << tau << "\n";
	
	
	//	'iniModel'
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


void SoftMaxAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != SoftMaxAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
	//	'epsilon'
	if (!getline(is, tmp)) { throwEOFMsg("tau"); }
	tau = atof(tmp.c_str());
	++i;
	
	
	//	'iniModel'
	if (iniModel) { delete iniModel; iniModel = 0; }
		
	if (!getline(is, tmp)) { throwEOFMsg("iniModel"); }
	int hasIniModel = atoi(tmp.c_str());
	
	if (!hasIniModel) { iniModel = 0; ++i; }
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
		++i;
	}
	
	
	//	'cModel'
	if (cModel) { delete cModel; cModel = 0; }
	
	
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
void SoftMaxAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
											throw (AgentException)
{
	if (iniModel) { return; }

	
	//	'DirMultiDistribution' case
	try
	{
		const DirMultiDistribution* dirDistrib = 
				dynamic_cast<const DirMultiDistribution*>(mdpDistrib);
		
		
		unsigned int nX = dirDistrib->getNbStates();
		unsigned int nU = dirDistrib->getNbActions();
		int iniState = dirDistrib->getIniState();
		const vector<double>& N = dirDistrib->getTheta();
		RewardType rType = dirDistrib->getRType();
		const vector<double>& R = dirDistrib->getR();
		const vector<double>& V = dirDistrib->getV();
		
		iniModel = new CModel(
				dirDistrib->getName(), nX, nU, iniState, N, rType, R, V);
		
		
		stringstream sstr;
		sstr << "Soft-Max (" << tau << ", ";
		sstr << iniModel->getName() << ")";
		setName(sstr.str());
	}
	
	
	//	Other cases
	catch (bad_cast)
	{
		string msg;
		msg += "Unsupported MDPDistribution for offline learning!\n";
		
		throw AgentException(msg);
	}
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


#ifndef NDEBUG
void SoftMaxAgent::checkIntegrity() const
{
	assert(tau > 0.0);
	assert(iniModel || (!iniModel && !cModel));
}
#endif
