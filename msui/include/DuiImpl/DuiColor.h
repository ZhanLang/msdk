#pragma once


namespace DuiKit{;

class CDuiARGB
{
public:
	CDuiARGB():m_byteR(0), m_byteG(0), m_byteB(0), m_byteA(0){}
	CDuiARGB(BYTE r, BYTE g, BYTE b, BYTE a = 0):m_byteR(r), m_byteG(g), m_byteB(b), m_byteA(a){}

	VOID SetColor(COLORREF clr)		//RGB(255, 0, 0) red =>0000FF
	{
		m_byteB = GetBValue(clr);
		m_byteG = GetGValue(clr);
		m_byteR = GetRValue(clr);	
		m_byteA = 255;
	}
	

	VOID SetColor(LPCTSTR lpszColor)	//ARGB(FF-FF-FF-FF) "FF0000" red =>FF0000
	{
		LPTSTR lpszEnd = NULL;
		DWORD clrTemp = _tcstoul(lpszColor, &lpszEnd, 16);

		m_byteB = GetRValue(clrTemp);
		m_byteG = GetGValue(clrTemp);
		m_byteR = GetBValue(clrTemp);
		m_byteA = (clrTemp&0xFF000000)>>24;
	}

	COLORREF GetColor() const
	{
		return RGB(m_byteR, m_byteG, m_byteB);
	}

	BOOL IsEmpty() const
	{
		return (m_byteA == 0 && m_byteB == 0 && m_byteG == 0 && m_byteR == 0);
	}

	BYTE GetA() const { return m_byteA; }
	BYTE GetG() const { return m_byteG; }
	BYTE GetR() const { return m_byteR; }
	BYTE GetB() const { return m_byteB; }

	BYTE& GetA()  { return m_byteA; }
	BYTE& GetG()  { return m_byteG; }
	BYTE& GetR()  { return m_byteR; }
	BYTE& GetB()  { return m_byteB; }

	VOID SetA(BYTE b)  { m_byteA = b; }
	VOID SetG(BYTE b)  { m_byteG = b; }
	VOID SetR(BYTE b)  { m_byteR = b; }
	VOID SetB(BYTE b)  { m_byteB = b; }

	VOID WriteToDIB4(LPBYTE lp) const
	{
		ASSERT(lp != NULL);
		memcpy(lp, this, 4);
	}

	VOID WriteToDIB3(LPBYTE lp) const
	{
		ASSERT(lp != NULL);
		memcpy(lp, this, 3);
	}

	LPBYTE ReadFromDIB4(LPBYTE lp)
	{
		ASSERT(lp != NULL);
		memcpy(this, lp, 4);
		return lp+4;
	}

	LPBYTE ReadFromDIB3(LPBYTE lp)
	{
		ASSERT(lp != NULL);
		memcpy(this, lp, 3);
		return lp+3;
	}

private:
	BYTE m_byteB;
	BYTE m_byteG;
	BYTE m_byteR;
	BYTE m_byteA;
};

class  CDuiHLSColor
{
public:
	unsigned char red;
	unsigned char green;
	unsigned char blue;

	double lightness, saturation, hue;

	CDuiHLSColor()
	{
	}

	CDuiHLSColor(CDuiARGB& clr)
	{
		red = clr.GetR();
		green = clr.GetG();
		blue = clr.GetB();
	}

	CDuiHLSColor(double h, double l, double s)
	{
		hue		   = h;
		lightness  = l;
		saturation = s;

		CorrectHLS();
	}

	CDuiHLSColor (BYTE r, BYTE g, BYTE b)
	{
		red   = r;
		green = g;
		blue  = b;
	}

	CDuiHLSColor(COLORREF color)
	{
		red   = GetRValue(color);
		green = GetGValue(color);
		blue  = GetBValue(color);
	}

	void ToHLS(void)
	{
		double mn, mx; 
		int	   major;

		if ( red < green )
		{
			mn = red; mx = green; major = Green;
		}
		else
		{
			mn = green; mx = red; major = Red;
		}

		if ( blue < mn )
			mn = blue;
		else if ( blue > mx )
		{
			mx = blue; major = Blue;
		}

		if ( mn==mx ) 
		{
			lightness    = mn/255;
			saturation   = 0;
			hue          = 0; 
		}   
		else 
		{ 
			lightness = (mn+mx) / 510;

			if ( lightness <= 0.5 )
				saturation = (mx-mn) / (mn+mx); 
			else
				saturation = (mx-mn) / (510-mn-mx);

			switch ( major )
			{
			case Red  : hue = (green-blue) * 60 / (mx-mn) + 360; break;
			case Green: hue = (blue-red) * 60  / (mx-mn) + 120;  break;
			case Blue : hue = (red-green) * 60 / (mx-mn) + 240;
			}

			if (hue >= 360)
				hue = hue - 360;
		}
	}

	void ToRGB(void)
	{
		CorrectHLS();

		if (saturation == 0)
		{
			red = green = blue = (unsigned char) (lightness*255);
		}
		else
		{
			double m1, m2;

			if ( lightness <= 0.5 )
				m2 = lightness + lightness * saturation;  
			else                     
				m2 = lightness + saturation - lightness * saturation;

			m1 = 2 * lightness - m2;   

			red   = Value(m1, m2, hue + 120);   
			green = Value(m1, m2, hue);
			blue  = Value(m1, m2, hue - 120);
		}
	}

	void CorrectHLS()
	{
		if(lightness < 0) lightness = 0;
		if(lightness > 1) lightness = 1;

		if(saturation <0 ) saturation = 0;
		if(saturation > 1) saturation = 1;

		if(hue <0 ) hue = 0;
		if(hue >= 359 ) hue = 358.9999;
	}

	operator COLORREF() const
	{
		return RGB(red, green, blue);
	}

protected:
	typedef enum
	{
		Red,
		Green,
		Blue
	};

private:
	unsigned char Value(double m1, double m2, double h)
	{
		while (h >= 360) h -= 360;
		while (h <    0) h += 360;

		if (h < 60) 
			m1 = m1 + (m2 - m1) * h / 60;   
		else if (h < 180) 
			m1 = m2;
		else if (h < 240) 
			m1 = m1 + (m2 - m1) * (240 - h) / 60;      

		return (unsigned char)(m1 * 255);
	}
};

};