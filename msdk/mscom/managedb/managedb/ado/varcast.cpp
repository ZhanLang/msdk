/********************************************************************
	created:	2009/09/14
	created:	14:9:2009   11:22
	filename: 	d:\C++Work\approduct2010\utm\src\unitive\managedb\varcast.cpp
	file path:	d:\C++Work\approduct2010\utm\src\unitive\managedb
	file base:	varcast
	file ext:	cpp
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "varcast.h"

VOID Var2Prop( const VARIANT& var, PROPVARIANT& prop )
{
	VARTYPE vt = var.vt;

	switch( vt)
	{
	case VT_DECIMAL:
		{
			_variant_t var_t( var );

			prop.vt = VT_I8;
			prop.hVal.QuadPart = (LONGLONG)var_t;
		}
		break;
	case VT_BSTR:
		{
#ifdef _UNICODE
			prop.vt = VT_LPWSTR;
			prop.pwszVal = (wchar_t*)(const wchar_t*)var.bstrVal;
#else
			prop.vt = VT_LPSTR;
			prop.pszVal = (char*)(const char*)var.bstrVal;
#endif
		}
		break;
	case VT_DATE:
		{
			LPTSTR szVal = NULL;
			TCHAR szTime[128] = {0};
			SYSTEMTIME* pSystem = new SYSTEMTIME;
			memset(pSystem,0,sizeof(SYSTEMTIME));
			INT result = VariantTimeToSystemTime(var.date,pSystem);
			if (result != FALSE)
			{
				_stprintf_s( szTime, _countof(szTime), _T("%04d-%02d-%02d %02d:%02d:%02d"),
					pSystem->wYear, pSystem->wMonth, pSystem->wDay,
			        pSystem->wHour, pSystem->wMinute, pSystem->wSecond );
			    _bstr_t bstrVal(szTime);
				szVal = bstrVal;
			}
			else
			{
				_bstr_t bstrVal(var);
				szVal = bstrVal;
			}

#ifdef _UNICODE
			prop.vt = VT_LPWSTR;
			prop.pwszVal = szVal;
#else
			prop.vt = VT_LPSTR;
			prop.pszVal = szVal;
#endif
		}
		break;
	default:
		{
			if( vt&VT_ARRAY )
			{
				SAFEARRAY* pArray = var.parray;
				prop.vt = VT_BUFFER;
				prop.cac.cElems = pArray->cbElements * pArray->rgsabound[0].cElements;
				prop.cac.pElems = (CHAR*)pArray->pvData;
			}
			else
				memcpy( &prop, &var, sizeof(VARIANT) );
		}
	}
}