// in 'custom.cpp'
//   (mu == 1.0, sigma == 1.0)
//        'alpha = 1.0' & 'beta'  =   0.19 --> Ini distance ~= Update distance (first iteration only)
//        'alpha + beta' == 22.5  --> Score ~= Min distance (first iteration)


#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

#include "DDS/DDSLib.h"
#include "Utils/UtilsLib.h"
#include "Xport/XportLib.h"

using namespace std;
using namespace dds;
using namespace utils;
using namespace xport;


int main(int argc, char* argv[])
{
     bool hasSeed = parsing::hasFlag(argc, argv, "--seed");
     if (hasSeed)
     {
          int seed = atoi(parsing::getValue(argc, argv, "--seed").c_str());
          init((seed > 0) ? seed : 0);
     }
     else { init(); }


     /* YOUR CODE HERE */
     
     /* YOUR CODE HERE */


     //   Return
     return 0;
}
