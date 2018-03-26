#pragma once

#include <vector>
namespace msdk{;

class Cifstream
{
	Cifstream(LPCTSTR lpszFileName)
	{
		m_hFile = CreateFile(lpszFileName);
	}


private:
	std::vector<byte> m_buf;
	HANDLE m_hFile;
};
};