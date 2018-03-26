

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Thu Mar 21 01:21:39 2013
 */
/* Compiler settings for .\icenter.idl:
    Oicf, W1, Zp8, env=Win64 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __icenter_h__
#define __icenter_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef ___MarginServerCommunication_INTERFACE_DEFINED__
#define ___MarginServerCommunication_INTERFACE_DEFINED__

/* interface _MarginServerCommunication */
/* [unique][version][uuid] */ 

error_status_t Ping( 
    /* [in] */ handle_t Binding);

error_status_t CheckSecurity( 
    /* [in] */ handle_t Binding);

error_status_t ServerCall( 
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

error_status_t ServerCall1( 
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
    /* [size_is][in] */ byte *lpInData,
    /* [in] */ int nInCch);

error_status_t ServerCall2( 
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
    /* [size_is][in] */ byte *lpInData,
    /* [in] */ int nInCch,
    /* [size_is][size_is][out] */ byte **lpOutData,
    /* [out] */ int *pOutCch);



extern RPC_IF_HANDLE _MarginServerCommunication_v1_0_c_ifspec;
extern RPC_IF_HANDLE _MarginServerCommunication_v1_0_s_ifspec;
#endif /* ___MarginServerCommunication_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


