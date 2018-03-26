#pragma once
namespace DuiKit{;

class CDuiRect : public tagRECT
{
public:
	CDuiRect()
	{
		left = top = right = bottom = 0;
	}

	CDuiRect(const RECT& src)
	{
		left = src.left;
		top = src.top;
		right = src.right;
		bottom = src.bottom;
	}

	CDuiRect(int iLeft, int iTop, int iRight, int iBottom)
	{
		left = iLeft;
		top = iTop;
		right = iRight;
		bottom = iBottom;
	}

	int GetWidth() const
	{
		return right - left;
	}

	int GetHeight() const
	{
		return bottom - top;
	}

	void Empty()
	{
		left = top = right = bottom = 0;
	}

	bool IsNull() const
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0); 
	}

	void Join(const RECT& rc)
	{
		if( rc.left < left ) left = rc.left;
		if( rc.top < top ) top = rc.top;
		if( rc.right > right ) right = rc.right;
		if( rc.bottom > bottom ) bottom = rc.bottom;
	}

	void ResetOffset()
	{
		::OffsetRect(this, -left, -top);
	}

	void Normalize()
	{
		if( left > right ) { int iTemp = left; left = right; right = iTemp; }
		if( top > bottom ) { int iTemp = top; top = bottom; bottom = iTemp; }
	}

	void Offset(int cx, int cy)
	{
		::OffsetRect(this, cx, cy);
	}

	void Inflate(int cx, int cy)
	{
		::InflateRect(this, cx, cy);
	}

	void Deflate(int cx, int cy)
	{
		::InflateRect(this, -cx, -cy);
	}

	void Union(CDuiRect& rc)
	{
		::UnionRect(this, this, &rc);
	}

	VOID SetRect( int xLeft,  int yTop,  int xRight,  int yBottom)
	{
		::SetRect(this, xLeft, yTop, xRight, yBottom);
	}

	bool operator == (const CDuiRect& rc)
	{
		if (top == rc.top && left == rc.left && right == rc.right && bottom == rc.bottom)
		{
			return true;
		}
		return false;
	}
};

};//namespace DuiKit{;