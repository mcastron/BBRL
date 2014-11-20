#include "simulator.h"

using namespace std;
using namespace UTILS;

SIMULATOR::STATUS::STATUS()
:   Phase(TREE)
{
}

SIMULATOR::SIMULATOR() 
:   
	NumActions(0),
	NumObservations(0),
	Discount(1.0),
	RewardRange(1.0)
{
}

SIMULATOR::SIMULATOR(int numActions, int numObservations, double discount)
:   NumActions(numActions),
    NumObservations(numObservations),
    Discount(discount)
{ 
    assert(discount > 0 && discount <= 1);
}

SIMULATOR::~SIMULATOR() 
{ 
}

//void SIMULATOR::Validate(const uint& state) const 
void SIMULATOR::Validate(const uint&) const 
{ 
}

//int SIMULATOR::SelectRandom(const uint& state,
//    const STATUS& status) const
int SIMULATOR::SelectRandom(const uint&,
    const STATUS&) const
{
  return Random(NumActions);
}

//void SIMULATOR::DisplayState(const uint& state, ostream& ostr) const 
void SIMULATOR::DisplayState(const uint&, ostream&) const 
{
}

void SIMULATOR::DisplayAction(int action, ostream& ostr) const 
{
    ostr << "Action " << action << endl;
}

//void SIMULATOR::DisplayObservation(const uint& state, uint observation, ostream& ostr) const
void SIMULATOR::DisplayObservation(const uint&, uint observation, ostream& ostr) const
{
    ostr << "Observation " << observation << endl;
}

void SIMULATOR::DisplayReward(double reward, std::ostream& ostr) const
{
    ostr << "Reward " << reward << endl;
}

double SIMULATOR::GetHorizon(double accuracy, int undiscountedHorizon) const 
{ 
    if (Discount == 1)
        return undiscountedHorizon;
    return log(accuracy) / log(Discount);
}
