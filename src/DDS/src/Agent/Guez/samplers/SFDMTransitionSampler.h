#pragma once

#include "sampler.h"
#include "../utils/guez_utils.h"

class SFDMsamplerFactory;

class SFDMTransitionSampler : public Sampler{
	public:

	SFDMTransitionSampler(double* _P, uint _S);
	~SFDMTransitionSampler();

	uint getNextStateSample();

	protected:
	 double* P;
	 uint S;

};

//-----------------------------------------------------


class SFDMTransitionParamSampler : public Sampler{
	public:

	SFDMTransitionParamSampler(const uint* _counts, double _alpha, uint _S,
			SFDMsamplerFactory* _pFact, uint _ind);
	~SFDMTransitionParamSampler();

	void getNextTParamSample(double* P);

	protected:

	 const uint* counts;
	 double alpha;
	 uint S;
		
	 SFDMsamplerFactory* pFact;
	 uint ind;
	 double* dirParam;
};


//-----------------------------------------------------

class SFDMMDPSampler : public Sampler{
	public:
		SFDMMDPSampler(const uint* counts, double alpha,
				uint S, uint A, double* R, bool rsas, double gamma,
				SFDMsamplerFactory* _pFact); 
		~SFDMMDPSampler();

		SIMULATOR* updateMDPSample();
	protected:

		const uint* counts;
		double alpha;
		uint S,A,SA;
		double* R;
		bool rsas;
		double gamma;
		double* T;
		SIMULATOR* simulator;

		SFDMsamplerFactory* pFact;
		double* dirParam;

};
