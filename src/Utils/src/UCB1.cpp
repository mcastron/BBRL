
#include "UCB1.h"

using namespace utils;
using namespace std;


// ===========================================================================
//	Public Constructor
// ===========================================================================
UCB1::UCB1(unsigned int nArms_, double c_) :
		nArms(nArms_), c(c_),
		muList(nArms), nList(nArms), nSum(0)
{
	assert(nArms != 0);
	
	
	for (unsigned int i = 0; i < nArms; ++i) { nList[i] = 0; }
}


// ===========================================================================
//	Public methods
// ===========================================================================
unsigned int UCB1::run(unsigned int n)
{
	//	Initialization: draw each arm one time (if needed)
	for (unsigned int i = 0; ((i < nArms) && (n > 0)); ++i)
	{
		if (nList[i] == 0)
		{
			muList[i]	= drawArm(i);
			++nList[i];
			++nSum;
			--n;
		}
	}


	//	Convergence criteria: same arm drawn several times in a row
	while (n > 0)
	{
		//	1.	Compute the 'score' associated to each arm with respect
		//		to UCB1 formula
		vector<double> ucbScoreList(nArms);
		for (unsigned int i = 0; i < nArms; ++i)
		{
			ucbScoreList[i]
				= (muList[i] + (c * sqrt(log(nSum) / nList[i])));
		}

		
		//	2.	Select the arm which obtained the best score
		vector<vector<double>::const_iterator> maxList;
		maxList = utils::search::max<vector<double> >(
				ucbScoreList.begin(), ucbScoreList.end());
		
		int maxI	= RandomGen::randIntRange_Uniform(0, (maxList.size() - 1));
		int s	= (maxList[maxI] - ucbScoreList.begin());
		
		
		//	3.	Draw the selected arm and update the data
		double drawScore = drawArm(s);
		muList[s] *= ((double) nList[s] / (nList[s] + 1.0));
		muList[s] += ((1.0 / (nList[s] + 1.0)) * drawScore);
		++nList[s];
		++nSum;
		--n;
	}
	
	
	//	Return the most drawn arm
	vector<vector<unsigned int>::const_iterator> maxList;
		maxList = utils::search::max<vector<unsigned int> >(
				nList.begin(), nList.end());
		
	int maxI = RandomGen::randIntRange_Uniform(0, (maxList.size() - 1));
	return (maxList[maxI] - nList.begin());
}


vector<unsigned int> UCB1::getTop(unsigned int n) const
{
	vector<unsigned int> top;
	vector<unsigned int> nListTmp = nList;
	while (n-- > 0)
	{	
		vector<vector<unsigned int>::const_iterator> maxList;
		maxList = utils::search::max<vector<unsigned int> >(
				nListTmp.begin(), nListTmp.end());
		
		int maxI	= RandomGen::randIntRange_Uniform(0, (maxList.size() - 1));
		top.push_back(maxList[maxI] - nListTmp.begin());
		nListTmp[maxList[maxI] - nListTmp.begin()] = 0;
	}
	
	return top;
}