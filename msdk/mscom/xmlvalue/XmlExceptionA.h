
#pragma once

#include <string>
using namespace std;

class CXmlExceptionA 
{
public:
	CXmlExceptionA(const std::string& message, int code=-1) :
	  _message(message), _code(code) {}

	 const std::string& getMessage() const { return _message; }

	 int getCode() const { return _code; }

private:
	std::string _message;
	int _code;
};