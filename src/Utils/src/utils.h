
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\namespace	utils
	\author 		Castronovo Michael
	
	\brief		A collection of general tools.
	
	\date		2015-02-11
*/
// ===========================================================================
namespace utils
{
	// ======================================================================
	//	Classes
	// ======================================================================
	/**
          \class    Chrono
          \brief    A chronometer to measure the wall time elapsed.
	*/
	class Chrono;
	
	
	/**
          \class    RandomGen
          \brief    A generator of random numbers.
	*/
	class RandomGen;
	
	
	/**
          \class    Thread
          \brief 	A C++ handler for pthreads.	
	*/
	class Thread;
	
	
	/**
          \class    ThreadException
          \brief    Exception
	*/
	class ThreadException;
	
	
	// ======================================================================
	//	Namespaces
	// ======================================================================
	/**
		\brief	This namespace gathers various algorithms.
	*/
	namespace algorithm
	{
	     /**
               \class    StoSOO
               \brief    An interface to specialize for StoSOO.
     	*/
     	class StoSOO;


     	/**
               \class    UCB1
               \brief    An interface to specialize for UCB1.
     	*/
     	class UCB1;
     	
     	
     	/**
               \class    UCT
               \brief    An interface to specialize for UCT.
     	*/
     	class UCT;
	}	
	

	/**
		\brief	This namespace gathers several functions related to
				the calculation of statistics data.
	*/
	namespace statistics
	{
		// =================================================================
		//	Details (to ignore)
		// =================================================================
		namespace details
		{			
			//	CI function
			//		Coefficient 'alpha' to adapt with respect to the
			//		confidence percentage.
			std::pair<double, double>
					computeCI(double alpha,
							double mean, double stDev, unsigned int n);
							
			
			//	Gamma function
			// 		The method for generating gamma variables appears in
			//		"A Simple Method for Generating Gamma Variables" by
			//		Marsaglia and Tsang, ACM Transactions on Mathematical
			//		Software, Vol. 26, No 3, Sep 2000, pages 363-372.
			//
			//		Adapted from Arthur Guez implementation:
			//			https://github.com/acguez/bamcp
			double gamma(double shape, double scale);			
		}
		
		
		// =================================================================
		//	Functions
		// =================================================================
		/**
			\brief		Compute the mean of 'data'.
			
			\param[data	The data for which to compute the mean.
			
			\return		The mean of 'data'.
		*/
		template<typename T>
		double computeMean(const std::vector<T>& data);
		
		
		/**
			\brief		Compute the variance of 'data'.
			
			\param[data	The data for which to compute the variance.
			
			\return		The variance of 'data'.
		*/		
		template<typename T>
		double computeVariance(
				double mean, const std::vector<T>& data);
		
		template<typename T>
		double computeVariance(const std::vector<T>& data)
		{
			return computeVariance<T>(computeMean<T>(data), data);
		}
		
		
		/**
			\brief		Compute the standard deviation of 'data'.
			
			\param[data	The data for which to compute the standard
						deviation.
			
			\return		The standard deviation of 'data'.
		*/
		template<typename T>
		double computeStandardDeviation(
				double mean, const std::vector<T>& data)
		{
			return sqrt(computeVariance<T>(mean, data));
		}
		
		template<typename T>
		double computeStandardDeviation(const std::vector<T>& data)
		{
			return sqrt(computeVariance<T>(data));
		}
		
	
		/**
			\brief		Compute the confidence interval of the mean of
						'data' with 68% confidence.
			
			\param[mean	The mean of 'data'.
			\param[stDev	The standard deviation of 'data'.
			\param[data	The data for which to compute the CI.
			
			\return		A pair where 'first' is the CI lower bound and
						'second' is the CI upper bound.
		*/		
		template<typename T>
		std::pair<double, double> computeCI68(
				double mean, double stDev, const std::vector<T>& data);
	
		template<typename T>
		std::pair<double, double> computeCI68(
				double mean, const std::vector<T>& data)
		{
			return computeCI68<T>(
					mean, computeStandardDeviation<T>(data), data);
		}
	
		template<typename T>
		std::pair<double, double> computeCI68(const std::vector<T>& data)
		{
			return computeCI68<T>(
					computeMean<T>(data),
					computeStandardDeviation<T>(data), data);
		}		
		
	
		/**
			\brief		Compute the confidence interval of the mean of
						'data' with 95% confidence.
			
			\param[mean	The mean of 'data'.
			\param[stDev	The standard deviation of 'data'.
			\param[data	The data for which to compute the CI.
			
			\return		A pair where 'first' is the CI lower bound and
						'second' is the CI upper bound.
		*/			
		template<typename T>
		std::pair<double, double> computeCI95(
				double mean, double stDev, const std::vector<T>& data);
	
		template<typename T>
		std::pair<double, double> computeCI95(
				double mean, const std::vector<T>& data)
		{
			return computeCI95<T>(
					mean, computeStandardDeviation<T>(data), data);
		}
	
		template<typename T>
		std::pair<double, double> computeCI95(const std::vector<T>& data)
		{
			return computeCI95<T>(
					computeMean<T>(data),
					computeStandardDeviation<T>(data), data);
		}
	
		
		/**
			\brief		Compute the confidence interval of the mean of
						'data' with 99% confidence.
			
			\param[mean	The mean of 'data'.
			\param[stDev	The standard deviation of 'data'.
			\param[data	The data for which to compute the CI.
			
			\return		A pair where 'first' is the CI lower bound and
						'second' is the CI upper bound.
		*/			
		template<typename T>
		std::pair<double, double> computeCI99(
				double mean, double stDev, const std::vector<T>& data);
	
		template<typename T>
		std::pair<double, double> computeCI99(
				double mean, const std::vector<T>& data)
		{
			return computeCI99<T>(
					mean, computeStandardDeviation<T>(data), data);
		}
	
		template<typename T>
		std::pair<double, double> computeCI99(const std::vector<T>& data)
		{
			return computeCI99<T>(
					computeMean<T>(data),
					computeStandardDeviation<T>(data), data);
		}
		
		
		/**
			\brief		Sample from a dirichlet distribution whose
						parameters are 'theta'.
					
						Adapted from Arthur Guez implementation:
							https://github.com/acguez/bamcp
			
			\param[nTheta	The index of the first element of 'theta' to use.
			\param[theta	The vector of parameters of the dirichlet
						distribution.
						(from theta[hTheta] to theta [hTheta + (n - 1)])	
			\param[hP		The index of the first element 'P' to use.
			\param[P		The vector where to store the results.
						(from P[hP] to P[hP + (n - 1)])
			\param[n		The total number of parameters to use.
						
			
		*/
		void sampleDirichlet(
				unsigned int hTheta, const std::vector<double>& theta,
				unsigned int hP, std::vector<double>& P,
				unsigned int n);
		
		void sampleDirichlet(
				const std::vector<double>& theta, std::vector<double>& P);
		
		std::vector<double> sampleDirichlet(
                    const std::vector<double>& theta);


          /**
               \brief    Compute a statistical value 'Z' to perform the
                         'paired Z-test'.

               \param[x  A series of data.
               \param[y  A series of data.
               
               \return   The value of 'Z'.
          */
          template<typename T>
          double computePairedZ(const std::vector<T>& x,
                                const std::vector<T>& y);
          
          
          /**
               \brief    Return the threshold value of 'Z' in the
                         'bilateral paired Z-test' with 95% confidence.

               \return   The threshold value of 'Z' in the 'bilateral paired
                         Z-test' with 95% confidence.
          */
          double getBilateralPairedZThreshold95();


          /**
               \brief    Return the threshold value of 'Z' in the
                         'unilateral paired Z-test' with 95% confidence.

               \return   The threshold value of 'Z' in the 'unilateral paired
                         Z-test' with 95% confidence.
          */
          double getUnilateralPairedZThreshold95();
	}
	
	
	/**
		\brief	This namespace gathers several functions related to
				the compression of a string (based on zlib)
	*/
	namespace compression
	{
	     // =================================================================
		//	Functions
		// =================================================================
		/**
			\brief		Compress a STL string using zlib with given
						compression level and return the binary data.
						
							'compressionLevel' possible values:
								Z_NO_COMPRESSION		(0)
								Z_BEST_SPEED			(1)
								Z_BEST_COMPRESSION		(9)
								Z_DEFAULT_COMPRESSION	(-1)
	
						http://panthema.net/2007/0328-ZLibString.html	
	
			\param[str	The STL string to compress.
			
			\return		The binary data.
		*/
		std::string compressStr(const std::string& str, int compressionlevel)
										throw (std::runtime_error);
		
		
		/**
			\brief		Decompress an STL string using zlib and return
						the original data.
	
						http://panthema.net/2007/0328-ZLibString.html
			
			\param[str	The binary data.
			
			\return		The original string.
		*/
		std::string uncompressStr(const std::string& str)
										throw (std::runtime_error);
	}
	
	
	/**
		\brief	This namespace gathers several functions consisting to
				search specific elements inside an STL container of
				type 'C'.
	*/
	namespace search
	{
	     // =================================================================
		//	Functions
		// =================================================================
		/**
			\brief		Return a list of iterator pointing on the
						minimal elements between 'begin' and 'end'
						('end' excluded).
						
						Undefined behaviour if 'begin' and 'end' are
						not from the same container or 'begin' lies
						after 'end'.
			
			\param[begin	An iterator to the first element of the range
						through which the search will be performed.
			
			\param[end	An iterator to the last element ('end' excluded)
						of the range through which the search will be
						performed.
			
			\return		A list of iterator pointing on the
						minimal elements between 'begin' and 'end'.
		*/
		template<typename C>
		std::vector<typename C::const_iterator> min(
				const typename C::const_iterator begin,
				const typename C::const_iterator end);

		
		/**
			\brief		Return a list of iterator pointing on the
						maximal elements between 'begin' and 'end'
						('end' excluded).
						
						Undefined behaviour if 'begin' and 'end' are
						not from the same container or 'begin' lies
						after 'end'.
			
			\param[begin	An iterator to the first element of the range
						through which the search will be performed.
			
			\param[end	An iterator to the last element ('end' excluded)
						of the range through which the search will be
						performed.
			
			\return		A list of iterator pointing on the
						maximal elements between 'begin' and 'end'.
		*/
		template<typename C>
		std::vector<typename C::const_iterator> max(
				const typename C::const_iterator begin,
				const typename C::const_iterator end);
	}
	
	
	/**
          \brief	This namespace gathers several functions for parsing
                    command-line parameters.
	*/
	namespace parsing
	{
	    // ==================================================================
	    //	Classes
	    // ==================================================================
	    /**
	         \class    ParsingException
	         \brief    Exception
	    */
	    class ParsingException: public std::exception
         {
          	public:
          		// =======================================================
          		//	Public Constructors/Destructors
          		// =======================================================
          		/**
          			\brief 		Constructor.
          			
          			\param[opt	The option concerned by the error.
          		*/
          		ParsingException(std::string opt)
          		{
                         msg = "Error while parsing option \'" + opt + "\'\n";
          		}
          		
          		
          		/**
          		     \brief    Destructor.
          		*/
          		virtual ~ParsingException() throw() {}
          
          
          		// =======================================================
          		//	Public methods
          		// =======================================================
          		/**
          			\brief		Returns the error message.
          			
          			\return		The error messIAge.
          		*/
          		virtual const char* what() const throw()
          		{
          		   return msg.c_str();
          		}
          	
          	
          	private:
          		// =======================================================
          		//	Private attributes
          		// =======================================================
          		/**
          			\brief	The error message.
          		*/
          		std::string msg;
         };
	    
	    
	    // ==================================================================
	    //	Functions
	    // ==================================================================
	    /**
	         \brief         Return the value of option 'opt', throws a
	                        ParsingException if an error occurs.
	         
	         \param[argc    The number of command-line arguments.
	         \param[argv    Command-line arguments.
	         \param[opt     The option for which to retrieve the value.
	         
	         \return        The value of option 'opt', throws a
	                        ParsingException if an error occurs.
	    */
	    std::string getValue(int argc, char* argv[],
	              std::string opt) throw (ParsingException);
	    
	    
	    /**
	         \brief         Return the 'n' values of option 'opt', throws a
	                        ParsingException if an error occurs.
	         
	         \param[argc    The number of command-line arguments.
	         \param[argv    Command-line arguments.
	         \param[opt     The option for which to retrieve the value.
	         \param[n       The number of values to retrieve.
	         
	         \return        The 'n' values of option 'opt', throws a
	                        ParsingException if an error occurs.
	    */
	    std::vector<std::string> getValues(int argc, char* argv[],
	              std::string opt, unsigned int n) throw (ParsingException);
	    
	    
	    /**
	         \brief         Return true if flag 'f' is present, false else.
	         
	         \param[argc    The number of command-line arguments.
	         \param[argv    Command-line arguments.
	         \param[opt     The option for which to retrieve the value.
	         
	         \return        True if flag 'f' is present, false else.
	    */
	    bool hasFlag(int argc, char* argv[], std::string opt);
	}
	
	
	/**
          \brief	This namespace gathers several classes and functions
                    concerning the manipulation of formulas.
	*/
	namespace formula
	{
	    // ==================================================================
	    //	Functions
	    // ==================================================================
	    /**
	         \class    Token
	         \brief    Interface of a token, an element of a formula.
	    */
	    class Token;
	    
	    
	    /**
	         \class    TokenException
	         \brief    Exception
	    */
	    class TokenException;
	    
	    
	    /**
	         \class    Constant
	         \brief    A Token representing a constant value.
	    */
	    class Constant;
	    
	    
	    /**
	         \class    Variable
	         \brief    A Token representing a variable value.
	    */
         class Variable;
         
         
         /**
               \class    AbsOp
               \brief    A Token representing an operator (absolute value).
         */
         class AbsOp;
         
         
         /**
               \class    LnOp
               \brief    A Token representing an operator (logarithm).
         */
         class LnOp;
         
         
         /**
               \class    SqrtOp
               \brief    A Token representing an operator (square root).
         */
         class SqrtOp;
         
         
         /**
               \class    InvOp
               \brief    A Token representing an operator (inverse).
         */
         class InvOp;
         
         
         /**
               \class    OppOp
               \brief    A Token representing an operator (opposite).
         */
         class OppOp;
         
         
         /**
               \class    SubOp
               \brief    A Token representing an operator (subtraction).
         */
         class SubOp;
         
         
         /**
               \class    DivOp
               \brief    A Token representing an operator (division).
         */
         class DivOp;
         
         
         /**
               \class    AddOp
               \brief    A Token representing an operator (addition).
         */
         class AddOp;
         
         
         /**
               \class    MulOp
               \brief    A Token representing an operator (multiplication).
         */
         class MulOp;
         
         
         /**
               \class    MinOp
               \brief    A Token representing an operator (minimum).
         */
         class MinOp;
         
         
         /**
               \class    MaxOp
               \brief    A Token representing an operator (maximum).
         */
         class MaxOp;
         
         
         /**
               \class    AvgOp
               \brief    A Token representing an operator (average value).
         */
         class AvgOp;
         
         
         /**
               \class    Formula
               \brief    Defines a Formula through a set of tokens, interpreted
                         in RPN notation.
         */
	    class Formula;
	    
	    
	    /**
               \class    FormulaException
               \brief    Exception
	    */
	    class FormulaException;
	}
}

#include "utils.tpp"


#endif
