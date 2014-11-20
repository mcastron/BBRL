
#include "Chrono.h"

using namespace utils;


// ===========================================================================
//	Public Constructor
// ===========================================================================
Chrono::Chrono() { restart(); }


// ===========================================================================
//	Public methods
// ===========================================================================
void Chrono::restart() { gettimeofday(&startClock, NULL); }


double Chrono::get()
{
	struct timeval endClock;
	gettimeofday(&endClock, NULL);

	return 	(((double) 1000.0*endClock.tv_sec
					+ (double) endClock.tv_usec * .001)
			- ((double) 1000.0*startClock.tv_sec
					+ (double) startClock.tv_usec * .001));
}
