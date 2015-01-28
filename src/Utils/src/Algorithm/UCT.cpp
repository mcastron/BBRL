
#include "UCT.h"

using namespace std;
using namespace utils::algorithm;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
UCT::UCT(	double c,
		const std::vector<
			std::pair<double, double> >& boundsList,
		const std::vector<double>& splitAccList_) :
				splitAccList(splitAccList_)
{
	assert(splitAccList.size() == boundsList.size());


	vector<unsigned int> splitCountList;
	for (unsigned int i = 0; i < boundsList.size(); ++i)
		splitCountList.push_back(0);
	
	root = new UCTNode(this, c, boundsList, splitCountList);
}


UCT::~UCT() { if (root) { delete root; } }


// ===========================================================================
//	Public methods
// ===========================================================================
vector<double> UCT::run(	unsigned int n,
					vector<pair<double, double> >* armBoundsList)
{
	//	Run the UCT
	while (n-- > 0) { root->drawArm(); }
	
	
	//	Find the arm selected by the UCBs:
	//		(1)	Drawn most of the time
	//		(2)	Best mean score
	//	Stops whenever it reaches a leaf or a uninitialized UCB1.
	const UCTNode* cur = root;		
	while (!cur->isLeaf()
			&& (cur->getLeft()->getN() > 0)
			&& (cur->getRight()->getN() > 0))
	{
		unsigned int lN = cur->getLeft()->getN();
		unsigned int rN = cur->getRight()->getN();
		double lMu = cur->getLeft()->getMu();
		double rMu = cur->getRight()->getMu();		
		
		if ((lN > rN) || ((lN == rN) && (lMu > rMu)))
			cur = cur->getLeft();
		
		else if ((rN > lN) || ((rN == lN) && (rMu > lMu)))
			cur = cur->getRight();	
		
		//	tie break
		else
		{			
			double f = RandomGen::rand01_Uniform();
			if (f < 0.5)	{ cur = cur->getLeft();	}
			else			{ cur = cur->getRight();	}
		}
	}

	
	//	Store some extra data	
	if (armBoundsList) { *armBoundsList = cur->getBoundsList(); }
	
	
	//	Return the parameters associated to this arm
	return cur->getMeanSample();
}


// ===========================================================================
//	Private Classes
// ===========================================================================
// ---------------------------------------------------------------------------
//	Class 'UCTNode'
// ---------------------------------------------------------------------------
// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
UCT::UCTNode::UCTNode(
		const UCT* uct_,
		double c_,
		const std::vector<std::pair<double, double> >& boundsList_,
		const std::vector<unsigned int>& splitCountList_) :
			uct(uct_), c(c_),
			boundsList(boundsList_),
			n(0), mu(0.0),
			splitCountList(splitCountList_), splitC(-1), splitV(-DBL_MAX),
			left(0), right(0)
			
{
	assert(uct);
	assert(!boundsList.empty());
	assert(splitCountList.size() == boundsList.size());
}


UCT::UCTNode::~UCTNode()
{
	if (!isLeaf()) { delete left; delete right; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
double UCT::UCTNode::drawArm()
{
	double score;
	
	
	//	If the current node is a leaf, try to split it.
	//	If it fails, draw the arm corresponding to the mean sample of this
	//	part of the space
	if (isLeaf() && !split())
	{
		score = uct->drawArm(getMeanSample());
		update(score);
		
		return score;
	}
	
	
	//	Select and draw an arm
	unsigned int lN = getLeft()->getN();
	unsigned int rN = getRight()->getN();
	
		//	Case 1:	UCB not initialized (the node has been split now)
		//		==> flip a coin to choose the UCB arm to be initialized
	if ((lN == 0) && (rN == 0))
	{
		double f = RandomGen::rand01_Uniform();
		if (f < 0.5)
		{
			score = uct->drawArm(getLeft()->getSample());
			left->update(score);
		}
		else
		{
			score = uct->drawArm(getRight()->getSample());
			right->update(score);
		}
	}
	
	
		//	Case 2: UCB partially initialized
		//		==> draw the other arm
	else if (lN == 0)
	{
		score = uct->drawArm(left->getSample());
		left->update(score);
	}
	else if (rN == 0)
	{
		score = uct->drawArm(right->getSample());
		right->update(score);
	}
	
	
		//	Case 3: UCB initialized
		//		==> check the UCB scores to select the UCB arm to draw
	else
	{
		double lUCB1Score = getLUCB1Score();
		double rUCB1Score = getRUCB1Score();
		
		if		(lUCB1Score > rUCB1Score) { score = left->drawArm();	}
		else if	(lUCB1Score < rUCB1Score) { score = right->drawArm();	}
		else	// tie break
		{
			double f = RandomGen::rand01_Uniform();
			if (f < 0.5)	{ score = left->drawArm();	}
			else			{ score = right->drawArm();	}
		}
	}


	//	Update the UCB
	update(score);
	
	
	//	Return
	return score;
}


bool UCT::UCTNode::split()
{
	assert(isLeaf());


	//	Select the parameter which has been used the less
	vector<unsigned int> tSplitCountList = splitCountList;
	for (unsigned int i = 0; i < boundsList.size(); ++i)
	{
		vector<vector<unsigned int>::const_iterator> minList;
			minList = utils::search::min<vector<unsigned int> >(
					tSplitCountList.begin(), tSplitCountList.end());

		int minI = RandomGen::randIntRange_Uniform(0, (minList.size() - 1));
		splitC = (minList[minI] - tSplitCountList.begin());


		//	If bounds are too tight, discard this parameter and try
		//	another one
		double bMin = boundsList[splitC].first;
		double bMax = boundsList[splitC].second;
		if ((bMax - bMin) < uct->splitAccList[splitC])
		{
			tSplitCountList[splitC] = UINT_MAX;
			splitC = -1;
		}
		else { break; }
	}
	
	
		//	Not able to select a parameter
	if (splitC == -1) { return false; }
	
	
	//	Split the space along the selected parameter
	splitV = ((boundsList[splitC].first + boundsList[splitC].second) / 2.0);

	
	//	Create the left node
	vector<pair<double, double> > lBoundsList = boundsList;
	vector<unsigned int> lSplitCountList = splitCountList;

	lBoundsList[splitC].second = splitV;
	++lSplitCountList[splitC];

	left = new UCTNode(uct, c, lBoundsList, lSplitCountList);
	
	
	//	Create the right node
	vector<pair<double, double> > rBoundsList = boundsList;
	vector<unsigned int> rSplitCountList = splitCountList;
	
	rBoundsList[splitC].first = splitV;
	++rSplitCountList[splitC];
	
	right = new UCTNode(uct, c, rBoundsList, rSplitCountList);


	//	Return
	return true;
}


std::vector<double> UCT::UCTNode::getSample() const
{
	//	Draw each parameter value within the bounds
	vector<double> paramList(boundsList.size());
	for (unsigned int i = 0; i < paramList.size(); ++i)
	{
		paramList[i] =
			RandomGen::randRange_Uniform(
					boundsList[i].first, boundsList[i].second);
	}
	
	return paramList;
}


vector<double> UCT::UCTNode::getMeanSample() const
{
	//	Retrieve the mean value of each parameter
	vector<double> paramList(boundsList.size());
	for (unsigned int i = 0; i < paramList.size(); ++i)
		paramList[i] = ((boundsList[i].first + boundsList[i].second) / 2.0);
	
	return paramList;
}
