#pragma once

#include "simulator.h"
#include "../samplers/LazyMDPSampler.h"

class LazyMDP : public SIMULATOR
{
public:

    LazyMDP(uint _S, uint _A, double discount, double* _reward,
		bool _rsas, LazyMDPSampler* _sampler);
		~LazyMDP();

    virtual void Validate(const uint state) const;
    virtual uint CreateStartState() const;
    virtual bool Step(uint state, uint action, 
        uint& observation, double& reward) const;
        
   // void GeneratePreferred(const uint state,
   //     std::vector<int>& legal, const STATUS& status) const;

    virtual void DisplayState(const uint state, std::ostream& ostr) const;
    virtual void DisplayObservation(const uint state, uint observation, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;
	
protected:
		uint S,A;
		
		//Cached value
		uint SA;
		LazyMDPSampler* sampler;	

		double** lT;
private:
		bool Tresp;

};

