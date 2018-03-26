#pragma once

#ifndef _VECTOR_INCLUDE_H_
#define _VECTOR_INCLUDE_H_

namespace msdk{;

template<class T>
class CVectorBaseImp
{
public:
	CVectorBaseImp(size_t itemSize);
	virtual ~CVectorBaseImp();
	void ClearAndFree();
	int Size() const ;
	bool IsEmpty() const ;
	bool Reserve(int newCapacity);
	void ReserveDown();
	virtual void Delete(int index, int num = 1);
	void Clear();
	void DeleteFrom(int index);
	void DeleteBack();

	
protected:
	void ReserveOnePosition();
	void InsertOneItem(int index);
	void TestIndexAndCorrectNum(int index, int &num) const;

protected:
	int _capacity;
	int _size;
	void *_items;
	size_t _itemSize;

private:
	void MoveItems(int destIndex, int srcIndex);
};


template<typename T>
class  CVectorBase: public CVectorBaseImp<T>
{
public:
	CVectorBase();
	CVectorBase(const CVectorBase &v);
	CVectorBase& operator=(const CVectorBase &v);
	CVectorBase& operator+=(const CVectorBase &v);
	int Add(T item);
	void Insert(int index, T item);
	const T& operator[](int index) const ;
	T& operator[](int index) ;
	const T& Front() const ;
	T& Front() ;
	const T& Back() const ;
	T& Back() ;
	void Swap(int i, int j);
	int FindInSorted(const T& item, int left, int right) const;
	int FindInSorted(const T& item) const;
	int AddToUniqueSorted(const T& item);
	static void SortRefDown(T* p, int k, int size, int (*compare)(const T*, const T*, void *), void *param);
	void Sort(int (*compare)(const T*, const T*, void *), void *param);
};

template <class T>
class CApiVector: public CVectorBase<void*>
{
public:
	CApiVector() ;
	~CApiVector() ;
	CApiVector(const CApiVector &v);
	CApiVector& operator=(const CApiVector &v);
	CApiVector& operator+=(const CApiVector &v);
	const T& operator[](int index) const ;
	T& operator[](int index) ;
	T& Front() ;
	const T& Front() const ;
	T& Back() ;
	const T& Back() const ;
	int Add(const T& item) ;
	void Insert(int index, const T& item) ;
	virtual void Delete(int index, int num = 1);
	int Find(const T& item) const;
	int FindInSorted(const T& item) const;
	int AddToSorted(const T& item);
	void Sort(int (*compare)(void *const *, void *const *, void *), void *param);
	static int CompareObjectItems(void *const *a1, void *const *a2, void * /* param */);
	void Sort() ;
private:
	inline int Compare(T a, T b);
};



/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
template<typename T>
CVectorBaseImp<T>::CVectorBaseImp(size_t itemSize)
	: _capacity(0)
	, _size(0)
	, _items(0)
	, _itemSize(itemSize) 
{

}

template<typename T>
CVectorBaseImp<T>::~CVectorBaseImp()
{
	ClearAndFree();
}

template<typename T>
void CVectorBaseImp<T>::ClearAndFree()
{
	Clear();
	delete []((unsigned char *)_items);
	_capacity = 0;
	_size = 0;
	_items = 0;
}

template<typename T>
int CVectorBaseImp<T>::Size() const 
{
	return _size;
}

template<typename T>
bool CVectorBaseImp<T>::IsEmpty() const 
{ 
	return (_size == 0); 
}

template<typename T>
bool CVectorBaseImp<T>::Reserve(int newCapacity)
{
	if (newCapacity == _capacity)
		return true;

	if ((unsigned)newCapacity >= ((unsigned)1 << (sizeof(unsigned) * 8 - 1)))
		return false;

	size_t newSize = (size_t)(unsigned)newCapacity * _itemSize;
	if (newSize / _itemSize != (size_t)(unsigned)newCapacity)
		return false;

	unsigned char *p = NULL;
	if (newSize > 0)
	{
		p = new unsigned char[newSize];
		if (p == 0)
			return false;

		int numRecordsToMove = (_size < newCapacity ? _size : newCapacity);
		memcpy(p, _items, _itemSize * numRecordsToMove);
	}

	delete [](unsigned char *)_items;
	_items = p;
	_capacity = newCapacity;
	return true;
}

template<typename T>
void CVectorBaseImp<T>::ReserveDown()
{
	Reserve(_size);
}

template<typename T>
void CVectorBaseImp<T>::Delete(int index, int num /*= 1*/)
{
	TestIndexAndCorrectNum(index, num);
	if (num > 0)
	{
		MoveItems(index, index + num);
		_size -= num;
	}
}

template<typename T>
void CVectorBaseImp<T>::Clear()
{
	DeleteFrom(0);
}

template<typename T>
void CVectorBaseImp<T>::DeleteFrom(int index)
{
	Delete(index, _size - index);
}

template<typename T>
void CVectorBaseImp<T>::DeleteBack()
{
	Delete(_size - 1);
}

template<typename T>
void CVectorBaseImp<T>::ReserveOnePosition()
{
	if (_size != _capacity)
		return;
	unsigned delta = 1;
	if (_capacity >= 64)
		delta = (unsigned)_capacity / 4;
	else if (_capacity >= 8)
		delta = 8;
	Reserve(_capacity + (int)delta);
}

template<typename T>
void CVectorBaseImp<T>::InsertOneItem(int index)
{
	ReserveOnePosition();
	MoveItems(index + 1, index);
	_size++;
}

template<typename T>
void CVectorBaseImp<T>::TestIndexAndCorrectNum(int index, int &num) const
{ 
	if (index + num > _size) 
		num = _size - index; 
}


template<typename T>
void CVectorBaseImp<T>::MoveItems(int destIndex, int srcIndex)
{
	memmove(((unsigned char *)_items) + destIndex * _itemSize,
		((unsigned char  *)_items) + srcIndex * _itemSize,
		_itemSize * (_size - srcIndex));
}



template<typename T>
CVectorBase<T>::CVectorBase()
	: CVectorBaseImp(sizeof(T))
{

};

template<typename T>
CVectorBase<T>::CVectorBase(const CVectorBase &v)
	: CVectorBaseImp(sizeof(T)) 
{ 
	*this = v; 
}

template<typename T>
CVectorBase<T>& CVectorBase<T>::operator=(const CVectorBase &v)
{
	Clear();
	return (*this += v);
}

template<typename T>
CVectorBase<T>& CVectorBase<T>::operator+=(const CVectorBase &v)
{
	int size = v.Size();
	Reserve(Size() + size);
	for (int i = 0; i < size; i++)
		Add(v[i]);
	return *this;
}

template<typename T>
int CVectorBase<T>::Add(T item)
{
	ReserveOnePosition();
	((T *)_items)[_size] = item;
	return _size++;
}

template<typename T>
void CVectorBase<T>::Insert(int index, T item)
{
	InsertOneItem(index);
	((T *)_items)[index] = item;
}

template<typename T>
const T& CVectorBase<T>::operator[](int index) const 
{
	return ((T *)_items)[index]; 
}

template<typename T>
T& CVectorBase<T>::operator[](int index) 
{
	return ((T *)_items)[index]; 
}

template<typename T>
const T& CVectorBase<T>::Front() const 
{
	return operator[](0); 
}

template<typename T>
T& CVectorBase<T>::Front() 
{
	return operator[](0); 
}

template<typename T>
const T& CVectorBase<T>::Back() const 
{
	return operator[](_size - 1); 
}

template<typename T>
T& CVectorBase<T>::Back() 
{
	return operator[](_size - 1); 
}

template<typename T>
void CVectorBase<T>::Swap(int i, int j)
{
	T temp = operator[](i);
	operator[](i) = operator[](j);
	operator[](j) = temp;
}

template<typename T>
int CVectorBase<T>::FindInSorted(const T& item, int left, int right) const
{
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

template<typename T>
int CVectorBase<T>::FindInSorted(const T& item) const
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

template<typename T>
int CVectorBase<T>::AddToUniqueSorted(const T& item)
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
	Insert(right, item);
	return right;
}

template<typename T>
static void CVectorBase<T>::SortRefDown(T* p, int k, int size, int (*compare)(const T*, const T*, void *), void *param)
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
void CVectorBase<T>::Sort(int (*compare)(const T*, const T*, void *), void *param)
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
CApiVector<T>::CApiVector() 
{

}


template <class T>
CApiVector<T>::~CApiVector() 
{ 
	Clear(); 
};


template <class T>
CApiVector<T>::CApiVector(const CApiVector &v)
	: CVectorBase<void*>() 
{ 
	*this = v; 
}


template <class T> 
CApiVector<T>& CApiVector<T>::operator=(const CApiVector &v)
{
	Clear();
	return (*this += v);
}

template <class T> 
CApiVector<T>& CApiVector<T>::operator+=(const CApiVector &v)
{
	int size = v.Size();
	Reserve(Size() + size);
	for (int i = 0; i < size; i++)
		Add(v[i]);
	return *this;
}

template <class T> 
const T& CApiVector<T>::operator[](int index) const 
{
	return *((T *)CVectorBase<void*>::operator[](index)); 
}

template <class T> 
T& CApiVector<T>::operator[](int index) 
{
	return *((T *)CVectorBase<void*>::operator[](index)); 
}

template <class T> 
T& CApiVector<T>::Front() 
{
	return operator[](0); 
}

template <class T> 
const T& CApiVector<T>::Front() const 
{
	return operator[](0); 
}

template <class T> 
T& CApiVector<T>::Back() 
{
	return operator[](_size - 1); 
}

template <class T> 
const T& CApiVector<T>::Back() const 
{
	return operator[](_size - 1); 
}

template <class T> 
int CApiVector<T>::Add(const T& item) 
{
	return CVectorBase<void*>::Add(new T(item)); 
}

template <class T> 
void CApiVector<T>::Insert(int index, const T& item) 
{
	CVectorBase<void*>::Insert(index, new T(item)); 
}

template <class T> 
void CApiVector<T>::Delete(int index, int num = 1)
{
	TestIndexAndCorrectNum(index, num);
	for (int i = 0; i < num; i++)
		delete (T *)(((void **)_items)[index + i]);
	CVectorBase<void*>::Delete(index, num);
}

template <class T> 
int CApiVector<T>::Find(const T& item) const
{
	for (int i = 0; i < Size(); i++)
		if (item == (*this)[i])
			return i;
	return -1;
}

template <class T> 
int CApiVector<T>::FindInSorted(const T& item) const
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
int CApiVector<T>::AddToSorted(const T& item)
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
void CApiVector<T>::Sort(int (*compare)(void *const *, void *const *, void *), void *param)
{
	CVectorBase<void*>::Sort(compare, param); 
}

template <class T> 
static int CApiVector<T>::CompareObjectItems(void *const *a1, void *const *a2, void * /* param */)
{
	return Compare(*(*((const T **)a1)), *(*((const T **)a2))); 
}

template <class T> 
void CApiVector<T>::Sort() 
{
	CVectorBase<void*>::Sort(CompareObjectItems, 0); 
}


template <class T> 
inline int CApiVector<T>::Compare(T a, T b)
{
	return a < b ? -1 : (a == b ? 0 : 1); 
}


};//namespace msdk

#endif