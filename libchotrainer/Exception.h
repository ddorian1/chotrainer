#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>

class Exception : public std::exception {
	private:
		const std::string errorStr;

	public:
		explicit Exception(const char *errorStr);
		explicit Exception(const std::string &errorStr);
		const char* what() const noexcept override; 
};

#endif //EXCEPTION_H
