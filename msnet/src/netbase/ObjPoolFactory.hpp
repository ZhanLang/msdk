#ifndef __OBJPOOLFACTORY_HPP__
#define __OBJPOOLFACTORY_HPP__

#include "MemPool.hpp"

////////////////////////////////////////////////////////////////////////////////
template <typename T>
class CObjPoolFactory
{
public:
	typedef Sync_object_bool<T> Objpool_type;

	static T* Alloc()
	{
		return s_ObjPool.construct();
	}

	static void Free(T * const ptr)
	{
		s_ObjPool.destroy(ptr);
	}


private:
	CObjPoolFactory();
	~CObjPoolFactory();

private:
	static Objpool_type s_ObjPool;
};

template <typename T>
typename CObjPoolFactory<T>::Objpool_type
CObjPoolFactory<T>::s_ObjPool;
////////////////////////////////////////////////////////////////////////////////
#endif //__OBJPOOLFACTORY_HPP__