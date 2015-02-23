// in 'custom.cpp'

#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "DDS/DDSLib.h"
#include "Utils/UtilsLib.h"

using namespace std;
using namespace dds;
using namespace utils;


int main(int argc, char* argv[])
{
     bool hasSeed = parsing::hasFlag(argc, argv, "--seed");
     if (hasSeed)
     {
          int seed = atoi(parsing::getValue(argc, argv, "--seed").c_str());
          init((seed > 0) ? seed : 0);
     }
     else { init(); }


     //
     //

     
     //   Return
     return 0;
}
