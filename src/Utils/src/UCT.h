
#ifndef UCT_H
#define UCT_H

#include "RandomGen.h"
#include "utils.h"
#include "../ExternalLibs.h"


// ===========================================================================
/*
	\class 	UCT
	\author 	Castronovo Michael
	
	\brief 	An interface to specialize for UCT.
	
	\date 	2015-01-20
*/
// ===========================================================================
class utils::UCT
{
	public:
		// =================================================================
		//	Public Constructor/Destructor
		// =================================================================
		/**
			\brief				Constructor.
			
			\param[c				The constant used in the UCB1 formula:
									I_t(i) = mu_i
										+ c * sqrt(ln(n_t) / n_i)
			\param[boundsList		The list of bounds (min/max) for each
								parameter.
			\param[splitAccList_	The list of split accuracies of the
								bounds.
		*/
		UCT(	double c,
			const std::vector<std::pair<double, double> >& boundsList,
			const std::vector<double>& splitAccList_);


		/**
			\brief	Destructor.
		*/
		virtual ~UCT();

		
		// =================================================================
		//	Public methods
		// =================================================================
		/**
			\brief			Draw 'n' arms and return the mean of the
							parameters of the arm which has been
							selected by the UCBs.
			
			\param[n			The number of draws.
			\param[boundsList	If specified, store the bounds of each
							parameter.
			
			\return			The mean of the parameters of the arm which
							has been selected by the UCBs.

		*/
		std::vector<double> run(	unsigned int n,
							std::vector<std::pair<double, double> >*
									armBoundsList = 0);
	
	
	protected:
		// =================================================================
		//	Protected methods
		// =================================================================
		/**
			\brief			Draw the arm defined by 'paramList' and
							return its score.
			
			\param[paramList	The parameters defining the arm to draw.
			
			\return			The score of the arm defined by 'paramList'.
		*/
		virtual double drawArm(const std::vector<double>& paramList)
                                             const throw (std::exception) = 0;


	private:		
		// =================================================================
		//	Private Class
		// =================================================================
		/**
			\brief	Defines a UCT node.
		*/
		class UCTNode
		{
			public:
				// =======================================================
				//	Public Constructor/Destructor
				// =======================================================
				/**
					\brief			Constructor
					
				\param[uct_	 		A pointer to a UCT
				\param[boundsList_		The bounds defining the part of
									space governed by this node.
				\param[splitCountList_	The number of times each parameter
									has been selected to split a node
									from the root to this node.
				\param[c_				The constant used in the UCB1
									formula:
										I_t(i) = mu_i
										+ c * sqrt(ln(n_t) / n_i)
				*/
				UCTNode(	const UCT* uct_,
						double c_,
						const std::vector<
							std::pair<double, double> >& boundsList_,
						const std::vector<unsigned int>& splitCountList_);
				
				
				/**
					\brief	Destructor
				*/
				~UCTNode();


				// =======================================================
				//	Public methods
				// =======================================================
				/**
					\brief	Draw an arm from this UCTNode and
							return its score.
					
					\return	The score obtained by the arm drawn.
				*/
				double drawArm();
				
				
				/**
					\brief	Try to split this node.
							Return in case of success, false else.
							
					\return	True if it split the node, false else.
				*/
				bool split();
				
				
				/**
					\brief	Return an arm parameters sample from
							the part of the space defined by this node.
					
					\return	An arm parameters sample from the part of
							the space defined by this node.
				*/
				std::vector<double> getSample() const;
				
				
				/**
					\brief	Return the parameters of the mean arm from
							the part of the space defined from this
							node.
					
					\return	The parameters of the mean arm from the part
							of the space defined from this node.
				*/
				std::vector<double> getMeanSample() const;
				
				
				/**
					\brief	Return true if this node is a leaf, false
							else.
					
					\return	True if this node is a leaf, false else.
				*/
				bool isLeaf() const { return (!left && !right); }
				
				
				/**
					\brief	Return the left child node (if it exists).
					
					\return	The left child node (if it exists).
				*/
				const UCTNode* getLeft() const { return left; }
				
				
				/**
					\brief	Return the right child node (if it exists).
					
					\return	The right child node (if it exists).
				*/
				const UCTNode* getRight() const { return right; }


				/**
					\brief	Return the number of times an arm within
							the part of the space defined by this node
							has been drawn.
					
					\return	The number of times an arm within
							the part of the space defined by this node
							has been drawn.
				*/
				unsigned int getN() const { return n; }
				
				
				/**
					\brief	Return the mean score obtained by an arm
							drawn within the part of the space defined
							by this node.
					
					\return	The mean score obtained by an arm drawn
							within the part of the space defined by
							this node.
				*/
				double getMu() const { return mu; }


				/**
					\brief	Return the bounds associated to this arm.
				*/
				const std::vector<std::pair<double, double> >&
						getBoundsList() const { return boundsList; }


			private:
				// =======================================================
				//	Private attributes
				// =======================================================
				/**
					\brief	A UCT.
				*/
				const UCT* uct;
				
				
				/**
					\brief	The constant used in the UCB1 formula:
								I_t(i) = mu_i + c * sqrt(ln(n_t) / n_i)
				*/
				double c;

				
				/**
					\brief	The bounds defining the part of space
							governed by this node.
				*/
				const std::vector<std::pair<double, double> > boundsList;
				
				
				/**
					\brief	The number of times an arm within
							the part of the space defined by this node
							has been drawn.
				*/
				unsigned int n;
				
				
				/**
					\brief	The mean score obtained by an arm drawn
							within the part of the space defined by
							this node.
				*/
				double mu;
				
				
				/**
					\brief	The number of times each parameter has been
							selected to split a node from the root to
							this node.
				*/
				const std::vector<unsigned int> splitCountList;
				
				
				/**
					\brief	The parameter along which this part of
							space is split (if not a leaf).
				*/
				int splitC;
				
				
				/**
					\brief	The limit value of the parameter along
							which this part of the space is split.
							(if not a leaf)
				*/
				double splitV;
				
				
				/**
					\brief	The left and right child node respectively.
							(if not a leaf)
				*/
				UCTNode *left, *right;
				
				
				// =======================================================
				//	Private methods
				// =======================================================
				/**
					\brief		Update 'n' and 'mu' with respect to
								'score'.
					
					\param[score	The score obtained by an arm drawn
								from the part of the space defined
								by this node.
				*/
				void update(double score)
				{
					mu *= (n / ((double) (n + 1)));
					mu += ((1.0 / (n + 1)) * score);
					++n;
				}
				
				
				/**
					\brief	Return the UCB1 score of the left child
							node.
					
					\return	The UCB1 score of the left child node.
				*/
				double getLUCB1Score() const
				{
					assert(getLeft());
					
					
					double lMu = getLeft()->getMu();
					double lN = getLeft()->getN();
					return (lMu + (c * sqrt(log(n) / lN)));
				}
				
				
				/**
					\brief	Return the UCB1 score of the right child
							node.
					
					\return	The UCB1 score of the right child nod.
				*/
				double getRUCB1Score() const
				{
					assert(getRight());
					
					
					double rMu = getRight()->getMu();
					double rN = getRight()->getN();
					return (rMu + (c * sqrt(log(n) / rN)));
				}
		};
		
		
		
		// =================================================================
		//	Private attributes
		// =================================================================
		/**
			\brief	The root of the UCT tree.
		*/
		UCTNode* root;


		/**
			\brief	The list of split accuracies of the bounds.
		*/
		std::vector<double> splitAccList;
		
		
		// =================================================================
		//	Friendships
		// =================================================================
		/**
			\brief	Give 'UCTNode' access to 'drawArm()' and 'splitAcc'.
		*/
		friend class UCTNode;		
};

#endif

