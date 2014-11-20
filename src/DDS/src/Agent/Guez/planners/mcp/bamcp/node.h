#ifndef NODE_H
#define NODE_H

#include "../../../utils/utils2.h"
#include "../../../utils/memorypool.h"
#include <iostream>
#include "../../../planners/mcp/value.h"

class HISTORY;
class SIMULATOR;
class QNODE;
class VNODE;

//-----------------------------------------------------------------------------

class QNODE
{
public:

    VALUE<int> Value;

    void Initialise();

    VNODE*& Child(int c) { return Children[c]; }
    VNODE* Child(int c) const { return Children[c]; }

    void DisplayValue(HISTORY& history, int maxDepth, std::ostream& ostr) const;
    void DisplayPolicy(HISTORY& history, int maxDepth, std::ostream& ostr) const;

    static int NumChildren;

private:

    std::vector<VNODE*> Children;

friend class VNODE;
};

//-----------------------------------------------------------------------------

class VNODE : public MEMORY_OBJECT
{
public:

    VALUE<int> Value;

    void Initialise();
    static VNODE* Create();
    static void Free(VNODE* vnode, const SIMULATOR& simulator);
    static void FreeAll();

    QNODE& Child(int c) { return Children[c]; }
    const QNODE& Child(int c) const { return Children[c]; }

    void SetChildren(int count, double value);

    void DisplayValue(HISTORY& history, int maxDepth, std::ostream& ostr) const;
    void DisplayPolicy(HISTORY& history, int maxDepth, std::ostream& ostr) const;

    static int NumChildren;

    bool preventfree;

private:

    std::vector<QNODE> Children;
    static MEMORY_POOL<VNODE> VNodePool;
};

#endif // NODE_H
