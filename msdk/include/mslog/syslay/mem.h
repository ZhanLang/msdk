//
// 项目名称: 
//
// 实现功能: 基于先进先出栈的堆内存分配器，具有很好的分配性能和无内存碎片的特点
//
// 文件名称: mem.h
//
// 创建作者: 
//
// 创建日期: 
//
// 修改作者:
//
// 修改日期: 
//
// （如有新的修改人请在这里添加）
//
// 使用说明: 注意内存分配器初始分配内存的实际大小“不小于1M”
//
//=============================================================================

#ifndef _MEM_INCLUDE_H_
#define _MEM_INCLUDE_H_
#ifdef _WIN32
#pragma once
#endif


#include <stddef.h>
#include "mslog/Syslay/platform.h"

#ifdef SYSLAY_DLL_EXPORT
#  define MEM_INTERFACE DLL_EXPORT
#else
#  define MEM_INTERFACE DLL_IMPORT
#endif


//-----------------------------------------------------------------------------
// 目的 : 分配内存
// 输入 : nMemSize - 大小
// 输出 : MEM_INTERFACE void * - 内存指针
//-----------------------------------------------------------------------------
MEM_INTERFACE void * MemAllocStack( int nMemSize );

//-----------------------------------------------------------------------------
// 目的 : 释放最近申请的内存
// 输出 : MEM_INTERFACE void - 
//-----------------------------------------------------------------------------
MEM_INTERFACE void MemFreeStack();


#ifdef _LINUX
MEM_INTERFACE void ZeroMemory( void *mem, size_t length );
#endif


#endif // _MEM_INCLUDE_H_
