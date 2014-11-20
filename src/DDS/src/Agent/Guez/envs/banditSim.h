#pragma once

#include "simulator.h"


class BANDIT : public SIMULATOR
{
public:

    BANDIT(int _numArms, double* _p, double discount);

    virtual void Validate(const uint& state) const;
    virtual uint CreateStartState() const;
    virtual bool Step(uint state, uint action, 
        uint& observation, double& reward) const;
        
    virtual void DisplayState(const uint& state, std::ostream& ostr) const;
    virtual void DisplayObservation(const uint& state, uint observation, std::ostream& ostr) const;
    virtual void DisplayAction(uint action, std::ostream& ostr) const;
		
		size_t getBestArmP(){return p[bestArm];}

protected:

    int numArms;
		double *p;
		size_t bestArm; 

};

