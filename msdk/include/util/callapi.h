
#if !defined(_INCLUDE_CALLAPI_H__)
#define _INCLUDE_CALLAPI_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace msdk {

#define msoffsetbuf(m) offsetof(DerivedType, m)

class __declspec(novtable) IImpModuleBase
{
public:
	struct STFunDesc
	{
		char *pFunName;
		size_t uOffFun;
	};
public:
	enum{
		FLAG_USEHMODULE=1,
			FLAG_LOADED=2,
	};
	char *m_pModuleName;	
	HMODULE m_hMod;
	unsigned int m_uFlags;		
	virtual void InitIAT()
	{
		STFunDesc *pFunDesc=GetFunDefs();
		while(pFunDesc->pFunName)
		{
			*(void**)(((char*)this)+pFunDesc->uOffFun)=(void*)0;			
			pFunDesc++;
		}
	}
public:
	IImpModuleBase(HMODULE hMod):m_pModuleName(NULL),m_hMod(hMod),m_uFlags(FLAG_USEHMODULE)
	{		
	}
	IImpModuleBase(char *pModuleName):m_pModuleName(pModuleName),m_hMod(NULL),m_uFlags(FLAG_LOADED)
	{
	}
	virtual ~IImpModuleBase(){UnLoad();}	
	virtual int Load(LPCSTR szPath = NULL)
	{
		STFunDesc *pFunDesc=GetFunDefs();
		int iRet=0;
		HMODULE hMod;
		if (m_uFlags & FLAG_USEHMODULE)
			hMod=m_hMod;
		else// &FLAG_LOADED
		{
			if(szPath && szPath[0] && m_pModuleName)
			{
				CHAR buf[260] = {0};
				lstrcpyA(buf, szPath);

				if(lstrlenA(m_pModuleName))
				{
					if(szPath[lstrlenA(szPath)-1]!='\\')
						lstrcatA(buf, "\\");
					lstrcatA(buf, m_pModuleName);
				}
				
				hMod=m_hMod=LoadLibraryA(buf);
			}
			else
				hMod=m_hMod=LoadLibraryA(m_pModuleName);
		}
		if (!hMod)
			iRet=-1;
		else
			while(pFunDesc->pFunName)
			{
				void *p=GetProcAddress(hMod,pFunDesc->pFunName);
				*(void**)(((char*)this)+pFunDesc->uOffFun)=p;
				if (!p)
				{
					iRet++;
					if(Exception(iRet, hMod, pFunDesc->pFunName))
						return iRet;
				}
				pFunDesc++;
			}			
		return iRet;
	}
	virtual int UnLoad()
	{
		if (m_uFlags & FLAG_LOADED)
		{
			if(m_hMod)
			{
				FreeLibrary(m_hMod);				
			}
			m_hMod=NULL;
			//InitIAT();
		}
		return 0;
	}
	virtual BOOL IsLoaded(){return NULL!=m_hMod;}
	virtual BOOL Exception(int iCount, HMODULE hModule, LPCSTR lpProcName){return TRUE;}//default: terminal
	virtual STFunDesc *GetFunDefs() = 0;
};

template <class tDerived>
class __declspec(novtable) tImpModuleMid : public IImpModuleBase
{	
public:	
	typedef tDerived DerivedType;
	inline tImpModuleMid(char *pName) : IImpModuleBase(pName){}
	inline tImpModuleMid(HMODULE hMod) : IImpModuleBase(hMod){}
};

#define DECLARE_FUN_BEGIN(c, d)	public:\
	inline c##(char* modname) : tImpModuleMid<##c>(modname){InitIAT();}\
	inline c##(HMODULE hMod) : tImpModuleMid<##c>(hMod){InitIAT();}\
	inline c##() : tImpModuleMid<##c>(d){InitIAT();}\
	virtual STFunDesc *GetFunDefs()\
	{	static STFunDesc s_FunDefs[]={

#define DECLARE_FUN_BEGIN_EX(c, h)	public:\
	inline c##(char* modname) : tImpModuleMid<##c>(modname){InitIAT();}\
	inline c##(HMODULE hMod) : tImpModuleMid<##c>(hMod){InitIAT();}\
	inline c##() : tImpModuleMid<##c>(h){InitIAT();}\
	virtual STFunDesc *GetFunDefs()\
	{	static STFunDesc s_FunDefs[]={

#define DECLARE_FUNALT(f,n) {#n,msoffsetbuf(f)},
#define DECLARE_FUN(f) {#f,msoffsetbuf(f)},

#define DECLARE_FUN_END() {NULL,0},};\
	return s_FunDefs;}

} //namespace msdk
#endif // !defined(_INCLUDE_CALLAPI_H__)
