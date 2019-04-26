// XOSDFilter.cpp: implementation of the XOSDFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XOSDFilter.h"
#include "VIDEC_csp.h"
#include "VIDEC_convert.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

bool GetOSDDIBBits(int nWidth,int nHeight,int nLeftOffset,int nAlign,COLORREF crText,COLORREF crBk,LOGFONTA logfont,LPCTSTR lpOSD,float fXRatio,PBYTE*ppDIBBits,int&biWidth,int&biHeight,int&biSizeImage);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XOSDFilter::XOSDFilter(void)
{
	m_nWidth=0;
	m_nHeight=0;
	m_nImageSizeY=0;

	m_fXRatio=1.0;
	m_fYRatio=1.0;
	m_strOSD=_T("");

	m_nAlign=0;
	m_crText=(RGB(0,0,255));
	m_crBk=(RGB(255,255,255));
	ZeroMemory(&m_fontOSD, sizeof(m_fontOSD));	
	m_fontOSD.lfWeight = FW_HEAVY;//FW_BOLD;
	strcpy(m_fontOSD.lfFaceName, _T("Tahoma"));
	m_fontOSD.lfHeight = -16;
	m_fontOSD.lfQuality = NONANTIALIASED_QUALITY;

	m_pOSDYUV=NULL;
	m_nOSDWidth=0;
	m_nOSDHeight=0;
	m_nOSDImageSizeY=0;

	m_nOSDClipWidth=0;
	m_nOSDWidthOffset=0;

	m_bFontChanged=false;
}

XOSDFilter::~XOSDFilter(void)
{
	if (m_pOSDYUV)
	{
		delete[]m_pOSDYUV;
		m_pOSDYUV=NULL;
	}
}

void XOSDFilter::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	m_fontOSD=fontOSD;
	m_fontOSD.lfQuality = NONANTIALIASED_QUALITY;

	{
		BYTE r=(BYTE)(crText&0xFF+128);
		BYTE g=(BYTE)((crText>>8)&0xFF+128);
		BYTE b=(BYTE)((crText>>16)&0xFF+128);

		if (r<16 && g<16 && b<16)
		{
			r=16;
			g=16;
			b=16;
		}
		m_crText=RGB(r,g,b);
	}
	{
		BYTE r=(BYTE)(crBk&0xFF+128);
		BYTE g=(BYTE)((crBk>>8)&0xFF+128);
		BYTE b=(BYTE)((crBk>>16)&0xFF+128);

		if (r<16 && g<16 && b<16)
		{
			r=16;
			g=16;
			b=16;
		}
		m_crBk=RGB(r,g,b);
	}

	m_bFontChanged=true;
}

	


bool XOSDFilter::Process(PBYTE pData,int nWidth,int nHeight,int nLeftOffset,int nTopOffset,float bXRatio,float bYRatio,LPCTSTR lpOSD,int nAlign,int nDoubleField)
{
	if (m_nWidth!=nWidth ||
		m_nHeight!=nHeight ||
		m_fXRatio!=bXRatio ||
		m_fYRatio!=bYRatio ||
		m_strOSD.Compare(lpOSD) ||
		m_nAlign!=nAlign ||
		m_bFontChanged)
	{
		m_bFontChanged=false;

		m_nWidth=nWidth;
		m_nHeight=nHeight;
		m_nImageSizeY=m_nWidth*m_nHeight;

		m_nAlign=nAlign;

		m_fXRatio=bXRatio;
		m_fYRatio=bYRatio;
		m_strOSD=lpOSD;

		if (m_pOSDYUV)
		{
			delete[]m_pOSDYUV;
			m_pOSDYUV=NULL;
		}	

		PBYTE pDIBBits=NULL;
		int biSizeImage=0;
		bool bRet= GetOSDDIBBits(m_nWidth,m_nHeight,nLeftOffset,nAlign,m_crText,m_crBk,m_fontOSD,m_strOSD,m_fXRatio,&pDIBBits,m_nOSDWidth,m_nOSDHeight,biSizeImage);
		
		if (pDIBBits)
		{
			m_nOSDWidthOffset=m_nOSDWidth%16;
			m_nOSDClipWidth=m_nOSDWidth-m_nOSDWidthOffset;
			m_nOSDImageSizeY=m_nOSDClipWidth*m_nOSDHeight;

			NEWRECORD( m_pOSDYUV,new BYTE[m_nOSDImageSizeY*3/2] )	
			//NULLReturnRet( m_pOSDYUV , false )  
			if (m_pOSDYUV)
			{
				PBYTE pSrcYUV[3];
				pSrcYUV[0]=m_pOSDYUV;
				pSrcYUV[1]=pSrcYUV[0]+m_nOSDImageSizeY;
				pSrcYUV[2]=pSrcYUV[1]+(m_nOSDImageSizeY>>2);

				RGB24_TO_YV12(pDIBBits,pSrcYUV,m_nOSDClipWidth,m_nOSDHeight,m_nOSDWidthOffset);
			}

			delete[]pDIBBits;
			pDIBBits=NULL;
		}
	}

	if (m_pOSDYUV)
	{
		unsigned char byteFilled=16;
		if (nAlign%2==0)
		{
			PBYTE pSrcY=m_pOSDYUV;
			PBYTE pSrcU=pSrcY+m_nOSDImageSizeY;
			PBYTE pSrcV=pSrcU+(m_nOSDImageSizeY>>2);
			PBYTE pDstY=pData;
			PBYTE pDstU=pDstY+m_nImageSizeY;
			PBYTE pDstV=pDstU+(m_nImageSizeY>>2);

			if (nDoubleField==0)
			{
				nTopOffset+=4;
				nTopOffset*=m_fYRatio;
				nTopOffset+=(nTopOffset%2);

				if (nTopOffset+m_nOSDHeight>m_nHeight)
					nTopOffset=m_nHeight-m_nOSDHeight;

				pDstY+=(m_nWidth*(nTopOffset));
				pDstU+=((m_nWidth*(nTopOffset))>>2);
				pDstV+=((m_nWidth*(nTopOffset))>>2);

				AddTopOSD(pSrcY,pSrcU,pSrcV,pDstY,pDstU,pDstV,nTopOffset);
			}
			else
			{
				nTopOffset+=4;
				nTopOffset*=m_fYRatio;
				nTopOffset+=(nTopOffset%2);

				if (nTopOffset+m_nOSDHeight>(m_nHeight>>1))
					nTopOffset=(m_nHeight>>1)-m_nOSDHeight;

				pDstY+=(m_nWidth*(nTopOffset));
				pDstU+=((m_nWidth*(nTopOffset))>>2);
				pDstV+=((m_nWidth*(nTopOffset))>>2);

				AddTopOSD(pSrcY,pSrcU,pSrcV,pDstY,pDstU,pDstV,nTopOffset);
			
				int nHalfHeight=m_nHeight>>1;
				pDstY=pData+m_nWidth*nHalfHeight;
				pDstU=pData+m_nImageSizeY;
				pDstV=pDstU+(m_nImageSizeY>>2);
				pDstV+=((m_nWidth*nHalfHeight)>>2);;
				pDstU+=((m_nWidth*nHalfHeight)>>2);

				pDstY+=(m_nWidth*(nTopOffset));
				pDstU+=((m_nWidth*(nTopOffset))>>2);
				pDstV+=((m_nWidth*(nTopOffset))>>2);

				AddTopOSD(pSrcY,pSrcU,pSrcV,pDstY,pDstU,pDstV,nTopOffset);
			}
		}
		else
		{
			PBYTE pSrcY=m_pOSDYUV;
			PBYTE pSrcU=pSrcY+m_nOSDImageSizeY;
			PBYTE pSrcV=pSrcU+(m_nOSDImageSizeY>>2);
			PBYTE pDstY=pData;
			PBYTE pDstU=pDstY+m_nImageSizeY;
			PBYTE pDstV=pDstU+(m_nImageSizeY>>2);

			if (nDoubleField==0)
			{
				nTopOffset+=4;
				nTopOffset*=m_fYRatio;
				nTopOffset+=(nTopOffset%2);

				if (nTopOffset+m_nOSDHeight>m_nHeight)
					nTopOffset=m_nHeight-m_nOSDHeight;

				pDstY+=(m_nWidth*(m_nHeight-m_nOSDHeight-(nTopOffset)));
				pDstU+=((m_nWidth*(m_nHeight-m_nOSDHeight-(nTopOffset)))>>2);
				pDstV+=((m_nWidth*(m_nHeight-m_nOSDHeight-(nTopOffset)))>>2);

				AddBottomOSD(pSrcY,pSrcU,pSrcV,pDstY,pDstU,pDstV,nTopOffset);
			}
			else
			{
				nTopOffset+=4;
				nTopOffset*=m_fYRatio;
				nTopOffset+=(nTopOffset%2);

				if (nTopOffset+m_nOSDHeight>(m_nHeight>>1))
					nTopOffset=(m_nHeight>>1)-m_nOSDHeight;

				pDstY+=(m_nWidth*(m_nHeight-m_nOSDHeight-(nTopOffset)));
				pDstU+=((m_nWidth*(m_nHeight-m_nOSDHeight-(nTopOffset)))>>2);
				pDstV+=((m_nWidth*(m_nHeight-m_nOSDHeight-(nTopOffset)))>>2);

				AddBottomOSD(pSrcY,pSrcU,pSrcV,pDstY,pDstU,pDstV,nTopOffset);
			
				pDstY=pData;
				pDstU=pDstY+m_nImageSizeY;
				pDstV=pDstU+(m_nImageSizeY>>2);

				pDstY+=(m_nWidth*((m_nHeight>>1)-m_nOSDHeight-(nTopOffset)));
				pDstU+=((m_nWidth*((m_nHeight>>1)-m_nOSDHeight-(nTopOffset)))>>2);
				pDstV+=((m_nWidth*((m_nHeight>>1)-m_nOSDHeight-(nTopOffset)))>>2);

				AddBottomOSD(pSrcY,pSrcU,pSrcV,pDstY,pDstU,pDstV,nTopOffset);
			}
		}
	}

	return true;
}

void XOSDFilter::AddTopOSD(PBYTE pOsdY,PBYTE pOsdU,PBYTE pOsdV,PBYTE pImgY,PBYTE pImgU,PBYTE pImgV,int nTopOffset)
{
	PBYTE pSrcY=pOsdY;
	PBYTE pSrcU=pOsdU;
	PBYTE pSrcV=pOsdV;
	PBYTE pDstY=pImgY;
	PBYTE pDstU=pImgU;
	PBYTE pDstV=pImgV;

	//nTopOffset+=4;
	//nTopOffset*=m_fYRatio;
	//nTopOffset+=(nTopOffset%2);

	//if (nTopOffset+m_nOSDHeight>m_nHeight)
	//	nTopOffset=m_nHeight-m_nOSDHeight;

	//pDstY+=(m_nWidth*(nTopOffset));
	//pDstU+=((m_nWidth*(nTopOffset))>>2);
	//pDstV+=((m_nWidth*(nTopOffset))>>2);

	if (m_nOSDClipWidth)
	{
		for (int j=0;j<(m_nOSDHeight>>1);j++)
		{
			for (int i=0;i<(m_nOSDClipWidth>>1);i++)
			{
				if (*pSrcY!=16)
				{
					*pDstY=*pSrcY;
				}
				++pSrcY;
				++pDstY;

				if (*pSrcY!=16)
				{
					*pDstY=*pSrcY;
				}
				++pSrcY;
				++pDstY;

				if (*pSrcU!=128)
				{
					*pDstU=*pSrcU;
				}
				++pSrcU;
				++pDstU;

				if (*pSrcV!=128)
				{
					*pDstV=*pSrcV;
				}
				++pSrcV;
				++pDstV;
			}

			pDstY+=m_nOSDWidthOffset;
			pDstU+=(m_nOSDWidthOffset>>1);
			pDstV+=(m_nOSDWidthOffset>>1);

			for (int k=0;k<m_nOSDClipWidth;k++)
			{
				if (*pSrcY!=16)
				{
					*pDstY=*pSrcY;
				}
				++pSrcY;
				++pDstY;
			}

			pDstY+=m_nOSDWidthOffset;				
		}
	}
	else
	{
		for (int i=0;i<((m_nOSDImageSizeY)>>2);i++)
		{
			if (*pSrcY!=16)
			{
				*pDstY=*pSrcY;
			}
			++pSrcY;
			++pDstY;

			if (*pSrcY!=16)
			{
				*pDstY=*pSrcY;
			}
			++pSrcY;
			++pDstY;

			if (*pSrcY!=16)
			{
				*pDstY=*pSrcY;
			}
			++pSrcY;
			++pDstY;

			if (*pSrcY!=16)
			{
				*pDstY=*pSrcY;
			}
			++pSrcY;
			++pDstY;

			if (*pSrcU!=128)
			{
				*pDstU=*pSrcU;
			}
			++pSrcU;
			++pDstU;

			if (*pSrcV!=128)
			{
				*pDstV=*pSrcV;
			}
			++pSrcV;
			++pDstV;
		}
	}
}

void XOSDFilter::AddBottomOSD(PBYTE pOsdY,PBYTE pOsdU,PBYTE pOsdV,PBYTE pImgY,PBYTE pImgU,PBYTE pImgV,int nTopOffset)
{
	PBYTE pSrcY=pOsdY;
	PBYTE pSrcU=pOsdU;
	PBYTE pSrcV=pOsdV;
	PBYTE pDstY=pImgY;
	PBYTE pDstU=pImgU;
	PBYTE pDstV=pImgV;

	//nTopOffset+=4;
	//nTopOffset*=m_fYRatio;
	//nTopOffset+=(nTopOffset%2);

	//if (nTopOffset+m_nOSDHeight>m_nHeight)
	//	nTopOffset=m_nHeight-m_nOSDHeight;

	//pDstY+=(m_nWidth*(m_nHeight-m_nOSDHeight-(nTopOffset)));
	//pDstU+=((m_nWidth*(m_nHeight-m_nOSDHeight-(nTopOffset)))>>2);
	//pDstV+=((m_nWidth*(m_nHeight-m_nOSDHeight-(nTopOffset)))>>2);

	if (m_nOSDClipWidth)
	{
		for (int j=0;j<(m_nOSDHeight>>1);j++)
		{
			for (int i=0;i<(m_nOSDClipWidth>>1);i++)
			{
				if (*pSrcY!=16)
				{
					*pDstY=*pSrcY;
				}
				++pSrcY;
				++pDstY;

				if (*pSrcY!=16)
				{
					*pDstY=*pSrcY;
				}
				++pSrcY;
				++pDstY;

				if (*pSrcU!=128)
				{
					*pDstU=*pSrcU;
				}
				++pSrcU;
				++pDstU;

				if (*pSrcV!=128)
				{
					*pDstV=*pSrcV;
				}
				++pSrcV;
				++pDstV;
			}

			pDstY+=m_nOSDWidthOffset;
			pDstU+=(m_nOSDWidthOffset>>1);
			pDstV+=(m_nOSDWidthOffset>>1);

			for (int k=0;k<m_nOSDClipWidth;k++)
			{
				if (*pSrcY!=16)
				{
					*pDstY=*pSrcY;
				}
				++pSrcY;
				++pDstY;
			}

			pDstY+=m_nOSDWidthOffset;				
		}
	}
	else
	{
		for (int i=0;i<((m_nOSDImageSizeY)>>2);i++)
		{
			if (*pSrcY!=16)
			{
				*pDstY=*pSrcY;
			}
			++pSrcY;
			++pDstY;

			if (*pSrcY!=16)
			{
				*pDstY=*pSrcY;
			}
			++pSrcY;
			++pDstY;

			if (*pSrcY!=16)
			{
				*pDstY=*pSrcY;
			}
			++pSrcY;
			++pDstY;

			if (*pSrcY!=16)
			{
				*pDstY=*pSrcY;
			}
			++pSrcY;
			++pDstY;

			if (*pSrcU!=128)
			{
				*pDstU=*pSrcU;
			}
			++pSrcU;
			++pDstU;

			if (*pSrcV!=128)
			{
				*pDstV=*pSrcV;
			}
			++pSrcV;
			++pDstV;
		}
	}
}


bool GetOSDDIBBits(int nWidth,int nHeight,int nLeftOffset,int nAlign,COLORREF crText,COLORREF crBk,LOGFONTA logfont,LPCTSTR lpOSD,float fXRatio,PBYTE*ppDIBBits,int&biWidth,int&biHeight,int&biSizeImage)
{
	bool bRet=false;
	
	HDC hDC = GetDC(NULL);
	HDC hMemDC = CreateCompatibleDC(NULL);
	if (hMemDC)
	{
		nLeftOffset*=fXRatio;
		nLeftOffset+=4;

		// Create a logical font based on the user's selection and 
		// return a handle identifying that font.  
		HFONT hFont = CreateFontIndirect(&logfont); 
		HFONT hOldFont = (HFONT) SelectObject(hMemDC, hFont);

		int nLines=1;
		CString strText=lpOSD;
		CString strTemp=strText;
		CString strMaxLenLine="";
		int nMaxLineLen=0;
		while (true)
		{
			int nPos=strTemp.Find("\r\n");
			if (nPos>=0)
			{
				nLines++;
				if (nMaxLineLen<nPos)
					strMaxLenLine=strTemp.Left(nPos);

				strTemp=strTemp.Mid(nPos+2);
			}
			else
			{
				if (nMaxLineLen<strTemp.GetLength())
					strMaxLenLine=strTemp;
				break;
			}
		}
		if (nLines==1)
		{
			strMaxLenLine=strText;
		}

		
		SIZE tOSDSize;
		GetTextExtentPoint32(hMemDC, strMaxLenLine, lstrlen(strMaxLenLine), &tOSDSize);

		biWidth=nWidth;
		//int nTempHeight=nTopOffset+tOSDSize.cy*nLines;
		int nTempHeight=tOSDSize.cy*nLines;

#define OSD_ALIGN	16
		biHeight=nTempHeight+(OSD_ALIGN-nTempHeight%OSD_ALIGN);
		if (biHeight>nHeight)
			biHeight=nHeight;

		if (nLeftOffset+OSD_ALIGN>biWidth)
			nLeftOffset=0;

		BITMAPINFOHEADER bmiHeader;
		memset(&bmiHeader,0,sizeof(BITMAPINFOHEADER));
		bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmiHeader.biPlanes = 1;
		bmiHeader.biBitCount = 24;
		bmiHeader.biCompression = BI_RGB;
		bmiHeader.biXPelsPerMeter = 0;
		bmiHeader.biYPelsPerMeter = 0;
		bmiHeader.biClrUsed = 0;
		bmiHeader.biClrImportant = 0;
		bmiHeader.biWidth = biWidth;
		bmiHeader.biHeight = biHeight;
		bmiHeader.biSizeImage = bmiHeader.biWidth*bmiHeader.biHeight*3;

		biSizeImage=bmiHeader.biSizeImage;
	
		HBITMAP hBitmap = CreateDIBitmap(hDC, &bmiHeader, 0, NULL, NULL, 0);
		if (hBitmap)
		{
			HGDIOBJ hOldBitmap = SelectObject(hMemDC, hBitmap);
			
			SetBkMode(hMemDC,TRANSPARENT);
			CRect rcText(0,0,biWidth,biHeight);
			
			UINT uFormat=DT_LEFT;
			if (nAlign==2 || nAlign==3)
			{
				uFormat=DT_CENTER;
			}
			else if (nAlign==4 || nAlign==5)
			{
				uFormat=DT_RIGHT;
			}
			
			CRect rcFilled;
			if (nAlign==0 || nAlign==1)
			{
				rcText.left+=nLeftOffset;

				rcFilled.left=rcText.left;
				rcFilled.right=tOSDSize.cx+rcFilled.left;
				rcFilled.top=0;
				rcFilled.bottom=tOSDSize.cy*nLines;
			}
			else if (nAlign==4 || nAlign==5)
			{
				rcText.right-=nLeftOffset;

				rcFilled.right=rcText.right;
				rcFilled.left=rcFilled.right-tOSDSize.cx;
				rcFilled.top=0;
				rcFilled.bottom=tOSDSize.cy*nLines;
			}
			else
			{
				rcFilled.right=(rcText.right+tOSDSize.cx)>>1;
				rcFilled.left=rcFilled.right-tOSDSize.cx;
				rcFilled.top=0;
				rcFilled.bottom=tOSDSize.cy*nLines;
			}


			BYTE byteTextR=crText&0xFF;
			BYTE byteTextG=(crText>>8)&0xFF;
			BYTE byteTextB=(crText>>16)&0xFF;
			if (byteTextR<16 && byteTextG<16 && byteTextB<16)
			{
				byteTextB=16;
				crText=RGB(byteTextR,byteTextG,byteTextB);
			}
			BYTE byteBkR=crBk&0xFF;
			BYTE byteBkG=(crBk>>8)&0xFF;
			BYTE byteBkB=(crBk>>16)&0xFF;
			if (byteBkR<16 && byteBkG<16 && byteBkB<16)
			{
				byteBkB=16;
				crBk=RGB(byteBkR,byteBkG,byteBkB);
			}

			CRect rcBk;
			for (int nX=-2;nX<=2;nX++)
			{
				for (int nY=-2;nY<=2;nY++)
				{
					rcBk=rcText;
					rcBk.OffsetRect(nX, nY);
					SetTextColor(hMemDC,crBk);	
					DrawText(hMemDC,strText,lstrlen(strText),rcBk, uFormat);
				}
			}

			SetTextColor(hMemDC,crText);	
			DrawText(hMemDC,strText,lstrlen(strText),rcText, uFormat);
			
		
			NEWRECORD3( PBYTE ,pDIBBits , new BYTE[bmiHeader.biSizeImage] )
			NULLReturnRet( pDIBBits , false )  
			memset(pDIBBits, 255, bmiHeader.biSizeImage);
			LONG lLines = GetDIBits(hMemDC, hBitmap, 0, bmiHeader.biHeight, pDIBBits, (BITMAPINFO*)&bmiHeader, DIB_RGB_COLORS);
			*ppDIBBits=pDIBBits;
			

			SelectObject(hMemDC, hOldBitmap);
			DeleteObject(hBitmap);
			bRet=true;
		}
	

		SelectObject(hMemDC, hOldFont);
		DeleteObject(hFont);

		DeleteDC(hMemDC);
	}

	ReleaseDC(NULL,hDC);
	return bRet;
}
