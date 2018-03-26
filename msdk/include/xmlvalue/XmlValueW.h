#pragma once

#include<vector>
#include <map>

namespace msdk{;

class CXmlValueW
{
public:
	typedef std::vector<char> BinaryDataW;
	typedef std::vector<CXmlValueW> ValueArrayW;
	typedef std::map<std::wstring, CXmlValueW> ValueStructW;

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

	CXmlValueW();
	CXmlValueW(bool value);
	CXmlValueW(int value);
	CXmlValueW(double value);
	CXmlValueW(std::wstring const& value);
	CXmlValueW(const wchar_t* value);
	CXmlValueW(struct tm* value);
	CXmlValueW(void* value, int nBytes)	;
	CXmlValueW(std::wstring const& xml, int* offset);
	CXmlValueW(CXmlValueW const& rhs);
	~CXmlValueW();

	void Clear();

	
	CXmlValueW& operator=(CXmlValueW const& rhs);
	CXmlValueW& operator=(int const& rhs);
	CXmlValueW& operator=(double const& rhs);
	CXmlValueW& operator=(const wchar_t* rhs);

	bool operator==(CXmlValueW const& other) const;
	bool operator!=(CXmlValueW const& other) const;


	operator bool&();
	operator int&()    ;
	operator double&() ;
	operator std::wstring&();
	operator BinaryDataW&();
	operator struct tm&() ;

	CXmlValueW const& operator[](int i) const;
	CXmlValueW& operator[](int i);

	CXmlValueW& operator[](std::wstring const& k);
	CXmlValueW& operator[](const wchar_t* k);

	bool BinaryDataToBuffer( void * pbuf, int size );
	bool Valid() const;
	Type const &GetType() const;

	int Size() const;
	void SetSize(int size);

	bool HasMember(const std::wstring& name) const;

	bool FromXml(std::wstring const& valueXml, int* offset);
	std::wstring ToXml() const;
	static std::wstring const& GetDoubleFormatW();
	static void SetDoubleFormat(const wchar_t* f);

protected:
	void Invalidate();

	
	void AssertTypeOrInvalid(Type t);
	void AssertArray(int size) const;
	void AssertArray(int size);
	void AssertStruct();

	
	bool BoolFromXml   (std::wstring const& valueXml, int* offset);
	bool IntFromXml    (std::wstring const& valueXml, int* offset);
	bool DoubleFromXml (std::wstring const& valueXml, int* offset);
	bool StringFromXml (std::wstring const& valueXml, int* offset);
	bool TimeFromXml   (std::wstring const& valueXml, int* offset);
	bool BinaryFromXml (std::wstring const& valueXml, int* offset);
	bool ArrayFromXml  (std::wstring const& valueXml, int* offset);
	bool StructFromXml (std::wstring const& valueXml, int* offset);

	
	std::wstring BoolToXml() const;
	std::wstring IntToXml() const;
	std::wstring DoubleToXml() const;
	std::wstring StringToXml() const;
	std::wstring TimeToXml() const;
	std::wstring BinaryToXml() const;
	std::wstring ArrayToXml() const;
	std::wstring StructToXml() const;



	static std::wstring _doubleFormat;

	Type _type;

	typedef union 
	{
		bool			asBool;
		int				asInt;
		double			asDouble;
		struct tm*		asTime;
		std::wstring*	asString;
		BinaryDataW*	asBinary;
		ValueArrayW*	asArray;
		ValueStructW*	asStruct;
	} XMLDataW;


	XMLDataW _value;
};

};//namespace msdk