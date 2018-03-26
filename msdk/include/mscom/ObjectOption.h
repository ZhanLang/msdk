#pragma once

#include "util/xbuffer.hpp"
#include <syncobject\criticalsection.h>
#include <map>

//一个非常简单的类属性实现方法
/*
使用方法：
	1、添加常量：
		CObjectOption option;
		CUseObjectOption useOption(&option);
		useOption.SetOption(1,10);
		DWORD dwValue = useOption.GetOption(1,DWORD(0));
	2、添加一段缓冲区：
		BYTE buf[MAX_PATH] = {0};
		memset(buf,0,MAX_PATH);
		useOption.SetOptionBuffer(1,buf,MAX_PATH);
		DWORD dwLen = 0;
		LPBYTE lpValue = useOption.GetOptionBuffer(1,LPBYTE(NULL),&dwLen);
	3、添加一段字符串：
		LPCTSTR lpstrValue = _T("CObjectOption");
		useOption.SetOptionBuffer(1,lpstrValue,lstrlen(lpstrValue)+1);
		LPCTSTR lpRetValue = useOption.GetOptionBuffer(1,_T(""));
*/
struct IObjectOption
        : public IMSBase
{
    virtual BOOL GetOption(DWORD option, OUT LPVOID *lpData, OUT DWORD &dwLen) = 0;
    virtual BOOL SetOption(DWORD option, LPVOID lpdata, DWORD dwLen) = 0;
};
MS_DEFINE_IID(IObjectOption, "{31171CC1-CCAB-47ae-BC0E-CAB39EDD0A43}");


class CObjectOptionImp
    : public IObjectOption
{
public:
    ~CObjectOptionImp()
    {
        m_option.clear();
    }

    BOOL GetOption(DWORD option, OUT LPVOID *lpData, OUT DWORD &dwLen)
    {
        RASSERT(lpData , FALSE);
        AUTOLOCK_CS(objectoption);
        COptions::const_iterator it = m_option.find(option);
        RASSERT(it != m_option.end() , FALSE);
        return  it->second.GetRawBuffer((UCHAR **)(lpData), &dwLen) ? TRUE : FALSE;
    }

    BOOL SetOption(DWORD option, LPVOID lpdata, DWORD dwLen)
    {
        RASSERT(lpdata && dwLen , FALSE);
        AUTOLOCK_CS(objectoption);
        XBuffer xBuf(static_cast<unsigned char *>(lpdata), dwLen);
        m_option[option] = xBuf;
        return TRUE;
    }

private:
    typedef std::map<DWORD , XBuffer> COptions;
    DECLARE_AUTOLOCK_CS(objectoption);
    COptions m_option;
};

//偷懒人士专用
class CUseObjectOption
{
public:
    explicit CUseObjectOption(IObjectOption *pOption)
    {
        m_pOption = pOption;
    }

    template<class T>
    BOOL SetOption(DWORD option, T value)
    {
        return m_pOption->SetOption(option, &value, sizeof(value));
    }

    template<class T>
    T GetOption(DWORD option, T default)
    {
        T *lpValue = NULL;
        DWORD dwLen = 0;
        BOOL bRet = m_pOption->GetOption(option, (LPVOID *)(&lpValue), dwLen);
        return lpValue ? *lpValue : default;
    }

    template<class T>
    BOOL SetOptionBuffer(DWORD option, T *value , DWORD dwCount)
    {
        RASSERT(value , FALSE);
        return m_pOption->SetOption(option, (LPVOID)value, dwCount * sizeof(value[0]));
    }

    template<class T>
    T *GetOptionBuffer(DWORD option, T *default, DWORD *dwOutLen = NULL)
    {
        T *lpValue = NULL;
        DWORD dwLen = 0;
        BOOL bRet = m_pOption->GetOption(option, (LPVOID *)(&lpValue), dwLen);
        if (dwOutLen)
        {
            *dwOutLen = dwLen;
        }

        return bRet ? lpValue : default;
    }

private:
    UTIL::com_ptr<IObjectOption> m_pOption;
};