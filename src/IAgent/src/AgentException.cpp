
#include "AgentException.h"


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
AgentException::AgentException(std::string msg_) : msg(msg_) {}


AgentException::~AgentException() throw() {}


// ===========================================================================
//	Public methods
// ===========================================================================
const char* AgentException::what() const throw() { return msg.c_str(); }
