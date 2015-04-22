#include "PCSamplerFactory.h"
#include "PCTransitionSampler.h"

PCSamplerFactory::PCSamplerFactory(std::vector<double>& priorcountList_) :
          priorcountList(priorcountList_){ }


//Sampler* PCSamplerFactory::getTransitionSampler(const uint* counts,uint s, uint a, uint S){
Sampler* PCSamplerFactory::getTransitionSampler(const uint* counts, uint s, uint a, uint S){
     uint A = (priorcountList.size() / (S * S));
	double* P = new double[S];
	double sum = 0;
	for(uint ll=0;ll<S;++ll){
		P[ll] = (double) counts[ll] + priorcountList[A*S*s + S*a + ll];	
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
Sampler* PCSamplerFactory::getTransitionParamSampler(const uint* counts, uint s, uint a, uint S){
     double* ncounts = new double[S];
     uint A = priorcountList.size() / (S * S);
     for (unsigned int sp = 0; sp < S; ++sp)
          ncounts[sp] = (counts[sp] + priorcountList[A*S*s + S*a + sp]);
     
	PCTransitionParamSampler* sampler = new PCTransitionParamSampler(ncounts,S);
	return (Sampler*) sampler;
}

double* PCSamplerFactory::getPostCounts(uint* counts, uint S, uint A)
{
     double* postCounts = new double[S*A*S];
     for (uint s = 0; s < S; ++s)
          for (uint a = 0; a < A; ++a)
               for (uint sp = 0; sp < S; ++sp)
               {
                    uint i = (s*A*S + a*S + sp);
                    postCounts[i] = priorcountList[i] + counts[i];
               }
     return postCounts;
}

