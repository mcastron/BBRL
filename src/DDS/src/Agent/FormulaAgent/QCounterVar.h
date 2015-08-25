
#ifndef QCOUNTERVAR_H
#define QCOUNTERVAR_H

#include "QVar.h"
#include "../../dds.h"
#include "../../MDPDistribution/DirMultiDistribution.h"


// ===========================================================================
/*
	\class 	QCounterVar
	\author 	Castronovo Michael
	
	\brief 	A variable for the FormulaAgent's, which represents a
	          Q-function based on counter model.

	\date 	2015-07-09
*/
// ===========================================================================
/* final */ class dds::QCounterVar : public QVar
{		
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
          /**
			\brief	Constructor.
			
			\param[is	The 'ifstream' containting the data representing
					the QCounterVar to load.
					(can either be compressed or uncompressed)
		*/
		QCounterVar(std::istream& is);
		
		
		/**
               \brief         Constructor.
               
               \param[nX_     The number of states.
               \param[nU_     The number of actions.
               \param[N_      The list of prior observations.
		*/
		QCounterVar(unsigned int nX_, unsigned int nU_,
		            const std::vector<double>& N_);

          
          /**
			\brief	Destructor.
		*/
		~QCounterVar();
		
		
		// =================================================================
		//	Public static methods
		// =================================================================
		/**
               \brief              Generate a QCounterVar by EDA, where each
                                   individual is evaluated with respect to
                                   its performances on the prior distribution
                                   when used alone and its distance to
                                   variables in 'varList'.
                                   
                                   In brief, we want to find a good variable
                                   which is also as different as possible than
                                   those we already have.
               
               \param[nX           The number of states.
               \param[nU           The number of actions.
               \param[prior        The prior distribution.
               \param[N            The number of MDPs to draw when evaluating
                                   the individuals.
               \param[gamma        The discount factory to use when evaluating
                                   the individuals on MDPs.
               \param[T            The length of the trajectories when
                                   evaluating the individuals on MDPs.
               \param[mu           The initial mean of the gaussians.
               \param[sigma        The initial st. deviation of the gaussians.
               \param[b            The number of individuals to select among
                                   the population.
               \param[np           The size of the population at each step.
               \param[accuracy     The maximal value of the st. deviation of
                                   a gaussian before stopping the search.
               \param[alpha        The weight of the distance score.
               \param[beta         The weight of the update score (of the
                                   distance score).
               \param[varList      A list of variables.
		*/
		static QCounterVar* generate(unsigned int nX, unsigned int nU,
		                             const MDPDistribution* prior,
		                             unsigned int N,
		                             double gamma, unsigned int T,
		                             double mu, double sigma,
		                             unsigned int b, unsigned int np,
		                             double accuracy,
		                             double alpha, double beta,
		                             const vector<QCounterVar*>& varList);
		
		
		
		/**
			\brief	Return the string representation of this class name.
			
			\return	The string representation of this class name.
		*/
		static std::string toString() { return "QCounterVar"; }


		// =================================================================
		//	Public methods
		// =================================================================
		/**
               \brief    Initialization.
                         (used to initialize 'iniModel')
		*/
		void init(const dds::MDPDistribution* mdpDistrib)
                                                  throw (FVariableException);


          /**
               \brief    Return the number of states.
               
               \return   The number of states.
          */
          unsigned int getNbStates() const { return nX; }


          /**
               \brief    Return the number of actions.
               
               \return   The number of actions.
          */
          unsigned int getNbActions() const { return nU; }


          /**
               \brief    Return the list of prior observations.

               \return   The list of prior observations.
          */
          const std::vector<double>& getN() const { return N; }


          /**
			\brief	Return a clone of this QCounterVar.
			
			\return	A clone of this QCounterVar.
		*/
		QCounterVar* clone() const
		{
               return cloneInstance<QCounterVar>(this);
		}


          /**
			\brief	Return the name of the class of this object.
					This method should be overloaded by any derived class.
			
			\return	The name of the class of this object.
		*/
		std::string getClassName() const
		{
		   return QCounterVar::toString();
		}


		/**
			\brief	Serialize this Object.
					If overloaded, the new 'serialize()' method should
					call the 'serialize()' method of the base class
					before doing anything else.
		*/
		void serialize(std::ostream& os) const;


		/**
			\brief	Deserialize this Object.
					If overloaded, the new 'deserialize()' method should
					call the 'deserialize()' method of the base class
					before doing anything else.
		*/
		void deserialize(std::istream& is) throw (SerializableException);


     private:
          // =================================================================
		//	Private static methods
		// =================================================================
		/**
               \brief         Evaluate a QCounterVar based on its performance
                              on a set of MDPs, and its "distance"
                              to the QCountersVars in 'varList'.
               
               \param[var     The variable to evaluate.
               \param[prior   A prior, on which the variable will be trained
                              on before being evaluated.
               \param[mdps    The set of MDPs on which we evaluate 'var'.
               \param[gamma   The discount factor to use when testing 'var'
                              on 'mdps'.
               \param[T       The horizon limite to use when testing 'var'
                              on 'mdps'.
               \param[varList Used to compute an additional score inversly
                              proportional to the 'distance' between 'var'
                              and 'varList'.
               \param[alpha   The weight of the init score (of the distance
                              score).
               \param[beta    The weight of the update score (of the distance
                              score).
               
               \return        The score of 'var'.
		*/
		static double evaluate(
		        QCounterVar* var,
		        const MDPDistribution* prior,
		        const vector<MDP*>& mdps,
		        double gamma, unsigned int T,
		        double alpha, double beta,
		        const std::vector<QCounterVar*>& varList);
	    
	    
	    /**
	         \brief         Compute the distance between two variables.
	         
	         \param[alpha   The weight of the init score (of the distance
                             score).
              \param[beta    The weight of the update score (of the distance
                             score).
	         \param[var1    A variable.
	         \parma[var2    A variable.

	         \return        The distance between two variables.
	    */
	    static double getDistance(double alpha, double beta,
	                              QCounterVar* var1, QCounterVar* var2);
		
     
          // =================================================================
		//	Private attributes
		// =================================================================
		/**
		   \brief    The number of states.
		*/
		unsigned int nX;


		/**
		   \brief    The number of actions.
		*/
		unsigned int nU;
		
		
		/**
               \brief    The list of prior observations.
		*/
		std::vector<double> N;
};

#endif
