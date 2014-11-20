
#ifndef MDPSIMULATOR_H
#define MDPSIMULATOR_H

#include <exception>
#include "../../MDP/MDP.h"
#include "envs/simulator.h"

using namespace dds;


// ===========================================================================
/**
	\class 	MDPSimulator
	\author 	Castronovo Michael
	
	\brief 	A SIMULATOR defined by a MDP.
	          Used as a substitute for Guez's algorithms (cannot be used to
	          actually simulate a MDP).

	\date 	2014-09-15
*/
// ===========================================================================
class MDPSimulator : public SIMULATOR
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief      Constructor.
			            (can simulate 'mdp_' with 'bool Step()' method)
		*/
		MDPSimulator(MDP* mdp_, double gamma) : SIMULATOR(), mdp(mdp_)
		{
		   //   Check errors
		   assert(mdp_);
		   assert((gamma > 0.0) && (gamma <= 1.0));
		   
		   
		   unsigned int nX = mdp->getNbStates();
		   unsigned int nU = mdp->getNbActions();		   
		   const std::vector<double>& RV = mdp->getR();
		   
		   NumObservations = nX;
		   NumActions = nU;
		   Discount = gamma;
		   rsas = true;
		   
		   std::vector<std::vector<double>::const_iterator > maxList;
		   maxList = utils::search::max<vector<double> >(
		             RV.begin(), RV.end());		   
		   RewardRange = *(maxList[0]);   
		   
		   T = 0;
		   R = new double[nX*nU*nX];
		   for (unsigned int x = 0; x < nX; ++x)
		        for (unsigned int u = 0; u < nU; ++u)
		             for (unsigned int y = 0; y < nX; ++y)
		                  R[nX*nU*x + nX*u + y] = RV[nX*nU*x + nX*u + y];
		}
		
		
		/**
			\brief           Constructor.
			
			\param[iniState_ The initial state.
			\param[nX        The number of states.
			\param[nU        The number of actions.
			\param[RV        The reward function.
			\param[gamma     The discount factor.
		*/
		MDPSimulator(unsigned int iniState_,
		             unsigned int nX, unsigned int nU, 
		             std::vector<double> RV, double gamma) :
		                  SIMULATOR(), mdp(0), iniState(iniState_)
		{
		   //   Check errors
		   assert(iniState_ < nX);
		   assert(nX > 0);
		   assert(nU > 0);
		   assert(RV.size() == (nX * nU * nX));
		   assert((gamma > 0.0) && (gamma <= 1.0));
	   
		   
		   NumObservations = nX;
		   NumActions = nU;
		   Discount = gamma;
		   rsas = true;
		   
		   std::vector<std::vector<double>::const_iterator > maxList;
		   maxList = utils::search::max<vector<double> >(
		             RV.begin(), RV.end());		   
		   RewardRange = *(maxList[0]);
		   
		   T = 0;
		   R = new double[nX*nU*nX];
		   for (unsigned int x = 0; x < nX; ++x)
		        for (unsigned int u = 0; u < nU; ++u)
		             for (unsigned int y = 0; y < nX; ++y)
		                  R[nX*nU*x + nX*u + y] = RV[nX*nU*x + nX*u + y];
		}
		
		
		/**
			\brief	Destructor.
		*/
		virtual ~MDPSimulator()
		{
		   if (mdp) { delete mdp; }
		   delete[] R;
		}


		// =================================================================
		//	Public methods
		// =================================================================
		uint CreateStartState() const
		{
		   if (mdp) { return mdp->getIniState(); }
		   else     { return iniState;           }
		}


		bool Step(uint state, uint action, uint& observation, double& reward) 
		                                                               const
          {
               if (mdp == 0)
                     throw std::runtime_error("Unable to simulate the MDP!");
               
               else
               {
                    assert(state == mdp->getCurrentState());
                    
                    unsigned int y;
                    mdp->perform((unsigned int) action, y, reward);
                    observation = (uint) y;              
     
               
                    return false;
               }
          }
     
     
     private:
          // =================================================================
		//	Private attributes
		// =================================================================
		/**
		   \brief    The MDP on which this SIMULATOR is based on.
		*/
		MDP* mdp;
		
		
		/**
		   \brief    The initial state (not used of 'mdp' exists).
		*/
		unsigned int iniState;
};

#endif
