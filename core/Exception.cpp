#include "Exception.h"

Exception::Exception(const char* errorStr)
:
	errorStr(errorStr)
{}

const char* Exception::what() const noexcept {
	return errorStr;
}
