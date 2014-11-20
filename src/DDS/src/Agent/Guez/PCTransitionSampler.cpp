#include "PCTransitionSampler.h"
#include "envs/basicMDP.h"
#include "utils/guez_utils.h"

//PCTransitionSampler functions
//-----------------------------------------------------


PCTransitionSampler::PCTransitionSampler(double* _P, uint _S) : P(_P), S(_S){

}

PCTransitionSampler::~PCTransitionSampler()
{
	delete[] P;
}
uint PCTransitionSampler::getNextStateSample(){
	uint sp;
	guez_utils::rng.multinom(P,S,sp);
	return sp;	
}

//PCTransitionParamSampler functions
//-----------------------------------------------------


PCTransitionParamSampler::PCTransitionParamSampler(const uint* _counts,
		std::vector<double>& priorcountList_, uint _S) :
	    counts(_counts),priorcountList(priorcountList_), S(_S){
     ncounts = new double[priorcountList.size()];
}

PCTransitionParamSampler::~PCTransitionParamSampler()
{
     delete[] ncounts;
}

void PCTransitionParamSampler::getNextTParamSample(double* P){
     for (unsigned int i = 0; i < priorcountList.size(); ++i)
          ncounts[i] = (counts[i] + priorcountList[i]);
     
	guez_utils::sampleDirichlet(P,ncounts,S,0.0);
}

//PCMDPSampler functions
//-----------------------------------------------------

PCMDPSampler::PCMDPSampler(const uint* _counts,
                    std::vector<double>& priorcountList_,
				uint _S, uint _A, double* _R, bool _rsas, double _gamma):
counts(_counts),priorcountList(priorcountList_), S(_S), A(_A), R(_R), rsas(_rsas),gamma(_gamma){
	SA = S*A;		
	T = new double[SA*S];
	simulator = 0;
	ncounts = new double[priorcountList.size()];

}
PCMDPSampler::~PCMDPSampler(){
	delete[] T;	
	if(simulator)
		delete simulator;
     delete[] ncounts;
}

SIMULATOR* PCMDPSampler::updateMDPSample(){
     for (unsigned int i = 0; i < priorcountList.size(); ++i)
          ncounts[i] = (counts[i] + priorcountList[i]);

	if(simulator)
		delete simulator;
	uint sSA;	
	//Sample transitions from the counts
	for(uint s=0;s<S;++s){
		sSA = s*SA;
		for(uint a=0;a<A;++a){
			guez_utils::sampleDirichlet(T+sSA+a*S,ncounts+(sSA+a*S),S,0.0);
		}
	}
	simulator = new BasicMDP(S,A,gamma,R,rsas,T);
	return (SIMULATOR*) simulator;	
}
