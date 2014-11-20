
#include "VDBEEGreedyAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
VDBEEGreedyAgent::VDBEEGreedyAgent(std::istream& is) :
          Agent(), iniModel(0), cModel(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


VDBEEGreedyAgent::VDBEEGreedyAgent(
		double sigma_, double delta_, double iniEpsilon_,
		CModel* iniModel_) :
			Agent(),
			sigma(sigma_), delta(delta_), iniEpsilon(iniEpsilon_),
			iniModel(iniModel_), cModel(0)
{
	assert(sigma > 0.0);
	assert((delta >= 0.0) && (delta < 1.0));
	assert((iniEpsilon_ >= 0.0) && (iniEpsilon_ <= 1.0));
	assert(iniModel);
	
	
	stringstream sstr;
	sstr << "VDBE e-Greedy (" << sigma << ", " << delta << ", no model)";
	setName(sstr.str());
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


VDBEEGreedyAgent::~VDBEEGreedyAgent()
{
	if (iniModel)	{ delete iniModel; }
	if (cModel)	{ delete cModel;   }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int VDBEEGreedyAgent::getAction(int xt) const throw (MDPException)
{
	assert((xt >= 0) && (xt < (int) nX));
	assert(Q.size() == (getMDP()->getNbStates() * getMDP()->getNbActions()));

	
	double epsilon = epsilonList[xt];
	double f = RandomGen::rand01_Uniform();
	
	//	'random' case
	if (f <= epsilon)
		return RandomGen::randIntRange_Uniform(0, (nU - 1));

	//	'greedy' case
	else
	{		
		vector<vector<double>::const_iterator> maxList;
		maxList = utils::search::max<vector<double> >(
				(Q.begin() + nU*xt), (Q.begin() + nU*xt + nU));
		
		assert(!maxList.empty());

		int maxI = RandomGen::randIntRange_Uniform(0, (maxList.size() - 1));
		return (maxList[maxI] - (Q.begin() + nU*xt));
	}
}


void VDBEEGreedyAgent::learnOnline(int x, int u, int y, double r)
											throw (MDPException)
{
	assert(cModel);
	
	
	vector<double> pQ = Q;
	
	cModel->update(x, u, y, r);
	Q = cModel->qIteration(getGamma(), getT(), pQ);
	
	
	for (unsigned int i = 0; i < nX; ++i)
	{
		epsilonList[i] = ((delta * f(x, u, sigma, pQ, Q))
				+ ((1.0 - delta) * epsilonList[i]));
	}
}


void VDBEEGreedyAgent::reset() throw (MDPException)
{
	nX = getMDP()->getNbStates();
	nU = getMDP()->getNbActions();
	
	
	epsilonList.resize(nX);
	for (unsigned int x = 0; x < nX; ++x)
		epsilonList[x] = iniEpsilon;
	
	
	assert(iniModel);
	
	if (cModel) { delete cModel; }
	cModel = iniModel->clone();
	Q = cModel->qIteration(getGamma(), getT());
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void VDBEEGreedyAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << VDBEEGreedyAgent::toString() << "\n";
	os << 4 << "\n";
	
	
	//	'sigma'
	os << sigma << "\n";


	//	'delta'
	os << delta << "\n";


	//	'iniEpsilon'
	os << iniEpsilon << "\n";
	
	
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


void VDBEEGreedyAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != VDBEEGreedyAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
	//	'sigma'
	if (!getline(is, tmp)) { throwEOFMsg("sigma"); }
	sigma = atof(tmp.c_str());
	++i;
	
	
	//	'delta'
	if (!getline(is, tmp)) { throwEOFMsg("delta"); }
	delta = atof(tmp.c_str());
	++i;
	
	
	//	'iniEpsilon'
	if (!getline(is, tmp)) { throwEOFMsg("iniEpsilon"); }
	iniEpsilon = atof(tmp.c_str());
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
void VDBEEGreedyAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
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
		sstr << "VDBE e-Greedy (" << sigma << ", " << delta << ", ";
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
void VDBEEGreedyAgent::checkIntegrity() const
{
	assert(sigma > 0.0);
	assert((delta >= 0.0) && (delta < 1.0));
	assert((iniEpsilon >= 0.0) && (iniEpsilon <= 1.0));
	assert(iniModel);
}
#endif
