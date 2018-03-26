
#ifndef _FILESTREAM_H_
#define _FILESTREAM_H_
#include "stream_i.h"

template<typename LockType = CNullCriticalSection>
class CFileStream
	:public IMsFileStream
	,private CUnknownImp
{
public:

	UNKNOWN_IMP3_(IMsFileStream,IMsStream,IMsSequentialStream);
	STDMETHOD(init_class)(IMSBase*, IMSBase*)
	{
		return S_OK;
	}

	typedef CStackLockWrapper<LockType> SRVATUOLOCK;

	CFileStream();
	~CFileStream();

	//IFileStream
	STDMETHOD(Open)(LPCWSTR lpFileName,DWORD dwCreationDisposition = OPEN_ALWAYS);
	STDMETHOD(Close)();
	STDMETHOD(FlushFileBuffers)();
	//IStream
	STDMETHOD(Seek)(IN LARGE_INTEGER dlibMove, IN SeekOrigin dwOrigin, OUT LARGE_INTEGER *plibNewPosition);
	STDMETHOD(SetSize)(IN LARGE_INTEGER libNewSize);
	STDMETHOD_(LARGE_INTEGER,GetSize)();
	STDMETHOD_(LARGE_INTEGER,GetDataSize)();
	STDMETHOD(Revert)();
	STDMETHOD(Clone)(IMsStream**pClone);

	//ISequentialStream
	STDMETHOD(Read)(OUT VOID*pv,IN ULONG cb, OUT ULONG* pcbRead);
	STDMETHOD(Write)(IN CONST VOID* pv, IN ULONG cb, OUT ULONG *pcbWritten);
private:
	UTIL::sentry<HANDLE, UTIL::handle_sentry> m_hFileHandle;
	LockType m_lock;
};

template<typename LockType>
CFileStream<LockType>::CFileStream()
{

}

template<typename LockType>
CFileStream<LockType>::~CFileStream()
{
	Close();
}

template<typename LockType>
STDMETHODIMP CFileStream<LockType>::Open(LPCWSTR lpFileName,DWORD dwCreationDisposition /*= OPEN_ALWAYS*/)
{
	SRVATUOLOCK lock(m_lock);
	RASSERT(lpFileName,E_FAIL);
	RASSERT(!m_hFileHandle, S_FALSE);

	m_hFileHandle = CreateFile(lpFileName
		,GENERIC_READ|GENERIC_WRITE
		,FILE_SHARE_READ|FILE_SHARE_WRITE
		,NULL,dwCreationDisposition
		,FILE_ATTRIBUTE_NORMAL
		,NULL);

	return (m_hFileHandle && m_hFileHandle != INVALID_HANDLE_VALUE) ? S_OK : E_FAIL;
}

template<typename LockType>
STDMETHODIMP CFileStream<LockType>::Close()
{
	SRVATUOLOCK lock(m_lock);
	m_hFileHandle = NULL;
	return S_OK;
}

template<typename LockType>
STDMETHODIMP CFileStream<LockType>::Seek(IN LARGE_INTEGER dlibMove, IN SeekOrigin dwOrigin, OUT LARGE_INTEGER *plibNewPosition)
{
	SRVATUOLOCK lock(m_lock);
	return SetFilePointerEx(m_hFileHandle, dlibMove,plibNewPosition,dwOrigin) ? S_OK : E_FAIL;
}

template<typename LockType>
STDMETHODIMP CFileStream<LockType>::SetSize(IN LARGE_INTEGER libNewSize)
{
	return E_NOTIMPL;
}


template<typename LockType>
STDMETHODIMP_(LARGE_INTEGER) CFileStream<LockType>::GetSize()
{
	LARGE_INTEGER ularge = {0};
	return ularge;
}


template<typename LockType>
STDMETHODIMP_(LARGE_INTEGER) CFileStream<LockType>::GetDataSize()
{
	LARGE_INTEGER ularge = {0};
	SRVATUOLOCK lock(m_lock);
	RASSERT(m_hFileHandle && m_hFileHandle != INVALID_HANDLE_VALUE, ularge);

	
	BOOL bRet = GetFileSizeEx(m_hFileHandle,&ularge);
	return ularge;
}


template<typename LockType>
STDMETHODIMP CFileStream<LockType>::Revert()
{
	SRVATUOLOCK lock(m_lock);
	RASSERT(m_hFileHandle && m_hFileHandle != INVALID_HANDLE_VALUE, E_FAIL);
	SetFilePointer(m_hFileHandle,0,NULL,FILE_BEGIN);
	return SetEndOfFile(m_hFileHandle) ? S_OK : E_FAIL;
}


template<typename LockType>
STDMETHODIMP CFileStream<LockType>::Clone(IMsStream**pClone)
{
	return E_NOTIMPL;
}


template<typename LockType>
STDMETHODIMP CFileStream<LockType>::Read(OUT VOID*pv,IN ULONG cb, OUT ULONG* pcbRead)
{
	SRVATUOLOCK lock(m_lock);
	RASSERT(m_hFileHandle && m_hFileHandle != INVALID_HANDLE_VALUE && pv, E_FAIL);
	return ReadFile(m_hFileHandle, pv, cb, pcbRead,NULL) ? S_OK:E_FAIL;
}


template<typename LockType>
STDMETHODIMP CFileStream<LockType>::Write(IN CONST VOID* pv, IN ULONG cb, OUT ULONG *pcbWritten)
{
	SRVATUOLOCK lock(m_lock);
	RASSERT(m_hFileHandle && m_hFileHandle != INVALID_HANDLE_VALUE && pv, E_FAIL);
	return WriteFile(m_hFileHandle, pv, cb,pcbWritten,NULL) ? S_OK:E_FAIL;
}

template<typename LockType>
STDMETHODIMP CFileStream<LockType>::FlushFileBuffers()
{
	SRVATUOLOCK lock(m_lock);
	RASSERT(m_hFileHandle && m_hFileHandle != INVALID_HANDLE_VALUE, E_FAIL);
	return ::FlushFileBuffers(m_hFileHandle) ? S_OK : E_FAIL;
}

#endif//_FILESTREAM_H_

