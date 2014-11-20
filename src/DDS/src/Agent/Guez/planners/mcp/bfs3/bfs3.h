#pragma once

/* BFS3 Algorithm implementation
 *
 * Reference:
 * Learning is planning: near Bayes-optimal reinforcement learning via Monte-Carlo tree search  
 * John Asmuth, Michael Littman
 * UAI 2011
 *
 */

class SIMULATOR;
class SamplerFactory;
class VNODE3;
class QNODE3;

typedef unsigned int uint;

class BFS3
{
public:

    struct PARAMS
    {
        PARAMS();

        int Verbose;
				uint D;
				uint C;
				uint N;
				double gamma;
				double Vmin;
				double Vmax;
    };

    BFS3(const SIMULATOR& simulator, const PARAMS& params,
				SamplerFactory& sampFact);
    ~BFS3();
		
		inline double getReward(uint,uint,uint);
    uint SelectAction(uint current_state);
    bool Update(uint state, uint action, uint observation, double reward);
		void bellmanBackup(VNODE3* vnode, int action, uint s);



private:

		double FSSS(uint prev_state,uint a, uint start_state);
		void FSSSRollout(VNODE3*& vnode, uint state, uint depth);
		VNODE3* ExpandNode(uint state); 
	
		void getQNodeValue(QNODE3& qnode, double& Usa, double& Lsa, uint ss,uint aa);
		
			uint* counts;
		uint* pcounts;
		//Cached values
		uint S,A,SA,SAS;

    PARAMS Params;
    VNODE3* Root;
	
		const SIMULATOR& Simulator;
	
		SamplerFactory& SampFact;

    int TreeDepth, PeakTreeDepth;
};

