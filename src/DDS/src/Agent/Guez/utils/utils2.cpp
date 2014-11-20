#include "utils2.h"

namespace UTILS
{

void UnitTest()
{
    assert(Sign(+10) == +1);
    assert(Sign(-10) == -1);
    assert(Sign(0) == 0);

    int n[6] = { 0 };
    for (int i = 0; i < 10000; i++)
        for (int j = 1; j < 6; j++)
            n[j] += (Random(j) == 0);
    assert(Near(n[1], 10000, 0));
    assert(Near(n[2], 5000, 250));
    assert(Near(n[3], 3333, 250));
    assert(Near(n[4], 2500, 250));
    assert(Near(n[5], 2000, 250));

    int c = 0;
    for (int i = 0; i < 10000; i++)
        c += Bernoulli(0.5);
    assert(Near(c, 5000, 250));
    assert(CheckFlag(5, 0));
    assert(!CheckFlag(5, 1));
    assert(CheckFlag(5, 2));
    assert(!CheckFlag(5, 3));
    int flag = 1;
    SetFlag(flag, 2);
    SetFlag(flag, 4);
    assert(flag == 21);
}

}
