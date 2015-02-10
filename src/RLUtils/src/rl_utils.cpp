
#include "rl_utils.h"

// ---------------------------------------------------------------------------
//	'rl_utils::simulation' namespace
// ---------------------------------------------------------------------------
// ===========================================================================
//   Functions
// ===========================================================================
double rl_utils::simulation::computeDSR(
          const std::vector<double>& rewardList, double gamma)
{
     assert((gamma > 0.0) && (gamma <= 1.0));
	
	double dsr = 0.0;
	double cGamma = 1.0;
	for (unsigned int i = 0; i < rewardList.size(); ++i)
	{
		dsr += (cGamma * rewardList[i]);
		cGamma *= gamma;
	}
	
	return dsr;
}
