
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include "../ExternalLibs.h"


// ===========================================================================
/**
	\namespace	utils
	\author 		Castronovo Michael
	
	\brief		A collection of general tools.
	
	\date		2015-05-12
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
          \brief    A generator of random numberÒs.
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
	
	
	/**
          \brief    This namespace gathers several classes and functions
                    to create graphs using gnuplot.
	*/
	namespace gnuplot
     {          
          // ==================================================================
          //   Enum definitions
          // ==================================================================
          /**
               \enum     NoiseFilterType
               \brief    A noise filtering condition.
          */
          enum NoiseFilterType
          {
               NFT_MIN, NFT_MAX, NFT_MEAN
          };

          
          // ==================================================================
          //    Classes
          // ==================================================================
          /**
               \class    GnuplotOptions
               \brief    Defines the Gnuplot options.
          */
          class GnuplotOptions
          {
               public:
                    // ========================================================
                    //   Public Constructor
                    // ========================================================
                    /**
                         \brief              Constructor.
                         \param[fontSize     The size of the font.
                    */
                    GnuplotOptions(unsigned int fontSize = 12) :
                              withValue (""), inBox(false)
                    {
                         std::stringstream sstr;
                         sstr << "postscript eps enhanced color font ";
                         sstr << "\'Helvetica," << fontSize << "\'";
                         setOption("terminal", sstr.str());

                         setOption("output", "\'plot-graph.eps\'");
                         setOption("key", "inside right bottom");
                    }


                    // ========================================================
                    //   Public methods
                    // ========================================================
                    /**
                         \brief         Add the following line to the header
                                        of the gnuplot script:
                                             'set [option] [value]'
                         
                         \param[option  The option to 'set'.
                         \param[value   The value of the option.
                    */
                    void setOption(std::string option, std::string value)
                    {
                         for (unsigned int i = 0; i < setOptions.size(); ++i)
                         {
                              if (setOptions[i] == option)
                              {
                                   values[i] = value;
                                   return;
                              }
                         }
                         
                         setOptions.push_back(option);
                         values.push_back(value);
                    }
                    
                    
                    /**
                         \brief         Add the following line to the header
                                        of the gnuplot script:
                                             'unset [option]'
                         
                         \param[option  The option to 'unset'.
                    */
                    void unsetOption(std::string option)
                    {
                         for (unsigned int i = 0; i < unsetOptions.size(); ++i)
                              if (unsetOptions[i] == option) { return; }
                         
                         unsetOptions.push_back(option);
                    }
                    
                    
                    /**
                         \brief         When plotting a curve with 'plot()',
                                        add the following sequence to each
                                        curve:
                                             '... with [value]'.

                         \param[value   The value of 'with' option.
                    */
                    void setWith(std::string value) { withValue = value; }
                    
                    
                    /**
                         \brief    Return the 'with' option, added to each
                                   curve definition when plotted.

                         \return   The 'with' option, added to each curve
                                   definition when plotted.
                    */
                    std::string getWith() const { return withValue; }                


                    /**
                         \brief         Set the 'inBox' option.
                         
                         \param[inBox_  The value of 'inBox' option.
                    */
                    void setInBox(bool inBox_) { inBox = inBox_; }
                    
                    
                    /**
                         \brief    Return the value of 'inBox' option.
                         
                         \return   The value of 'inBox' option.
                    */
                    bool getInBox() const { return inBox; }


                    /**
                         \brief    Return the gnuplot script header
                                   corresponding to the options defined.

                         \return   The gnuplot script header corresponding
                                   to the options defined.
                    */
                    std::string getScriptOptions() const
                    {
                         std::string scriptOptions = "";
                         for (unsigned int i = 0; i < setOptions.size(); ++i)
                         {
                              scriptOptions +=
                                        "set " +
                                        setOptions[i] + " " + values[i] + "\n";
                         }

                         for (unsigned int i = 0; i < unsetOptions.size(); ++i)
                         {
                              scriptOptions +=
                                        "unset " + unsetOptions[i] + "\n";
                         }
                         
                         return scriptOptions;
                    }

               private:
                    // ========================================================
                    //   Private attributes
                    // ========================================================
                    /**
                         \brief    The 'set'/'unset' options.
                    */
                    std::vector<std::string> setOptions, unsetOptions;
                    
                    
                    /**
                         \brief    The values of the 'set' options.
                    */
                    std::vector<std::string> values;
                    
                    
                    /**
                         \brief    The 'with' options.
                    */
                    std::string withValue;
                    
                    
                    /**
                         \brief    If true, the 2D-plane is split in 'areas',
                                   where are all dots within the area are
                                   occupied by the same functions.
                    */
                    bool inBox;
          };
          
          
          // =================================================================
		//	Details (to ignore)
		// =================================================================
		namespace details
		{
		     // Uses 'StoSOO' algorithm to find the best spot for putting
               // a label into a polygon.
               class LabelPositionOptim;
		   
		   
		     // Represent a point of the space
		     class Point
		     {
		          public:
                         Point(double x_, double y_,
                               std::set<unsigned int> list_ =
                                        std::set<unsigned int>()) :
                                             x(x_), y(y_), list(list_) {}


                         double getX() const { return x; }
                         double getY() const { return y; }

                         void applyLog10X() { x = log10(x); }
                         void applyLog10Y() { y = log10(y); }

                         void apply10PowX() { x = pow(10.0, x); }
                         void apply10PowY() { y = pow(10.0, y); }

                         void addToList(unsigned int f) { list.insert(f); }
                         
                         
                         std::set<unsigned int> getList() const
                         {
                              return list;
                         }
                         

                         bool operator==(const Point& p) const
                         {
                              if (p.getX() == x && p.getY() == y)
                                   return (list == p.list);
                              
                              return false;
                         }
		        
		        
                    private:
                         double x, y;
                         std::set<unsigned int> list;
		     };
		     
		     
		     // Represent an edge
		     class Edge
		     {
                    public:
                         Edge(Point from_, Point to_) : from(from_), to(to_) {}


                         const Point& getFrom() const { return from; }
                         const Point& getTo()   const { return to;   }
                         
                         
                         void reverse()
                         {
                              Point tmp = from;
                              from      = to;
                              to        = tmp;
                         }


                         bool operator==(const Edge& e)
                         {    
                              return ((from == e.getFrom() && to == e.getTo())
                                             || (from == e.getTo()
                                                       && to == e.getFrom()));
                         }


                    private:
                         Point from, to;
		     };
		     
		     
		     // Represent a path
		     class Path
		     {
                    public:
                         Path() : surfaceHasChanged(true) {}
                         
                         
                         bool add(Edge e)
                         {
                              if (empty())
                              {
                                   edges.push_back(e);
                                   surfaceHasChanged = true;
                                   
                                   double x1 = e.getFrom().getX();
                                   double x2 = e.getTo().getX();
                                   double y1 = e.getFrom().getY();
                                   double y2 = e.getTo().getY();
                                   
                                   xMin = min(x1, x2);
                                   xMax = max(x1, x2);
                                   yMin = min(y1, y2);
                                   yMax = max(y1, y2);
                                   
                                   return true;
                              }


                              Edge& lastEdge = edges.back();
                              if (lastEdge.getTo() == e.getFrom())
                              {
                                   edges.push_back(e);
                                   surfaceHasChanged = true;
                                   
                                   double x1 = e.getFrom().getX();
                                   double x2 = e.getTo().getX();
                                   double y1 = e.getFrom().getY();
                                   double y2 = e.getTo().getY();
                                   
                                   xMin = min(xMin, x1, x2);
                                   xMax = max(xMax, x1, x2);
                                   yMin = min(yMin, y1, y2);
                                   yMax = max(yMax, y1, y2);
                                   
                                   return true;
                              }


                              return false;                              
                         }
                         
                         
                         bool isLoop() const
                         {
                              if (empty()) { return false; }

                              return (edges.back().getTo()
                                        == edges[0].getFrom());
                         }
                         
                         
                         void reverse()
                         {
                              std::vector<Edge> nEdges;
                              for (int i = size() - 1; i >= 0; --i)
                              {
                                   Edge e = edges[i];
                                   e.reverse();
                                   nEdges.push_back(e);
                              }
                              
                              edges = nEdges;
                         }
                         
                         
                         std::vector<std::pair<int, int> >
                                   getCommonEdges(Path path) const
                         {
                              //   Optimization trick #1
                              if (empty() || path.empty()
                                        || get(0).getTo().getList()
                                             != path.get(0).getTo().getList())
                              {
                                   return std::vector<std::pair<int, int> >();
                              }
                              
                              
                              //   Optimization trick #2
                              std::vector<std::pair<double, double> > bounds;
                              bounds = path.getBounds();
                              
                              double xMin2 = bounds[0].first;
                              double xMax2 = bounds[0].second;
                              double yMin2 = bounds[1].first;
                              double yMax2 = bounds[1].second;
                              
                              if (xMax < xMin2 || xMax2 < xMin
                                        || yMax < yMin2 || yMax2 < yMin)
                              {
                                   return std::vector<std::pair<int, int> >();
                              }
                              
                              
                              //   Check all edges
                              std::vector<std::pair<int, int> > commonEdges;
                              for (int i = 0; i < size(); ++i)
                              {
                                   for (int j = 0; j < path.size(); ++j)
                                   {
                                        if (get(i) == path.get(j))
                                        {
                                             std::pair<int, int> p(i, j);
                                             commonEdges.push_back(p);
                                        }
                                   }
                              }

                              return commonEdges;
                         }


                         std::vector<std::pair<double, double> >
                                   getBounds() const
                         {
                              std::vector<std::pair<double, double> > bounds;

                              bounds.push_back(
                                   std::pair<double, double>(xMin, xMax));

                              bounds.push_back(
                                   std::pair<double, double>(yMin, yMax));

                              return bounds;
                         }


                         std::vector<Path> split(std::vector<int> cuts) const;
                         double computeSurface();

          
                         Edge get(unsigned int i) const { return edges[i]; }
                         unsigned int size() const { return edges.size(); }
                         bool empty() const { return edges.empty(); }


                    private:
                         std::vector<Edge> edges;
                         double surface;
                         bool surfaceHasChanged;
                         double xMin, xMax, yMin, yMax;
                         
                         double min(double a, double b) const
                         {
                              return (a < b) ? a : b;
                         }
                         
                         double min(double a, double b, double c) const
                         {
                              return min(min(a, b), c);
                         }
                         
                         double max(double a, double b) const
                         {
                              return (a > b) ? a : b;
                         }
                         
                         double max(double a, double b, double c) const
                         {
                              return max(max(a, b), c);
                         }
		     };
		     
		     
		     // Represent a polygon
		     class Polygon
		     {
                    public:
                         Polygon() {}
                         
                         
                         bool isValid() const
                         {
                              if (extLoop.empty() || !extLoop.isLoop())
                                   return false;

                              for (int i = 0; i < intLoops.size(); ++i)
                              {
                                   const Path& cur = intLoops[i];
                                   if (cur.empty() || !cur.isLoop())
                                        return false;
                              }

                              return true;
                         }
                    
                         
                         Path getExtLoop() const { return extLoop; }


                         //   Does not check if 'nExtLoop' is external
                         bool setExtLoop(Path nExtLoop)
                         {
                              if (nExtLoop.empty() || !nExtLoop.isLoop())
                                   return false;

                              extLoop = nExtLoop;
                              return true;
                         }
                         
                         
                         std::vector<Path> getIntLoops() const
                         {
                              return intLoops;
                         }
                         
                         
                         //   Does not check if all loops in 'nIntLoops' are
                         //   internal
                         bool setIntLoops(std::vector<Path> nIntLoops)
                         {
                              for (int i = 0; i < nIntLoops.size(); ++i)
                              {
                                   Path& cur = nIntLoops[i];
                                   if (cur.empty() || !cur.isLoop())
                                        return false;
                              }
                              
                              intLoops = nIntLoops;
                              return true;
                         }

                         
                         bool mergeWith(const Polygon& poly);
                    
                         
                         Point getCenter(bool logScaleX, bool logScaleY) const;
                         
                         
                         std::set<unsigned int> getList() const
                         {
                              return extLoop.get(0).getFrom().getList();
                         }
                         
                         
                         void print() const;
                    
                    
                    private:
                         Path extLoop;
                         std::vector<Path> intLoops;
                         
                         
                         std::vector<std::pair<double, double> >
                                   getBounds() const
                         {
                              return extLoop.getBounds();
                         }
		     };


		     // Return a pack corresponding to similar points. A point is
		     // part of the pack if the shortest distance between this point
		     // and a point of the pack is at most 'epsilon'
		     // (first coordinate). The data in 'dataV' have to be sorted.
               std::vector<std::pair<double, double> > getFirstPack(
                         const std::vector<std::pair<double, double> >& dataV,
                         double epsilon);
                         
                         
               // The 'plot' function for handling 'inBox' case.
               std::vector<Polygon> getPolygons(const std::vector<
                         std::vector<std::pair<double, double> > >& data,
                         bool logScaleX = false, bool logScaleY = false);
		}
         
         
          // ==================================================================
	     //	Functions
	     // ==================================================================
	     /**
	          \brief        Plot the data on a 2D graph.
	          \param[opt    The gnuplot options to use.
	          \param[data   The data to plot (1 vector per curve).
	          \param[titles Defines the title associated to each curve.
	          \param[bounds Defines the bounds associated to each point of
	                        each curve.
	     */
          void plot(GnuplotOptions opt,
                    std::vector<
                              std::vector<std::pair<double, double> > > data,
                    const std::vector<std::string>& titles,
                    std::vector<std::vector<
                              std::pair<double, double> > > bounds
                                   = std::vector<std::vector<
                                             std::pair<double, double> > >());


          /**
               \brief         Remove the noise present on the data by merging
                              too close points (if the preceeding point is at
                              a distance of at most 'epsilon' with respect to
                              the first coordinate).
                              
                              The point replacing a set of similar points is
                              depending of the type of filter used
                              (chosen by 'nft').
               
               \param[data    The data to filter (must be SORTED by their first
                              coordinate).

               \param[epsilon The filtering threshold value.
               \param[nft     The type of filter to apply.
               
               \return        The filtered data.
          */
          std::vector<std::pair<double, double> > removeNoise(
                         const std::vector<std::pair<double, double> >& dataV,
                         double epsilon, NoiseFilterType nft);
     }
     
     
     namespace latex
     {
          // ==================================================================
          //   Details (to ignore)
          // ==================================================================
          namespace details
          {
               //   Split time 't' (in ms) in days, hours, minutes, seconds
               //   and milliseconds.
               void splitTime(double t,
                              unsigned int& days, unsigned int& hours,
                              unsigned int& minutes, unsigned int& seconds,
                              unsigned int& milliseconds);
          }


          // ==================================================================
	     //	Classes
	     // ==================================================================
	     /**
               \class    Cell
               \brief    Represent a latex table cell.
	     */
          class Cell
          {
               public:
                    // ========================================================
                    //   Public Constructor/Destructor
                    // ========================================================
                    /**
                         \brief    Constructor.
                    */
                    Cell() : bold(false) {}


                    /**
                         \brief    Destructor.
                    */
                    virtual ~Cell() {}

                    
                    // ========================================================
                    //   Public methods
                    // ========================================================
                    /**
                         \brief    Return a string representing the content of
                                   this Cell, formatted for latex output.

                         \return   A string representing the content of
                                   this Cell, formatted for latex output.
                    */
                    virtual std::string getStr() const = 0;


                    /**
                         \brief    Set this cell in bold.
                    */
                    void setBold()   { bold = true; }


                    /**
                         \brief    If this cell has been set in bold
                                   previously, cancel it.                                   
                    */
                    void unsetBold() { bold = false; }
                    
               protected:
                    // ========================================================
                    //   Protected methods
                    // ========================================================
                    /**
                         \brief    Return true if this cell is in bold, false
                                   else.

                         \return   True if this cell is in bold, false else.
                    */
                    bool isBold() const { return bold; }


               private:
                    // ========================================================
                    //   Private attributes
                    // ========================================================
                    /**
                         \brief    Tru if this cell is in bold, false else.
                    */
                    bool bold;
          };


          /**
               \class    StrCell
               \brief    Represent a latex table cell containing a string.
          */
          class StrCell : public Cell
          {
               public:
                    // ========================================================
                    //   Public Constructor/Destructor
                    // ========================================================
                    /**
                         \brief         Constructor.
                         
                         \param[str_    The string to represent.
                    */
                    StrCell(std::string str_) : str(str_) {}


                    // ========================================================
                    //   Public methods
                    // ========================================================
                    /**
                         \brief    Return a string representing the content of
                                   this Cell, formatted for latex output.

                         \return   A string representing the content of
                                   this Cell, formatted for latex output.
                    */
                    std::string getStr() const
                    {
                         if (isBold()) { return "\\textbf{" + str + "}"; }
                         else          { return str;                     }
                    }
               
               
               private:
                    // ========================================================
                    //   Public attributes
                    // ========================================================
                    /**
                         \brief    The string to represent.
                    */
                    std::string str;
          };


          /**
               \class    NumberCell
               \brief    Represent a latex table cell containing a number.
          */
          class NumberCell : public Cell
          {
               public:
                    // ========================================================
                    //   Public Constructor/Destructor
                    // ========================================================
                    /**
                         \brief         Constructor.
                         
                         \param[value_  The value to represent.
                    */
                    NumberCell(double value_) : value(value_) {}
                    
                    
                    // ========================================================
                    //   Public methods
                    // ========================================================
                    /**
                         \brief    Return a string representing the content of
                                   this Cell, formatted for latex output.

                         \return   A string representing the content of
                                   this Cell, formatted for latex output.
                    */
                    std::string getStr() const
                    {
                         std::stringstream sstr;
                         sstr << "$";
                         
                         if (isBold()) { sstr << "\\boldsymbol{"; }
                         sstr << round(100.0*value)/100.0;
                         if (isBold()) { sstr << "}"; }
                         sstr << "$";
                         
                         return sstr.str();
                    }


               private:
                    // ========================================================
                    //   Private attributes
                    // ========================================================
                    /**
                         \brief    The value to represent.
                    */
                    double value;
          };


          /**
               \class    NumberIntervalCell
               \brief    Represent a latex table cell containing a bounded
                         number.
          */
          class NumberIntervalCell : public Cell
          {
               public:
                    // ========================================================
                    //   Public Constructor/Destructor
                    // ========================================================
                    /**
                         \brief              Constructor.
                         
                         \param[lowerBound_  The lower bound of the value to
                                             represent.
                         \param[upperBound_  The upper bound of the value to
                                             represent.
                    */
                    NumberIntervalCell(double lowerBound_, double upperBound_) :
                              lowerBound(lowerBound_),
                              upperBound(upperBound_) {}
                    
                    
                    // ========================================================
                    //   Public methods
                    // ========================================================
                    /**
                         \brief    Return a string representing the content of
                                   this Cell, formatted for latex output.

                         \return   A string representing the content of
                                   this Cell, formatted for latex output.
                    */
                    std::string getStr() const
                    {
                         double mid   = ((upperBound + lowerBound) / 2.0);
                         double delta = (upperBound - mid);
                         
                         std::stringstream sstr;
                         sstr << "$";
                         if (isBold()) { sstr << "\\boldsymbol{"; }
                         sstr << round(100.0*mid)/100.0;
                         sstr << " \\pm " << round(100.0*delta)/100.0;
                         if (isBold()) { sstr << "}"; }
                         sstr << "$";
                         
                         
                         return sstr.str();
                    }


               private:
                    // ========================================================
                    //   Private attributes
                    // ========================================================
                    /**
                         \brief    The lower bound of the value to represent.
                    */
                    double lowerBound;

                    
                    /**
                         \brief    The upper bound of the value to represent.
                    */
                    double upperBound;
          };

          
          /**
               \class    TimeCell
               \brief    Represent a latex table cell containing a time value.
          */
          class TimeCell : public Cell
          {
               public:
                    // ========================================================
                    //   Public Constructor/Destructor
                    // ========================================================
                    /**
                         \brief              Constructor.
                         
                         \param[timeInMs     The time value to represent
                                             (in ms).
                    */
                    TimeCell(double timeInMs)
                    {
                         details::splitTime(
                                   timeInMs,
                                   days, hours, minutes, seconds, milliseconds);
                    }
                    
                    
                    // ========================================================
                    //   Public methods
                    // ========================================================
                    /**
                         \brief    Return a string representing the content of
                                   this Cell, formatted for latex output.

                         \return   A string representing the content of
                                   this Cell, formatted for latex output.
                    */
                    std::string getStr() const
                    {
                         unsigned int value;
                         std::string unit;

                         if (days > 0)
                         {
                              value = (days + (hours >= 12 ? 1 : 0));
                              unit = "d";
                         }

                         else if (hours > 0)
                         {
                              value = (hours + (minutes >= 30 ? 1 : 0));
                              unit = "h";
                         }

                         else if (minutes > 0)
                         {
                              value = (minutes + (seconds >= 30 ? 1 : 0));
                              unit = "m";
                         }

                         else if (seconds > 0)
                         {
                              value = (seconds + (milliseconds >= 500 ? 1 : 0));
                              unit = "s";
                         }

                         else
                         {
                              value = milliseconds;
                              unit = "ms";
                         }
                         
                         std::stringstream sstr;

                         sstr << "$";
                         if (isBold()) { sstr << "\\boldsymbol{"; }
                         sstr << "\\sim " << value;
                         if (isBold()) { sstr << "}"; }
                         sstr << "$";
                         
                         if (isBold()) { sstr << "\\textbf{"; }
                         sstr << unit;
                         if (isBold()) { sstr << "}"; }
                         
                         return sstr.str();
                    }


               private:
                    // ========================================================
                    //   Private attributes
                    // ========================================================
                    /**
                         \brief    The number of days/hours/minutes/seconds/
                                   milliseconds contained in the value to
                                   represent, such that the sum is a close
                                   approximation of the real value.
                    */
                    unsigned int days, hours,minutes, seconds, milliseconds;
          };


          // ==================================================================
	     //	Functions
	     // ==================================================================
	     /**
	         \brief              Create a latex table in 'outputFile'.
	         
	         \param[grid         The content of the table.
	         \param[hLines       Put an horizontal line before row 'i'.
	         \param[vLines       Put a vertical line after column 'i'.
	         \param[columnWidth  Fix the width of each column
	                             (default: adapted to the content).
	     */
          void table(const std::vector<std::vector<Cell*> >& grid,
                     std::string outputFile,
                     std::vector<unsigned int> hLines
                              = std::vector<unsigned int>(),
                     std::vector<unsigned int> vLines
                              = std::vector<unsigned int>(),
                     std::vector<double> columnWidth
                              = std::vector<double>());
          
     }
}

#include "utils.tpp"


#endif
