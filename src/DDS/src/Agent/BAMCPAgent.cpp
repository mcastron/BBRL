
/* TODO - replace "BAMCPAgent" the agent class name */


#include "BAMCPAgent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructors/Destructor
// ===========================================================================
BAMCPAgent::BAMCPAgent(unsigned int K_) :
          K(K_), bamcp(0), simulator(0), samplerFact(0)
{
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


BAMCPAgent::BAMCPAgent(std::istream& is) :
          Agent(), bamcp(0), simulator(0), samplerFact(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


BAMCPAgent::~BAMCPAgent()
{
     if (bamcp)          { delete bamcp;       }
     if (simulator)      { delete simulator;   }
     if (samplerFact)    { delete samplerFact; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int BAMCPAgent::getAction(int xt) const throw (MDPException)
{
	assert(bamcp);
	
	
	return bamcp->SelectAction(xt);
}


void BAMCPAgent::learnOnline(int x, int u, int y, double r)
											throw (MDPException)
{
     assert(bamcp);
     
     
	bamcp->Update(x, u, y, r);
}


void BAMCPAgent::reset() throw (MDPException)
{
     if (bamcp)          { delete bamcp;       bamcp       = 0; }
     if (simulator)      { delete simulator;   simulator   = 0; }
     if (samplerFact)    { delete samplerFact; samplerFact = 0; }
     
     BAMCP::PARAMS searchParamsBAMCP;
     searchParamsBAMCP.MaxDepth 			= getT();
	searchParamsBAMCP.NumSimulations 		= K;
	searchParamsBAMCP.ExplorationConstant 	= 3.0;
	searchParamsBAMCP.RB 				= -1;
	searchParamsBAMCP.eps 				= 0.5;
     
     unsigned int s;
     if (iniState == -1) { s = RandomGen::randIntRange_Uniform(0, (nX - 1)); }
     else                { s = iniState;                                     }
     
     simulator = new MDPSimulator(s, nX, nU, R, getGamma());
     samplerFact = new FDMsamplerFactory(0.0);
     bamcp = new BAMCP(
               *simulator, searchParamsBAMCP, *samplerFact, priorcountList);
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void BAMCPAgent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << BAMCPAgent::toString() << "\n";
     os << 6 << "\n";
	
	//  'K'
	os << K << "\n";
	
	
	//  'iniState'
	os << iniState << "\n";
	
	
	//  'nX'
	os << nX << "\n";
	
	
	//  'nU'
	os << nU << "\n";
	
	
	//  'R'
	os << (nX*nU*nX) << "\n";
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
	os << (nX*nU*nX) << "\n";
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


void BAMCPAgent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != BAMCPAgent::toString())
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


	//  'iniState'
	if (!getline(is, tmp)) { throwEOFMsg("iniState"); }
	iniState = atoi(tmp.c_str());
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
     
     
     //   'bamcp', 'simulator' and 'samplerFact'
     if (bamcp)       { delete bamcp;       bamcp       = 0; }
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
void BAMCPAgent::learnOffline_aux(const MDPDistribution* mdpDistrib)
											throw (AgentException)
{
	//	'DirMultiDistribution' case
	try
	{
		const DirMultiDistribution* dirDistrib = 
				dynamic_cast<const DirMultiDistribution*>(mdpDistrib);
				
		iniState = dirDistrib->getIniState();
		nX = dirDistrib->getNbStates();
		nU = dirDistrib->getNbActions();
		R = dirDistrib->getR();
		priorcountList = dirDistrib->getTheta();
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
void BAMCPAgent::checkIntegrity() const
{
	assert(K > 0);
}
#endif
