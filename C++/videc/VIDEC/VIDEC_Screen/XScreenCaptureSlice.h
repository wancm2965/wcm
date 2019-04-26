// XScreenCaptureSlice.h: interface for the XScreenCaptureSlice class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XSCREENCAPTURESLICE_H__
#define __XSCREENCAPTURESLICE_H__


class XScreenCaptureSlice  
{
public:
	XScreenCaptureSlice();
	virtual ~XScreenCaptureSlice();
public:
	bool Connect(HDC hDDC,HDC hNullDC,int x,int y,int cx,int cy,int nBitCount);
	void ReleaseConnections(void);
	bool Capture(char**ppData,int&nLen);
	void Move(int x,int y);
	char*GetData(void);
	int GetDataLen(void);
	int GetLeft(void){return m_nX;};
	int GetTop(void){return m_nY;};
	int GetWidth(void){return m_nCX;};
	int GetHeight(void){return m_nCY;};
	int GetBitCount(void){return m_nBitCount;};

	void GetColors(char**ppColorsData,int &nLen);

	void EnableCursor(bool bEnable);
protected:
	HDC			m_hMemDC;
	HBITMAP		m_hDIBitmap;
	int			m_nX;
	int			m_nY;
	int			m_nCX;
	int			m_nCY;
	int			m_nBitCount;

	HDC			m_hDDC;
	HDC			m_hNullDC;
	char*		m_pDIB;
	int			m_nDataLen;
	char*		m_pDIBData;

	bool		m_bEnableCursor;
	int			m_nCursorOffsetX;
	int			m_nCursorOffsetY;
};

#endif 