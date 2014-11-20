#pragma once

#include "sampler.h"
#include "samplerFactory.h"
//#include "../utils/utils.h"


//-----------------------------------------------------

class LazyMDPSampler : public Sampler{
	public:
		LazyMDPSampler(const uint* counts, SamplerFactory* _samplerFact,
				uint S, uint A, double* R, bool rsas, double gamma); 
		~LazyMDPSampler();

		void getTransitionParam(uint s, uint a, double* P);
		SIMULATOR* updateMDPSample();
	protected:

		const uint* counts;

		SamplerFactory* samplerFact;
		uint S,A,SA;
		double* R;
		bool rsas;
		double gamma;
		
		SIMULATOR* simulator;
};
