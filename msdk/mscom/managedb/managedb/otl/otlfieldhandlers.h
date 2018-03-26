/********************************************************************
	created:	2009/12/19
	created:	19:12:2009   15:02
	filename: 	d:\C++Work\approduct2010\utm\src\unitive\managedb\otlfieldhandlers.h
	file path:	d:\C++Work\approduct2010\utm\src\unitive\managedb
	file base:	otlfieldhandlers
	file ext:	h
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#pragma once

#include "otlinc.h"
#include <rscom/prophelpers.h>
#include <iostream>
#include <vector>

using namespace std;

enum otl_handler_type
{
	otl_handler_numeric,
	otl_handler_blob,
	otl_handler_char,
	otl_handler_binary,
	otl_handler_varchar_long,
	otl_handler_datetime
};

class IOtlFieldHandler
{
public:
	IOtlFieldHandler( otl_stream* pStream )
		:m_pStream(pStream)
	{
	}

	virtual ~IOtlFieldHandler()
	{
	}

	virtual BOOL PrepareColumn( otl_column_desc* pColumn )
	{
		return TRUE;
	}

	virtual BOOL PrepareIO( BOOL bStreamMode )
	{
		return TRUE;
	}

	virtual VOID GetField( CPropVar* prop, int otlType ) = 0;

protected:
	otl_stream* m_pStream;
};

template <DWORD hander_type>
class concrete_handler : public IOtlFieldHandler
{
public:
	concrete_handler( otl_stream* pStream )
		: IOtlFieldHandler(pStream) {}

	virtual BOOL PrepareColumn( otl_column_desc* pColumn, BOOL& bStreamMode )
	{
		bStreamMode = FALSE;
		return FALSE;
	}

	virtual BOOL PrepareIO( BOOL bStreamMode )
	{
		return FALSE;
	}

	virtual VOID GetField( CPropVar* prop, int otlType )
	{
		*prop = CPropVar::GetNull();
	}
};

#define  OTL_NUM_CASE( otlType, dataType )\
case otlType:\
	{\
		dataType num;\
		(*m_pStream) >> num;\
		*prop = m_pStream->is_null() ? CPropVar::GetNull() : num;\
	}\
	break;

#define  OTL_VT_CASE( vt, dataType )\
case vt:\
	{\
		dataType num = *prop;\
		stream << num;\
	}\
	break;


template <>
class concrete_handler<otl_handler_numeric> : public IOtlFieldHandler
{
public:
	concrete_handler( otl_stream* pStream )
		:IOtlFieldHandler(pStream)
	{}
public:
	virtual VOID GetField( CPropVar* prop, int otlType )
	{
		switch( otlType )
		{
		OTL_NUM_CASE( otl_var_bigint, OTL_BIGINT )
		OTL_NUM_CASE( otl_var_float, float )
		OTL_NUM_CASE( otl_var_int, int )
		OTL_NUM_CASE( otl_var_long_int, long int )
		OTL_NUM_CASE( otl_var_short, short )
		OTL_NUM_CASE( otl_var_unsigned_int, unsigned int )
		}
	}

	static VOID PutVar( otl_stream& stream, CPropVar* prop )
	{
		switch( prop->vt )
		{
		OTL_VT_CASE( VT_INT, int )
		OTL_VT_CASE( VT_UINT, unsigned int )
		OTL_VT_CASE( VT_I4, long )
		OTL_VT_CASE( VT_UI4, long )
		OTL_VT_CASE( VT_I8, OTL_BIGINT )
		}
	}
};

template <>
class concrete_handler<otl_handler_blob>: public IOtlFieldHandler
{
public:
	concrete_handler( otl_stream* pStream )
		:IOtlFieldHandler(pStream)
	{
		m_pLongStr = NULL;
	}

	virtual ~concrete_handler()
	{
		delete m_pLongStr;
	}

	virtual BOOL PrepareIO( BOOL bStreamMode )	
	{
		m_bStreamMode = bStreamMode;
		m_pLongStr = new otl_long_string(LOB_CHUNK_SIZE);

		return TRUE;
	}

	virtual VOID GetField( CPropVar* prop, int otlType )
	{
		if( m_bStreamMode )
		{
			m_lobBuff.clear();
			(*m_pStream) >> m_lobStream;

			if( m_pStream->is_null() )
			{
				*prop = CPropVar::GetNull();
				return;
			}

			while( m_lobStream.eof() )
			{
				m_lobStream >> (*m_pLongStr);

				unsigned char* pStart = &(*m_pLongStr)[0];
				m_lobBuff.insert( m_lobBuff.end(), pStart, pStart+m_pLongStr->len() );
			}

			m_lobStream.close();

			prop->vt = VT_BUFFER;
			prop->SetVector( &m_lobBuff.front(), (int)m_lobBuff.size() );
		}
		else
		{
			(*m_pStream) >> (*m_pLongStr);

			if( m_pStream->is_null() )
			{
				*prop = CPropVar::GetNull();
				return;
			}

			prop->SetVector( (LPVOID)&( (*m_pLongStr)[0] ), m_pLongStr->len() );
		}
	}

	static VOID PutVar( otl_stream& stream, CPropVar* prop )
	{
		LPBYTE pBuffer;
		INT dwLen;
		prop->GetVector( (VOID**)&pBuffer, (INT)dwLen );

		otl_lob_stream lobStream;
		stream << lobStream;

		DWORD dwIndex = 0;

		while( dwIndex < (DWORD)dwLen )
		{
			DWORD dwRest = dwLen - dwIndex;
			DWORD dwChunkSize = (dwRest > LOB_CHUNK_SIZE) ? LOB_CHUNK_SIZE : dwRest;

			// No copy here
			otl_long_string longStr( pBuffer+dwIndex, dwChunkSize, dwChunkSize );
			lobStream << longStr;

			dwIndex += dwChunkSize;
		}

		lobStream.close();
	}

private:
	BOOL				m_bStreamMode;
	otl_lob_stream		m_lobStream;
	otl_long_string*	m_pLongStr;
	std::vector<unsigned char>	m_lobBuff;
};

template <>
class concrete_handler<otl_handler_varchar_long>: public IOtlFieldHandler
{
public:
	concrete_handler( otl_stream* pStream )
		:IOtlFieldHandler(pStream)
	{
		m_pLongStr = NULL;
	}

	virtual ~concrete_handler()
	{
		delete m_pLongStr;
	}

	virtual BOOL PrepareIO( BOOL bStreamMode )	
	{
#ifdef _UNICODE
		m_pLongStr = new otl_long_unicode_string(LOB_CHUNK_SIZE/sizeof(TCHAR));
#else
		m_pLongStr = new otl_long_string(LOB_CHUNK_SIZE/sizeof(TCHAR));
#endif

		m_bStreamMode = bStreamMode;
		return TRUE;
	}

	virtual VOID GetField( CPropVar* prop, int otlType )
	{
		m_lobBuff.clear();

		if(m_bStreamMode)
		{
			(*m_pStream) >> m_lobStream;

			if( m_pStream->is_null() )
				*prop = CPropVar::GetNull();

			while( !m_lobStream.eof() )
			{
				m_lobStream >> (*m_pLongStr);

				OTL_CHAR* pStart = &(*m_pLongStr)[0];
				m_lobBuff.insert( m_lobBuff.end(), pStart, pStart+m_pLongStr->len() );
			}

			m_lobBuff.push_back( 0 );
			m_lobStream.close();

			*prop = (LPTSTR)&m_lobBuff.front();
		}
		else
		{
			(*m_pStream) >> (*m_pLongStr);

			if( m_pStream->is_null() )
			{
				*prop = CPropVar::GetNull();
				return;
			}

			*prop = (LPTSTR)&(*m_pLongStr)[0];
		}
	}

	static VOID PutVar( otl_stream& stream, CPropVar* prop )
	{
		LPCTSTR szValue = *prop;
		DWORD dwLen = (DWORD)_tcslen( szValue );

		otl_lob_stream lobStream;
		stream << lobStream; // Initialize lob stream by writing it into otl_stream
		lobStream.set_len( dwLen );

		DWORD lobLen = (dwLen > LOB_CHAR_COUNT) ? LOB_CHAR_COUNT : dwLen;

#ifdef _UNICODE
		otl_long_unicode_string longStr( lobLen+100 );
#else
		otl_long_string			longStr( lobLen+100 );
#endif

		DWORD dwIndex = 0;

		while( dwIndex < dwLen )
		{
			DWORD dwRest = dwLen - dwIndex;
			DWORD dwChunkSize = (dwRest > LOB_CHAR_COUNT) ? LOB_CHAR_COUNT : dwRest;

			for( DWORD i=0; i<dwChunkSize; i++ )
			{
				longStr[i] = (unsigned short)szValue[dwIndex+i];
			}
			longStr.set_len( dwChunkSize );

			lobStream << longStr;
			dwIndex += dwChunkSize;
		}

		lobStream.close();
	}

private:
	BOOL	m_bStreamMode;

	otl_lob_stream				m_lobStream;
#ifdef _UNICODE
	otl_long_unicode_string*	m_pLongStr;
#else
	otl_long_string*			m_pLongStr;
#endif
	
	std::vector<OTL_CHAR>		m_lobBuff;
};	

template <>
class concrete_handler<otl_handler_char> : public IOtlFieldHandler
{
public:
	concrete_handler( otl_stream* pStream )
		:IOtlFieldHandler(pStream)
	{
		m_dwSize = 0;
		m_szValue = NULL;
	}

	virtual ~concrete_handler()
	{
		delete[] m_szValue;
	}

	virtual BOOL PrepareColumn( otl_column_desc* pColumn )
	{
		if( (DWORD)pColumn->dbsize > m_dwSize )
			m_dwSize = pColumn->dbsize;

		return TRUE;
	}

	virtual BOOL PrepareIO( BOOL bStreamMode )	
	{
		m_szValue = new TCHAR[m_dwSize/sizeof(TCHAR)+1];

		return TRUE;
	}

	virtual VOID GetField( CPropVar* prop, int otlType )
	{
		(*m_pStream) >> (unsigned char*)m_szValue;

		if( m_pStream->is_null() )
			*prop = CPropVar::GetNull();
		else
			*prop = m_szValue;
	}

	static VOID PutVar( otl_stream& stream, CPropVar* prop )
	{
		LPCTSTR szValue = *prop;
		stream << szValue;
	}

private:
	DWORD	m_dwSize;
	LPTSTR	m_szValue;
};

template <>
class concrete_handler<otl_handler_binary> : public IOtlFieldHandler
{
public:
	concrete_handler( otl_stream* pStream )
		:IOtlFieldHandler(pStream)
	{
		m_dwSize = 0;
		m_pBuffer = NULL;
	}

	virtual ~concrete_handler()
	{
		delete m_pBuffer;
	}

	virtual BOOL PrepareColumn( otl_column_desc* pColumn )
	{
		if( (DWORD)pColumn->dbsize > m_dwSize )
			m_dwSize = pColumn->dbsize;

		return TRUE;
	}

	virtual BOOL PrepareIO( BOOL bStreamMode )	
	{
		m_pBuffer = new otl_long_string( m_dwSize );

		return TRUE;
	}

	virtual VOID GetField( CPropVar* prop, int otlType )
	{
		(*m_pStream) >> (*m_pBuffer);

		if( m_pStream->is_null() )
		{
			*prop = CPropVar::GetNull();
			return;
		}
		
		prop->SetVector( (LPVOID)&( (*m_pBuffer)[0] ), m_pBuffer->len() );
	}

	static VOID PutVar( otl_stream& stream, CPropVar* prop )
	{
		LPBYTE pBuffer;
		INT dwSize;

		prop->GetVector( &pBuffer, dwSize );

		if( pBuffer && dwSize )
		{
			otl_long_string longStr( pBuffer, dwSize, dwSize );
			stream << longStr;
		}
	}

private:
	DWORD				m_dwSize;
	otl_long_string*	m_pBuffer;
};

template <>
class concrete_handler<otl_handler_datetime> : public IOtlFieldHandler
{
public:
	concrete_handler( otl_stream* pStream )
		:IOtlFieldHandler(pStream)
	{
		memset( m_szTime, 0, sizeof(m_szTime) );
	}

	virtual VOID GetField( CPropVar* prop, int otlType )
	{
		otl_datetime otlTime;
		(*m_pStream) >> otlTime;

		if( m_pStream->is_null() )
		{
			*prop = CPropVar::GetNull();
			return;
		}

		_stprintf_s( m_szTime, _countof(m_szTime), _T("%d-%d-%d %d:%d:%d"),
			otlTime.year, otlTime.month, otlTime.day,
			otlTime.hour, otlTime.minute, otlTime.second );

		*prop = (LPTSTR)m_szTime;
	}

	static VOID PutVar( otl_stream& stream, CPropVar* prop )
	{
		SYSTEMTIME sysTime = *prop;
		otl_datetime otlTime;

		otlTime.year = sysTime.wYear;
		otlTime.month = sysTime.wMonth;
		otlTime.day = sysTime.wDay;
		otlTime.hour = sysTime.wHour;
		otlTime.minute = sysTime.wMinute;
		otlTime.second = sysTime.wSecond;

		stream << otlTime;
	}

private:
	TCHAR	m_szTime[128];
};