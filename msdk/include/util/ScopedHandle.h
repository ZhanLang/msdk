#pragma once
namespace msdk{;
namespace UTIL{;

class CScopedHandle
{
	CScopedHandle(const CScopedHandle&);
	CScopedHandle& operator=(const CScopedHandle&);

public:

	CScopedHandle(HANDLE handle)
		: _handle(handle)
	{

	}

	~CScopedHandle()
	{
		if (_handle != NULL) {
			CloseHandle(_handle);
		}
	}

	operator HANDLE() const 
	{
		return _handle;
	}

	PHANDLE  operator& () 
	{
		return &_handle;
	}

	void operator=(HANDLE handle)
	{
		if (_handle != NULL) {
			CloseHandle(_handle);
		}
		_handle = handle;
	}

private:
	HANDLE _handle;
};

};};