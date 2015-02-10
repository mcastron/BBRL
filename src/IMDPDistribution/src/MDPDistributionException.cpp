
#include "MDPDistributionException.h"


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
MDPDistributionException::MDPDistributionException(std::string msg_) :
		msg(msg_) {}


MDPDistributionException::~MDPDistributionException() throw() {}


// ===========================================================================
//	Public methods
// ===========================================================================
const char* MDPDistributionException::what() const throw()
{
	return msg.c_str();
}
