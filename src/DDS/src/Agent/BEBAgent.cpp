
#include "BEBAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructors/Destructor
// ===========================================================================
BEBAgent::BEBAgent(std::istream& is) :
          Agent(), beb(0), simulator(0), samplerFact(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


BEBAgent::BEBAgent(double beta_) :
          beta(beta_), beb(0), simulator(0), samplerFact(0)
{
     stringstream sstr;
	sstr << "BEB (beta = " << setprecision(ceil(log10(beta) + 2)) << beta;
	sstr << ", no model)";
	setName(sstr.str());
     
     
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


BEBAgent::~BEBAgent()
{
     if (beb)            { delete beb;         }
     if (simulator)      { delete simulator;   }
     if (samplerFact)    { delete samplerFact; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int BEBAgent::getAction(int xt) const throw (AgentException)
{
	assert(beb);


	return beb->SelectAction(xt);
}


void BEBAgent::learnOnline(int x, int u, int y, double r)
											throw (AgentException)
{
     assert(beb);
     
     
	beb->Update(x, u, y, r);
}


void BEBAgent::reset() throw (AgentException)
{
     if (beb)            { delete beb;         }
     if (simulator)      { delete simulator;   }
     if (samplerFact)    { delete samplerFact; }
     
     BEB::PARAMS searchParamsBEB;
	searchParamsBEB.b = beta;
	
     
     unsigned int s = getMDP()->getCurrentState();     
     simulator      = new MDPSimulator(s, nX, nU, R, getGamma());     
     samplerFact    = new PCSamplerFactory(priorcountList);
     beb = new BEB(*simulator, searchParamsBEB, *samplerFact);
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void BEBAgent::freeData()
{
     if (beb)            { delete beb;         beb         = 0; }
     if (simulator)      { delete simulator;   simulator   = 0; }
     if (samplerFact)    { delete samplerFact; samplerFact = 0; }
     
     
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void BEBAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << BEBAgent::toString() << "\n";
     os << 5 << "\n";


	//  'beta'
	os << beta << "\n";
	
	
	//  'nX'
	os << nX << "\n";
	
	
	//  'nU'
	os << nU << "\n";
	
	
	//  'R'
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
	
	
	//  'priorcountList'
	for (unsigned int u = 0; u < nU; ++u)
		for (unsigned int x = 0; x < nX; ++x)
		{
			for (unsigned int y = 0; y < nX; ++y)
			{
				os << priorcountList[nX*nU*x + nX*u + y];
				if (y < (nX - 1))	{ os << "\t"; }
				else				{ os << "\n"; }
			}
		}
}


void BEBAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != BEBAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;


	//  'beta'
	if (!getline(is, tmp)) { throwEOFMsg("beta"); }
	beta = atof(tmp.c_str());
	++i;


	//  'nX'
	if (!getline(is, tmp)) { throwEOFMsg("nX"); }
	nX = atoi(tmp.c_str());
	++i;


	//  'nU'
	if (!getline(is, tmp)) { throwEOFMsg("nU"); }
	nU = atoi(tmp.c_str());
	++i;


	//  'R'
	R.resize(nX * nU * nX);
	for (unsigned int u = 0; u < nU; ++u)
	{		
		for (unsigned int x = 0; x < nX; ++x)
		{
			for (unsigned int y = 0; y < (nX - 1); ++y)
			{
				if (!getline(is, tmp, '\t')) { throwEOFMsg("R value"); }
				R[nX*nU*x + nX*u + y] = atof(tmp.c_str());
			}
			
			if (!getline(is, tmp)) { throwEOFMsg("R value"); }
			R[nX*nU*x + nX*u + (nX - 1)] = atof(tmp.c_str());
		}
	}
	++i;


	//  'priorcountList'
	priorcountList.resize(nX * nU * nX);
	for (unsigned int u = 0; u < nU; ++u)
	{		
		for (unsigned int x = 0; x < nX; ++x)
		{
			for (unsigned int y = 0; y < (nX - 1); ++y)
			{
				if (!getline(is, tmp, '\t'))
				     throwEOFMsg("priorcountList value");
				priorcountList[nX*nU*x + nX*u + y] = atof(tmp.c_str());
			}
			
			if (!getline(is, tmp)) { throwEOFMsg("priorcountList value"); }
			priorcountList[nX*nU*x + nX*u + (nX - 1)] = atof(tmp.c_str());
		}
	}
	++i;
     
     
     //   'BEB', 'simulator' and 'samplerFact'
     if (beb)         { delete beb;         beb         = 0; }
     if (simulator)   { delete simulator;   simulator   = 0; }
     if (samplerFact) { delete samplerFact; samplerFact = 0; }
     
	
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
void BEBAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
											throw (AgentException)
{
	//	'DirMultiDistribution' case
	try
	{
		const DirMultiDistribution* dirDistrib = 
				dynamic_cast<const DirMultiDistribution*>(mdpDistrib);

		nX = dirDistrib->getNbStates();
		nU = dirDistrib->getNbActions();
		R = dirDistrib->getR();
		priorcountList = dirDistrib->getTheta();		
		stringstream sstr;
		sstr << "BEB (beta = " << setprecision(ceil(log10(beta) + 2)) << beta;
		sstr << ", " << dirDistrib->getShortName() << ")";
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
void BEBAgent::checkIntegrity() const
{
	assert(beta > 0.0);
}
#endif
