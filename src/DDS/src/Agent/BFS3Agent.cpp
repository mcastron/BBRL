
#include "BFS3Agent.h"

using namespace std;
using namespace dds;
using namespace utils;


// ===========================================================================
//	Public Constructors/Destructor
// ===========================================================================
BFS3Agent::BFS3Agent(std::istream& is) :
          Agent(), bfs3(0), simulator(0), samplerFact(0)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


BFS3Agent::BFS3Agent(unsigned int K_, unsigned int C_, unsigned int D_) :
          K(K_), C(C_), D(D_), bfs3(0), simulator(0), samplerFact(0)
{
     stringstream sstr;
	sstr << "BFS3 (K = " << K << ", C = " << C;
	if (D > 0) { sstr << ", depth = " << D; }
	sstr << ", no model)";
	setName(sstr.str());
     
     
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


BFS3Agent::~BFS3Agent()
{
     if (bfs3)           { delete bfs3;        }
     if (simulator)      { delete simulator;   }
     if (samplerFact)    { delete samplerFact; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int BFS3Agent::getAction(int xt) const throw (AgentException)
{
	assert(bfs3);
	
	
	return bfs3->SelectAction(xt);
}


void BFS3Agent::learnOnline(int x, int u, int y, double r)
											throw (AgentException)
{
     assert(bfs3);
     
     
	bfs3->Update(x, u, y, r);
}


void BFS3Agent::reset() throw (AgentException)
{
     if (bfs3)           { delete bfs3;        }
     if (simulator)      { delete simulator;   }
     if (samplerFact)    { delete samplerFact; }
     
     BFS3::PARAMS searchParamsBFS3;
	searchParamsBFS3.D = ((D == 0) ? getT() : D);
	searchParamsBFS3.Vmin = 0;	
	searchParamsBFS3.Vmax = (maxR / (1.0 - getGamma()));
	searchParamsBFS3.C = C;
	searchParamsBFS3.N = std::max(1, (int) (K / searchParamsBFS3.C));
	searchParamsBFS3.gamma = getGamma();
	
     
     unsigned int s = getMDP()->getCurrentState();     
     simulator      = new MDPSimulator(s, nX, nU, R, getGamma());     
     samplerFact    = new PCSamplerFactory(priorcountList);
     bfs3           = new BFS3(*simulator, searchParamsBFS3, *samplerFact);
	
	
	//	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void BFS3Agent::freeData()
{
     if (bfs3)           { delete bfs3;        bfs3        = 0; }
     if (simulator)      { delete simulator;   simulator   = 0; }
     if (samplerFact)    { delete samplerFact; samplerFact = 0; }
     
     
     //	Check integrity
	#ifndef NDEBUG
	checkIntegrity();
	#endif
}


void BFS3Agent::serialize(ostream& os) const
{
	Agent::serialize(os);
	
	
	os << BFS3Agent::toString() << "\n";
     os << 8 << "\n";


	//  'K'
	os << K << "\n";


	//  'C'
	os << C << "\n";


	//  'D'
	os << D << "\n";


	//  'maxR'
	os << maxR << "\n";
	
	
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


void BFS3Agent::deserialize(istream& is) throw (SerializableException)
{
	Agent::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != BFS3Agent::toString())
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


	//  'C'
	if (!getline(is, tmp)) { throwEOFMsg("C"); }
	C = atoi(tmp.c_str());
	++i;


	//  'D'
	if (!getline(is, tmp)) { throwEOFMsg("D"); }
	D = atoi(tmp.c_str());
	++i;


	//  'maxR'
	if (!getline(is, tmp)) { throwEOFMsg("maxR"); }
	maxR = atof(tmp.c_str());
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
     
     
     //   'BFS3', 'simulator' and 'samplerFact'
     if (bfs3)        { delete bfs3;        bfs3        = 0; }
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
void BFS3Agent::learnOffline_aux(const MDPDistribution* mdpDistrib)
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
	     maxR = *(search::max<vector<double> >(R.begin(), R.end())[0]);
		
		stringstream sstr;
		sstr << "BFS3 (K = " << K << ", C = " << C;
		if (D > 0) { sstr << ", depth = " << D; }
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
void BFS3Agent::checkIntegrity() const
{
	assert(K > 0);
	assert(C > 0);
}
#endif
