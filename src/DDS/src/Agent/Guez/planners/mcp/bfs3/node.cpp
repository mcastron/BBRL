 #include "node.h"
#include "../../../utils/utils2.h"
#include <limits>

using namespace std;

//-----------------------------------------------------------------------------


void QNODE3::Add(uint child){
	C++;
	map<uint,VNODE3Pc>::iterator it;
	it = Children.find(child);
	if(it == Children.end()){
		VNODE3Pc node;
		node.v = 0; 
		node.count = 1;
		Children.insert(pair<uint,VNODE3Pc>(child,node));
	}
	else
		it->second.count += 1;
}

map<uint,VNODE3Pc>::iterator QNODE3::selectPath(double Vmax,double Vmin){	
	double bestDiff = 0;
	vector<map<uint,VNODE3Pc>::iterator> bestVNODE3;
	
	map<uint,VNODE3Pc>::iterator it;
	for(it=Children.begin();it != Children.end(); it++){
		double diff;	
		if(it->second.v == 0){
			diff = (Vmax - Vmin)*it->second.count;	
		}else{
			diff = (it->second.v->Us-it->second.v->Ls)*it->second.count;
		}	
		if(diff > bestDiff){
			bestDiff = diff;
			bestVNODE3.clear();
			bestVNODE3.push_back(it);
		}
		else if(diff == bestDiff){
			bestVNODE3.push_back(it);
		}
	}
	return bestVNODE3[rand() % bestVNODE3.size()];
}
//-----------------------------------------------------------------------------

MEMORY_POOL<VNODE3> VNODE3::VNodePool;

int VNODE3::NumChildren = 0;

void VNODE3::Initialise()
{
    assert(NumChildren);
    Children.resize(VNODE3::NumChildren);
}

VNODE3* VNODE3::Create()
{
    VNODE3* vnode = VNodePool.Allocate();
    vnode->Initialise();
    return vnode;
}

void VNODE3::Free(VNODE3* vnode)
{
		map<uint,VNODE3Pc>::iterator it;
    for (int action = 0; action < VNODE3::NumChildren; action++){
			QNODE3& qnode = Children[action];
			for(it=qnode.Children.begin();it != qnode.Children.end(); it++){
        if (it->second.v)
					Free(it->second.v);
				}
		}
    VNodePool.Free(vnode);
}

void VNODE3::FreeAll()
{
	VNodePool.DeleteAll();
}

uint VNODE3::bestA(){
	vector<uint> besta;
	double bestq = -std::numeric_limits<double>::max();
	for(int aa=0;aa<NumChildren;++aa){
		QNODE3& qnode = Children[aa];
		double qa = qnode.Usa;
		if(qa > bestq){
			bestq = qa;
			besta.clear();
			besta.push_back(aa);
		}
		else if(qa == bestq)
			besta.push_back(aa);

		//std::cout << "Us," << aa << ": " << qa << std::endl;
	}
	
	return besta[rand() % besta.size()];
}

void VNODE3::SetChildren(double Usa, double Lsa)
{
    for (int action = 0; action < NumChildren; action++)
    {
        QNODE3& qnode = Children[action];
        qnode.Usa = Usa;
				qnode.Lsa = Lsa;
    }
}

//-----------------------------------------------------------------------------
