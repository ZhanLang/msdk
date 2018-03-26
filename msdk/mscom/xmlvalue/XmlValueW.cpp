#include "stdafx.h"
#include "xmlvalue/XmlValueW.h"


#include "base64.h"
#include "XmlExceptionW.h"
#include "XmlUtilW.h"


#pragma warning(disable: 4996)

namespace msdk{;
// Format strings
std::wstring CXmlValueW::_doubleFormat(L"%f");


//! Constructors
CXmlValueW::CXmlValueW()				
	: _type(TypeInvalid)	
{ 
	_value.asBinary = 0;     
}
CXmlValueW::CXmlValueW(bool value)	
	: _type(TypeBoolean)	
{
	_value.asBool   = value; 
}

CXmlValueW::CXmlValueW(int value)		
	: _type(TypeInt)		
{ 
	_value.asInt    = value; 
}

CXmlValueW::CXmlValueW(double value)	
	: _type(TypeDouble)		
{ 
	_value.asDouble = value; 
}

CXmlValueW::CXmlValueW(std::wstring const& value)	
	: _type(TypeString) 
{
	_value.asString = new std::wstring(value); 
}

CXmlValueW::CXmlValueW(const wchar_t* value)		
	: _type(TypeString)
{ 
	_value.asString = new std::wstring(value); 
}

CXmlValueW::CXmlValueW(struct tm* value)	
	: _type(TypeDateTime) 
{
	_value.asTime = new struct tm(*value);
}

CXmlValueW::CXmlValueW(void* value, int nBytes)		: _type(TypeBase64)				// ²âÊÔ
{
	_value.asBinary = new BinaryDataW((char*)value, ((char*)value)+nBytes);
}

//! Construct from xml, beginning at *offset chars into the string, updates offset
CXmlValueW::CXmlValueW(std::wstring const& xml, int* offset) : _type(TypeInvalid)
{ 
	if ( !FromXml(xml,offset) ) 
		_type = TypeInvalid; 
}

//! Copy
CXmlValueW::CXmlValueW(CXmlValueW const& rhs) 
	: _type(TypeInvalid) 
{ 
	*this = rhs;
}


//! Destructor (make virtual if you want to subclass)
CXmlValueW::~CXmlValueW() 
{ 
	Invalidate(); 
}

//! Erase the current value
void CXmlValueW::Clear() 
{ 
	Invalidate();
}

CXmlValueW& CXmlValueW::operator=(int const& rhs)
{ 
	return operator=(CXmlValueW(rhs)); 
}

CXmlValueW& CXmlValueW::operator=(double const& rhs) 
{ 
	return operator=(CXmlValueW(rhs)); 
}

CXmlValueW& CXmlValueW::operator=(const wchar_t* rhs)	
{ 
	return operator=(CXmlValueW(std::wstring(rhs))); 
}

void CXmlValueW::Invalidate()
{
	switch (_type) {
	case TypeString:    delete _value.asString; break;
	case TypeDateTime:  delete _value.asTime;   break;
	case TypeBase64:    delete _value.asBinary; break;
	case TypeArray:     delete _value.asArray;  break;
	case TypeStruct:    delete _value.asStruct; break;
	default: break;
	}
	_type = TypeInvalid;
	_value.asBinary = 0;
}


// Type checking
void CXmlValueW::AssertTypeOrInvalid(Type t)
{
	if (_type == TypeInvalid)
	{
		_type = t;
		switch (_type) {    // Ensure there is a valid value for the type
		case TypeString:   _value.asString = new std::wstring(); break;
		case TypeDateTime: _value.asTime   = new struct tm();    break;
		case TypeBase64:   _value.asBinary = new BinaryDataW();   break;
		case TypeArray:    _value.asArray  = new ValueArrayW();   break;
		case TypeStruct:   _value.asStruct = new ValueStructW();  break;
		default:           _value.asBinary = 0; break;
		}
	}
	else if (_type != t)
	{
		throw CXmlExceptionW( L"type error" );
	}
}


void CXmlValueW::AssertArray(int size) const
{
	if (_type != TypeArray)
	{
		throw CXmlExceptionW(L"type error: expected an array");	
	}
	else if (int(_value.asArray->size()) < size)
	{
		throw CXmlExceptionW(L"range error: array index too large");
	}
}


void CXmlValueW::AssertArray(int size)
{
	if (_type == TypeInvalid) 
	{
		_type = TypeArray;
		_value.asArray = new ValueArrayW(size);
	}
	else if (_type == TypeArray) 
	{
		if (int(_value.asArray->size()) < size)
			_value.asArray->resize(size);
	} 
	else
	{
		throw CXmlExceptionW(L"type error: expected an array");
	}
}


void CXmlValueW::AssertStruct()
{
	if (_type == TypeInvalid) 
	{
		_type = TypeStruct;
		_value.asStruct = new ValueStructW();
	}
	else if (_type != TypeStruct)
	{
		throw CXmlExceptionW(L"type error: expected a struct");
	}
}


// Operators
CXmlValueW& CXmlValueW::operator=(CXmlValueW const& rhs)
{
	if (this != &rhs)
	{
		Invalidate();
		_type = rhs._type;

		switch (_type) 
		{
		case TypeBoolean:  _value.asBool   = rhs._value.asBool;   break;
		case TypeInt:      _value.asInt    = rhs._value.asInt;    break;
		case TypeDouble:   _value.asDouble = rhs._value.asDouble; break;
		case TypeDateTime: _value.asTime   = new struct tm(*rhs._value.asTime);      break;
		case TypeString:   _value.asString = new std::wstring(*rhs._value.asString); break;
		case TypeBase64:   _value.asBinary = new BinaryDataW(*rhs._value.asBinary);   break;
		case TypeArray:    _value.asArray  = new ValueArrayW(*rhs._value.asArray);    break;
		case TypeStruct:   _value.asStruct = new ValueStructW(*rhs._value.asStruct);  break;
		default:           _value.asBinary = 0; break;
		}
	}
	return *this;
}


// Predicate for tm equality
static bool tmEqW(struct tm const& t1, struct tm const& t2)
{
	return	t1.tm_sec == t2.tm_sec && t1.tm_min == t2.tm_min &&
		t1.tm_hour == t2.tm_hour && t1.tm_mday == t1.tm_mday &&
		t1.tm_mon == t2.tm_mon && t1.tm_year == t2.tm_year;
}


bool CXmlValueW::operator==(CXmlValueW const& other) const
{
	if (_type != other._type)
	{
		return false;
	}

	switch (_type)
	{
	case TypeBoolean:  
		{
			return ( !_value.asBool && !other._value.asBool) ||
				( _value.asBool && other._value.asBool);
		}

	case TypeInt:     
		{
			return _value.asInt == other._value.asInt;
		}

	case TypeDouble:   
		{
			return _value.asDouble == other._value.asDouble;
		}

	case TypeDateTime: 
		{
			return tmEqW(*_value.asTime, *other._value.asTime);
		}

	case TypeString:   
		{
			return *_value.asString == *other._value.asString;
		}

	case TypeBase64:   
		{
			return *_value.asBinary == *other._value.asBinary;
		}

	case TypeArray:    
		{
			return *_value.asArray == *other._value.asArray;
		}

		// The map<>::operator== requires the definition of value< for kcc
	case TypeStruct:   //return *_value.asStruct == *other._value.asStruct;
		{
			if (_value.asStruct->size() != other._value.asStruct->size())
				return false;

			ValueStructW::const_iterator it1=_value.asStruct->begin();
			ValueStructW::const_iterator it2=other._value.asStruct->begin();

			while (it1 != _value.asStruct->end())
			{
				const CXmlValueW& v1 = it1->second;
				const CXmlValueW& v2 = it2->second;
				if ( ! (v1 == v2) )
					return false;
				it1++;
				it2++;
			}
			return true;
		}
	default: break;
	}
	return true;    // Both invalid values ...
}

bool CXmlValueW::operator!=(CXmlValueW const& other) const
{
	return !(*this == other);
}

CXmlValueW::operator bool&()     
{ 
	AssertTypeOrInvalid(TypeBoolean);	
	return _value.asBool;
}

CXmlValueW::operator int&()         
{
	AssertTypeOrInvalid(TypeInt);		
	return _value.asInt;
}

CXmlValueW::operator double&()       
{
	AssertTypeOrInvalid(TypeDouble);	
	return _value.asDouble; 
}

CXmlValueW::operator std::wstring&() 
{ 
	AssertTypeOrInvalid(TypeString);
	return *_value.asString; 
}

CXmlValueW::operator BinaryDataW&()   
{
	AssertTypeOrInvalid(TypeBase64);
	return *_value.asBinary; 
}

CXmlValueW::operator struct tm&()   
{
	AssertTypeOrInvalid(TypeDateTime);	
	return *_value.asTime;
}

CXmlValueW const& CXmlValueW::operator[](int i) const
{ 
	AssertArray(i+1); return _value.asArray->at(i);
}

CXmlValueW& CXmlValueW::operator[](int i)       
{ 
	AssertArray(i+1); return _value.asArray->at(i); 
}


CXmlValueW& CXmlValueW::operator[](std::wstring const& k) 
{ 
	AssertStruct(); return (*_value.asStruct)[k]; 
}

CXmlValueW& CXmlValueW::operator[](const wchar_t* k) 
{ 
	AssertStruct(); std::wstring s(k);
	return (*_value.asStruct)[s]; 
}


bool CXmlValueW::Valid() const 
{
	return _type != TypeInvalid; 
}

CXmlValueW::Type const &CXmlValueW::GetType() const 
{
	return _type;
}

// Works for strings, binary data, arrays, and structs.
int CXmlValueW::Size() const
{
	switch (_type) 
	{
	case TypeString: return int(_value.asString->size());
	case TypeBase64: return int(_value.asBinary->size() * sizeof(char) );
	case TypeArray:  return int(_value.asArray->size());
	case TypeStruct: return int(_value.asStruct->size());
	default: break;
	}

	throw CXmlExceptionW(L"type error");
}
void CXmlValueW::SetSize(int size)  
{ 
	AssertArray(size);
}

// Checks for existence of struct member
bool CXmlValueW::HasMember(const std::wstring& name) const
{
	return _type == TypeStruct && _value.asStruct->find(name) != _value.asStruct->end();
}


// Set the value from xml. The chars at *offset into valueXml 
// should be the start of a <value> tag. Destroys any existing value.
bool CXmlValueW::FromXml(std::wstring const& valueXml, int* offset)
{
	int savedOffset =  offset ? *offset : 0;

	Invalidate();
	if ( ! CXmlUtilW::NextTagIs(VALUE_TAGW, valueXml, offset))
		return false;       // Not a value, offset not updated

	int afterValueOffset = *offset;

	std::wstring typeTag = CXmlUtilW::GetNextTag(valueXml, offset);

	bool result = false;

	if ( typeTag == BOOLEAN_TAGW )
	{
		result = BoolFromXml(valueXml, offset);
	}
	else if ( typeTag == I4_TAGW || typeTag == INT_TAGW )
	{
		result = IntFromXml(valueXml, offset);
	}
	else if ( typeTag == DOUBLE_TAGW )
	{
		result = DoubleFromXml(valueXml, offset);
	}
	else if (typeTag.empty() || typeTag == STRING_TAGW)
	{
		result = StringFromXml(valueXml, offset);
	}
	else if ( typeTag == DATETIME_TAGW )
	{
		result = TimeFromXml(valueXml, offset);
	}
	else if ( typeTag == BASE64_TAGW )
	{
		result = BinaryFromXml(valueXml, offset);
	}
	else if ( typeTag == ARRAY_TAGW )
	{
		result = ArrayFromXml(valueXml, offset);
	}
	else if ( typeTag == STRUCT_TAGW )
	{
		result = StructFromXml(valueXml, offset);
	}
	// Watch for empty/blank strings with no <string>tag
	else if ( typeTag == VALUE_ETAGW  )
	{
		*offset = afterValueOffset;   // back up & try again
		result = StringFromXml(valueXml, offset);
	}

	if (result)  // Skip over the </value> tag
		CXmlUtilW::FindTag(VALUE_ETAGW, valueXml, offset);
	else        // Unrecognized tag after <value>
		*offset = savedOffset;

	return result;
}


// Boolean
bool CXmlValueW::BoolFromXml(std::wstring const& valueXml, int* offset)
{
	const wchar_t* valueStart = valueXml.c_str() + *offset;
	wchar_t* valueEnd;
	long ivalue = wcstol(valueStart, &valueEnd, 10);

	if (valueEnd == valueStart || (ivalue != 0 && ivalue != 1))
		return false;

	_type = TypeBoolean;
	_value.asBool = (ivalue == 1);
	*offset += int(valueEnd - valueStart);
	return true;
}


std::wstring CXmlValueW::BoolToXml() const
{
	std::wstring xml = VALUE_TAGW;
	xml += BOOLEAN_TAGW;
	xml += (_value.asBool ? L"1" : L"0");
	xml += BOOLEAN_ETAGW;
	xml += VALUE_ETAGW;
	return xml;
}

// Int
bool CXmlValueW::IntFromXml(std::wstring const& valueXml, int* offset)
{
	const wchar_t* valueStart = valueXml.c_str() + *offset;
	wchar_t* valueEnd;

	long ivalue = wcstol(valueStart, &valueEnd, 10);

	if (valueEnd == valueStart)
		return false;

	_type = TypeInt;
	_value.asInt = int(ivalue);
	*offset += int(valueEnd - valueStart);
	return true;
}


std::wstring CXmlValueW::IntToXml() const
{
	wchar_t buf[MIN_COUNT];
	_snwprintf( buf, MIN_COUNT - 1, L"%d", _value.asInt);

	buf[MIN_COUNT - 1] = 0;

	std::wstring xml = VALUE_TAGW;
	xml += I4_TAGW;
	xml += buf;
	xml += I4_ETAGW;
	xml += VALUE_ETAGW;
	return xml;
}


// Double
bool CXmlValueW::DoubleFromXml(std::wstring const& valueXml, int* offset)
{
	const wchar_t* valueStart = valueXml.c_str() + *offset;

	wchar_t* valueEnd;

	double dvalue = wcstod(valueStart, &valueEnd);

	if (valueEnd == valueStart)
		return false;

	_type = TypeDouble;
	_value.asDouble = dvalue;
	*offset += int(valueEnd - valueStart);
	return true;
}


std::wstring CXmlValueW::DoubleToXml() const
{
	wchar_t buf[MIN_COUNT];
	_snwprintf( buf, MIN_COUNT - 1, GetDoubleFormatW().c_str(), _value.asDouble);

	buf[MIN_COUNT - 1] = 0;

	std::wstring xml = VALUE_TAGW;
	xml += DOUBLE_TAGW;
	xml += buf;
	xml += DOUBLE_ETAGW;
	xml += VALUE_ETAGW;
	return xml;
}


// String
bool CXmlValueW::StringFromXml(std::wstring const& valueXml, int* offset)
{
	size_t valueEnd = valueXml.find(L'<', *offset);

	if (valueEnd == std::wstring::npos)
		return false;     // No end tag;

	_type = TypeString;
	_value.asString = new std::wstring( CXmlUtilW::XmlDecode(valueXml.substr(*offset, valueEnd-*offset)));
	*offset += int(_value.asString->length());
	return true;
}

std::wstring CXmlValueW::StringToXml() const
{
	std::wstring xml = VALUE_TAGW;
	//xml += STRING_TAG; optional
	xml += CXmlUtilW::XmlEncode(*_value.asString);
	//xml += STRING_ETAG;
	xml += VALUE_ETAGW;
	return xml;
}


// DateTime (stored as a struct tm)
bool CXmlValueW::TimeFromXml(std::wstring const& valueXml, int* offset)
{
	size_t valueEnd = valueXml.find( L'<', *offset);
	if (valueEnd == std::wstring::npos)
		return false;     // No end tag;

	std::wstring stime = valueXml.substr(*offset, valueEnd-*offset);

	struct tm t;
	if ( swscanf( stime.c_str(), L"%4d%2d%2dT%2d:%2d:%2d",&t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec) != 6 )
		return false;

	t.tm_isdst = -1;
	_type = TypeDateTime;
	_value.asTime = new struct tm(t);
	*offset += int(stime.length());
	return true;
}


std::wstring CXmlValueW::TimeToXml() const
{
	struct tm* t = _value.asTime;
	wchar_t buf[MIN_COUNT];

	_snwprintf (buf, MIN_COUNT-1, L"%4d%02d%02dT%02d:%02d:%02d", 
		t->tm_year,t->tm_mon,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec );

	buf[MIN_COUNT-1] = 0;

	std::wstring xml = VALUE_TAGW;
	xml += DATETIME_TAGW;
	xml += buf;
	xml += DATETIME_ETAGW;
	xml += VALUE_ETAGW;
	return xml;
}


// Base64
bool CXmlValueW::BinaryFromXml(std::wstring const& valueXml, int* offset)
{
	size_t valueEnd = valueXml.find( L'<', *offset);

	if (valueEnd == std::wstring::npos)
		return false;     // No end tag;

	_type = TypeBase64;
	std::wstring asString = valueXml.substr(*offset, valueEnd-*offset);
	_value.asBinary = new BinaryDataW();
	// check whether base64 encodings can contain chars xml encodes...

	// convert from base64 to binary
	int iostatus = 0;
	base64<char> decoder;
	std::back_insert_iterator<BinaryDataW> ins = std::back_inserter(*(_value.asBinary));
	decoder.get(asString.begin(), asString.end(), ins, iostatus);

	*offset += int(asString.length());
	return true;
}


std::wstring CXmlValueW::BinaryToXml() const
{
	// convert to base64
	std::vector<wchar_t> base64data;
	int iostatus = 0;
	base64<char> encoder;
	std::back_insert_iterator<std::vector<wchar_t> > ins = std::back_inserter(base64data);

	encoder.put(_value.asBinary->begin(), _value.asBinary->end(), ins, iostatus, base64<>::crlf());

	// 	// Wrap with xml
	std::wstring xml = VALUE_TAGW;
	xml += BASE64_TAGW;
	xml.append(base64data.begin(), base64data.end());
	xml += BASE64_ETAGW;
	xml += VALUE_ETAGW;
	return xml;
}


// Array
bool CXmlValueW::ArrayFromXml(std::wstring const& valueXml, int* offset)
{
	if ( ! CXmlUtilW::NextTagIs(DATA_TAGW, valueXml, offset))
		return false;

	_type = TypeArray;
	_value.asArray = new ValueArrayW;

	CXmlValueW v;
	while (v.FromXml(valueXml, offset))
		_value.asArray->push_back(v);       // copy...

	// Skip the trailing </data>
	(void) CXmlUtilW::NextTagIs(DATA_ETAGW, valueXml, offset);
	return true;
}


// In general, its preferable to generate the xml of each element of the
// array as it is needed rather than glomming up one big string.
std::wstring CXmlValueW::ArrayToXml() const
{
	std::wstring xml = VALUE_TAGW;
	xml += ARRAY_TAGW;
	xml += DATA_TAGW;

	int s = int(_value.asArray->size());

	for (int i=0; i<s; ++i)
		xml += _value.asArray->at(i).ToXml();

	xml += DATA_ETAGW;
	xml += ARRAY_ETAGW;
	xml += VALUE_ETAGW;
	return xml;
}


// Struct
bool CXmlValueW::StructFromXml(std::wstring const& valueXml, int* offset)
{
	_type = TypeStruct;
	_value.asStruct = new ValueStructW;

	while ( CXmlUtilW::NextTagIs(MEMBER_TAGW, valueXml, offset)) 
	{
		// name
		const std::wstring name = CXmlUtilW::ParseTag(NAME_TAGW, valueXml, offset);

		// value
		CXmlValueW val(valueXml, offset);

		if ( ! val.Valid() ) 
		{
			Invalidate();
			return false;
		}
		const std::pair<const std::wstring, CXmlValueW> p(name, val);
		_value.asStruct->insert(p);

		(void) CXmlUtilW::NextTagIs(MEMBER_ETAGW, valueXml, offset);
	}
	return true;
}


// In general, its preferable to generate the xml of each element
// as it is needed rather than glomming up one big string.
std::wstring CXmlValueW::StructToXml() const
{
	std::wstring xml = VALUE_TAGW;
	xml += STRUCT_TAGW;

	ValueStructW::const_iterator it;
	for (it=_value.asStruct->begin(); it!=_value.asStruct->end(); ++it) 
	{
		xml += MEMBER_TAGW;
		xml += NAME_TAGW;
		xml += CXmlUtilW::XmlEncode(it->first);
		xml += NAME_ETAGW;
		xml += it->second.ToXml();
		xml += MEMBER_ETAGW;
	}

	xml += STRUCT_ETAGW;
	xml += VALUE_ETAGW;
	return xml;
}


// Encode the Value in xml
std::wstring CXmlValueW::ToXml() const
{
	switch (_type)
	{
	case TypeBoolean:  return BoolToXml();
	case TypeInt:      return IntToXml();
	case TypeDouble:   return DoubleToXml();
	case TypeString:   return StringToXml();
	case TypeDateTime: return TimeToXml();
	case TypeBase64:   return BinaryToXml();
	case TypeArray:    return ArrayToXml();
	case TypeStruct:   return StructToXml();
	default: break;
	}
	return std::wstring();   // Invalid value
}

std::wstring const& CXmlValueW::GetDoubleFormatW() { return _doubleFormat; }
void CXmlValueW::SetDoubleFormat(const wchar_t* f) { _doubleFormat = f; }

bool CXmlValueW::BinaryDataToBuffer( void * pbuf, int size )
{
	char *pwch = NULL;
	if( _type != TypeBase64 ) { return false; }
	if( !pbuf ) { return false; }
	if( size < Size() ) { return false; }

	pwch = (char*)pbuf;

	for( BinaryDataW::iterator it = (*_value.asBinary).begin(); it != (*_value.asBinary).end(); it++ )
	{
		*pwch = *it;
		++pwch; 
	}

	return true;
}


};