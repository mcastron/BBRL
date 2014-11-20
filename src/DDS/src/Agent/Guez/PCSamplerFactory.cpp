#include "PCSamplerFactory.h"
#include "PCTransitionSampler.h"

PCSamplerFactory::PCSamplerFactory(std::vector<double>& priorcountList_) :
          priorcountList(priorcountList_){ }


//Sampler* PCSamplerFactory::getTransitionSampler(const uint* counts,uint s, uint a, uint S){
Sampler* PCSamplerFactory::getTransitionSampler(const uint* counts, uint, uint, uint S){
	double* P = new double[S];
	double sum = 0;
	for(uint ll=0;ll<S;++ll){
		P[ll] = (double) counts[ll] + priorcountList[ll];	
		sum += P[ll];
	}
	//Normalize 
	for(uint ll=0;ll<S;++ll){
		P[ll] = P[ll] / sum; 
	}
	
	PCTransitionSampler* sampler = new PCTransitionSampler(P,S);
	return (Sampler*) sampler;
}

Sampler* PCSamplerFactory::getMDPSampler(const uint* counts, uint S, uint A,
		double* R, bool rsas, double gamma){
	PCMDPSampler* sampler = new PCMDPSampler(counts, priorcountList, S, A, R, rsas, gamma);
	return (Sampler*) sampler;
}

//Sampler* PCSamplerFactory::getTransitionParamSampler(const uint* counts, uint s, uint a, uint S){
Sampler* PCSamplerFactory::getTransitionParamSampler(const uint* counts, uint, uint, uint S){
	PCTransitionParamSampler* sampler = new PCTransitionParamSampler(counts,priorcountList,S);
	return (Sampler*) sampler;
}


