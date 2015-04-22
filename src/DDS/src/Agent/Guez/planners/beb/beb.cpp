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
		uint* counts = new uint[SAS];
		for (uint i = 0; i < SAS; ++i) { counts[i] = 0; }
          postCounts = sampFact.getPostCounts(counts, S, A);
          delete[] counts;
          
          postCountsSum = new double[SA];
          P = new double[SAS];
          for (uint s = 0; s < S; ++s)
               for (uint a = 0; a < A; ++a)
               {
                    postCountsSum[A*s + a] = 0.0;
                    for (uint sp = 0; sp < S; ++sp)
                         postCountsSum[A*s + a] += postCounts[A*S*s + S*a + sp];

                    for (uint sp = 0; sp < S; ++sp)
                         P[A*S*s + S*a + sp] = postCounts[A*S*s + S*a + sp]
                                                  / postCountsSum[A*s + a];
               }
		
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
						R[ss*SA+aa*S+sp] += Params.b/(1+postCountsSum[ss*A+aa]);
				}
				else
					R[ss*A+aa] += Params.b/(1+postCountsSum[ss*A+aa]);	
			}
		}
	
}

BEB::~BEB()
{
	delete[] postCounts;
	delete[] postCountsSum;
	delete[] P;
	delete[] R;
	delete[] V;
	delete[] RLPI;
}

//bool BEB::Update(uint ss, uint aa, uint observation, double reward)
bool BEB::Update(uint ss, uint aa, uint observation, double)
{
		//Update posterior
		postCounts[ss*SA+S*aa+observation] += 1;
		postCountsSum[ss*A+aa] += 1;	


		//Update mean model
		memcpy(P+ss*SA+aa*S,postCounts+ss*SA+aa*S,S*sizeof(double));
		for(uint sp=0;sp<S;++sp){
			P[ss*SA+aa*S+sp] = P[ss*SA+aa*S+sp]/postCountsSum[ss*A+aa];
		}
		//Update Reward function
		
		if(Simulator.rsas){
			memcpy(R+ss*SA+aa*S,Simulator.R+ss*SA+aa*S,S*sizeof(double));
			for(uint sp=0; sp<S;++sp)
				R[ss*SA+aa*S+sp] += Params.b/(1+postCountsSum[ss*A+aa]);
		}
		else{
			R[ss*A+aa] = Simulator.R[ss*A+aa];
			R[ss*A+aa] += Params.b/(1+postCountsSum[ss*A+aa]);
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


