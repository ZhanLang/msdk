#ifndef _tstring_include_h_
#define _tstring_include_h_

#include <string>
#include <tchar.h>
namespace msdk {;
namespace strext {;

// Function name	: trim
// Description	    : Trm function
// Return type		: basic_string< E >
// Argument         : const E* s
template< class E >
std::basic_string< E > trim( const E* s ) 
{
	if( s == NULL )
		return std::basic_string<E>();

	const E* i = s;
	while( ( *i != '\0' ) && ( *i == ' ' ) )
		i++;

	if( *i == '\0' )
		return std::basic_string< E >();

	const E* j = i;

	while( *( j + 1 ) )
		j++;

	while( ( j != i ) && ( *j == ' ' ) )
		j--;

	return std::basic_string< E >( i, j - i + 1 );
}

// Function name	: remove_non_lws
// Description	    : Removes non linear white spaces
// Return type		: basic_string< E >
// Argument         : const E* s
template< class E >
std::basic_string< E > remove_non_lws( const E* s ) 
{
	std::basic_string< E > strResult = s;

	std::basic_string< E >::size_type i = basic_string< E >::npos;

	// Remove \r
	while( ( i = strResult.find( '\r' ) ) != basic_string< E >::npos )
		strResult.erase( i, 1 );

	// Remove \n
	while( ( i = strResult.find( '\n' ) ) != basic_string< E >::npos )
		strResult.erase( i, 1 );

	// Remove \r
	while( ( i = strResult.find( '\r' ) ) != basic_string< E >::npos )
		strResult.erase( i, 1 );

	return strResult;
}

template< class E >
std::basic_string< E >& strupr(std::basic_string< E >& str) 
{
	int idx = 0;
	while (idx != str.size()) {
		if ((str[idx] <= 'z') && (str[idx] >= 'a'))
			str[idx] = (str[idx] + ('A' - 'a'));
		idx++;
	}
	return str;
}

template< class T >
std::basic_string< T > strlwr(const T* _str) 
{
	std::basic_string<T> str = _str;
	int idx = 0;
	while (idx != str.size()) {
		if ((str[idx] <= 'Z') && (str[idx] >= 'A'))
			str[idx] = (str[idx] - ('A' - 'a'));
		idx++;
	}
	return str;
}


template< class E >
std::basic_string<E> replace(const std::basic_string<E>& str, E _o, E _n) 
{
	std::basic_string<E> strResult = str;
	std::basic_string<E>::size_type i = std::basic_string<E>::npos;

	while( ( i = strResult.find(_o) ) != std::basic_string<E>::npos )
		strResult[i] = _n;

	return strResult;
}

template< class T >
std::basic_string<T> replace(const T* _str, const const T* _old_value, const T* new_value) 
{
	std::basic_string<T> str = _str;
	std::basic_string<T> old_value = _old_value;
	while(true)   
	{
		std::basic_string<T>::size_type pos(0);
		if( (pos=str.find(old_value))!=std::basic_string<T>::npos )
			str.replace(pos,old_value.length(),new_value);
		else   break;
	}
	return   str;
}


template< class E >
std::basic_string<E> replace_distinct(std::basic_string<E>& str, const std::basic_string<E>& old_value, const std::basic_string<E>& new_value) 
{
	for(std::basic_string<E>::size_type pos(0); pos!=std::basic_string<E>::npos; pos += new_value.length() ) 
	{
		if( ( pos=str.find(old_value,pos))!=std::basic_string<E>::npos )
			str.replace( pos,old_value.length(),new_value );
		else   break;
	}
	return   str;

} 



//×Ö·û´®·Ö¸î
template<class T>
std::vector<std::basic_string<T>> split(const T* _str, const T* _delimiters)  
{  
	const std::basic_string<T> str = _str;
	const std::basic_string<T> delimiters = _delimiters;
	std::vector<std::basic_string<T>> tokens;
	std::basic_string<T>::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::basic_string<T>::size_type pos     = str.find_first_of(delimiters, lastPos);
	while (std::basic_string<T>::npos != pos || std::basic_string<T>::npos != lastPos)
	{  
		tokens.push_back(str.substr(lastPos, pos - lastPos));  
		lastPos = str.find_first_not_of(delimiters, pos);  
		pos = str.find_first_of(delimiters, lastPos);  
	} 

	return tokens;
} ; 

//×Ö·û´®·Ö¸î
template<class T>
int compare(const T* _str, const T* _delimiters)  
{  
	const std::basic_string<T> str = _str;
	const std::basic_string<T> delimiters = _delimiters;

	return str.compare(delimiters);
} ; 

template<class T>
int compare_no_case(const T* _str, const T* _delimiters)  
{  
		std::basic_string<T> str = strlwr(_str);
		std::basic_string<T> delimiters = strlwr(_delimiters);

	return str.compare(delimiters);
} ; 



static std::wstring format(const wchar_t * _Format, ...){  
	va_list marker = NULL;
	va_start(marker, _Format);

	std::wstring _str;
	int num_of_chars = _vscwprintf(_Format, marker);

	if (num_of_chars > _str.capacity())
		_str.resize(num_of_chars + 1);

	vswprintf ((wchar_t *) _str.c_str(), _Format, marker);
	va_end(marker);

	return _str.c_str();
}  

static std::string format(const char * _Format, ...){  
	va_list marker = NULL;
	va_start(marker, _Format);

	std::string _str;
	int num_of_chars = _vscprintf(_Format, marker);

	if (num_of_chars > _str.capacity())
		_str.resize(num_of_chars + 1);

	vsprintf ((char *) _str.c_str(), _Format, marker);
	va_end(marker);
	return _str.c_str();  
} 

};// Namespace extension
}; //namespace msdk

#endif // _tstring_include_h_