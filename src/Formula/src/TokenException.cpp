
#include "TokenException.h"


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
TokenException::TokenException(std::string msg_) : msg(msg_) {}


TokenException::~TokenException() throw() {}


// ===========================================================================
//	Public methods
// ===========================================================================
const char* TokenException::what() const throw() { return msg.c_str(); }
