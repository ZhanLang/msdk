#ifndef _RULEBUFFER_H_
#define _RULEBUFFER_H_

/************************************************************************/
//
//		用于跨模块间的数据内容传递，便于分拆数据
//		降低模块间传递buf的难度，用法是这个样子的
//		定义一个函数 aa(bufdef& buf)
//		然后当你需要用这个函数的时候
//
//		bufdef buf;
//		aa(buf);
//		这个时候，buf就是你想得到的内容
//
/************************************************************************/

#include <vector>

template <class T>
class CRuleBuffer
{
public:
	CRuleBuffer(){}
	typedef std::vector<T> RULEBUFFERVECTOR;

	long  GetSize() const    {return (long)m_Vector.size();}
	
	//注意，rulebuf是跨dll的，所以这里必须是virtual
	virtual void push_back(const T& Unit) {m_Vector.push_back(Unit);}

	size_t GetNoOfBytes()const  
	{
		return m_Vector.size()*sizeof(T);
	}

	T& operator[](long nIndex)
	{
		return m_Vector[nIndex];
	}

	const T& operator[](long nIndex)const
	{
		return m_Vector[nIndex];
	}

	operator const T*()const {return &m_Vector[0];}
	operator T*()            {return &m_Vector[0];}

	RULEBUFFERVECTOR& GetVector(){return m_Vector;}

	void  Clear()   {m_Vector.clear();}

protected:
	RULEBUFFERVECTOR m_Vector;
};

#endif