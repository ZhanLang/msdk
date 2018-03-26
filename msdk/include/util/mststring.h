#include <stdarg.h>
#ifndef _TSTRING_INCLUDE_H_
#define _TSTRING_INCLUDE_H_


namespace msdk{;

struct StringBaseAlloc
{
	VOID* operator()(size_t size)
	{
		VOID* lpVoid = malloc(size);
		memset(lpVoid, 0 , size);
		return lpVoid;
	}
};

struct StringBaseFree
{
	void operator()(VOID* lpVoid)
	{
		if (lpVoid)
		{
			free(lpVoid);
		}
	}
};

template<typename T, typename AllocMem=StringBaseAlloc, typename FreeMem=StringBaseFree>
class CStringBase
{
public:
	enum{ INVALID_POS = -1};  //错误的长度，位置等
	CStringBase();
	CStringBase(T c);
	CStringBase(const T *chars);
	CStringBase(const CStringBase &s);

	~CStringBase();
	operator const T*() const ;
	T Back() const ;

	T* GetBuffer();
	T* GetBufferSetLength(int minBufLenght);
	void ReleaseBuffer() ;
	void ReleaseBuffer(int newLength);

	CStringBase& operator=(T c);
	CStringBase& operator=(const T *chars);
	CStringBase& operator=(const CStringBase& s);
	CStringBase& operator+=(T c);
	CStringBase& operator+=(const T *s);
	CStringBase& operator+=(const CStringBase &s);

	CStringBase Mid(int startIndex) const;
	CStringBase Mid(int startIndex, int count) const;
	CStringBase Left(int count) const;
	CStringBase Right(int count) const;

	void MakeUpper();
	void MakeLower(); 
	
	int Compare(const CStringBase<T,AllocMem,FreeMem>& s) const;
	int Compare(const T *s) const;
	int CompareNoCase(const CStringBase& s) const;
	int CompareNoCase(const T *s) const;

	int Find(T c) const ;
	int Find(T c, int startIndex) const;
	int Find(const CStringBase &s) const ;
	int Find(const CStringBase &s, int startIndex) const;
	int ReverseFind(T c) const;
	int FindOneOf(const CStringBase &s) const;

	void TrimLeft(T c);
	void TrimLeft();
	void TrimRight();
	void TrimRight(T c);
	void Trim();

	int Insert(int index, const CStringBase &s);
	int Insert(int index, T c);
	int Append(T c);
	int Append(const CStringBase &s);

	int Format(const T* pstrFormat, ...);
	int FormatV(const wchar_t* pszFormat, va_list args );
	int FormatV(const char* pszFormat, va_list args );

	int AppendFormat(const T* pstrFormat, ...);
	void Empty();

	int Length() const;
	bool IsEmpty() const ;

	int Replace(T oldChar, T newChar);
	int Replace(const CStringBase &oldString, const CStringBase &newString);

	int Delete(int index, int count = 1);
	void DeleteBack();
	void TrimLeftWithCharSet(const CStringBase &charSet);
	void TrimRightWithCharSet(const CStringBase &charSet);
private:

	inline void GrowLength(int n);
	inline void CorrectIndex(int &index) const;
	CStringBase GetTrimDefaultCharSet();
	inline T * StringCopy(T *dest, const T *src);
	inline int StringLen(const T *s);
	inline const T* GetNextCharPointer(const T *p)const;
	inline const T* GetPrevCharPointer(const T *, const T *p)const;
	inline void SetCapacity(int newCapacity);
	void MoveItems(int destIndex, int srcIndex);
	void InsertSpace(int &index, int size);
	inline int StringCompare(const char *s1, const char *s2) const;
	inline int StringCompare(const wchar_t *s1, const wchar_t *s2) const;
	inline int StringCompareNoCase(const T *s1, const T *s2) const;
	inline T CharUpper(T c) const ;
	inline T CharLower(T c) const;
private:
	T *_chars;
	int _length;
	int _capacity;
};

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> operator+(const CStringBase<T,AllocMem,FreeMem>& s1, const CStringBase<T,AllocMem,FreeMem>& s2);

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> operator+(const CStringBase<T,AllocMem,FreeMem>& s, T c);

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> operator+(T c, const CStringBase<T,AllocMem,FreeMem>& s);

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> operator+(const CStringBase<T,AllocMem,FreeMem>& s, const T * chars);


template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> operator+(const T * chars, const CStringBase<T,AllocMem,FreeMem>& s);

template<typename T,typename AllocMem,typename FreeMem>
bool operator==(const CStringBase<T,AllocMem,FreeMem>& s1, const CStringBase<T,AllocMem,FreeMem>& s2);

template<typename T,typename AllocMem,typename FreeMem>
bool operator<(const CStringBase<T,AllocMem,FreeMem>& s1, const CStringBase<T,AllocMem,FreeMem>& s2);

template<typename T,typename AllocMem,typename FreeMem>
bool operator==(const T *s1, const CStringBase<T,AllocMem,FreeMem>& s2);

template<typename T,typename AllocMem,typename FreeMem>
bool operator==(const CStringBase<T,AllocMem,FreeMem>& s1, const T *s2);

template<typename T,typename AllocMem,typename FreeMem>
bool operator!=(const CStringBase<T,AllocMem,FreeMem>& s1, const CStringBase<T,AllocMem,FreeMem>& s2);

template<typename T,typename AllocMem,typename FreeMem>
bool operator!=(const T *s1, const CStringBase<T,AllocMem,FreeMem>& s2);

template<typename T,typename AllocMem,typename FreeMem>
bool operator!=(const CStringBase<T,AllocMem,FreeMem>& s1, const T *s2);

typedef CStringBase<char,StringBaseAlloc,StringBaseFree>   CApiStringA;
typedef CStringBase<wchar_t,StringBaseAlloc,StringBaseFree>CApiStringW;

#ifdef UNICODE
typedef CApiStringW CApiString;
#else
typedef CApiStringA CApiString;
#endif


static CApiStringW MultiByteToUnicodeString(const CApiStringA &srcString, unsigned int codePage = CP_ACP);
static CApiStringA UnicodeStringToMultiByte(const CApiStringW &srcString, unsigned int codePage, char defaultChar, bool &defaultCharWasUsed);
static CApiStringA UnicodeStringToMultiByte(const CApiStringW &srcString, UINT codePage = CP_ACP);



/*-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_*/
template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem>::CStringBase() : _chars(0), _length(0) , _capacity(0) 
{ 
		SetCapacity(3); 
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem>::CStringBase(T c):  _chars(0), _length(0), _capacity(0)
{
	SetCapacity(1);
	_chars[0] = c;
	_chars[1] = 0;
	_length = 1;
}


template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem>::CStringBase(const T *chars): _chars(0), _length(0), _capacity(0)
{
	if (chars)
	{
		int length = StringLen(chars);
		SetCapacity(length);
		StringCopy(_chars, chars);
		_length = length;
	}
}


template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem>::CStringBase(const CStringBase &s):  _chars(0), _length(0), _capacity(0)
{
	SetCapacity(s._length);
	StringCopy(_chars, s._chars);
	_length = s._length;
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem>::~CStringBase() 
{  
	delete []_chars;
	_chars = NULL;
}


template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem>::operator const T*() const 
{ 
	return _chars;
}

template<typename T,typename AllocMem,typename FreeMem>
T CStringBase<T,AllocMem,FreeMem>::Back() const 
{ 
	return _chars[_length - 1]; 
}

template<typename T,typename AllocMem,typename FreeMem>
T* CStringBase<T,AllocMem,FreeMem>::GetBuffer()
{
	return _chars;
}

template<typename T,typename AllocMem,typename FreeMem>
T* CStringBase<T,AllocMem,FreeMem>::GetBufferSetLength(int minBufLenght)
{
	if (minBufLenght >= _capacity)
		SetCapacity(minBufLenght);

	return _chars;
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::ReleaseBuffer() 
{ 
	ReleaseBuffer(StringLen(_chars));
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::ReleaseBuffer(int newLength)
{
	_chars[newLength] = 0;
	_length = newLength;
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem>& CStringBase<T,AllocMem,FreeMem>::operator=(T c)
{
	Empty();
	SetCapacity(1);
	_chars[0] = c;
	_chars[1] = 0;
	_length = 1;
	return *this;
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem>& CStringBase<T,AllocMem,FreeMem>::operator=(const T *chars)
{
	Empty();
	int length = StringLen(chars);
	SetCapacity(length);
	StringCopy(_chars, chars);
	_length = length;
	return *this;
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem>& CStringBase<T,AllocMem,FreeMem>::operator=(const CStringBase<T,AllocMem,FreeMem>& s)
{
	if (&s == this)
		return *this;
	Empty();
	SetCapacity(s._length);
	StringCopy(_chars, s._chars);
	_length = s._length;
	return *this;
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem>& CStringBase<T,AllocMem,FreeMem>::operator+=(T c)
{
	GrowLength(1);
	_chars[_length] = c;
	_chars[++_length] = 0;
	return *this;
}


template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem>& CStringBase<T,AllocMem,FreeMem>::operator+=(const T *s)
{
	int len = StringLen(s);
	GrowLength(len);
	StringCopy(_chars + _length, s);
	_length += len;
	return *this;
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem>& CStringBase<T,AllocMem,FreeMem>::operator+=(const CStringBase<T,AllocMem,FreeMem> &s)
{
	GrowLength(s._length);
	StringCopy(_chars + _length, s._chars);
	_length += s._length;
	return *this;
}


template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> CStringBase<T,AllocMem,FreeMem>::Mid(int startIndex) const
{ 
	return Mid(startIndex, _length - startIndex); 
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> CStringBase<T,AllocMem,FreeMem>::Mid(int startIndex, int count) const
{
	if (startIndex + count > _length)
		count = _length - startIndex;

	if (count < 1) return (T*)(NULL);

	if (startIndex == 0 && startIndex + count == _length)
		return *this;

	CStringBase<T,AllocMem,FreeMem> result;
	result.SetCapacity(count);

	for (int i = 0; i < count; i++)
		result._chars[i] = _chars[startIndex + i];

	result._chars[count] = 0;
	result._length = count;
	return result;
}


template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> CStringBase<T,AllocMem,FreeMem>::Left(int count) const
{ 
	return Mid(0, count); 
}


template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> CStringBase<T,AllocMem,FreeMem>::Right(int count) const
{
	if (count > _length)
		count = _length;
	return Mid(_length - count, count);
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::MakeUpper() 
{ 
	for (int i = 0; i <= _length; i++)
		_chars[i] = CharUpper(_chars[i]);
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::MakeLower() 
{ 
	for (int i = 0; i <= _length; i++)
		_chars[i] = CharLower(_chars[i]);
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Compare(const CStringBase& s) const
{ 
	return StringCompare(_chars, s._chars); 
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Compare(const T *s) const
{ 
	return StringCompare(_chars, s); 
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::CompareNoCase(const CStringBase& s) const
{ 
	return StringCompareNoCase(_chars, s._chars); 
}


template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::CompareNoCase(const T *s) const
{ 
	return StringCompareNoCase(_chars, s); 
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Find(T c) const 
{ 
	return Find(c, 0); 
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Find(T c, int startIndex) const
{
	const T *p = _chars + startIndex;
	for (;;)
	{
		if (*p == c)
			return (int)(p - _chars);

		if (*p == 0)
			return -1;

		p = GetNextCharPointer(p);
	}
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Find(const CStringBase<T,AllocMem,FreeMem> &s) const 
{ 
	return Find(s, 0); 
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Find(const CStringBase<T,AllocMem,FreeMem> &s, int startIndex) const
{
	if (s.IsEmpty())
		return startIndex;
	for (; startIndex < _length; startIndex++)
	{
		int j;
		for (j = 0; j < s._length && startIndex + j < _length; j++)
			if (_chars[startIndex+j] != s._chars[j])
				break;
		if (j == s._length)
			return startIndex;
	}
	return -1;
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::ReverseFind(T c) const
{
	if (_length == 0)
		return -1;

	const T *p = _chars + _length - 1;
	for (;;)
	{
		if (*p == c)
			return (int)(p - _chars);
		if (p == _chars)
			return -1;
		p = GetPrevCharPointer(_chars, p);
	}
}


template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::FindOneOf(const CStringBase<T,AllocMem,FreeMem> &s) const
{
	for (int i = 0; i < _length; i++)
		if (s.Find(_chars[i]) >= 0)
			return i;
	return -1;
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::TrimLeft(T c)
{
	const T *p = _chars;
	while (c == *p)
		p = GetNextCharPointer(p);
	Delete(0, p - _chars);
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::TrimLeft()
{
	TrimLeftWithCharSet(GetTrimDefaultCharSet());
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::TrimRight()
{
	TrimRightWithCharSet(GetTrimDefaultCharSet());
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::TrimRight(T c)
{
	const T *p = _chars;
	const T *pLast = NULL;
	while (*p != 0)
	{
		if (*p == c)
		{
			if (pLast == NULL)
				pLast = p;
		}
		else
			pLast = NULL;
		p = GetNextCharPointer(p);
	}

	if (pLast != NULL)
	{
		int i = pLast - _chars;
		Delete(i, _length - i);
	}
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::Trim()
{
	TrimRight();
	TrimLeft();
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Insert(int index, const CStringBase<T,AllocMem,FreeMem> &s)
{
	CorrectIndex(index);
	if (s.IsEmpty())
		return _length;
	int numInsertChars = s.Length();
	InsertSpace(index, numInsertChars);
	for (int i = 0; i < numInsertChars; i++)
		_chars[index + i] = s[i];
	_length += numInsertChars;
	return _length;
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Insert(int index, T c)
{
	InsertSpace(index, 1);
	_chars[index] = c;
	_length++;
	return _length;
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Append(T c)
{
	return Insert(Length(),c);
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Append(const CStringBase<T,AllocMem,FreeMem> &s)
{
	return Insert(Length(),s);
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Format(const T* pstrFormat, ...)
{
	va_list argList;
	va_start(argList, pstrFormat);
	int iRet = FormatV(pstrFormat,argList); 
	va_end(argList);
	return iRet;
}

#pragma warning(disable:4996)
template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::FormatV(const char* pszFormat, va_list args )
{
	T* szSprintf = NULL;
	int nLen = 0;
	nLen = ::_vsnprintf(NULL, 0, pszFormat, args);
	szSprintf = (char*) AllocMem()((nLen + 1)*sizeof(char));
	int iRet = ::_vsnprintf(szSprintf, nLen + 1, pszFormat, args);
	Empty();
	Insert(0,szSprintf);
	FreeMem()(szSprintf);
	return iRet;
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::FormatV(const wchar_t * pszFormat, va_list args )
{
	T* szSprintf = NULL;
	int nLen = 0;
	nLen = ::_vsnwprintf(NULL, 0, pszFormat, args);
	szSprintf = (wchar_t*) AllocMem()((nLen + 1)*sizeof(wchar_t));
	memset(szSprintf, 0, (nLen + 1) * sizeof(wchar_t));
	int iRet = ::_vsnwprintf(szSprintf, nLen + 1, pszFormat, args);
	Empty();
	Insert(0,szSprintf);
	FreeMem()(szSprintf);
	return iRet;
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::AppendFormat(const T* pstrFormat, ...)
{
	CStringBase<T,AllocMem,FreeMem> s;
	va_list argList;
	va_start(argList, pstrFormat);
	int iRet = s.FormatV(pstrFormat,argList);
	Insert(Length(),s);
	va_end(argList);
	return iRet;
}


template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::Empty()
{
	_length = 0;
	if (_chars) _chars[0] = 0;
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Length() const 
{ 
	return _length; 
}

template<typename T,typename AllocMem,typename FreeMem>
bool CStringBase<T,AllocMem,FreeMem>::IsEmpty() const 
{ 
	return (_length == 0); 
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Replace(T oldChar, T newChar)
{
	if (oldChar == newChar)
		return 0;
	int number  = 0;
	int pos  = 0;
	while (pos < Length())
	{
		pos = Find(oldChar, pos);
		if (pos < 0)
			break;
		_chars[pos] = newChar;
		pos++;
		number++;
	}

	return number;
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Replace(const CStringBase<T,AllocMem,FreeMem> &oldString, const CStringBase<T,AllocMem,FreeMem> &newString)
{
	if (oldString.IsEmpty())
		return 0;
	if (oldString == newString)
		return 0;
	int oldStringLength = oldString.Length();
	int newStringLength = newString.Length();
	int number  = 0;
	int pos  = 0;
	while (pos < _length)
	{
		pos = Find(oldString, pos);
		if (pos < 0)
			break;
		Delete(pos, oldStringLength);
		Insert(pos, newString);
		pos += newStringLength;
		number++;
	}
	return number;
}

template<typename T,typename AllocMem,typename FreeMem>
int CStringBase<T,AllocMem,FreeMem>::Delete(int index, int count = 1)
{
	if (index + count > _length)
		count = _length - index;
	if (count > 0)
	{
		MoveItems(index, index + count);
		_length -= count;
	}
	return _length;
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::DeleteBack() 
{ 
	Delete(_length - 1); 
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::TrimLeftWithCharSet(const CStringBase &charSet)
{
	const T *p = _chars;
	while (charSet.Find(*p) >= 0 && (*p != 0))
		p = GetNextCharPointer(p);
	Delete(0, (int)(p - _chars));
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::TrimRightWithCharSet(const CStringBase &charSet)
{
	const T *p = _chars;
	const T *pLast = NULL;
	while (*p != 0)
	{
		if (charSet.Find(*p) >= 0)
		{
			if (pLast == NULL)
				pLast = p;
		}
		else
			pLast = NULL;
		p = GetNextCharPointer(p);
	}
	if (pLast != NULL)
	{
		int i = (int)(pLast - _chars);
		Delete(i, _length - i);
	}
}


template<typename T,typename AllocMem,typename FreeMem>
inline void CStringBase<T,AllocMem,FreeMem>::GrowLength(int n)
{
	int freeSize = _capacity - _length - 1;
	if (n <= freeSize)
		return;

	int delta = 0;
	if (_capacity > 64)
		delta = _capacity / 2;
	else if (_capacity > 8)
		delta = 16;
	else
		delta = 4;

	if (freeSize + delta < n)
		delta = n - freeSize;

	SetCapacity(_capacity + delta);
}

template<typename T,typename AllocMem,typename FreeMem>
inline void CStringBase<T,AllocMem,FreeMem>::CorrectIndex(int &index) const
{
	if(index > _length)
		index = _length;
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> CStringBase<T,AllocMem,FreeMem>::GetTrimDefaultCharSet()
{
	CStringBase<T,AllocMem,FreeMem> charSet;
	charSet += (T)' ';
	charSet += (T)'\n';
	charSet += (T)'\t';
	return charSet;
}

template<typename T,typename AllocMem,typename FreeMem>
inline T * CStringBase<T,AllocMem,FreeMem>::StringCopy(T *dest, const T *src)
{
	if (dest && src)
	{
		T *destStart = dest;
		while ((*dest++ = *src++) != 0);
		return destStart;
	}
	
	return NULL;
}

template<typename T,typename AllocMem,typename FreeMem>
inline int CStringBase<T,AllocMem,FreeMem>::StringLen(const T *s)
{
	if(!s) return -1;

	int i;
	for (i = 0; s[i] != '\0'; i++);
	return i;
}

template<typename T,typename AllocMem,typename FreeMem>
inline const T* CStringBase<T,AllocMem,FreeMem>::GetNextCharPointer(const T *p)const
{ 
	return (p + 1); 
}

template<typename T,typename AllocMem,typename FreeMem>
inline const T* CStringBase<T,AllocMem,FreeMem>::GetPrevCharPointer(const T *, const T *p)const
{ 
	return (p - 1); 
}

template<typename T,typename AllocMem,typename FreeMem>
inline void CStringBase<T,AllocMem,FreeMem>::SetCapacity(int newCapacity)
{
	int realCapacity = newCapacity + 1;
	if (realCapacity == _capacity)
		return;

	T *newBuffer = (T *)AllocMem()(realCapacity*sizeof(T));
	
	if (_capacity > 0)
	{
		for (int i = 0; i < _length; i++)
			newBuffer[i] = _chars[i];
		FreeMem()(_chars);
	}

	_chars = newBuffer;
	_chars[_length] = 0;
	_capacity = realCapacity;
}


template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::MoveItems(int destIndex, int srcIndex)
{
	memmove(_chars + destIndex, _chars + srcIndex,sizeof(T) * (_length - srcIndex + 1));
}

template<typename T,typename AllocMem,typename FreeMem>
void CStringBase<T,AllocMem,FreeMem>::InsertSpace(int &index, int size)
{
	CorrectIndex(index);
	GrowLength(size);
	MoveItems(index + size, index);
}

template<typename T,typename AllocMem,typename FreeMem>
inline int CStringBase<T,AllocMem,FreeMem>::StringCompare(const char *s1, const char *s2) const
{
	for (;;)
	{
		unsigned char c1 = (unsigned char)*s1++;
		unsigned char c2 = (unsigned char)*s2++;
		if (c1 < c2) return -1;
		if (c1 > c2) return 1;
		if (c1 == 0) return 0;
	}
}

template<typename T,typename AllocMem,typename FreeMem>
inline int CStringBase<T,AllocMem,FreeMem>::StringCompare(const wchar_t *s1, const wchar_t *s2) const
{	
	if (s1 && s2)
	{
		for (;;)
		{
			wchar_t c1 = *s1++;
			wchar_t c2 = *s2++;
			if (c1 < c2) return -1;
			if (c1 > c2) return 1;
			if (c1 == 0) return 0;
		}
	}
	
	return -1;
}

template<typename T,typename AllocMem,typename FreeMem>
inline T CStringBase<T,AllocMem,FreeMem>::CharUpper(T c) const 
{
	if (c>='a'&&c<='z'){ 
		c = (c - 32);
	}
	return c;
}

template<typename T,typename AllocMem,typename FreeMem>
inline T CStringBase<T,AllocMem,FreeMem>::CharLower(T c) const 
{
	if (c>='A'&&c<='Z'){ 
		c = (c + 32);
	}
	return c;
}

template<typename T,typename AllocMem,typename FreeMem>
inline int CStringBase<T,AllocMem,FreeMem>::StringCompareNoCase(const T *s1, const T *s2) const
{
	if (s1 && s2){
		for (;;){
			T c1 = *s1++;T c2 = *s2++;
			if (c1 != c2){
				T u1 = CharUpper(c1);
				T u2 = CharUpper(c2);
				if (u1 < u2) return -1;
				if (u1 > u2) return 1;
			}
			if (c1 == 0) return 0;
		}
	}

	return -1;
}


template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> operator+(const CStringBase<T,AllocMem,FreeMem>& s1, const CStringBase<T,AllocMem,FreeMem>& s2)
{
	CStringBase<T,AllocMem,FreeMem> result(s1);
	result += s2;
	return result;
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> operator+(const CStringBase<T,AllocMem,FreeMem>& s, T c)
{
	CStringBase<T,AllocMem,FreeMem> result(s);
	result += c;
	return result;
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> operator+(T c, const CStringBase<T,AllocMem,FreeMem>& s)
{
	CStringBase<T,AllocMem,FreeMem> result(c);
	result += s;
	return result;
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> operator+(const CStringBase<T,AllocMem,FreeMem>& s, const T * chars)
{
	CStringBase<T,AllocMem,FreeMem> result(s);
	result += chars;
	return result;
}

template<typename T,typename AllocMem,typename FreeMem>
CStringBase<T,AllocMem,FreeMem> operator+(const T * chars, const CStringBase<T,AllocMem,FreeMem>& s)
{
	CStringBase<T,AllocMem,FreeMem> result(chars);
	result += s;
	return result;
}

template<typename T,typename AllocMem,typename FreeMem>
bool operator==(const CStringBase<T,AllocMem,FreeMem>& s1, const CStringBase<T,AllocMem,FreeMem>& s2)
{ 
	return (s1.Compare(s2) == 0); 
}

template<typename T,typename AllocMem,typename FreeMem>
bool operator<(const CStringBase<T,AllocMem,FreeMem>& s1, const CStringBase<T,AllocMem,FreeMem>& s2)
{ 
	return (s1.Compare(s2) < 0); 
}

template<typename T,typename AllocMem,typename FreeMem>
bool operator==(const T *s1, const CStringBase<T,AllocMem,FreeMem>& s2)
{ 
	return (s2.Compare(s1) == 0); 
}

template<typename T,typename AllocMem,typename FreeMem>
bool operator==(const CStringBase<T,AllocMem,FreeMem>& s1, const T *s2)
{
	return (s1.Compare(s2) == 0); 
}

template<typename T,typename AllocMem,typename FreeMem>
bool operator!=(const CStringBase<T,AllocMem,FreeMem>& s1, const CStringBase<T,AllocMem,FreeMem>& s2)
{ 
	return (s1.Compare(s2) != 0);
}

template<typename T,typename AllocMem,typename FreeMem>
bool operator!=(const T *s1, const CStringBase<T,AllocMem,FreeMem>& s2)
{ 
	return (s2.Compare(s1) != 0);
}

template<typename T,typename AllocMem,typename FreeMem>
bool operator!=(const CStringBase<T,AllocMem,FreeMem>& s1, const T *s2)
{ 
	return (s1.Compare(s2) != 0);
}

static CApiStringW MultiByteToUnicodeString(const CApiStringA &srcString, unsigned int codePage)
{
	CApiStringW resultString;
	if (!srcString.IsEmpty())
	{
		int numChars = MultiByteToWideChar(codePage, 0, srcString,
			srcString.Length(), resultString.GetBufferSetLength(srcString.Length()),
			srcString.Length() + 1);
		
		resultString.ReleaseBuffer(numChars);
	}

	return resultString;
}


static CApiStringA UnicodeStringToMultiByte(const CApiStringW &srcString, unsigned int codePage, char defaultChar, bool &defaultCharWasUsed)
{
	CApiStringA dest;
	defaultCharWasUsed = false;
	if (!srcString.IsEmpty())
	{
		int numRequiredBytes = srcString.Length() * 2;
		BOOL defUsed;
		int numChars = WideCharToMultiByte(codePage, 0, srcString, srcString.Length(),
			dest.GetBufferSetLength(numRequiredBytes), numRequiredBytes + 1,
			&defaultChar, &defUsed);
		defaultCharWasUsed = (defUsed != FALSE);
		dest.ReleaseBuffer(numChars);
	}

	return dest;
}

static CApiStringA UnicodeStringToMultiByte(const CApiStringW &srcString, UINT codePage )
{
	bool defaultCharWasUsed;
	return UnicodeStringToMultiByte(srcString, codePage, '_', defaultCharWasUsed);
}


}//namespace msdk

#endif // _TSTRING_INCLUDE_H_