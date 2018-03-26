

#ifndef _IXMLTREEITEM3_H
#define _IXMLTREEITEM3_H

#include <xml/ixmltree2.h>
//xml 解析库
//msxml3w.dll unicode 版本
//msxml3a.dll ansi    版本
namespace msdk {

interface IXMLTree3 : public IXMLTree2
{
	//************************************
	// Method:    GetXML
	// Returns:   int
	// Parameter: HXMLTREEITEM hItem
	// Parameter: LPTSTR pBuff
	// Parameter: int bufLen
	// Parameter: INT * nWritten
	//************************************
	virtual int GetXML( HXMLTREEITEM hItem, LPTSTR pBuff, int bufLen, INT* nWritten ) PURE;


	//************************************
	// Method:    CopyBranch				: Copy the entire branch rooted at source node
	// Returns:   void
	// Parameter: HXMLTREEITEM hDestNode
	// Parameter: HXMLTREEITEM hSourceNode
	//************************************
	virtual HXMLTREEITEM CopyBranch( HXMLTREEITEM hSourceNode ) PURE;
	

	//************************************
	// Method:    CopyBranchAsChild
	// Returns:   void
	// Parameter: HXMLTREEITEM hDestNode
	// Parameter: HXMLTREEITEM hSourceNode
	//************************************
	virtual HXMLTREEITEM CopyBranchAsChild( HXMLTREEITEM hDestNode, HXMLTREEITEM hSourceNode ) PURE;
};

#ifndef _UNICODE  //ansi 版本的xml
MS_DEFINE_IID( IXMLTree3, "{3E5744B1-9F52-4ce6-9777-8029FFE0F1C6}" );
// {6D52DF92-904D-43af-A222-5EC599DB78D0}
MS_DEFINE_GUID(CLSID_IXMLTreeA, 
			0x6d52df92, 0x904d, 0x43af, 0xa2, 0x22, 0x5e, 0xc5, 0x99, 0xdb, 0x78, 0xd0);
#else 
MS_DEFINE_IID( IXMLTree3, "{6D16D8C8-C3D6-4cf1-91B7-B0B71044808A}" );
// {35FD921E-B758-46d8-B0AA-FCD033B0E66D}
MS_DEFINE_GUID(CLSID_IXMLTreeW, 
			   0x35fd921e, 0xb758, 0x46d8, 0xb0, 0xaa, 0xfc, 0xd0, 0x33, 0xb0, 0xe6, 0x6d);
#endif
} //namespace msdk

#endif //_IXMLTREEITEM3_H





