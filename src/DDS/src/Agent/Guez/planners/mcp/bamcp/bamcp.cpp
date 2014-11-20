
#include "bamcp.h"
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

BAMCP::PARAMS::PARAMS()
:   Verbose(0),
    MaxDepth(100),
    NumSimulations(1000),
    NumStartStates(1000),
    ExpandCount(1),
    ExplorationConstant(1),
		ReuseTree(false),
		BANDIT(false),   //Deal with bandits a bit differently in the code
		RB(-1),
		eps(1)
{
}

//BAMCP::BAMCP(const SIMULATOR& simulator, const PARAMS& params,
//		SamplerFactory& sampFact)
//	TODO: ADDED
BAMCP::BAMCP(const SIMULATOR& simulator, const PARAMS& params,
		SamplerFactory& sampFact, std::vector<double> priorcountList)
//		
:   Params(params),
		Simulator(simulator),
    TreeDepth(0),
		SampFact(sampFact)
{     
    VNODE::NumChildren = Simulator.GetNumActions();
    QNODE::NumChildren = Simulator.GetNumObservations();

    Root = ExpandNode();

		A = Simulator.GetNumActions();
		S = Simulator.GetNumObservations();
		SA = S*A;
		SAS = S*A*S;
		
		//	TODO - Added
		if (priorcountList.empty())
		{
		   priorcountList.resize(SAS);
		   for (unsigned int i = 0; i < SAS; ++i) { priorcountList[i] = 1.0; }
		}
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
    QlearningRate = 0.2;
		
		//Initialize transition counts for posterior estimation
		if(!Params.BANDIT){
			counts = new uint[SAS];
			for(uint c=0;c<SAS;++c){
			//	counts[c] = 0;
			//
				counts[c] = priorcountList[c];
			//				
			}
		}
		else{
			//	TODO: ADDED			
			cout << "Warning: BANDIT case not supported!\n";
			//			
			assert(S == 2);
			counts = new uint[SA];
			for(uint c=0;c<SA;++c){
				counts[c] = 0;
			}
		}
		
		countsSum = new double[SA];
		std::fill(countsSum,countsSum+SA,SampFact.getAlphaMean()*S);
		//	TODO: ADDED
		for (unsigned int ss = 0; ss < S; ++ss)
			for (unsigned int aa = 0; aa < A; ++aa)
				for (unsigned int observation = 0; observation < S;
				                                             ++observation)
					countsSum[ss*A+aa] += counts[ss*SA+S*aa+observation];
					
		//
		step = 0;
  

//		meand.open("BMCP_meandepth",std::ios_base::app);	
//		maxd.open("BMCP_maxdepth",std::ios_base::app);

	//	TODO:	ADDED
//	cout << "Initialising fast UCB table with exp. const. ";
//	cout << params.ExplorationConstant << "...  ";
			
//	UCB.resize(UCB_N);
//	for (unsigned int N = 0; N < UCB_N; ++N)
//	{
//		UCB[N].resize(UCB_n);
//		
//		UCB[N][0] = Infinity;
//		for (unsigned int n = 1; n < UCB_n; ++n)
//			UCB[N][n] = params.ExplorationConstant * sqrt(log(N + 1) / n);		
//	}
//	cout << "done" << endl;
	
	InitialisedFastUCB = true;
	//	TODO:	ADDED
	
}

BAMCP::~BAMCP()
{
	delete[] counts;
	delete[] countsSum;	
 
	delete[] RLPI;
	delete[] V;
	
	delete[] Q;
	for(uint s=0;s<S;++s)
		delete GreedyA[s];
	delete[] GreedyA;

	VNODE::Free(Root, Simulator);
  // TODO:	VNODE::FreeAll();
}

bool BAMCP::Update(uint ss, uint aa, uint observation, double reward)
{
    History.Add(aa, observation);
	
		//Update posterior
		if(!Params.BANDIT){
			counts[ss*SA+S*aa+observation] += 1;
			SampFact.updateCounts(ss,aa,observation);	
			countsSum[ss*A+aa] += 1;
		}
		else
			counts[aa*S+observation] += 1;

    //Q value update
		Q[ss*A+aa] += QlearningRate*(reward + Simulator.GetDiscount()*Q[observation*A+GreedyA[observation]->at(0)] - Q[ss*A+aa]);
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
		
    //Reuse previous subtree for next search 
    //Get to subtree
    QNODE& qnode = Root->Child(aa);
    VNODE* vnode = qnode.Child(observation);
    if(vnode && Params.ReuseTree){
      //Delete other subtrees but this one
      vnode->preventfree = true;
      VNODE::Free(Root,Simulator);
      vnode->preventfree = false;
      Root = vnode;
    }else{ 
      // Delete old tree and create new root
      VNODE::Free(Root,Simulator);
      VNODE* newRoot = ExpandNode();
      Root = newRoot;
    }	

    return true;
}

int BAMCP::SelectAction(uint current_state)
{
  UCTSearch(current_state);
  step++;
  return GreedyUCB(Root,false);;
}

void BAMCP::UCTSearch(uint state)
{
		
    ClearStatistics();
    int historyDepth = History.Size();
		double* parm = 0;
		double* p_samp = 0;
		Sampler* MDPSampler = 0;
		if(Params.BANDIT){
			parm = new double[2];
			p_samp = new double[Simulator.GetNumActions()];
		}
		else{
			MDPSampler = SampFact.getMDPSampler(counts,S,A,
					Simulator.R,Simulator.rsas,Simulator.GetDiscount());
		}
    
			
    for (int n = 0; n < Params.NumSimulations; n++)
    {
				SIMULATOR* mdp;
				if(!Params.BANDIT){
					// Sample MDP given counts	
					mdp = MDPSampler->updateMDPSample();
					
				}else if(Params.BANDIT){  //TODO Move this to a special BANDIT sampler?
					state = 0; //Ignore state itself
					//sample Bandit
					for(int a=0;a<Simulator.GetNumActions();++a){
						guez_utils::sampleDirichlet(parm,counts+a*2,2,1);
						p_samp[a] = parm[0];
					}
					mdp = new BANDIT(Simulator.GetNumActions(),p_samp,Simulator.GetDiscount());	
				}

				Status.Phase = SIMULATOR::STATUS::TREE;

        if (Params.Verbose >= 2)
        {
            cout << "Starting simulation" << endl;
        }

        TreeDepth = 0;
        PeakTreeDepth = 0;
        
				double totalReward = SimulateV(mdp, state, Root);
        StatTotalReward.Add(totalReward);
        StatTreeDepth.Add(PeakTreeDepth);

        if (Params.Verbose >= 2)
            cout << "Total reward = " << totalReward << endl;
        if (Params.Verbose >= 4)
            DisplayValue(4, cout);

        History.Truncate(historyDepth);

				if(Params.BANDIT)
					delete mdp;

		}
		if(!Params.BANDIT)
			delete MDPSampler;
		else if(Params.BANDIT){	
			delete[] parm;
			delete[] p_samp;
		}
    DisplayStatistics(cout);
}

double BAMCP::SimulateV(const SIMULATOR* mdp, uint state, VNODE* vnode)
{
    uint action = GreedyUCB(vnode, true);

    PeakTreeDepth = TreeDepth;
    if (TreeDepth >= Params.MaxDepth) // search horizon reached
        return 0;

    QNODE& qnode = vnode->Child(action);
    double totalReward = SimulateQ(mdp, state, qnode, action);
    vnode->Value.Add(totalReward);
    return totalReward;
}

double BAMCP::SimulateQ(const SIMULATOR* mdp, uint state, QNODE& qnode, uint action)
{
    uint observation;
    double immediateReward, delayedReward = 0;

    bool terminal = mdp->Step(state, action, observation, immediateReward);
    //assert(observation >= 0 && observation < mdp.GetNumObservations());
    History.Add(action, observation);
		
    if (Params.Verbose >= 3)
    {
        mdp->DisplayAction(action, cout);
        mdp->DisplayObservation(state, observation, cout);
        mdp->DisplayReward(immediateReward, cout);
        mdp->DisplayState(state, cout);
    }
    
		VNODE*& vnode = qnode.Child(observation);
    if (!vnode && !terminal && qnode.Value.GetCount() >= Params.ExpandCount)
        vnode = ExpandNode(); //&state);

    if (!terminal)
    {
        TreeDepth++;
        if (vnode)
            delayedReward = SimulateV(mdp, observation, vnode);
        else{
						if(Params.BANDIT){
							double bestP = ((BANDIT*)mdp)->getBestArmP();
							delayedReward = bestP/(1-mdp->GetDiscount()); //Rollout(mdp, state);
						}
						else{
							if(Params.RB < 0){
								delayedReward = Rollout(mdp, observation);
							}else{
								//Warning this will fail with lazy sampling factories
								assert(mdp->T != 0);
								MDPutils::valueIterationRmax(S,
										A,
										Simulator.rsas,
										mdp->T,
										Simulator.R,
										Simulator.GetDiscount(),
										0.0001,
										RLPI,
										V,counts,Params.RB);
								delayedReward = V[observation];
							}	
						}
						
				}
        TreeDepth--;
    }

    double totalReward = immediateReward + mdp->GetDiscount() * delayedReward;
    qnode.Value.Add(totalReward);
    return totalReward;
}

VNODE* BAMCP::ExpandNode()
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


int BAMCP::GreedyUCB(VNODE* vnode, bool ucb) const
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

double BAMCP::Rollout(const SIMULATOR* mdp, uint state)
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
					action = mdp->SelectRandom(state, Status);
				else{

					action = GreedyA[state]->at(rand() % GreedyA[state]->size());
				}
        terminal = mdp->Step(state, action, observation, reward);
        History.Add(action, observation);

        if (Params.Verbose >= 4)
        {
            mdp->DisplayAction(action, cout);
            mdp->DisplayObservation(state, observation, cout);
            mdp->DisplayReward(reward, cout);
            mdp->DisplayState(state, cout);
        }
				
				state = observation;
        totalReward += reward * discount;
        discount *= mdp->GetDiscount();
    }

    StatRolloutDepth.Add(numSteps);
    if (Params.Verbose >= 3)
        cout << "Ending rollout after " << numSteps
            << " steps, with total reward " << totalReward << endl;
    return totalReward;
}

/* TODO	double BAMCP::UCB[UCB_N][UCB_n];*/
/* TODO	bool BAMCP::InitialisedFastUCB = true;*/

//	TODO
//void BAMCP::InitFastUCB(double exploration) /*TODO: ADDED*/ {} /*TODO: ADDED*/
void BAMCP::InitFastUCB(double) /*TODO: ADDED*/ {} /*TODO: ADDED*/
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

//	TODO - Modified
double BAMCP::QRoot(uint action)
{
	QNODE& qnode = Root->Child(action);
     double q = qnode.Value.GetValue();
     
     return q;
}
//	TODO - Modified

//inline double BAMCP::FastUCB(int N, int n, double logN) const
inline double BAMCP::FastUCB(int, int n, double logN) const
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

void BAMCP::ClearStatistics()
{
    StatTreeDepth.Clear();
    StatRolloutDepth.Clear();
    StatTotalReward.Clear();
}

void BAMCP::DisplayStatistics(ostream& ostr) const
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

void BAMCP::DisplayValue(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "BAMCP Values:" << endl;
    Root->DisplayValue(history, depth, ostr);
}

void BAMCP::DisplayPolicy(int depth, ostream& ostr) const
{
    HISTORY history;
    ostr << "BAMCP Policy:" << endl;
    Root->DisplayPolicy(history, depth, ostr);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
