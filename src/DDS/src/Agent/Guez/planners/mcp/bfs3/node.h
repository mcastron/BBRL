#pragma once


#include "../../../utils/utils2.h"
#include <map>

typedef unsigned int uint;
class HISTORY;
class SIMULATOR;
class QNODE3;
class VNODE3;

struct VNODE3Pc{
	VNODE3* v;
	uint count;
};

//-----------------------------------------------------------------------------

class QNODE3
{
public:
		//Upper and lower bound on the value
    double Usa;
		double Lsa;
		uint C;
		
		QNODE3(){C=0;}
		void Add(uint key);
		
		std::map<uint,VNODE3Pc>::iterator selectPath(double max,double min);

		std::map<uint,VNODE3Pc> Children;

};

//-----------------------------------------------------------------------------

class VNODE3 : public MEMORY_OBJECT
{
public:
		//Upper and lower bound on the value
		double Us;
		double Ls;

		uint bestA();

    void Initialise();
    static VNODE3* Create();
    void Free(VNODE3* vnode);
    static void FreeAll();

    QNODE3& Child(int c) { return Children[c]; }
    const QNODE3& Child(int c) const { return Children[c]; }

    void SetChildren(double max, double min);


    static int NumChildren;
		
		std::vector<QNODE3> Children;
private:
		
    static MEMORY_POOL<VNODE3> VNodePool;
};

