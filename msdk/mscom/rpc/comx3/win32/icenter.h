/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu May 22 13:52:17 2008
 */
/* Compiler settings for icenter.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __icenter_h__
#define __icenter_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef ___MarginServerCommunication_INTERFACE_DEFINED__
#define ___MarginServerCommunication_INTERFACE_DEFINED__

/* interface _MarginServerCommunication */
/* [implicit_handle][unique][version][uuid] */ 

/* [fault_status][comm_status] */ error_status_t Ping( 
    /* [in] */ handle_t Binding);

/* [fault_status][comm_status] */ error_status_t CheckSecurity( 
    /* [in] */ handle_t Binding);

/* [fault_status][comm_status] */ error_status_t ServerCall( 
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

/* [fault_status][comm_status] */ error_status_t ServerCall1( 
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

/* [fault_status][comm_status] */ error_status_t ServerCall2( 
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


extern handle_t RisingServerCenter_IfHandle;


extern RPC_IF_HANDLE _MarginServerCommunication_v1_0_c_ifspec;
extern RPC_IF_HANDLE _MarginServerCommunication_v1_0_s_ifspec;
#endif /* ___MarginServerCommunication_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
