
/**
	TODO
		Changes from original in order to all several instances:
			- Static elements moved into the instance
			- VNODE::FreeAll() commented
*/

#pragma once

/* BAUCT algorithm implementation
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

class BAUCT
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
				double eps;
    };

    //    BAUCT(const SIMULATOR& simulator, const PARAMS& params,SamplerFactory& sampFact);
//	TODO: ADDED
	BAUCT(const SIMULATOR& simulator, const PARAMS& params,SamplerFactory& sampFact, std::vector<double> priorcountList = std::vector<double>());
//
    ~BAUCT();

    int SelectAction(uint current_state);
    bool Update(uint state, uint action, uint observation, double reward);

    void UCTSearch(uint current_state);

    double Rollout(uint state);

    const HISTORY& GetHistory() const { return History; }
    const SIMULATOR::STATUS& GetStatus() const { return Status; }
    void ClearStatistics();
    void DisplayStatistics(std::ostream& ostr) const;
    void DisplayValue(int depth, std::ostream& ostr) const;
    void DisplayPolicy(int depth, std::ostream& ostr) const;

    static void InitFastUCB(double exploration);

//private:
//	TODO - Modified
		inline double getReward(uint ss, uint aa, uint sp);

		uint* counts;
		uint* pcounts;
		
		//Cached values
		uint S,A,SA,SAS;

    PARAMS Params;
    const SIMULATOR& Simulator;
    VNODE* Root;
    HISTORY History;
    SIMULATOR::STATUS Status;
    int TreeDepth, PeakTreeDepth;
		
		SamplerFactory& SampFact;

		/*
		//Leaf computation
		double* Rbeb;
		double Paramsb;
		*/
		uint* RLPI;
		double* V;
		
		double* Q;
		std::vector<uint>** GreedyA;

    STATISTIC StatTreeDepth;
    STATISTIC StatRolloutDepth;
    STATISTIC StatTotalReward;

    int GreedyUCB(VNODE* vnode, bool ucb) const;
    int SelectRandom() const;
    double SimulateV(uint state, VNODE* vnode);
    double SimulateQ(uint state, QNODE& qnode, uint action);
    VNODE* ExpandNode(); 

    // Fast lookup table for UCB
    //static const int UCB_N = 10000, UCB_n = 100;
//		static const int UCB_N = 10000, UCB_n = 5000;
    /*TODO 	static double UCB[UCB_N][UCB_n];*/
    //TODO:	ADDED
//    std::vector<std::vector<double> > UCB;
    //TODO:	ADDED
    /*TODO 	static*/ bool InitialisedFastUCB;

    double FastUCB(int N, int n, double logN) const;

		uint step;

		std::ofstream meand;
		std::ofstream maxd;

};

