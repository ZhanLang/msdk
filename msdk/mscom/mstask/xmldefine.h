#ifndef _XMLDEFINE_H_
#define _XMLDEFINE_H_
#include "mstask\imstask.h"
#include <list>
using std::list;

#define ROOT_PRODUCT   _T("RsTasks")
#define ROOT_TASK      _T("Task")
 
#define ATTRIB_PATH     _T("path")
#define ATTRIB_VERSION  _T("version")

#define ATTRIB_TYPE     _T("type")
#define ATTRIB_USE      _T("use")
#define ATTRIB_POWER    _T("power")
#define ATTRIB_CMD      _T("cmd")
#define ATTRIB_END      _T("end")
#define ATTRIB_INDEX    _T("index")
#define ATTRIB_ISRISING  _T("isrising")
#define ATTRIB_TASKNAME  _T("name")
#define ATTRIB_TASKDESC  _T("desc")
#define ATTRIB_LOCK      _T("lock")
#define ATTRIB_CLASS     _T("class")
#define ATTRIB_DELETE    _T("delete")


#define ATTRIB_ACTION   _T("action")
#define ATTRIB_RIGHT    _T("right")
#define ATTRIB_SECOND   _T("second")
#define ATTRIB_METHOD   _T("method")

#define ATTRIB_DATE     _T("date")
#define ATTRIB_NUMBER   _T("number")
#define ATTRIB_HOUR     _T("hour")
#define ATTRIB_MINUTE   _T("minute")
#define ATTRIB_WEEKMARK  _T("weekmark")
#define ATTRIB_STOP      _T("stop")
#define ATTRIB_DELETE    _T("delete")
#define ATTRIB_LASTTIME      _T("lasttime")
#define ATTRIB_STARTDATE     _T("startdate")
#define ATTRIB_STOPDATE      _T("stopdate")
#define ATTRIB_MONTHMARK     _T("monthmark")
#define ATTRIB_DAY           _T("day")
#define ATTRIB_DAYMARK       _T("daymark")
#define ATTRIB_WEEKNUM      _T( "weeknumber")
#define ATTRIB_DAYOFWEEK     _T("dayofweek")

#define ATTRIB_INTERVALTYPE   _T("intervaltype")
#define ATTRIB_INTERVALTIME   _T("intervaltime")
#define ATTRIB_STOPTYPE       _T("stoptype")
#define ATTRIB_STOPRUNNING    _T("stoprunning")
#define ATTRIB_HOURS		  _T("hours")
#define ATTRIB_MINUTES        _T("minutes")
#define ATTRIB_SECONDS        _T("seconds")


#define NODE_COMMANDLINE    _T("CommandLine")
#define NODE_SPECIFIC       _T("Specific")
#define NODE_TIME           _T("Time")
#define NODE_EVERYWEEK      _T("EveryWeek")
#define NODE_EVERYMONTH     _T("EveryMonth")     
#define NODE_EVERYDAY       _T("EveryDay")
#define NODE_EVERYHOUR      _T("EveryHour")
#define NODE_REPEAT         _T("Repeat")
#define NODE_FINISH         _T("Finish")
#define NODE_INTERVALHOURS  _T("IntervalHours")
#define NODE_RUNONESHOT     _T("RunOneShot")
#define NODE_AFTERBOOT      _T("AfterBoot")

template<class T>
class pointer_list : public std::list<T>
{
public:
	void remove_pitems()
	{
		for( pointer_list<T>::iterator i=begin(); i!=end(); i++ )
		{
			delete *i;
			*i = 0;
		}
	}
	~pointer_list()
	{
		remove_pitems();
	}

	pointer_list()
	{
	}

	pointer_list(const pointer_list& _X)
	{
		DebugBreak();// 指针列表不能复制，因为此列表会自动释放指针指向内存
	}

	pointer_list& operator=(const pointer_list& _X)
	{
		DebugBreak();// 指针列表不能复制，因为此列表会自动释放指针指向内存
		return (*this); 
	}
};
//#pragma warning( default : 4183 )

typedef pointer_list<LPST_RSTASK> TASK_LIST;
typedef TASK_LIST::iterator TASK_LIST_ITER;

#define RSTASK_VERSION _T("2.0")
typedef struct tagPRODUCT_ATTRIBUTE{
	//现在只有版本信息,留给升级使用
	TCHAR szVersion[20];
	//所属产品
	//TCHAR szProduct[32];
}PRODUCT_ATTRIBUTE,*PPRODUCT_ATTRIBUTE;

typedef struct tagTASK_XML{
	PRODUCT_ATTRIBUTE product_attribute;
	TASK_LIST         task_list;
}TASK_XML,*PTASK_XML;

#endif