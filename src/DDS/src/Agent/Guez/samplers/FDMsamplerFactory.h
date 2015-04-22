#pragma once

#include "samplerFactory.h"

//Flat Dirichlet Multinomial sampler factory

class FDMsamplerFactory : public SamplerFactory{
	public:
		
		FDMsamplerFactory(double priorcount);
		Sampler* getTransitionSampler(const uint* counts, uint s, uint a, uint S);
		Sampler* getMDPSampler(const uint* counts, uint S, uint A, double* R, bool rsas, double gamma);
		Sampler* getTransitionParamSampler(const uint* counts, uint s, uint a, uint S);
		
		void reset(){}

//		void updateCounts(uint s, uint a, uint obs){};
		void updateCounts(uint, uint, uint){};
		double getAlphaMean(){return alpha;}
		double* getPostCounts(uint* counts, uint S, uint A);
	protected:
		
		double alpha;

};
