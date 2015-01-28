
#include "StoSOO.h"

using namespace std;
using namespace utils::algorithm;


// ===========================================================================
//	Public Constructor/Destructor
// ===========================================================================
StoSOO::StoSOO(unsigned int d_, unsigned int K_,
               unsigned int k_, unsigned int hMax_, double delta_,
               const std::vector<std::pair<double, double> >& boundsList) :
                    d(d_), K(K_), k(k_), hMax(hMax_), delta(delta_), t(0),
                    cMaxDepth(0)
{
     assert(d > 0);
     assert(K % 2 == 1);
     assert(k > 0);
     assert(hMax > 0);
     assert(delta > 0.0);
     
     leavesByDepth.resize(hMax + 1);
     
	root = new StoSOONode(this, 0, boundsList);
}


StoSOO::~StoSOO() { if (root) { delete root; } }


// ===========================================================================
//	Public methods
// ===========================================================================
vector<double> StoSOO::run(unsigned int n_) throw (std::exception)
{
     //   Improve the tree.
     n = n_;
     while (improve());


     //   Retrieve the list of the deepest inner nodes.
     vector<StoSOONode*> deepestInnerNodes;
     if (!root->isLeaf()) { deepestInnerNodes.push_back(root); }
     
     vector<StoSOONode*> pNodes;
     pNodes.push_back(root);
     for (unsigned int cDepth = 1; cDepth <= cMaxDepth; ++cDepth)
     {
          //   Retrieve the nodes at depth 'cDepth'.
          vector<StoSOONode*> nodes = getNodesAt(1, pNodes);
          
          
          //   Build the list of inner nodes from 'nodes'.
          vector<StoSOONode*> cInnerNodes;
          for (unsigned int i = 0; i < nodes.size(); ++i)
               if (!nodes[i]->isLeaf()) { cInnerNodes.push_back(nodes[i]); }


          //   Update the list of the deepest inner nodes (if needed).
          if (!cInnerNodes.empty()) { deepestInnerNodes = cInnerNodes; }
          
          
          //   Proceed to next depth.
          pNodes = nodes;
     }
     
     
     //   Return the best solution.
     StoSOONode* bestNode = getBestMeanEvalNode(deepestInnerNodes);
     if (bestNode) { return bestNode->getMeanSample(); }
     else          { return root->getMeanSample();     }
}



// ===========================================================================
//	Private Classes
// ===========================================================================
// ---------------------------------------------------------------------------
//	Class 'StoSOONode'
// ---------------------------------------------------------------------------
// ===========================================================================
//	Public Constructor/Destructor (in 'StoSOONode')
// ===========================================================================
StoSOO::StoSOONode::StoSOONode(
		StoSOO* stoSOO_, unsigned int depth_,
		const std::vector<std::pair<double, double> >& boundsList_) :
			stoSOO(stoSOO_), depth(depth_),
			boundsList(boundsList_), meanEval(0.0), nbEval(0)
{
	assert(stoSOO);
	assert(!boundsList.empty());
	
	
     //   Update the maximal depth.
     if (depth > stoSOO->cMaxDepth) { stoSOO->cMaxDepth = depth; }
     
	
     //   Insert the new node in appropriate list of leaves.
	stoSOO->leavesByDepth[depth].push(this);
	
	
     //   Initialize the mean sample
	meanSample.resize(stoSOO->d);
     for (unsigned int i = 0; i < meanSample.size(); ++i)
          meanSample[i] = ((boundsList[i].first + boundsList[i].second) / 2.0);
}


StoSOO::StoSOONode::~StoSOONode()
{
     for (unsigned int i = 0; i < childs.size(); ++i)
          if (childs[i]) { delete childs[i]; }
}



// ===========================================================================
//	Public methods (in 'StoSOONode')
// ===========================================================================
void StoSOO::StoSOONode::eval() throw (std::exception)
{
     double y = stoSOO->f(getMeanSample());
     
     meanEval = (((nbEval * meanEval) + y) / (double) (nbEval + 1));
     ++nbEval;
}

void StoSOO::StoSOONode::split()
{
     if (!isLeaf()) { return; }    //   cannot split an inner node.
     
     
     //   Select randomly a parameter along which we will perform the split.
     unsigned int s = RandomGen::randIntRange_Uniform(0, (stoSOO->d - 1));
     
     
     //   Perform the split.
     double step = ((boundsList[s].second - boundsList[s].first)
                         / (double) stoSOO->K);
               
     for (unsigned int i = 0; i < stoSOO->K; ++i)
     {
          vector<pair<double, double> > cBoundsList = boundsList;
          cBoundsList[s].first  = (boundsList[s].first + (i       * step));
          cBoundsList[s].second = (boundsList[s].first + ((i + 1) * step));
          
          childs.push_back(new StoSOONode(stoSOO, (depth + 1), cBoundsList));
     }
     
     
     //   Update the results obtained for this node to its 'center' child
     //   (only works for odd 'K' case).
     if ((stoSOO->K % 2) == 1)
     {
          unsigned int c = (stoSOO->K / 2);
          childs[c]->setData(meanEval, nbEval);
     }
}


// ===========================================================================
//	Private methods
// ===========================================================================
bool StoSOO::improve() throw (std::exception)
{
     if (n == 0) { return false; } // no budget


     bool improved = false;
     
     double bMax;
     bool iniBMax = false;     
     for (unsigned int cDepth = 0; (cDepth <= cMaxDepth); ++cDepth)
     {
          if (t >= n) { break; }

          //   Continue if there is at least one leaf at depth 'cDepth'.
          if (!leavesByDepth[cDepth].empty())
          {
               //   Select the leaf with the best score.
               //   (first leaf of the queue)
               StoSOONode* node = leavesByDepth[cDepth].top();
               leavesByDepth[cDepth].pop();
               
               double b = node->getScore();
     
     
               //   If the score is the best one observed at this time of the
               //   descent, try to evaluate or split it.
               if (!iniBMax || (b >= bMax))
               {
                    //   Evaluate the node if it has not been fully evaluated
                    //   yet.
                    if (node->getNbEval() < k)
                    {
                         node->eval();
                         ++t;
     
                         improved = true;
                    }
                    
                    
                    //   Split it if the maximal depth has not been reached
                    //   yet.
                    else if (cDepth < hMax)
                    {
                         node->split();

                         iniBMax = true;
                         bMax = b;
     
                         improved = true;
                    }
                    
                    
                    //   Re-push the node in its queue if it is still a leaf.
                    if (node->isLeaf()) { leavesByDepth[cDepth].push(node); }
               }
          }
     }

     return improved;
}


vector<StoSOO::StoSOONode*> StoSOO::getNodesAt(
               unsigned int depth, StoSOONode* node) const
{
     //   'node' is the node we are searching for.
     if (node && depth == 0)
     {
          vector<StoSOO::StoSOONode*> nodesAt;
          nodesAt.push_back(node);
          return nodesAt;
     }


     else
     {          
          //   The childs of 'node' could be the ones we are searching for.
          vector<StoSOO::StoSOONode*> cChilds = node->getChilds();
          if (node && depth > 0 && !(cChilds.empty()))
               return getNodesAt(depth - 1, cChilds);

          //   No node at depth 'depth' from 'node'.
          else
               return vector<StoSOO::StoSOONode*>();
     }
}


vector<StoSOO::StoSOONode*> StoSOO::getNodesAt(
               unsigned int depth, const vector<StoSOONode*>& pNodes) const
{
     //   'pNodes' are the nodes we are searching for.
     if (depth == 0) { return pNodes; }
     
     
     //   The childs of the nodes of 'pNodes' could be the ones we are
     //   searching for.
     vector<StoSOO::StoSOONode*> nodesAt;
     for (unsigned int i = 0; i < pNodes.size(); ++i)
     {
          vector<StoSOO::StoSOONode*> cNodesAt =
                    getNodesAt(depth, pNodes[i]);

          nodesAt.insert(nodesAt.end(), cNodesAt.begin(), cNodesAt.end());
     }
     return nodesAt;
}


StoSOO::StoSOONode* StoSOO::getBestMeanEvalNode(
          const vector<StoSOO::StoSOONode*>& nodes) const
{
     double maxMeanEval;
     vector<StoSOO::StoSOONode*> maxNodes;
     for (unsigned int i = 0; i < nodes.size(); ++i)
     {
          double cMeanEval = nodes[i]->getMeanEval();
          if (maxNodes.empty() || maxMeanEval <= cMeanEval)
          {
               if (!maxNodes.empty() && maxMeanEval < cMeanEval)
                    maxNodes.clear();
          
               maxMeanEval = cMeanEval;
               maxNodes.push_back(nodes[i]);
          }
     }


     if (maxNodes.empty()) { return 0; }     
     return maxNodes[RandomGen::randIntRange_Uniform(0, maxNodes.size() - 1)];
}


void StoSOO::print(unsigned int nSpace, StoSOO::StoSOONode* node)
{
     //   White spaces
     for (unsigned int i = 0; i < nSpace; ++i) { cout << " "; }


     //   'node'
     node->print();
     cout << "\n";
     
     
     //   Childs of 'node'
     vector<StoSOO::StoSOONode*> cChilds = node->getChilds();
     for (unsigned int i = 0; i < cChilds.size(); ++i)
          print(nSpace + 1, cChilds[i]);
}
