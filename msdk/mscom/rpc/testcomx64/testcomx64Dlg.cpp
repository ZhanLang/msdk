
// testcomx64Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "ItemOprDlg.h"
#include "testcomx64.h"
#include "testcomx64Dlg.h"

#include <productpath\rspathbase.h>

#include <psapi.h>
#pragma comment(lib, "psapi.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



static CString  GetProductType()
{
	TCHAR szProductINI[64] = {0};
	TCHAR szNetConfig[MAX_PATH] = {0};
	TCHAR szPath[MAX_PATH] = {0};

	// 获取安装目录
	::GetModuleFileName(::GetModuleHandle(NULL), szPath, sizeof szPath);
	LPTSTR pszP = _tcsrchr(szPath, TEXT('\\'));
	if ( pszP )	*pszP = 0;
	_tcscpy_s(szNetConfig, MAX_PATH, szPath);
	_tcscat_s(szNetConfig, MAX_PATH,  TEXT("\\netconfig.ini"));

	//读配置文件
	GetPrivateProfileString(TEXT("SETTING"), TEXT("INI"), TEXT("rav.ini"), szProductINI, 64, szNetConfig);
	pszP = _tcsrchr(szProductINI, TEXT('.'));
	if ( pszP )	*pszP = 0;

	return CString(szProductINI);
}

#define NT4_SYSTEM_PROCESS 0x2
#define W2K_SYSTEM_PROCESS 0x8
#define WXP_SYSTEM_PROCESS 0x4
BOOL GetModuleNameByProcessId(DWORD ProcessId,TCHAR *pMainModuleName,int NameLength, TCHAR *pszCmdLine, int nCmdLineLen)
{
	HANDLE hProcess;
	TCHAR ModuleFileName[1024] = {0};
	HMODULE hModule[10] = {0};
	DWORD Needed;

	if( ProcessId==NT4_SYSTEM_PROCESS ||
		ProcessId==W2K_SYSTEM_PROCESS ||
		ProcessId==WXP_SYSTEM_PROCESS )
	{
		_tcscpy_s(pMainModuleName, NameLength, _T("System"));
		return	TRUE;
	}

	if( ProcessId==0)
	{
		_tcscpy_s(pMainModuleName, NameLength, _T("Idle"));
		return	FALSE;
	}

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,ProcessId);
	if(hProcess)
	{		
		if(EnumProcessModules(hProcess,hModule,sizeof(hModule),&Needed))
		{
			GetModuleBaseName(hProcess,hModule[0], ModuleFileName, sizeof(ModuleFileName));
		}

		//cmd line
		try
		{			
			DWORD	dwP1 = (DWORD)GetCommandLine;			

			if(dwP1!=0)
			{
				dwP1 = dwP1 + 1;
				dwP1 = *(DWORD*)dwP1;
				//TCHAR s[100] = { 0 };
				SIZE_T	cb;
				if(ReadProcessMemory(hProcess, (LPVOID)dwP1, &dwP1, 4, &cb))
				{
					ReadProcessMemory(hProcess, (LPVOID)dwP1, pszCmdLine, nCmdLineLen, &cb);						
				}				
			}		
		}
		catch(...)
		{
			;
		}
		CloseHandle(hProcess);
	}
	else
	{
		TRACE("OpenProcess [%d] ERROR = %d\n", ProcessId, GetLastError());
	}

	if(ModuleFileName[0]==0)
	{
		return	FALSE;
	}

	if(pMainModuleName)
		_tcsncpy_s(pMainModuleName,NameLength, ModuleFileName,NameLength);
	return NULL!=hProcess;
}

static CString ShowProcessInfo(DWORD dwPid)
{	
	CString str;
	TCHAR ModuleName[MAX_PATH] = {0};
	TCHAR CmdLine[MAX_PATH] = {0};
	TCHAR ProcessName[MAX_PATH] = {0};
	if(GetModuleNameByProcessId(dwPid,
		ModuleName,
		MAX_PATH-1,
		CmdLine,
		MAX_PATH-1))
	{
		TCHAR *p = _tcsrchr(ModuleName, _T('\\'));
		if(p)
			lstrcpyn(ProcessName, p+1, MAX_PATH-1);
		else
			lstrcpyn(ProcessName, ModuleName, MAX_PATH-1);

		//str.Format("%s, %s", ModuleName, CmdLine);
		str = CmdLine;
	}
	else
	{
		str = "描述的进程名或者进程ID没有找到!";
	}	
	return str;
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Ctestcomx64Dlg dialog




Ctestcomx64Dlg::Ctestcomx64Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ctestcomx64Dlg::IDD, pParent)
	, m_strSubsysName(_T(""))
	, m_strWorkPath(_T(""))
	, m_strOnMessages(_T(""))
	, m_defid(10000)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ctestcomx64Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strSubsysName);
	DDX_Text(pDX, IDC_EDIT_WORKPATH, m_strWorkPath);
	DDX_Text(pDX, IDC_EDIT_DEFID, m_defid);
	DDX_Control(pDX, IDC_COMBO_KNOWNSYS, m_cKnownSubsys);
	DDX_Text(pDX, IDC_EDIT_ONMESSAGE, m_strOnMessages);
	DDX_Control(pDX, IDC_LIST_OBJECT, m_cList);
}

BEGIN_MESSAGE_MAP(Ctestcomx64Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_LOGON, OnBnClickedButtonLogon)
	ON_BN_CLICKED(IDC_BUTTON_LOGOFF, OnBnClickedButtonLogoff)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnBnClickedButtonRefresh)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_OBJECT, OnNMRclickListObject)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OBJECT, OnNMDblclkListObject)
	ON_COMMAND(ID_RIGHT_SENDMESSAGE, OnRightSendmessage)
	ON_MESSAGE(WM_MY_ONMESSAGE, OnMyMessage)
	ON_CBN_SELCHANGE(IDC_COMBO_KNOWNSYS, OnCbnSelchangeComboKnownsys)
END_MESSAGE_MAP()


// Ctestcomx64Dlg message handlers

BOOL Ctestcomx64Dlg::OnInitDialog()
{
	AddRef();
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_cList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_cList.InsertColumn (0, _T("defid"), LVCFMT_LEFT, 100);
	m_cList.InsertColumn (1, _T("Grpid"), LVCFMT_LEFT, 50);
	m_cList.InsertColumn (2, _T("objid"), LVCFMT_LEFT, 60);
	m_cList.InsertColumn (3, _T("Pid"), LVCFMT_LEFT, 70);
	m_cList.InsertColumn (4, _T("Sid"), LVCFMT_LEFT, 50);
	m_cList.InsertColumn (5, _T("pobj"), LVCFMT_LEFT, 70);
	m_cList.InsertColumn (6, _T("type"), LVCFMT_LEFT, 40);
	m_cList.InsertColumn (7, _T("process"), LVCFMT_LEFT, 300);
	m_cList.InsertColumn (8, _T("subsys"), LVCFMT_LEFT, 70);


	m_cKnownSubsys.AddString(_T("rav"));
	m_cKnownSubsys.AddString(_T("ris"));
	m_cKnownSubsys.AddString(_T("rfw"));
	m_cKnownSubsys.AddString(_T("kaka"));
	m_cKnownSubsys.AddString(_T("rsa"));

	CString strCurSys = GetProductType();
	int index = m_cKnownSubsys.FindStringExact(0, strCurSys);
	if(index<0) index = m_cKnownSubsys.AddString(strCurSys);		
	m_cKnownSubsys.SetCurSel(index);

	OnCbnSelchangeComboKnownsys();

	ShowUI();

#ifdef X64
	SetWindowText(_T("test X64"));
#else
	SetWindowText(_T("test win 32"));	
#endif

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Ctestcomx64Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Ctestcomx64Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Ctestcomx64Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ctestcomx64Dlg::ShowUI()
{
	GetDlgItem(IDC_BUTTON_LOGON)->EnableWindow(m_pClient==NULL);
	GetDlgItem(IDC_BUTTON_LOGOFF)->EnableWindow(m_pClient!=NULL);
	GetDlgItem(IDC_BUTTON_REFRESH)->EnableWindow(m_pClient!=NULL);
}


LRESULT Ctestcomx64Dlg::OnMyMessage(WPARAM, LPARAM)
{
	SetDlgItemText(IDC_EDIT_ONMESSAGE, m_strOnMessages);
	//UpdateData(FALSE);
	return 0;
}

HRESULT Ctestcomx64Dlg::Dispatch(IMessage* pMsg)
{
	//TRACE("\tobjid=%d, defid=%d, maskid=%X, pid = %d, pobj=%d, susys=%08X\n", ci.objid, ci.defid, ci.maskid, ci.pid, ci.pobjid, ci.subsysid);
	MESSAGEINFO *pMsgInfo = NULL;		
	pMsg->GetMessageInfo(&pMsgInfo);

	m_strOnMessages.Format(_T("Dispatch from [%d, %X][object=%d], CallType = %X, msgid = %d(0x%x), InCch = %d"),
		pMsgInfo->source_defid,
		pMsgInfo->source_maskid,
		pMsgInfo->source_objid,
		pMsgInfo->dwCallType,		
		pMsgInfo->msgid,pMsgInfo->msgid,
		pMsgInfo->nInCch
		);

	PostMessage(WM_MY_ONMESSAGE);	
	return S_OK;
}

void Ctestcomx64Dlg::OnCbnSelchangeComboKnownsys()
{
	// 获取产品AppData
	int index = m_cKnownSubsys.GetCurSel();
	m_cKnownSubsys.GetLBText(index, m_strSubsysName);
	DWORD dwLen = MAX_BUFFER_LEN;
	ZeroMemory(m_szAppDatapPath, sizeof(m_szAppDatapPath));
	GetRsProductDataPath(m_szAppDatapPath, dwLen);	
	if ( m_szAppDatapPath[_tcslen(m_szAppDatapPath)-sizeof TCHAR] == TEXT('\\') )
		memset(&m_szAppDatapPath[_tcslen(m_szAppDatapPath)-sizeof TCHAR], 0, sizeof TCHAR);
	m_strWorkPath = m_szAppDatapPath;

	UpdateData(FALSE);
}

void Ctestcomx64Dlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonLogoff();
	OnOK();
}

void Ctestcomx64Dlg::OnBnClickedButtonLogon()
{	
	UpdateData(TRUE);

	// 加载commx3.dll	
	if(m_hComHelp.IsLoaded())
		return ;

	CString strComx3;
	GetDlgItemText(IDC_EDIT_COMX3, strComx3.GetBuffer(MAX_PATH*2), MAX_PATH);
	strComx3.ReleaseBuffer();
	strComx3.Replace(_T('/'), _T('\\'));
	if(strComx3.Find(_T("\\comx3.dll")) > 0)
	{
		strComx3.Replace(_T("\\comx3.dll"), _T(""));
	}

	{
#ifdef UNICODE
		USES_CONVERSION;
		if(0!=m_hComHelp.Load(W2A(strComx3)))
#else
		if(0!=m_hComHelp.Load(strComx3))
#endif
		{
			MessageBox(_T("加载comx3.dll失败"));
			return ;
		}
	}

#ifdef UNICODE
	USES_CONVERSION;
	m_pCCenter = m_hComHelp.MS_InitializeCallCenter(W2A(m_strSubsysName), W2A(m_strWorkPath), CCENTER_START_BOTH);
#else
	m_pCCenter = m_hComHelp.MS_InitializeCallCenter(m_strSubsysName, m_strWorkPath, CCENTER_START_BOTH);
#endif
	if(m_pCCenter==NULL)
	{
		m_hComHelp.UnLoad();
		MessageBox(_T("MS_InitializeCallCenter失败"));
		return ;
	}

	m_pClient = m_pCCenter->Logon(m_defid, 0, this);
	if(m_pClient==NULL)
	{
		m_hComHelp.MS_UninitializeCallCenter(m_pCCenter);
		m_hComHelp.UnLoad();
		m_pCCenter = INULL;
		return ;
	}

	OnBnClickedButtonRefresh();
	ShowUI();
}

void Ctestcomx64Dlg::OnBnClickedButtonLogoff()
{
	if ( m_pCCenter )
	{
		m_pCCenter->Logoff(m_pClient);
		m_pClient = INULL;

		m_hComHelp.MS_UninitializeCallCenter(m_pCCenter);
		m_hComHelp.MS_ShutDown(0);
		m_pCCenter = INULL;

		m_hComHelp.UnLoad();	
	}

	ShowUI();
}

void Ctestcomx64Dlg::OnBnClickedButtonRefresh()
{	
	SetDlgItemText(IDC_EDIT_ONMESSAGE, _T(""));
	if(m_pClient)
	{
		m_arList.RemoveAll();

		m_cList.DeleteAllItems();
		m_cList.SetRedraw(FALSE);
		int iItem = 0;
		TRACE("\nBEGIN LIST...\n");
		HANDLE32 hFind;
		for(HRESULT hr=m_pCCenter->QueryFirstObject(hFind);S_OK==hr;hr=m_pCCenter->QueryNextObject(hFind))
		{
			CLIENTINFO ci;
			if(S_OK==m_pCCenter->QueryObject(hFind, &ci))
			{
				TRACE("\tobjid=%d, defid=%d, maskid=%X, pid = %d, pobj=%d, subsys=%08X\n", 
					ci.objid, ci.defid, ci.maskid, ci.pid, ci.pobjid, ci.subsysid);
				INT_PTR nPos = m_arList.Add(ci);
				CString strItem;
				//strItem.Format("defid=%d, maskid=%X, objid=%d, pid = %d, pobj=%d, subsys=%08X", 
				//	ci.defid, ci.maskid, ci.objid, ci.pid, ci.pobjid, ci.subsysid);
				strItem.Format(_T("%d[0x%08x]"), ci.defid, ci.defid);	m_cList.InsertItem(iItem, strItem);
				strItem.Format(_T("%X"), ci.maskid);		m_cList.SetItemText(iItem, 1, strItem);
				strItem.Format(_T("%d"), ci.objid);			m_cList.SetItemText(iItem, 2, strItem);
				strItem.Format(_T("%d"), ci.pid);			m_cList.SetItemText(iItem, 3, strItem);
				strItem.Format(_T("%d"), ci.sid);			m_cList.SetItemText(iItem, 4, strItem);
				strItem.Format(_T("%d"), ci.pobjid);		m_cList.SetItemText(iItem, 5, strItem);
				strItem.Format(_T("%d"), ci.dwType);		m_cList.SetItemText(iItem, 6, strItem);
				m_cList.SetItemText(iItem, 7, ShowProcessInfo(ci.pid));
				strItem.Format(_T("%X"), ci.subsysid);		m_cList.SetItemText(iItem, 8, strItem);
				m_cList.SetItemData(iItem, nPos);

				iItem++;
			}
		}
		m_cList.SetRedraw(TRUE);
		TRACE("END LIST\n\n");
		TRACE("\n");
	}
}

void Ctestcomx64Dlg::OnNMRclickListObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_cList.GetFirstSelectedItemPosition();
	if (!pos)
		return ;

	m_iRightSelObject = m_cList.GetNextSelectedItem (pos);
	if(m_iRightSelObject >= 0)
	{
		CMenu m;
		m.LoadMenu (IDR_MENU_RIGHT);

		CPoint pt;
		GetCursorPos (&pt);

		m.GetSubMenu(0)->TrackPopupMenu (TPM_LEFTALIGN, pt.x, pt.y, this);
	}
	*pResult = 0;
}

void Ctestcomx64Dlg::OnNMDblclkListObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void Ctestcomx64Dlg::OnRightSendmessage()
{
	// TODO: Add your command handler code here
	if(m_iRightSelObject >= 0)
	{
		CLIENTINFO& ci = m_arList[m_iRightSelObject];
		TRACE("index=%d\tobjid=%d, defid=%d, maskid=%X, pid = %d, pobj=%d, subsys=%08X\n", 
			m_iRightSelObject, ci.objid, ci.defid, ci.maskid, ci.pid, ci.pobjid, ci.subsysid);

		CItemOprDlg dlg(ci, this);
		if(IDOK==dlg.DoModal())
		{
			int nrCount = dlg.m_rCount;
			if(nrCount <= 0)
				nrCount = 1;

			int i = 0;
			DWORD dwBeginTick = GetTickCount();
			for(; i < nrCount; i++)
			{
				if(FAILED(m_pClient->SendMessage(ci.defid, ci.maskid, dlg.m_iMsgid, (LPVOID)(LPCTSTR)dlg.m_strMsgInfo, dlg.m_strMsgInfo.GetLength(), 1000)))
					break;
			}

			DWORD dwTick = GetTickCount() - dwBeginTick;
			CString str;
			str.Format(_T("发送成功%d/%d, 耗时 %d 毫秒"), i, nrCount, dwTick);
			MessageBox(str, _T("发送结果"));
		}
	}
	else
		MessageBox(_T("选中无效"));
}
