
#include "MDPException.h"


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
MDPException::MDPException(std::string msg_) : msg(msg_) {}


MDPException::~MDPException() throw() {}


// ===========================================================================
//	Public methods
// ===========================================================================
const char* MDPException::what() const throw() { return msg.c_str(); }
