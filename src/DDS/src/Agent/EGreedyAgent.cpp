
#include "EGreedyAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
EGreedyAgent::EGreedyAgent(std::istream& is) :
          Agent(), iniModel(0), cModel(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


EGreedyAgent::EGreedyAgent(double epsilon_, CModel* iniModel_) :
		Agent(), epsilon(epsilon_), iniModel(iniModel_), cModel(0)
{
	assert((epsilon_ >= 0.0) && (epsilon_ <= 1.0));
	

	stringstream sstr;
	sstr << "e-Greedy (" << setprecision(ceil(log10(epsilon) + 2)) << epsilon;
	if (iniModel) { sstr << ", " << iniModel->getName() << ")"; }
	else          { sstr << ", no model)";                      }
	setName(sstr.str());

	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


EGreedyAgent::~EGreedyAgent()
{
	if (iniModel)	{ delete iniModel; }
	if (cModel)	{ delete cModel;   }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int EGreedyAgent::getAction(int xt) const throw (AgentException)
{
	assert((xt >= 0) && (xt < (int) nX));
	assert(Q.size() == (getMDP()->getNbStates() * getMDP()->getNbActions()));

	
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


void EGreedyAgent::learnOnline(int x, int u, int y, double r)
											throw (AgentException)
{
	assert(cModel);
	
	cModel->update(x, u, y, r);
	Q = cModel->qIteration(getGamma(), getT(), Q);
}


void EGreedyAgent::reset() throw (AgentException)
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


void EGreedyAgent::freeData()
{
     if (cModel) { delete cModel; cModel = 0; }
     Q.clear();
     
     
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void EGreedyAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << EGreedyAgent::toString() << "\n";
	os << 2 << "\n";
	
	
	//	'epsilon'
	os << epsilon << "\n";
	
	
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


void EGreedyAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != EGreedyAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;
	
	
	//	'epsilon'
	if (!getline(is, tmp)) { throwEOFMsg("epsilon"); }
	epsilon = atof(tmp.c_str());
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
void EGreedyAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
											throw (AgentException)
{
	if (iniModel) { return; }

	
	//	'DirMultiDistribution' case
	const DirMultiDistribution* dirDistrib = 
				dynamic_cast<const DirMultiDistribution*>(mdpDistrib);

     if (dirDistrib)
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
				dirDistrib->getShortName(), nX, nU, iniState, N, rType,
				R, V);
		
		
		stringstream sstr;
		sstr << "e-Greedy (" << setprecision(ceil(log10(epsilon) + 2));
		sstr << epsilon << ", ";
		sstr << iniModel->getName() << ")";
		setName(sstr.str());
	}


	//	Other cases
     else
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
void EGreedyAgent::checkIntegrity() const
{
	assert((epsilon >= 0.0) && (epsilon <= 1.0));
	assert(iniModel || (!iniModel && !cModel));
}
#endif
