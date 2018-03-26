#ifndef __ISYNCOBJECT_H__
#define __ISYNCOBJECT_H__
namespace msdk {

interface ISyncObject : public IUnknown 
{
	STDMETHOD(Lock)() PURE;
	STDMETHOD(Unlock)() PURE;
};

MS_DEFINE_IID( ISyncObject, "{17707D42-873E-4dd6-B618-3CDA6B636E6B}" );

}//namespace msdk
#endif //__ISYNCOBJECT_H__