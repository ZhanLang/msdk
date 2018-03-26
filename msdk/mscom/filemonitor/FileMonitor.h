#pragma once
#include "thread/SubThreadImp.h"
#include <map>
#include "SyncObject/criticalsection.h"
#include <winioctl.h>
#include <Shlobj.h>
#include <filemonitor/IFileMonitor.h>

#include <shobjidl.h>
class CFileMonitor
	:public IFileMonitor
	,protected CSubThread
	,protected CUnknownImp
{
public:
	typedef  HANDLE HEVENT;
	typedef  HANDLE HVOLUME; 

	typedef std::map<HEVENT,HVOLUME> CVolumeEventMap;
	typedef std::map<std::wstring,HEVENT,strless<std::wstring>>	 CVolumeHandleMap;	//保存映射

	typedef IShellDetails	FAR*	PSHELLDETAILS;
	typedef IShellFolder2	FAR*	LPSHELLFOLDER2;

	struct CFileOverlappedPoint
	{
		CFileOverlappedPoint()
		{
			lpPoint = NULL;
			ZeroMemory(&overlapped,sizeof(overlapped));
		}

		LPVOID lpPoint;
		OVERLAPPED overlapped;
	};
	typedef std::map<HEVENT,CFileOverlappedPoint>  CVolumeEventPointMap;

	UNKNOWN_IMP1(IFileMonitor);
public:

	CFileMonitor(void);
	~CFileMonitor(void);

	STDMETHOD(OpenMonitor)(LPCWSTR* szVolumes, DWORD dwCount, IOnFileMonitor* pOnVolumeMonitor);
	STDMETHOD(CloseMonitor)();
	STDMETHOD(AppendMonitor)(LPCWSTR szVolumes);
	STDMETHOD(RemoveMonitor)(LPCWSTR szVolumes = NULL);
protected:
	
	HRESULT	Run();

private:
	HVOLUME GetVolume(HEVENT hEvent);
	LPCTSTR	GetVolumeName(HEVENT hEvent);

	//通过回收站路劲获取该文件源路径
	BOOL GetRecycleFileDisplayName(LPCTSTR lpszRecycleFile, LPTSTR lpszDisplayName, DWORD dwDisplayNameLen);
	BOOL GetGetRecycleFileDisplayName2(LPCTSTR lpszRecycleFile, LPTSTR lpszDisplayName, DWORD dwDisplayNameLen);
	BOOL GetFolder2 (void);
	BOOL GetFolder (void);
private:
	HEVENT m_appendMonitor;
	UTIL::com_ptr<IOnFileMonitor> m_pOnVolumeMonitor;
	CVolumeEventMap		m_VolumeEventMap;
	CVolumeHandleMap	m_VolumeHandleMap;
	CVolumeEventPointMap m_VolumeEventPointMap;
	DECLARE_AUTOLOCK_CS(VolumeMap);

	//回收站
	LPSHELLFOLDER2	m_pFolder2;
	LPSHELLFOLDER	m_pRecycleBin;
};
