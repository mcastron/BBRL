#pragma once

#include <vector>
#include "samplers/samplerFactory.h"

//Flat Dirichlet Multinomial sampler factory

class PCSamplerFactory : public SamplerFactory{
	public:
		
		PCSamplerFactory(std::vector<double>& priorcountList_);
		Sampler* getTransitionSampler(const uint* counts, uint s, uint a, uint S);
		Sampler* getMDPSampler(const uint* counts, uint S, uint A, double* R, bool rsas, double gamma);
		Sampler* getTransitionParamSampler(const uint* counts, uint s, uint a, uint S);
		
		void reset(){}

//		void updateCounts(uint s, uint a, uint obs){};
		void updateCounts(uint, uint, uint){};
		double getAlphaMean()
		{
		   double mean = 0.0;
		   for (unsigned int i = 0; i < priorcountList.size(); ++i)
		        mean += priorcountList[i];
		   mean /= priorcountList.size();
		   
		   return mean;
		   
		}
		double* getPostCounts(uint* counts, uint S, uint A);
	protected:
		
		std::vector<double> priorcountList;

};
