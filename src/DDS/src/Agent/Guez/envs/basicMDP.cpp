#include "basicMDP.h"
#include "../utils/guez_utils.h"

#include <iostream>
#include <fstream>

using namespace UTILS;

BasicMDP::BasicMDP(uint _S, uint _A, double discount, double* _reward,
		bool _rsas,
		const uint* counts)
:   S(_S), A(_A)
{
		NumObservations = _S;
		NumActions = _A;
		Discount = discount;
		R = _reward;
		rsas = _rsas;

    RewardRange = 1;
			
		SA = S*A;
		Tresp = true;
		T = new double[SA*S];
		//Sample transitions from the counts
		for(uint s=0;s<S;++s){
			for(uint a=0;a<A;++a){
				guez_utils::sampleDirichlet(T+s*SA+a*S,counts+(s*SA+a*S),S,1/(double)S);
			}
		}
}

BasicMDP::BasicMDP(uint _S, uint _A, double discount, double* _R,
		bool _rsas, double* _T): S(_S), A(_A){

	NumObservations = _S;
	NumActions = _A;
	Discount = discount;
	R = _R;
	rsas = _rsas;
	Tresp = false;			
	T = _T;

	SA = S*A;
	RewardRange = 1;
}

//Load from file
BasicMDP::BasicMDP(uint _S, uint _A, std::string filename, double discount){
	Discount = discount;	
	Tresp = true;
	double rmin = 99999;
	double rmax = -99999;
	std::ifstream ifs;
	ifs.open(filename.c_str());
	assert(ifs.good());
	ifs >>	S; 
	assert(S == _S);
	NumObservations = S;
	ifs >> A;
	assert(A == _A);
	NumActions = A;	
	SA = S*A;
	T = new double[SA*S];
	for(uint s=0;s<S;++s){
		for(uint a=0;a<A;++a){
			for(uint sp = 0; sp < S; ++sp){
				ifs >> T[s*SA+a*S+sp];
			}
		}
	}
	ifs >> rsas;
	if(rsas){
		R = new double[SA*S];
		for(uint s=0;s<S;++s){
			for(uint a=0;a<A;++a){
				for(uint sp = 0; sp < S; ++sp){
					ifs >> R[s*SA+a*S+sp];
					if(R[s*SA+a*S+sp] > rmax)
						rmax =  R[s*SA+a*S+sp];
					if(R[s*SA+a*S+sp] < rmin)
						rmin = R[s*SA+a*S+sp];
				}
			}
		}
	}
	else{
		R = new double[SA];
		for(uint s=0;s<S;++s){
			for(uint a=0;a<A;++a){
				ifs >> R[s*A+a];
				if(R[s*A+a] > rmax)
					rmax = R[s*A+a];
				if(R[s*A+a] < rmin)
					rmin = R[s*A+a];
			}
		}
	}
	if(ifs.good())
		ifs.close();
	
	RewardRange = rmax-rmin;
}


BasicMDP::~BasicMDP(){
	if(Tresp)
		delete[] T;
}

void BasicMDP::saveMDP(std::string filename){
	std::ofstream ofs;
	ofs.open(filename.c_str());
	ofs << S << " " << A << std::endl;
	for(uint s=0;s<S;++s){
		for(uint a=0;a<A;++a){
			for(uint sp = 0; sp < S; ++sp){
				ofs << T[s*SA+a*S+sp] << " ";
			}
			ofs << std::endl;
		}
	}	
	ofs << rsas << std::endl;
	if(rsas){
		for(uint s=0;s<S;++s){
			for(uint a=0;a<A;++a){
				for(uint sp = 0; sp < S; ++sp){
					ofs << R[s*SA+a*S+sp] << " ";
				}
				ofs << std::endl;
			}
		}
	}
	else{
		for(uint s=0;s<S;++s){
			for(uint a=0;a<A;++a){
				ofs << R[s*A+a] << " ";
			}
			ofs << std::endl;
		}
	}
	ofs.close();	
}

uint BasicMDP::CreateStartState() const
{ 
		return 0; //(rand() % S);
}


//void BasicMDP::Validate(const uint state) const{
void BasicMDP::Validate(const uint) const{

}

bool BasicMDP::Step(uint state, uint action, 
    uint& observation, double& reward) const
{
	guez_utils::rng.multinom(T+state*SA+action*S,S,observation);
	if(rsas)	
		reward = R[state*SA+action*S+observation];
	else
		reward = R[state*A+action];

	//Never terminates
	return false;
}


//void BasicMDP::GeneratePreferred(const uint state,
//    std::vector<int>& actions, const STATUS& status) const
void BasicMDP::GeneratePreferred(const uint,
    std::vector<int>& actions, const STATUS&) const
{
    for (uint a = 0; a < A; ++a)
			actions.push_back(a);
}


//void BasicMDP::DisplayState(const uint state, std::ostream& ostr) const
void BasicMDP::DisplayState(const uint, std::ostream&) const
{
    
}

//void BasicMDP::DisplayObservation(const uint state, uint observation, std::ostream& ostr) const
void BasicMDP::DisplayObservation(const uint, uint, std::ostream&) const
{
   
}

void BasicMDP::DisplayAction(int action, std::ostream& ostr) const
{
        ostr << action << std::endl;
}
