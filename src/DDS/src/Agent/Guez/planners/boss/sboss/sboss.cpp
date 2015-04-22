#include "sboss.h"

#include <math.h>
#include "../../../utils/guez_utils.h"
#include <algorithm>

#include "../../../planners/MDPutils.h" 

#include "../../../samplers/sampler.h"
#include "../../../envs/simulator.h"
#include "../../../samplers/samplerFactory.h"


using namespace std;
using namespace UTILS;

//-----------------------------------------------------------------------------

SBOSS::PARAMS::PARAMS()
:   Verbose(0),
		delta(2),
		epsilon(0.1),
		maxError(0.0001)

{
}

SBOSS::SBOSS(const SIMULATOR& simulator, const PARAMS& params,
					 SamplerFactory& sampFact)
:   Simulator(simulator),
    Params(params),
		SampFact(sampFact)
{
		A = Simulator.GetNumActions();
		S = Simulator.GetNumObservations();
		SA = S*A;
		SAS = S*A*S;


		//Initialize transition counts for posterior estimation
		counts = new uint[SAS];
		for (uint i = 0; i < SAS; ++i) { counts[i] = 0; }
          postCounts = sampFact.getPostCounts(counts, S, A);
          
          postCountsSum = new double[SA];
          for (uint s = 0; s < S; ++s)
               for (uint a = 0; a < A; ++a)
               {
                    postCountsSum[A*s + a] = 0.0;
                    for (uint sp = 0; sp < S; ++sp)
                         postCountsSum[A*s + a] += postCounts[A*S*s + S*a + sp];
               }
          
          postCountsLastResample = new double[SAS];
          postCountsSumLastResample = new double[SA];

	
	     Pm = 0;
	     Rm = 0;
		RLPI =  new uint[S];
		V = new double[S];

		
		K = new uint[SA];
		for (uint s = 0; s < S; ++s)
		   for (uint a = 0; a < A; ++a) { updateK(s, a); }
		
		do_sample = true;
}

SBOSS::~SBOSS()
{
	delete[] postCounts;
	delete[] postCountsSum;
	delete[] postCountsLastResample;
	delete[] postCountsSumLastResample;
	delete[] RLPI;
	delete[] V;
	if (Pm) { delete[] Pm; }
	if (Rm) { delete[] Rm; }
	delete[] K;
}

//Compute the deviation of the means from one posterior to the other,
//as done in Precup, Castro 2010.
double SBOSS::posteriorDeviation(const double* counts1, double sum1,
		                       const double* counts2, double sum2){	
	double dev = 0.0;
	for (uint sp = 0; sp < S; ++sp)
	{
	    double P1 = (counts1[sp] / sum1);
	    double P2 = (counts2[sp] / sum2);
	    
	    double sigma = sqrt(
	              (counts1[sp] * (sum1 - counts1[sp]))
	                   / (sum1 * sum1 * (sum1 + 1)));
	    
	    dev += (fabs(P1 - P2) / sigma);
	}
	return dev;
}


//bool SBOSS::Update(uint state, uint action, uint observation, double reward)
bool SBOSS::Update(uint state, uint action, uint observation, double)
{
		uint ArrayPos = state*SA+S*action;
		//Update posterior
		counts[ArrayPos+observation] += 1;
		postCounts[ArrayPos+observation] += 1;
		postCountsSum[state*A+action] += 1;
		SampFact.updateCounts(state,action,observation);	
		//Check resampling criterion
		if(posteriorDeviation(postCountsLastResample+ArrayPos,
					       postCountsSumLastResample[state*A+action],
						  postCounts+ArrayPos,
						  postCountsSum[state*A+action]) > Params.delta)
          {
			do_sample = true;
          }

          updateK(state, action);

		return true;
}

void SBOSS::createMergedModel(){
     //   Get the maximal number of samples to draw for a <state, action> pair    
     uint maxNbSamples = 0;
     for (uint s = 0; s < S; ++s)
          for (uint a = 0; a < A; ++a)
               if (maxNbSamples < K[A*s + a]) { maxNbSamples = K[A*s + a]; }

     //   Adjust the size of the variables representing the merged MDP
     Am = A*maxNbSamples;
	SAm = S*Am;
	
	if (Pm) { delete[] Pm; }
     Pm = new double[S*Am*S];

     if (Rm) { delete[] Rm; }
	if(Simulator.rsas){
		Rm = new double[SAm*S];
		for(uint s=0;s<S;++s){
			for(uint a=0;a<Am;++a)
				memcpy(Rm+s*SAm+a*S,
				       Simulator.R+s*SA+(a%A)*S,S*sizeof(double));
		}
	}
	else{
		Rm = new double[SAm];
		for(uint s=0;s<S;++s){
			for(uint a=0;a<Am;++a)
				Rm[s*Am+a] = Simulator.R[s*A+(a%A)];
		}
	}


     //   Build the merged MDP
	for(size_t i=0; i<S; ++i){
		uint iSAm = i*SAm;
		uint iSA = i*SA;
		for(size_t a=0; a<A; ++a){		   
			//Get a sampler to sample K T(s,a,.) parameters
			Sampler* TParamSampler = SampFact.getTransitionParamSampler(counts+iSA+a*S,i,a,S);
			uint kA = 0;	
			for(size_t k=0; k < K[i*A + a]; ++k){
				TParamSampler->getNextTParamSample(Pm+iSAm+(a+kA)*S);
				kA+=A;
			}
			for (size_t k = K[i*A + a]; k < maxNbSamples; ++k)
			{
                    for (uint j = 0; j < S; ++j)
                    {
                         if (i == j) { Pm[iSAm+(a+kA)*S+j] = 1.0; }
                         else        { Pm[iSAm+(a+kA)*S+j] = 0.0; }
                         Rm[iSAm+(a+kA)*S+j] = INT_MIN;
                    }
			     kA += A;
			} 
			
			delete TParamSampler;
		}
	}
}

uint SBOSS::SelectAction(uint state)
{
		uint a;
		if(do_sample){
			//Sample K MDPs and create merged model
			createMergedModel();
			//Solve merged model
			MDPutils::valueIteration(S,
															 Am,
															 Simulator.rsas,
															 Pm,
															 Rm,
															 Simulator.GetDiscount(),
															 Params.maxError,
															 RLPI,
															 V);
			memcpy(postCountsLastResample,postCounts,SAS*sizeof(uint));
			memcpy(postCountsSumLastResample,postCountsSum,SA*sizeof(uint));
			do_sample = false;
		}
		//Use computed policy from the merged model	
		a = RLPI[state]%A;
		
		return a;
}


void SBOSS::updateK(uint state, uint action)
{
     double alpha0 = postCountsSum[state*A+action];
     double den = (alpha0*alpha0) * (alpha0 + 1);
     
     K[state*A+action] = 0;
     for (uint sp = 0; sp < S; ++sp)
     {
          double alphai = postCounts[state*SA+action*S+sp];
          double num = alphai * (alpha0 - alphai);
          double var = (num / den);

          uint curVal = ceil(var / Params.epsilon);
          if (K[state*A+action] < curVal) { K[state*A+action] = curVal; }
     }
}

//-----------------------------------------------------------------------------


