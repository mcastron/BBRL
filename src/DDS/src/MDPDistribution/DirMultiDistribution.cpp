
#include "DirMultiDistribution.h"

using namespace std;
using namespace dds;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
DirMultiDistribution:: DirMultiDistribution(std::istream& is) :
		MDPDistribution()
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


DirMultiDistribution::DirMultiDistribution(
		string name,
		unsigned int nX_, unsigned int nU_,
		int iniState_,
		const vector<double>& theta_,
		RewardType rType_,
		const vector<double>& R_, const vector<double>& V_) :
			MDPDistribution(name),
			nX(nX_), nU(nU_), iniState(iniState_),
			theta(theta_), rType(rType_), R(R_), V(V_)
{
	assert((iniState == -1) || ((iniState >= 0) && (iniState < (int) nX)));
	assert(theta.size() == (nX * nU * nX));
	assert(R.size() == (nX * nU * nX));
	assert(V.empty() || (V.size() == (nX * nU * nX)));
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


// ===========================================================================
//	Public methods
// ===========================================================================
MDP* DirMultiDistribution::draw() const
{
	vector<double> P(nX * nU * nX);
	
	for (unsigned int x = 0; x < nX; ++x)
		for (unsigned int u = 0; u < nU; ++u)
		{
			utils::statistics::sampleDirichlet(
					(nX*nU*x + nX*u), theta,
					(nX*nU*x + nX*u), P,
					nX);
		}

	return (new MDP("", nX, nU, iniState, P, rType, R, V));
}


void DirMultiDistribution::update(
		unsigned int x, unsigned int u, unsigned int y, double inc)
{
	assert(x < nX);
	assert(u < nU);
	assert(y < nX);
	assert(inc >= 0.0);
	
	
	theta[nX*nU*x + nX*u + y] += inc;
}


void DirMultiDistribution::serialize(ostream& os) const
{
	MDPDistribution::serialize(os);
	
	
	os << DirMultiDistribution::toString() << "\n";
	os << (3 + theta.size() + 1 + R.size() + 1 + V.size()) << "\n";

	
	//	'nX', 'nU', 'xt'
	os << nX << "\n";
	os << nU << "\n";
	os << iniState << "\n";


	//	'theta'
	for (unsigned int u = 0; u < nU; ++u)
		for (unsigned int x = 0; x < nX; ++x)
		{
			for (unsigned int y = 0; y < nX; ++y)
			{
				os << theta[nX*nU*x + nX*u + y];
				if (y < (nX - 1))	{ os << "\t"; }
				else				{ os << "\n"; }
			}
		}

	
	//	'rType', 'R', 'V'
	os << rType << "\n";	
	for (unsigned int u = 0; u < nU; ++u)
		for (unsigned int x = 0; x < nX; ++x)
		{
			for (unsigned int y = 0; y < nX; ++y)
			{
				os << R[nX*nU*x + nX*u + y];
				if (y < (nX - 1))	{ os << "\t"; }
				else				{ os << "\n"; }
			}
		}

	os << !V.empty() << "\n";
	if (!V.empty())
	{
		for (unsigned int u = 0; u < nU; ++u)
			for (unsigned int x = 0; x < nX; ++x)
			{
				for (unsigned int y = 0; y < nX; ++y)
				{
					os << V[nX*nU*x + nX*u + y];
					if (y < (nX - 1))	{ os << "\t"; }
					else				{ os << "\n"; }
				}
			}
	}
}


void DirMultiDistribution::deserialize(istream& is) throw (SerializableException)
{
	MDPDistribution::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != DirMultiDistribution::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	
	int i = 0;
	
	
	//	'nX', 'nU', 'xt'
	if (!getline(is, tmp)) { throwEOFMsg("nX"); }
	nX = atoi(tmp.c_str());
	++i;
	
	if (!getline(is, tmp)) { throwEOFMsg("nU"); }
	nU = atoi(tmp.c_str());
	++i;
	
	if (!getline(is, tmp)) { throwEOFMsg("iniState"); }
	iniState = atoi(tmp.c_str());
	++i;
	
	
	//	'theta'
	theta.resize(nX * nU * nX);
	for (unsigned int u = 0; u < nU; ++u)
	{		
		for (unsigned int x = 0; x < nX; ++x)
		{
			for (unsigned int y = 0; y < (nX - 1); ++y)
			{
				if (!getline(is, tmp, '\t')) { throwEOFMsg("Theta value"); }
				theta[nX*nU*x + nX*u + y] = atof(tmp.c_str());
				++i;
			}
			
			if (!getline(is, tmp)) { throwEOFMsg("Theta value"); }
			theta[nX*nU*x + nX*u + (nX - 1)] = atof(tmp.c_str());
			++i;
		}
	}


	//	'rType', 'R', 'V'
	if (!getline(is, tmp)) { throwEOFMsg("rType"); }
	rType = (RewardType) atoi(tmp.c_str());	
	try					{ MDP::checkRewardType(rType);			}
	catch (MDPException e)	{ throw SerializableException(e.what());	}
	++i;
	
	
	R.resize(nX * nU * nX);
	for (unsigned int u = 0; u < nU; ++u)
	{		
		for (unsigned int x = 0; x < nX; ++x)
		{
			for (unsigned int y = 0; y < (nX - 1); ++y)
			{
				if (!getline(is, tmp, '\t')) { throwEOFMsg("R value"); }
				R[nX*nU*x + nX*u + y] = atof(tmp.c_str());
				++i;
			}
			
			if (!getline(is, tmp)) { throwEOFMsg("R value"); }
			R[nX*nU*x + nX*u + (nX - 1)] = atof(tmp.c_str());
			++i;
		}
	}
	
	if (!getline(is, tmp)) { throwEOFMsg("has a V matrix"); }
	bool hasV = atoi(tmp.c_str());
	++i;
	
	if (hasV)
	{
		V.resize(nX * nU * nX);
		for (unsigned int u = 0; u < nU; ++u)
		{		
			for (unsigned int x = 0; x < nX; ++x)
			{
				for (unsigned int y = 0; y < (nX - 1); ++y)
				{
					if (!getline(is, tmp, '\t'))
						throwEOFMsg("V value");

					V[nX*nU*x + nX*u + y] = atof(tmp.c_str());
					++i;
				}
				
				if (!getline(is, tmp)) { throwEOFMsg("V value"); }
				V[nX*nU*x + nX*u + (nX - 1)] = atof(tmp.c_str());
				++i;
			}
		}
	}
	else { V.clear(); }
	
	
	//	Number of parameters check
	if (n != i)
	{
		cout << n << "\t" << i << "\n";
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
#ifndef NDEBUG
void DirMultiDistribution::checkIntegrity() const
{
	assert(nX > 0);
	assert(nU > 0);
	
	assert(theta.size() == (nX * nU * nX));
	
	try					{ MDP::checkRewardType(rType);	}
	catch (MDPException e)	{ assert(false);				}
	assert(R.size() == (nX * nU * nX));
	assert(V.empty() || (V.size() == (nX * nU * nX)));
	
	for (unsigned int x = 0; x < nX; ++x)
		for (unsigned int u = 0; u < nU; ++u)
		{
			double thetaSum = 0.0;
			for (unsigned int y = 0; y < nX; ++y)
			{				
				thetaSum += theta[nX*nU*x + nX*u + y];
				
				if (!V.empty()) { assert(V[nX*nU*x + nX*u + y] >= 0.0); }
			}
			
			assert(thetaSum > 0.0);
		}
}
#endif
