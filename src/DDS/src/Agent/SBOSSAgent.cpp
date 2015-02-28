
#include "SBOSSAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructors/Destructor
// ===========================================================================
SBOSSAgent::SBOSSAgent(std::istream& is) :
          Agent(), sboss(0), simulator(0), samplerFact(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


SBOSSAgent::SBOSSAgent(unsigned int K_, double delta_) :
          K(K_), delta(delta_), sboss(0), simulator(0), samplerFact(0)
{
     stringstream sstr;
	sstr << "SBOSS (K = " << K << ", delta = ";
	sstr << setprecision(ceil(log10(delta) + 2)) << delta;
	sstr << ", no model)";
	setName(sstr.str());
     
     
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


SBOSSAgent::~SBOSSAgent()
{
     if (sboss)          { delete sboss;       }
     if (simulator)      { delete simulator;   }
     if (samplerFact)    { delete samplerFact; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int SBOSSAgent::getAction(int xt) const throw (AgentException)
{
	assert(sboss);


	return sboss->SelectAction(xt);
}


void SBOSSAgent::learnOnline(int x, int u, int y, double r)
											throw (AgentException)
{
     assert(sboss);
     
     
	sboss->Update(x, u, y, r);
}


void SBOSSAgent::reset() throw (AgentException)
{
     if (sboss)          { delete sboss;       }
     if (simulator)      { delete simulator;   }
     if (samplerFact)    { delete samplerFact; }
     
     SBOSS::PARAMS searchParamsSBOSS;
	searchParamsSBOSS.K = K;
	searchParamsSBOSS.delta = delta;
	
     
     unsigned int s = getMDP()->getCurrentState();     
     simulator      = new MDPSimulator(s, nX, nU, R, getGamma());     
     samplerFact    = new PCSamplerFactory(priorcountList);
     sboss = new SBOSS(*simulator, searchParamsSBOSS, *samplerFact);
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void SBOSSAgent::freeData()
{
     if (sboss)          { delete sboss;       sboss       = 0; }
     if (simulator)      { delete simulator;   simulator   = 0; }
     if (samplerFact)    { delete samplerFact; samplerFact = 0; }
     
     
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void SBOSSAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << SBOSSAgent::toString() << "\n";
     os << 6 << "\n";


	//  'K'
	os << K << "\n";


	//  'delta'
	os << delta << "\n";
	
	
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


void SBOSSAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != SBOSSAgent::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;


	//  'K'
	if (!getline(is, tmp)) { throwEOFMsg("K"); }
	K = atoi(tmp.c_str());
	++i;


	//  'delta'
	if (!getline(is, tmp)) { throwEOFMsg("delta"); }
	delta = atof(tmp.c_str());
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
     
     
     //   'SBOSS', 'simulator' and 'samplerFact'
     if (sboss)       { delete sboss;       sboss       = 0; }
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
void SBOSSAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
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
		sstr << "SBOSS (K = " << K << ", delta = ";
		sstr << setprecision(ceil(log10(delta) + 2)) << delta;
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
void SBOSSAgent::checkIntegrity() const
{
	assert(K > 0);
	assert(delta > 0.0);
}
#endif
