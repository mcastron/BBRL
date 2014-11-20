
#include "RandomGen.h"

using namespace utils;


// ===========================================================================
//	Public static methods.
// ===========================================================================
void RandomGen::setSeed(long int seed)
{	
	#ifdef BOOST_FOUND
	mutexSeedGen_.lock();
	mutexGenerators_.lock();

	seedGen_.seed(seed);
	
	boost::uniform_int<> dist(0, INT_MAX);
	std::map<boost::thread::id, boost::mt19937>::iterator it, end;
	for (it = generators_.begin(), end = generators_.end(); it != end; ++it)
		(it->second).seed(dist(seedGen_));
	
	mutexGenerators_.unlock();
	mutexSeedGen_.unlock();
	
	#else
	srand(seed);
	
	#endif
}


double RandomGen::rand01_Uniform()
{
	#ifdef BOOST_FOUND
	std::map<boost::thread::id, boost::mt19937>::iterator it;
	it = generators_.find(boost::this_thread::get_id());
	if (it == generators_.end())
	{
		mutexSeedGen_.lock();
		mutexGenerators_.lock();
	
		boost::uniform_int<> dist(0, INT_MAX);
		generators_[boost::this_thread::get_id()] =
									boost::mt19937(dist(seedGen_));
		
		mutexGenerators_.unlock();
		mutexSeedGen_.unlock();
		
		//	TODO - Not optimal
		it = generators_.find(boost::this_thread::get_id());
	}
	
	return rand01Dist_(it->second);
	
	#else
	return ((double) rand() / (double) RAND_MAX);
	
	#endif
}


double RandomGen::rand01_open_Uniform()
{
	return (RandomGen::randIntRange_Uniform(1, (RAND_MAX - 1))
			/ (double) RAND_MAX);
}


double RandomGen::rand_Gaussian(double mean, double stDev)
{
	//	Box-Muller approach
	double u1 = RandomGen::rand01_Uniform();
	double u2 = RandomGen::rand01_Uniform();
	double z0 = sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
	
	//	Adapted to the given 'mean' and 'stdev'
	return ((z0 * stDev) + mean);
}


double RandomGen::randRange_Uniform(double min, double max)
{
	return (RandomGen::rand01_Uniform() * (max - min)) + min; 
}


int RandomGen::randIntRange_Uniform(int min, int max)
{
	return std::floor(RandomGen::randRange_Uniform(min, max + 1));
}


// ===========================================================================
//	Private static attributes initialization
// ===========================================================================
#ifdef BOOST_FOUND
boost::mt19937 RandomGen::seedGen_;


std::map<boost::thread::id, boost::mt19937> RandomGen::generators_;


boost::uniform_real<> RandomGen::rand01Dist_(0.0, 1.0);


boost::mutex RandomGen::mutexSeedGen_;


boost::mutex RandomGen::mutexGenerators_;
#endif
