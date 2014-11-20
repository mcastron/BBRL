#include "FDMTransitionSampler.h"
#include "../envs/basicMDP.h"

//FDMTransitionSampler functions
//-----------------------------------------------------


FDMTransitionSampler::FDMTransitionSampler(double* _P, uint _S) : P(_P), S(_S){

}

FDMTransitionSampler::~FDMTransitionSampler()
{
	delete[] P;
}
uint FDMTransitionSampler::getNextStateSample(){
	uint sp;
	guez_utils::rng.multinom(P,S,sp);
	return sp;	
}

//FDMTransitionParamSampler functions
//-----------------------------------------------------


FDMTransitionParamSampler::FDMTransitionParamSampler(const uint* _counts,
		double _alpha, uint _S) :
	counts(_counts),alpha(_alpha), S(_S){
		
}

FDMTransitionParamSampler::~FDMTransitionParamSampler()
{
}

void FDMTransitionParamSampler::getNextTParamSample(double* P){
	guez_utils::sampleDirichlet(P,counts,S,alpha);
}

//FDMMDPSampler functions
//-----------------------------------------------------

FDMMDPSampler::FDMMDPSampler(const uint* _counts, double _alpha,
				uint _S, uint _A, double* _R, bool _rsas, double _gamma):
counts(_counts),alpha(_alpha), S(_S), A(_A), R(_R), rsas(_rsas),gamma(_gamma){
	SA = S*A;		
	T = new double[SA*S];
	simulator = 0;

}
FDMMDPSampler::~FDMMDPSampler(){
	delete[] T;	
	if(simulator)
		delete simulator;
}

SIMULATOR* FDMMDPSampler::updateMDPSample(){

	if(simulator)
		delete simulator;
	uint sSA;	
	//Sample transitions from the counts
	for(uint s=0;s<S;++s){
		sSA = s*SA;
		for(uint a=0;a<A;++a){
			guez_utils::sampleDirichlet(T+sSA+a*S,counts+(sSA+a*S),S,alpha);
		}
	}
	simulator = new BasicMDP(S,A,gamma,R,rsas,T);
	return (SIMULATOR*) simulator;	
}
