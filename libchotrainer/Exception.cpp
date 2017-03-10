#include "Exception.h"

Exception::Exception(const char* errorStr)
:
	errorStr(errorStr)
{}

Exception::Exception(const std::string &errorStr)
:
	errorStr(errorStr)
{}

const char* Exception::what() const noexcept {
	return errorStr.c_str();
}
