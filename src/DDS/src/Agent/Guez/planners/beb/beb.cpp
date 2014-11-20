#include "beb.h"

#include <math.h>
#include "../../utils/guez_utils.h"
#include <algorithm>

#include "../../planners/MDPutils.h" 

#include "../../envs/simulator.h"

#include "../../samplers/samplerFactory.h"

using namespace std;
using namespace UTILS;

//-----------------------------------------------------------------------------

BEB::PARAMS::PARAMS()
:   Verbose(0),
		b(5),
		epsilon(0.0001)

{
}

BEB::BEB(const SIMULATOR& simulator, const PARAMS& params,
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
		counts = new double[SAS];	
		std::fill(counts,counts+SAS,SampFact.getAlphaMean());
		countsSum = new double[SA];
		std::fill(countsSum,countsSum+SA,SampFact.getAlphaMean()*S);

		P = new double[SAS];
		std::fill(P,P+SAS,1/(double)S);//For symmetric priors
		
		if(Simulator.rsas){
			R = new double[SA*S];
		}
		else{
			R = new double[SA];
		}

		RLPI = new uint[S];
		V = new double[S];
		
		
		//Modify reward function
		if(Simulator.rsas)
			memcpy(R,Simulator.R,SAS*sizeof(double));
		else
			memcpy(R,Simulator.R,SA*sizeof(double));
		
		for(uint ss=0; ss<S; ++ss){
			for(uint aa=0; aa<A; ++aa){
				if(Simulator.rsas){
					for(uint sp=0; sp<S;++sp)
						R[ss*SA+aa*S+sp] += Params.b/(1+countsSum[ss*A+aa]);
				}
				else
					R[ss*A+aa] += Params.b/(1+countsSum[ss*A+aa]);	
			}
		}
	
}

BEB::~BEB()
{
	delete[] counts;
	delete[] countsSum;
	delete[] P;
	delete[] R;
	delete[] V;
	delete[] RLPI;
}

//bool BEB::Update(uint ss, uint aa, uint observation, double reward)
bool BEB::Update(uint ss, uint aa, uint observation, double)
{
		//Update posterior
		counts[ss*SA+S*aa+observation] += 1;
		countsSum[ss*A+aa] += 1;	


		//Update mean model
		memcpy(P+ss*SA+aa*S,counts+ss*SA+aa*S,S*sizeof(double));
		for(uint sp=0;sp<S;++sp){
			P[ss*SA+aa*S+sp] = P[ss*SA+aa*S+sp]/countsSum[ss*A+aa];
		}
		//Update Reward function
		
		if(Simulator.rsas){
			memcpy(R+ss*SA+aa*S,Simulator.R+ss*SA+aa*S,S*sizeof(double));
			for(uint sp=0; sp<S;++sp)
				R[ss*SA+aa*S+sp] += Params.b/(1+countsSum[ss*A+aa]);
		}
		else{
			R[ss*A+aa] = Simulator.R[ss*A+aa];
			R[ss*A+aa] += Params.b/(1+countsSum[ss*A+aa]);
		}
		return true;
}

uint BEB::SelectAction(uint state)
{
		uint a;
		
		//Solve mean model with modified reward function
		MDPutils::valueIteration(S,
				A,
				Simulator.rsas,
				P,
				R,
				Simulator.GetDiscount(),
				Params.epsilon,
				RLPI,
				V);

		//Use computed policy from the merged model	
		a = RLPI[state];

		return a;
}


//-----------------------------------------------------------------------------


