#pragma once

/* Implementation of the BOSS algorithm variant from this paper:
 *
 * Smarter Sampling in Model-Based Bayesian Reinforcement Learning. 
 * Pablo Samuel Castro, Doina Precup
 * ECML/PKDD 2010
 */

class SIMULATOR;
class SamplerFactory;

typedef unsigned int uint;

class SBOSS
{
public:

    struct PARAMS
    {
        PARAMS();

        int Verbose;
				double delta;
				double epsilon;
				double maxError;
    };

    SBOSS(const SIMULATOR& simulator, const PARAMS& params,
				SamplerFactory& sampFact);
    ~SBOSS();

    uint SelectAction(uint state);
    bool Update(uint state, uint action, uint observation, double reward);
		
		double posteriorDeviation(const double* counts1, double sum1,
		                          const double* counts2, double sum2);
		void createMergedModel();
   
	private:
		uint* counts;
    const SIMULATOR& Simulator;
	
		//SBOSS-specific
		bool do_sample;
		
		double* postCounts;
		double* postCountsSum;
		double* postCountsLastResample;
		double* postCountsSumLastResample;
		PARAMS Params;
		
		//Merged model
		uint Am;	
		double* Pm;
		double* Rm;
		uint* RLPI;
		double* V;
		
		uint* K;
		

		//Cached values
		uint S,A,SA,SAS,SAm;

		SamplerFactory& SampFact;

          void updateK(uint state, uint action);
};

