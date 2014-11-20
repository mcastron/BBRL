#pragma once


typedef unsigned int uint;

namespace MDPutils{

	void policyEvaluation(uint S, 
			                  uint A, 
												bool rsas, 
												double* P,
												double* R,
												double gamma,
												double epsilon,
												const uint* PI,
												double* V);

	void valueIteration(uint S,
											uint A,
											bool rsas,
											double* P,
											double* R,
											double gamma,
											double epsilon,
											uint* PI,
											double* V);
	
	void valueIterationRmax(uint S,
			uint A,
			bool rsas,
			double* P,
			double* R,
			double gamma,
			double epsilon,
			uint* PI,
			double* V,
			const uint* counts,
			uint B);

	//void valueIterationEpisodic(double epsilon, size_t PI[], double V, size_t termState);
	//void expectedOptimalReturn(std::vector<double>& Rt, size_t numSteps);

}



