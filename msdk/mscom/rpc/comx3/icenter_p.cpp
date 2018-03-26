
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <rpc.h>

#include "rscenter.h"

#include "icenter_p.h"

#pragma comment(lib, "Rpcrt4.lib")

//
// RPC configuration.
//

// 自动识别操作系统
BOOL s_bIsNt;
static struct st_get_os
{
	st_get_os()
	{
		s_bIsNt = !(GetVersion() & 0x80000000);
	}
}s_my_get_os;



// This service listens to all the protseqs listed in this array.
// This should be read from the service's configuration in the
// registery.

TCHAR *ProtocolArray[] = { TEXT("ncalrpc"),
                           TEXT("ncacn_ip_tcp"),
                           TEXT("ncacn_np"),
                           TEXT("ncadg_ip_udp")
                         };


// Use in RpcServerListen().  More threads will increase performance,
// but use more memory.
ULONG MinimumThreads = 3;
//DWORD g_dwThreadID = 0;

RPC_STATUS ServiceStart ( LPCTSTR szEndpoint, HANDLE hFireStartOver )
{
    RPC_BINDING_VECTOR *pbindingVector = 0;
    RPC_STATUS status;
    BOOL fListening = FALSE;

	unsigned char * pszProtocolSequence = (unsigned char *)ProtocolArray[0];
	unsigned char * pszSecurity         = (unsigned char *)NULL;
	unsigned char * pszEndpoint         = (unsigned char *)szEndpoint;
	unsigned int    cMinCalls           = 3;
	unsigned int    cMaxCalls           = 30;
	unsigned int	fDontWait			= TRUE;

	Assert(szEndpoint);

//	g_dwThreadID = 0;
 
 	status = RpcServerUseProtseqEp(pszProtocolSequence,
		cMaxCalls,//Backlog queue length for the ncacn_ip_tcp protocol sequence. All other protocol sequences ignore this parameter. Use RPC_C_PROTSEQ_MAX_REQS_DEFAULT to specify the default value. 
		pszEndpoint,
		pszSecurity);  // Security descriptor

 	
	if (status != RPC_S_OK)
	{
		if(RPC_S_DUPLICATE_ENDPOINT!=status)
		{
			// Unable to listen to any protocol!
			//
			Msg(TEXT("RpcServerUseProtseq() failed\n"));
			if(hFireStartOver) SetEvent(hFireStartOver);
			return status;
		}
		//接口必须注册，所以不能跳过
		//goto DirectListening;
	}


    Msg(TEXT("Register the services interface(s).\n"));
    //

	//注册一个自动监听的接口。这种接口在发注册的时候能自动停止监听，如果本进程只有一个自动监听接口
	//那么反注册会就等于关闭了进程的rpc监听，调用RpcMgmtWaitServerListen()将立即返回。
    status = RpcServerRegisterIfEx(_MarginServerCommunication_v1_0_s_ifspec,   // from rpcsvc.h
                                 NULL,
                                 NULL,
								 RPC_IF_AUTOLISTEN,
								 RPC_C_LISTEN_MAX_CALLS_DEFAULT,
								 NULL);

	if (status != RPC_S_OK)
	{
		Msg(TEXT("RpcServerRegisterIf() failed\n"));
		if(hFireStartOver) SetEvent(hFireStartOver);
		return status;
	}
 
	if(s_bIsNt)
	{
 		Msg(TEXT("Enable NT LM Security Support Provider (NtLmSsp service).\n"));
 		//
 		status = RpcServerRegisterAuthInfo(0,
 										RPC_C_AUTHN_WINNT,
 										0,
 										0
 										);
 	 
 		if (status != RPC_S_OK)
 		{
 			Msg("放弃注册");

//  				if(hFireStartOver) 
//  				{
//  					SetEvent(hFireStartOver);
//  				}
//  				return status;
 		}
	}

// DirectListening:
  //  Msg(TEXT("Start accepting client calls.\n"));

	//这些代码已经不需要了。
// 	status = RpcMgmtIsServerListening(NULL);
// 	if(RPC_S_NOT_LISTENING ==status)
// 	{
// 		//
// 		status = RpcServerListen(MinimumThreads,
// 			RPC_C_LISTEN_MAX_CALLS_DEFAULT,  // rpcdce.h
// 			TRUE);                           // don't block.
// 
// 		if (status != RPC_S_OK)
// 		{
// 			if(hFireStartOver) SetEvent(hFireStartOver);
// 			return status;
// 		}
// 
// 		if(hFireStartOver) SetEvent(hFireStartOver);
// 		status = RpcMgmtWaitServerListen();
// 	}
// 	else
// 	{
// 
// 		status = RpcMgmtIsServerListening(NULL);
// 		if(RPC_S_NOT_LISTENING ==status)
// 		{
// 			status = RpcServerListen(MinimumThreads,
// 				RPC_C_LISTEN_MAX_CALLS_DEFAULT,  // rpcdce.h
// 				TRUE);       
// 		}

 
//		Msg(TEXT("%d RpcMgmt Is Server Listening! PeekMessage...\n"), g_dwThreadID);

		if(hFireStartOver) SetEvent(hFireStartOver);

// 		MSG msg;
// 		while( GetMessage(&msg, NULL,0, 0) )
// 		{
// 			DispatchMessage(&msg);
// 		}

		//}

		//
		// End of initialization
		//
		////////////////////////////////////////////////////////////

		Msg(TEXT("RpcMgmtWaitServerListen() will block until the server has stopped listening.\n"));

		////////////////////////////////////////////////////////////
		//
		// Cleanup
		//

		// RpcMgmtWaitServerListen() will block until the server has
		// stopped listening.  If this service had something better to
		// do with this thread, it would delay this call until
		// ServiceStop() had been called. (Set an event in ServiceStop()).
		//

		// ASSERT(status == RPC_S_OK)
		/*
		Msg(TEXT("Remove entries from the endpoint mapper database.\n"));
		//
		RpcEpUnregister(_MarginCommunication_v1_0_s_ifspec,   // from rpcsvc.h
		pbindingVector,
		0);

		Msg(TEXT("Delete the binding vector.\n"));
		//
		RpcBindingVectorFree(&pbindingVector);
		*/
		//
		////////////////////////////////////////////////////////////
		if(hFireStartOver) SetEvent(hFireStartOver);
		return RPC_S_OK;
}
  
VOID ServiceStop()
{
	//老代码

//	if(g_dwThreadID!=0)
//	{
//		PostThreadMessage(g_dwThreadID, WM_QUIT, 0, 0);
//		g_dwThreadID = 0;
		//RpcMgmtWaitServerListen();
	//RPC_BINDING_VECTOR *rpcBindingVector;
	//status = RpcServerInqBindings(&rpcBindingVector);
// 	if(rpcBindingVector->Count == 1)
// 	{
// 		status = RpcMgmtStopServerListening(NULL);
// 		status = RpcMgmtWaitServerListen();
// 	}else
// 	{
//		status = RpcMgmtStopServerListening(rpcBindingVector->BindingH[0]);
/*	}*/ 	 
// 	}
// 	else
// 	{
// 		// Stop's the server, wakes the main thread.
// 		RPC_STATUS status;
// 		status = RpcMgmtStopServerListening(0);
// 		if (status != RPC_S_OK)
// 		{
// 			Warning(TEXT("Stop Server Listening faild. ecode=%d\n"), status);
// 		}
// 		//status = RpcServerUnregisterIf(NULL, NULL, FALSE);
// 		if (status != RPC_S_OK)
// 		{
// 			Warning(TEXT("Server Unregister faild. ecode=%d\n"), status);
// 		}
// 	}	


	RPC_STATUS status;

 
	//反注册会自动关闭该接口的监听，因为注册的是自动监听接口
	status = RpcServerUnregisterIf(_MarginServerCommunication_v1_0_s_ifspec, NULL, TRUE);

	if(status != RPC_S_OK)
	{
		Warning(TEXT("反注册接口失败"));
	}
}


error_status_t
Ping( handle_t h )
{
	Msg(TEXT("%d   Ping\n"), GetCurrentThreadId());
	DWORD dwPid = 0;
	/*
	//Applications must not call these	routines! for test!
	RPC_STATUS status = I_RpcBindingInqLocalClientPID(h, &dwPid);
	Msg(TEXT("I_RpcBindingInqLocalClientPID = %d. err=%d\n"), dwPid, status);
	*/
    return(0);
}

//
//  FUNCTION: CheckSecurity
//
//  PURPOSE: Demonstrates the RPC security APIs.
//
//  PARAMETERS:
//    h - binding to client which made the call.
//
//  RETURN VALUE:
//    0 - no error
//
error_status_t
CheckSecurity(  handle_t h  )
{
	Msg(TEXT("%d   CheckSecurity\n"), GetCurrentThreadId());
	RPC_STATUS status;

	// At this point the thread is running in the server
	// security context.  There is guarantee that the client
	// even used a secure connection.

	status = RpcImpersonateClient(h);

	if (status != RPC_S_OK)
	{
		return(RPC_S_ACCESS_DENIED);
	}

	// This thread is now running in the clients security context.

	//
	// The server should now open a file, mutex, event or its own data
	// structure which has an ACL associated with it to check that the
	// client has the right to access the server's protected data.
	//
	Msg(TEXT("RpcImpersonateClient ok\n"));

	status = RpcRevertToSelf();

	// ASSERT(status == RPC_S_OK);

	// This thread is now running in the server's security context.

	return(0);
}

error_status_t ServerCall(	/* [in] */ handle_t Binding,
							/* [in] */ int nSourceId,
							/* [in] */ int nSourceMaskId,
							/* [in] */ int nSourceObjectId,
							/* [in] */ int nTargetId,
							/* [in] */ int nTargetMaskId,
							/* [in] */ int nTargetObjectId,
							/* [in] */ int nCallType,
							/* [in] */ int nTimeout,
							/* [in] */ int nMessageId)
{

	Msg(TEXT("%d  %d  ServerCall\n"), GetCurrentThreadId(), GetCurrentProcessId());
	//*lpOutData = (byte*)RpcSsAllocate(sizeof(int));
	//*pOutCch = sizeof(int);
	MESSAGEINFO callmsg = 
	{
		sizeof(MESSAGEINFO),			
		nCallType&0x0000ffff,
		nTimeout,
		nSourceId,
		nSourceMaskId,
		nSourceObjectId,
		nTargetId,
		nTargetMaskId,
		nTargetObjectId,
		nCallType&0xffff0000,
		nMessageId,
		NULL,
		0,
		NULL,
		0,
	};
	return OnCallEvent(&callmsg);	
}

error_status_t ServerCall1(	/* [in] */ handle_t Binding,
							/* [in] */ int nSourceId,
							/* [in] */ int nSourceMaskId,
							/* [in] */ int nSourceObjectId,
							/* [in] */ int nTargetId,
							/* [in] */ int nTargetMaskId,
							/* [in] */ int nTargetObjectId,
							/* [in] */ int nCallType,
							/* [in] */ int nTimeout,
							/* [in] */ int nMessageId,
							/* [size_is][in] */ byte __RPC_FAR *lpInData,
							/* [in] */ int nInCch)
{

	Msg(TEXT("%d  %d  ServerCall1\n"), GetCurrentThreadId(), GetCurrentProcessId());
	//*lpOutData = (byte*)RpcSsAllocate(sizeof(int));
	//*pOutCch = sizeof(int);
	MESSAGEINFO callmsg = 
	{
		sizeof(MESSAGEINFO),			
			nCallType&0x0000ffff,
			nTimeout,
			nSourceId,
			nSourceMaskId,
			nSourceObjectId,
			nTargetId,
			nTargetMaskId,
			nTargetObjectId,
			nCallType&0xffff0000,
			nMessageId,
			lpInData,
			nInCch,
			NULL,
			0,
	};
	return OnCallEvent(&callmsg);	
}

error_status_t ServerCall2( /* [in] */ handle_t Binding,
							/* [in] */ int nSourceId,
							/* [in] */ int nSourceMaskId,
							/* [in] */ int nSourceObjectId,
							/* [in] */ int nTargetId,
							/* [in] */ int nTargetMaskId,
							/* [in] */ int nTargetObjectId,
							/* [in] */ int nCallType,
							/* [in] */ int nTimeout,
							/* [in] */ int nMessageId,
							/* [size_is][in] */ byte __RPC_FAR *lpInData,
							/* [in] */ int nInCch,
							/* [size_is][size_is][out] */ byte __RPC_FAR *__RPC_FAR *lpOutData,
							/* [out] */ int __RPC_FAR *pOutCch)
{
	Msg(TEXT("%d  %d  ServerCall2\n"), GetCurrentThreadId(), GetCurrentProcessId());
	MESSAGEINFO callmsg = 
	{
		sizeof(MESSAGEINFO),
		nCallType&0x0000ffff,
		nTimeout,
		nSourceId,
		nSourceMaskId,
		nSourceObjectId,
		nTargetId,
		nTargetMaskId,
		nTargetObjectId,
		nCallType&0xffff0000,
		nMessageId,
		lpInData,
		nInCch,
		lpOutData,
		pOutCch,
	};
	return OnCallEvent(&callmsg);
}
//
//  FUNCTIONS: MIDL_user_allocate and MIDL_user_free
//
//  PURPOSE: Used by stubs to allocate and free memory
//           in standard RPC calls. Not used when
//           [enable_allocate] is specified in the .acf.
//
//
//  PARAMETERS:
//    See documentations.
//
//  RETURN VALUE:
//    Exceptions on error.  This is not required,
//    you can use -error allocation on the midl.exe
//    command line instead.
//
//

void * __RPC_USER MIDL_user_allocate(size_t size)
{
//#ifdef _DEBUG
//	return Sys_Alloc(size);
//#else
    return(HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, size));
//#endif
}

void __RPC_USER MIDL_user_free( void *pointer)
{
//#ifdef _DEBUG
//	Sys_Free(pointer);
//#else
    HeapFree(GetProcessHeap(), 0, pointer);
//#endif
}

RPC_BINDING_HANDLE ClientConnect(LPCTSTR szEndpoint, unsigned char* &stringBinding)
{
	char *serverAddress = NULL;
	char *protocol = "ncalrpc";
	//char *szEndpoint = "68F0002B-CB21-4b20-BC31-016F7357B271";	
	UINT iIterations = 1;	
	RPC_BINDING_HANDLE Binding;
	RPC_STATUS status;
	ULONG SecurityLevel = RPC_C_AUTHN_LEVEL_NONE;

	Msg("Client binding Endpoint: %s\n", szEndpoint);

	status = RpcStringBindingCompose(0,
		(unsigned char *)protocol,
		(unsigned char *)serverAddress,
		(unsigned char *)szEndpoint,
		0,
		&stringBinding);
	if (status != RPC_S_OK)
	{
		Msg("RpcStringBindingCompose failed - %d\n", status);
		return NULL;
	}

	status = RpcBindingFromStringBinding(stringBinding, &Binding);

	if (status != RPC_S_OK)
	{
		Msg("RpcBindingFromStringBinding failed - %d\n", status);
		return NULL;
	}
	
	if(s_bIsNt)
	{
		status = RpcBindingSetAuthInfo(Binding,
			0,
			SecurityLevel,
			RPC_C_AUTHN_WINNT,
			0,
			0
			);

		if (status != RPC_S_OK)
		{
			Msg("RpcBindingSetAuthInfo  RPC_C_AUTHN_WINNT failed - %d\n", status);
			status = RpcBindingSetAuthInfo(Binding,
				0,
				SecurityLevel,
				RPC_C_AUTHN_GSS_NEGOTIATE,
				0,
				0
				);

			if (status != RPC_S_OK)
			{
				Msg("RpcBindingSetAuthInfo  RPC_C_AUTHN_GSS_NEGOTIATE failed - %d\n", status);
				return NULL;
			}
		}
	}

//	status = RPC_Ping(Binding);
// 	if (status != RPC_S_OK)
// 	{
// 		Msg("Ping failed - %d\n", status);
// 		DisConnect(Binding, stringBinding);
// 		Binding = NULL;
// 	}

	Msg("RPC Connected.\n");

	return Binding;
}

void DisConnect(RPC_BINDING_HANDLE &Binding, unsigned char *&stringBinding)
{
	Msg("Rpc stringBinding Free.\n");

	RPC_STATUS status;
	// Cleanup

	status = RpcBindingFree(&Binding);

	// ASSERT(status == RPC_S_OK):

	status = RpcStringFree(&stringBinding);

	// ASSERT(status == RPC_S_OK);

	Msg("DisConnect.\n");
}