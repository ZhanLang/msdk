/********************************************************************
	created:	2009/03/18
	created:	18:3:2009   13:43
	filename: 	d:\C++Work\appproduct2009\app\utm\datacenter\objpool.h
	file path:	d:\C++Work\appproduct2009\app\utm\datacenter
	file base:	objpool
	file ext:	h
	author:		zhangming1
	
	purpose:	
*********************************************************************/

#pragma once

#include <deque>

template <typename T>
class CObjRing
{
public:
	~CObjRing()
	{
		Release();
	}

	BOOL Create( int nRingLen )
	{
		m_pBuffer = new T[nRingLen];
		if( !m_pBuffer )
			return FALSE;

		m_nFree = nRingLen;
		return TRUE;
	}

	void Release( void ) { delete[] m_pBuffer; }

	BOOL IsEmpty( int nRingLen ) { return m_nFree==nRingLen || m_nObjHead==m_nObjTail; }


	T*	AllocObj( int nRingLen )
	{
		if( m_nFree <= 0 )
			return NULL;

		int indexObj = -1;

		if( m_nObjTail < nRingLen )
			indexObj = m_nObjTail;
		else
			indexObj = 0;

		
		m_nObjTail = indexObj+1;
		m_nFree--;

		T* pObj = m_pBuffer + indexObj;

		return pObj;
	}
	
	T* GetFrontObj( int nRingLen )
	{
		if( IsEmpty(nRingLen) )
			return NULL;

		if( m_nObjHead==nRingLen )
			m_nObjHead = 0;

		return &m_pBuffer[m_nObjHead];
	}

	void PopFrontObj( int nRingLen )
	{
		if( m_nObjHead==nRingLen )
			m_nObjHead = 0;

		T &Obj = m_pBuffer[m_nObjHead];
		Obj.Reset();

		m_nObjHead++;
		m_nFree++;
	}

	T* GetFrontObjs( int nRingLen, int& nNum )
	{

	}

private:
	T*			m_pBuffer;
	int			m_nObjHead;
	int			m_nObjTail;
	int			m_nFree;
};



template <class T>
class CObjPool
{
public:
	typedef CObjRing<T>						RingType;
	typedef RingType						*RingPtr;
	typedef std::deque<RingPtr>				QueueType;
	typedef typename QueueType::iterator	IterType;

public:
	CObjPool()
	{
		
	}

	BOOL Init( int nRingLen )
	{
		if( !m_Rings.empty() )
			Uninit();

		RingPtr initRing = new RingType;
		if( !initRing->Create( nRingLen ) )
			return FALSE;

		m_Rings.push_back( initRing );

		return TRUE;
	}

	void Uninit( void )
	{
		IterType iter = m_Rings.begin();
		IterType end = m_Rings.end();

		for( ; iter!=end; iter++ )
		{
			RingPtr pRing = *iter;
			delete pRing;
		}

		m_Rings.clear();
	}

	T* GetFrontObj( void )
	{
		if( m_Rings.empty() )
			return NULL;

		RingPtr pHead = m_Rings.front();
		return pHead->GetFrontObj( m_nRingLen );
	}

	void PopFrontObj( void )
	{
		RingPtr pHead = m_Rings.front();
		pHead->PopFrontObj(m_nRingLen);

		if( pHead->IsEmpty(m_nRingLen) && m_Rings.size() > 1 )
		{
			delete pHead;
			m_Rings.pop_front();
		}
	}

	T* GetFrontObjs( int& nNum )
	{

	}

	T*	AllocObj( void )
	{
		if( m_Rings.empty() )
		{
			return NULL;
		}

		RingPtr pTail = m_Rings.back();

		T* pObj = pTail->AllocObj( m_nRingLen );

		if( pObj == NULL )
		{
			// The last ring is full, allocate a new one
			RingPtr pNew = new RingType;
			if( !pNew )
				return NULL;

			if( !pNew->Create( m_nRingLen ) )
				return NULL;

			m_Rings.push_back( pNew );
			pTail = pNew;

			pObj = pTail->AllocObj( m_nRingLen );
		}

		return pObj;
	}

private:
	QueueType		m_Rings;
	int				m_nRingLen;
};