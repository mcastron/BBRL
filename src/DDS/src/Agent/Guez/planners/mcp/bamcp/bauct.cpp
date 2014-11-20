#include "bauct.h"


#include <math.h>
#include "../../../utils/guez_utils.h"
#include <algorithm>

#include <iomanip>

#include "node.h"

#include "../../../samplers/sampler.h"
#include "../../../samplers/samplerFactory.h"

#include "../../../envs/banditSim.h"
#include "../../../planners/MDPutils.h"

#include "../../../envs/basicMDP.h"

using namespace std;
using namespace UTILS;

//-----------------------------------------------------------------------------

BAUCT::PARAMS::PARAMS()
:   Verbose(0),
    MaxDepth(100),
    NumSimulations(1000),
    NumStartStates(1000),
    ExpandCount(1),
    ExplorationConstant(1),
		eps(1)
{
}

//BAUCT::BAUCT(const SIMULATOR& simulator, const PARAMS& params,
//		SamplerFactory& sampFact)
//	TODO: ADDED
BAUCT::BAUCT(const SIMULATOR& simulator, const PARAMS& params,
		SamplerFactory& sampFact, std::vector<double> priorcountList)
//
:   Params(params),
		Simulator(simulator),
    TreeDepth(0),
		SampFact(sampFact)
{
    VNODE::NumChildren = Simulator.GetNumActions();
    QNODE::NumChildren = Simulator.GetNumObservations();

    Root = ExpandNode(); //Simulator.CreateStartState());

		A = Simulator.GetNumActions();
		S = Simulator.GetNumObservations();
		SA = S*A;
		SAS = S*A*S;
		
		//	TODO - Added
		if (priorcountList.empty()) { priorcountList.resize(SAS); }
		//
		
		RLPI = new uint[S];
		V = new double[S];
	
		Q = new double[SA];
		std::fill(Q,Q+SA,0);
		GreedyA = new std::vector<uint>*[S];
		std::vector<uint>	 tmp(A);
		for(uint a = 0;a<A;++a){
			tmp[a] = a;
		}
		for(uint s = 0;s<S;++s){
			GreedyA[s] = new std::vector<uint>(tmp);
		}
		
		//Initialize transition counts for posterior estimation
		counts = new uint[SAS];
		for(uint c=0;c<SAS;++c){
//			counts[c] = 0;
//
			counts[c] = priorcountList[c];
//
		}

		pcounts = new uint[SAS];

	
		step = 0;

//		meand.open("BMCP_meandepth",std::ios_base::app);	
//		maxd.open("BMCP_maxdepth",std::ios_base::app);
	//	TODO:	ADDED
		InitialisedFastUCB = true;
	//	TODO:	ADDED
}

BAUCT::~BAUCT()
{
	delete[] counts;
	delete[] pcounts;
 
	delete[] RLPI;
	delete[] V;
	
	delete[] Q;
	for(uint s=0;s<S;++s)
		delete GreedyA[s];
	delete[] GreedyA;

	VNODE::Free(Root, Simulator);
   // TODO:	VNODE::FreeAll();
}

bool BAUCT::Update(uint ss, uint aa, uint observation, double reward)
{
    History.Add(aa, observation);
	
		//Update posterior
		counts[ss*SA+S*aa+observation] += 1;
		SampFact.updateCounts(ss,aa,observation);	

    //Update Q
		Q[ss*A+aa] += 0.2*(reward + Simulator.GetDiscount()*Q[observation*A+GreedyA[observation]->at(0)] - Q[ss*A+aa]);
    //Update GreedyA
    double maxQ = -Infinity;
    GreedyA[ss]->clear();
    for(uint a2=0;a2<A;++a2){
      if(Q[ss*A+a2] >= maxQ)
      {
        if (Q[ss*A+a2] > maxQ)
          GreedyA[ss]->clear();
        maxQ = Q[ss*A+a2];
        GreedyA[ss]->push_back(a2);
      }
    }	


		//TODO: Reuse previous subtree for next search 
		// Delete old tree and create new root
    VNODE::Free(Root, Simulator);
    VNODE* newRoot = ExpandNode(); //ss);
    //newRoot->Beliefs() = beliefs;
    Root = newRoot;
    return true;
}

int BAUCT::SelectAction(uint current_state)
{
	UCTSearch(current_state);
	
  step++;

  return GreedyUCB(Root,false);;
}

void BAUCT::UCTSearch(uint state)
{
		
    ClearStatistics();
    int historyDepth = History.Size();
		
    for (int n = 0; n < Params.NumSimulations; n++)
    {
				Status.Phase = SIMULATOR::STATUS::TREE;

        if (Params.Verbose >= 2)
        {
            cout << "Starting simulation" << endl;
            //Simulator.DisplayState(*state, cout);
        }

        TreeDepth = 0;
        PeakTreeDepth = 0;
				
				//Reset pcounts (use pcounts during simulation)
				memcpy(pcounts,counts,SAS*sizeof(uint));
        
				double totalReward = SimulateV(state, Root);
        StatTotalReward.Add(totalReward);
        StatTreeDepth.Add(PeakTreeDepth);

        if (Params.Verbose >= 2)
            cout << "Total reward = " << totalReward << endl;
        if (Params.Verbose >= 4)
            DisplayValue(4, cout);

        History.Truncate(historyDepth);

		}
		
    DisplayStatistics(cout);
		//meand << StatTreeDepth.GetMean() << " ";
		//maxd << StatTreeDepth.GetMax() << " ";
}

inline double BAUCT::getReward(uint ss, uint aa, uint sp){
	if(Simulator.rsas)
		return Simulator.R[ss*SA+aa*S+sp];
	else
		return Simulator.R[ss*A+aa];
}

double BAUCT::SimulateV(uint state, VNODE* vnode)
{
    uint action = GreedyUCB(vnode, true);
	
    PeakTreeDepth = TreeDepth;
    if (TreeDepth >= Params.MaxDepth) // search horizon reached
        return 0;

		QNODE& qnode = vnode->Child(action);
    double totalReward = SimulateQ(state, qnode, action);
    vnode->Value.Add(totalReward);
    return totalReward;
}

double BAUCT::SimulateQ(uint state, QNODE& qnode, uint action)
{
    uint observation;
    double immediateReward, delayedReward = 0;

		uint ii = state*SA+action*S;
		Sampler* nextSSampler = SampFact.getTransitionSampler(pcounts+ii,state,action,S);
		observation =  nextSSampler->getNextStateSample();
		immediateReward = getReward(state,action,observation);
		delete nextSSampler;
		pcounts[state*SA+action*S+observation] += 1;
		bool terminal = false; //FIXME Assumes non-episodic tasks...

    History.Add(action, observation);
	    
		VNODE*& vnode = qnode.Child(observation);
    if (!vnode && !terminal && qnode.Value.GetCount() >= Params.ExpandCount)
        vnode = ExpandNode(); //&state);

    if (!terminal)
    {
        TreeDepth++;
        if (vnode)
            delayedReward = SimulateV(observation, vnode);
        else{
					delayedReward = Rollout(observation);
				}
        TreeDepth--;
    }

    double totalReward = immediateReward + Simulator.GetDiscount() * delayedReward;
    qnode.Value.Add(totalReward);
    return totalReward;
}

VNODE* BAUCT::ExpandNode()
{
    VNODE* vnode = VNODE::Create();
    vnode->Value.Set(0, 0);
		vnode->SetChildren(0,0);

    if (Params.Verbose >= 2)
    {
        cout << "Expanding node: ";
        History.Display(cout);
        cout << endl;
    }

    return vnode;
}


int BAUCT::GreedyUCB(VNODE* vnode, bool ucb) const
{
    static std::vector<int> besta;
    besta.clear();
    double bestq = -Infinity;
    int N = vnode->Value.GetCount();
    double logN = log(N + 1);

    for (int action = 0; action < Simulator.GetNumActions(); action++)
    {
        double q;
        int n;

        QNODE& qnode = vnode->Child(action);
        q = qnode.Value.GetValue();
        n = qnode.Value.GetCount();

				if (ucb)
            q += FastUCB(N, n, logN);
				
        if (q >= bestq)
        {
            if (q > bestq)
                besta.clear();
            bestq = q;
            besta.push_back(action);
        }
    }

    assert(!besta.empty());
    return besta[Random(besta.size())];
}

double BAUCT::Rollout(uint state)
{
    Status.Phase = SIMULATOR::STATUS::ROLLOUT;
    if (Params.Verbose >= 3)
        cout << "Starting rollout" << endl;
		
    double totalReward = 0.0;
    double discount = 1.0;
    bool terminal = false;
    int numSteps;
		int action;
    for (numSteps = 0; numSteps + TreeDepth < Params.MaxDepth && !terminal; ++numSteps)
    {
        uint observation;
        double reward;
				if(guez_utils::rng.rand_closed01() < Params.eps)
					action = Simulator.SelectRandom(state, Status);
				else{

					action = GreedyA[state]->at(rand() % GreedyA[state]->size());
				}
				
				uint ii = state*SA+action*S;
				Sampler* nextSSampler = SampFact.getTransitionSampler(pcounts+ii,state,action,S);
				observation =  nextSSampler->getNextStateSample();
				reward = getReward(state,action,observation);
				delete nextSSampler;
				pcounts[state*SA+action*S+observation] += 1;
        
				History.Add(action, observation);
				
				state = observation;
        totalReward += reward * discount;
        discount *= Simulator.GetDiscount();
    }

    StatRolloutDepth.Add(numSteps);
    if (Params.Verbose >= 3)
        cout << "Ending rollout after " << numSteps
            << " steps, with total reward " << totalReward << endl;
    return totalReward;
}

//double BAUCT::UCB[UCB_N][UCB_n];
//bool BAUCT::InitialisedFastUCB = true;

//void BAUCT::InitFastUCB(double exploration) /*TODO: ADDED*/ {} /*TODO: ADDED*/
void BAUCT::InitFastUCB(double) /*TODO: ADDED*/ {} /*TODO: ADDED*/
//{
//    cout << "Initialising fast UCB table with exp. const. " 
//			<< exploration << "...  ";
//    for (int N = 0; N < UCB_N; ++N)
//        for (int n = 0; n < UCB_n; ++n)
//            if (n == 0)
//                UCB[N][n] = Infinity;
//            else
//                UCB[N][n] = exploration * sqrt(log(N + 1) / n);
//    cout << "done" << endl;
//    InitialisedFastUCB = true;
//}
//	TODO


//inline double BAUCT::FastUCB(int N, int n, double logN) const
inline double BAUCT::FastUCB(int, int n, double logN) const
{
	/*	TODO: REMOVED
    if (InitialisedFastUCB && N < UCB_N && n < UCB_n)
        return UCB[N][n];
	*/
    if (n == 0)
        return Infinity;
    else
        return Params.ExplorationConstant * sqrt(logN / n);
}

void BAUCT::ClearStatistics()
{
    StatTreeDepth.Clear();
    StatRolloutDepth.Clear();
    StatTotalReward.Clear();
}

void BAUCT::DisplayStatistics(ostream& ostr) const
{
    if (Params.Verbose >= 1)
    {
        StatTreeDepth.Print("Tree depth", ostr);
        StatRolloutDepth.Print("Rollout depth", ostr);
        StatTotalReward.Print("Total reward", ostr);
    }

    if (Params.Verbose >= 2)
    {
        ostr << "Policy after " << Params.NumSimulations << " simulations" << endl;
        DisplayPolicy(6, ostr);
        ostr << "Values after " << Params.NumSimulations << " simulations" << endl;
        DisplayValue(6, ostr);
    }
}

void BAUCT::DisplayValue(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "BAUCT Values:" << endl;
    Root->DisplayValue(history, depth, ostr);
}

void BAUCT::DisplayPolicy(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "BAUCT Policy:" << endl;
    Root->DisplayPolicy(history, depth, ostr);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
