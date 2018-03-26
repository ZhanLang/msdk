#include "stdafx.h"
#include "xmlvalue/XmlValueA.h"

#include "base64.h"
#include "XmlExceptionA.h"
#include "XmlUtilA.h"


#pragma warning(disable: 4996)

namespace msdk{;
std::string CXmlValueA::_doubleFormat("%f");


CXmlValueA::CXmlValueA()				
	: _type(TypeInvalid) 
{ 
	_value.asBinary = 0; 
}

CXmlValueA::CXmlValueA(bool value)		
	: _type(TypeBoolean) 
{
	_value.asBool = value; 
}

CXmlValueA::CXmlValueA(int value)		
	: _type(TypeInt)	 
{ 
	_value.asInt = value; 
}

CXmlValueA::CXmlValueA(double value)	
	: _type(TypeDouble) 
{
	_value.asDouble = value; 
}

CXmlValueA::CXmlValueA(std::string const& value)	
	: _type(TypeString) 
{ 
	_value.asString = new std::string(value); 
}


CXmlValueA::CXmlValueA(const char* value)			
	: _type(TypeString)
{ 
	_value.asString = new std::string(value); 
}

CXmlValueA::CXmlValueA(struct tm* value)		
	: _type(TypeDateTime) 
{ 
	_value.asTime = new struct tm(*value); 
}


CXmlValueA::CXmlValueA(void* value, int nBytes)		
	: _type(TypeBase64)
{
	_value.asBinary = new BinaryDataA((char*)value, ((char*)value)+nBytes);
}

CXmlValueA::CXmlValueA(std::string const& xml, int* offset) 
	: _type(TypeInvalid)
{ 
	if ( !FromXml(xml,offset) ) 
		_type = TypeInvalid; 
}

CXmlValueA::CXmlValueA(CXmlValueA const& rhs)	
	: _type(TypeInvalid) 
{ 
	*this = rhs; 
}


CXmlValueA::~CXmlValueA() 
{ 
	Invalidate(); 
}


void CXmlValueA::Clear() 
{ 
	Invalidate();
}

void CXmlValueA::Invalidate()
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


void CXmlValueA::AssertTypeOrInvalid(Type t)
{
	if (_type == TypeInvalid)
	{
		_type = t;
		switch (_type) {   
		case TypeString:   _value.asString = new std::string(); break;
		case TypeDateTime: _value.asTime = new struct tm();     break;
		case TypeBase64:   _value.asBinary = new BinaryDataA();  break;
		case TypeArray:    _value.asArray = new ValueArrayA();   break;
		case TypeStruct:   _value.asStruct = new ValueStructA(); break;
		default:           _value.asBinary = 0; break;
		}
	}
	else if (_type != t)
		throw CXmlExceptionA("type error");
}

void CXmlValueA::AssertArray(int size) const
{
	if (_type != TypeArray)
		throw CXmlExceptionA("type error: expected an array");
	else if (int(_value.asArray->size()) < size)
		throw CXmlExceptionA("range error: array index too large");
}


void CXmlValueA::AssertArray(int size)
{
	if (_type == TypeInvalid) {
		_type = TypeArray;
		_value.asArray = new ValueArrayA(size);
	} else if (_type == TypeArray) {
		if (int(_value.asArray->size()) < size)
			_value.asArray->resize(size);
	} else
		throw CXmlExceptionA("type error: expected an array");
}

void CXmlValueA::AssertStruct()
{
	if (_type == TypeInvalid) {
		_type = TypeStruct;
		_value.asStruct = new ValueStructA();
	} else if (_type != TypeStruct)
		throw CXmlExceptionA("type error: expected a struct");
}


CXmlValueA& CXmlValueA::operator=(CXmlValueA const& rhs)
{
	if (this != &rhs)
	{
		Invalidate();
		_type = rhs._type;
		switch (_type) {
		case TypeBoolean:  _value.asBool = rhs._value.asBool; break;
		case TypeInt:      _value.asInt = rhs._value.asInt; break;
		case TypeDouble:   _value.asDouble = rhs._value.asDouble; break;
		case TypeDateTime: _value.asTime = new struct tm(*rhs._value.asTime); break;
		case TypeString:   _value.asString = new std::string(*rhs._value.asString); break;
		case TypeBase64:   _value.asBinary = new BinaryDataA(*rhs._value.asBinary); break;
		case TypeArray:    _value.asArray = new ValueArrayA(*rhs._value.asArray); break;
		case TypeStruct:   _value.asStruct = new ValueStructA(*rhs._value.asStruct); break;
		default:           _value.asBinary = 0; break;
		}
	}
	return *this;
}


CXmlValueA& CXmlValueA::operator=(int const& rhs) 
{ 
	return operator=(CXmlValueA(rhs)); 
}

CXmlValueA& CXmlValueA::operator=(double const& rhs) 
{ 
	return operator=(CXmlValueA(rhs)); 
}

CXmlValueA& CXmlValueA::operator=(const char* rhs) 
{ 
	return operator=(CXmlValueA(std::string(rhs))); 
}


static bool tmEqA(struct tm const& t1, struct tm const& t2) 
{
	return t1.tm_sec == t2.tm_sec && t1.tm_min == t2.tm_min &&
		t1.tm_hour == t2.tm_hour && t1.tm_mday == t1.tm_mday &&
		t1.tm_mon == t2.tm_mon && t1.tm_year == t2.tm_year;
}


bool CXmlValueA::operator==(CXmlValueA const& other) const
{
	if (_type != other._type)
		return false;

	switch (_type) {
	case TypeBoolean:  return ( !_value.asBool && !other._value.asBool) ||
						   ( _value.asBool && other._value.asBool);
	case TypeInt:      return _value.asInt == other._value.asInt;
	case TypeDouble:   return _value.asDouble == other._value.asDouble;
	case TypeDateTime: return tmEqA(*_value.asTime, *other._value.asTime);
	case TypeString:   return *_value.asString == *other._value.asString;
	case TypeBase64:   return *_value.asBinary == *other._value.asBinary;
	case TypeArray:    return *_value.asArray == *other._value.asArray;

		// The map<>::operator== requires the definition of value< for kcc
	case TypeStruct:   //return *_value.asStruct == *other._value.asStruct;
		{
			if (_value.asStruct->size() != other._value.asStruct->size())
				return false;

			ValueStructA::const_iterator it1=_value.asStruct->begin();
			ValueStructA::const_iterator it2=other._value.asStruct->begin();
			while (it1 != _value.asStruct->end()) 
			{
				const CXmlValueA& v1 = it1->second;
				const CXmlValueA& v2 = it2->second;
				if ( ! (v1 == v2))
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

CXmlValueA::operator bool&()         
{ 
	AssertTypeOrInvalid(TypeBoolean); return _value.asBool; 
}

CXmlValueA::operator int&()           
{ 
	AssertTypeOrInvalid(TypeInt); return _value.asInt; 
}

CXmlValueA::operator double&()        
{ 
	AssertTypeOrInvalid(TypeDouble); 
	return _value.asDouble; 
}

CXmlValueA::operator std::string&()   
{ 
	AssertTypeOrInvalid(TypeString); 
	return *_value.asString; 
}

CXmlValueA::operator BinaryDataA&()  
{ 
	AssertTypeOrInvalid(TypeBase64); 
	return *_value.asBinary;
}

CXmlValueA::operator struct tm&()     
{
	AssertTypeOrInvalid(TypeDateTime);
	return *_value.asTime; 
}

bool CXmlValueA::operator!=(CXmlValueA const& other) const
{
	return !(*this == other);
}

CXmlValueA const& CXmlValueA::operator[](int i) const	 
{ 
	AssertArray(i+1); return _value.asArray->at(i); 
}

CXmlValueA& CXmlValueA::operator[](int i)				
{
	AssertArray(i+1); 
	return _value.asArray->at(i); 
}

CXmlValueA& CXmlValueA::operator[](std::string const& k) 
{ 
	AssertStruct();
	return (*_value.asStruct)[k];
}

CXmlValueA& CXmlValueA::operator[](const char* k)	
{
	AssertStruct(); std::string s(k);
	return (*_value.asStruct)[s];
}


bool CXmlValueA::Valid() const
{
	return _type != TypeInvalid; 
}

CXmlValueA::Type const &CXmlValueA::GetType() const 
{ 
	return _type; 
}

int CXmlValueA::Size() const
{
	switch (_type) {
	case TypeString: return int(_value.asString->size());
	case TypeBase64: return int(_value.asBinary->size());
	case TypeArray:  return int(_value.asArray->size());
	case TypeStruct: return int(_value.asStruct->size());
	default: break;
	}

	throw CXmlExceptionA("type error");
}

void CXmlValueA::SetSize(int size)    
{ 
	AssertArray(size); 
}

bool CXmlValueA::HasMember(const std::string& name) const
{
	return _type == TypeStruct && _value.asStruct->find(name) != _value.asStruct->end();
}

// Set the value from xml. The chars at *offset into valueXml 
// should be the start of a <value> tag. Destroys any existing value.
bool CXmlValueA::FromXml(std::string const& valueXml, int* offset)
{
	int savedOffset = offset ? *offset : 0;

	Invalidate();
	if ( ! CXmlUtilA::NextTagIs(VALUE_TAGA, valueXml, offset))
		return false;       // Not a value, offset not updated

	int afterValueOffset = *offset;
	std::string typeTag = CXmlUtilA::GetNextTag(valueXml, offset);
	bool result = false;
	if (typeTag == BOOLEAN_TAGA)
		result = BoolFromXml(valueXml, offset);
	else if (typeTag == I4_TAGA || typeTag == INT_TAGA)
		result = IntFromXml(valueXml, offset);
	else if (typeTag == DOUBLE_TAGA)
		result = DoubleFromXml(valueXml, offset);
	else if (typeTag.empty() || typeTag == STRING_TAGA)
		result = StringFromXml(valueXml, offset);
	else if (typeTag == DATETIME_TAGA)
		result = TimeFromXml(valueXml, offset);
	else if (typeTag == BASE64_TAGA)
		result = BinaryFromXml(valueXml, offset);
	else if (typeTag == ARRAY_TAGA)
		result = ArrayFromXml(valueXml, offset);
	else if (typeTag == STRUCT_TAGA)
		result = StructFromXml(valueXml, offset);
	// Watch for empty/blank strings with no <string>tag
	else if (typeTag == VALUE_ETAGA)
	{
		*offset = afterValueOffset;   // back up & try again
		result = StringFromXml(valueXml, offset);
	}

	if (result)  // Skip over the </value> tag
		CXmlUtilA::FindTag(VALUE_ETAGA, valueXml, offset);
	else        // Unrecognized tag after <value>
		*offset = savedOffset;

	return result;
}


// Encode the Value in xml
std::string CXmlValueA::ToXml() const
{
	switch (_type) {
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
	return std::string();   // Invalid value
}

std::string const& CXmlValueA::GetDoubleFormatA() 
{ 
	return _doubleFormat; 
}


void CXmlValueA::SetDoubleFormat(const char* f) 
{
	_doubleFormat = f; 
}

// Boolean
bool CXmlValueA::BoolFromXml(std::string const& valueXml, int* offset)
{
	const char* valueStart = valueXml.c_str() + *offset;
	char* valueEnd;
	long ivalue = strtol(valueStart, &valueEnd, 10);
	if (valueEnd == valueStart || (ivalue != 0 && ivalue != 1))
		return false;

	_type = TypeBoolean;
	_value.asBool = (ivalue == 1);
	*offset += int(valueEnd - valueStart);
	return true;
}


std::string CXmlValueA::BoolToXml() const
{
	std::string xml = VALUE_TAGA;
	xml += BOOLEAN_TAGA;
	xml += (_value.asBool ? "1" : "0");
	xml += BOOLEAN_ETAGA;
	xml += VALUE_ETAGA;
	return xml;
}

// Int
bool CXmlValueA::IntFromXml(std::string const& valueXml, int* offset)
{
	const char* valueStart = valueXml.c_str() + *offset;
	char* valueEnd;
	long ivalue = strtol(valueStart, &valueEnd, 10);
	if (valueEnd == valueStart)
		return false;

	_type = TypeInt;
	_value.asInt = int(ivalue);
	*offset += int(valueEnd - valueStart);
	return true;
}


std::string CXmlValueA::IntToXml() const
{
	char buf[256];
	_snprintf(buf, sizeof(buf)-1, "%d", _value.asInt);
	buf[sizeof(buf)-1] = 0;
	std::string xml = VALUE_TAGA;
	xml += I4_TAGA;
	xml += buf;
	xml += I4_ETAGA;
	xml += VALUE_ETAGA;
	return xml;
}

// Double
bool CXmlValueA::DoubleFromXml(std::string const& valueXml, int* offset)
{
	const char* valueStart = valueXml.c_str() + *offset;
	char* valueEnd;
	double dvalue = strtod(valueStart, &valueEnd);
	if (valueEnd == valueStart)
		return false;

	_type = TypeDouble;
	_value.asDouble = dvalue;
	*offset += int(valueEnd - valueStart);
	return true;
}

std::string CXmlValueA::DoubleToXml() const
{
	char buf[256];
	_snprintf(buf, sizeof(buf)-1, GetDoubleFormatA().c_str(), _value.asDouble);
	buf[sizeof(buf)-1] = 0;

	std::string xml = VALUE_TAGA;
	xml += DOUBLE_TAGA;
	xml += buf;
	xml += DOUBLE_ETAGA;
	xml += VALUE_ETAGA;
	return xml;
}

// String
bool CXmlValueA::StringFromXml(std::string const& valueXml, int* offset)
{
	size_t valueEnd = valueXml.find('<', *offset);
	if (valueEnd == std::string::npos)
		return false;     // No end tag;

	_type = TypeString;
	_value.asString = new std::string(CXmlUtilA::XmlDecode(valueXml.substr(*offset, valueEnd-*offset)));
	*offset += int(_value.asString->length());
	return true;
}

std::string CXmlValueA::StringToXml() const
{
	std::string xml = VALUE_TAGA;
	//xml += STRING_TAG; optional
	xml += CXmlUtilA::XmlEncode(*_value.asString);
	//xml += STRING_ETAG;
	xml += VALUE_ETAGA;
	return xml;
}


// DateTime (stored as a struct tm)
bool CXmlValueA::TimeFromXml(std::string const& valueXml, int* offset)
{
	size_t valueEnd = valueXml.find('<', *offset);
	if (valueEnd == std::string::npos)
		return false;     // No end tag;

	std::string stime = valueXml.substr(*offset, valueEnd-*offset);

	struct tm t;
	if (sscanf(stime.c_str(),"%4d%2d%2dT%2d:%2d:%2d",&t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec) != 6)
		return false;

	t.tm_isdst = -1;
	_type = TypeDateTime;
	_value.asTime = new struct tm(t);
	*offset += int(stime.length());
	return true;
}

std::string CXmlValueA::TimeToXml() const
{
	struct tm* t = _value.asTime;
	char buf[20];
	_snprintf(buf, sizeof(buf)-1, "%4d%02d%02dT%02d:%02d:%02d", 
		t->tm_year,t->tm_mon,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
	buf[sizeof(buf)-1] = 0;

	std::string xml = VALUE_TAGA;
	xml += DATETIME_TAGA;
	xml += buf;
	xml += DATETIME_ETAGA;
	xml += VALUE_ETAGA;
	return xml;
}


// Base64
bool CXmlValueA::BinaryFromXml(std::string const& valueXml, int* offset)
{
	size_t valueEnd = valueXml.find('<', *offset);
	if (valueEnd == std::string::npos)
		return false;     // No end tag;

	_type = TypeBase64;
	std::string asString = valueXml.substr(*offset, valueEnd-*offset);
	_value.asBinary = new BinaryDataA();
	// check whether base64 encodings can contain chars xml encodes...

	// convert from base64 to binary
	int iostatus = 0;
	base64<char> decoder;
	std::back_insert_iterator<BinaryDataA> ins = std::back_inserter(*(_value.asBinary));
	decoder.get(asString.begin(), asString.end(), ins, iostatus);

	*offset += int(asString.length());
	return true;
}


std::string CXmlValueA::BinaryToXml() const
{
	// convert to base64
	std::vector<char> base64data;
	int iostatus = 0;
	base64<char> encoder;
	std::back_insert_iterator<std::vector<char> > ins = std::back_inserter(base64data);
	encoder.put(_value.asBinary->begin(), _value.asBinary->end(), ins, iostatus, base64<>::crlf());

	// Wrap with xml
	std::string xml = VALUE_TAGA;
	xml += BASE64_TAGA;
	xml.append(base64data.begin(), base64data.end());
	xml += BASE64_ETAGA;
	xml += VALUE_ETAGA;
	return xml;
}


// Array
bool CXmlValueA::ArrayFromXml(std::string const& valueXml, int* offset)
{
	if ( !CXmlUtilA::NextTagIs(DATA_TAGA, valueXml, offset))
		return false;

	_type = TypeArray;
	_value.asArray = new ValueArrayA;
	CXmlValueA v;
	while (v.FromXml(valueXml, offset))
		_value.asArray->push_back(v);       // copy...

	// Skip the trailing </data>
	(void) CXmlUtilA::NextTagIs(DATA_ETAGA, valueXml, offset);
	return true;
}


// In general, its preferable to generate the xml of each element of the
// array as it is needed rather than glomming up one big string.
std::string CXmlValueA::ArrayToXml() const
{
	std::string xml = VALUE_TAGA;
	xml += ARRAY_TAGA;
	xml += DATA_TAGA;

	int s = int(_value.asArray->size());
	for (int i=0; i<s; ++i)
		xml += _value.asArray->at(i).ToXml();

	xml += DATA_ETAGA;
	xml += ARRAY_ETAGA;
	xml += VALUE_ETAGA;
	return xml;
}


// Struct
bool CXmlValueA::StructFromXml(std::string const& valueXml, int* offset)
{
	_type = TypeStruct;
	_value.asStruct = new ValueStructA;

	while (CXmlUtilA::NextTagIs(MEMBER_TAGA, valueXml, offset)) {
		// name
		const std::string name = CXmlUtilA::ParseTag(NAME_TAGA, valueXml, offset);
		// value
		CXmlValueA val(valueXml, offset);
		if ( ! val.Valid()) {
			Invalidate();
			return false;
		}
		const std::pair<const std::string, CXmlValueA> p(name, val);
		_value.asStruct->insert(p);

		(void) CXmlUtilA::NextTagIs(MEMBER_ETAGA, valueXml, offset);
	}
	return true;
}


// In general, its preferable to generate the xml of each element
// as it is needed rather than glomming up one big string.
std::string CXmlValueA::StructToXml() const
{
	std::string xml = VALUE_TAGA;
	xml += STRUCT_TAGA;

	ValueStructA::const_iterator it;
	for (it=_value.asStruct->begin(); it!=_value.asStruct->end(); ++it) {
		xml += MEMBER_TAGA;
		xml += NAME_TAGA;
		xml += CXmlUtilA::XmlEncode(it->first);
		xml += NAME_ETAGA;
		xml += it->second.ToXml();
		xml += MEMBER_ETAGA;
	}

	xml += STRUCT_ETAGA;
	xml += VALUE_ETAGA;
	return xml;
}

};