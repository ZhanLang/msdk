#pragma once

#include <string>
using namespace std;

class CXmlExceptionW
{
public:

	CXmlExceptionW(const std::wstring& message, int code=-1) :
	  _message(message), _code(code) {}

	const std::wstring& GetMessage() const { return _message; }

	int GetCode() const { return _code; }

private:
	std::wstring _message;
	int _code;
};

