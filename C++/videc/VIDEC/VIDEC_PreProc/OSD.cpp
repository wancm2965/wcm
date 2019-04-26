#include "OSD.h"
#include "VIDEC_CSP.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"

#define BOTTOM_MARGN	0

OSD::OSD(void)
{
	m_pPaintBuffer = NULL;
	m_hPaintDC = NULL;
	m_hDibSection = NULL;
	m_hObject = NULL;
	m_hFont = NULL;
	m_hOldFont = NULL;

	m_nOverlayWidth=0;
	m_nOverlayHeight=0;
	m_nOverlayImageSize=0;
	m_nImageSize = 0;
	m_nOverlayImageOffset = 0;
	m_nOverlayYOffset=0;
	m_nOverlayUVOffset=0;

	m_szTitle[0]='\0';
	m_nCorlor=RGB(0,0,255);

	m_nOSDAlign=0;
	m_nTop=-1;
	m_nLeft=-1;

	m_nDTFormatFlags=DT_CENTER;
	m_nTopDownOffset=0;
	m_nLeftRightOffset=0;

	m_nImageWidth=0;
	m_nImageHeight=0;

	m_pOverlayYUV[0]=NULL;
	m_nOSDWidth=0;
	m_nOSDWidthUV=0;
	m_nOverlayWidthUV=0;
	m_nOverlayHeightUV=0;

	m_nOSDOffsetY=0;
	m_nOSDOffsetUV=0;

	m_pRGBBuffer=NULL;
}

OSD::~OSD(void)
{
	Close();
}

#define WIDTHBYTES(bits) ((DWORD)(((bits)+31) & (~31)) / 8)
#define DIBWIDTHBYTES(bi) (DWORD)WIDTHBYTES((DWORD)(bi).biWidth * (DWORD)(bi).biBitCount)
#define _DIBSIZE(bi) (DIBWIDTHBYTES(bi) * (DWORD)(bi).biHeight)
#define DIBSIZE(bi) ((bi).biHeight < 0 ? (-1)*(_DIBSIZE(bi)) : _DIBSIZE(bi))


void OSD::ProcessYUV420(unsigned char*pData,int nWidth,int nHeight)
{
	if (strlen(m_szTitle)<=0)
	{
		return ;
	}

	if (nWidth!=m_nImageWidth || nHeight!=m_nImageHeight)
	{
		Open(nWidth,nHeight);
	}

	unsigned char*pYUV[3];
	pYUV[0]=pData;
	pYUV[1]=pYUV[0]+m_nImageWidth*m_nImageHeight;
	pYUV[2]=pYUV[1]+((m_nImageWidth*m_nImageHeight)>>2);
	int nStride[3];
	nStride[0]=m_nImageWidth;
	nStride[1]=nStride[2]=(m_nImageWidth>>1);

	DrawYUV420(pYUV,nStride);
}

int OSD::Open(int nWidth,int nHeight)
{
	Close();

	m_nImageWidth=nWidth;
	m_nImageHeight=nHeight;

	m_nOverlayWidthUV=(nWidth>>1);
	m_nOverlayWidth=nWidth;
	m_nImageSize=nWidth*nHeight*3;

	m_nTopDownOffset=(nHeight-nWidth*9/16)/2;
	if (m_nTopDownOffset<0)
	{
		m_nTopDownOffset=0;
	}
	m_nTopDownOffset+=(m_nTopDownOffset%2);

	m_nLeftRightOffset=(nWidth-nHeight*4/3)/2;
	if (m_nLeftRightOffset<0)
	{
		m_nLeftRightOffset=0;
	}
	m_nLeftRightOffset+=(m_nLeftRightOffset%2);

	memset(&m_bmpInfo,0,sizeof(BITMAPINFO));

	m_bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmpInfo.bmiHeader.biWidth  = nWidth;
	m_bmpInfo.bmiHeader.biHeight = nHeight;
	m_bmpInfo.bmiHeader.biPlanes = 1;

	m_bmpInfo.bmiHeader.biCompression = 3828804477;
	m_bmpInfo.bmiHeader.biBitCount = 24;

	m_bmpInfo.bmiHeader.biSizeImage = DIBSIZE(m_bmpInfo.bmiHeader);

	LOGFONTA logFont;
	ZeroMemory(&logFont, sizeof(logFont));	
	logFont.lfWeight = FW_THIN;
	strcpy(logFont.lfFaceName, "Tahoma");
	logFont.lfHeight = (LONG) (-10.00*((double)nWidth/320.00));
	logFont.lfQuality = NONANTIALIASED_QUALITY;

	m_nOverlayHeight = abs(logFont.lfHeight) + 16;

	int nTempMode=(m_nOverlayHeight%16);
	//if (nTempMode>0)
	{
		m_nOverlayHeight+=(16-nTempMode);
	}
	m_nOverlayImageSize=nWidth*m_nOverlayHeight*3;

	m_nOverlayHeightUV=(m_nOverlayHeight>>1);

	SetAlign(m_nOSDAlign,m_nTop,m_nLeft);


	void*pPaintBuffer=NULL;
	m_bmpInfo.bmiHeader.biCompression=BI_RGB;
	m_bmpInfo.bmiHeader.biHeight=m_nOverlayHeight;
	m_bmpInfo.bmiHeader.biSizeImage=DIBSIZE(m_bmpInfo.bmiHeader);

	m_hDibSection = CreateDIBSection(m_hPaintDC, (BITMAPINFO *) &m_bmpInfo, DIB_RGB_COLORS, &pPaintBuffer, NULL, 0);
	m_pPaintBuffer=(unsigned char*)pPaintBuffer;

	HDC hDC = GetDC(NULL);
	m_hPaintDC = CreateCompatibleDC(hDC);
	SetMapMode(m_hPaintDC, GetMapMode(hDC));
	SetBkMode(m_hPaintDC, TRANSPARENT);
	SetTextColor(m_hPaintDC, m_nCorlor);


	m_hFont = CreateFontIndirect(&logFont); 
	m_hOldFont = (HFONT) SelectObject(m_hPaintDC, m_hFont);
	m_hObject = SelectObject(m_hPaintDC, m_hDibSection);

	CalcOSDWidth();
	SetAlign(m_nOSDAlign,m_nTop,m_nLeft);

	return 0;
}

void OSD::Close(void)
{
	if (m_hPaintDC) 
	{
		if (m_hOldFont)
		{
			SelectObject(m_hPaintDC, m_hOldFont);
		}

		DeleteDC(m_hPaintDC); 
		m_hPaintDC=NULL;
	}

	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont=NULL;
	}

	if (m_hObject)
	{
		DeleteObject(m_hObject);
		m_hObject=NULL;
	}

	m_pPaintBuffer=NULL;
	m_hDibSection=NULL;

	if (m_pOverlayYUV[0])
	{
		FREEMSG(m_pOverlayYUV[0]);
		m_pOverlayYUV[0]=NULL;
	}

	if (m_pRGBBuffer)
	{
		FREEMSG(m_pRGBBuffer);
		m_pRGBBuffer=NULL;
	}
}

int OSD::SetTitle(const char*cszTitle,unsigned long nColor,int nAlign,int nTop,int nLeft)
{
	if (strcmp(m_szTitle,cszTitle)==0 && nColor == m_nCorlor && nAlign == m_nOSDAlign && m_nTop == nTop && m_nLeft==nLeft)
	{
		return 0;
	}
	if (cszTitle)
	{
		int nLen=strlen(cszTitle)+1;
		if (nLen>1023)
		{
			nLen=1023;
		}
		strncpy(m_szTitle,cszTitle,nLen);
	}
	else
	{
		m_szTitle[0]='\0';
	}
	m_nCorlor=nColor;

	if (m_hPaintDC)
	{
		SetTextColor(m_hPaintDC, m_nCorlor);

		CalcOSDWidth();
	}

	SetAlign(nAlign,nTop,nLeft);

	return 0;
}

void OSD::CalcOSDWidth(void)
{
	if (m_nLeft >=0 && m_nTop >= 0)
	{
		return;
	}
	if (strlen(m_szTitle)>0 && m_hPaintDC)
	{
		RECT rc;
		rc.left=0;
		rc.top=0;
		rc.right=m_nOverlayWidth;
		rc.bottom=m_nOverlayHeight;

		if(::DrawText(m_hPaintDC,m_szTitle, strlen(m_szTitle),&rc,DT_CALCRECT|DT_SINGLELINE|DT_LEFT))
		{
			int nOSDWidth=rc.right;
			int nTempMode=(nOSDWidth%16);
			if (nTempMode>0)
			{
				nOSDWidth+=(16-nTempMode);
			}

			if (nOSDWidth>m_nOverlayWidth)
			{
				nOSDWidth=m_nOverlayWidth;
			}
			m_nOSDWidth=nOSDWidth;
			m_nOSDWidthUV=(m_nOSDWidth>>1);
		}
	}
}

void OSD::SetAlign(int nAlign,int nTop,int nLeft)
{
	switch (nAlign)
	{
	default:
	case OSD_ALIGN_TOP_LEFT:
		{
			nAlign=OSD_ALIGN_TOP_LEFT;
			m_nDTFormatFlags=DT_LEFT;
			m_nOverlayImageOffset=m_nImageSize-m_nOverlayImageSize;
			m_nOverlayYOffset=0;
		}
		break;
	case OSD_ALIGN_TOP_CENTER:
		{
			m_nDTFormatFlags=DT_CENTER;
			m_nOverlayImageOffset=m_nImageSize-m_nOverlayImageSize;
			m_nOverlayYOffset=0;
		}
		break;
	case OSD_ALIGN_TOP_RIGHT:
		{
			m_nDTFormatFlags=DT_RIGHT;
			m_nOverlayImageOffset=m_nImageSize-m_nOverlayImageSize;
			m_nOverlayYOffset=0;
		}
		break;
	case OSD_ALIGN_BOTTOM_LEFT:
		{
			m_nDTFormatFlags=DT_LEFT;
			m_nOverlayImageOffset=(BOTTOM_MARGN*m_nOverlayWidth*3);
			m_nOverlayYOffset=(m_nImageHeight-m_nOverlayHeight-BOTTOM_MARGN)*m_nOverlayWidth;
		}
		break;
	case OSD_ALIGN_BOTTOM_CENTER:
		{
			m_nDTFormatFlags=DT_CENTER;
			m_nOverlayImageOffset=(BOTTOM_MARGN*m_nOverlayWidth*3);
			m_nOverlayYOffset=(m_nImageHeight-m_nOverlayHeight-BOTTOM_MARGN)*m_nOverlayWidth;
		}
		break;
	case OSD_ALIGN_BOTTOM_RIGHT:
		{
			m_nDTFormatFlags=DT_RIGHT;
			m_nOverlayImageOffset=(BOTTOM_MARGN*m_nOverlayWidth*3);
			m_nOverlayYOffset=(m_nImageHeight-m_nOverlayHeight-BOTTOM_MARGN)*m_nOverlayWidth;
		}
		break;
	case OSD_ALIGN_TOP_CENTER_DOWN:
		{
			m_nDTFormatFlags=DT_CENTER;
			m_nOverlayImageOffset=m_nImageSize-m_nOverlayImageSize-m_nTopDownOffset*m_nOverlayWidth*3;
			m_nOverlayYOffset=m_nTopDownOffset*m_nOverlayWidth;
		}
		break;
	case OSD_ALIGN_BOTTOM_CENTER_UP:
		{
			m_nDTFormatFlags=DT_CENTER;
			m_nOverlayImageOffset=m_nTopDownOffset*m_nOverlayWidth*3;
			m_nOverlayYOffset=m_nImageWidth*m_nImageHeight-m_nTopDownOffset*m_nOverlayWidth-m_nOverlayWidth*m_nOverlayHeight;
		}
		break;
	//case OSD_ALIGN_TOP_LEFT_RIGHT:
	//	{
	//		m_nDTFormatFlags=DT_LEFT;
	//		m_nOverlayImageOffset=m_nImageSize-m_nOverlayImageSize-m_nLeftRightOffset*m_nOverlayHeight*3;
	//		m_nOverlayYOffset=m_nLeftRightOffset;
	//	}
	//	break;
	}
	
	m_nOSDAlign=nAlign;
	m_nTop=nTop;
	m_nLeft=nLeft;

	if (m_nTop >= m_nImageHeight)
	{
		m_nTop = -1;
	}
	if (m_nLeft >= m_nImageWidth)
	{
		m_nTop = -1;
	}

	if (m_nTop >= 0 && m_nLeft >= 0 )
	{
		if (m_nTop%2 > 0)
		{
			m_nTop+=1;
		}
		if (m_nLeft%2 > 0)
		{
			m_nLeft+=1;
		}

		m_nDTFormatFlags = DT_LEFT;

		//if (m_nOverlayWidth + m_nLeft > m_nImageWidth)
		//{
		//	m_nOverlayWidth = m_nImageWidth - m_nLeft;
		//}
		if (m_nOverlayHeight + m_nTop > m_nImageHeight)
		{
			m_nTop = m_nImageHeight - m_nOverlayHeight;
		}
	}

	m_nOverlayUVOffset=(m_nOverlayYOffset>>2);

	switch (m_nDTFormatFlags)
	{
	case DT_CENTER:
		m_nOSDOffsetY=(m_nOverlayWidth-m_nOSDWidth)/2;
		break;
	case DT_RIGHT:
		m_nOSDOffsetY=(m_nOverlayWidth-m_nOSDWidth);
		break;
	default:
		m_nOSDOffsetY=0;
	}
	m_nOSDOffsetUV=(m_nOSDOffsetY>>1);
}

void OSD::DrawRGB24(unsigned char*pRGBData)
{
	if (m_szTitle && strlen(m_szTitle)>0 && m_pPaintBuffer)
	{
		//static int nFrameCount=0;
		//nFrameCount++;
		//if (nFrameCount%100==99)
		//{
		//	SetAlign(m_nOSDAlign+1);
		//}
		CopyMemory(m_pPaintBuffer, pRGBData+m_nOverlayImageOffset, m_nOverlayImageSize);

		RECT rc;
		rc.left=0;
		rc.top=0;
		rc.right=m_nOverlayWidth;
		rc.bottom=m_nOverlayHeight;

		if( ::DrawText(m_hPaintDC,m_szTitle, strlen(m_szTitle),&rc,m_nDTFormatFlags))
		{
			CopyMemory(pRGBData+m_nOverlayImageOffset, m_pPaintBuffer, m_nOverlayImageSize);
		}
	}
}

void OSD::DrawYUV420(unsigned char*pYUV420Data[],int nStride[])
{
	if (m_szTitle && strlen(m_szTitle)>0 && m_pPaintBuffer)
	{
		unsigned char*pYV12[3];
		pYV12[0]=pYUV420Data[0]+m_nOverlayYOffset;
		pYV12[1]=pYUV420Data[1]+m_nOverlayUVOffset;
		pYV12[2]=pYUV420Data[2]+m_nOverlayUVOffset;

		if (m_nTop >= 0 && m_nLeft >= 0)
		{
			pYV12[0]=pYUV420Data[0]+m_nTop*m_nImageWidth + m_nLeft;;
			pYV12[1]=pYUV420Data[1]+m_nTop*m_nImageWidth/4 + m_nLeft/2;
			pYV12[2]=pYUV420Data[2]+m_nTop*m_nImageWidth/4 + m_nLeft/2;
		}

		if (m_pOverlayYUV[0]==NULL)
		{
			m_pOverlayYUV[0]=(unsigned char*)MALLOCMSG(m_nOverlayWidth*m_nOverlayHeight*2);
			if (m_pOverlayYUV[0]==NULL)
			{
				return ;
			}
			m_pOverlayYUV[1]=m_pOverlayYUV[0]+m_nOverlayWidth*m_nOverlayHeight;
			m_pOverlayYUV[2]=m_pOverlayYUV[1]+((m_nOverlayWidth*m_nOverlayHeight)>>2);
		}

		if (m_pRGBBuffer==NULL)
		{
			m_pRGBBuffer=(unsigned char*)MALLOCMSG(m_nOverlayWidth*m_nOverlayHeight*4);
			if (m_pRGBBuffer==NULL)
			{
				return ;
			}
		}

		//static int nFrameCount=0;
		//nFrameCount++;
		//if (nFrameCount%100==99)
		//{
		//	SetAlign(m_nOSDAlign+1);
		//}
		//SetAlign(OSD_ALIGN_TOP_LEFT_RIGHT);



		RECT rc;
		rc.left=0;
		rc.top=0;
		rc.right=m_nOverlayWidth;
		rc.bottom=m_nOverlayHeight;

		int nRet=YV12_TO_RGB24(pYV12,m_pPaintBuffer,m_nOverlayWidth,m_nOverlayHeight,m_nOverlayWidth*3,1);
		if (nRet!=0)
		{
			return ;
		}


		if( ::DrawText(m_hPaintDC,m_szTitle, strlen(m_szTitle),&rc,m_nDTFormatFlags))
		{
			CopyMemory(m_pRGBBuffer,m_pPaintBuffer,m_nOverlayImageSize);

			RGB24_TO_YV12(m_pRGBBuffer,m_pOverlayYUV,m_nOverlayWidth,m_nOverlayHeight);

			if (m_nTop >= 0 && m_nLeft >= 0)
			{
				unsigned char*dstY=pYUV420Data[0];
				unsigned char*dstU=pYUV420Data[1];
				unsigned char*dstV=pYUV420Data[2];
				
				unsigned char*srcY=m_pOverlayYUV[0];
				unsigned char*srcU=m_pOverlayYUV[1];
				unsigned char*srcV=m_pOverlayYUV[2];

				dstY +=m_nTop*m_nImageWidth + m_nLeft;
				dstU +=m_nTop*m_nImageWidth/4 + m_nLeft/2;
				dstV +=m_nTop*m_nImageWidth/4 + m_nLeft/2;

				for (int i = 0;i<m_nOverlayHeight/2;i++)
				{
					memcpy(dstY,srcY,m_nOverlayWidth);
					dstY+=m_nImageWidth;
					srcY+=m_nOverlayWidth;

					memcpy(dstY,srcY,m_nOverlayWidth);
					dstY+=m_nImageWidth;
					srcY+=m_nOverlayWidth;

					memcpy(dstU,srcU,m_nOverlayWidth/2);
					dstU+= (m_nImageWidth/2);
					srcU+= (m_nOverlayWidth/2);

					memcpy(dstV,srcV,m_nOverlayWidth/2);
					dstV+= (m_nImageWidth/2);
					srcV+= (m_nOverlayWidth/2);
				}
			}
			else
			{
				unsigned char*pDstY=pYUV420Data[0]+m_nOSDOffsetY+m_nOverlayYOffset;
				unsigned char*pDstU=pYUV420Data[1]+m_nOSDOffsetUV+m_nOverlayUVOffset;
				unsigned char*pDstV=pYUV420Data[2]+m_nOSDOffsetUV+m_nOverlayUVOffset;
				unsigned char*pSrcY=m_pOverlayYUV[0]+m_nOSDOffsetY;
				unsigned char*pSrcU=m_pOverlayYUV[1]+m_nOSDOffsetUV;
				unsigned char*pSrcV=m_pOverlayYUV[2]+m_nOSDOffsetUV;
				for (int i=0;i<m_nOverlayHeightUV;++i)
				{
					memcpy(pDstY,pSrcY,m_nOSDWidth);
					pDstY+=m_nOverlayWidth;
					pSrcY+=m_nOverlayWidth;
					memcpy(pDstY,pSrcY,m_nOSDWidth);
					pDstY+=m_nOverlayWidth;
					pSrcY+=m_nOverlayWidth;

					memcpy(pDstU,pSrcU,m_nOSDWidthUV);
					pDstU+=m_nOverlayWidthUV;
					pSrcU+=m_nOverlayWidthUV;

					memcpy(pDstV,pSrcV,m_nOSDWidthUV);
					pDstV+=m_nOverlayWidthUV;
					pSrcV+=m_nOverlayWidthUV;
				}
			}

		}
	}
}
