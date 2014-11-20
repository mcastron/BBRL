#include "boss.h"

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

BOSS::PARAMS::PARAMS()
:   Verbose(0),
		K(20),
		B(5),
		epsilon(0.0001)

{
}

BOSS::BOSS(const SIMULATOR& simulator, const PARAMS& params,
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
		std::fill(counts,counts+SAS,0);
	
		qcounts = new uint[SA];
		std::fill(qcounts,qcounts+SA,0);
		
		RLPI =  new uint[S];
		V = new double[S];
		
		Pm = new double[S*Am*S];
		
		//Create reward function for merged model
		if(Simulator.rsas){
			Rm = new double[SAm*S];
			for(uint s=0;s<S;++s){
				for(uint a=0;a<Am;++a)
					memcpy(Rm+s*SAm+a*S,Simulator.R+s*SA+S*(a%A),S*sizeof(double));
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

BOSS::~BOSS()
{
	delete[] counts;
	delete[] qcounts;
	delete[] RLPI;
	delete[] V;
	delete[] Pm;
	delete[] Rm;
}

//bool BOSS::Update(uint state, uint action, uint observation, double reward)
bool BOSS::Update(uint state, uint action, uint observation, double)
{
		//Update posterior
		counts[state*SA+S*action+observation] += 1;
		qcounts[state*A+action] += 1;
		SampFact.updateCounts(state,action,observation);

		//Check resampling criterion
		if(qcounts[state*A+action] == Params.B)
			do_sample = true;

		return true;
}

void BOSS::createMergedModel(){

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

uint BOSS::SelectAction(uint state)
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
			do_sample = false;
		}
		//Use computed policy from the merged model	
		a = RLPI[state]%A;
		
		return a;
}


//-----------------------------------------------------------------------------


