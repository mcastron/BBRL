
#ifndef RANDOMGEN_H
#define RANDOMGEN_H

#ifdef BOOST_FOUND
#define NB_GENERATOR 32
#endif

#include "utils.h"
#include "../ExternalLibs.h"


// ===========================================================================
/*
	\class 	RandomGen
	\author 	Castronovo Michael
	
	\brief 	A generator of random numbers.
	\date 	2014-12-13
*/
// ===========================================================================
class utils::RandomGen
{
	public:
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
			\brief		Sets the generator's seed.
			
			\param[seed	The seed to use.
		*/
		static void setSeed(long int seed);
		
		
		/**
			\brief	Returns a number in [0; 1[ range, following a uniform
					distribution.
			
			\return 	A number in [0; 1[ range, following a uniform
					distribution.
		*/			
		static double rand01_Uniform();


		/**
			\brief	Returns a number in ]0; 1[ range, following a uniform
					distribution.
			
			\return 	A number in ]0; 1[ range, following a uniform
					distribution.
		*/			
		static double rand01_open_Uniform();
		
		
		/**
			\brief	Returns a number following a gaussian distribution,
					defined by 'mean' and 'stDev'.
					
					Applies the Box-Muller approach.
			
			\return 	A number following a gaussian distribution,
					defined by 'mean' and 'stDev'.
		*/			
		static double rand_Gaussian(double mean, double stDev);
		
		
		/**
			\brief	Returns a number in [min; max[ range, following 
					a uniform	distribution.
			
			\return 	A number in [min; max[ range, following a uniform
					distribution.
		*/
		static double randRange_Uniform(double min, double max);
		
		
		/**
			\brief	Returns an integer in [min; max] range, following 
					a uniform	distribution.
			
			\return 	An integer in [min; max] range, following a uniform
					distribution.
		*/
		static int randIntRange_Uniform(int min, int max);
		
		
	private:
		// =================================================================
		//	Private static attributes
		// =================================================================
		#ifdef BOOST_FOUND
		/**
			\brief	Generates the seeds of the other generators.
		*/
		static boost::mt19937 seedGen_;
		
		
		/**
			\brief	The set of generators (one per thread).
		*/
		static std::map<boost::thread::id, boost::mt19937> generators_;
		
		
		/**
			\brief	The distribution to use for the
					'RandomGen::rand01_Uniform()' method.
		*/
		static boost::uniform_real<> rand01Dist_;
		
		
		/**
			\brief	Mutexes used to limit access of shared data structure
					to one single thread at the same time.
		*/
		static boost::mutex mutexSeedGen_, mutexGenerators_;
		#endif

	
		// =================================================================
		//	Private Constructor
		// =================================================================
		/**
			\brief	Private constructor in order to prevent any 
					instanciation of this class.
		*/
		RandomGen() {}
};

#endif

