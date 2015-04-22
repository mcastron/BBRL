#include "FDMsamplerFactory.h"
#include "FDMTransitionSampler.h"

FDMsamplerFactory::FDMsamplerFactory(double priorcount) : alpha(priorcount){ }


//Sampler* FDMsamplerFactory::getTransitionSampler(const uint* counts,uint s, uint a, uint S){
Sampler* FDMsamplerFactory::getTransitionSampler(const uint* counts, uint, uint, uint S){
	double* P = new double[S];
	double sum = 0;
	for(uint ll=0;ll<S;++ll){
		P[ll] = (double) counts[ll] + alpha;	
		sum += P[ll];
	}
	//Normalize 
	for(uint ll=0;ll<S;++ll){
		P[ll] = P[ll] / sum; 
	}
	
	FDMTransitionSampler* sampler = new FDMTransitionSampler(P,S);
	return (Sampler*) sampler;
}

Sampler* FDMsamplerFactory::getMDPSampler(const uint* counts, uint S, uint A,
		double* R, bool rsas, double gamma){
	FDMMDPSampler* sampler = new FDMMDPSampler(counts, alpha, S, A, R, rsas, gamma);
	return (Sampler*) sampler;
}

//Sampler* FDMsamplerFactory::getTransitionParamSampler(const uint* counts, uint s, uint a, uint S){
Sampler* FDMsamplerFactory::getTransitionParamSampler(const uint* counts, uint, uint, uint S){
	FDMTransitionParamSampler* sampler = new FDMTransitionParamSampler(counts,alpha,S);
	return (Sampler*) sampler;
}

double* FDMsamplerFactory::getPostCounts(uint* counts, uint S, uint A)
{
     double* postCounts = new double[S*A*S];
     for (uint s = 0; s < S; ++s)
          for (uint a = 0; a < A; ++a)
               for (uint sp = 0; sp < S; ++sp)
               {
                    uint i = (s*A*S + a*S + sp);
                    postCounts[i] = alpha + counts[i];
               }
     return postCounts;
}
