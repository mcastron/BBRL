
#include "ThreadException.h"


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
utils::ThreadException::ThreadException(std::string msg_) : msg(msg_) {}


utils::ThreadException::~ThreadException() throw() {}


// ===========================================================================
//	Public methods
// ===========================================================================
const char* utils::ThreadException::what() const throw() { return msg.c_str(); }
