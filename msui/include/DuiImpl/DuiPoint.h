#pragma once
namespace DuiKit{;

class CDuiPoint : public tagPOINT
{
public:
	CDuiPoint()
	{
		x = y = 0;
	}

	CDuiPoint(const POINT& src)
	{
		x = src.x;
		y = src.y;
	}


	CDuiPoint(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	CDuiPoint(LPARAM lParam)
	{
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
	}
};

};//na