
#include "ExperimentException.h"


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
ExperimentException::ExperimentException(std::string msg_) : msg(msg_) {}


ExperimentException::~ExperimentException() throw() {}


// ===========================================================================
//	Public methods
// ===========================================================================
const char* ExperimentException::what() const throw() { return msg.c_str(); }
