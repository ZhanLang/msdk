#pragma once
#include <comdef.h>
#include <database/genericdb.h>
#include "inc/stdtp.h"
#include "inc/cli.h"

#include <vector>
#include <map>
#include <string>
#include <cassert>
using namespace std;

#define ASSERT assert


namespace Helper
{

	class CTimeUsed
	{
	public:
		CTimeUsed()
		{
#ifdef _DEBUG
			m_dwCount = GetTickCount();
#endif
		};
		CTimeUsed(LPCTSTR lpFuncName)
		{
#ifdef _DEBUG
			m_strFuncName = lpFuncName;
			m_dwCount = GetTickCount();
#endif

		};
		~CTimeUsed()
		{
#ifdef _DEBUG
			DWORD dwTemp = 	GetTickCount();
			DWORD dwThreadID = 	GetCurrentThreadId();
			owstringstream oo;

			oo << _T("[线程],")<<dwThreadID;
			oo << _T(",")<<m_strFuncName;
			oo << _T(",")<<m_dwCount;
			oo << _T(",")<<dwTemp;
			oo << _T(",")<<dwTemp - m_dwCount;
			oo << _T("\n");
			wstring strTemp = oo.str();
			OutputDebugString(strTemp.c_str());
#endif
		};

	private:
		DWORD m_dwCount;
		wstring m_strFuncName;

	};

	void GetDllPath(std::wstring& strDllName,std::wstring&  strDllPath);
	void GetDllPath(std::string& strDllName,std::string&  strDllPath);
	void GetExePath(std::wstring&  strExePath);
	void GetExePath(std::string&  strExePath);

	
	class CMyEvent
	{
	public: 
		CMyEvent()
		{
			m_hEvent = CreateEvent(
				0,
				TRUE,
				FALSE,
				NULL
				);
			ASSERT(m_hEvent);
		};
		~CMyEvent()
		{
			ASSERT(m_hEvent);
			BOOL bRet = CloseHandle(m_hEvent);
		};
	public:
		BOOL SetEvent()
		{
			ASSERT(m_hEvent);
			return ::SetEvent(m_hEvent);

		};
		BOOL ResetEvent()
		{
			ASSERT(m_hEvent);
			return ::ResetEvent(m_hEvent);
		};

		BOOL Wait(DWORD dwTimeOutInterval)
		{
			DWORD dwRet = ::WaitForSingleObject(
				m_hEvent
				,dwTimeOutInterval
				);
			if(WAIT_TIMEOUT == dwRet)
				return FALSE;
			return TRUE;
		};

		HANDLE GetEvtHandle(){return m_hEvent;};
	private:
		HANDLE m_hEvent;
	};

	//信号量.
	class CMySemaphore
	{
	public:
		CMySemaphore()
		{
			const INT MAX_SEM_COUNT = 500;
			m_hSemaphore = CreateSemaphore( 
				NULL,           // default security attributes
				MAX_SEM_COUNT,  // initial count
				MAX_SEM_COUNT,  // maximum count
				NULL);          // unnamed semaphore
			if(NULL == m_hSemaphore)
				return;
			m_lMaxCount = MAX_SEM_COUNT;


		};
		CMySemaphore(INT nSemaCount)
		{
			m_hSemaphore = CreateSemaphore( 
				NULL,           // default security attributes
				nSemaCount,		// initial count
				nSemaCount,		// maximum count
				NULL);          // unnamed semaphore
			if(NULL == m_hSemaphore)
				return;
			m_lMaxCount = nSemaCount;

		};
		~CMySemaphore()
		{
			if(m_hSemaphore)
				CloseHandle(m_hSemaphore);
		};

		BOOL WaitSem(INT iTimeOutInterval)
		{
			DWORD dwWaitResult = WaitForSingleObject(m_hSemaphore,iTimeOutInterval);  
			if(WAIT_TIMEOUT == dwWaitResult)
				return FALSE;
			return TRUE;
		}
		BOOL ReleaseSem(LONG* pCount = NULL)
		{
			BOOL bRet = ReleaseSemaphore(m_hSemaphore,1,pCount);
			return bRet;
		}

		//等待10ms * iTimeOutInterval
		BOOL WaitAllCountFree(UINT uTimeOutInterval)
		{

			DWORD dwWaitTimeOut = 0;
			while(TRUE)
			{
				BOOL bRet = WaitSem(10);
				if(FALSE == bRet)
				{//超时
					continue;
				}
				LONG lCount = 0;
				bRet = ReleaseSem(&lCount);
				if(m_lMaxCount - 1 ==lCount)
					return TRUE;

				//循环等待.
				Sleep(10);
				if(++dwWaitTimeOut >= uTimeOutInterval)
					return TRUE;					
			}

			return FALSE;
		}

	private:
		LONG	m_lMaxCount;
		HANDLE m_hSemaphore;
	};

	class CGetSem
	{
	public:
		CGetSem(CMySemaphore* pSem)
		{
			if(NULL == pSem)
				return ;
			m_pSem = pSem;
			BOOL bRet = m_pSem->WaitSem(-1);

		};
		~CGetSem()
		{
			if(NULL == m_pSem)
				return ;
			BOOL bRet = m_pSem->ReleaseSem();
		};
	private:
		CMySemaphore* m_pSem;		

	};


	//互斥锁
	class CMyLockObj
	{
	public:
		CMyLockObj()
		{
			InitializeCriticalSection(&m_theLock); 
		};
		~CMyLockObj()
		{
			DeleteCriticalSection(&m_theLock);
		};
		LPCRITICAL_SECTION GetLock()
		{
			return &m_theLock;
		};
	private:
		CRITICAL_SECTION	m_theLock;
	};

	class CritLock
	{
	public:
		CritLock(LPCRITICAL_SECTION lpCriticalSection)
			:m_prit(lpCriticalSection)
		{
			EnterCriticalSection(lpCriticalSection);
		}

		~CritLock()
		{
			LeaveCriticalSection(m_prit);
		}

	private:
		LPCRITICAL_SECTION m_prit;
		CritLock& operator=(const CritLock& rhs);
		CritLock();
	};

	//注:这里只接受可以浅拷和sizoef的简单类型,包括类和结构.
	template <typename SimpleT = CHAR>
	class CSimpleBuffer
	{
	public:
		typedef SimpleT item_type;
		CSimpleBuffer()
		{
			m_pBuffer = NULL;
			m_nSize = 0;
		}
		CSimpleBuffer(INT nSize)
		{
			m_pBuffer = NULL;
			m_nSize = nSize;
			m_pBuffer = new SimpleT[nSize];
			if(NULL == m_pBuffer)
				return;
			m_nSize = nSize;
		};
		~CSimpleBuffer()
		{
			if(m_pBuffer)
				delete[] m_pBuffer;
			m_nSize = 0;
		};
		SimpleT* GetPointer(){return m_pBuffer;}
		INT GetSize(){return m_nSize;};
		INT GetByteLen(){return sizeof(SimpleT) * m_nSize;};

	private:
		SimpleT* m_pBuffer;
		INT		 m_nSize;

	};

	class CTransStr
	{
	public:
		CTransStr()
		{
			m_szA = NULL;
			m_szW = NULL;
		};
		~CTransStr()
		{
			if(m_szA)
				delete[] m_szA;
			if(m_szW)
				delete[] m_szW;
		}
	public:
		LPSTR MYW2A( LPCWSTR szW )
		{
			if( !szW || !szW[0] ) 
				return NULL;

			int len = WideCharToMultiByte( CP_ACP, 0, szW, -1, NULL, 0, NULL, NULL );

			LPSTR szA = NULL;
			szA = new CHAR[len];
			if(NULL == szA)
				return NULL;

			len = WideCharToMultiByte( CP_ACP, 0, szW, -1, szA, len, NULL, NULL );

			if(m_szA)
				delete[] m_szA;

			m_szA = szA;
			return m_szA;
		}

		LPWSTR MYA2W( LPCSTR szA )
		{
			if( !szA || !szA[0] ) 
				return NULL;

			int len = MultiByteToWideChar( CP_ACP, 0, szA, -1, NULL, 0 );

			LPWSTR szW = NULL;
			szW = new WCHAR[len];
			if(NULL == szW)
				return NULL;

			len = MultiByteToWideChar( CP_ACP, 0, szA, -1, szW, len );

			if(m_szW)
				delete[] m_szW;

			m_szW = szW;
			return m_szW;
		}

		LPCWSTR ToLower(LPCWSTR szIn)
		{
			if(NULL == szIn)
				return NULL;

			m_strW.clear();
			wstring strTemp = szIn;

			for(UINT i = 0; i < strTemp.size(); ++i)
			{
				WCHAR wch = towlower(strTemp[i]);
				m_strW += wch;
			}

			return m_strW.c_str();
		}

		LPCSTR ToLower(LPCSTR szIn)
		{
			if(NULL == szIn)
				return NULL;

			m_strA.clear();
			string strTemp = szIn;

			for(UINT i = 0; i < strTemp.size(); ++i)
			{
				CHAR ch = tolower(strTemp[i]);
				m_strA += ch;
			}

			return m_strA.c_str();
		}

		BOOL CompareNoCase(LPCWSTR szIn1,LPCWSTR szIn2)
		{
			wstring strTemp1 = ToLower(szIn1);
			wstring strTemp2 = ToLower(szIn2);
			if(strTemp1 == strTemp2)
				return TRUE;
			return FALSE;		
		}
		BOOL CompareNoCase(LPCSTR szIn1,LPCSTR szIn2)
		{
			string strTemp1 = ToLower(szIn1);
			string strTemp2 = ToLower(szIn2);
			if(strTemp1 == strTemp2)
				return TRUE;
			return FALSE;		
		}

		BOOL FindNoCase(LPCSTR szSub,LPCSTR szIn)
		{
			string strSub = ToLower(szSub);
			string strIn = ToLower(szIn);

			static const string::size_type npos = -1;
			string::size_type indexCh2a;
			indexCh2a = strIn.find(strSub.c_str());
			if(indexCh2a == npos )
			{
				return FALSE;
			}
			return TRUE;
		}

		BOOL FindNoCase(LPCWSTR szSub,LPCWSTR szIn)
		{
			wstring strSub = ToLower(szSub);
			wstring strIn = ToLower(szIn);

			static const wstring::size_type npos = -1;
			wstring::size_type indexCh2a;
			indexCh2a = strIn.find(strSub.c_str());
			if(indexCh2a == npos )
			{
				return FALSE;
			}
			return TRUE;
		}

		BOOL IsFirstWord(LPCSTR szSub,LPCSTR szIn)
		{
			string strSub = szSub;
			string strIn = szIn;
			TrimStr(strSub);
			TrimStr(strIn);
			static const wstring::size_type npos = -1;
			string::size_type indexCh2a;
			indexCh2a = strIn.find(strSub.c_str());
			if(npos == indexCh2a )
				return FALSE;
			if(0 != indexCh2a )
				return FALSE;
			return TRUE;
		}

		BOOL IsFirstWord(LPCWSTR szSub,LPCWSTR szIn)
		{
			wstring strSub = szSub;
			wstring strIn = szIn;
			TrimStr(strSub);
			TrimStr(strIn);

			static const wstring::size_type npos = -1;
			wstring::size_type indexCh2a;
			indexCh2a = strIn.find(strSub.c_str());
			if(npos == indexCh2a )
				return FALSE;
			if(0 != indexCh2a )
				return FALSE;
			return TRUE;
		}

		BOOL FindCharInStr(LPCWSTR lpPath,WCHAR wch)
		{
			if(NULL == lpPath)
				return FALSE;
			wstring strPath = lpPath;
			wstring::size_type nPos = -1;
			wstring::size_type iFind = strPath.find(wch);
			if(nPos == iFind)
				return FALSE;

			return TRUE;
		}

		BOOL FindCharInStr(LPCSTR lpPath,CHAR ch)
		{
			if(NULL == lpPath)
				return FALSE;
			string strPath = lpPath;
			string::size_type nPos = -1;
			string::size_type iFind = strPath.find(ch);
			if(nPos == iFind)
				return FALSE;

			return TRUE;
		}


		BOOL FindNextWord(LPCSTR szSub,LPCSTR szIn,string& strFinded)
		{
			string strInBak = szIn;
			string strSub = ToLower(szSub);
			string strIn = ToLower(szIn);

			INT iLeftBlankCount = CountLeftBlank(strIn);

			TrimStr(strSub);
			TrimStr(strIn);
			static const string::size_type npos = -1;
			string::size_type indexCh2a;
			indexCh2a = strIn.find(strSub.c_str());
			if(npos == indexCh2a )
				return FALSE;
			
			//取余下的字串.
			string strTemp(strInBak,iLeftBlankCount + indexCh2a + strSub.size(),strIn.length() - 1 );
			TrimStr(strTemp);

			string::size_type indexCh1a;
			indexCh1a = strTemp.find(TEXT(' '));
			if(npos == indexCh1a )
			{
				strFinded = strTemp;
				return TRUE;
			}


			strFinded = string(strTemp,0,indexCh1a);

			return TRUE;
		}

		INT CountLeftBlank(wstring& strIn)
		{
			INT iCount = 0;
			for(UINT i = 0;i < strIn.size(); ++i)
			{
				if(L' ' == strIn[i])
					++iCount;
				else
					break;
			}
			return iCount;
		}

		INT CountLeftBlank(string& strIn)
		{
			INT iCount = 0;
			for(UINT i = 0;i < strIn.size(); ++i)
			{
				if(' ' == strIn[i])
					++iCount;
				else
					break;
			}
			return iCount;
		}



		BOOL FindNextWord(LPCWSTR szSub,LPCWSTR szIn,wstring& strFinded)
		{
			wstring strInBak = szIn;
			wstring strSub = ToLower(szSub);
			wstring strIn = ToLower(szIn);
			TrimStr(strSub);
			TrimStr(strIn);
			static const wstring::size_type npos = -1;
			wstring::size_type indexCh2a;
			indexCh2a = strIn.find(strSub.c_str());
			if(npos == indexCh2a )
				return FALSE;

			//取余下的字串.
			wstring strTemp(strInBak,indexCh2a + strSub.size(),strIn.length() - 1 );
			TrimStr(strTemp);

			wstring::size_type indexCh1a;
			wstring::size_type indexCh3a;
			indexCh1a = strTemp.find(TEXT(' '));
			indexCh3a = strTemp.find(TEXT(';'));
			if(npos == indexCh1a && indexCh3a == npos && strTemp.size() > 0)
			{
				strFinded = strTemp;				
				return TRUE;
			}



			strFinded = wstring(strTemp,0,indexCh1a);
			return TRUE;
		}

		BOOL TrimStr(wstring& strText)
		{
			TrimLeft(strText);
			TrimRight(strText);
			return TRUE;
		}

		BOOL TrimLeft(wstring& strText)
		{
			if(strText.length() <= 0)
				return TRUE;

			wstring::size_type indexCh1a;
			static const wstring::size_type npos = -1;
			if(strText[0] != TEXT(' '))
				return TRUE;


			indexCh1a = strText.find_first_not_of(TEXT(' '));
			if ( indexCh1a != npos )
			{
				wstring strTemp(strText,indexCh1a ,strText.length() - 1 );
				strText = strTemp;
			}

			return TRUE;
		}
		BOOL TrimRight(wstring& strText)
		{
			if(strText.length() <= 0)
				return TRUE;

			wstring::size_type indexCh1a;
			static const wstring::size_type npos = -1;

			if(strText[strText.length() - 1] != TEXT(' '))
				return TRUE;

			indexCh1a = strText.find_last_not_of(TEXT(' '));
			if ( indexCh1a != npos )
			{
				wstring strTemp(strText,0,indexCh1a + 1);
				strText = strTemp;
			}

			return TRUE;
		}


		BOOL TrimStr(string& strText)
		{
			TrimLeft(strText);
			TrimRight(strText);
			return TRUE;
		}

		BOOL TrimLeft(string& strText)
		{
			if(strText.length() <= 0)
				return TRUE;

			string::size_type indexCh1a;
			static const string::size_type npos = -1;
			if(strText[0] != ' ')
				return TRUE;


			indexCh1a = strText.find_first_not_of(' ');
			if ( indexCh1a != npos )
			{
				string strTemp(strText,indexCh1a ,strText.length() - 1 );
				strText = strTemp;
			}

			return TRUE;
		}
		BOOL TrimRight(string& strText)
		{
			if(strText.length() <= 0)
				return TRUE;

			string::size_type indexCh1a;
			static const string::size_type npos = -1;

			if(strText[strText.length() - 1] != ' ')
				return TRUE;

			indexCh1a = strText.find_last_not_of(' ');
			if ( indexCh1a != npos )
			{
				string strTemp(strText,0,indexCh1a + 1);
				strText = strTemp;
			}

			return TRUE;
		}




	private:
		LPWSTR m_szW;
		LPSTR  m_szA;

		wstring m_strW;
		string m_strA;
	};

}

//
//class dbList { 
//public:
//	enum NodeType { 
//		nInteger,
//		nBool,
//		nReal,
//		nString,
//		nTuple,
//		nAutoinc,
//		nIdentifier
//	};
//
//	BOOL GetDBListTypeByVTType(INT nVTType)
//	{
//		//把VT类型映射到cli类型.cli_var_type 
//		switch( nVTType )
//		{
//		case VT_INT:
//		case VT_UINT:
//		case VT_I4:
//		case VT_UI4:
//			return dbList::nInteger;
//		case VT_LPSTR:
//			return dbList::nString;
//		case VT_LPWSTR:
//			return dbList::nString;
//		case VT_I8:
//			return dbList::nInteger;
//		default:
//			{
//				FAILEXIT_FALSE(FALSE);
//			}
//			break;
//		}
//
//		return FALSE;
//	}
//
//	dbList* next;
//	int     type;
//	union { 
//		bool  bval;
//		db_int8  ival;
//		real8 fval;
//		char* sval;
//		struct { 
//			int     nComponents;
//			dbList* components;
//		} aggregate;
//	};
//
//	~dbList() { 
//		if (type == nTuple) {
//			dbList* list = aggregate.components;
//			while (list != NULL) { 
//				dbList* tail = list->next;
//				delete list;
//				list = tail;
//			}
//		} else if (type == nString || type == nIdentifier) {
//			delete[] sval;
//		}
//	}
//
//	dbList(int type) { 
//		this->type = type;
//		next = NULL; 
//	}
//
//	dbList(VARIANT* pVar) 
//	{ 
//		next = NULL;
//		
//		switch( pVar->vt )
//		{
//		case VT_INT:
//		case VT_I4:
//			type = nInteger;
//			ival = pVar->intVal;
//			break;
//		case VT_UINT:
//		case VT_UI4:
//			type = nInteger;
//			 ival = pVar->uintVal;
//			 break;
//		case VT_LPSTR:
//			{	
//				type = nString;
//				int nLen = strlen(pVar->pcVal);
//				sval = new CHAR[nLen + 1];
//				strcpy_s(sval,nLen + 1,pVar->pcVal);
//			}
//			 break;
//		case VT_LPWSTR:
//			{
//				type = nString;
//				Helper::CTransStr theTrans;
//				string strVal = theTrans.MYW2A((LPCWSTR)pVar->puiVal);
//				int nLen = wcslen((LPCWSTR)pVar->puiVal);
//				sval = new CHAR[nLen + 1];
//				strcpy_s(sval,nLen + 1,strVal.c_str());
//			}
//		case VT_I8:
//			type = nInteger;
//			ival = pVar->ullVal;
//			break;
//		default:
//			{
//				ASSERT(0);
//			}
//			break;
//		}
//		return;
//	}
//};
//struct tableField { 
//	char* name;
//	char* refTableName;
//	char* inverseRefName;
//	int   type;
//
//	tableField() { name = refTableName = inverseRefName = NULL; }
//	~tableField() { delete[] name; delete[] refTableName; delete[] inverseRefName; }
//};




struct tag_Field
{
	INT		iFieldType;	//VT_XXXX
	WCHAR	szFieldName[MAX_PATH];
	INT		iFieldSize;					//数组中元素个数.
	BOOL	iFieldFlag;	

	INT GetFieldLenByType()
	{
		//把VT类型映射到cli类型.cli_var_type 
		return this->iFieldSize;
	}

	INT GetCliTypeByVTType()
	{
		//把VT类型映射到cli类型.cli_var_type 
		switch( this->iFieldType )
		{
		case VT_INT:
		case VT_UINT:
		case VT_I4:
		case VT_UI4:
			return cli_int4;
		case VT_LPSTR:
			return cli_asciiz;
		case VT_LPWSTR:
			return cli_wstring;
		case VT_I8:
			return cli_int8;
		default:
			{
				FAILEXIT_NULL(FALSE);
			}
			break;
		}

		return 0;
	}



	INT GetTypeNameByVTType(wstring& strTypeName)
	{
		//把VT类型映射到cli类型.cli_var_type 
		switch( this->iFieldType )
		{
		case VT_INT:
			strTypeName = TEXT("VT_INT");
			break;
		case VT_UINT:
			strTypeName = TEXT("VT_UINT");	
			break;
		case VT_I4:
			strTypeName = TEXT("VT_I4");
			break;
		case VT_UI4:
			strTypeName = TEXT("VT_UI4");
			break;
		case VT_LPSTR:
			strTypeName = TEXT("VT_LPSTR");
			break;
		case VT_LPWSTR:
			strTypeName = TEXT("VT_LPWSTR");
			break;
		case VT_I8:
			strTypeName = TEXT("VT_I8");
			break;
		default:
			{
				FAILEXIT_NULL(FALSE);
			}
			break;
		}

		return this->iFieldType;
	}

	INT GetVTTypeByCliType(INT iCliType)
	{
		//把VT类型映射到cli类型.cli_var_type 
		switch(iCliType )
		{
		case cli_int4:
			return VT_I4;

		case cli_asciiz:
			return VT_LPSTR;

		case cli_wstring:
			return VT_LPWSTR;

		case cli_int8:
			return  VT_I8;
		default:
			{
				FAILEXIT_NULL(FALSE);
			}
			break;
		}

		return 0;
	}

	INT GetFieldByteLenByVTType()
	{
		int iByteLen = 0;
		switch( this->iFieldType )
		{
		case VT_INT:
		case VT_UINT:
		case VT_I4:
		case VT_UI4:
			{
				iByteLen = sizeof(UINT) * this->iFieldSize;
			}
			break;
		case VT_LPSTR:
			{
				iByteLen = sizeof(CHAR) * this->iFieldSize;
			}
			break;
		case VT_LPWSTR:
			{
				iByteLen = sizeof(WCHAR) * this->iFieldSize;
			}
			break;
		case VT_I8:
			{
				iByteLen = sizeof(LONGLONG) * this->iFieldSize;
			}
			break;
		default:
			{
				FAILEXIT_NULL(FALSE);
			}
			break;
		}	

		return iByteLen;
	}

	Helper::CSimpleBuffer<>* NewSimpleBuf()
	{

		using namespace Helper;
		FAILEXIT_NULL(this->iFieldSize);
		int iSize = this->iFieldSize;
		Helper::CSimpleBuffer<>* pBuff = NULL;
		switch( this->iFieldType )
		{
		case VT_INT:
		case VT_UINT:
		case VT_I4:
		case VT_UI4:
			{
				pBuff = (CSimpleBuffer<>*)new CSimpleBuffer<UINT>(iSize);
				FAILEXIT_NULL(pBuff);
			}
			break;
		case VT_LPSTR:
			{
				pBuff = (CSimpleBuffer<>*)new CSimpleBuffer<CHAR>(iSize);
				FAILEXIT_NULL(pBuff);
			}
			break;
		case VT_LPWSTR:
			{
				pBuff = (CSimpleBuffer<>*)new CSimpleBuffer<WCHAR>(iSize);
				FAILEXIT_NULL(pBuff);
			}
			break;
		case VT_I8:
			{
				pBuff = (CSimpleBuffer<>*)new CSimpleBuffer<LONGLONG>(iSize);
				FAILEXIT_NULL(pBuff);
			}
			break;
		default:
			{
				FAILEXIT_NULL(FALSE);
			}
			break;
		}	

		return pBuff;
	}

	BOOL GetVar(Helper::CSimpleBuffer<>* pBuff,_variant_t& theVar)
	{
		FAILEXIT_FALSE(pBuff);

		using namespace Helper;
		switch( this->iFieldType )
		{
		case VT_INT:
		case VT_UINT:
		case VT_I4:
		case VT_UI4:
			{
				theVar = *((INT*)pBuff->GetPointer());
			}
			break;
		case VT_LPSTR:
			{
				theVar = (LPSTR)pBuff->GetPointer();
			}
			break;
		case VT_LPWSTR:
			{
				theVar = (LPWSTR)pBuff->GetPointer();
			}
			break;
		case VT_I8:
			{
				theVar = *((LONGLONG*)pBuff->GetPointer());
			}
			break;
		default:
			{
				FAILEXIT_FALSE(FALSE);
			}
			break;
		}	

		return TRUE;
	}


	INT TransFieldFlag()
	{
		return this->iFieldFlag;
	}
};

class CPropertySet
{
public:
	static HRESULT CreateProp( IProperty2** ppProp )
	{
		TProperty<IProperty2>* pImpl = new TProperty<IProperty2>;
		RASSERTP(pImpl, E_OUTOFMEMORY);

		return pImpl->QueryInterface( __uuidof(IProperty2), (void**)ppProp );
	}
};

typedef vector<tag_Field>			VEC_FIELDSET;
typedef map<string,VEC_FIELDSET >  MAP_TABLE;