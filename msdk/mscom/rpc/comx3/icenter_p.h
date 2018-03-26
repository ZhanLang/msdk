// icenter_p.h
//

#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"


#ifndef __icenter_p_h__
#define __icenter_p_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* [fault_status][comm_status] */ error_status_t RPC_Ping( 
    /* [in] */ handle_t Binding);

/* [fault_status][comm_status] */ error_status_t RPC_CheckSecurity( 
    /* [in] */ handle_t Binding);

/* [fault_status][comm_status] */ error_status_t RPC_ServerCall( 
	/* [in] */ handle_t Binding,
	/* [in] */ int nSourceId,
	/* [in] */ int nSourceMaskId,
	/* [in] */ int nSourceObjectId,
	/* [in] */ int nTargetId,
	/* [in] */ int nTargetMaskId,
	/* [in] */ int nTargetObjectId,
	/* [in] */ int nCallType,
	/* [in] */ int nTimeout,
	/* [in] */ int nMessageId);

/* [fault_status][comm_status] */ error_status_t RPC_ServerCall1( 
	/* [in] */ handle_t Binding,
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
	/* [in] */ int nInCch);

/* [fault_status][comm_status] */ error_status_t RPC_ServerCall2( 
	/* [in] */ handle_t Binding,
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
	/* [out] */ int __RPC_FAR *pOutCch);

#ifdef __cplusplus
}
#endif


#define RPC_CALL_INTERFACE_VER			1


//server
RPC_STATUS ServiceStart( LPCTSTR szEndpoint, HANDLE hFireStartOver = NULL );
VOID ServiceStop();

//client
RPC_BINDING_HANDLE ClientConnect(LPCTSTR szEndpoint, unsigned char* &stringBinding);
void DisConnect(RPC_BINDING_HANDLE &Binding, unsigned char* &stringBinding);

#endif