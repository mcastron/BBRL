
#include "UCB1.h"

using namespace std;
using namespace utils::algorithm;


// ===========================================================================
//	Public Constructor
// ===========================================================================
UCB1::UCB1(unsigned int nArms_, double c_) :
		nArms(nArms_), c(c_),
		muList(nArms), nList(nArms), discardFlags(nArms),
		nDiscardedArms(0), nSum(0)
{
	assert(nArms != 0);
	
	
	for (unsigned int i = 0; i < nArms; ++i)
	{
	    muList[i] = 0.0;
	    nList[i] = 0;
	    discardFlags[i] = false;
     }
}


// ===========================================================================
//	Public methods
// ===========================================================================
int UCB1::run(unsigned int n)
{
	//	Initialization: draw each arm one time (if needed)
	for (unsigned int i = 0; ((i < nArms) && (n > 0)); ++i)
	{
		if (nList[i] == 0)
		{
               try
               {
                    muList[i] = drawArm(i);
     			++nList[i];
                    ++nSum;
                    --n;
               }
               
               catch (std::exception& e)
               {
                    discardFlags[i] = true;
                    ++nDiscardedArms;
                    continue;
               }
		}
	}


	//	Convergence criteria: same arm drawn several times in a row
	while ((n > 0) && (nDiscardedArms < nArms))
	{
		//	1.	Compute the 'score' associated to each arm with respect
		//		to UCB1 formula
		vector<double> ucbScoreList(nArms);
		for (unsigned int i = 0; i < nArms; ++i)
		{
               if (discardFlags[i]) { ucbScoreList[i] = -DBL_MAX; }
               else
               {
                    ucbScoreList[i]
                              = (muList[i] + (c * sqrt(log(nSum) / nList[i])));
               }
		}

		
		//	2.	Select the arm which obtained the best score
		vector<vector<double>::const_iterator> maxList;
		maxList = utils::search::max<vector<double> >(
				ucbScoreList.begin(), ucbScoreList.end());
		
		int maxI	= RandomGen::randIntRange_Uniform(0, (maxList.size() - 1));
		int s	= (maxList[maxI] - ucbScoreList.begin());
		
		
		//	3.	Draw the selected arm and update the data
		try
		{
               double drawScore = drawArm(s);
               muList[s] *= ((double) nList[s] / (nList[s] + 1.0));
               muList[s] += ((1.0 / (nList[s] + 1.0)) * drawScore);
               ++nList[s];
               ++nSum;
               --n;
		}

		catch (std::exception& e)
		{
		   discardFlags[s] = true;
		   ++nDiscardedArms;
		   continue;
          }
	}
	
     //   Return -1 if all arms have been discarded
	if (nDiscardedArms >= nArms) { return -1; }


	//	Return the most drawn arm else
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
