
#include "Chrono.h"

using namespace utils;


// ===========================================================================
//	Public Constructor
// ===========================================================================
Chrono::Chrono() { restart(); }


// ===========================================================================
//	Public methods
// ===========================================================================
void Chrono::pause()
{
     if (!paused)
     {
          update();
          paused = true;
     }
}


void Chrono::resume()
{
     if (paused)
     {
          gettimeofday(&startClock, NULL);
          paused = false;
     }
}


void Chrono::restart(bool paused_)
{
     paused = paused_;
     timeElapsed = 0.0;
     if (!paused) { gettimeofday(&startClock, NULL); }
}


double Chrono::get()
{
	if (!paused) { update(); }
	return timeElapsed;
}


void Chrono::update()
{
     struct timeval endClock;
	gettimeofday(&endClock, NULL);

     timeElapsed += (((double) 1000.0*endClock.tv_sec
					+ (double) endClock.tv_usec * .001)
			   - ((double) 1000.0*startClock.tv_sec
					+ (double) startClock.tv_usec * .001));

     startClock = endClock;
}
