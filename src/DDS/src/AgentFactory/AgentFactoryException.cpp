
#include "AgentFactoryException.h"


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
AgentFactoryException::AgentFactoryException(std::string msg_) : msg(msg_) {}


AgentFactoryException::~AgentFactoryException() throw() {}


// ===========================================================================
//	Public methods
// ===========================================================================
const char* AgentFactoryException::what() const throw() { return msg.c_str(); }
