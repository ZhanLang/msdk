/* this ALWAYS GENERATED file contains the RPC server stubs */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu May 22 13:52:17 2008
 */
/* Compiler settings for icenter.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )

#include <string.h>
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


extern RPC_DISPATCH_TABLE _MarginServerCommunication_v1_0_DispatchTable;

static const RPC_SERVER_INTERFACE _MarginServerCommunication___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0xFF37DF02,0xE8B0,0x4db8,{0x93,0xD1,0xFD,0x2F,0x10,0x24,0xD3,0xE1}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    &_MarginServerCommunication_v1_0_DispatchTable,
    0,
    0,
    0,
    0,
    0
    };
RPC_IF_HANDLE _MarginServerCommunication_v1_0_s_ifspec = (RPC_IF_HANDLE)& _MarginServerCommunication___RpcServerInterface;

extern const MIDL_STUB_DESC _MarginServerCommunication_StubDesc;

void __RPC_STUB
_MarginServerCommunication_Ping(
    PRPC_MESSAGE _pRpcMessage )
{
    handle_t Binding;
    error_status_t _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &_MarginServerCommunication_StubDesc);
    
    Binding = _pRpcMessage->Handle;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0] );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = Ping(Binding);
        
        _StubMsg.BufferLength = 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( error_status_t __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
_MarginServerCommunication_CheckSecurity(
    PRPC_MESSAGE _pRpcMessage )
{
    handle_t Binding;
    error_status_t _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &_MarginServerCommunication_StubDesc);
    
    Binding = _pRpcMessage->Handle;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0] );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = CheckSecurity(Binding);
        
        _StubMsg.BufferLength = 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( error_status_t __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
_MarginServerCommunication_ServerCall(
    PRPC_MESSAGE _pRpcMessage )
{
    handle_t Binding;
    error_status_t _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    int nCallType;
    int nMessageId;
    int nSourceId;
    int nSourceMaskId;
    int nSourceObjectId;
    int nTargetId;
    int nTargetMaskId;
    int nTargetObjectId;
    int nTimeout;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &_MarginServerCommunication_StubDesc);
    
    Binding = _pRpcMessage->Handle;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[4] );
            
            nSourceId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nSourceMaskId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nSourceObjectId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nTargetId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nTargetMaskId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nTargetObjectId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nCallType = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nTimeout = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nMessageId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = ServerCall(
                     Binding,
                     nSourceId,
                     nSourceMaskId,
                     nSourceObjectId,
                     nTargetId,
                     nTargetMaskId,
                     nTargetObjectId,
                     nCallType,
                     nTimeout,
                     nMessageId);
        
        _StubMsg.BufferLength = 4U;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( error_status_t __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
_MarginServerCommunication_ServerCall1(
    PRPC_MESSAGE _pRpcMessage )
{
    handle_t Binding;
    error_status_t _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    byte __RPC_FAR *lpInData;
    int nCallType;
    int nInCch;
    int nMessageId;
    int nSourceId;
    int nSourceMaskId;
    int nSourceObjectId;
    int nTargetId;
    int nTargetMaskId;
    int nTargetObjectId;
    int nTimeout;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &_MarginServerCommunication_StubDesc);
    
    Binding = _pRpcMessage->Handle;
    ( byte __RPC_FAR * )lpInData = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[26] );
            
            nSourceId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nSourceMaskId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nSourceObjectId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nTargetId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nTargetMaskId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nTargetObjectId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nCallType = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nTimeout = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nMessageId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            NdrConformantArrayUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                          (unsigned char __RPC_FAR * __RPC_FAR *)&lpInData,
                                          (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6],
                                          (unsigned char)0 );
            
            _StubMsg.Buffer = (unsigned char __RPC_FAR *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            nInCch = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = ServerCall1(
                      Binding,
                      nSourceId,
                      nSourceMaskId,
                      nSourceObjectId,
                      nTargetId,
                      nTargetMaskId,
                      nTargetObjectId,
                      nCallType,
                      nTimeout,
                      nMessageId,
                      lpInData,
                      nInCch);
        
        _StubMsg.BufferLength = 4U;
        _StubMsg.BufferLength += 16;
        
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        *(( error_status_t __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
_MarginServerCommunication_ServerCall2(
    PRPC_MESSAGE _pRpcMessage )
{
    handle_t Binding;
    byte __RPC_FAR *_M3;
    int _M4;
    error_status_t _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    byte __RPC_FAR *lpInData;
    byte __RPC_FAR *__RPC_FAR *lpOutData;
    int nCallType;
    int nInCch;
    int nMessageId;
    int nSourceId;
    int nSourceMaskId;
    int nSourceObjectId;
    int nTargetId;
    int nTargetMaskId;
    int nTargetObjectId;
    int nTimeout;
    int __RPC_FAR *pOutCch;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &_MarginServerCommunication_StubDesc);
    
    Binding = _pRpcMessage->Handle;
    ( byte __RPC_FAR * )lpInData = 0;
    ( byte __RPC_FAR *__RPC_FAR * )lpOutData = 0;
    ( int __RPC_FAR * )pOutCch = 0;
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[54] );
            
            nSourceId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nSourceMaskId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nSourceObjectId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nTargetId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nTargetMaskId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nTargetObjectId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nCallType = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nTimeout = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            nMessageId = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            NdrConformantArrayUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                          (unsigned char __RPC_FAR * __RPC_FAR *)&lpInData,
                                          (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6],
                                          (unsigned char)0 );
            
            _StubMsg.Buffer = (unsigned char __RPC_FAR *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            nInCch = *(( int __RPC_FAR * )_StubMsg.Buffer)++;
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        lpOutData = &_M3;
        _M3 = 0;
        pOutCch = &_M4;
        
        _RetVal = ServerCall2(
                      Binding,
                      nSourceId,
                      nSourceMaskId,
                      nSourceObjectId,
                      nTargetId,
                      nTargetMaskId,
                      nTargetObjectId,
                      nCallType,
                      nTimeout,
                      nMessageId,
                      lpInData,
                      nInCch,
                      lpOutData,
                      pOutCch);
        
        _StubMsg.BufferLength = 8U + 11U + 7U;
        _StubMsg.MaxCount = pOutCch ? *pOutCch : 0;
        
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char __RPC_FAR *)lpOutData,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[16] );
        
        _StubMsg.BufferLength += 16;
        
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *) _pRpcMessage->Buffer;
        
        _StubMsg.MaxCount = pOutCch ? *pOutCch : 0;
        
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char __RPC_FAR *)lpOutData,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[16] );
        
        _StubMsg.Buffer = (unsigned char __RPC_FAR *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( int __RPC_FAR * )_StubMsg.Buffer)++ = *pOutCch;
        
        *(( error_status_t __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        _StubMsg.MaxCount = pOutCch ? *pOutCch : 0;
        
        NdrPointerFree( &_StubMsg,
                        (unsigned char __RPC_FAR *)lpOutData,
                        &__MIDL_TypeFormatString.Format[16] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}


static const MIDL_STUB_DESC _MarginServerCommunication_StubDesc = 
    {
    (void __RPC_FAR *)& _MarginServerCommunication___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
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

static RPC_DISPATCH_FUNCTION _MarginServerCommunication_table[] =
    {
    _MarginServerCommunication_Ping,
    _MarginServerCommunication_CheckSecurity,
    _MarginServerCommunication_ServerCall,
    _MarginServerCommunication_ServerCall1,
    _MarginServerCommunication_ServerCall2,
    0
    };
RPC_DISPATCH_TABLE _MarginServerCommunication_v1_0_DispatchTable = 
    {
    5,
    _MarginServerCommunication_table
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
