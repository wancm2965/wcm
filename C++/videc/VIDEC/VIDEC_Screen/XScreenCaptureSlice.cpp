// XScreenCaptureSlice.cpp: implementation of the XScreenCaptureSlice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XScreenCaptureSlice.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define	LPBMIH				LPBITMAPINFOHEADER

extern BOOL IsVistaOrAbove();

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XScreenCaptureSlice::XScreenCaptureSlice()
{
	m_hMemDC=NULL;
	m_hDIBitmap=NULL;
	m_hDDC=NULL;
	m_nX=0;
	m_nY=0;
	m_nCX=0;
	m_nCY=0;
	m_nBitCount=0;
	m_pDIB=NULL;
	m_hNullDC=NULL;
	m_pDIB=NULL;
	m_pDIBData=NULL;
	m_nDataLen=0;

	m_bEnableCursor=true;
	
	if (IsVistaOrAbove())
	{
		m_nCursorOffsetX=0;
		m_nCursorOffsetY=0;
	}
	else
	{
		m_nCursorOffsetX=10;
		m_nCursorOffsetY=10;
	}
}

XScreenCaptureSlice::~XScreenCaptureSlice()
{

}

void XScreenCaptureSlice::Move(int x,int y)
{
	m_nX=x;
	m_nY=y;
}

bool XScreenCaptureSlice::Connect(HDC hDDC,HDC hNullDC,int x,int y,int cx,int cy,int nBitCount)
{
	m_nX=x;
	m_nY=y;
	m_nCX=cx;
	m_nCY=cy;
	m_nBitCount=nBitCount;

	m_hDDC=hDDC;
	m_hNullDC=hNullDC;
	// Create a Regional Compatible Memory DCs for the DC
	m_hMemDC = CreateCompatibleDC(hDDC);
	if (!m_hMemDC)
		return false;

	// Create a Regional Compatible Bitmaps for the DC
	m_hDIBitmap = CreateCompatibleBitmap(hDDC,cx,cy);
	if (!m_hDIBitmap)
		return false;

	return true;
}

void XScreenCaptureSlice::ReleaseConnections(void)
{
	// Delete the Memory DC
	if (m_hMemDC)
	{
		DeleteDC(m_hMemDC);
		m_hMemDC=NULL;
	}

	// Delete the Bitmap Object
	if (m_hDIBitmap)
	{
		DeleteObject(m_hDIBitmap);
		m_hDIBitmap=NULL;
	}

	if (m_pDIB)
	{
		FREEMSG(m_pDIB);
		m_pDIB=NULL;
	}
};

#define PT_IN_RECT(px,py,rx,ry,rcx,rcy)	(px>=rx && px<rx+rcx && py>=ry && py<=ry+rcy)

bool XScreenCaptureSlice::Capture(char**ppData,int&nLen)
{
	if (!m_hMemDC || !m_hDIBitmap)
		return false;

	// Select the Regional Bitmap Associated with the Device Context into the Regional Memory DC
	HBITMAP	hOldDIBitmap=(HBITMAP)SelectObject(m_hMemDC,m_hDIBitmap);

	// Blit the Screen in the Regional DC (DDB) to the Regional Memory DC
	BitBlt(m_hMemDC,0,0,m_nCX,m_nCY,m_hDDC,m_nX,m_nY,SRCCOPY);

	if (m_bEnableCursor)
	{
		CURSORINFO cursorInfo;

		cursorInfo.cbSize = sizeof(CURSORINFO);
		if (GetCursorInfo(&cursorInfo) && (cursorInfo.flags == CURSOR_SHOWING))
		{
			//CRect	rc1(m_nX, m_nY, m_nCX+m_nX, m_nCY+m_nY);
			//CRect	rc2(cursorInfo.ptScreenPos.x-16, cursorInfo.ptScreenPos.y-16, cursorInfo.ptScreenPos.x+16, cursorInfo.ptScreenPos.y+16);
			//CRect	rc3;

			//rc3.IntersectRect(rc1, rc2);
			//if (!rc3.IsRectNull())
			if (PT_IN_RECT((cursorInfo.ptScreenPos.x-16),(cursorInfo.ptScreenPos.y-16),m_nX, m_nY, m_nCX+m_nX, m_nCY+m_nY) ||
				PT_IN_RECT((cursorInfo.ptScreenPos.x-16),(cursorInfo.ptScreenPos.y+16),m_nX, m_nY, m_nCX+m_nX, m_nCY+m_nY) ||
				PT_IN_RECT((cursorInfo.ptScreenPos.x+16),(cursorInfo.ptScreenPos.y-16),m_nX, m_nY, m_nCX+m_nX, m_nCY+m_nY) ||
				PT_IN_RECT((cursorInfo.ptScreenPos.x+16),(cursorInfo.ptScreenPos.y+16),m_nX, m_nY, m_nCX+m_nX, m_nCY+m_nY))
			{
				DrawIcon(	m_hMemDC,
							cursorInfo.ptScreenPos.x-m_nX-m_nCursorOffsetX,
							cursorInfo.ptScreenPos.y-m_nY-m_nCursorOffsetY,
							cursorInfo.hCursor);
			}
		}
	}

	/* Step 2 : Convert the DDB to a DIB */
	// DIB
	// DIB Information Header

	BITMAPINFOHEADER	bmiHeader;
	// Initialize the Bitmap Info Header
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = m_nCX;
	bmiHeader.biHeight = m_nCY;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = m_nBitCount;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biSizeImage = 0;
	bmiHeader.biXPelsPerMeter = 0;
	bmiHeader.biYPelsPerMeter = 0;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;

	// Pointer to the DIB Information Header
	LPBITMAPINFOHEADER	lpbmiHeader;


	// Set the Regional Number of Colors
	int nColors = 1 << bmiHeader.biBitCount;
	if (nColors > 256)
		nColors = 0; // Palette Not Used

	// Set the Size of the Allocation to Hold the Bitmap Info Header and the Color Table
	DWORD dwLen = (DWORD)(sizeof(BITMAPINFOHEADER) + nColors * sizeof(RGBQUAD));

	// Allocate Memory to hold the Regional Bitmapinfoheader and Color Table
	if (!m_pDIB)
	{
		m_pDIB = (char *)MALLOCMSG(dwLen+dwLen/4);
		if(!m_pDIB) return false;
	}
	char*pStartDIB=m_pDIB;

	// Cast the Allocated Memory to a Bitmap Info Header for the Region
	lpbmiHeader = (LPBMIH)m_pDIB;

	// Set the Regional Bitmap Info Header to the Regional DIB
	*(lpbmiHeader) = bmiHeader;

	// Get the Regional DIB Size
	GetDIBits(m_hNullDC,m_hDIBitmap,0L,(DWORD)bmiHeader.biHeight,(LPBYTE)NULL,(LPBITMAPINFO)lpbmiHeader,DIB_RGB_COLORS);

	// Set the Regional Bitmap Info Header with the Calculated Size
	bmiHeader = *(lpbmiHeader);

	// Save the Size of the BitMap Header
	DWORD dwBitMapHeader = dwLen;

	// Calculate the New Size of the Regional DIB w/ the Size of the Image Added
	dwLen += (DWORD)(bmiHeader.biSizeImage);

	// ReAllocate the Memory to Hold the Entire Regional DIB
	m_pDIB = pStartDIB;
	m_pDIB = (char *)REALLOCMSG(m_pDIB,dwLen+dwLen/4);

	// Cast the Storage for the Regional DIB Bits
	lpbmiHeader = (LPBMIH)m_pDIB;

	// Get the Regional DIB
	int bGotBits = GetDIBits(m_hNullDC,m_hDIBitmap,0L,(DWORD)bmiHeader.biHeight,(LPBYTE)lpbmiHeader + (bmiHeader.biSize + nColors * sizeof(RGBQUAD)),(LPBITMAPINFO)lpbmiHeader,DIB_RGB_COLORS);

	*ppData=m_pDIB;
	nLen=dwLen;
	m_nDataLen=bmiHeader.biSizeImage;
	m_pDIBData=m_pDIB+dwBitMapHeader;
	SelectObject(m_hMemDC,hOldDIBitmap);

	return true;
}

char*XScreenCaptureSlice::GetData(void)
{
	return m_pDIBData;
}

int XScreenCaptureSlice::GetDataLen(void)
{
	return m_nDataLen;
}

void XScreenCaptureSlice::GetColors(char**ppColorsData,int &nLen)
{
	int nColors = 1 << m_nBitCount;
	if (nColors > 256)
		nColors = 0; // Palette Not Used

	// Set the Size of the Allocation to Hold the Bitmap Info Header and the Color Table
	nLen= nColors * sizeof(RGBQUAD);
	if (nLen)
	{
		*ppColorsData=m_pDIB+sizeof(BITMAPINFOHEADER);
	}
}

void XScreenCaptureSlice::EnableCursor(bool bEnable)
{
	m_bEnableCursor=bEnable;
}
