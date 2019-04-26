// XDCCapture.cpp: implementation of the XDCCapture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XDCCapture.h"
#include "VIDEC_CSP.h"
#include "../SubTitle/SubTitle.h"


#define XDC_SLICE_HEIGHT	64	

extern BOOL IsVistaOrAbove();


VIDEC_DC*VIDEC_DC::Create(void)
{
	NEWRECORD3( XDCCapture*,pXDCCapture,new XDCCapture() )
	//NULLReturnRet( pXDCCapture , pXDCCapture )  
	return pXDCCapture;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XDCCapture::XDCCapture(void)
:XImageSource(this)

,m_bEnableOSD(false)
,m_strOSD(_T(""))
,m_nAlign(0)
,m_nTop(-1)
,m_nLeft(-1)
{
	m_hDC=NULL;
	m_hNullDC=NULL;

	m_paXDCCaptureSlice=NULL;
	m_paXDCSliceYV12=NULL;
	m_nSliceCount=0;

	m_nX=0;
	m_nY=0;
	m_nCX=0;
	m_nCY=0;

	m_nBitCount=24;

	m_pYV12Buffer=NULL;
	m_nYV12Size=0;

	ZeroMemory(&m_fontOSD, sizeof(m_fontOSD));	
	m_fontOSD.lfWeight = FW_HEAVY;//FW_BOLD;
	strcpy(m_fontOSD.lfFaceName, _T("Tahoma"));
	m_fontOSD.lfHeight = -16;
	m_fontOSD.lfQuality = NONANTIALIASED_QUALITY;

	m_crText=(RGB(0,0,255));
	m_crBk=(RGB(255,255,255));

	m_bIsVistaOrAbove=IsVistaOrAbove();
	m_nSliceHeight=XDC_SLICE_HEIGHT;
}

XDCCapture::~XDCCapture(void)
{
}

int XDCCapture::Open(HDC hDC,int x,int y,int cx,int cy)
{
	if (cx<=32 && cy<=32)
	{
		cx=32;
		cy=32;
	}

	m_hDC=hDC;
	// Get a DC to Get the DIB From and Remap the System Palette
	m_hNullDC = GetDC(NULL);
	if (!m_hNullDC)
		return -1;

	m_nBitCount=24;

	if (cx%16!=0)
	{
		cx-=(cx%16);
	}
	if (cy%16!=0)
	{
		cy-=(cy%16);
	}

	if (cx<16)
		cx=16;
	if (cy<16)
		cy=16;

	m_nX=x;
	m_nY=y;
	m_nCX=cx;
	m_nCY=cy;

	if (m_bIsVistaOrAbove)
	{
		m_nSliceHeight=m_nCY;
	}

	if (0!=InitMem())
	{
		return -1;
	}

	return 0;
}

void XDCCapture::Close(void)
{
	XImageSource::Close();

	ClearMem();

	// Delete the Null DC
	if (m_hNullDC)
	{
		ReleaseDC(NULL,m_hNullDC);
		m_hNullDC=NULL;
	}
}

int XDCCapture::InitMem(void)
{
	m_nYV12Size=m_nCX*m_nCY*3/2;
	m_pYV12Buffer=(PBYTE)MALLOCMSG(m_nYV12Size+(m_nYV12Size>>2));
	if (m_pYV12Buffer==NULL)
	{
		return -1;
	}

	unsigned char*pY=m_pYV12Buffer;
	unsigned char*pU=pY+m_nCX*m_nCY;
	unsigned char*pV=pU+(m_nCX*m_nCY>>2);

	m_nSliceCount=m_nCY/m_nSliceHeight;
	int nLeftHeight=m_nCY%m_nSliceHeight;
	
	NEWRECORD( m_paXDCCaptureSlice,new XDCCaptureSlice[(m_nSliceCount+(nLeftHeight>0?1:0))] )
	NEWRECORD( m_paXDCSliceYV12,new XDC_SLICE_YV12[(m_nSliceCount+(nLeftHeight>0?1:0))] )
	NULLReturnRet( m_paXDCCaptureSlice , -1 )  
	NULLReturnRet( m_paXDCSliceYV12 , -1 )  

	int i=0;
	for (i=0;i<m_nSliceCount;i++)
	{
		m_paXDCCaptureSlice[i].Connect(m_hDC,m_hNullDC,m_nX,m_nY+i*m_nSliceHeight,m_nCX,m_nSliceHeight,m_nBitCount);
		m_paXDCSliceYV12[i].pYV12[0]=pY;
		m_paXDCSliceYV12[i].pYV12[1]=pV;
		m_paXDCSliceYV12[i].pYV12[2]=pU;
		m_paXDCSliceYV12[i].width=m_nCX;
		m_paXDCSliceYV12[i].height=m_nSliceHeight;

		pY+=m_paXDCSliceYV12[i].width*m_paXDCSliceYV12[i].height;
		pU+=(m_paXDCSliceYV12[i].width*m_paXDCSliceYV12[i].height>>2);
		pV+=(m_paXDCSliceYV12[i].width*m_paXDCSliceYV12[i].height>>2);
	}

	if (nLeftHeight>0)
	{
		m_paXDCCaptureSlice[i].Connect(m_hDC,m_hNullDC,m_nX,m_nY+m_nSliceCount*m_nSliceHeight,m_nCX,nLeftHeight,m_nBitCount);
		m_paXDCSliceYV12[i].pYV12[0]=pY;
		m_paXDCSliceYV12[i].pYV12[1]=pV;
		m_paXDCSliceYV12[i].pYV12[2]=pU;
		m_paXDCSliceYV12[i].width=m_nCX;
		m_paXDCSliceYV12[i].height=nLeftHeight;
		m_nSliceCount++;
	}

	return 0;
}

int XDCCapture::ResetMem(void)
{
	if (m_pYV12Buffer==NULL)
	{
		m_pYV12Buffer=(PBYTE)MALLOCMSG(m_nYV12Size+(m_nYV12Size>>2));
		if (m_pYV12Buffer==NULL)
		{
			return -1;
		}

		unsigned char*pY=m_pYV12Buffer;
		unsigned char*pU=pY+m_nCX*m_nCY;
		unsigned char*pV=pU+(m_nCX*m_nCY>>2);

		m_nSliceCount=m_nCY/m_nSliceHeight;
		int nLeftHeight=m_nCY%m_nSliceHeight;
		
		int i=0;
		for (i=0;i<m_nSliceCount;i++)
		{
			m_paXDCSliceYV12[i].pYV12[0]=pY;
			m_paXDCSliceYV12[i].pYV12[1]=pU;
			m_paXDCSliceYV12[i].pYV12[2]=pV;

			pY+=m_paXDCSliceYV12[i].width*m_paXDCSliceYV12[i].height;
			pU+=(m_paXDCSliceYV12[i].width*m_paXDCSliceYV12[i].height>>2);
			pV+=(m_paXDCSliceYV12[i].width*m_paXDCSliceYV12[i].height>>2);
		}

		if (nLeftHeight>0)
		{
			m_paXDCSliceYV12[i].pYV12[0]=pY;
			m_paXDCSliceYV12[i].pYV12[1]=pU;
			m_paXDCSliceYV12[i].pYV12[2]=pV;
			m_nSliceCount++;
		}
	}

	return (m_pYV12Buffer!=NULL?0:-1);
}

void XDCCapture::ClearMem(void)
{
	if (m_paXDCCaptureSlice)
	{
		for (int i=0;i<m_nSliceCount;i++)
		{
			m_paXDCCaptureSlice[i].ReleaseConnections();
		}

		delete[]m_paXDCCaptureSlice;
		m_paXDCCaptureSlice=NULL;
	}
	if (m_paXDCSliceYV12)
	{
		delete[]m_paXDCSliceYV12;
		m_paXDCSliceYV12=NULL;
	}

	if (m_pYV12Buffer)
	{
		FREEMSG(m_pYV12Buffer);
		m_pYV12Buffer=NULL;
	}
}

int XDCCapture::RequestFrame(void)
{
	if (0==ResetMem())
	{
		if (m_paXDCCaptureSlice[0].GetLeft()!=m_nX ||
			m_paXDCCaptureSlice[0].GetTop()!=m_nY)
		{
			for (int i=0;i<m_nSliceCount;i++)
			{
				m_paXDCCaptureSlice[i].Move(m_nX,m_nY+i*m_nSliceHeight);
			}
		}

		{
			char*pData=NULL;
			int nLen=0;
			for (int i=0;i<m_nSliceCount;i++)
			{
				m_paXDCCaptureSlice[i].Capture(&pData,nLen);
			}
		}

		{
			int nRet=-1;
			unsigned char*pData=NULL;
			for (int i=0;i<m_nSliceCount;i++)
			{
				pData=(unsigned char*)m_paXDCCaptureSlice[i].GetData();
				switch (m_nBitCount)
				{
				case 16:
					nRet=RGB565_TO_YV12((unsigned char*)pData,m_paXDCSliceYV12[i].pYV12,m_paXDCSliceYV12[i].width,m_paXDCSliceYV12[i].height);
					break;
				case 24:
					nRet=RGB24_TO_YV12((unsigned char*)pData,m_paXDCSliceYV12[i].pYV12,m_paXDCSliceYV12[i].width,m_paXDCSliceYV12[i].height);
					break;
				case 32:
					nRet=RGB32_TO_YV12((unsigned char*)pData,m_paXDCSliceYV12[i].pYV12,m_paXDCSliceYV12[i].width,m_paXDCSliceYV12[i].height);
					break;
				}
			}

			if (nRet!=0)
			{
				return -1;
			}
		}

		NEWRECORD3( XImage*,pXImage,new XImage(m_pYV12Buffer,m_nYV12Size) )
		NULLReturnRet( pXImage , -1 )  

		pXImage->AddRef();
		m_pYV12Buffer=NULL;

		pXImage->SetActualWidth(m_nCX);
		pXImage->SetActualHeight(m_nCY);
		pXImage->SetVirtualWidth(m_nCX);
		pXImage->SetVirtualHeight(m_nCY);
		pXImage->SetDoubleField(false);
		pXImage->SetEvenField(false);
		pXImage->SetOddField(false);
		pXImage->SetTimestamp(XGetTimestamp());

		if (m_bEnableOSD)
		{
#ifdef USED_SUB_TITLE
			SubTitle::DrawTitle(m_pYV12Buffer,m_nCX,m_nCX,m_nCY,m_nAlign,m_strOSD);
#else
			m_XOSDFilter.Process(m_pYV12Buffer,m_nCX,m_nCY,0,0,1.0,1.0,m_strOSD,m_nAlign);
#endif
		}

		SendImage(pXImage);

		pXImage->Release();
		pXImage=NULL;
	}	
	return 0;
}

int XDCCapture::AddImageSink(VIDEC_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void XDCCapture::RemoveImageSink(VIDEC_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

void XDCCapture::EnableOSD(bool bEnable)
{
	m_bEnableOSD=bEnable;
}

bool XDCCapture::IsOSDEnable(void)
{
	return m_bEnableOSD;
}

void XDCCapture::SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft)
{
	m_strOSD=cszText;

	m_strOSD.TrimRight();
	m_strOSD.TrimRight(_T("\r\n"));
	m_nAlign=nAlign;
	m_nTop=nTop;
	m_nLeft=nLeft;
}

void XDCCapture::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	m_fontOSD=fontOSD;
	m_crText=crText;
	m_crBk=crBk;

	m_XOSDFilter.SetOSDFont(m_fontOSD,m_crText,m_crBk);
}