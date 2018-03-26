// Copyright (c) 2008-2009 Michael Kazakov
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
/////////////////////////////////////////////////////////////////////
#pragma once

#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <string>
#include <assert.h>

namespace mluabind
{

namespace impl
{

class SimpleString
{
	enum {BufferSize = 14, MaxLen = 65535};
public:
	inline SimpleString():
		m_Size(0)
	{
		memset(&m_Buffer[0], 0, sizeof(m_Buffer));
	};

	inline SimpleString(const char* _input)
	{
		unsigned short slen = (unsigned short )strlen(_input);
		assert(slen <= MaxLen);
		if(strlen(_input) < BufferSize)
		{
			m_Size = slen;
			memcpy(&m_Buffer[0], _input, slen+1);
		}
		else
		{
			m_Size = slen;
			m_Data = (char*)malloc(slen+1);
			memcpy(m_Data, _input, slen+1);			
		}
	};

	inline SimpleString(const std::string& _input)
	{
		unsigned short slen = (unsigned short )_input.size();
		assert(slen <= MaxLen);
		if(_input.size() < BufferSize)
		{
			m_Size = slen;
			memcpy(&m_Buffer[0], _input.c_str(), slen+1);
		}
		else
		{
			m_Size = slen;
			m_Data = (char*)malloc(slen+1);
			memcpy(m_Data, _input.c_str(), slen+1);
		}
	};

	inline SimpleString(const SimpleString& _str)
	{
		if(_str.m_Size < BufferSize)
		{
			m_Size = _str.m_Size;
			memcpy(&m_Buffer[0], &_str.m_Buffer[0], _str.m_Size+1);
		}
		else
		{
			m_Size = _str.m_Size;
			m_Data = (char*)malloc(_str.m_Size+1);
			memcpy(m_Data, _str.m_Data, _str.m_Size+1);			
		}
	};

	inline ~SimpleString()
	{
		if(m_Size>=BufferSize)
			free(m_Data);
	};

	inline size_t size() const
	{
		return m_Size;
	};

	inline size_t empty() const
	{
		return m_Size == 0;
	};

	inline const char* c_str() const
	{
		if(m_Size < BufferSize)
			return &m_Buffer[0];
		else
			return m_Data;
	};

	inline const SimpleString& operator=(const SimpleString& _right)
	{
		if(m_Size >= BufferSize)
			free(m_Data);
		m_Size = _right.m_Size;
		if(_right.m_Size < BufferSize)
		{
			memcpy(&m_Buffer[0], &_right.m_Buffer[0], m_Size+1);
		}
		else
		{
			m_Data = (char*)malloc(m_Size+1);
			memcpy(m_Data, _right.m_Data, m_Size+1);
		}
		return *this;
	};

	inline const SimpleString& operator=(const char *_right)
	{
		if(m_Size >= BufferSize)
			free(m_Data);
		assert(strlen(_right) <= MaxLen);
		m_Size = (unsigned short)strlen(_right);
		if(m_Size < BufferSize)
		{
			memcpy(&m_Buffer[0], _right, m_Size+1);
		}
		else
		{
			m_Data = (char*)malloc(m_Size+1);
			memcpy(m_Data, _right, m_Size+1);
		}
		return *this;
	};

	inline bool operator ==(const SimpleString& _right) const
	{
		if(m_Size != _right.m_Size)
			return false;

		if(!m_Size)
			return true;

		const char *s1 = m_Size < BufferSize ? &m_Buffer[0] : m_Data;
		const char *s2 = m_Size < BufferSize ? &_right.m_Buffer[0] : _right.m_Data;
		for(size_t i = 0; i < m_Size; ++i)
			if(s1[i] != s2[i])
				return false;

		return true;
	};

	inline bool operator ==(const char* _right) const
	{
		size_t s = strlen(_right);

		if(m_Size != s)
			return false;

		if(!m_Size)
			return true;

		const char *s1 = m_Size < BufferSize ? &m_Buffer[0] : m_Data;
		const char *s2 = _right;
		for(size_t i = 0; i < m_Size; ++i)
			if(s1[i] != s2[i])
				return false;

		return true;
	};

	inline bool operator ==(const std::string& _right) const
	{
		if(m_Size != _right.size())
			return false;

		if(!m_Size)
			return true;

		const char *s1 = m_Size < BufferSize ? &m_Buffer[0] : m_Data;
		const char *s2 = _right.c_str();
		for(size_t i = 0; i < m_Size; ++i)
			if(s1[i] != s2[i])
				return false;

		return true;
	};

	inline bool operator <(const SimpleString& _right) const
	{
		if(m_Size < _right.m_Size)
			return true;
		if(m_Size > _right.m_Size)
			return false;

		const char *s1 = m_Size < BufferSize ? &m_Buffer[0] : m_Data;
		const char *s2 = m_Size < BufferSize ? &_right.m_Buffer[0] : _right.m_Data;

        int ret = 0 ;
        while( ! (ret = *(unsigned char *)s1 - *(unsigned char *)s2) && *s2)
                ++s1, ++s2;

		return ret < 0;
	};

	inline bool operator <(const char* _right) const
	{
		size_t s = strlen(_right);

		if(m_Size < s)
			return true;
		if(m_Size > s)
			return false;

		const char *s1 = m_Size < BufferSize ? &m_Buffer[0] : m_Data;
		const char *s2 = _right;

        int ret = 0 ;
        while( ! (ret = *(unsigned char *)s1 - *(unsigned char *)s2) && *s2)
                ++s1, ++s2;

		return ret < 0;
	};

	inline bool operator <(const std::string& _right) const
	{
		size_t s = _right.size();

		if(m_Size < s)
			return true;
		if(m_Size > s)
			return false;

		const char *s1 = m_Size < BufferSize ? &m_Buffer[0] : m_Data;
		const char *s2 = _right.c_str();

        int ret = 0 ;
        while( ! (ret = *(unsigned char *)s1 - *(unsigned char *)s2) && *s2)
                ++s1, ++s2;

		return ret < 0;
	};

	inline bool operator <=(const SimpleString& _right) const
	{
		if(m_Size > _right.m_Size)
			return false;

		const char *s1 = m_Size < BufferSize ? &m_Buffer[0] : m_Data;
		const char *s2 = m_Size < BufferSize ? &_right.m_Buffer[0] : _right.m_Data;

        int ret = 0 ;
        while( ! (ret = *(unsigned char *)s1 - *(unsigned char *)s2) && *s2)
                ++s1, ++s2;

		return ret <= 0;
	};

	inline bool operator <=(const char* _right) const
	{
		size_t s = strlen(_right);

		if(m_Size > s)
			return false;

		const char *s1 = m_Size < BufferSize ? &m_Buffer[0] : m_Data;
		const char *s2 = _right;

        int ret = 0 ;
        while( ! (ret = *(unsigned char *)s1 - *(unsigned char *)s2) && *s2)
                ++s1, ++s2;

		return ret <= 0;
	};

	inline bool operator <=(const std::string& _right) const
	{
		size_t s = _right.size();

		if(m_Size > s)
			return false;

		const char *s1 = m_Size < BufferSize ? &m_Buffer[0] : m_Data;
		const char *s2 = _right.c_str();

        int ret = 0 ;
        while( ! (ret = *(unsigned char *)s1 - *(unsigned char *)s2) && *s2)
                ++s1, ++s2;

		return ret <= 0;
	};

	inline bool operator >(const SimpleString& _right) const
	{
		return !(*this <= _right);
	};

	inline bool operator >(const char* _right) const
	{
		return !(*this <= _right);
	};

	inline bool operator >(const std::string& _right) const
	{
		return !(*this <= _right);
	};

	inline bool operator >=(const SimpleString& _right) const
	{
		return !(*this < _right);
	};

	inline bool operator >=(const char* _right) const
	{
		return !(*this < _right);
	};

	inline bool operator >=(const std::string& _right) const
	{
		return !(*this < _right);
	};

	inline bool operator !=(const SimpleString& _right) const
	{
		return !(*this == _right);
	};

	inline bool operator !=(const char* _right) const
	{
		return !(*this == _right);
	};

	inline bool operator !=(const std::string& _right) const
	{
		return !(*this == _right);
	};

	inline SimpleString operator+(const std::string& _right) const
	{
		return *this + _right.c_str();
	};

	inline SimpleString operator+(const SimpleString& _right) const
	{
		// very ineffective approach, fixme later
		char *tmp = (char*)malloc(size() + _right.size() + 1);
		memcpy(tmp, c_str(), size());
		memcpy(tmp + size(), _right.c_str(), _right.size() + 1);
		SimpleString ret = tmp;
		free(tmp);
		return ret;
	};

	inline SimpleString operator+(const char*_right) const
	{
		// very ineffective approach, fixme later
		size_t slen = strlen(_right);
		char *tmp = (char*)malloc(size() + slen + 1);
		memcpy(tmp, c_str(), size());
		memcpy(tmp + size(), _right, slen + 1);
		SimpleString ret = tmp;
		free(tmp);
		return ret;
	};

	inline const SimpleString& operator+=(const SimpleString& _right)
	{
		// very ineffective approach, fixme later
		*this = *this + _right;
		return *this;
	};

private:
	union
	{
		char* m_Data;
		char m_Buffer[BufferSize];
	};
	unsigned short m_Size;
};

} // namespace impl

} // namespace mluabind
