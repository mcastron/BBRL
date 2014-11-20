#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "../utils/utils2.h"
#include <iostream>
#include <math.h>

typedef unsigned int uint;

class SIMULATOR
{
public:

    struct STATUS
    {
        STATUS();
        
        enum
        {
            TREE,
            ROLLOUT,
            NUM_PHASES
        };
        
        int Phase;
    };

    SIMULATOR();
    SIMULATOR(int numActions, int numObservations, double discount = 1.0);    
    virtual ~SIMULATOR();

    // Create start start state (can be stochastic)
    virtual uint CreateStartState() const = 0;

    // Update state according to action, and get observation and reward. 
    // Return value of true indicates termination of episode (if episodic)
    virtual bool Step(uint state, uint action, 
        uint& observation, double& reward) const = 0;
    
    // Sanity check
    virtual void Validate(const uint& state) const;

    int SelectRandom(const uint& state,
        const STATUS& status) const;

    // Textual display
    virtual void DisplayState(const uint& state, std::ostream& ostr) const;
    virtual void DisplayAction(int action, std::ostream& ostr) const;
    virtual void DisplayObservation(const uint& state, uint observation, std::ostream& ostr) const;
    virtual void DisplayReward(double reward, std::ostream& ostr) const;

    // Accessors
    int GetNumActions() const { return NumActions; }
    int GetNumObservations() const { return NumObservations; }
    bool IsEpisodic() const { return false; }
    double GetDiscount() const { return Discount; }
    double GetRewardRange() const { return RewardRange; }
    double GetHorizon(double accuracy, int undiscountedHorizon = 100) const;

		double* T;
		double* R;
		
		//Reward R(s,a,s') ? (otherwise reward function is R(s,a))
		bool rsas;
		
	//	ADDED (Michael)
	void SetDiscount(double discount)
	{
		assert(discount > 0 && discount <= 1);
		Discount = discount;
	}
	//	ADDED

protected:

    int NumActions, NumObservations;
    double Discount, RewardRange;
};

#endif // SIMULATOR_H
