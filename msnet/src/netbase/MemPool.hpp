#ifndef __MEMPOOL_HPP__
#define __MEMPOOL_HPP__

#include <boost/pool/object_pool.hpp>
#include <boost/pool/detail/guard.hpp>
////////////////////////////////////////////////////////////////////////////////
//
template <typename T, 
		typename UserAllocator = boost::default_user_allocator_new_delete, 
		typename Mutex = boost::details::pool::default_mutex>
class Sync_object_bool : public boost::object_pool<T, UserAllocator>
{
public:
	using boost::object_pool<T,UserAllocator>::element_type;
	using boost::object_pool<T,UserAllocator>::size_type;

    // This constructor parameter is an extension!
	explicit Sync_object_bool(const size_type next_size = 32)
	:boost::object_pool<T, UserAllocator>(next_size){}

    element_type * construct()
    {
		boost::details::pool::guard<Mutex> g(m_Mutex);
		return boost::object_pool<T,UserAllocator>::construct();
    }

    void destroy(element_type * const chunk)
    {
		boost::details::pool::guard<Mutex> g(m_Mutex);
		boost::object_pool<T,UserAllocator>::destroy(chunk);
    }

private:
	Mutex m_Mutex;
};

#endif //__MEMPOOL_HPP__