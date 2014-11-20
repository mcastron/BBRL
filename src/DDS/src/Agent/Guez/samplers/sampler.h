#pragma once

#include "../envs/simulator.h"

class Sampler{
	
	public:
		virtual ~Sampler(){};
		
		virtual uint getNextStateSample(){return 0;};
		virtual SIMULATOR* updateMDPSample(){return 0;};
//		virtual void getNextTParamSample(double* P){};
		virtual void getNextTParamSample(double*){};
};
