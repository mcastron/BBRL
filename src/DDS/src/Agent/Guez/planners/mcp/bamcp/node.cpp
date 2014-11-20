#include "node.h"
#include "history.h"
#include "../../../utils/utils2.h"

using namespace std;

//-----------------------------------------------------------------------------

int QNODE::NumChildren = 0;

void QNODE::Initialise()
{
    assert(NumChildren);
    Children.resize(NumChildren);
    for (int observation = 0; observation < QNODE::NumChildren; observation++)
        Children[observation] = 0;
}

void QNODE::DisplayValue(HISTORY& history, int maxDepth, ostream& ostr) const
{
    history.Display(ostr);
    ostr << ": " << Value.GetValue() << " (" << Value.GetCount() << ")\n";
    if (history.Size() >= (uint) maxDepth)
        return;

    for (int observation = 0; observation < NumChildren; observation++)
    {
        if (Children[observation])
        {
            history.Back().Observation = observation;
            Children[observation]->DisplayValue(history, maxDepth, ostr);
        }
    }
}

void QNODE::DisplayPolicy(HISTORY& history, int maxDepth, ostream& ostr) const
{
    history.Display(ostr);
    ostr << ": " << Value.GetValue() << " (" << Value.GetCount() << ")\n";
    if (history.Size() >= (uint) maxDepth)
        return;

    for (int observation = 0; observation < NumChildren; observation++)
    {
        if (Children[observation])
        {
            history.Back().Observation = observation;
            Children[observation]->DisplayPolicy(history, maxDepth, ostr);
        }
    }
}

//-----------------------------------------------------------------------------

MEMORY_POOL<VNODE> VNODE::VNodePool;

int VNODE::NumChildren = 0;

void VNODE::Initialise()
{
    preventfree = false;
    assert(NumChildren);
    Children.resize(VNODE::NumChildren);
    for (int action = 0; action < VNODE::NumChildren; action++)
        Children[action].Initialise();
}

VNODE* VNODE::Create()
{
    VNODE* vnode = VNodePool.Allocate();
    vnode->Initialise();
    return vnode;
}

void VNODE::Free(VNODE* vnode, const SIMULATOR& simulator)
{
  if(!vnode->preventfree){
    VNodePool.Free(vnode);
    for (int action = 0; action < VNODE::NumChildren; action++)
        for (int observation = 0; observation < QNODE::NumChildren; observation++)
            if (vnode->Child(action).Child(observation))
                Free(vnode->Child(action).Child(observation), simulator);
  }
}

void VNODE::FreeAll()
{
	VNodePool.DeleteAll();
}

void VNODE::SetChildren(int count, double value)
{
    for (int action = 0; action < NumChildren; action++)
    {
        QNODE& qnode = Children[action];
        qnode.Value.Set(count, value);
    }
}

void VNODE::DisplayValue(HISTORY& history, int maxDepth, ostream& ostr) const
{
    if (history.Size() >= (uint) maxDepth)
        return;

    for (int action = 0; action < NumChildren; action++)
    {
        history.Add(action,-1);
        Children[action].DisplayValue(history, maxDepth, ostr);
        history.Pop();
    }
}

void VNODE::DisplayPolicy(HISTORY& history, int maxDepth, ostream& ostr) const
{
    if (history.Size() >= (uint) maxDepth)
        return;

    double bestq = -Infinity;
    int besta = -1;
    for (int action = 0; action < NumChildren; action++)
    {
        if (Children[action].Value.GetValue() > bestq)
        {
            besta = action;
            bestq = Children[action].Value.GetValue();
        }
    }

    if (besta != -1)
    {
        history.Add((uint)besta,0);
        Children[besta].DisplayPolicy(history, maxDepth, ostr);
        history.Pop();
    }
}

//-----------------------------------------------------------------------------
