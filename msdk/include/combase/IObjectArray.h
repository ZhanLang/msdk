#pragma once

namespace msdk{;
namespace mscom{;

struct IObjectArray : public IMSBase
{
	virtual VOID Push(IMSBase* pItem) = 0;

	virtual DWORD GetSize() = 0;
	virtual BOOL GetAt(DWORD dwAt, IMSBase** pItem) = 0;

	virtual BOOL RemoveAt(DWORD dwAt) = 0;
	virtual VOID Clear() = 0;
};

MS_DEFINE_IID(IObjectArray, "{6F8F6202-4FC5-4830-80CA-C304DA1E5AEB}");


};};



// {4A8FEF83-F0E5-4B2C-91B7-62184A6DF200}
MS_DEFINE_GUID(CLSID_ObjectArray, 
	0x4a8fef83, 0xf0e5, 0x4b2c, 0x91, 0xb7, 0x62, 0x18, 0x4a, 0x6d, 0xf2, 0x0);


// {E2852CEB-DFF0-43A3-9167-F0D4461365DF}
MS_DEFINE_GUID(CLSID_SafeObjectArray, 
	0xe2852ceb, 0xdff0, 0x43a3, 0x91, 0x67, 0xf0, 0xd4, 0x46, 0x13, 0x65, 0xdf);
