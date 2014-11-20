#pragma once

#include "sampler.h"
#include "../utils/guez_utils.h"

class FDMTransitionSampler : public Sampler{
	public:

	FDMTransitionSampler(double* _P, uint _S);
	~FDMTransitionSampler();

	uint getNextStateSample();

	protected:
	 double* P;
	 uint S;

};

//-----------------------------------------------------


class FDMTransitionParamSampler : public Sampler{
	public:

	FDMTransitionParamSampler(const uint* _counts, double _alpha, uint _S);
	~FDMTransitionParamSampler();

	void getNextTParamSample(double* P);

	protected:

	 const uint* counts;
	 double alpha;
	 uint S;

};


//-----------------------------------------------------

class FDMMDPSampler : public Sampler{
	public:
		FDMMDPSampler(const uint* counts, double alpha,
				uint S, uint A, double* R, bool rsas, double gamma); 
		~FDMMDPSampler();

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
};
