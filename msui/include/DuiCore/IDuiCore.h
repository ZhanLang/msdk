#pragma once

namespace DuiKit{;

struct IDuiCore;
struct IDuiSkin;
struct IDuiLangSet;
struct IDuiControlCtrl;
struct IDuiBuilder;
struct DuiMsg
{
	IDuiControlCtrl* CtrlFrom;
	IDuiControlCtrl* CtrlTo;
	INT		 nMsg;
	WPARAM	 wParam;
	LPARAM	 lParam;
	LPVOID	 lpData;
};

struct DuiEvent
{
	IDuiControlCtrl* Ctrl; //事件的触发者
	INT nEvent;
	WPARAM wParam;
    LPARAM lParam;
};

struct IDuiPreMessageFilter
{
	virtual LRESULT OnPreMessageFilter(const DuiMsg& duiMsg, BOOL& bHandle) = 0;
};

struct IDuiProMessageFilter
{
	virtual LRESULT OnProMessageFilter(const DuiMsg& duiMsg, BOOL& bHandle) = 0;
};

struct IDuiTranslateAccelerator
{
	virtual LRESULT OnTranslateAccelerator(const MSG& pMsg, BOOL& bHandle) = 0;
};

struct IDuiRoutedEventHandler
{
	/*返回FALSE 终止向上传递*/
	virtual BOOL OnRoutedEvent(const DuiEvent& event) = 0;
};

//所有对象都应继承该基类
struct IDuiObject
{
	virtual LPCWSTR GetObjectClassName() = 0;
	virtual IDuiObject* QueryInterface(IIDef iid) = 0;
	virtual VOID DeleteThis() = 0;

	//为了方便从XML直接设置属性
 	virtual VOID SetAttribute(LPCWSTR lpszName, LPCWSTR lpszValue) = 0;

	/*
		返回值：S_OK , S_FALSE , E_FAIL
			 S_OK	 ：成功，解析子节点
			 S_FALSE ：失败，解析子节点，在解析完成子节点之后删除。
			 E_FAIL	 ：失败，不解析子节点，并且删除。
	*/
 	virtual HRESULT DoCreate(IDuiObject* Parent, IDuiCore* Core, IDuiBuilder* pBuilder) = 0;
};

//对象创建回调
struct IDuiObjectBuilder
{
	virtual IDuiObject* CreateObject() = 0;
	virtual VOID	   DestoryObject(IDuiObject* Object) = 0;
};



//作为全局对象返回
struct IDuiObjectFactory
{
	virtual IDuiObject* CreateObject(LPCWSTR lpszObjectClassName) = 0;
	virtual VOID	   DestoryObject(IDuiObject* pObject) = 0;

	virtual VOID	   RegisterObjectBuilder(LPCWSTR lpszObjectClassName, IDuiObjectBuilder* pCallBack) = 0;
	virtual VOID	   UnRegisterObjectBuilder(LPCWSTR lpszObjectClassName) = 0;
};


struct IDuiCoreCallBack
{
	virtual LRESULT OnCoreCallBack( IDuiObject* pCtrl, int nCallBack) = 0;
};

/*
核心接口
*/
struct IDuiCore : public IDuiObject
{
	virtual BOOL InitDuiCore() = 0;
	virtual BOOL UnInitDuiCore() = 0;

	/*不负责释放操作*/
	virtual IDuiObject* GetSignalObject(LPCWSTR lpszName) = 0;
	virtual VOID AddSignalObject(LPCWSTR lpszName, IDuiObject* pObject) = 0;
	virtual VOID RemoveSignalObject(LPCWSTR lpszName) = 0;
	

	virtual VOID SetModuleInstance(HINSTANCE hInstance) = 0;
	virtual HINSTANCE GetModuleInstace() = 0;


	/*全局消息处理*/
	virtual VOID AddPreMessageFilter(IDuiPreMessageFilter* pFilter) = 0;
	virtual VOID RemovePreMessageFilter(IDuiPreMessageFilter* pFilter) = 0;

	virtual VOID AddProMessageFilter(IDuiProMessageFilter* pFilter) = 0;
	virtual VOID RemoveProMessageFilter(IDuiProMessageFilter* pFilter) = 0;

	virtual VOID AddTranslateAccelerator(IDuiTranslateAccelerator* pFilter) = 0;
	virtual VOID RemoveTranslateAccelerator(IDuiTranslateAccelerator* pFilter) = 0;

	virtual BOOL PostMessage(IDuiControlCtrl* pCtrl,INT nMsg,IDuiControlCtrl* pToCtrl = NULL,WPARAM wParam = NULL,LPARAM lParam = NULL,LPVOID	lpData = NULL) = 0;
	virtual LRESULT SendMessage(IDuiControlCtrl* pCtrl,INT nMsg,IDuiControlCtrl* pToCtrl = NULL,WPARAM wParam = NULL,LPARAM lParam = NULL,LPVOID	lpData = NULL) = 0;

	/*创建皮肤*/

	/*皮肤管理*/
	virtual IDuiSkin* GetSkin(LPCWSTR lpszSkinName) = 0;
	virtual VOID AddSkin( LPCWSTR lpszSkinName, IDuiSkin* pTheme ) = 0;
	virtual VOID RemoveSkin(LPCWSTR lpszSkinName) = 0;

	//设置当前语言类型
	virtual VOID SetCurrentLang(LPCWSTR lpszLangName) = 0;
	virtual LPCWSTR GetCurrentLang() = 0;


	virtual DWORD MessageLoop() = 0;
	virtual BOOL  Term(DWORD dwExitCode) = 0;
};

};