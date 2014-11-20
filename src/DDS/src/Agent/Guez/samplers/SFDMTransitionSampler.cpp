#include "SFDMTransitionSampler.h"
#include "../envs/basicMDP.h"

#include "../samplers/SFDMsamplerFactory.h"
//SFDMTransitionSampler functions
//-----------------------------------------------------


SFDMTransitionSampler::SFDMTransitionSampler(double* _P, uint _S) : P(_P), S(_S){

}

SFDMTransitionSampler::~SFDMTransitionSampler()
{
	delete[] P;
}
uint SFDMTransitionSampler::getNextStateSample(){
	uint sp;
	guez_utils::rng.multinom(P,S,sp);
	return sp;	
}

//SFDMTransitionParamSampler functions
//-----------------------------------------------------


SFDMTransitionParamSampler::SFDMTransitionParamSampler(const uint* _counts,
		double _alpha, uint _S, SFDMsamplerFactory* _pFact, uint _ind) :
	counts(_counts),alpha(_alpha), S(_S), pFact(_pFact), ind(_ind){
		
	dirParam = new double[S];
}

SFDMTransitionParamSampler::~SFDMTransitionParamSampler()
{
	delete[] dirParam;
}

void SFDMTransitionParamSampler::getNextTParamSample(double* P){
	//Sampling Method 2
	
	//  sample kd ~ PkD
	uint kd;
	guez_utils::rng.multinom(pFact->PkD[ind],S,kd);
	kd = kd + 1;
	//  sample Vd 
  //  (i.e. want kd-K0 elements not in SIG0)
	//  Use Knuth-Fisher-Yates's to
	//  sample kd+K0 elements without replacement
	
	uint n;
	uint tmp;
	for(uint i=0; i < std::min((size_t)S,kd+pFact->SIG0[ind]->size()); i++){
		n = rand() % (S-i);
		tmp = pFact->ordering[n];
		pFact->ordering[n] = pFact->ordering[i];
		pFact->ordering[i] = tmp;
	}
	std::fill(dirParam,dirParam+S,0);

	//TODO Make it independent of S by maintaining the set of non-zero entries of
	//dirParam and passing that to a spare dirichlet sampling function
	uint count = 0;
	uint i = 0;
	
	
	while(count < (kd-pFact->SIG0[ind]->size())){
		if(pFact->OFlag[ind][pFact->ordering[i]]==0){
			dirParam[pFact->ordering[i]] = alpha;
			count++;
		}
		i++;
	}	
	for(i=0; i < pFact->SIG0[ind]->size(); ++i){
		dirParam[pFact->SIG0[ind]->at(i)] = alpha+counts[pFact->SIG0[ind]->at(i)];
	}
	guez_utils::sampleDirichlet(P,dirParam,S);
}

//SFDMMDPSampler functions
//-----------------------------------------------------

SFDMMDPSampler::SFDMMDPSampler(const uint* _counts, double _alpha,
				uint _S, uint _A, double* _R, bool _rsas, double _gamma, SFDMsamplerFactory* _pFact):
	counts(_counts),alpha(_alpha), S(_S), A(_A), R(_R), rsas(_rsas),gamma(_gamma), pFact(_pFact){
		SA = S*A;		
		T = new double[SA*S];
		simulator = 0;

		dirParam = new double[S];

}
SFDMMDPSampler::~SFDMMDPSampler(){
	delete[] T;	
	if(simulator)
		delete simulator;
	delete[] dirParam;
}


SIMULATOR* SFDMMDPSampler::updateMDPSample(){
	if(simulator)
		delete simulator;

	uint sSA;	
	//Sample transitions from the counts
	for(uint ss=0;ss<S;++ss){
		sSA = ss*SA;
		for(uint aa=0;aa<A;++aa){
			uint lind = sSA+aa*S;	
			uint ind = ss*A+aa;
			
			//  sample kd ~ PkD
			uint kd;
			guez_utils::rng.multinom(pFact->PkD[ind],S,kd);
			kd = kd + 1;
			//  sample Vd 
			//  (i.e. want kd-K0 elements not in SIG0)
			//  Use Knuth-Fisher-Yates's to
			//  sample kd+K0 elements without replacement
			uint n;
			uint tmp;
			for(uint i=0; i < std::min((size_t)S,kd+pFact->SIG0[ind]->size()); i++){
				n = rand() % (S-i);
				tmp = pFact->ordering[n];
				pFact->ordering[n] = pFact->ordering[i];
				pFact->ordering[i] = tmp;
			}
			std::fill(dirParam,dirParam+S,0);

			//TODO Make it indepedent of S by maintaining the set of non-zero entries of
			//dirParam and passing that to a spare dirichlet sampling function
			uint count = 0;
			uint i = 0;

			while(count < (kd-pFact->SIG0[ind]->size())){
				if(pFact->OFlag[ind][pFact->ordering[i]]==0){
					dirParam[pFact->ordering[i]] = alpha;
					count++;
				}
				i++;
			}	
			for(i=0; i < pFact->SIG0[ind]->size(); ++i){
				dirParam[pFact->SIG0[ind]->at(i)] = alpha+counts[lind+pFact->SIG0[ind]->at(i)];
			}
		
			guez_utils::sampleDirichlet(T+lind,dirParam,S);
		}
	}
	simulator = new BasicMDP(S,A,gamma,R,rsas,T);
	return (SIMULATOR*) simulator;	
}
