
#include "QCounterVar.h"
#include "FormulaAgent.h"

using namespace std;
using namespace dds;
using namespace simulation;
using namespace utils;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
QCounterVar::QCounterVar(std::istream& is)
{
	try							{ dDeserialize(is);	}
	catch (SerializableException e)	{ deserialize(is);	}
}


QCounterVar::QCounterVar(unsigned int nX_, unsigned int nU_,
		               const std::vector<double>& N_) :
		                       nX(nX_), nU(nU_), N(N_) {}


QCounterVar::~QCounterVar() {}


// ===========================================================================
//	Public static methods
// ===========================================================================
QCounterVar* QCounterVar::generate(unsigned int nX, unsigned int nU,
                                   const MDPDistribution* prior,
                                   unsigned int N,
                                   double gamma, unsigned int T,
                                   double mu, double sigma,
                                   unsigned int b, unsigned int np,
                                   double accuracy,
                                   double alpha, double beta,
                                   const vector<QCounterVar*>& varList)
{
     //   Initialize the gaussians
     vector<double> gMu(nX*nU*nX);
     vector<double> gSigma(nX*nU*nX);
     for (unsigned int i = 0; i < nX*nU*nX; ++i)
     {
          gMu[i]    = mu;
          gSigma[i] = sigma;
     }
     
     
     //   Initialize some parameters
     double maxSigma = sigma;
     
     
     //   Iterations
     unsigned int t = 0;
     double n = 1.0;
     while (maxSigma > accuracy)
     {
          cout << "(" << t++ << ")\n";

          
          //   Draw a population of 'p' individuals
          vector<vector<double> > pop;
          for (unsigned int i = 0; i < np; ++i)
          {
               vector<double> sample;
               for (unsigned int j = 0; j < nX*nU*nX; ++j)
               {
                    sample.push_back(
                         RandomGen::rand_Gaussian(gMu[j], gSigma[j]));
                    
                    if (sample.back() <  0.0) { sample.back() =  0.0; }
                    if (sample.back() > 10.0) { sample.back() = 10.0; }
               }

               pop.push_back(sample);
          }
          
          
          //   Evaluate each individual and put them in a queue
          vector<MDP*> mdps;
          for (unsigned int j = 0; j < N; ++j)
               mdps.push_back(prior->draw());
          
          priority_queue<pair<double, unsigned int> > pq;
          for (unsigned int i = 0; i < np; ++i)
          {
               //   Evaluate
               QCounterVar* var = new QCounterVar(nX, nU, pop[i]);
               double score = evaluate(var, prior, mdps, gamma, T,
                                       alpha, beta, varList);

               //   Free data           
               delete var;
               
               
               //   Push the results
               pq.push(pair<double, int>(score, i));
               
//               cout << (i / (double) np)*100 << "%\n";
          }
          
          for (unsigned int j = 0; j < N; ++j) { delete mdps[j]; }
          
          
          //   Retrieve the 'b' best individuals of the population
          vector<vector<double> > bestList;
          while (bestList.size() < b)
          {
               bestList.push_back(pop[pq.top().second]);
               pq.pop();
          }
          
          
          //   Update the gaussians
          for (unsigned int i = 0; i < nX*nU*nX; ++i)
          {
               vector<double> val;
               for (unsigned int j = 0; j < b; ++j)
                    val.push_back(bestList[j][i]);

               gMu[i]    = statistics::computeMean<double>(val);
               gSigma[i] =
                    statistics::computeStandardDeviation<double>(gMu[i], val);
               
               
               //   Update 'maxSigma'
               if (i == 0 || maxSigma < gSigma[i]) { maxSigma = gSigma[i]; }
          }
          cout << "max sigma: " << maxSigma << "\n\n";
     }
     
     
     return new QCounterVar(nX, nU, gMu);
}


// ===========================================================================
//	Public methods
// ===========================================================================
void QCounterVar::init(const dds::MDPDistribution* mdpDistrib)
                                                  throw (FVariableException)
{
     if (iniModel) { delete(iniModel); iniModel = 0; }

     
     //   'DirMultiDistribution' case
     const dds::DirMultiDistribution* dirDistrib = 
               dynamic_cast<const dds::DirMultiDistribution*>(mdpDistrib);

     if (dirDistrib
               && nX == dirDistrib->getNbStates()
               && nU == dirDistrib->getNbActions())
     {
          int iniState = dirDistrib->getIniState();
          RewardType rType = dirDistrib->getRType();
          const vector<double>& R = dirDistrib->getR();
          const vector<double>& V = dirDistrib->getV();

          iniModel = new dds::CModel(
                    "counter", nX, nU, iniState, N, rType, R, V);
     }


     //   Other case
     else
     {
          string msg;
          msg += "Unsupported MDPDistribution for offline ";
          msg += "learning!\n";
          
          throw FVariableException(msg);
     }
}


void QCounterVar::serialize(ostream& os) const
{
	QVar::serialize(os);
	
	
	os << QCounterVar::toString() << "\n";
	os << 3 << "\n";

	
	//  'nX'
	os << nX << "\n";
	
	
	//  'nU'
	os << nU << "\n";


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
}


void QCounterVar::deserialize(istream& is) throw (SerializableException)
{
	QVar::deserialize(is);
	
	string tmp;


	//	Class name check
	if (!getline(is, tmp)) { throwEOFMsg("class name"); }
	string className = tmp;
	if (className != QCounterVar::toString())
	{
		string msg = "Error with 'class name'.\n";
		throw SerializableException(msg);
	}
	
	
	//	Number of parameters
	if (!getline(is, tmp)) { throwEOFMsg("number of parameters"); }
	int n = atoi(tmp.c_str());

	int i = 0;


     //   'nX'
     if (!getline(is, tmp)) { throwEOFMsg("nX"); }
     nX = atoi(tmp.c_str());
     ++i;


     //   'nU'
     if (!getline(is, tmp)) { throwEOFMsg("nU"); }
     nU = atoi(tmp.c_str());
     ++i;
     
     
     //	'N'
	N.resize(nX * nU * nX);
	for (unsigned int u = 0; u < nU; ++u)
	{		
		for (unsigned int x = 0; x < nX; ++x)
		{
			for (unsigned int y = 0; y < (nX - 1); ++y)
			{
				if (!getline(is, tmp, '\t')) { throwEOFMsg("N"); }
				N[nX*nU*x + nX*u + y] = atof(tmp.c_str());
			}
			
			if (!getline(is, tmp)) { throwEOFMsg("N"); }
			N[nX*nU*x + nX*u + (nX - 1)] = atof(tmp.c_str());
		}
	}
	++i;


	//	Number of parameters check
	if (n != i)
	{
		string msg = "Error with 'number of parameters'.\n";
		throw SerializableException(msg);
	}
}


// ===========================================================================
//	Private static methods
// ===========================================================================
double QCounterVar::evaluate(QCounterVar* var,
                             const MDPDistribution* prior,
		                   const vector<MDP*>& mdps,
		                   double gamma, unsigned int T,
		                   double alpha, double beta,
		                   const std::vector<QCounterVar*>& varList)
{
     //   Score
          //   Agent
     vector<FVariable*> tmpVarList;
     tmpVarList.push_back(var);
     Agent* agent = new FormulaAgent(
               new formula::Formula("X0"), tmpVarList);
     agent->learnOffline(prior);


     //   Simulation
     vector<double> dsrList;
     for (unsigned int j = 0; j < mdps.size(); ++j)
     {
          SimulationRecord simRec
                    = simulate(agent, mdps[j], gamma, T);

          dsrList.push_back(simRec.computeDSR());
     }


     //   Compute the score
     double score = statistics::computeMean<double>(dsrList);


     //   Free
     delete agent;


     //   Distance
     double minDist = 0.0;
     for (unsigned int i = 0; i < varList.size(); ++i)
     {
          double curDist = getDistance(alpha, beta, var, varList[i]);
          if (i == 0 || minDist > curDist) { minDist = curDist; }
     }


     
     //   Return
//     cout << "score:   " << score << "\n";
//     cout << "minDist: " << minDist << "\n";
//     cout << "total:   " << (score + minDist) << "\n\n";
//     cout << (score / minDist) << endl;
     return score + minDist;
}


double QCounterVar::getDistance(double alpha, double beta,
                                QCounterVar* var1, QCounterVar* var2)
{
     unsigned int nX = var1->getNbStates();
     unsigned int nU = var1->getNbActions();
     
     const vector<double>& N1 = var1->getN();
     const vector<double>& N2 = var2->getN();
     
     
     //   Distance w.r.t. initialization
     double iniDist = 0.0;

          //   Normalize N1 and N2
     if (alpha != 0.0)
     {
          vector<double> normalizedN1, normalizedN2;
          for (unsigned int x = 0; x < nX; ++x)
          {
               for (unsigned int u = 0; u < nU; ++u)
               {
                    double sum1 = 0.0, sum2 = 0.0;
                    for (unsigned int y = 0; y < nX; ++y)
                    {
                         sum1 += N1[nX*nU*x + nX*u + y];
                         sum2 += N2[nX*nU*x + nX*u + y];
                    }
                    
                    for (unsigned int y = 0; y < nX; ++y)
                    {
                         normalizedN1.push_back(N1[nX*nU*x + nX*u + y]);
                         if (sum1 != 0.0) { normalizedN1.back() /= sum1; }
                         
                         normalizedN2.push_back(N2[nX*nU*x + nX*u + y]);
                         if (sum2 != 0.0) { normalizedN2.back() /= sum2; }
                    }
               }
          }
          
               //   Compute euclidian distance
          double sumDiff2 = 0.0;
          for (unsigned int i = 0; i < nX*nU*nX; ++i)
          {
               sumDiff2 += ((normalizedN1[i] - normalizedN2[i])
                              * (normalizedN1[i] - normalizedN2[i]));
          }
          iniDist = sqrt(sumDiff2);
     }


     //   Distance w.r.t. to update rate
     double updateDist = 0.0;
     
     if (beta != 0.0)
     {
               //   Compute the weights associated to each state-action pair
          vector<double> weights1, weights2;
          for (unsigned int x = 0; x < nX; ++x)
          {
               for (unsigned int u = 0; u < nU; ++u)
               {
                    weights1.push_back(0.0);
                    weights2.push_back(0.0);
     
                    for (unsigned int y = 0; y < nX; ++y)
                    {
                         weights1.back() += N1[nX*nU*x + nX*u + y];
                         weights2.back() += N2[nX*nU*x + nX*u + y];
                    }
               }
          }
          
               //   Compute the euclidian distance
          double sumDiff2 = 0.0;
          for (unsigned int i = 0; i < nX*nU; ++i)
          {
               sumDiff2 += ((weights1[i] - weights2[i])
                              * (weights1[i] - weights2[i]));
          }
          updateDist = sqrt(sumDiff2);
     }
     
     
     //   Return
//     cout << "iniDist:    " << iniDist << "\n";
//     cout << "updateDist: " << updateDist << "\n";
     cout << (alpha*iniDist) / (beta*updateDist) << endl;
     return (alpha * iniDist) + (beta * updateDist);
}
