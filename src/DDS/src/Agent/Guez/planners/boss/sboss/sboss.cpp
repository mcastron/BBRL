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
		K(20),
		delta(2),
		epsilon(0.0001)

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
		
		Am = A*Params.K;
		SAm = S*Am;

		//Initialize transition counts for posterior estimation
		counts = new uint[SAS];	
		countsLastResample = new uint[SAS];
		std::fill(counts,counts+SAS,0);
		std::fill(countsLastResample,countsLastResample+SAS,0);

		countsSum = new uint[SA];
		std::fill(countsSum,countsSum+SA,0);
		countsSumLastResample = new uint[SA];
		std::fill(countsSumLastResample,countsSumLastResample+SA,0);
		
	
		RLPI =  new uint[S];
		V = new double[S];
		
		Pm = new double[S*Am*S];
		
		//Create reward function for merged model
		if(Simulator.rsas){
			Rm = new double[SAm*S];
			for(uint s=0;s<S;++s){
				for(uint a=0;a<Am;++a)
					memcpy(Rm+s*SAm+a*S,Simulator.R+s*SA+(a%A)*S,S*sizeof(double));
			}
		}
		else{
			Rm = new double[SAm];
			for(uint s=0;s<S;++s){
				for(uint a=0;a<Am;++a)
					Rm[s*Am+a] = Simulator.R[s*A+(a%A)];
			}
		}
		do_sample = true;
}

SBOSS::~SBOSS()
{
	delete[] counts;
	delete[] countsLastResample;
	delete[] countsSum;
	delete[] countsSumLastResample;
	delete[] RLPI;
	delete[] V;
	delete[] Pm;
	delete[] Rm;
}

//Compute the deviation of the means from one posterior to the other,
//as done in Precup, Castro 2010.
double SBOSS::posteriorDeviation(const uint* counts1,
													uint sum1,
		                      const uint* counts2,
													uint sum2){
	double alphamean = SampFact.getAlphaMean();
	double sum2c = sum2 + alphamean*S;
	double sum1c = sum1 + alphamean*S;
	double dev = 0;
	double tmp;
	if(sum2 == 0)
		return 0.0; //no change
	else{
		for(uint s=0; s<S; ++s){
			tmp = fabs(((counts1[s]+alphamean)/sum1c-(counts2[s]+alphamean)/sum2c));
			//Divide by variance of the marginal for s
			dev += tmp/sqrt(((counts1[s]+alphamean)*(sum1c-(counts1[s]+alphamean))));
		}
		dev *= sqrt((sum1c*sum1c*(sum1c+1)));
	}
	return dev;
}


//bool SBOSS::Update(uint state, uint action, uint observation, double reward)
bool SBOSS::Update(uint state, uint action, uint observation, double)
{
		uint ArrayPos = state*SA+S*action;
		//Update posterior
		counts[ArrayPos+observation] += 1;
		countsSum[state*A+action] += 1;
		SampFact.updateCounts(state,action,observation);	
		//Check resampling criterion
		if(posteriorDeviation(countsLastResample+ArrayPos,
					                countsSumLastResample[state*A+action],
													counts+ArrayPos,
													countsSum[state*A+action]) > Params.delta)
			do_sample = true;

		return true;
}

void SBOSS::createMergedModel(){

	for(size_t i=0; i<S; ++i){
		uint iSAm = i*SAm;
		uint iSA = i*SA;
		for(size_t a=0; a<A; ++a){
			//Get a sampler to sample K T(s,a,.) parameters
			Sampler* TParamSampler = SampFact.getTransitionParamSampler(counts+iSA+a*S,i,a,S);
			uint kA = 0;	
			for(size_t k=0; k<Params.K; ++k){
				TParamSampler->getNextTParamSample(Pm+iSAm+(a+kA)*S);
				kA+=A;
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
															 Params.epsilon,
															 RLPI,
															 V);
			memcpy(countsLastResample,counts,SAS*sizeof(uint));
			memcpy(countsSumLastResample,countsSum,SA*sizeof(uint));
			do_sample = false;
		}
		//Use computed policy from the merged model	
		a = RLPI[state]%A;
		
		return a;
}


//-----------------------------------------------------------------------------


