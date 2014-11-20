
#ifndef UCB1_H
#define UCB1_H

#include "RandomGen.h"
#include "utils.h"
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\class 	UCB1
	\author 	Castronovo Michael
	
	\brief 	An interface to specialize for UCB1	
	
	\date 	2014-07-23
*/
// ===========================================================================
class utils::UCB1
{
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief		Constructor.
			
			\param[nArms_	The number of arms of this UCB1.
			\param[c_		The constant used in the UCB1 formula:
							I_t(i) = mu_i + c * sqrt(ln(n_t) / n_i)
		*/
		UCB1(unsigned int nArms_, double c_);

		
		/**
			\brief	Destructor.
		*/
		virtual ~UCB1() {}
		
		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief	Draw 'n' arms and return the index of the arm which
					has been drawn the most.
					
					The initialization draws (1 per arm) are part of 'n',
					which means that if 'n' < 'nArms', the UCB1 won't
					be initialized.
			
			\param[n	The number of draws.
			
			\return	The index of the arm which has been drawn the most.
		*/
		unsigned int run(unsigned int n);
		
		
		/**
			\brief	Return the top 'n' of the most selected arms.
		
			\param[n	The size of the top to return.	
			
			\return	The top 'n' of the most selected arms.
		*/
		std::vector<unsigned int> getTop(unsigned int n) const;
		
		
		/**
			\brief	Return the number of times arm 'i' has been drawn.
			
			\param[i	An arm.
			
			\return	The number of times arm 'i' has been drawn.
		*/
		unsigned int getNDraws(unsigned int i) const
		{
			assert(i < nArms);


			return nList[i];
		}
		
		
		/**
			\brief	Return the mean score obtained by arm 'i' when it
					was drawn.
			
			\param[i	An arm.
			
			\return	The mean score obtained by arm 'i' when it was drawn.
		*/
		double getMu(unsigned int i) const
		{
			assert(i < nArms);


			return muList[i];
		}


	protected:
		// =================================================================
		//	Protected methods
		// =================================================================
		/**
			\brief	Draw the arm 'i' and return its score.
			
			\param[i	The arm to draw.
		*/
		virtual double drawArm(unsigned int i) const = 0;


	private:
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The total number of arms.
		*/
		unsigned int nArms;
		
		
		/**
			\brief	The constant used in the UCB1 formula:
						I_t(i) = mu_i + c * sqrt(ln(n_t) / n_i)
		*/
		double c;
		
		
		/**
			\brief	The mean score observed for each arm.
		*/
		std::vector<double> muList;


		/**
			\brief	The number of draws for each arm.
		*/
		std::vector<unsigned int> nList;

	
		/**
			\brief	The total number of draws.
		*/
		unsigned int nSum;
};

#endif

