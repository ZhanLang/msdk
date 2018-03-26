/* this ALWAYS GENERATED file contains the RPC client stubs */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu May 22 13:52:17 2008
 */
/* Compiler settings for icenter.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )

#include <string.h>
#if defined( _ALPHA_ )
#include <stdarg.h>
#endif

#include "icenter.h"

#define TYPE_FORMAT_STRING_SIZE   39                                
#define PROC_FORMAT_STRING_SIZE   91                                

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;

/* Standard interface: _MarginServerCommunication, ver. 1.0,
   GUID={0xFF37DF02,0xE8B0,0x4db8,{0x93,0xD1,0xFD,0x2F,0x10,0x24,0xD3,0xE1}} */

handle_t RisingServerCenter_IfHandle;


static const RPC_CLIENT_INTERFACE _MarginServerCommunication___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0xFF37DF02,0xE8B0,0x4db8,{0x93,0xD1,0xFD,0x2F,0x10,0x24,0xD3,0xE1}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    0,
    0,
    0,
    0,
    0
    };
RPC_IF_HANDLE _MarginServerCommunication_v1_0_c_ifspec = (RPC_IF_HANDLE)& _MarginServerCommunication___RpcClientInterface;

extern const MIDL_STUB_DESC _MarginServerCommunication_StubDesc;

static RPC_BINDING_HANDLE _MarginServerCommunication__MIDL_AutoBindHandle;


/* [fault_status][comm_status] */ error_status_t RPC_Ping( 
    /* [in] */ handle_t Binding)
{

    RPC_BINDING_HANDLE _Handle	=	0;
    
    error_status_t _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    RPC_STATUS _Status;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    RpcTryExcept
        {
        RpcTryFinally
            {
            NdrClientInitializeNew(
                          ( PRPC_MESSAGE  )&_RpcMessage,
                          ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                          ( PMIDL_STUB_DESC  )&_MarginServerCommunication_StubDesc,
                          0);
            
            
            _Handle = Binding;
            
            
            _StubMsg.BufferLength = 0U;
            NdrGetBuffer( (PMIDL_STUB_MESSAGE) &_StubMsg, _StubMsg.BufferLength, _Handle );
            
            NdrSendReceive( (PMIDL_STUB_MESSAGE) &_StubMsg, (unsigned char __RPC_FAR *) _StubMsg.Buffer );
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0] );
            
            _RetVal = *(( error_status_t __RPC_FAR * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrFreeBuffer( (PMIDL_STUB_MESSAGE) &_StubMsg );
            
            }
        RpcEndFinally
        
        }
    RpcExcept( 1 )
        {
        if(_Status = NdrMapCommAndFaultStatus(( PMIDL_STUB_MESSAGE  )&_StubMsg,( unsigned long __RPC_FAR * )&_RetVal,( unsigned long __RPC_FAR * )&_RetVal,RpcExceptionCode()))
            {
            RpcRaiseException(_Status);
            }
        }
    RpcEndExcept
    return _RetVal;
}


/* [fault_status][comm_status] */ error_status_t RPC_CheckSecurity( 
    /* [in] */ handle_t Binding)
{

    RPC_BINDING_HANDLE _Handle	=	0;
    
    error_status_t _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    RPC_STATUS _Status;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    RpcTryExcept
        {
        RpcTryFinally
            {
            NdrClientInitializeNew(
                          ( PRPC_MESSAGE  )&_RpcMessage,
                          ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                          ( PMIDL_STUB_DESC  )&_MarginServerCommunication_StubDesc,
                          1);
            
            
            _Handle = Binding;
            
            
            _StubMsg.BufferLength = 0U;
            NdrGetBuffer( (PMIDL_STUB_MESSAGE) &_StubMsg, _StubMsg.BufferLength, _Handle );
            
            NdrSendReceive( (PMIDL_STUB_MESSAGE) &_StubMsg, (unsigned char __RPC_FAR *) _StubMsg.Buffer );
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0] );
            
            _RetVal = *(( error_status_t __RPC_FAR * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrFreeBuffer( (PMIDL_STUB_MESSAGE) &_StubMsg );
            
            }
        RpcEndFinally
        
        }
    RpcExcept( 1 )
        {
        if(_Status = NdrMapCommAndFaultStatus(( PMIDL_STUB_MESSAGE  )&_StubMsg,( unsigned long __RPC_FAR * )&_RetVal,( unsigned long __RPC_FAR * )&_RetVal,RpcExceptionCode()))
            {
            RpcRaiseException(_Status);
            }
        }
    RpcEndExcept
    return _RetVal;
}


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
    /* [in] */ int nMessageId)
{

    RPC_BINDING_HANDLE _Handle	=	0;
    
    error_status_t _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    RPC_STATUS _Status;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    RpcTryExcept
        {
        RpcTryFinally
            {
            NdrClientInitializeNew(
                          ( PRPC_MESSAGE  )&_RpcMessage,
                          ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                          ( PMIDL_STUB_DESC  )&_MarginServerCommunication_StubDesc,
                          2);
            
            
            _Handle = Binding;
            
            
            _StubMsg.BufferLength = 0U + 4U + 4U + 4U + 4U + 4U + 4U + 4U + 4U + 4U;
            NdrGetBuffer( (PMIDL_STUB_MESSAGE) &_StubMsg, _StubMsg.BufferLength, _Handle );
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nSourceId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nSourceMaskId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nSourceObjectId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nTargetId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nTargetMaskId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nTargetObjectId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nCallType;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nTimeout;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nMessageId;
            
            NdrSendReceive( (PMIDL_STUB_MESSAGE) &_StubMsg, (unsigned char __RPC_FAR *) _StubMsg.Buffer );
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4] );
            
            _RetVal = *(( error_status_t __RPC_FAR * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrFreeBuffer( (PMIDL_STUB_MESSAGE) &_StubMsg );
            
            }
        RpcEndFinally
        
        }
    RpcExcept( 1 )
        {
        if(_Status = NdrMapCommAndFaultStatus(( PMIDL_STUB_MESSAGE  )&_StubMsg,( unsigned long __RPC_FAR * )&_RetVal,( unsigned long __RPC_FAR * )&_RetVal,RpcExceptionCode()))
            {
            RpcRaiseException(_Status);
            }
        }
    RpcEndExcept
    return _RetVal;
}


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
    /* [in] */ int nInCch)
{

    RPC_BINDING_HANDLE _Handle	=	0;
    
    error_status_t _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    RPC_STATUS _Status;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    RpcTryExcept
        {
        if(!lpInData)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            NdrClientInitializeNew(
                          ( PRPC_MESSAGE  )&_RpcMessage,
                          ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                          ( PMIDL_STUB_DESC  )&_MarginServerCommunication_StubDesc,
                          3);
            
            
            _Handle = Binding;
            
            
            _StubMsg.BufferLength = 0U + 4U + 4U + 4U + 4U + 4U + 4U + 4U + 4U + 4U + 4U + 11U;
            _StubMsg.MaxCount = nInCch;
            
            NdrConformantArrayBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                          (unsigned char __RPC_FAR *)lpInData,
                                          (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
            
            NdrGetBuffer( (PMIDL_STUB_MESSAGE) &_StubMsg, _StubMsg.BufferLength, _Handle );
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nSourceId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nSourceMaskId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nSourceObjectId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nTargetId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nTargetMaskId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nTargetObjectId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nCallType;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nTimeout;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nMessageId;
            
            _StubMsg.MaxCount = nInCch;
            
            NdrConformantArrayMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                        (unsigned char __RPC_FAR *)lpInData,
                                        (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
            
            _StubMsg.Buffer = (unsigned char __RPC_FAR *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nInCch;
            
            NdrSendReceive( (PMIDL_STUB_MESSAGE) &_StubMsg, (unsigned char __RPC_FAR *) _StubMsg.Buffer );
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[26] );
            
            _RetVal = *(( error_status_t __RPC_FAR * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrFreeBuffer( (PMIDL_STUB_MESSAGE) &_StubMsg );
            
            }
        RpcEndFinally
        
        }
    RpcExcept( 1 )
        {
        if(_Status = NdrMapCommAndFaultStatus(( PMIDL_STUB_MESSAGE  )&_StubMsg,( unsigned long __RPC_FAR * )&_RetVal,( unsigned long __RPC_FAR * )&_RetVal,RpcExceptionCode()))
            {
            RpcRaiseException(_Status);
            }
        }
    RpcEndExcept
    return _RetVal;
}


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
    /* [out] */ int __RPC_FAR *pOutCch)
{

    RPC_BINDING_HANDLE _Handle	=	0;
    
    error_status_t _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    RPC_STATUS _Status;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    RpcTryExcept
        {
        if(!lpInData)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!lpOutData)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!pOutCch)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            NdrClientInitializeNew(
                          ( PRPC_MESSAGE  )&_RpcMessage,
                          ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                          ( PMIDL_STUB_DESC  )&_MarginServerCommunication_StubDesc,
                          4);
            
            
            _Handle = Binding;
            
            
            _StubMsg.BufferLength = 0U + 4U + 4U + 4U + 4U + 4U + 4U + 4U + 4U + 4U + 4U + 11U;
            _StubMsg.MaxCount = nInCch;
            
            NdrConformantArrayBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                          (unsigned char __RPC_FAR *)lpInData,
                                          (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
            
            NdrGetBuffer( (PMIDL_STUB_MESSAGE) &_StubMsg, _StubMsg.BufferLength, _Handle );
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nSourceId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nSourceMaskId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nSourceObjectId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nTargetId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nTargetMaskId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nTargetObjectId;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nCallType;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nTimeout;
            
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nMessageId;
            
            _StubMsg.MaxCount = nInCch;
            
            NdrConformantArrayMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                        (unsigned char __RPC_FAR *)lpInData,
                                        (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
            
            _StubMsg.Buffer = (unsigned char __RPC_FAR *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( int __RPC_FAR * )_StubMsg.Buffer)++ = nInCch;
            
            NdrSendReceive( (PMIDL_STUB_MESSAGE) &_StubMsg, (unsigned char __RPC_FAR *) _StubMsg.Buffer );
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[54] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char __RPC_FAR * __RPC_FAR *)&lpOutData,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[16],
                                  (unsigned char)0 );
            
            _StubMsg.Buffer = (unsigned char __RPC_FAR *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *pOutCch = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            _RetVal = *(( error_status_t __RPC_FAR * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrFreeBuffer( (PMIDL_STUB_MESSAGE) &_StubMsg );
            
            }
        RpcEndFinally
        
        }
    RpcExcept( 1 )
        {
        if(_Status = NdrMapCommAndFaultStatus(( PMIDL_STUB_MESSAGE  )&_StubMsg,( unsigned long __RPC_FAR * )&_RetVal,( unsigned long __RPC_FAR * )&_RetVal,RpcExceptionCode()))
            {
            RpcRaiseException(_Status);
            }
        }
    RpcEndExcept
    return _RetVal;
}


static const MIDL_STUB_DESC _MarginServerCommunication_StubDesc = 
    {
    (void __RPC_FAR *)& _MarginServerCommunication___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &RisingServerCenter_IfHandle,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x10001, /* Ndr library version */
    0,
    0x50100a4, /* MIDL Version 5.1.164 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    1,  /* Flags */
    0,  /* Reserved3 */
    0,  /* Reserved4 */
    0   /* Reserved5 */
    };

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {
			0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xf,		/* FC_IGNORE */
/*  2 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x10,		/* FC_ERROR_STATUS_T */
/*  4 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xf,		/* FC_IGNORE */
/*  6 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/*  8 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 10 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 12 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 14 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 16 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 18 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 20 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 22 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 24 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x10,		/* FC_ERROR_STATUS_T */
/* 26 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xf,		/* FC_IGNORE */
/* 28 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 30 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 32 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 34 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 36 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 38 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 40 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 42 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 44 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 46 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 48 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 50 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 52 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x10,		/* FC_ERROR_STATUS_T */
/* 54 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xf,		/* FC_IGNORE */
/* 56 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 58 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 60 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 62 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 64 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 66 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 68 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 70 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 72 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 74 */	
			0x4d,		/* FC_IN_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 76 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 78 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 80 */	
			0x51,		/* FC_OUT_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 82 */	NdrFcShort( 0x10 ),	/* Type Offset=16 */
/* 84 */	
			0x51,		/* FC_OUT_PARAM */
#ifndef _ALPHA_
			0x1,		/* x86, MIPS & PPC Stack size = 1 */
#else
			0x2,		/* Alpha Stack size = 2 */
#endif
/* 86 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */
/* 88 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x10,		/* FC_ERROR_STATUS_T */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x0,	/* FC_RP */
/*  4 */	NdrFcShort( 0x2 ),	/* Offset= 2 (6) */
/*  6 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/*  8 */	NdrFcShort( 0x1 ),	/* 1 */
/* 10 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
#ifndef _ALPHA_
/* 12 */	NdrFcShort( 0x2c ),	/* x86, MIPS, PPC Stack size/offset = 44 */
#else
			NdrFcShort( 0x58 ),	/* Alpha Stack size/offset = 88 */
#endif
/* 14 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 16 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] */
/* 18 */	NdrFcShort( 0x2 ),	/* Offset= 2 (20) */
/* 20 */	
			0x12, 0x0,	/* FC_UP */
/* 22 */	NdrFcShort( 0x2 ),	/* Offset= 2 (24) */
/* 24 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 26 */	NdrFcShort( 0x1 ),	/* 1 */
/* 28 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x54,		/* FC_DEREFERENCE */
#ifndef _ALPHA_
/* 30 */	NdrFcShort( 0x34 ),	/* x86, MIPS, PPC Stack size/offset = 52 */
#else
			NdrFcShort( 0x68 ),	/* Alpha Stack size/offset = 104 */
#endif
/* 32 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 34 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 36 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

			0x0
        }
    };
