#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>

class Exception : public std::exception {
	private:
		const char* errorStr;

	public:
		Exception(const char *errorStr);
		const char* what() const noexcept override; 
};

#endif //EXCEPTION_H
