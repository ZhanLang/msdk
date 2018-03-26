#ifndef __StdAfx_H__
#define __StdAfx_H__


#include <atlstr.h>

#include <assert.h>
#include <mscom/mscominc.h>
using namespace mscom;

#define SIZEOFV(value)      (sizeof(value)/sizeof(value[0]))


// loging 
#define MSP2PLOG_TEST          "[L0] "
#define MSP2PLOG_NOTICE        "[L1] "
#define MSP2PLOG_WARNING       "[L2] "
#define MSP2PLOG_ERROR         "[L8] "

#ifdef _MS_P2P_LOG
	#define MSP2PCLOG(level, format, ...)	\
		{ \
			char szTime[256] = {0}; \
			SYSTEMTIME stNow; \
			GetLocalTime(&stNow); \
			printf("%u-%02u-%02u %02u:%02u:%02u ", stNow.wYear , stNow.wMonth , stNow.wDay , stNow.wHour ,stNow.wMinute, stNow.wSecond); \
			printf(level##format, __VA_ARGS__); \
			printf("\n"); \
		}


	#define p2pfprintf(file, format, ...)	\
		{ \
			/*printf(format, __VA_ARGS__);*/ \
			fprintf(file, format, __VA_ARGS__); \
		}


	#include "P2PClientTool.h"
	class CLogLock : public CBaseLockHandler
	{
	public:
		CLogLock()
			: CBaseLockHandler(m_loglock)
		{}
		~CLogLock()
		{}

	private:
		static CCriticalSetionObject m_loglock;
	};


	typedef struct LogFile_t
	{
		#define LOG_FILE_MAX_SIZE	1024 * 1024		// 1M
		#define MSP2PLOG_NAME		"p2pclient.log"

		int  nSize;
		bool bFirst;

		std::string szName;
		std::string szBakName;

		LogFile_t()
			: bFirst(true), nSize(0) 
		{
			szName = CP2PClientTool::GetModuleDirectoryA() + std::string(MSP2PLOG_NAME);
			szBakName = szName + ".log";
			assert(!szName.empty() && !szName.empty());
		}

		void Update()
		{ 
			if(nSize > LOG_FILE_MAX_SIZE) 
			{
				::remove(szBakName.c_str()); 
				::rename(szName.c_str(), szBakName.c_str());
			}
		}
	}LogFile_t;

	extern LogFile_t g_lfm;


	#define MSP2PFLOG(file, level, format, ...)	\
		{ \
			CLogLock lock; \
			SYSTEMTIME stNow; \
			GetLocalTime(&stNow); \
			FILE* f = NULL; \
			int err = fopen_s(&f, file, "a"); \
			if(f != NULL) \
			{ \
				p2pfprintf(f, "%u-%02u-%02u %02u:%02u:%02u ", stNow.wYear , stNow.wMonth , stNow.wDay , stNow.wHour ,stNow.wMinute, stNow.wSecond); \
				p2pfprintf(f, level##format, __VA_ARGS__); \
				p2pfprintf(f, "\n"); \
				fflush(f); \
				g_lfm.nSize = ftell(f); \
				fclose(f); \
			} \
			else \
			{ \
				MSP2PCLOG(level, format, __VA_ARGS__); \
			} \
		}

	#define MSP2PFLOGX(level, format, ...) \
		{ \
			g_lfm.Update(); \
			MSP2PFLOG(g_lfm.szName.c_str(), level, format, __VA_ARGS__); \
		}

	#define MSP2PLOG		MSP2PFLOGX


#else
	#define MSP2PLOG
#endif


#endif	// #ifndef __StdAfx_H__
