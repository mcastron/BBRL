#include "LazySamplerFactory.h"

#include "LazyMDPSampler.h"

LazySamplerFactory::LazySamplerFactory(SamplerFactory* _sampFact): sampFact(_sampFact){}

LazySamplerFactory::~LazySamplerFactory(){
	delete sampFact;
}

void LazySamplerFactory::reset(){
	sampFact->reset();
}
void LazySamplerFactory::updateCounts(uint s, uint a, uint obs){
	sampFact->updateCounts(s,a,obs);
}

Sampler* LazySamplerFactory::getMDPSampler(const uint* counts, uint S, uint A,
		double* R, bool rsas, double gamma){
	LazyMDPSampler* sampler = new LazyMDPSampler(counts, sampFact, S, A, R, rsas, gamma);

	return (Sampler*) sampler;
}


