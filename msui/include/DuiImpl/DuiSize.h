#pragma once
namespace DuiKit{;

class CDuiSize : public tagSIZE
{
public:
	CDuiSize()
	{
		cx = cy = 0;
	}

	CDuiSize(const SIZE& src)
	{
		cx = src.cx;
		cy = src.cy;
	}

	CDuiSize(const RECT rc)
	{
		cx = rc.right - rc.left;
		cy = rc.bottom - rc.top;
	}

	CDuiSize(int _cx, int _cy)
	{
		cx = _cx;
		cy = _cy;
	}
};

};//namespace DuiKit{;