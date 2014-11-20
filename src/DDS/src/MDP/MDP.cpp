
#include "MDP.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
MDP::MDP(std::istream& is) : IMDP()
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


MDP::MDP(	string name,
		unsigned int nX_, unsigned int nU_,
		int iniState_,
		const vector<double>& P_,
		RewardType rType_,
		const vector<double>& R_, const vector<double>& V_) :
			IMDP(name),
			nX(nX_), nU(nU_), iniState(iniState_),
			P(P_), rType(rType_), R(R_), V(V_),
			known(true)
{
	assert((iniState == -1) || ((iniState >= 0) && (iniState < (int) nX)));
	assert(P.size() == (nX * nU * nX));
	assert(R.size() == (nX * nU * nX));
	assert(V.empty() || (V.size() == (nX * nU * nX)));
	
	
	//	'connexityMap'
	connexityMap = computeConnexityMap(nX, nU, P);
	
	
	//	Reset
	reset();
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


// ===========================================================================
//	Public static methods
// ===========================================================================
void MDP::checkRewardType(const RewardType rType_) throw (MDPException)
{
	switch (rType_)
	{
		case RT_CONSTANT:
		case RT_GAUSSIAN:
			break;
		
		default:
		{
			std::string msg;
			msg += "Invalid RewardType provided!\n";
			
			throw MDPException(msg);
		}
	}
}


// ===========================================================================
//	Public methods
// ===========================================================================
void MDP::perform(unsigned int u, unsigned int& y, double& r)
{
	assert(u < nU);
	
	
	//	Compute 'y'
	double s = RandomGen::rand01_Uniform();
	double cumul = 0.0;
	
	for (y = 0; y < nX; ++y)
	{
		cumul += P[nX*nU*xt + nX*u + y];
		if (s <= cumul) { break; }
	}
	y %= nX;	//	WARNING:	In rare cases, due to imprecision in number
			//			representation, the sum of probabilities could
			//			be slightly different to 1.0.
			//			(under ZERO_ACCURACY)
			//
			//			When this sum is less than 1.0, it is possible
			//			to draw a value 's' higher than this sum.
			//
			//			We decided to ignore this error and let the
			//			agent go to state 'nX - 1'.
	
	
	//	Compute 'r'
	switch (rType)
	{
		case RT_CONSTANT:
			r = R[nX*nU*xt + nX*u + y];
			break;
		
		case RT_GAUSSIAN:
		{
			double var = V.empty() ? 0.0 : V[nX*nU*xt + nX*u + y];
			r = RandomGen::rand_Gaussian(R[nX*nU*xt + nX*u + y], sqrt(var));
			break;
		}
	}
	
	
	//	'y' becomes the new 'xt'
	xt = y;


	assert(xt < nX);
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void MDP::reset()
{
	if (iniState != -1) { xt = iniState; }
	else { xt = RandomGen::randIntRange_Uniform(0, (nX - 1)); }
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


vector<double> MDP::valueIteration(double gamma, unsigned int T,
						vector<double> pV) const throw (MDPException)
{
	if (!isKnown())
	{
		std::string msg;
		msg += "Cannot use Value Iteration algorithm on an unknown MDP!\n";
		
		throw MDPException(msg);
	}
	
	assert((gamma > 0.0) && (gamma < 1.0));
	assert(T > 0);


	if (pV.size() != nX) { pV.clear(); pV.resize(nX); }
	vector<double> V(nX);
	
	bool hasConverged;
	unsigned int t = 0;
	do
	{		
		//	Save the previous 'V'
		for (unsigned int x = 0; x < nX; ++x) { pV[x] = V[x]; }
		
		
		//	Update 'V'
		hasConverged = true;
		for (unsigned int x = 0; x < nX; ++x)
		{
			//	Get 'max_u sum_y P(x, u, y) (R(x, u, y)+ gamma pV(y))'
			double max = 0.0;
			for (unsigned int y = 0; y < nX; ++y)
			{
				unsigned int i = (nX*nU*x + y);
				max += (P[i] * (R[i] + (gamma * pV[y])));
			}
			
			
			for (unsigned int u = 1; u < nU; ++u)
			{
				//	Compute the current value
				double cur = 0.0;
				for (unsigned int y = 0; y < nX; ++y)
				{
					unsigned int i = (nX*nU*x + nX*u + y);
					cur += (P[i] * (R[i] + (gamma * pV[y])));
				}
				
				
				//	Update the maximum
				if (max < cur) { max = cur; }
			}
			
			
			//	Update 'V(x)'
			V[x] = max;
			
			
			//	Check if 'V(x)' has converged
			if (fabs(V[x] - pV[x]) > ACCURACY) { hasConverged = false; }
		}
		
	}
	while ((t++ < T) && !hasConverged);
	
	return V;
}


vector<double> MDP::qIteration(double gamma, unsigned int T,
						vector<double> pQ) const throw (MDPException)
{
	if (!isKnown())
	{
		std::string msg;
		msg += "Cannot use Q-Iteration algorithm on an unknown MDP!\n";
		
		throw MDPException(msg);
	}
	
	assert((gamma > 0.0) && (gamma < 1.0));
	assert(T > 0);
	

	if (pQ.size() != (nX * nU)) { pQ.clear(); pQ.resize(nX * nU); }
	vector<double> Q(nX * nU);
	
	bool hasConverged;
	unsigned int t = 0;
	do
	{		
		//	Save the previous 'Q'
		for (unsigned int x = 0; x < nX; ++x)
			for (unsigned int u = 0; u < nU; ++u)
				pQ[nU*x + u] = Q[nU*x + u];
		
		
		//	Update 'Q'
		hasConverged = true;
		for (unsigned int x = 0; x < nX; ++x)
			for (unsigned int u = 0; u < nU; ++u)
			{
				//	Compute 'Q(x, u)'
				unsigned int i = (nU*x + u);
				Q[i] = 0.0;
				

				set<unsigned int>::iterator it, end;
				it	= connexityMap[nU*x + u].begin();
				end	= connexityMap[nU*x + u].end();
				for (unsigned int y = *it; it != end; ++it, y = *it)
				{					
					//	'max_v pQ(y, v)'
					double maxV = pQ[nU*y];
					for (unsigned int v = 1; v < nU; ++v)
						if (maxV < pQ[nU*y + v]) { maxV = pQ[nU*y + v]; }
					
					
					//	Update 'Q(x, u)'
					unsigned int j = (nX*nU*x + nX*u + y);
					Q[i] += (P[j] * (R[j] + (gamma * maxV)));
				}
				
				
				//	Check if 'Q(x, u)' has converged
				if (fabs(Q[i] - pQ[i]) > ACCURACY) { hasConverged = false; }
			}
	}
	while ((t++ < T) && !hasConverged);
	
	return Q;
}


void MDP::serialize(ostream& os) const
{
	IMDP::serialize(os);
	
	
	os << MDP::toString() << "\n";
	os << (3 + P.size() + 1 + R.size() + 1 + V.size()) << "\n";


	//	'nX', 'nU', 'xt'
	os << nX << "\n";
	os << nU << "\n";
	os << iniState << "\n";


	//	'P'
	for (unsigned int u = 0; u < nU; ++u)
		for (unsigned int x = 0; x < nX; ++x)
		{
			for (unsigned int y = 0; y < nX; ++y)
			{
				os << P[nX*nU*x + nX*u + y];
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


void MDP::deserialize(istream& is) throw (SerializableException)
{
	IMDP::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != MDP::toString())
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
	
	
	//	'P'
	P.resize(nX * nU * nX);
	for (unsigned int u = 0; u < nU; ++u)
	{		
		for (unsigned int x = 0; x < nX; ++x)
		{
			for (unsigned int y = 0; y < (nX - 1); ++y)
			{
				if (!getline(is, tmp, '\t')) { throwEOFMsg("P value"); }
				P[nX*nU*x + nX*u + y] = atof(tmp.c_str());
				++i;
			}
			
			if (!getline(is, tmp)) { throwEOFMsg("P value"); }
			P[nX*nU*x + nX*u + (nX - 1)] = atof(tmp.c_str());
			++i;
		}
	}


	//	'rType', 'R', 'V'
	if (!getline(is, tmp)) { throwEOFMsg("rType"); }
	rType = (RewardType) atoi(tmp.c_str());	
	try { checkRewardType(rType); }
	catch (MDPException e) { throw SerializableException(e.what()); }
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
	
	
	//	'connexityMap'
	connexityMap = computeConnexityMap(nX, nU, P);
	
	
	//	'known'
	known = true;
	
	
	//	Reset
	reset();
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


// ===========================================================================
//	Private methods
// ===========================================================================
vector<set<unsigned int> > MDP::computeConnexityMap(
		unsigned int nX, unsigned int nU, const vector<double>& P)
{
	vector<set<unsigned int> > connexityMap(nX*nU);
	
	for (unsigned int x = 0; x < nX; ++x)
		for (unsigned int u = 0; u < nU; ++u)
		{
			for (unsigned int y = 0; y < nX; ++y)
				if (P[nX*nU*x + nX*u + y] > 0.0)
					connexityMap[nU*x + u].insert(y);
		}
	
	return connexityMap;
}


#ifndef NDEBUG
void MDP::checkIntegrity() const
{
	assert(nX > 0);
	assert(nU > 0);
	assert((iniState == -1) || ((iniState >= 0) && (iniState < (int) nX)));
	assert(xt < nX);
	
	assert(P.size() == (nX * nU * nX));
	
	try					{ checkRewardType(rType);	}
	catch (MDPException e)	{ assert(false);			}
	assert(R.size() == (nX * nU * nX));
	assert(V.empty() || (V.size() == (nX * nU * nX)));
	
	for (unsigned int x = 0; x < nX; ++x)
		for (unsigned int u = 0; u < nU; ++u)
		{
			double pSum = 0.0;
			for (unsigned int y = 0; y < nX; ++y)
			{
				assert(P[nX*nU*x + nX*u + y] >= 0.0
						&& P[nX*nU*x + nX*u + y] <= 1.0);
				
				assert((P[nX*nU*x + nX*u + y] == 0.0)
						|| (connexityMap[nU*x + u].find(y)
							!= connexityMap[nU*x + u].end()));
				
				pSum += P[nX*nU*x + nX*u + y];
				
				
				if (!V.empty()) { assert(V[nX*nU*x + nX*u + y] >= 0.0); }
			}

			assert(fabs(pSum - 1.0) < ZERO_ACCURACY);
		}
}
#endif
