#pragma once

#include "XmlValueW.h"
#include <Windows.h>
#include <Guiddef.h>


// 关于时间转换常量定义
#define TICKSPERMSEC       10000
#define TICKSPERSEC        10000000
#define SECSPERDAY         86400
#define SECSPERDAY         86400
#define SECSPERHOUR        3600
#define SECSPERMIN         60
#define EPOCHWEEKDAY       1
#define DAYSPERWEEK        7
#define EPOCHYEAR          1601
#define DAYSPERNORMALYEAR  365
#define DAYSPERLEAPYEAR    366
#define MONSPERYEAR        12


typedef struct _CALL_HEADER_
{
	std::wstring   strfrom;
	std::wstring   strto;

}call_header, *pcall_header;

enum reponse_type
{
	NORMAL_REPONSE = 1,
	FAULT_REPONSE = 2,
	UNKNOWN_REPONSE = 3,
};




class CXmlCall
{
public:

	// 生成传输信息
	static std::wstring GenerateBody( const std::wstring& methodName, CXmlValueW const& params );

 	static std::wstring GenerateRequest(
		const call_header & header,
		const std::wstring& methodName, 
		CXmlValueW const& params
		);

	// 获取调用头
	static bool GetCallHeaderFromRequest(  const std::wstring & strRequest, call_header & header );
	
	// 获取调用函数名，调用参数
	static bool GetParamsFromRequest( const std::wstring & strRequest, std::wstring & strFunName, CXmlValueW & params );

	// 生成正常答复
	static std::wstring GenerateNormalResponse(const call_header & header, 
		const std::wstring &methodName,
		const std::wstring &plugName,
		const CXmlValueW & params, 
		const CXmlValueW & result);

	// 检测是否是第一次生成回执结果
	static bool IsFirstReponse( const std::wstring & str );

	// 获取回执类型
	static reponse_type  GetReponseType( std::wstring & orgStr, const std::wstring & plugName );


	// 追加结果
	static std::wstring AppendNormalResponse( std::wstring & orgStr, const std::wstring & plugName, CXmlValueW& result );

	// 追加错误结果
	static std::wstring AppendFaultReponse( std::wstring & orgStr, const std::wstring & plugName, CXmlValueW& result );

	
	// 生成错误答复
	static std::wstring GenerateFaultResponse( const call_header & header,  const std::wstring & strErrorDescription, const int iErrorCode ); 

	// 获取正常执行结果
	static bool GetNormalResponse(CXmlValueW& result,  std::wstring strResponse, const std::wstring & plugName );

	// 获取错误执行结果
	static bool GetFaultResponse( CXmlValueW & fault, std::wstring strResponse, const std::wstring & plugName );


	// 生成GUID
	static bool CreateGuid( GUID * pGuid );
	static bool ConvertGuidToString(const GUID* pGuid , std::wstring & strUuid );
	static int  XmlSwprintf(  wchar_t *string, size_t size, const WCHAR *format, ...);
	static bool CreateGuidString( std::wstring & strUuid );

	// 生成时间
	static int DaysSinceEpoch(int Year);
	static bool ConvertTimeToString( LONGLONG IntTime , std::wstring & strTime );
	static int IsLeapYear(int Year);
	static bool XmlGetCurrentTime( std::wstring & strTime );
};

