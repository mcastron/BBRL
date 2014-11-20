
#include "FVariableException.h"


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
FVariableException::FVariableException(std::string msg_) : msg(msg_) {}


FVariableException::~FVariableException() throw() {}


// ===========================================================================
//	Public methods
// ===========================================================================
const char* FVariableException::what() const throw() { return msg.c_str(); }
