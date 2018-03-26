#pragma once

template <class T> class CStlAlloc
{
public:
	typedef T                 value_type;
	typedef value_type*       pointer;
	typedef const value_type* const_pointer;
	typedef value_type&       reference;
	typedef const value_type& const_reference;
	typedef std::size_t       size_type;
	typedef std::ptrdiff_t    difference_type;

	template <class U>
	struct rebind { typedef CStlAlloc<U> other; };

	CStlAlloc() {}

	CStlAlloc(const CStlAlloc&) {}

	template <class U>
	CStlAlloc(const CStlAlloc<U>&) {}

	~CStlAlloc() {}

	pointer address(reference x) const{
		return &x;
	}

	const_pointer address(const_reference x) const{
		return x;
	}

	pointer allocate(size_type n, const_pointer = 0){
		void* p = malloc(n*sizeof(T));

		if (!p)
			throw std::bad_alloc();

		return static_cast<pointer>(p);
	}

	void deallocate(pointer p, size_type){
		if (p)
			free(p);
	}

	size_type max_size() const{
		return static_cast<size_type>(-1) / sizeof(T);
	}

	void construct(pointer p, const value_type& x){
		new(p) value_type(x);
	}

	void destroy(pointer p){
		p->~value_type();
	}

//private:
	void operator=(const CStlAlloc&);

};


template<> class CStlAlloc<void>
{
	typedef void        value_type;
	typedef void*       pointer;
	typedef const void* const_pointer;
	template <class U>
	struct rebind { typedef CStlAlloc<U> other; };
};


template <class T>
inline bool operator==(const CStlAlloc<T>&, const CStlAlloc<T>&)
{
	return true;
};

template <class T>
inline bool operator!=(const CStlAlloc<T>&, const CStlAlloc<T>&)
{
	return false;
};