
#pragma once

#include<map>
#include <vector>
namespace msdk{;

class CXmlValueA
{
public:

	typedef std::vector<char> BinaryDataA;
	typedef std::vector<CXmlValueA> ValueArrayA;
	typedef std::map<std::string, CXmlValueA> ValueStructA;



	enum Type 
	{
		TypeInvalid,
		TypeBoolean,
		TypeInt,
		TypeDouble,
		TypeString,
		TypeDateTime,
		TypeBase64,
		TypeArray,
		TypeStruct
	};

	CXmlValueA();
	CXmlValueA(bool value);
	CXmlValueA(int value);
	CXmlValueA(double value);
	CXmlValueA(std::string const& value);
	CXmlValueA(const char* value);
	CXmlValueA(struct tm* value);
	CXmlValueA(void* value, int nBytes);
	CXmlValueA(std::string const& xml, int* offset);
	CXmlValueA(CXmlValueA const& rhs);
	~CXmlValueA();
	void Clear();

	CXmlValueA& operator=(CXmlValueA const& rhs);
	CXmlValueA& operator=(int const& rhs);
	CXmlValueA& operator=(double const& rhs);
	CXmlValueA& operator=(const char* rhs);

	bool operator==(CXmlValueA const& other) const;
	bool operator!=(CXmlValueA const& other) const;

	operator bool&();
	operator int&();
	operator double&();
	operator std::string&();
	operator BinaryDataA&();
	operator struct tm&();

	CXmlValueA const& operator[](int i) const;
	CXmlValueA& operator[](int i);

	CXmlValueA& operator[](std::string const& k);
	CXmlValueA& operator[](const char* k);

	bool Valid() const;
	Type const &GetType() const;
	int Size() const;
	void SetSize(int size);
	bool HasMember(const std::string& name) const;
	bool FromXml(std::string const& valueXml, int* offset);
	std::string ToXml() const;
	static std::string const& GetDoubleFormatA();
	static void SetDoubleFormat(const char* f);

protected:
	
	void Invalidate();

	
	void AssertTypeOrInvalid(Type t);
	void AssertArray(int size) const;
	void AssertArray(int size);
	void AssertStruct();

	
	bool BoolFromXml(std::string const& valueXml, int* offset);
	bool IntFromXml(std::string const& valueXml, int* offset);
	bool DoubleFromXml(std::string const& valueXml, int* offset);
	bool StringFromXml(std::string const& valueXml, int* offset);
	bool TimeFromXml(std::string const& valueXml, int* offset);
	bool BinaryFromXml(std::string const& valueXml, int* offset);
	bool ArrayFromXml(std::string const& valueXml, int* offset);
	bool StructFromXml(std::string const& valueXml, int* offset);

	
	std::string BoolToXml() const;
	std::string IntToXml() const;
	std::string DoubleToXml() const;
	std::string StringToXml() const;
	std::string TimeToXml() const;
	std::string BinaryToXml() const;
	std::string ArrayToXml() const;
	std::string StructToXml() const;

	
	static std::string _doubleFormat;

	
	Type _type;

	typedef union 
	{
		bool			asBool;
		int				asInt;
		double			asDouble;
		struct tm*		asTime;
		std::string*	asString;
		BinaryDataA*	asBinary;
		ValueArrayA*	asArray;
		ValueStructA*	asStruct;
	} XMLDataA;

	
	XMLDataA _value;

};


};//namespace msdk