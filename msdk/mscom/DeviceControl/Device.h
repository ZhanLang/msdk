#pragma once

#include "DeviceControl\IDeviceControl.h"
#include "wmihelp.h"





/********************************************************************
	Summary:
		设备信息结构体
*********************************************************************/
struct DeviceNodeInfo
{
	std::wstring	m_strClass;					//设备CLASS
	std::wstring	m_strClassGuid;				//设备CLASSGUID
	std::wstring	m_strDesc;					//设备描述
	std::wstring	m_strHID;						//设备HARDWAREID
	std::wstring	m_strFName;					//设备FriendlyName
	std::wstring	m_strDriver;					//设备Driver
	std::wstring	m_strCompatibleID;			//兼容设备ID
	std::wstring	m_strServiceName;				//服务名
	std::wstring	m_strNumeratorName;			//枚举器名称
	std::wstring	m_strDeviceInstanceId;		//获取设备范例ID
	std::wstring	m_strManufacturer;			//制造商名称
	std::wstring	m_strDriverProvider;			//驱动提供商
	std::wstring	m_strDriverVersion;			//驱动版本
	std::wstring	m_strDriverDay;				//驱动日期
	std::wstring	m_strDriveInfFilePath;		//驱动Inf文件 
	std::wstring	m_strDeviceModel;
}; 

class CDevice
	:public IDevice
	,protected DeviceNodeInfo
{
public:
	CDevice(void){}
	virtual ~CDevice(void){}
	virtual DWORD			GetDeviceID(){return -1;}
	virtual DeviceTypeEnum	GetDeviceType(){return DEVICE_TYPE_UNKNOWN;}
	virtual	VOID			DelayDelete(){delete this;}


	//对设备的操作
	virtual BOOL StartDevice(); //启用设备
	virtual BOOL StopDevice();  //停用设备

	//获取属性
	virtual LPCWSTR GetSerialNumber(){return m_strHID.c_str();}
	virtual	LPCWSTR GetManufacturer(){return this->m_strManufacturer.c_str();}
	virtual LPCWSTR GetDescription(){return m_strDesc.c_str();}
	virtual LPCWSTR GetProperty(DWORD dwProperty){return NULL;}
	virtual LPCWSTR	GetSpecification(){return m_strServiceName.c_str();}
	virtual LPCWSTR GetDeviceModel(){return m_strFName.c_str();}

	//模块内部使用
	void SetDeviceNodeInfo(DeviceNodeInfo nodeInfo){*((DeviceNodeInfo*)this) = nodeInfo;}
	void SetClass(std::wstring str){m_strClass = m_strClass;}
	void SetClassGuid (std::wstring str){m_strClassGuid =  str ;}
	void SetDesc (std::wstring str){this->m_strDesc = str;}
	void SetHID (std::wstring str){this->m_strHID = str;}
	void SetFName (std::wstring str){this->m_strFName = str;}
	void SetDriver (std::wstring str){this->m_strDriver = str;}
	void SetCompatibleID (std::wstring str){this->m_strCompatibleID = str ;}
	void SetServiceName (std::wstring str){this->m_strServiceName  = str;}
	void SetNumeratorName (std::wstring str){this->m_strNumeratorName = str ;}
	void SetDeviceInstanceId (std::wstring str){this->m_strDeviceInstanceId  = str;}
	void SetManufacturer (std::wstring str){this->m_strManufacturer  = str;}
	void SetDriverProvider (std::wstring str){this->m_strDriverProvider  = str;}
	void SetDriverVersion (std::wstring str){this->m_strDriverVersion  = str;}
	void SetDriverDay (std::wstring str){this->m_strDriverDay  = str;}
	void SetDriveInfFilePath (std::wstring str){this->m_strDriveInfFilePath  = str;}
	
	void SetDeviceModel(std::wstring str){m_strDeviceModel = str;}
	//模块内部使用
public:
	std::wstring GetNumeratorName(){return m_strNumeratorName;}
	std::wstring GetCompatibleID(){return m_strCompatibleID;}
private:
	//各种必需的成员变量
};

struct DeviceScanNotify
{
	virtual VOID OnDeviceScanNotify(CDevice* pDevice) = 0;
};