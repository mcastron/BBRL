#pragma once

#include "samplerFactory.h"

//Wrapper to sample lazily (on demand)

class LazySamplerFactory : public SamplerFactory{
	public:
		
		LazySamplerFactory(SamplerFactory* _sampFact);
		~LazySamplerFactory();	
		Sampler* getMDPSampler(const uint* counts, uint S, uint A, double* R, bool rsas, double gamma);
	
		void updateCounts(uint s, uint a, uint obs);
		void reset();

		double getAlphaMean(){return sampFact->getAlphaMean();}
	protected:
	
		SamplerFactory* sampFact;
};
