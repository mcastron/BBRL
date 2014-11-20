#include "lazyMDP.h"
#include "../utils/guez_utils.h"

using namespace UTILS;

LazyMDP::LazyMDP(uint _S, uint _A, double discount, double* _reward,
		bool _rsas, LazyMDPSampler* _sampler)
:   S(_S), A(_A), sampler(_sampler)
{
		NumObservations = _S;
		NumActions = _A;
		Discount = discount;
		R = _reward;
		rsas = _rsas;

    RewardRange = 1;
			
		SA = S*A;
		Tresp = false;
		T = 0;	
		lT = new double*[SA];
		std::fill(lT,lT+SA,(double*)0);	

}

LazyMDP::~LazyMDP(){
	if(Tresp)
		delete[] T;

	for(uint i=0;i<SA;++i){
		if(lT[i] != 0)
			delete[] lT[i];
	}
	delete[] lT;	
}

uint LazyMDP::CreateStartState() const{ 
		return 0; //(rand() % S);
}


//void LazyMDP::Validate(const uint state) const{
void LazyMDP::Validate(const uint) const{

}

bool LazyMDP::Step(uint state, uint action, 
    uint& observation, double& reward) const
{
	uint index = state*A+action;
	if(lT[index] == 0){
		lT[index] = new double[S];
		sampler->getTransitionParam(state, action, lT[index]);
	}
	
	guez_utils::rng.multinom(lT[index],S,observation);
	if(rsas)	
		reward = R[state*SA+action*S+observation];
	else
		reward = R[state*A+action];

	//Never terminates
	return false;
}

//void LazyMDP::DisplayState(const uint state, std::ostream& ostr) const {}
void LazyMDP::DisplayState(const uint, std::ostream&) const {}

//void LazyMDP::DisplayObservation(const uint state, uint observation, std::ostream& ostr) const {}
void LazyMDP::DisplayObservation(const uint, uint, std::ostream&) const {}

void LazyMDP::DisplayAction(int action, std::ostream& ostr) const {
        ostr << action << std::endl;
}
