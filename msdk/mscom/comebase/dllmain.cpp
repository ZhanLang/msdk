// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <map>
#include <string>
#include "MsBufferManangerImp.h"
#include "mscom\tproperty.h"
#include "StringMgr.h"

#include "combase/filestream.h"
#include "combase/HeapMemImp.h"
#include "ObjeceQueue.h"
#include "MsVector.h"
#include "ObjectArray.h"
USE_DEFAULT_DLL_MAIN
BEGIN_CLIDMAP
		CLIDMAPENTRY_BEGIN

			//buffer
			CLIDMAPENTRY_NOROT_PROGID(CLSID_MsBufferMgr , CMsBufferManangerImp,_T("MsBufferMgr.1")) //缓冲区
			CLIDMAPENTRY_NOROT_PROGID(CLSID_MsBuffer , CMsBufferImp<CNullCriticalSection> , _T("MsBufferImp.1"))
			CLIDMAPENTRY_NOROT_PROGID(CLSID_MsBuffer_Safe ,CMsBufferImp<CAutoCriticalSection>, _T("MsBufferImpSafe.1"))

			//属性
 			CLIDMAPENTRY_PROGID(CLSID_CProperty, CProperty, _T("Property.1"))
 			CLIDMAPENTRY_PROGID(CLSID_CProperty_thread, CPropertyThread, _T("PropertyThread.1"))
 			CLIDMAPENTRY_PROGID(CLSID_CProperty2, CProperty2, _T("Property2.1"))
 			CLIDMAPENTRY_PROGID(CLSID_CProperty2_thread, CProperty2Thread, _T("Property2Thread.1"))
			CLIDMAPENTRY_PROGID(CLSID_CPropertyStr, CPropertyStr, _T("PropertyStr.1"))
			CLIDMAPENTRY_PROGID(CLSID_CPropertyStr_thread, CPropertyStrThread, _T("PropertyStrThread.1"))

			//vector
			CLIDMAPENTRY_NOROT_PROGID(CLSID_MsDwordVector , CMsDwordVector, _T("CLSID_MsDwordVector.1"))
			CLIDMAPENTRY_NOROT_PROGID(CLSID_MsSafeDwordVector, CMsSafeDwordVector, _T("CLSID_MsSafeDwordVector.1"))

			//string
			CLIDMAPENTRY_PROGID(CLSID_StringA , MStringA<CNullCriticalSection> , _T("MStringA.1"))
			CLIDMAPENTRY_PROGID(CLSID_StringW , MStringW<CNullCriticalSection> , _T("MStringW.1"))
			CLIDMAPENTRY_PROGID(CLSID_StringA_Safe , MStringA<CAutoCriticalSection> , _T("MStringA_Safe.1"))
			CLIDMAPENTRY_PROGID(CLSID_StringW_Safe , MStringW<CAutoCriticalSection> , _T("MStringW_Safe.1"))
			CLIDMAPENTRY_PROGID(CLSID_StringMgr , MStringMgr , _T("MStringMgr.1"))


			//stream
			CLIDMAPENTRY_PROGID(CLSID_MsFileStream , CFileStream<CNullCriticalSection> , _T("FileStream.1"))
			CLIDMAPENTRY_PROGID(CLSID_MsFileStream_Safe , CFileStream<CAutoCriticalSection> , _T("FileStream_Safe.1"))

			//heap
			CLIDMAPENTRY_NOROT_PROGID(CLSID_HeapMem , CHeapMem, _T("CLSID_HeapMem.1"))


			//queue
			CLIDMAPENTRY_NOROT_PROGID(CLSID_ObjeceQueue , CObjeceQueue, _T("CLSID_ObjeceQueue.1"))


			//array
			CLIDMAPENTRY_NOROT_PROGID(CLSID_ObjectArray , CObjectArray<CNullCriticalSection> , _T("FileStream.1"))
			CLIDMAPENTRY_NOROT_PROGID(CLSID_SafeObjectArray , CObjectArray<CAutoCriticalSection> , _T("FileStream_Safe.1"))

			
		CLIDMAPENTRY_END
END_CLIDMAP
DEFINE_ALL_EXPORTFUN

 //CMsVector<CNullCriticalSection, DWORD> v;
/*

<moudle guid='{B50E1612-AC97-4b4d-9DB9-894F89DE0203}'  path='.\Common File\mscom\comebase.dll'>
	<!-- buffer-->
	<class clsid='{98FB74FA-6BE0-401b-B986-A78A80EED7A1}'  progid='MsBufferMgr.1'  name='CLSID_MsBufferMgr'/>
	<class clsid='{B021C8D6-7D21-4F41-99ED-259DD3DB996B}'  progid='MsBufferImp.1'  name='CLSID_MsBuffer_Safe'/>

	<!-- Property-->
	<class clsid='{ED20E0E5-2357-4825-B3FA-198AEC674E81}'  progid='Property.1'  name='CLSID_CProperty'/>
	<class clsid='{AD4F3A47-0CD6-43de-BC22-E8BE24FFD424}'  progid='PropertyThread.1'  name='CLSID_CProperty_thread'/>
	<class clsid='{2100E98D-B13E-4306-8081-50F325B10586}'  progid='Property2.1'  name='CLSID_CProperty2'/>
	<class clsid='{0AEF80FB-9BAF-4e66-96B3-784ED0FCECF1}'  progid='Property2Thread.1'  name='CLSID_CProperty2_thread'/>

	<!--string-->
	<class clsid='{34A5F0CC-E038-4C0E-AE02-6291C34248FA}'  progid='MStringA.1'  name='CLSID_StringA'/>
	<class clsid='{78B6343D-6D65-4F09-9AE4-2958E057B5FB}'  progid='MStringW.1'  name='CLSID_StringW'/>
	<class clsid='{FA208B82-7587-4dfc-BB3D-8CF85278F366}'  progid='MStringA_Safe.1'  name='CLSID_StringA_Safe'/>
	<class clsid='{9EC1DD66-5C82-4b66-B23A-0596983F9684}'  progid='MStringW_Safe.1'  name='CLSID_StringW_Safe'/>
	<class clsid='{860FAAF4-3E28-46d3-8EF6-DCD021FDCA53}'  progid='MStringMgr.1'  name='CLSID_StringMgr'/>
	<!--heap-->
	<class clsid='{2E0B4570-415B-4f47-96B7-85700C113ED7}'  progid='CLSID_HeapMem.1'  name='CLSID_HeapMem'/>
</moudle>
*/