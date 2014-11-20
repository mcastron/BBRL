#pragma once

#include <vector>
#include "samplers/sampler.h"

class PCTransitionSampler : public Sampler{
	public:

	PCTransitionSampler(double* _P, uint _S);
	~PCTransitionSampler();

	uint getNextStateSample();

	protected:
	 double* P;
	 uint S;

};

//-----------------------------------------------------


class PCTransitionParamSampler : public Sampler{
	public:

	PCTransitionParamSampler(const uint* _counts,
	         std::vector<double>& priorcountList_, uint _S);
	~PCTransitionParamSampler();

	void getNextTParamSample(double* P);

	protected:

	 const uint* counts;
	 std::vector<double> priorcountList;
	 double* ncounts;
	 uint S;

};


//-----------------------------------------------------

class PCMDPSampler : public Sampler{
	public:
		PCMDPSampler(const uint* counts,
		          std::vector<double>& priorcountList_,
				uint S, uint A, double* R, bool rsas, double gamma); 
		~PCMDPSampler();

		SIMULATOR* updateMDPSample();
	protected:

		const uint* counts;
		std::vector<double> priorcountList;
		double* ncounts;
		uint S,A,SA;
		double* R;
		bool rsas;
		double gamma;
		double* T;
		SIMULATOR* simulator;
};
