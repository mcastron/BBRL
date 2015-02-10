
#ifndef STOSOO_H
#define STOSOO_H

#include <iostream>

#include "../RandomGen.h"
#include "../utils.h"
#include "../../ExternalLibs.h"


// ===========================================================================
/*
	\class 	StoSOO
	\author 	Castronovo Michael
	
	\brief 	An interface to specialize for StoSOO.
	
	\date 	2015-01-28
*/
// ===========================================================================
class utils::algorithm::StoSOO
{
	public:
	    	// =================================================================
		//	Public static methods
		// =================================================================
		/**
               \brief    Return the suggested value of 'k' (number of
                         evaluations of 'f' per node) with respect to 'n' (the
                         total number of evaluations).
                         
                              ==> k = (n / (log(n) * log(n) * log(n))

               \return   The suggested value of 'k'.
		*/
		static unsigned int getNbEvalPerNode(unsigned int n)
		{
               if (n <= 1) { return 1; }
               
               unsigned int k = floor(n / (log(n) * log(n) * log(n)));
               return ((k > 0) ? k : 1);
		}


          /**
               \brief    Return the suggested value of 'hMax' (maximal depth)
                         with respect to 'n' (the total number of evaluations).
                         
                              ==> k = (n / (log(n) * log(n) * log(n))
                              ==> hMax = sqrt(n / k)

               \return   The suggested value of 'hMax.
		*/
		static unsigned int getMaxDepth(unsigned int n)
		{
               if (n <= 1) { return 1; }

               unsigned hMax = floor(sqrt(n / (double) getNbEvalPerNode(n)));
               return ((hMax > 0) ? hMax : 1);
		}

          
          /**
               \brief    Return the suggested value of 'delta' (confidence
                         parameter) with respect to 'n' (the total number of
                         evaluations).

               \return   The suggested value of 'delta'.
		*/
		static double getDelta(unsigned int n)
		{
               if (n == 0) { return 1.0; }

               return (1.0 / sqrt(n));
		}

	
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
               \brief           Constructor.
			
			\param[d_        The number of parameters of 'f'.
			\param[K_        The arity of the tree to develop (odd number).
			\param[k_        The maximal number of evaluations per node.
			\param[hMax_     The maximal depth of the tree to develop.
			\param[delta_    The confidence parameter.
			
		*/
		StoSOO(unsigned int d_, unsigned int K_,
                 unsigned int k_, unsigned int hMax_, double delta_,
                 const std::vector<std::pair<double, double> >& boundsList);


		/**
			\brief	Destructor.
		*/
		virtual ~StoSOO();

		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
               \brief    Develop a KD-Tree following the Sto-SOO algorithm
                         with 'n' evaluations of 'f', and return the optimal
                         solution found.
			
			\param[n_ The number of evaluations of 'f'.
			
			\return   The optimal solution found.

		*/
		std::vector<double> run(unsigned int n_) throw (std::exception);
		
          
          /**
               \brief    Print the tree.
          */
		void print() { print(0, root); }
	
	
	protected:
		// =================================================================
		//	Protected methods
		// =================================================================
		/**
               \brief    Represent the function to optimize.
                         Return a noisy evaluation of 'f' in 'x'.

               \param[x  The point in which to evaluate 'f'.

               \return   A noisy evaluation of 'f' in 'x'.
		*/
		virtual double f(const std::vector<double>& x) const
                                                  throw (std::exception) = 0;
                                                  

          /**
               \brief    Return the number of dimensions of the domain.
               
               \return   The number of dimensions of the domain.
		*/
		unsigned int getNbDimensions() const { return d; }


	private:		
		// =================================================================
		//	Private Class
		// =================================================================
		/**
			\brief	Defines a StoSOO node.
		*/
		class StoSOONode
		{
			public:
				// =======================================================
				//	Public Constructor/Destructor.
				// =======================================================
				/**
					\brief			Constructor
					
                         \param[StoSOO_	     A pointer to a StoSOO.
                         \param[boundsList_  The bounds defining the part of
                                             space governed by this node.
                    
				*/
				StoSOONode(StoSOO* StoSOO_, unsigned int depth_,
				           const std::vector<
                                   std::pair<double, double> >& boundsList_);
				
				
				/**
					\brief	Destructor.
				*/
				~StoSOONode();


				// =======================================================
				//	Public methods
				// =======================================================
				/**
                         \brief    Evaluate the function 'f' one time on the
                                   mean sample.
				*/
				void eval() throw (std::exception);


				/**
                         \brief    Split this node if possible.
				*/
				void split();
				
				
				/**
                         \brief	Return the parameters of the mean sample
                                   from the part of the space defined from this
							node.
					
					\return	The parameters of the mean sample from the
					          part of the space defined from this node.
				*/
                    std::vector<double> getMeanSample() const
                    {
                         return meanSample;
                    }
                    
                    
                    /**
                         \brief    Return the mean evaluation of 'f' on the
                                   mean sample.
                         
                         \return   The mean evaluation of 'f' on the mean
                                   sample.
                    */
                    double getMeanEval() const { return meanEval; }
                    
                    
                    /**
                         \brief    Return the number of evaluations of 'f'
                                   on the mean sample.
                         
                         \return   The number of evaluations of 'f' on the mean
                                   sample.
                    */
                    unsigned int getNbEval() const { return nbEval; }
                    
                    
                    /**
                         \brief    Compute the score ('b') of this node.
                    */
                    double getScore() const
                    {
                         if (nbEval == 0) { return HUGE_VAL; }
                         
                         const unsigned int& n     = stoSOO->n;
                         const unsigned int& k     = stoSOO->k;
                         const double&       delta = stoSOO->delta;
                         
                         return (getMeanEval()
                                   + sqrt(log(n*k/delta) / (2*nbEval)));
                    }
                    
                    
                    /**
                         \brief           Set 'meanEval' and 'nbEval' to
                                          'meanEval_' and 'nbEval_'
                                          respectively.
                                         
                         \param[meanEval_ The new value of 'meanEval_'. 
                         \param[nbEval_   The new value of 'nbEval_'.
                    */
                    void setData(double meanEval_, double nbEval_)
                    {
                         meanEval = meanEval_;
                         nbEval   = nbEval_;
                    }
                    

                    /**
                         \brief    Return true if this node is a leaf,
                                   false else.

                         \return   True if this node is a leaf, false else.
                    */
                    bool isLeaf() const { return childs.empty(); }


                    /**
                         \brief    Return the list of child nodes.
                         
                         \return   The list of child nodes.
                    */                    
                    std::vector<StoSOONode*> getChilds() const
                    {
                         return childs;
                    }

                    
                    /**
                         \brief    Print this node (no carriage return).
				*/
                    void print() const
                    {
                         //   Head
                         std::cout << "[ ";


                         //   'meanEval' & 'nbEval'
                         std::cout << meanEval << ", " << nbEval;


                         //   'meanSample
                         std::cout << ", < " << meanSample[0];
                         for (unsigned int i = 1; i < meanSample.size(); ++i)
                              std::cout << ", " << meanSample[i];
                         std::cout << " >";


                         //   'boundsList'                         
                         std::cout << ", < ";
                         std::cout << "[ " << boundsList[0].first << "; ";
                         std::cout << boundsList[0].second << " ]";
                         for (unsigned int i = 1; i < boundsList.size(); ++i)
                         {
                              std::cout << ", ";
                              std::cout << "[ " << boundsList[i].first << "; ";
                              std::cout << boundsList[i].second << " ]";
                         }
                         std::cout << " >";
                         
                         
                         //   Tail
                         std::cout << " ]";
                    }


			private:
				// =======================================================
				//	Private attributes
				// =======================================================
				/**
					\brief	A StoSOO.
				*/
				StoSOO* stoSOO;


                    /**
                         \brief    The depth of this node.
                    */
                    unsigned int depth;

				
				/**
					\brief	The bounds defining the part of space
							governed by this node.
				*/
				const std::vector<std::pair<double, double> > boundsList;
				
				
				/**
                         \brief    The parameters of the mean sample from the
					          part of the space defined from this node.
				*/
				std::vector<double> meanSample;
				
				
				/**
                         \brief    The mean evaluation of 'f' on the mean
                                   sample of this node.
				*/
				double meanEval;
				
				
				/**
                         \brief    The number of evaluations of 'f' on the mean
                                   sample of this node.
				*/
				unsigned int nbEval;


				/**
                         \brief    The childs of this StoSOONode.
				*/
				std::vector<StoSOONode*> childs;
		};
		
		
		/**
               \brief    A comparator for comparing nodes with respect to
                         their score.
		*/
          class CompNodeByScore
          {
               public:
                    CompNodeByScore() {}
               
                    bool operator()(
                         StoSOONode* const & x, StoSOONode* const & y) const
                    {
                         return (x->getScore() < y->getScore());
                    }
          };

		
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The root of the StoSOO tree.
		*/
		StoSOONode* root;


		/**
               \brief    The number of parameters of 'f'.
		*/
		unsigned int d;
		
		
		/**
               \brief    The arity of the tree to develop.
		*/
		unsigned int K;
		
		
		/**
               \brief    The maximal number of evaluations per node.
		*/
          unsigned int k;
          
          
          /**
               \brief    The maximal depth of the tree to develop.
          */
          unsigned int hMax;
          
          
          /**
               \brief    The confidence parameter.
          */
          double delta;
          
          
          /**
               \brief    The number of evaluations of 'f' to perform.
          */
          unsigned int n;
          
          
          /**
               \brief    The number of evaluations of 'f' performed.
          */
          unsigned int t;
          
          
          /**
               \brief    The current maximal depth of the tree.
          */
          unsigned int cMaxDepth;


          /**
               \brief    The leaves of the tree, sorted by:
                              (1) depth
                              (2) score
          */
          std::vector<std::priority_queue<
               StoSOONode*,
               std::vector<StoSOONode*>,
               CompNodeByScore> > leavesByDepth;
          
          
          // =================================================================
		//	Private methods
		// =================================================================
		/**
               \brief    Try to improve the tree.
                         Return true if it improved it, false else.
                         
                         The tree is considered to be improved if a node has
                         been split or 'f' has been evaluated at least once.
                         
                         If we need to evaluate 'f' and the budget is
                         exhausted, the process is stopped immediately
                         (notice that the tree could still have been improved
                         even in this case).
                         
                         This function throws an exception if 'f' throws an
                         exception.

               \return   True if it improved the tree, false else.
          */
          bool improve() throw (std::exception);

		
		/**
               \brief         Return the list of the nodes at depth 'depth'
                              from the root node.

               \param[depth   The depth at which we are searching for nodes.

               \return        The list of the nodes at depth 'depth' from
                              the root node.
		*/
		std::vector<StoSOONode*> getNodesAt(unsigned int depth) const
		{
               return getNodesAt(depth, root);
		}
		
		
		/**
               \brief         Return the list of the nodes at depth 'depth'
                              from the sub-tree defined by 'node'.

               \param[depth   The depth at which we are searching for nodes.
               \param[node    Defines the sub-tree from which to search.

               \return        The list of the nodes at depth 'depth' from
                              the sub-tree defined by 'node'
		*/
		std::vector<StoSOONode*> getNodesAt(
                    unsigned int depth, StoSOONode* node) const;
		
		
		/**
               \brief         Return the list of the nodes at depth 'depth'
                              from the sub-trees defined by the nodes in
                              'pNodes' (we assume that the nodes in 'pNode' are
                              at the same depth in the tree).

               \param[depth   The depth at which we are searching for nodes.
               \param[node    Defines the sub-trees from which to search.

               \return        The list of the nodes at depth 'depth' from
                              the sub-trees defined by the nodes in 'pNodes'
		*/
		std::vector<StoSOONode*> getNodesAt(
                    unsigned int depth,
                    const std::vector<StoSOONode*>& pNodes) const;


          /**
               \brief         Return the best node in 'nodes' with respect to
                              the mean evaluation of 'f' on their mean sample.

               \param[nodes   A list of nodes.
               
               \return        The best node in 'nodes' with respect to the
                              mean evaluation of 'f' on their mean sample.
                              
          */
          StoSOONode* getBestMeanEvalNode(
                    const std::vector<StoSOONode*>& nodes) const;


          /**
               \brief         Print the sub-tree defined by 'node' with 'nSpace'
                              white spaces.

               \param[nSpace  The number of tabulations to apply.
               \param[node    The sub-tree to print.
          */
		void print(unsigned int nSpace, StoSOO::StoSOONode* node);
		
		
		// =================================================================
		//	Friendships
		// =================================================================
		/**
			\brief	Give 'StoSOONode' access to 'drawArm()' and 'splitAcc'.
		*/
		friend class StoSOONode;		
};

#endif

