#pragma once

class CMyBitMap
{
public:
	CMyBitMap(void);
	~CMyBitMap(void);
	BITMAP m_bmp ; //位图格式

private:
	HBITMAP m_hBitMap ;	//位图句柄

public:
	void SetBitmap(UINT uResID);
	void SetBitmap(char* sFile, int& nBmpW, int& nBmpH);
	bool drawOnDc( HDC hDC, long lx, long ly, long lex, long ley ) ;
	bool drawOnDcD( HDC hDC, long x, long y,long lx, long ly,long bmpX ,long bmpY );
	bool drawOnDcN( HDC hDC, long lx, long ly ) ;
	bool drawOnDcNXY( HDC hDC, long lsx, long lsy, long lex, long ley ) ;

};
