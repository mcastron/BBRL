
#include "CModel.h"

using namespace std;
using namespace dds;


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
CModel::CModel(std::istream& is) : Model()
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


CModel::CModel(string name,
			unsigned int nX, unsigned int nU,
			int iniState,
			const vector<double>& N_,
			RewardType rType,
			const vector<double>& R, const vector<double>& V) :
				Model(	name, nX, nU, iniState, computeP(nX, nU, N_),
						rType, R, V),
				N(N_)
{
	//	Initialize 'Np'
	Np.resize(nX * nU);
	for (unsigned int x = 0; x < nX; ++x)
		for (unsigned int u = 0; u < nU; ++u)
			for (unsigned int y = 0; y < nX; ++y)
				Np[nU*x + u] += N[nX*nU*x + nX*u + y];


	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


// ===========================================================================
//	Public methods
// ===========================================================================
void CModel::update(unsigned int x, unsigned int u, unsigned int y, double)
{
	assert(x < nX);
	assert(u < nU);
	assert(y < nX);
	
	
	//	Update the counters matrices
	updateN(x, u, y);
	
	
	//	Update the transition matrix
	updateP(x, u);

	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void CModel::updateN(unsigned int x, unsigned int u, unsigned int y)
{
	assert(x < nX);
	assert(u < nU);
	assert(y < nX);
	
	
	N[nX*nU*x + nX*u + y] += 1.0;
	Np[nU*x + u] += 1.0;
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}
		

void CModel::updateP()
{
	for (unsigned int x = 0; x < nX; ++x)
		for (unsigned int u = 0; u < nU; ++u) { updateP(x, u); }
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void CModel::updateP(unsigned int x, unsigned int u)
{
	assert(x < nX);
	assert(u < nU);
	
	
	for (unsigned int y = 0; y < nX; ++y)
	{
		assert(Np[nU*x + u] > 0.0);
		
		P[nX*nU*x + nX*u + y]
				= (N[nX*nU*x + nX*u + y] / Np[nU*x + u]);
				
		if (N[nX*nU*x + nX*u + y]  != 0)
			connexityMap[nU*x + u].insert(y);
	}
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void CModel::serialize(ostream& os) const
{
	Model::serialize(os);
	
	
	os << CModel::toString() << "\n";
	os << (N.size() + Np.size()) << "\n";
	
	
	//	'N'
	for (unsigned int u = 0; u < nU; ++u)
		for (unsigned int x = 0; x < nX; ++x)
		{
			for (unsigned int y = 0; y < nX; ++y)
			{
				os << N[nX*nU*x + nX*u + y];
				if (y < (nX - 1))	{ os << "\t"; }
				else				{ os << "\n"; }
			}
		}
	
	
	//	'Np'
	for (unsigned int x = 0; x < nX; ++x)
		for (unsigned int u = 0; u < nU; ++u) { os << Np[nU*x + u] << "\n"; }
}


void CModel::deserialize(istream& is) throw (SerializableException)
{
	Model::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != CModel::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());
	
	int i = 0;
	
	
	//	'N'
	N.resize(nX * nU * nX);
	for (unsigned int u = 0; u < nU; ++u)
	{		
		for (unsigned int x = 0; x < nX; ++x)
		{
			for (unsigned int y = 0; y < (nX - 1); ++y)
			{
				if (!getline(is, tmp, '\t')) { throwEOFMsg("N value"); }
				N[nX*nU*x + nX*u + y] = atof(tmp.c_str());
				++i;
			}
			
			if (!getline(is, tmp)) { throwEOFMsg("N value"); }
			N[nX*nU*x + nX*u + (nX - 1)] = atof(tmp.c_str());
			++i;
		}
	}
	
	
	//	'Np'
	Np.resize(nX * nU);
	for (unsigned int j = 0; j < Np.size(); ++j)
	{
		if (!getline(is, tmp)) { throwEOFMsg("Np value"); }
		Np[j] = atof(tmp.c_str());
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
//	Private static methods
// ===========================================================================
vector<double> CModel::computeP(
		unsigned int nX, unsigned int nU, const vector<double>& N)
{
	vector<double> P(nX * nU * nX);
	for (unsigned int x = 0; x < nX; ++x)
		for (unsigned int u = 0; u < nU; ++u)
		{
			double NSum = 0.0;
			for (unsigned int y = 0; y < nX; ++y)
				NSum += N[nX*nU*x + nX*u + y];
			
			assert(NSum > 0.0);
			
			for (unsigned int y = 0; y < nX; ++y)
				P[nX*nU*x + nX*u + y] = (N[nX*nU*x + nX*u + y] / NSum);
		}
		
	return P;
}


// ===========================================================================
//	Private methods
// ===========================================================================
#ifndef NDEBUG
void CModel::checkIntegrity() const
{
	assert(N.size() == (nX * nU * nX));
	assert(Np.size() == (nX * nU));
	
	for (unsigned int x = 0; x < nX; ++x)
		for (unsigned int u = 0; u < nU; ++u)
		{
			double NSum = 0.0;
			for (unsigned int y = 0; y < nX; ++y)
				NSum += N[nX*nU*x + nX*u + y];

			assert(fabs(Np[nU*x + u] - NSum) < ZERO_ACCURACY);
			assert(Np[nU*x + u] > 0.0);
			
		}
}
#endif
