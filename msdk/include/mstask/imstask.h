#pragma once
#include <Unknwn.h>

#include <pshpack1.h>
//---------------------------------------------------
//在一段时间里,可重复启动任务的条件
//这主要在满足每周,每月,每天里的条件里,在加上可启动的扩展
//先定义在此,暂不实现
typedef struct tagREPEAT{
	//-----------------------------------
	WORD wIntervalType;    //间隔类型

	WORD wIntervalTime;    //在间隔类型wIntervalType判断 
						   //1:每间隔多少小时
						   //2:每间隔多少分钟 2选一
	//---------------------------------
	WORD wStopType;        //停止类型,
	WORD wHour;            //1:直停止时间, 直到几点几分停止
	WORD wMinute;          //1:
						   //2:持续的时间, 持续几小时几分后停止
						   //2:
	//----------------------------------
	WORD wStopRunning;     //到停止时间,任务还正在运行,就停止
}REPEAT,*LPREPEAT;

//----------------------------------------------------
//把任务的不同部分分开定义,相同的放到RSTASK结构里
//有每天,每周,每月
//每小时
typedef struct tagEVERYHOUR{
	UINT uInterval_Hour;   //每隔几小时
	WORD wMinute;          //在几分启动 
}EVERYHOUR,*LPEVERYHOUR;

//每几天
typedef struct tagEVERYDAY{
	UINT uInterval_Day;     //每隔几天
	WORD wHour;             //在几点几分启动  
	WORD wMinute;
	//REPEAT RepeatTime;
}EVERYDAY,*LPEVERYDAY;

//周
typedef struct tagEVERYWEEK{
	UINT uInterval_Week;    //每隔几周
	WORD wHour;             //在几点几分启动  
	WORD wMinute;
	//
	WORD wWeekMark;       //以位来标计多选的星期几.  0111 1111 从右到左是星期日,星期一 ...星期六 
	//REPEAT RepeatTime;
}EVERYWEEK,*LPEVERYWEEK;

//月
typedef struct tagEVERYMONTH{
	WORD wHour;             //在几点几分启动  
	WORD wMinute;
	//---------------------------------------------
	// 几号和 星期几 2选1
	WORD wDay;              //1:每月的第几号

	WORD wWeekNum;          //2:每月的第几个周1-5 ,5表示最后一周
	WORD wDayOfWeek;        //2:星期几

	DWORD wDayMark;         //3:每月的几号 多选, 先不实现
	//----------------------------------------------
	WORD wMonthMark;       //在几月,按位计算多选的月份.如果是0x0FFF,就是每月,0x1是一月,0x7就是1-3月	 
	                       //0x1 :1, 0x2 :2,0x4 :3, 0x8 :4

	//REPEAT RepeatTime;
}EVERYMONTH,*LPEVERYMONTH;

//-------------------------------------------------------
//任务开始停止日期
typedef struct tagDATE{
	WORD wYear;
	WORD wMonth;
	WORD wDay;
}ST_DATE;

//定点一次的运行时间,年月日时分
typedef struct tagRUNONESHOT{
	ST_DATE date;
	WORD wHour;             //在几点几分启动  
	WORD wMinute;
}RUNONESHOT,*LPRUNONESHOT;

//起动多久开始启动
typedef struct tagAfetrBoot{
	WORD wHours;             //在几点几分启动  
	WORD wMinutes;
	WORD wSeconds;
}AFTERBOOT,*LPAFTERBOOT;

#define NAME_LEN 128
typedef struct tagTASKATTRIB{
	UINT  uType;		      //启动类型  对应enum RS_RAVTASK_START_TYPE
	UINT  uIndex;		      //启动程序的index  对应enum RS_RAVTASK_START_INDEX
	WORD  wUse;               //是否启用这个任务1,0
	WORD  wPower;             //启用笔记电池时 是否启动任务

	WORD  wDelete;            //是否启用这个任务1,0   //内部删除用，对出厂任务不真正删除
	WORD  wLock;			  //是否锁定这个任务1,0  //网络版保留
	UINT  uClassID;           //任务细分大类- 对应RS_RAVTASK_CLASS
	UINT  uIsRising;          //=1是瑞星带的xml,
	TCHAR szTaskName[NAME_LEN];    //是瑞星的,这就是ID.=0,用户的直接是串
	TCHAR szTaskDesc[MAX_PATH];    //
}TASKATTRIB,*LPTASKATTRIB;

typedef struct tagTASKSPECIFIC{
	//特殊部分,可选用
	WORD wFindAction;      //发现任务已经启动 0不启动,1关闭这个任务,再重新启动
	WORD wRight;           //启动权限,现在没需求,没实现
	WORD wFloatSecond;     //浮动参数,任务可在超过多少秒的范围内,仍认为有效
	WORD wSpecificMethod;  //特殊操作方法标记,扩展一些特殊的任务见 RS_RAVTASK_START_SPECIFIC.
}TASKSPECIFIC,*LPTASKSPECIFIC;

typedef struct tagTASKCMD{
	TCHAR cmd[MAX_PATH]; //启动的参数 可处理%SYSTEM% %INSTALL% %WINDIR% 三个宏 
	TCHAR end[MAX_PATH]; //启动的参数 可处理%SYSTEM% %INSTALL% %WINDIR% 三个宏 
}TASKCMD,*LPTASKCMD;


typedef struct tagTASKTIME{
	SYSTEMTIME   last_time;   //保存上次启动的时间,留给查询用
	SYSTEMTIME   next_time;   //保存下次启动的时间,留给查询用
	//时间相关
	ST_DATE start_time;  //任务开始的时间,年月日指从这个时间开开始,不能为空
	ST_DATE stop_time;   //任务结束的时间 ,如果用空,表示不会停止任务
	//----这个union 要UINT  uType来决定要那个结构
	union{
		EVERYHOUR   every_hour;    //每小时,START_TASK_TYPE_HOUR
		EVERYDAY    every_day;     //每天,  START_TASK_TYPE_DAY   
		EVERYWEEK   every_week;    //每周,  START_TASK_TYPE_WEEK
		EVERYMONTH  every_month;   //每月,  START_TASK_TYPE_MONTH
		//-----------------------------------------------------
		RUNONESHOT  oneshot_time;  //A:只运行一次的任务, 时间年月日小时分钟
		                         
								   //B:开机加载, Task运行后启动一次,马上启动的任务,它不管时间,在nType中定义
		AFTERBOOT	after_boot;	   //C:随系统启动,但是等系统启动一段时间以后启动
		//------------------------------------------------------
		//注意和every_hour的区别: every_hour有固定的分钟才启动; interval_hour从Task运行起计算时间
		//调用的人员注意自己的情况
		//间隔小时操作.(太长的间隔时间没用,可以上面的代替)
		UINT      interval_hour;   //从Task运行起间隔多少小时启动
	}every; 
}TASKTIME,*LPTASKTIME;

typedef struct tagTASKFINISH{
	//完成启动任务后的操作
	WORD  wFinishDel;   //不再计划启动的任务是否删除
	WORD  wHours;       //完成启动后的任务后 超过了多少小时多少分钟自动停止
	WORD  wMintues;     //同上一起
}TASKFINISH,*LPTASKFINISH;

//----------------------------------------------------------------------
typedef struct tagRSTASK
{
	//XML配置部分
	TASKATTRIB   task_attrib;
	//---------------------------------------------------------
	TASKCMD      task_cmd;
	//---------------------------------------------------------
	//可选部分
	TASKSPECIFIC task_specific;
	//---------------------------------------------------------
	TASKTIME     task_time;
	//---------------------------------------------------------
	//可选部分
	TASKFINISH   task_finish;
	//XML配置部分结束
	//---------------------------------------------------------
	//---------------------------------------------------------
	DWORD        dwProgressID;  //存放进程ID,以便停止使用
}ST_RSTASK,*LPST_RSTASK; 
#include "PopPack.h"

//RavTask wType - 程序启动的方式 对应RSTASK的nType
enum RS_RAVTASK_START_TYPE
{
	START_TASK_TYPE_DISABLE=0,	//设置为无效
	START_TASK_TYPE_WITHBOOT,	//开机加载, Task运行后启动一次,马上启动的任务
	START_TASK_TYPE_AFTERBOOT,	//随系统启动,但是等系统启动一段时间以后启动

	START_TASK_TYPE_RUNONESHOT, //定点时间启动一次 读-年月日小时分钟
	START_TASK_TYPE_MONTH,		//每月		读-wDay, wHour, wMinute
	START_TASK_TYPE_WEEK,		//每周		读-wDayOfWeek, wHour, wMinute
	START_TASK_TYPE_DAY,		//每天		读-wHour, wMinute
	START_TASK_TYPE_HOUR,		//每小时	读-wMinute

	START_TASK_TYPE_INTERVAL_HOUR,	      //每间隔几小时启动
	START_TASK_TYPE_MESSEAGE,            //响应我们的消息的任务,没时间结构,屏保扫描
	START_TASK_TYPE_AFTERUPDATE,          //升级后启动杀毒
	START_TASK_TYPE_INSTANTUPDATE,        //即时升级

};

//RavTask 启动对象的索引,对应RSTASK中nInex
//修改,控制,查询任务时需要
//每个任务都要不同，出厂配置 对应定义在rstask.xml的index="xx"
//用户手动添加的任务都从 1001 开始定义
//这里只是说明一下
enum RS_RAVTASK_START_INDEX
{
	START_TASK_INDEX_RAV_TIMER=1,  
	START_TASK_INDEX_RAV_BOOT,  
	START_TASK_INDEX_RAVMON,  

	START_TASK_INDEX_CUSTOM=1000,    //可从此值加+1,完成没有定义的索引,以后有任务查看器使用
};

//---------------------------------
//有特殊操作的任务,要添加相关代码支持,现在基本没有
enum RS_RAVTASK_START_SPECIFIC
{
	START_TASK_SPECIFIC_DISABLE = 0,	//没有特殊处理的
	START_TASK_SPECIFIC_SMARTUP,		//定时升级

	//START_TASK_SPECIFIC_RAVMON ,		//开机监控	RavMon.exe
	//START_TASK_SPECIFIC_RAV_BOOT,		//开机扫描	Rav.exe -boot
	//START_TASK_SPECIFIC_RAV_TIMER,	//定时杀毒
	//START_TASK_SPECIFIC_NEWVER,		//检查新的版本
	//START_TASK_SPECIFIC_RAVHDBAK,		//硬盘备份
	//START_TASK_SPECIFIC_CLEAN_LOG,	//定时清理日志
};

//任务大类,给查询用
enum RS_RAVTASK_CLASS
{
	RS_RAVTASK_CLASS_ALL = 0,
	RS_RAVTASK_CLASS_SMARTUP,
	RS_RAVTASK_CLASS_CLEAN_LOG,
	RS_RAVTASK_CLASS_BACUP_BOOT,
	RS_RAVTASK_CLASS_IDLE_SCAN,   //空闲都再用它
	RS_RAVTASK_CLASS_TMIER_TASK,  
	RS_RAVTASK_CLASS_SCREEN,      // 屏保任务
};
//#define ERROR_SUCCESS         0
#define ERROR_EXIST_TASK      (-1)
#define ERROR_NOEXIST_TASK    (-2)
#define ERROR_SAVE_FILE       (-3)
#define ERROR_LOAD_FILE       (-4)
#define ERROR_NOEXIST_FILE    (-5)


interface ITASK :public IUnknown
{
	//------------------------------------------------------------
	//任务配置相关
	//------------------------------------------------------------
	//功能: 用于设置产品配置XML文件,其他的程序不使用,
	//      默认的xml为同rstask.dll目录下的rstask.xml 
	//参数: lpFileName 文件名 
	//影响: 在同目录下生成rstask.ini记录rstask.xml的全路径
	STDMETHOD(SetConfigFile)(IN const TCHAR * lpFileName) = 0;
	//------------------------------------------------------------
	//功能: 设置任务以后都不启用或者启用,
	//参数: dwset 1启用,0,不启用
	//影响: 配置文件,修改USE标识
	STDMETHOD(SetTask)(IN DWORD dwTaskIndex,IN DWORD dwSet) = 0;
	//------------------------------------------------------------
	//功能: 设置所有的任务以后都不启用或者启用,
	//参数: dwset 1启用,0,不启用
	//影响: 配置文件,修改USE标识
	STDMETHOD(SetAllTasks)(IN DWORD dwSet) = 0;
	//-------------------------------------------------------------
	//功能: 添加一个任务配置 
	//参数: lpTask =RSTASK的任务结构
	//返回: 成功和错误码
	//影响: 配置文件
	STDMETHOD(AddTask)(IN ST_RSTASK * lpTask) = 0;
	//-------------------------------------------------------------
	//功能: 删除一个任务配置 
	//参数: dwTaskIndex 任务索引
	//影响: 配置文件
	STDMETHOD(DeleteTask)(IN DWORD dwTaskIndex) = 0;
	//-------------------------------------------------------------
	//功能: 修改一个任务配置 
	//参数: lpTask 任务结构
	//影响: XML配置文件
	STDMETHOD(ModifyTask)(IN ST_RSTASK * lpTask) = 0;
	//-------------------------------------------------------------
	//功能: 取一个任务配置 
	//参数: dwTaskIndex 任务索引,lpTask的任务结构
	//影响: 无
	//返回值 :0成功,1没有此任务,其他是错误码
	STDMETHOD(GetTask)(IN DWORD dwTaskIndex,OUT ST_RSTASK& lpTask) = 0;
	//--------------------------------------------------------------------
	//dwTaskClassID = 0 查所有的类别  TaskList任务列表
	//list 应该改掉，不用用list做参数
	//第一调用得dwCount QueryTask(1,NULL,dwCount);
	//第二次 调用者分配 ST_RSTASK*pTaskArray =  new ST_RSTASK[dwCount];QueryTask(1,pTaskArray,dwCount);
	STDMETHOD(QueryTask)(IN DWORD dwTaskClassID,IN OUT ST_RSTASK* pTaskArray,IN OUT DWORD dwCount) = 0;

	//--------------------------------------------------------------
	//当前任务控制相关 - 以后给任务查看器使用,其他进程可使用
	//--------------------------------------------------------------
	//功能: 控制当前的任务 
	//参数: dwControlID = 1 启动dwControlID = 0 停止
	STDMETHOD(ControlTask)(IN DWORD dwTaskIndex,IN DWORD dwControlID) = 0;
	//--------------------------------------------------------------
	//功能: 控制当前所有任务 
	//参数: 启动dwControlID = 0 停止
	STDMETHOD(ControlTasks)(IN DWORD dwControlID) = 0;
	//------------------------------------------------------------
	//功能: 取任务状态
	//参数: dwTaskIndex,任务索引号, 
	//pPrevStartTime上次运行时间, pNextStartTime下次运行时间
	//函数返回值: 是否成功,和错误码
	STDMETHOD(QueryTaskStatus)(IN DWORD dwTaskIndex,OUT SYSTEMTIME*pPrevStartTime,OUT SYSTEMTIME*pNextStartTime) = 0;

	//没有读配置文件,直接通过ST_RSTASK& Task计算下次启动时间
	STDMETHOD(QueryTaskTime)(IN OUT ST_RSTASK& Task) = 0;

};
struct __declspec(uuid("{67941D87-76CE-4c05-BED2-8E33F79CF747}")) ITASK;

interface IRSTask :public ITASK
{
	//-----------------------------------------------------------
	//升级相关: 以后给升级用,现在先放在这个里说明一下,现在没实现直接返回0
	//-----------------------------------------------------------
	//功能: 升级接口,提供给升级程序发现 检查配置文件版本,升级成新的配置格式 
	//参数: 无
	//返回: 成功0 失败-1 错误码
	//影响: 生成或者修改配置文件
	STDMETHOD(UpdateXMLCfg)() =0;
};
struct __declspec(uuid("{B74B6598-ABE8-4a5b-BD03-EDCBAE364CBC}")) IRSTask;
// {53180022-3A69-4f17-BADD-205DA3F94E62}
MS_DEFINE_GUID(CLSID_RSTask, 
			   0x53180022, 0x3a69, 0x4f17, 0xba, 0xdd, 0x20, 0x5d, 0xa3, 0xf9, 0x4e, 0x62 );

interface IRSTaskSrv :public ITASK
{
	//给服务程序用;
	//功能: 完成读取配置,启动timer,完成task的运行
	STDMETHOD(Run)() = 0;
	STDMETHOD(Stop)() = 0;
	STDMETHOD(CfgModify)() = 0;
	STDMETHOD(OnMessage)(IN DWORD msgid,IN LPVOID buffin,IN size_t cc)=0;
};
struct __declspec(uuid("{CAC019D7-235D-4e90-B63B-D09E59DE21E3}")) IRSTaskSrv;
// {E1828137-480E-492e-A577-37F9968CE730}
MS_DEFINE_GUID(CLSID_RSTaskSrv, 
			   0xe1828137, 0x480e, 0x492e,  0xa5, 0x77, 0x37, 0xf9, 0x96, 0x8c, 0xe7, 0x30  );

/////////////////////////////////////////
//封装IRSTaskSrv 给RSCOM start接口使用
interface ITaskSvrWrap :public IUnknown
{

};
struct __declspec(uuid("{7BC66DD7-2C21-4597-8084-302F64974C6F}")) ITaskSvrWrap;
MS_DEFINE_GUID(CLSID_RSTaskSvrWrap, 
			   // {3CEE7027-0966-48fa-9BBC-B3DAEC3785F3}
				0x3cee7027, 0x966, 0x48fa,  0x9b, 0xbc, 0xb3, 0xda, 0xec, 0x37, 0x85, 0xf3  );