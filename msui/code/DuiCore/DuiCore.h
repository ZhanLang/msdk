#pragma once

#include <DuiImpl/DuiVector.h>
#include <DuiImpl/DuiString.h>
#include <DuiImpl/DuiMap.h>
#include <DuiCore/IDuiCore.h>
#include <DuiImpl/DuiCriticalsection.h>
#include <DuiCore/IDuiSkin.h>
#include <DuiCore/IDuiFont.h>
#include <DuiCore/IDuiLang.h>
using namespace DuiKit;
class CDuiCore : 
	public IDuiCore
{

public:
	DUI_BEGIN_DEFINE_INTERFACEMAP(IDuiCore, OBJECT_CORE)
		DUI_DEFINE_INTERFACE(IDuiCore, IIDuiCore)
	DUI_END_DEFINE_INTERFACEMAP;
	virtual VOID SetAttribute(LPCWSTR lpszName, LPCWSTR lpszValue){}
	virtual HRESULT DoCreate(IDuiObject* pParent, IDuiCore* pCore, IDuiBuilder* pBuilder){return TRUE;}


	CDuiCore();
	~CDuiCore();

	virtual BOOL InitDuiCore();
	virtual BOOL UnInitDuiCore();

	virtual VOID SetModuleInstance(HINSTANCE hInstance);
	virtual HINSTANCE GetModuleInstace();

	virtual IDuiObject* GetSignalObject(LPCWSTR lpszName);
	virtual VOID AddSignalObject(LPCWSTR lpszName, IDuiObject* pObject);
	virtual VOID RemoveSignalObject(LPCWSTR lpszName);
	
	virtual BOOL PostMessage(IDuiControlCtrl* pCtrl,INT nMsg,IDuiControlCtrl* pToCtrl = NULL,WPARAM wParam = NULL,LPARAM lParam = NULL,LPVOID	lpData = NULL);
	virtual LRESULT SendMessage(IDuiControlCtrl* pCtrl,INT nMsg,IDuiControlCtrl* pToCtrl = NULL,WPARAM wParam = NULL,LPARAM lParam = NULL,LPVOID	lpData = NULL);

	virtual VOID AddPreMessageFilter(IDuiPreMessageFilter* pFilter);
	virtual VOID RemovePreMessageFilter(IDuiPreMessageFilter* pFilter);

	virtual VOID AddProMessageFilter(IDuiProMessageFilter* pFilter);
	virtual VOID RemoveProMessageFilter(IDuiProMessageFilter* pFilter);

	virtual VOID AddTranslateAccelerator(IDuiTranslateAccelerator* pFilter);
	virtual VOID RemoveTranslateAccelerator(IDuiTranslateAccelerator* pFilter);

	virtual IDuiSkin* GetSkin(LPCWSTR lpszSkinName);
	virtual VOID AddSkin(LPCWSTR lpszSkinName, IDuiSkin* pTheme);
	virtual VOID RemoveSkin(LPCWSTR lpszSkinName);

	virtual VOID SetCurrentLang(LPCWSTR lpszLangName);
	virtual LPCWSTR GetCurrentLang();

	virtual DWORD MessageLoop();
	virtual BOOL  Term(DWORD dwExitCode);
	BOOL TranslateMessage(const MSG &msg);
private:
	
	BOOL PreMessageFilter(const MSG &msg );
	BOOL ProMessageFilter(const MSG &msg);
private:
	typedef CDuiMap<CDuiString, IDuiObject*> CSignalObjectMap;
	CSignalObjectMap m_SignalObjectMap;
	DECLARE_AUTOLOCK_CS(m_SignalObjectMap);

	typedef CDuiVector<IDuiPreMessageFilter*> CDuiPreMessageFilterSet;
	CDuiPreMessageFilterSet m_DuiPreMessageFilterSet;
	DECLARE_AUTOLOCK_CS(m_DuiPreMessageFilterSet);

	typedef CDuiVector<IDuiProMessageFilter*> CDuiProMessageFilterSet;
	CDuiProMessageFilterSet m_DuiProMessageFilterSet;
	DECLARE_AUTOLOCK_CS(m_DuiProMessageFilterSet);

	typedef CDuiVector<IDuiTranslateAccelerator*> CDuiTranslateAcceleratorSet;
	CDuiTranslateAcceleratorSet m_DuiTranslateAcceleratorSet;
	DECLARE_AUTOLOCK_CS(m_DuiTranslateAcceleratorSet);

	typedef CDuiMap<CDuiString, IDuiSkin*> CDuiSkinMap;
	CDuiSkinMap m_SkinMap;
	DECLARE_AUTOLOCK_CS(m_SkinMap);


	

	HINSTANCE m_hInstance;
	HANDLE m_hThread;
	CDuiString m_strCurrentLang;
};