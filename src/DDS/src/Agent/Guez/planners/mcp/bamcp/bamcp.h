#pragma once

/* BAMCP algorithm implementation
 *
 * Reference:
 *
 * Guez, A. and Silver, D. and Dayan, P.
 * Efficient Bayes-Adaptive Reinforcement Learning using Sample-Based Search
 * Advances in Neural Information Processing Systems (NIPS) 2012
 *
 */


#include "history.h"
#include "../../../envs/simulator.h"
#include "../../../planners/mcp/statistic.h"

#include <fstream>

class VNODE;
class QNODE;
class SamplerFactory;

class BAMCP
{
public:

    struct PARAMS
    {
        PARAMS();

        int Verbose;
        int MaxDepth;
        int NumSimulations;
        int NumStartStates;
        int ExpandCount;
        double ExplorationConstant;
        bool ReuseTree;
				bool BANDIT;
				int RB;
				double eps;
    };

    BAMCP(const SIMULATOR& simulator, const PARAMS& params,SamplerFactory& sampFact);
    ~BAMCP();

    int SelectAction(uint current_state);
    bool Update(uint state, uint action, uint observation, double reward);

    void UCTSearch(uint current_state);
    void RolloutSearch();

    double Rollout(const SIMULATOR* mdp, uint state);

    const HISTORY& GetHistory() const { return History; }
    const SIMULATOR::STATUS& GetStatus() const { return Status; }
    void ClearStatistics();
    void DisplayStatistics(std::ostream& ostr) const;
    void DisplayValue(int depth, std::ostream& ostr) const;
    void DisplayPolicy(int depth, std::ostream& ostr) const;

    static void InitFastUCB(double exploration);

private:
		uint* counts;
		double* countsSum;
		//Cached values
		uint S,A,SA,SAS;

    PARAMS Params;
    const SIMULATOR& Simulator;
    VNODE* Root;
    HISTORY History;
    SIMULATOR::STATUS Status;
    int TreeDepth, PeakTreeDepth;
		
		SamplerFactory& SampFact;

		uint* RLPI;
		double* V;
		
		double* Q;
    double QlearningRate;
		std::vector<uint>** GreedyA;

    STATISTIC StatTreeDepth;
    STATISTIC StatRolloutDepth;
    STATISTIC StatTotalReward;

    int GreedyUCB(VNODE* vnode, bool ucb) const;
    int SelectRandom() const;
    double SimulateV(const SIMULATOR* senv, uint state, VNODE* vnode);
    double SimulateQ(const SIMULATOR* senv, uint state, QNODE& qnode, uint action);
    
    VNODE* ExpandNode();

    // Fast lookup table for UCB
    //static const int UCB_N = 10000, UCB_n = 100;
//		static const int UCB_N = 10000, UCB_n = 5000;
//    static double UCB[UCB_N][UCB_n];
//    static bool InitialisedFastUCB;

    double FastUCB(int N, int n, double logN) const;

		uint step;

		std::ofstream meand;
		std::ofstream maxd;

};

