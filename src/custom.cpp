// in 'custom.cpp'

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


     //   Return
     return 0;
}
