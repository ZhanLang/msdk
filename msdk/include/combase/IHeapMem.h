#pragma once

namespace msdk{;
namespace mscom{;


struct IHeapMem
	:public IMSBase
{
	STDMETHOD(CreateHeap)(DWORD dwInitialSize, DWORD dwMaximumSize = 0) = 0;
	STDMETHOD(DestroyHeap)() = 0;
	STDMETHOD_(LPVOID,AllocMemory)(DWORD dwSize, BOOL bInitZero) = 0;
	STDMETHOD_(LPVOID,ReallocMemory)(PVOID pPoint,DWORD dwSize) = 0;
	STDMETHOD(FreeMemory)(LPVOID lpPoint) = 0;
};

MS_DEFINE_IID(IHeapMem,"{D54E7957-327D-431b-861A-67CE3AEA327C}");

// {2E0B4570-415B-4f47-96B7-85700C113ED7}
MS_DEFINE_GUID(CLSID_HeapMem, 
			0x2e0b4570, 0x415b, 0x4f47, 0x96, 0xb7, 0x85, 0x70, 0xc, 0x11, 0x3e, 0xd7);

};}