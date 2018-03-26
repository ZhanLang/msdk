
#pragma once
#include <memory>
#include "utility.h"
//注意：该类为智能句柄类，如果已将一句柄复制，请勿再次调用CloseHandle
namespace msdk {
namespace UTIL { ;
template<class _Ptr , class _Traits = default_sentry>
class CShareHandle
{
public:
	inline CShareHandle(_Ptr handle = NULL)
	{
		m_ptr = std::tr1::shared_ptr<CShareHandlePtr<_Ptr , _Traits>>(new CShareHandlePtr<_Ptr , _Traits>(handle));
	}

	inline CShareHandle(const CShareHandle& handle)
	{
		this->m_ptr = handle.m_ptr;
	}


	inline ~CShareHandle()
	{
		this->m_ptr = std::tr1::shared_ptr<CShareHandlePtr<_Ptr , _Traits>>();
	}

	inline CShareHandle& operator=(const CShareHandle& handle)
	{
		this->m_ptr = handle.m_ptr;
		return *this;
	}

	inline operator _Ptr() const
	{
		if (m_ptr.get()){
			return m_ptr->m_p;
		}

		return (_Ptr)NULL;
	}

	inline _Ptr operator ->() const
	{
		if (m_ptr.get()){
			return m_ptr->m_p;
		}

		return (_Ptr)NULL;
	}

	inline _Ptr detach()
	{
		if (m_ptr.get()){
			return m_ptr->detach();
		}

		return NULL;
	}

	inline _OLD _Ptr attach(const _Ptr& handle)
	{
		if (m_ptr.get()){
			return m_ptr->attach(handle);
		}

		return NULL;
	}

	inline void reset()
	{
		if (m_ptr.get()){
			m_ptr->reset();
		}
	}

private:
	template<class _Ptr , class _Traits>
	struct CShareHandlePtr
	{
	public:
		inline CShareHandlePtr(_Ptr h): m_p(h)
		{

		}

		inline CShareHandlePtr()
		{
			m_p = (_Ptr)m_tr.default_value();
		}

		inline ~CShareHandlePtr()
		{
			m_tr.destroy(m_p);
		}

		inline _Ptr detach()
		{
			_Ptr tmp = m_p;
			m_p = (_Ptr)m_tr.default_value();
			return tmp;
		}

		inline _OLD _Ptr attach(const _Ptr& handle)
		{
			if (!m_tr.equal_to(m_p , handle))
			{
				_Ptr tmp = detach();
				m_p = handle;
				return tmp;
			}

			return m_p;
		}

		inline void reset()
		{
			m_p = (_Ptr)m_tr.default_value();
		}
	public:
		_Ptr m_p;
		_Traits m_tr;
	};
private:
	std::tr1::shared_ptr<CShareHandlePtr<_Ptr , _Traits>> m_ptr;
};


struct share_handle_sentry
{
	static inline  HANDLE default_value(){return 0;}
	static inline  bool equal_to(HANDLE l, HANDLE r){return l == r;}
	static inline  void destroy(HANDLE h){if(h)::CloseHandle(h);}
};

typedef  CShareHandle<HANDLE , share_handle_sentry> CAutoHandle;
} // namespace UTIL-
} //namespace msdk