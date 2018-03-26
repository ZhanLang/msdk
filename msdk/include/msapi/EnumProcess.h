#include <Tlhelp32.h>

namespace msdk{;
namespace msapi{;

class CEnumProcess
{
public:
	//·µ»ØFALSEÌø³öÃ¶¾Ù
	typedef BOOL(*ProcEnumFunction)(const PROCESSENTRY32W* pe, LPVOID lParam);

	CEnumProcess(ProcEnumFunction pEnum, LPVOID lParam)
	{
		m_lParam = lParam;
		m_pEnumFunc = pEnum;
	}

	~CEnumProcess() {};
	BOOL EnumProcess(std::wstring ProcessName[], DWORD dwCount)
	{
		PROCESSENTRY32 pe32 = {sizeof(pe32)} ;
		HANDLE hProcessShot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 ) ;
		if ( hProcessShot == INVALID_HANDLE_VALUE )
			return FALSE;

		if ( Process32First ( hProcessShot, &pe32 ) )
		{
			do
			{
				for ( DWORD dwLoop = 0 ; dwLoop < dwCount ; dwLoop++ )
				{
					if ( _tcsicmp(pe32.szExeFile, ProcessName[dwLoop].c_str()) == 0 )
					{
						if ( !m_pEnumFunc( &pe32, m_lParam) )
						{
							goto exit;
						}
					}
				}
				
			}while ( Process32Next ( hProcessShot, &pe32 ) ) ;  
		}

	exit:
		CloseHandle ( hProcessShot) ;  
		return dwCount;
	}
private:
	LPVOID m_lParam;
	ProcEnumFunction m_pEnumFunc;
};
};};