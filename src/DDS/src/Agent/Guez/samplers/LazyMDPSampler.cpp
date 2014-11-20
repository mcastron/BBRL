#include "LazyMDPSampler.h"

#include "FDMsamplerFactory.h"

#include "../envs/lazyMDP.h"
#include "../envs/basicMDP.h"


//LazyMDPSampler functions
//-----------------------------------------------------

LazyMDPSampler::LazyMDPSampler(const uint* _counts, SamplerFactory* _samplerFact,
				uint _S, uint _A, double* _R, bool _rsas, double _gamma):
counts(_counts),samplerFact(_samplerFact), S(_S), A(_A), R(_R), rsas(_rsas),gamma(_gamma){
	SA = S*A;		
	
	simulator = 0;
}
LazyMDPSampler::~LazyMDPSampler(){
	if(simulator)
		delete simulator;
}

void LazyMDPSampler::getTransitionParam(uint s, uint a, double* P){
	Sampler* transParamSampler = samplerFact->getTransitionParamSampler(counts+s*SA+a*S,s,a, S);
	transParamSampler->getNextTParamSample(P);
	delete transParamSampler;
}

SIMULATOR* LazyMDPSampler::updateMDPSample(){
	
	if(simulator)
		delete simulator;
	simulator = new LazyMDP(S,A,gamma,R,rsas,this);
	
	return (SIMULATOR*) simulator;	
}
