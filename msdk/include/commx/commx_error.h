// rsct_error.h

#if !defined(_INCLUDE_RSCT_ERROR_H__)
#define _INCLUDE_RSCT_ERROR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace msdk {

/*
// 标准的rpc调用
	#define RPC_S_OK                          ERROR_SUCCESS
	#define RPC_S_INVALID_ARG                 ERROR_INVALID_PARAMETER
	#define RPC_S_OUT_OF_MEMORY               ERROR_OUTOFMEMORY
	#define RPC_S_OUT_OF_THREADS              ERROR_MAX_THRDS_REACHED
	#define RPC_S_INVALID_LEVEL               ERROR_INVALID_PARAMETER
	#define RPC_S_BUFFER_TOO_SMALL            ERROR_INSUFFICIENT_BUFFER
	#define RPC_S_INVALID_SECURITY_DESC       ERROR_INVALID_SECURITY_DESCR
	#define RPC_S_ACCESS_DENIED               ERROR_ACCESS_DENIED
	#define RPC_S_SERVER_OUT_OF_MEMORY        ERROR_NOT_ENOUGH_SERVER_MEMORY
	#define RPC_S_ASYNC_CALL_PENDING          ERROR_IO_PENDING

	#define RPC_X_NO_MEMORY                   RPC_S_OUT_OF_MEMORY
	#define RPC_X_INVALID_BOUND               RPC_S_INVALID_BOUND
	#define RPC_X_INVALID_TAG                 RPC_S_INVALID_TAG
	#define RPC_X_ENUM_VALUE_TOO_LARGE        RPC_X_ENUM_VALUE_OUT_OF_RANGE
	#define RPC_X_SS_CONTEXT_MISMATCH         ERROR_INVALID_HANDLE
	#define RPC_X_INVALID_BUFFER              ERROR_INVALID_USER_BUFFER
	#define RPC_X_PIPE_APP_MEMORY             ERROR_OUTOFMEMORY
	#define RPC_X_INVALID_PIPE_OPERATION      RPC_X_WRONG_PIPE_ORDER
*/

enum
{
	E_COMMX_ADD_EXIST_ONE =						-1, //通讯对象名字冲突
	E_COMMX_OUTOF_PROCESS_BLOCK =				-2, //太多的进程，只支持有限个数的进程
	E_COMMX_OUTOF_OBJECTS_BLOCK =				-3, //太多的通讯对象，只支持有限个数的通讯对象
	E_COMMX_OPEN_SHARE_FILE =					-4, //打开文件共享内存映射失败
	E_COMMX_OPEN_SHARE_MUTEX =					-5, //打开机器唯一互斥对象失败
	E_COMMX_START_RPC_SERVICE =					-6, //启动RPC服务失败
	E_COMMX_CALL_RPC_SERVICE =					-7, //调用RPC服务失败
	E_COMMX_NO_FOUND_OBJECT =					-8, //通讯对象不存在
	E_COMMX_ERROR_SERVERNAME =					-9, //通讯对象服务名字错误
	
};

} //namespace msdk

#endif // !defined(_INCLUDE_RSCT_ERROR_H__)
