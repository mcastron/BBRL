
#include "FormulaException.h"


// ===========================================================================
//	Public Constructors/Destructors
// ===========================================================================
FormulaException::FormulaException(std::string msg_) : msg(msg_) {}


FormulaException::~FormulaException() throw() {}


// ===========================================================================
//	Public methods
// ===========================================================================
const char* FormulaException::what() const throw() { return msg.c_str(); }
