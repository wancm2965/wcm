#pragma once

class CMyBitMap
{
public:
	CMyBitMap(void);
	~CMyBitMap(void);
	BITMAP m_bmp ; //位图格式

public:
	HBITMAP m_hBitMap ;	//位图句柄

public:
	bool SetBitmap(UINT uResID);
	bool SetBitmap(const char * pBitMapFile);
	bool drawOnDc( HDC hDC, long lx, long ly, long lex, long ley ) ;
	bool drawOnDcN( HDC hDC, long lx, long ly ) ;
	bool drawOnDcNXY( HDC hDC, long lsx, long lsy, long lex, long ley ) ;

};
