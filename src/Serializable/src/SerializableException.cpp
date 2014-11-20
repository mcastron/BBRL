
#include "SerializableException.h"


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
SerializableException::SerializableException(std::string msg_) : msg(msg_) {}


SerializableException::~SerializableException() throw() {}


// ===========================================================================
//	Public methods
// ===========================================================================
const char* SerializableException::what() const throw() { return msg.c_str(); }
