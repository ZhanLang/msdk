/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,16th,2014

Module Name:

	IdsActivity.h 

Abstract: listview接口


*/
/************************************************************************/


#ifndef __IDSLISTVIEW_H__
#define __IDSLISTVIEW_H__

#define UPDATE_LAYOUT_REFRESH	0
#define UPDATE_LAYOUT_INSERT	1
#define UPDATE_LAYOUT_DELETE	2
#define UPDATE_LAYOUT_EXCHANGE	3

typedef struct tagUpdateLayoutInfo{
	IdsUIBase *pbaseParent;
	int nFlag;
	int nItemFirst;
	int nItemLast;
	CSize sizeItem;		//所有属性只能改这个，改了其他的后果自负
	CPoint ptPrevBottomRight;
	tagUpdateLayoutInfo():pbaseParent(NULL),nFlag(UPDATE_LAYOUT_REFRESH), nItemFirst(-1), nItemLast(-1){}
}ULI,*PULI;


typedef BOOL (CALLBACK *ENUMUIPROC_USER)(IdsUIBase *pbase, LPARAM lParam, int nIndex);

interface DSUI_API IdsListView//:public IdsUIBase
{
public:
	virtual BOOL SetItemLayoutInfo(__in LPSIZE lpsizeItem, __in LPRECT lprcPadding, int xyDisOfItem, BOOL bVert) DSUI_PURE;
	virtual BOOL SetDefaultItemClass(__in LPCWSTR lpszClass) DSUI_PURE;
	virtual IdsUIBase *InsertItem(__in LPCWSTR lpszClass, __in LPCWSTR lpszUIName, int nIndex, LPARAM lParam, BOOL bUpdateLayout) DSUI_PURE;
	virtual void UpdateItemLayout(__in PULI puli) DSUI_PURE;
	virtual BOOL SetExtLayoutProc(__in ENUMUIPROC_USER prcExtLayout) DSUI_PURE; //this lParam is
	virtual void SetVisualOffset(int x, int y, BOOL bLayout) DSUI_PURE;
	virtual CSize GetVisualOffset() DSUI_PURE;
	virtual CSize GetTotalDrawSize() DSUI_PURE;
	virtual void DeleteAllItem() DSUI_PURE;
};








#endif /*__IDSLISTVIEW_H__*/