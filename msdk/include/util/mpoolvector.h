#pragma once

#ifndef _VECTOR_INCLUDE_H_
#define _VECTOR_INCLUDE_H_

namespace msdk{;

template<class T>
class CPoolVectorBaseImp
{
public:
	CPoolVectorBaseImp(size_t itemSize);
	virtual ~CPoolVectorBaseImp();
	void ClearAndFree();
	inline DWORD Size() const ;
	inline bool IsEmpty() const ;
	inline bool Reserve(DWORD newCapacity);
	inline void ReserveDown();
	virtual void Delete(DWORD index, DWORD num = 1);
	virtual void Clear();
	void DeleteFrom(DWORD index);
	void DeleteBack();
	T* GetPoint() const;
protected:
	inline void ReserveOnePosition();
	inline void InsertOneItem(DWORD index);
	inline void TestIndexAndCorrectNum(DWORD index, DWORD &num) const;

public:
	DWORD _capacity;
	DWORD _size;
	void *_items;
	size_t _itemSize;
	const int _step;
	HANDLE _heap;
private:
	void MoveItems(DWORD destIndex, DWORD srcIndex);
};


template<typename T>
class  CPoolVectorBase: public CPoolVectorBaseImp<T>
{
public:
	CPoolVectorBase();
	CPoolVectorBase(const CPoolVectorBase &v);
	CPoolVectorBase& operator=(const CPoolVectorBase &v);
	CPoolVectorBase& operator+=(const CPoolVectorBase &v);

	inline int Add(T item);
	inline void Insert(DWORD index, T item);
	
	inline const T& operator[](DWORD index) const ;
	inline T& operator[](DWORD index) ;
	inline T* GetAt(DWORD index);
	const T& Front() const ;
	T& Front() ;
	const T& Back() const ;
	T& Back() ;
	void Swap(DWORD i, DWORD j);
	inline int FindInSorted(const T& item, int left, int right) const;
	inline int FindInSorted(const T& item) const;
	inline int AddToUniqueSorted(const T& item);

	//
	inline int AddToSorted(const T& item, int (*compare)(const T*, const T*, void *), void* param);
	int FindInSorted(const T& item, int (*compare)(const T*, const T*, void *), void* param, int* count);

	inline static void SortRefDown(T* p, int k, int size, int (*compare)(const T*, const T*, void *), void *param);
	void Sort(int (*compare)(const T*, const T*, void *), void *param);

	

};

template <class T>
class CPoolVector: public CPoolVectorBase<void*>
{
public:
	CPoolVector() ;
	~CPoolVector() ;
	

	inline const T& operator[](DWORD index) const ;
	inline T& operator[](DWORD index) ;

	T& Front() ;
	const T& Front() const ;
	T& Back() ;
	const T& Back() const ;

	inline int Add(const T& item) ;
	inline void Insert(DWORD index, const T& item) ;

	void Delete(DWORD index, DWORD num = 1);
	virtual void Clear();

	inline int Find(const T& item) const;
	inline int FindInSorted(const T& item) const;

	inline int AddToSorted(const T& item);

	inline void Sort(int (*compare)(void *const *, void *const *, void *), void *param);

	static int CompareObjectItems(void *const *a1, void *const *a2, void * /* param */);

	inline void Sort() ;
private:
	inline int Compare(T a, T b);

private:
	CPoolVector(const CPoolVector &v);
	CPoolVector& operator=(const CPoolVector &v);
	CPoolVector& operator+=(const CPoolVector &v);

private:
};



/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
template<typename T>
CPoolVectorBaseImp<T>::CPoolVectorBaseImp(size_t itemSize)
	: _capacity(0)
	, _size(0)
	, _items(0)
	, _itemSize(itemSize)
	, _step(5120)
{
	_heap = HeapCreate(0,0,0);
}

template<typename T>
CPoolVectorBaseImp<T>::~CPoolVectorBaseImp()
{
	ClearAndFree();
	HeapDestroy(_heap);
}

template<typename T>
void CPoolVectorBaseImp<T>::ClearAndFree()
{
	Clear();
	HeapFree(_heap,0,_items);
	_capacity = 0;
	_size = 0;
	_items = 0;
}

template<typename T>
DWORD CPoolVectorBaseImp<T>::Size() const 
{
	return _size;
}

template<typename T>
bool CPoolVectorBaseImp<T>::IsEmpty() const 
{ 
	return (_size == 0); 
}

template<typename T>
bool CPoolVectorBaseImp<T>::Reserve(DWORD newCapacity)
{
	if (newCapacity == _capacity)
		return true;

	if ((DWORD)newCapacity >= ((DWORD)1 << (sizeof(DWORD) * 8 - 1)))
		return false;

	size_t newSize = (size_t)(DWORD)newCapacity * _itemSize;
	if (newSize / _itemSize != (size_t)(DWORD)newCapacity)
		return false;

	//unsigned char *p = NULL;
	if (newSize > 0)
	{
		if (!_items)
		{
			_items = HeapAlloc(_heap, 0, newSize);
		}
		else
		{
			_items = HeapReAlloc(_heap,0,_items, newSize);
		}
		
	}
	
	_capacity = newCapacity;
	return true;
}

template<typename T>
void CPoolVectorBaseImp<T>::ReserveDown()
{
	Reserve(_size);
}

template<typename T>
void CPoolVectorBaseImp<T>::Delete(DWORD index, DWORD num /*= 1*/)
{
	TestIndexAndCorrectNum(index, num);
	if (num > 0)
	{
		MoveItems(index, index + num);
		_size -= num;
	}
}

template<typename T>
void CPoolVectorBaseImp<T>::Clear()
{
	DeleteFrom(0);
}

template<typename T>
void CPoolVectorBaseImp<T>::DeleteFrom(DWORD index)
{
	Delete(index, _size - index);
}

template<typename T>
void CPoolVectorBaseImp<T>::DeleteBack()
{
	Delete(_size - 1);
}

template<typename T>
void CPoolVectorBaseImp<T>::ReserveOnePosition()
{
	if (_size != _capacity)
		return;
	/*
	
	unsigned delta = 1;
	if (_capacity >= 64)
		delta = (unsigned)_capacity / 4;
	else if (_capacity >= 8)
		delta = 8;
		
	Reserve(_capacity + (int)delta);
	*/
	Reserve(_capacity + _step);
}

template<typename T>
void CPoolVectorBaseImp<T>::InsertOneItem(DWORD index)
{
	ReserveOnePosition();
	MoveItems(index + 1, index);
	_size++;
}

template<typename T>
void CPoolVectorBaseImp<T>::TestIndexAndCorrectNum(DWORD index, DWORD &num) const
{ 
	if (index + num > _size) 
		num = _size - index; 
}


template<typename T>
void CPoolVectorBaseImp<T>::MoveItems(DWORD destIndex, DWORD srcIndex)
{
	memmove(((unsigned char *)_items) + destIndex * _itemSize,
		((unsigned char  *)_items) + srcIndex * _itemSize,
		_itemSize * (_size - srcIndex));
}

template<typename T>
T* CPoolVectorBaseImp<T>::GetPoint() const
{
	return (T*)_items;
}

template<typename T>
CPoolVectorBase<T>::CPoolVectorBase()
	: CPoolVectorBaseImp(sizeof(T))
{
	
};

template<typename T>
CPoolVectorBase<T>::CPoolVectorBase(const CPoolVectorBase &v)
	: CPoolVectorBaseImp(sizeof(T)) 
{ 
	*this = v; 
}

template<typename T>
CPoolVectorBase<T>& CPoolVectorBase<T>::operator=(const CPoolVectorBase &v)
{
	Clear();
	return (*this += v);
}

template<typename T>
CPoolVectorBase<T>& CPoolVectorBase<T>::operator+=(const CPoolVectorBase &v)
{
	int size = v.Size();
	Reserve(Size() + size);
	for (int i = 0; i < size; i++)
		Add(v[i]);
	return *this;
}

template<typename T>
int CPoolVectorBase<T>::Add(T item)
{
	ReserveOnePosition();
	((T *)_items)[_size] = item;
	return _size++;
}

template<typename T>
void CPoolVectorBase<T>::Insert(DWORD index, T item)
{
	InsertOneItem(index);
	((T *)_items)[index] = item;
}

template<typename T>
const T& CPoolVectorBase<T>::operator[](DWORD index) const 
{
	return ((T *)_items)[index]; 
}

template<typename T>
T& CPoolVectorBase<T>::operator[](DWORD index) 
{
	return ((T *)_items)[index]; 
}

template<typename T>
inline T* CPoolVectorBase<T>::GetAt(DWORD index)
{
	return ((T *)_items)+index; 
}

template<typename T>
const T& CPoolVectorBase<T>::Front() const 
{
	return operator[](0); 
}

template<typename T>
T& CPoolVectorBase<T>::Front() 
{
	return operator[](0); 
}

template<typename T>
const T& CPoolVectorBase<T>::Back() const 
{
	return operator[](_size - 1); 
}

template<typename T>
T& CPoolVectorBase<T>::Back() 
{
	return operator[](_size - 1); 
}

template<typename T>
void CPoolVectorBase<T>::Swap(DWORD i, DWORD j)
{
	T temp = operator[](i);
	operator[](i) = operator[](j);
	operator[](j) = temp;
}

template<typename T>
int CPoolVectorBase<T>::FindInSorted(const T& item, int left, int right) const
{
	T* pPoint = this->GetPoint();
	while (left != right)
	{
		DWORD mid = (left + right) / 2;
		const T& midValue = /*(*this)[mid]*/pPoint[mid];
		if (item == midValue)
			return mid;
		if (item < midValue)
			right = mid;
		else
			left = mid + 1;
	}
	return -1;
}

template<typename T>
int CPoolVectorBase<T>::FindInSorted(const T& item) const
{
	DWORD left = 0, right = Size();
	T* pPoint = this->GetPoint();
	while (left != right)
	{
		int mid = (left + right) / 2;
		const T& midValue = /*(*this)[mid]*/pPoint[mid];
		if (*item == *midValue)
			return mid;
		if (*item < *midValue)
			right = mid;
		else
			left = mid + 1;
	}
	return -1;
}

template<typename T>
int CPoolVectorBase<T>::AddToUniqueSorted(const T& item)
{
	int left = 0, right = Size();
	T* pPoint = this->GetPoint();
	while (left != right)
	{
		int mid = (left + right) / 2;
		const T& midValue = /*(*this)[mid]*/pPoint[mid];
		if (*item == *midValue)
			return mid;
		if (*item < *midValue)
			right = mid;
		else
			left = mid + 1;
	}
	Insert(right, item);
	return right;
}

template<typename T>
int CPoolVectorBase<T>::AddToSorted(const T& item, int (*compare)(const T*, const T*, void *),void* param)
{
	int left = 0, right = Size();
	T* pPoint = this->GetPoint();
	while (left != right)
	{
		int mid = (left + right) / 2;
		const T& midValue = /*(*this)[mid]*/pPoint[mid];
// 		if (compare(&item , &midValue, param) == 0)
// 			return mid;
		if (compare(&item , &midValue,param) <= 0)
			right = mid;
		else
			left = mid + 1;
	}

	Insert(right, item);
	return right;
}


template<typename T>
int CPoolVectorBase<T>::FindInSorted(const T& item, int (*compare)(const T*, const T*, void *), void* param, int* end)
{
	DWORD left = 0, right = Size();
	T* pPoint = this->GetPoint();

	int nIndex = -1;
	while (left != right)
	{
		int mid = (left + right) / 2;
		const T& midValue = /*(*this)[mid]*/pPoint[mid];
		if (compare(&item , &midValue,param) == 0)
		{
			nIndex = mid;
			break;
		}
		if (compare(&item , &midValue,param) < 0)
			right = mid;
		else
			left = mid + 1;
	}


	
	

	if (nIndex != -1)
	{
		int nEnd = Size();
		int nBegin = -1;
		int nSize = Size();

		//向前索引
		for (nBegin = nIndex ; nBegin != 0 ; nBegin--)
		{
			const T& midValue = /*(*this)[mid]*/pPoint[nBegin];
			if (compare(&item , &midValue,param) != 0)
			{
				nBegin++;
				break;
			}
		}

		//向后索引
		for (nEnd = nIndex ; nEnd < nSize ; nEnd++)
		{
			const T& midValue = /*(*this)[mid]*/pPoint[nEnd];
			if (compare(&item , &midValue,param) != 0)
			{
				break;
			}
		}

		*end = nEnd - nBegin;
		return nBegin;
	}


	return -1;
}

template<typename T>
void CPoolVectorBase<T>::SortRefDown(T* p, int k, int size, int (*compare)(const T*, const T*, void *), void *param)
{
	T temp = p[k];
	for (;;)
	{
		int s = (k << 1);
		if (s > size)
			break;
		if (s < size && compare(p + s + 1, p + s, param) > 0)
			s++;
		if (compare(&temp, p + s, param) >= 0)
			break;
		p[k] = p[s];
		k = s;
	}
	p[k] = temp;
}

template<typename T>
void CPoolVectorBase<T>::Sort(int (*compare)(const T*, const T*, void *), void *param)
{
	int size = _size;
	if (size <= 1)
		return;
	T* p = (&Front()) - 1;
	{
		int i = size / 2;
		do
		SortRefDown(p, i, size, compare, param);
		while (--i != 0);
	}
	do
	{
		T temp = p[size];
		p[size--] = p[1];
		p[1] = temp;
		SortRefDown(p, 1, size, compare, param);
	}
	while (size > 1);
}


template <class T>
CPoolVector<T>::CPoolVector()
{
	//ZeroMemory(&m_Mpool, sizeof(m_Mpool));
	//mpool_init_default(&m_Mpool,4096,sizeof(T));
}


template <class T>
CPoolVector<T>::~CPoolVector() 
{ 
	Clear(); 
};


template <class T>
CPoolVector<T>::CPoolVector(const CPoolVector &v)
	: CPoolVectorBase<void*>() 
{ 
	*this = v; 
}


template <class T> 
CPoolVector<T>& CPoolVector<T>::operator=(const CPoolVector &v)
{
	Clear();
	return (*this += v);
}

template <class T> 
CPoolVector<T>& CPoolVector<T>::operator+=(const CPoolVector &v)
{
	int size = v.Size();
	Reserve(Size() + size);
	for (int i = 0; i < size; i++)
		Add(v[i]);
	return *this;
}

template <class T> 
const T& CPoolVector<T>::operator[](DWORD index) const 
{
	return *((T *)CPoolVectorBase<void*>::operator[](index)); 
}

template <class T> 
T& CPoolVector<T>::operator[](DWORD index) 
{
	return *((T *)CPoolVectorBase<void*>::operator[](index)); 
}

template <class T> 
T& CPoolVector<T>::Front() 
{
	return operator[](0); 
}

template <class T> 
const T& CPoolVector<T>::Front() const 
{
	return operator[](0); 
}

template <class T> 
T& CPoolVector<T>::Back() 
{
	return operator[](_size - 1); 
}

template <class T> 
const T& CPoolVector<T>::Back() const 
{
	return operator[](_size - 1); 
}

template <class T> 
int CPoolVector<T>::Add(const T& item) 
{
	T* pitem = m_Mpool ? (T*)mpool_salloc(m_Mpool, sizeof(item)): new T;
	*pitem = item;
	return CPoolVectorBase<void*>::Add(pitem); 
}

template <class T> 
void CPoolVector<T>::Insert(DWORD index, const T& item) 
{	
	//T* pitem = m_Mpool.sa ? (T*)mpool_salloc(&m_Mpool, sizeof(item)): new T;
	T* pitem = (T*)m_Mpool.Alloc(sizeof(item));

	*pitem = item;

	CPoolVectorBase<void*>::Insert(index, pitem); 
}

template <class T> 
void CPoolVector<T>::Delete(DWORD index, DWORD num = 1)
{
	TestIndexAndCorrectNum(index, num);
	for (int i = 0; i < num; i++)
	{
		//delete (T *)(((void **)_items)[index + i]);
		//m_Mpool.sa ? mpool_sfree(&m_Mpool,(T *)(((void **)_items)[index + i]),sizeof(T)) : (T *)(((void **)_items)[index + i]);
		m_Mpool.Free((T *)(((void **)_items)[index + i]),sizeof(T));

	}
	CPoolVectorBase<void*>::Delete(index, num);
}

template <class T> 
void CPoolVector<T>::Clear()
{
	m_Mpool.Clear();
	CPoolVectorBase<void*>::Clear();
}
template <class T> 
int CPoolVector<T>::Find(const T& item) const
{
	for (int i = 0; i < Size(); i++)
		if (item == (*this)[i])
			return i;
	return -1;
}

template <class T> 
int CPoolVector<T>::FindInSorted(const T& item) const
{
	int left = 0, right = Size();
	while (left != right)
	{
		int mid = (left + right) / 2;
		const T& midValue = (*this)[mid];
		if (item == midValue)
			return mid;
		if (item < midValue)
			right = mid;
		else
			left = mid + 1;
	}
	return -1;
}

template <class T> 
int CPoolVector<T>::AddToSorted(const T& item)
{
	int left = 0, right = Size();
	while (left != right)
	{
		int mid = (left + right) / 2;
		const T& midValue = (*this)[mid];
		if (item == midValue)
		{
			right = mid + 1;
			break;
		}
		if (item < midValue)
			right = mid;
		else
			left = mid + 1;
	}
	Insert(right, item);
	return right;
}

template <class T> 
void CPoolVector<T>::Sort(int (*compare)(void *const *, void *const *, void *), void *param)
{
	CPoolVectorBase<void*>::Sort(compare, param); 
}

template <class T> 
static int CPoolVector<T>::CompareObjectItems(void *const *a1, void *const *a2, void * /* param */)
{
	return Compare(*(*((const T **)a1)), *(*((const T **)a2))); 
}

template <class T> 
void CPoolVector<T>::Sort() 
{
	CPoolVectorBase<void*>::Sort(CompareObjectItems, 0); 
}


template <class T> 
inline int CPoolVector<T>::Compare(T a, T b)
{
	return a < b ? -1 : (a == b ? 0 : 1); 
}


};//namespace msdk

#endif