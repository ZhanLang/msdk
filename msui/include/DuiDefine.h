#pragma once


#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif


#ifndef lengthof
#define lengthof(x) (sizeof(x)/sizeof(*x))
#endif

#ifndef MAX
#define MAX max
#endif

#ifndef MIN
#define MIN min
#endif

#ifndef CLAMP //取中间数
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))
#endif



#define RASSERT(x, _h_r_) { if(!(x)) return _h_r_; }
#define RASSERTV(x) { if(!(x)) return; }
#define RASSERTP(x, _h_r_) { if(NULL==(x)) return _h_r_; }
#define RASSERTPV(x) RASSERTP(x, ; )
#define RASSERT2(x, _t) { if(!(x)) {_t;} }
#define TASSERT(x, e) { if (!(x)) throw (e); }
#define RTEST(x, _h_r_) { if((x)) return _h_r_; }

#define DEFINE_OBJECT(_C, _N) static DuiKit::CUIObjectFactoryRegistHelp<_C> __g__##_C(_N);

//#define DEFINE_OBJECT_EX(_C, _I, _N) DuiKit::CUIObjectFactoryRegistHelp<_C, _I> __g__##_C(_N);


//////////////////////////////////////////////////////////////////////////
#define DUI_BEGIN_DEFINE_INTERFACEMAP(_I, _CN)\
	public:\
	virtual LPCWSTR GetObjectClassName(){return _CN;}\
	virtual VOID DeleteThis(){delete this;}\
	virtual IDuiObject* QueryInterface(IIDef __iid) {\
		_I* p = (_I*)(this);\
		if( __iid == IIDuiObject){ return (IDuiObject*)(p);}\
		
	
#define DUI_DEFINE_INTERFACE(_I, _IIDef)\
	if( __iid == _IIDef){ return (IDuiObject*)((_I*)this);}\

#define DUI_END_DEFINE_INTERFACEMAP return NULL; }
//////////////////////////////////////////////////////////////////////////

#define DUI_BEGIN_SETATTRIBUTE(__THIS) \
	virtual VOID SetAttribute(LPCWSTR lpszName, LPCWSTR lpszValue) {\
	if ( !(lpszName && wcslen(lpszName) && lpszValue && wcslen(lpszValue))){ return ;}

#define DUI_SETATTRIBUTE_STRING(__NAME, _VALUE)\
	if (_wcsicmp(lpszName,  L#__NAME) == 0)  { _VALUE = lpszValue;}
	
#define DUI_SETATTRIBUTE_STRING_FUNCTION(__NAME, _FUNCTION)\
	if (_wcsicmp(lpszName,  L#__NAME) == 0)  { _FUNCTION(lpszValue);}

#define DUI_SETATTRIBUTE_INT(__NAME, _VALUE)\
	if (_wcsicmp(lpszName,  L#__NAME) == 0)  \
	{ if(lpszValue[0] == L'#')  _VALUE = wcstoul(lpszValue+1, NULL, 16); else _VALUE = _wtoi(lpszValue);}

#define DUI_SETATTRIBUTE_INT_FUNCTION(__NAME, _FUNC)\
	if (_wcsicmp(lpszName,  L#__NAME) == 0)  \
	{ if(lpszValue[0] == L'#') _FUNC(wcstoul(lpszValue+1, NULL, 16)); else _FUNC(_wtoi(lpszValue));}


#define DUI_SETATTRIBUTE_BOOL(__NAME, _VALUE)\
	if (_wcsicmp(lpszName,  L#__NAME) == 0)  { _VALUE = _wtoi(lpszValue) == 0 ? FALSE : TRUE;}

#define DUI_SETATTRIBUTE_BOOL_FUNCTION(__NAME, _FUNCTION)\
	if (_wcsicmp(lpszName, L#__NAME) == 0)  { _FUNCTION(_wtoi(lpszValue) == 0 ? FALSE : TRUE);}

#define DUI_SETATTRIBUTE_SIZE(__NAME, _VALUE)\
	if (_wcsicmp(lpszName,  L#__NAME) == 0)  { swscanf_s(lpszValue,L"%d,%d", &_VALUE.cx, &_VALUE.cy);}

#define DUI_SETATTRIBUTE_SIZE_FUNCTION(__NAME, _FUNCTION)\
	if (_wcsicmp(lpszName,  L#__NAME) == 0)  { CDuiSize size; swscanf_s(lpszValue,L"%d,%d", &size.cx, &size.cy); _FUNCTION(size);}


#define DUI_SETATTRIBUTE_RECT(__NAME, _VALUE)\
	if (_wcsicmp(lpszName,  L#__NAME) == 0)  { swscanf_s(lpszValue,L"%d,%d,%d,%d", &_VALUE.left, &_VALUE.top,&_VALUE.right,&_VALUE.bottom);}

#define DUI_SETATTRIBUTE_RECT_FUNCTION(__NAME, _FUNCTION)\
	if (_wcsicmp(lpszName,  L#__NAME) == 0)  { \
		CDuiRect rect ; \
		swscanf_s(lpszValue,L"%d,%d,%d,%d", &rect.left, &rect.top,&rect.right,&rect.bottom);\
		_FUNCTION(rect);}

#define DUI_SETATTRIBUTE_POINT(__NAME, _VALUE)\
	if (_wcsicmp(lpszName,  L#__NAME) == 0)  { swscanf_s(lpszValue,L"%d,%d", &_VALUE.x, &_VALUE.y);}



#define DUI_BENGIN_SUB_SETATTRIBUTE(__NAME)\
	if (_wcsicmp(lpszName, L#__NAME) == 0){\

#define DUI_SUB_SETATTRIBUTE_FUNCTION(_SUBNAME, __FUNCTION, __VALUE)\
		if(_wcsicmp(lpszValue, L#_SUBNAME) == 0){__FUNCTION(__VALUE);}
	
#define DUI_END_SUB_SETATTRIBUTE }



#define DUI_SETATTRIBUTE_SUB_CLASS(__CLASS) __CLASS::SetAttribute(lpszName, lpszValue);

#define DUI_END_SETATTRIBUTE }


//////////////////////////////////////////////////////////////////////////
//消息定义
#define DUI_BEGIN_MSG_MAP( _Class)\
	virtual LRESULT DoMessage(const DuiMsg& duiMsg, BOOL& bHandle){ \
		LRESULT lResult = 0; if (0){}

#define DUI_MESSAGE_HANDLER(_Msg, _Func)\
	else if (duiMsg.nMsg == (_Msg)){\
	lResult = _Func(duiMsg.nMsg, duiMsg.CtrlFrom, duiMsg.CtrlTo, duiMsg.wParam, duiMsg.lParam, duiMsg.lpData, bHandle);\
	if( !bHandle ) return lResult;}\

#define DUI_MESSAGE_HANDLER_NO_PARAM(_Msg, _Func)\
	else if (duiMsg.nMsg == _Msg){\
	lResult = _Func();\
	if( !bHandle ) return lResult;}\

#define DUI_MESSAGE_HANDLER_HAVE_HANDELER(_Msg, _Func)\
	else if (duiMsg.nMsg == _Msg){\
	lResult = _Func(bHandle);\
	if( !bHandle ) return lResult;}\


#define DUI_END_MSG_MAP()\
	return __super::DoMessage(duiMsg, bHandle);}


#define DUI_MSG_TIMER_HANDLE(_TimerID, _Func)\
	else if (duiMsg.nMsg == DuiMsg_Timer && duiMsg.wParam == _TimerID){\
		lResult = _Func();\
		if( !bHandle ) return lResult;}\

#define DUI_END_MSG_MAP_SUPER()\
	return lResult;}
//////////////////////////////////////////////////////////////////////////
//事件定义

#define DUI_BEGIN_EVENT_MAP( _Class)\
	virtual BOOL OnRoutedEvent(const DuiEvent& event){ BOOL bHandle = FALSE;

#define DUI_EVENT_HANDLER(_Event, _Func)\
	if (event.nEvent == _Event){\
		bHandle =  _Func(event);\
	}\

#define DUI_END_EVENT_MAP()\
	return bHandle;}


//原始事件过滤器
#define DUI_BEGIN_CTRL_EVENT_MAP( _Class)\
	virtual BOOL DoEvent(const DuiEvent& event){ BOOL bHandle = TRUE; \

#define DUI_END_CTRL_EVENT_MAP() if(bHandle) return __super::DoEvent(event); return FALSE;}\

//////////////////////////////////////////////////////////////////////////
#ifndef GET_X_LPARAM
#	define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#	define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

//////////////////////////////////////////////////////////////////////////
//CDuiMap 操作帮助宏

#define DuiMapRemoveDuiObject(_Key, _Obj)	\

//////////////////////////////////////////////////////////////////////////

//核心对象
#define OBJECT_CORE						L"Core.Core"

//控件创建器
#define OBJECT_CORE_BUILD				L"Core.Build"

#define OBJECT_CORE_LANG				L"Lang"
#define OBJECT_CORE_LANGTEXT			L"LangText"
#define OBJECT_CORE_LANG_SET			L"Core.LangSet"

#define OBJECT_CORE_IMAGE				L"Image"
//流
#define OBJECT_CORE_BYTE_STREAM			L"Core.ByteStream"


/************************************************************************/
/*                                                                      */
/************************************************************************/
//文件类型皮肤
#define OBJECT_CORE_SKIN_FILE			L"Core.Skin.File"

//ZIP压缩包文件皮肤
#define OBJECT_CORE_SKIN_ZIP_FILE		L"Core.Skin.Zip.File"




//ZIP压缩包资源皮肤
#define OBJECT_SKIN_ZIP_RESOURCE		L"SKIN.ZIPRESOURCE"

#define CTRL_CONTROL					L"Control"

#define CTRL_CONTAINER					L"Container"

#define CTRL_WINDOW						L"Window"
#define CTRL_HOST_WINDOW				L"HostWindow"

#define CTRL_VERTICALLAYOUT				L"VerticalLayout"
#define CTRL_HORIZONTALLAYOUT			L"HorizontalLayout"
#define CTRL_LABEL						L"Label"
#define CTRL_BUTTON						L"Button"
#define CTRL_OPTION						L"Option"
#define CTRL_PROGRESS					L"Progress"
#define CTRL_TEXT						L"Text"
#define CTRL_TABLELAYOUT				L"TableLayout"
#define CTRL_EDIT						L"Edit"
#define CTRL_GROUP						L"Group"
#define CTRL_GIF						L"Gif"
#define CTRL_PICTURE					L"Picture"

#define CTRL_SCROLL						L"Scroll"

#define CTRL_Theme						L"Theme"

#define CTRL_RichEdit					L"RichEdit"

typedef INT IIDef; //接口ID

//定义接口

//////////////////////////////////////////////////////////////////////////
//能从XML中访问到的对象
#define OBJECT_CORE_FONT				L"Font"

//////////////////////////////////////////////////////////////////////////
//消息定义
//消息被定义为直接到达目的地的，带有一定操作的消息
enum
{
	DuiMsg_RefreshPost	= (WM_USER + 1000), //刷新消息，内部使用
	DuiMsg_Create,							//对象创建,这时候子控件尚未创建完成
	DuiMsg_Initialize,						//对象初始化，这时候所有的子空间都已经初始化完成，
											//该消息只能由IDuiContainerCtrl控件响应

	DuiMsg_Destory,							//对象销毁
	//DuiMsg_Close,							//关闭,bHandle = TRUE时允许退出

	DuiMsg_Z_OrderChanged,					//Z 轴顺序发生变化
	DuiMsg_SizeChanged,						//窗口大小发生变化

	DuiMsg_Timer,

	//鼠标消息
	DuiMsg_MouseEnter,						//鼠标进入
	DuiMsg_MouseLeave,						//鼠标离开
	DuiMsg_MouseMove,						//鼠标移动
	DuiMsg_MouseHover,						//鼠标停留
	DuiMsg_LButtonDown,						//鼠标左键单击
	DuiMsg_LButtonDblClk,					//左键双击事件
	DuiMsg_LButtonUp,						//
	DuiMsg_RButtonDown,
	DuiMsg_MouseWheel,
	DuiMsg_SetCursor,

	//内部使用
	DuiMsg_OwnerDuiKit = 9999,

	//Option设置Group之后如果需要通知其他Option
	DuiMsg_OptionSelectChanged = DuiMsg_OwnerDuiKit + 1,

	

};

//事件定义为从可视树中，从事件源依次往跟节点传递的消息
enum
{
	DuiEvent_Initialize,
	DuiEvent_Click,			//鼠标单击
	DuiEvent_DbClick,
	DuiEvent_Timer,			//定时器
	DuiEvent_SelectChanged,	//选中状态改变
	DuiEvent_TextChange,
	DuiEvent_SetFocus,
	DuiEvent_KillFocus,
	DuiEvent_VisableChanged,	//显示状态发生变化
	DuiEvent_Size,				//大小发生变化
};

//////////////////////////////////////////////////////////////////////////
//控件状态
#define UISTATE_NORMAL       0x00000001
#define UISTATE_FOCUSED      0x00000002
#define UISTATE_SELECTED     0x00000004
#define UISTATE_DISABLED     0x00000008
#define UISTATE_HOT          0x00000010   //鼠标在控件上
#define UISTATE_PUSHED       0x00000020   //鼠标按下
#define UISTATE_READONLY     0x00000040
#define UISTATE_CAPTURED     0x00000080   //获得焦点


// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

namespace DuiKit{;


//定义接口

enum 
{
	IIDuiObject,
	IIDuiControlCtrl, 
	IIDuiAnime,
	IIDuiCore,
	IIDuiContainerCtrl,
	IIDuiWindowCtrl,
	IIDuiHostWindowCtrl,
	IIDuiSkin,
	IIDuiBuilder,
	IIDuiByteStream,
	IIDuiSkinSet,
	IIDuiFont,
	IIDuiFontSet,
	IIDuiLang,
	IIDuiLangSet,
	IIDuiImage,
	IIDuiVerticalLayoutCtrl,
	IIDuiHorizontalLayoutCtrl,
	IIDuiButtonCtrl,
	IIDuiLabelCtrl,
	IIDuiTextCtrl,
	IIDuiEditCtrl,
	IIDuiOptionCtrl,
	IIDuiProgressCtrl,
	IIDuiTableLayoutCtrl,
	IIDuiGroupCtrl,
	IIDuiGifCtrl,
	IIDuiPictureCtrl,
	IIDuiVerticalScrollCtrl,
	IIDuiHorizontalScrollCtrl,
	IIDuiScrollCtrl,
	IIDuiRichEditCtrl,
};


//定义一些模板帮助类





};//namespace DuiKit{;
