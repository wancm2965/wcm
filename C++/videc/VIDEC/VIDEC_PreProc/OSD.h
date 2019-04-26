//OSD.h
#ifndef __OSD_H__
#define __OSD_H__

#include <WinSock2.h>
#include <WinGDI.h>

#define OSD_ALIGN_TOP_LEFT			0
#define OSD_ALIGN_BOTTOM_LEFT		1
#define OSD_ALIGN_TOP_CENTER		2
#define OSD_ALIGN_BOTTOM_CENTER		3
#define OSD_ALIGN_TOP_RIGHT			4
#define OSD_ALIGN_BOTTOM_RIGHT		5
#define OSD_ALIGN_TOP_CENTER_DOWN	6
#define OSD_ALIGN_BOTTOM_CENTER_UP	7
//#define OSD_ALIGN_TOP_LEFT_RIGHT	8


class OSD
{
public:
	OSD(void);
	virtual~OSD(void);
public:
	void ProcessYUV420(unsigned char*pData,int nWidth,int nHeight);

	int Open(int nWidth,int nHeight);
	void Close(void);
	int SetTitle(const char*cszTitle,unsigned long nColor,int nAlign=0,int nTop=-1,int nLeft=-1);
	void DrawRGB24(unsigned char*pRGBData);

	void DrawYUV420(unsigned char*pYUV420Data[],int nStride[]);
protected:
	void SetAlign(int nAlign,int nTop,int nLeft);
	void CalcOSDWidth(void);
protected:
	BITMAPINFO			m_bmpInfo;
	unsigned char*		m_pPaintBuffer;
	HDC					m_hPaintDC;
	HBITMAP				m_hDibSection;
	HGDIOBJ				m_hObject;
	HFONT				m_hFont;
	HFONT				m_hOldFont;

	int					m_nImageWidth;
	int					m_nImageHeight;

	int					m_nOverlayWidth;
	int					m_nOverlayHeight;
	int					m_nOverlayImageSize;
	int					m_nImageSize;
	int					m_nOverlayImageOffset;
	int					m_nOverlayYOffset;
	int					m_nOverlayUVOffset;

	char				m_szTitle[1024];
	int					m_nCorlor;

	int					m_nOSDAlign;
	int					m_nTop;
	int					m_nLeft;
	int					m_nDTFormatFlags;
	int					m_nTopDownOffset;
	int					m_nLeftRightOffset;

	unsigned char*		m_pOverlayYUV[3];
	int					m_nOSDWidth;
	int					m_nOSDWidthUV;
	int					m_nOverlayWidthUV;
	int					m_nOverlayHeightUV;

	int					m_nOSDOffsetY;
	int					m_nOSDOffsetUV;

	unsigned char*		m_pRGBBuffer;
};

#endif