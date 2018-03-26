
#pragma once
struct IDownloadTaskEx : public IMSBase
{
	STDMETHOD(SetCreateTime)(const FILETIME& time) = 0;
	STDMETHOD(SetTaskState)(DL_TaskState state) = 0;
	STDMETHOD(SetTaskError)(DL_Error error) = 0;

	STDMETHOD_(DWORD, GetSpeed2)() = 0;

	STDMETHOD(SetTotalSize)(LONGLONG llTotalSize) = 0;
	STDMETHOD(SetCurSize)(LONGLONG llCurSize) = 0;
	
};

MS_DEFINE_IID(IDownloadTaskEx, "{EF37CFB6-3D7A-4E51-B4EF-32890DBF797E}");