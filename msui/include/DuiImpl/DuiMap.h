#pragma once

namespace DuiKit{;


template<class KEY, class TYPE> 
class CDuiMapNode
{
public:
	TYPE    Value;
	KEY	    Key;

public:
	typedef CDuiMapNode<KEY,TYPE> Node;
	enum  NodeColor {RED,BLACK};
	Node *  Parent;
	Node *  RightChild;
	Node *  LelfChild;
	BOOLEAN IsSnil;
	BYTE    Color;
	
};

template<class KEY, class TYPE> 
class CDuiMapIterator								//为了支持对map进行按升序或降序遍历
{
public:
	typedef CDuiMapNode<KEY,TYPE> Node;
	Node * pRBT_Node;
	CDuiMapIterator(){pRBT_Node=NULL;};
	CDuiMapIterator(Node* pNode){pRBT_Node=pNode;};
	void Increment();
	void Decrement();
	void operator++(int){ Increment();}				//向更大的KEY
	void operator--(int){ Decrement();}				//向更小的KEY
	operator Node* () const { return pRBT_Node; }
	Node* operator -> () const {return pRBT_Node;}
};



template<typename KEY>
struct CDuiMapNodLess
{
	bool operator ()(const KEY& l , const KEY& r) const
	{
		return l < r;
	}
};

template<class KEY, class TYPE, class _CMP = CDuiMapNodLess<KEY>> 
class CDuiMap
{
public:
	typedef CDuiMapIterator<KEY,TYPE> Iterator;
	typedef CDuiMapNode<KEY,TYPE>*     ValueType;
	typedef CDuiMapNode<KEY,TYPE> Node;
	inline UINT Count();
	
	CDuiMap();
	~CDuiMap();
	CDuiMap(const CDuiMap &v);
	VOID  Clear();
	BOOL  Insert(const KEY& Key, const TYPE& Value);
	BOOL  Remove(const KEY& Key);
	inline Node* Find(const KEY& Key);
	Node* Min() const;						  //返回最小Key节点,以便使用TRB_TreeIter 进行遍历
	Node* Max() const;						  //返回最大Key节点,以便使用TRB_TreeIter 进行遍历

	CDuiMap& operator= (const CDuiMap& t);
private:
	VOID  DeleteFixUp(Node *TreeNode);	  //删除后的树平衡
	VOID  DeleteTree(Node * TreeNode);	  //删除指定的树
	VOID  LeftRotate(Node * TreeNode);    //对指定点进行左旋,假设这里指定点存在右孩子结点.
	VOID  RightRotate(Node * TreeNode);   //对指定点进行右旋,假设这里指定点存在左孩子结点.

	Node *  m_Head;
	Node    m_Snil;
	UINT    m_Count;
};
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
/*使用方式
CDuiMap<CDuiString, CDuiString> strMap;
strMap.Insert(_T("5"), _T("hello"));
strMap.Insert(_T("4"), _T("mpimap"));

//遍历
CDuiMap<CDuiString,CDuiString>::Iterator it = strMap.Min();
for (; it ; it++)
{
}

//查找
CDuiMap<CDuiString,CDuiString>::ValueType value = strMap.Find(_T("2"));
*/
/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
template<class KEY, class TYPE> 
void CDuiMapIterator<KEY,TYPE>::Increment()			
{
	if(pRBT_Node==NULL)
		return;

	if(pRBT_Node->IsSnil)
	{	//这个节点是空
		pRBT_Node=NULL;
		return;
	}
	else
	{	
		if(!pRBT_Node->RightChild->IsSnil)
		{	//这个节点有右节点,找出该右节点的最小值
			pRBT_Node=pRBT_Node->RightChild;
			while(!pRBT_Node->LelfChild->IsSnil)
				pRBT_Node=pRBT_Node->LelfChild;
		}
		else
		{   //这个节点右节点为空
			while(pRBT_Node->Parent&&pRBT_Node->Parent->RightChild==pRBT_Node)
				pRBT_Node=pRBT_Node->Parent;
			pRBT_Node=pRBT_Node->Parent;
		}
	}
}

template<class KEY, class TYPE> 
void CDuiMapIterator<KEY,TYPE>::Decrement()
{
	if(pRBT_Node==NULL)
		return;

	if(pRBT_Node->IsSnil)
	{	//这个节点是空
		pRBT_Node=NULL;
		return;
	}
	else
	{	
		if(!pRBT_Node->LelfChild->IsSnil)
		{	//这个节点有右节点,找出该右节点的最小值
			pRBT_Node=pRBT_Node->LelfChild;
			while(!pRBT_Node->RightChild->IsSnil)
				pRBT_Node=pRBT_Node->RightChild;
		}
		else
		{   //这个节点右节点为空
			while(pRBT_Node->Parent&&pRBT_Node->Parent->LelfChild==pRBT_Node)
				pRBT_Node=pRBT_Node->Parent;
			pRBT_Node=pRBT_Node->Parent;
		}
	}
}

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
template<class KEY, class TYPE, class _CMP> 
CDuiMapNode<KEY,TYPE>*  CDuiMap<KEY,TYPE,_CMP>::Min() const
{
	Node* pResult=m_Head;
	if(!pResult->IsSnil)
	{
		while(!pResult->LelfChild->IsSnil)
			pResult=pResult->LelfChild;
	}

	if(pResult->IsSnil)
		return NULL;
	return pResult;
}

template<class KEY, class TYPE, class _CMP> 
CDuiMapNode<KEY,TYPE>*  CDuiMap<KEY,TYPE,_CMP>::Max() const 
{
	Node* pResult=m_Head;
	if(!pResult->IsSnil)
	{
		while(!pResult->RightChild->IsSnil)
			pResult=pResult->RightChild;
	}

	if(pResult->IsSnil)
		return NULL;
	return pResult;
}

template<class KEY, class TYPE, class _CMP> 
CDuiMapNode<KEY,TYPE>*  CDuiMap<KEY,TYPE,_CMP>::Find(const KEY& Key)
{
	Node* pResult=m_Head;
	while(!pResult->IsSnil)
	{
		if(pResult->Key==Key)			//该Key存在
			return pResult;

		if(_CMP()(Key,pResult->Key))
			pResult=pResult->LelfChild;
		else
			pResult=pResult->RightChild;
	}

	if(pResult->IsSnil)
		pResult=NULL;

	return pResult;
}

template<class KEY, class TYPE, class _CMP> 
BOOL  CDuiMap<KEY,TYPE,_CMP>::Insert(const KEY& Key, const TYPE& Value)
{
	Node * TreeNode,*InsertPoint,*NewNode;
	TreeNode=m_Head;
	InsertPoint=NULL;

	//找出一个插入点
	while(!TreeNode->IsSnil)
	{
		InsertPoint=TreeNode;
		if(Key==TreeNode->Key)			//该Key存在
			return FALSE;

		if(_CMP()(Key,TreeNode->Key))
			TreeNode=TreeNode->LelfChild;
		else
			TreeNode=TreeNode->RightChild;
	}

	NewNode=new Node;
	if(NewNode==NULL)
		return FALSE;

	NewNode->Color=Node::RED;
	NewNode->Parent=InsertPoint;
	NewNode->LelfChild=&m_Snil;
	NewNode->RightChild=&m_Snil;
	NewNode->Value=Value;
	NewNode->Key=Key;
	NewNode->IsSnil=false;
	//新插入节点为根节点.
	if(InsertPoint==NULL) 
		m_Head=NewNode;
	else
	{
  		if(_CMP()(Key, InsertPoint->Key))
  			InsertPoint->LelfChild=NewNode;
  		else
  			InsertPoint->RightChild=NewNode;
	}
	//对刚插入新节点的红黑树进行平衡调整.
	while(NewNode!=m_Head&&NewNode->Parent->Color==Node::RED)
	{   //当插入的节点的父节点为爷爷节点的左边.
		if(NewNode->Parent==NewNode->Parent->Parent->LelfChild)
		{
			InsertPoint=NewNode->Parent->Parent->RightChild;
			if(InsertPoint->Color==Node::RED) 
			{
				NewNode->Parent->Color=Node::BLACK;
				NewNode->Parent->Parent->Color=Node::RED;
				InsertPoint->Color=Node::BLACK;
				NewNode=NewNode->Parent->Parent;
			}
			else
			{
				if(NewNode==NewNode->Parent->RightChild)
				{
					NewNode=NewNode->Parent;
					LeftRotate(NewNode);
				}
				NewNode->Parent->Color=Node::BLACK;
				NewNode->Parent->Parent->Color=Node::RED;
				RightRotate(NewNode->Parent->Parent);
			}
		}
		else
		{//当插入的节点的父节点为爷爷节点的右边.
			InsertPoint=NewNode->Parent->Parent->LelfChild;
			if(InsertPoint->Color==Node::RED)
			{
				NewNode->Parent->Color=Node::BLACK;
				NewNode->Parent->Parent->Color=Node::RED;
				InsertPoint->Color=Node::BLACK;
				NewNode=NewNode->Parent->Parent;
			}
			else
			{
				if(NewNode==NewNode->Parent->LelfChild)
				{
					NewNode=NewNode->Parent;
					RightRotate(NewNode);
				}
				NewNode->Parent->Color=Node::BLACK;
				NewNode->Parent->Parent->Color=Node::RED;
				LeftRotate(NewNode->Parent->Parent);
			}
		}
	}//end while 进行红黑树插入平衡调整
	m_Count++;
	m_Head->Color=Node::BLACK; //根结点为黑
	return TRUE;
}

template<class KEY, class TYPE, class _CMP> 
CDuiMap<KEY,TYPE,_CMP>::CDuiMap()
{
	m_Snil.IsSnil=TRUE;
	m_Snil.LelfChild=NULL;
	m_Snil.RightChild=NULL;
	m_Snil.Color=Node::BLACK;
	m_Head=&m_Snil;m_Count=0;
};

template<class KEY, class TYPE, class _CMP> 
CDuiMap<KEY,TYPE,_CMP>::CDuiMap(const CDuiMap<KEY,TYPE,_CMP>& t)
{
	m_Snil.IsSnil		=TRUE;
	m_Snil.LelfChild	=NULL;
	m_Snil.RightChild	=NULL;
	m_Snil.Color		=Node::BLACK;
	m_Head				=&m_Snil;
	m_Count				=0;

	*this = t;
}

template<class KEY, class TYPE, class _CMP> 
CDuiMap<KEY,TYPE,_CMP>::~CDuiMap()
{
	Clear();
};

template<class KEY, class TYPE, class _CMP> 
BOOL CDuiMap<KEY,TYPE,_CMP>::Remove(const KEY& Key)
{
	Node * DeleteNode,*TreeNode,*R;
	DeleteNode=m_Head;
	//找要删除的节点
	while(!DeleteNode->IsSnil&&DeleteNode->Key!=Key)
	{
		if(Key<DeleteNode->Key)
			DeleteNode=DeleteNode->LelfChild;
		else
			DeleteNode=DeleteNode->RightChild;
	}
	//当没有找到要删除的节点.
	if(DeleteNode->IsSnil)
		return FALSE;

	if(DeleteNode->LelfChild->IsSnil||DeleteNode->RightChild->IsSnil)
		TreeNode=DeleteNode;
	else
	{	//要删除的节点有两个子节点.所以就找出它的后续节点.
		TreeNode=DeleteNode->RightChild;
		while(!TreeNode->LelfChild->IsSnil)
			TreeNode=TreeNode->LelfChild;
	}

	if(!TreeNode->LelfChild->IsSnil)//R为删除节点的下个节点.
		R=TreeNode->LelfChild;
	else
		R=TreeNode->RightChild;

	R->Parent=TreeNode->Parent;

	if(TreeNode->Parent==NULL)
		m_Head=R;//删掉的是树根节点
	else
	{
		if(TreeNode==TreeNode->Parent->LelfChild)
			TreeNode->Parent->LelfChild=R;
		else
			TreeNode->Parent->RightChild=R;
	}

	if(TreeNode!=DeleteNode)
	{
		DeleteNode->Value=TreeNode->Value;
		DeleteNode->Key=TreeNode->Key;
	}

	if(TreeNode->Color==Node::BLACK)
		DeleteFixUp(R);

	delete TreeNode;
	m_Count--;
	return TRUE;
}


template<class KEY, class TYPE, class _CMP> 
VOID CDuiMap<KEY,TYPE,_CMP>::DeleteTree(Node *TreeNode)
{
	if(TreeNode->IsSnil)
		return ;
	if(!TreeNode->LelfChild->IsSnil)
		DeleteTree(TreeNode->LelfChild);

	if(!TreeNode->RightChild->IsSnil)
		DeleteTree(TreeNode->RightChild);

	m_Count--;
	delete TreeNode;
}

template<class KEY, class TYPE, class _CMP> 
VOID CDuiMap<KEY,TYPE,_CMP>::LeftRotate(Node * TreeNode)
{
	Node * Rotate_Right;
	Rotate_Right=TreeNode->RightChild;
	TreeNode->RightChild=Rotate_Right->LelfChild;

	if(!Rotate_Right->LelfChild->IsSnil)
		Rotate_Right->LelfChild->Parent=TreeNode;

	Rotate_Right->Parent=TreeNode->Parent;
	if(TreeNode->Parent==NULL) //被左旋的是根节点.
		m_Head=Rotate_Right;
	else
		if(TreeNode->Parent->LelfChild==TreeNode)//被旋的节点是它上一个节点的左
			TreeNode->Parent->LelfChild=Rotate_Right;
		else
			TreeNode->Parent->RightChild=Rotate_Right;

	Rotate_Right->LelfChild=TreeNode;
	TreeNode->Parent=Rotate_Right;
}

template<class KEY, class TYPE, class _CMP> 
VOID CDuiMap<KEY,TYPE,_CMP>::RightRotate(Node * TreeNode)
{
	Node * Rotate_Left;
	Rotate_Left=TreeNode->LelfChild;
	TreeNode->LelfChild=Rotate_Left->RightChild;
	if(!Rotate_Left->RightChild->IsSnil)
		Rotate_Left->RightChild->Parent=TreeNode;

	Rotate_Left->Parent=TreeNode->Parent;
	if(TreeNode->Parent==NULL)//为根节点
		m_Head=Rotate_Left;
	else
		if(TreeNode->Parent->RightChild==TreeNode)
			TreeNode->Parent->RightChild=Rotate_Left;
		else
			TreeNode->Parent->LelfChild=Rotate_Left;

	Rotate_Left->RightChild=TreeNode;
	TreeNode->Parent=Rotate_Left;
}

template<class KEY, class TYPE, class _CMP> 
VOID CDuiMap<KEY,TYPE,_CMP>::DeleteFixUp(Node * TreeNode)
{
	Node * Brother;
	while(TreeNode!=m_Head&&TreeNode->Color==Node::BLACK)
	{
		if(TreeNode==TreeNode->Parent->LelfChild)
		{//当前节点是父节点的左孩子
			Brother=TreeNode->Parent->RightChild;

			if(Brother->Color==Node::RED) 
			{//CASE 1
				Brother->Color=Node::BLACK;
				TreeNode->Parent->Color=Node::RED;
				LeftRotate(TreeNode->Parent);
				Brother=TreeNode->Parent->RightChild;
			}

			if(Brother->LelfChild->Color==Node::BLACK&&Brother->RightChild->Color==Node::BLACK)
			{//CASE 2
				Brother->Color=Node::RED;
				TreeNode=TreeNode->Parent;
			}
			else
			{
				if(Brother->RightChild->Color==Node::BLACK)
				{//CASE 3
					Brother->LelfChild->Color=Node::BLACK;
					Brother->Color=Node::RED;
					RightRotate(Brother);
					Brother=TreeNode->Parent->RightChild;
				}
				//CASE 4
				Brother->Color=TreeNode->Parent->Color;
				TreeNode->Parent->Color=Node::BLACK;
				Brother->RightChild->Color=Node::BLACK;
				LeftRotate(TreeNode->Parent);
				TreeNode=m_Head;	//退出
			}
			// end TreeNode==TreeNode->Parent->LelfChild
		}
		else			//对称
		{//当前节点是父节点的右孩子
			Brother=TreeNode->Parent->LelfChild;
			if (Brother->Color==Node::RED)
			{//CASE 1
				Brother->Color=Node::BLACK;
				TreeNode->Parent->Color=Node::RED;
				RightRotate(TreeNode->Parent);
				Brother=TreeNode->Parent->LelfChild;
			}
			if(Brother->LelfChild->Color==Node::BLACK&&Brother->RightChild->Color==Node::BLACK)
			{//CASE 2
				Brother->Color=Node::RED;
				TreeNode=TreeNode->Parent;
			}
			else
			{
				if(Brother->LelfChild->Color==Node::BLACK)
				{//CASE 3
					Brother->RightChild->Color=Node::BLACK;
					Brother->Color=Node::RED;
					LeftRotate(Brother);
					Brother=TreeNode->Parent->LelfChild;
				}
				//CASE 4
				Brother->Color=TreeNode->Parent->Color;
				TreeNode->Parent->Color=Node::BLACK;
				Brother->LelfChild->Color=Node::BLACK;
				RightRotate(TreeNode->Parent);
				TreeNode=m_Head;	//退出
			}
		}
	}//end while 调整平衡树

	TreeNode->Color=Node::BLACK;
}

template<class KEY, class TYPE, class _CMP> 
VOID CDuiMap<KEY,TYPE,_CMP>::Clear()
{
	DeleteTree(m_Head);
	m_Head=&m_Snil;
	m_Count=0;
}

template<class KEY, class TYPE, class _CMP> 
inline UINT CDuiMap<KEY,TYPE,_CMP>::Count()
{
	return m_Count;
}

template<class KEY, class TYPE, class _CMP> 
CDuiMap<KEY,TYPE,_CMP>& CDuiMap<KEY,TYPE,_CMP>::operator= (const CDuiMap<KEY,TYPE,_CMP>& t)
{
	Clear();
	CDuiMap<KEY,TYPE,_CMP>::Iterator it = t.Min();
	for (; it; it++)
	{
		Insert(KEY(it->Key),TYPE(it->Value));
	}

	return *this;
}


};//namespace msdk{
