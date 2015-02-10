
//   boost
#ifdef BOOST_FOUND
#include <boost/assert.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/thread.hpp>
#endif


//   standard libraries
#include <algorithm>
#include <cassert>
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <map>
#include <queue>
#include <pthread.h>
#include <set>
#include <stdexcept>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <vector>


//   zlib
#ifdef ZLIB_FOUND
#include <zlib.h>

#else
#include "src/zlib/zlib.h"

#endif
