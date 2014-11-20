#pragma once

#include "samplerFactory.h"
class SFDMTransitionParamSampler;

//Sparse Flat Dirichlet Multinomial sampler factory
// For details, see Friedman, Singer (1999)


class SFDMsamplerFactory : public SamplerFactory{
	public:
		
		SFDMsamplerFactory(double priorcount, bool expDecay, double beta, uint S, uint A);
		~SFDMsamplerFactory();	
		Sampler* getTransitionSampler(const uint* counts,uint s, uint a, uint S);
		Sampler* getMDPSampler(const uint* counts, uint S, uint A, double* R, bool rsas, double gamma);
		Sampler* getTransitionParamSampler(const uint* counts, uint s, uint a, uint S);
		
		void reset();

		void updateCounts(uint s, uint a, uint obs);
		double getAlphaMean(){return alpha;}
	protected:
	
		void checkAndUpdate(uint ind);

		double alpha;
		bool expDecay; // P(S = k) = beta^k or k^-beta
		double beta;
		uint S;
		uint A;
		uint SA;
		double* Pk;
		double* sumLog;
		double* lgam;	
		
		//Set of observed outcomes (for each s,a)
		std::vector<uint>** SIG0;
		//binary strings indicating if outcome has been observed or not 
		// for O(1) access)
		bool** OFlag;
		bool* dirtyPkD;
		double** PkD;
		uint* CSum;	
		
		double** CDLpre;

		std::vector<uint> ordering;

		friend class SFDMTransitionParamSampler;
		friend class SFDMMDPSampler;
};
