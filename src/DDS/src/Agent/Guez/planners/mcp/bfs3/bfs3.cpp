#include "bfs3.h"
#include <math.h>
#include "../../../utils/guez_utils.h"
#include <algorithm>

#include <map>
#include "../../../samplers/sampler.h"
#include "../../../envs/simulator.h"
#include "../../../samplers/samplerFactory.h"

#include "node.h"

using namespace std;
using namespace UTILS;

//-----------------------------------------------------------------------------

BFS3::PARAMS::PARAMS()
:   Verbose(0),
    D(100), //Max Depth
    C(10),  //Branching factor
		N(10),  //Number of trajectories
		gamma(0.95),
		Vmin(0),
		Vmax(1/(1-0.95))
{
}

BFS3::BFS3(const SIMULATOR& simulator, const PARAMS& params, SamplerFactory& sampFact)
:   Params(params),
		Simulator(simulator),
		SampFact(sampFact),
    TreeDepth(0)
{
    VNODE3::NumChildren = Simulator.GetNumActions();
		Root = 0;
		A = Simulator.GetNumActions();
		S = Simulator.GetNumObservations();
		SA = S*A;
		SAS = S*A*S;

		//Initialize transition counts for posterior estimation
		counts = new uint[SAS];
		for(uint c=0;c<SAS;++c){
			counts[c] = 0;
		}
		pcounts = new uint[SAS];
}

BFS3::~BFS3()
{
	delete[] counts;
	delete[] pcounts;
	VNODE3::FreeAll();
}

//bool BFS3::Update(uint state, uint action, uint observation, double reward)
bool BFS3::Update(uint state, uint action, uint observation, double)
{
    //History.Add(action, observation);
	
		//Update posterior
		counts[state*SA+S*action+observation] += 1;
		SampFact.updateCounts(state,action,observation);
    
		return true;
}

////Generate C transitions from a FDM model using counts
////and a fixed prior (1/S) count
//void BFS3::genTransitions(uint* sp, uint* pcounts){
//	//Create transition probs	
//	double* P = new double[S];
//	for(uint s=0;s<S;++s)
//		P[i] = (double) pcounts + 1/(double)S;
//	//sample C transitions
//	uint sampled = 0;
//	utils::rng.multinom(C,P,S,sp,sampled);
//
//}

inline double BFS3::getReward(uint ss, uint aa, uint sp){
	if(Simulator.rsas)
		return Simulator.R[ss*SA+aa*S+sp];
	else
		return Simulator.R[ss*A+aa];
}

uint BFS3::SelectAction(uint state)
{

	//int historyDepth = History.Size();

	std::vector<uint> besta;
	double bestq = -std::numeric_limits<double>::max();

	for(uint aa=0; aa < A; ++aa){

		double qa = 0;

		//Construct T(s,a) //TODO include prior in counts to avoid doing that?
		uint ii = state*SA+aa*S;
		//Get a next-state sampler from the factory
		Sampler* nextSSampler = SampFact.getTransitionSampler(counts+ii,state,aa,S);

		uint sp;
		for(uint c=0; c < Params.C; ++c){

			TreeDepth = 0;
			PeakTreeDepth = 0;

			//Sample transition
			sp = nextSSampler->getNextStateSample();

			if (Params.Verbose >= 2)
			{
				cout << "Starting simulation in state " << sp << endl;

			}
			//MTS using FSSS in belief-MDP
			double totalReward = FSSS(state,aa,sp);

			qa += (getReward(state,aa,sp) 
				    + Params.gamma*totalReward)/Params.C;	
			
			//History.Truncate(historyDepth);
			// Delete old tree and create new root
			VNODE3::FreeAll();
			Root = 0;
		}

		delete nextSSampler;

		if(qa > bestq){
			bestq = qa;
			besta.clear();
			besta.push_back(aa);
		}
		else if(qa == bestq)
			besta.push_back(aa);


		if (Params.Verbose >= 2)
			cout << "Reward for action " << aa << " = " << qa << endl;

	}
	return besta[rand() % besta.size()];	
}

double BFS3::FSSS(uint prevs, uint aa, uint state){
	for(uint n=0;n<Params.N;++n){
		//Copy current counts into planning counts
		memcpy(pcounts,counts,SAS*sizeof(uint));
		pcounts[prevs*SA+aa*S+state] += 1;
		FSSSRollout(Root,state,0);	
	}
	//return max_a U(s,a) at Root
	return Root->Us;
}


void BFS3::FSSSRollout(VNODE3*& vnode, uint state, uint depth){
	if(depth == Params.D)
		return;
	if(!vnode){
		vnode = ExpandNode(state);
		bellmanBackup(vnode, -1, state);
	}
	uint a = vnode->bestA();
	QNODE3& qnode = vnode->Child(a);
	//Select transition optimisticaly
	map<uint,VNODE3Pc>::iterator nextVNode = qnode.selectPath(Params.Vmax,Params.Vmin);

	if(Params.Verbose >= 2){
		std::cout << "D: " << depth << " a: " << a << " sp: " 
			<< nextVNode->first << std::endl; 
	}

	//Update pcounts
	pcounts[state*SA+a*S+nextVNode->first] += 1;
	FSSSRollout(nextVNode->second.v,nextVNode->first,depth+1);
	bellmanBackup(vnode, a, state);
}

//not used...
void BFS3::getQNodeValue(QNODE3& qnode, double& Usa, double& Lsa, uint ss, uint aa){
	Usa = 0;
	Lsa = 0;
	map<uint,VNODE3Pc>::iterator it;
	for(it=qnode.Children.begin();it != qnode.Children.end(); it++){
		if(it->second.v == 0){
			Usa += (getReward(ss,aa,it->first)+Params.gamma*Params.Vmax)*it->second.count/qnode.C;
			Lsa += (getReward(ss,aa,it->first)+Params.gamma*Params.Vmin)*it->second.count/qnode.C;
			//std::cout << "Not explored: " << it->second.count << " " << qnode.Usa << " " << qnode.Lsa << std::endl;
		}else{
			Usa += (getReward(ss,aa,it->first)+Params.gamma*it->second.v->Us)*it->second.count/qnode.C;
			Lsa += (getReward(ss,aa,it->first)+Params.gamma*it->second.v->Ls)*it->second.count/qnode.C;
			//std::cout << "Explored: " << it->second.count << " " << qnode.Usa << " " << qnode.Lsa << std::endl;
		}
	}
}
//TODO:could avoid recomputing constant parts of the sum
//that have not changed for the last rollout
//void BFS3::bellmanBackup(VNODE3* vnode, int action, uint s){
void BFS3::bellmanBackup(VNODE3* vnode, int, uint s){

	map<uint,VNODE3Pc>::iterator it;
	double maxU = Params.Vmin;
	double maxL = Params.Vmin;
	for(int aa=0;aa<vnode->NumChildren;++aa){
		QNODE3& qnode = vnode->Children[aa];
		qnode.Usa = 0;
		qnode.Lsa = 0;
		for(it=qnode.Children.begin();it != qnode.Children.end(); it++){
			if(it->second.v == 0){
				qnode.Usa += (getReward(s,aa,it->first)+Params.gamma*Params.Vmax)*it->second.count/qnode.C;
				qnode.Lsa += (getReward(s,aa,it->first)+Params.gamma*Params.Vmin)*it->second.count/qnode.C;
				//std::cout << "Not explored: " << it->second.count << " " << qnode.Usa << " " << qnode.Lsa << std::endl;
			}else{
				qnode.Usa += (getReward(s,aa,it->first)+Params.gamma*it->second.v->Us)*it->second.count/qnode.C;
				qnode.Lsa += (getReward(s,aa,it->first)+Params.gamma*it->second.v->Ls)*it->second.count/qnode.C;
				//std::cout << "Explored: " << it->second.count << " " << qnode.Usa << " " << qnode.Lsa << std::endl;
			}
		}

		if(qnode.Usa > maxU)
			maxU = qnode.Usa;
		if(qnode.Lsa > maxL)
			maxL = qnode.Lsa;
	}
	vnode->Us = maxU;
	vnode->Ls = maxL;

}

VNODE3* BFS3::ExpandNode(uint state) 
{
	VNODE3* vnode = VNODE3::Create();
	vnode->Us = Params.Vmax;
		vnode->Ls = Params.Vmin; 
		vnode->SetChildren(Params.Vmax,Params.Vmin);

		if (Params.Verbose >= 2)
    {
        //cout << "Expanding node: ";
        //History.Display(cout);
        //cout << endl;
    }
		//Sample transitions

	for(uint aa=0; aa < A; ++aa){

		//Construct T(s,a)
		uint ii = state*SA+aa*S;
		Sampler* nextSSampler = SampFact.getTransitionSampler(pcounts+ii,state,aa,S);

		uint sp;
		for(uint c=0; c < Params.C; ++c){
			//Sample transition
			sp =  nextSSampler->getNextStateSample();
			vnode->Child(aa).Add(sp);
		}
		delete nextSSampler;
	}
	return vnode;
}



//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
