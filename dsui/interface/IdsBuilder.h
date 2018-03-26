#ifndef IdsBuilder_h__
#define IdsBuilder_h__

class DSUI_API CIdsBuilder
{
public:
	typedef map<DWORD,CRect> MAP_MARKEDRECT;

	CIdsBuilder();
	~CIdsBuilder();

	void builder();
	void Attach(HWND hWnd);
	void AssignXMLPath(LPCTSTR szFilePath);
	void SetBorderPadding(int l, int t, int r, int b, int c);
	
	IdsUIBase* GetDsuiElement(LPARAM lParam, LPVOID pUserInfo);
	IdsUIBase* GetDsuiElement(WORD wUIID);

	void OnSize(UINT nType, CSize size);
	UINT OnNcHitTest(CPoint point);
	BOOL OnEraseBkgnd(CDCHandle dc);
	BOOL OnNcActivate(BOOL bActive);
	LRESULT OnNcCalcSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

private:
	void InitWindow();
	void InitSkin();
	void InitUI();
	void InitFont();
	void InitImage();
	void InitElement(TiXmlElement* pElement, IdsUIBase* pParentBase=NULL);
	void AddClientHitRect(DWORD dwMark, LPRECT lprc);

	DWORD GetSysVer(PDWORD dwMinorVersion /*= NULL*/);

	// XML½âÎö
	static void SplitString(CString strSrc, TCHAR cSplit, vector<CString> &vecDest);
	static void ConvertGBKtoUTF8( LPCSTR lptcsValue, LPSTR lptcsBuffer, int nBufferLen );
	static void ConvertUTF8toGBK( LPCSTR lptcsValue, LPTSTR lptcsBuffer, int nBufferLen );
	static BOOL GetElementAttrValue( TiXmlElement *pElement, LPCSTR lptcsNodeAttr, LPTSTR lptcsAttrValue, int nBufferLen );
	static BOOL GetElementAttrIntValue( TiXmlElement *pElement, LPCSTR lptcsNodeAttr, int &nValue );
	static BOOL GetElementAttrBooleanValue( TiXmlElement *pElement, LPCSTR lptcsNodeAttr, BOOL &bValue );
	static BOOL GetElementAttrColorValue( TiXmlElement *pElement, LPCSTR lptcsNodeAttr, DWORD &dwValue );
	static BOOL GetElementAttrRectValue( TiXmlElement *pElement, LPCSTR lptcsNodeAttr, LPRECT lprcPos );

private:
	CString m_strXMLPath;
	int m_nBorderLeft;
	int m_nBorderTop;
	int m_nBorderRight;
	int m_nBorderBottom;
	int m_nBorderCaption;

	CWindow m_wndAttached;
	MAP_MARKEDRECT m_mapClientHitRect;
	IdsActivity *m_pactDef;
};

#endif // IdsBuilder_h__