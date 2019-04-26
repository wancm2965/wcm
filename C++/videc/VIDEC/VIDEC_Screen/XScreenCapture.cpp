// XScreenCapture.cpp: implementation of the XScreenCapture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XScreenCapture.h"
#include "VIDEC_CSP.h"
#include "../SubTitle/SubTitle.h"


#define XSCREEN_SLICE_HEIGHT	64	

extern BOOL IsVistaOrAbove();

VIDEC_Screen*VIDEC_Screen::Create(void)
{
	NEWRECORD3( XScreenCapture*,pXScreenCapture,new XScreenCapture(NULL) )
	//NULLReturnRet( pXScreenCapture , pXScreenCapture )  
	return pXScreenCapture;
}

VIDEC_Screen*VIDEC_Screen::Create(HDC hDC)
{
	NEWRECORD3( XScreenCapture*,pXScreenCapture,new XScreenCapture(hDC) )
	//NULLReturnRet( pXScreenCapture , pXScreenCapture )  
	return pXScreenCapture;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XScreenCapture::XScreenCapture(HDC hDC)
:XImageSource(this)

,m_bEnableOSD(false)
,m_strOSD(_T(""))
,m_nAlign(0)
,m_nTop(-1)
,m_nLeft(-1)
,m_hDC(hDC)
{
	m_hDDC=NULL;
	m_hNullDC=NULL;

	m_paXScreenCaptureSlice=NULL;
	m_paXScreenSliceYV12=NULL;
	m_nSliceCount=0;

	m_nX=0;
	m_nY=0;
	m_nCX=0;
	m_nCY=0;

	m_nBitCount=16;

	m_pYV12Buffer=NULL;
	m_nYV12Size=0;

	m_bRunning=false;
	m_ulFrameInterval=0;
	m_nFrameRate=30;
	m_bRequestFrame=false;

	m_bEnableCursor=true;

	m_evFrameInterval = CreateEvent(NULL,TRUE,TRUE,NULL);
	ResetEvent(m_evFrameInterval);

	ZeroMemory(&m_fontOSD, sizeof(m_fontOSD));	
	m_fontOSD.lfWeight = FW_HEAVY;//FW_BOLD;
	strcpy(m_fontOSD.lfFaceName, _T("Tahoma"));
	m_fontOSD.lfHeight = -16;
	m_fontOSD.lfQuality = NONANTIALIASED_QUALITY;

	m_crText=(RGB(0,0,255));
	m_crBk=(RGB(255,255,255));

	m_pImage=NULL;
	m_nSkipFrameCount=0;

	m_bIsVistaOrAbove=IsVistaOrAbove();
	m_nSliceHeight=XSCREEN_SLICE_HEIGHT;
}

XScreenCapture::~XScreenCapture(void)
{
	CloseHandle(m_evFrameInterval);
}

int XScreenCapture::Open(int x,int y,int cx,int cy,int nFrameRate,int nBitCount)
{
	if (cx<=32 && cy<=32)
	{
		cx=32;
		cy=32;
	}

	m_nBitCount=nBitCount;
	if (m_nBitCount<=16)
	{
		m_nBitCount=16;
	}
	else if (m_nBitCount<=24)
	{
		m_nBitCount=24;
	}
	else 
	{
		m_nBitCount=32;
	}

	if (nFrameRate<=0)
	{
		m_ulFrameInterval=0;
		m_nFrameRate=1;
	}
	else
	{
		if (nFrameRate>=30)
		{
			nFrameRate=30;
		}
		m_nFrameRate=nFrameRate;
		m_ulFrameInterval=1000/nFrameRate-2;
	}

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

	if (!InitDisplay())
	{
		return -1;
	}

	if (0!=InitMem())
	{
		return -1;
	}

	m_bRunning=true;
	ResetEvent(m_evFrameInterval);
	if (!StartThread())
	{
		return -1;
	}

	ImageInput::Open();

	return 0;
}

void XScreenCapture::Close(void)
{
	ImageInput::Close();

	XImageSource::Close();

	if (m_bRunning)
	{
		m_bRunning=false;
		SetEvent(m_evFrameInterval);

		WaitForStop();
	}

	ClearDisplay();

	ClearMem();

	{
		XAutoLock l(m_csImage);
		if (m_pImage!=NULL)
		{
			m_pImage->Release();
			m_pImage=NULL;
		}
	}
}

int XScreenCapture::InitMem(void)
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
	
	NEWRECORD( m_paXScreenCaptureSlice,new XScreenCaptureSlice[(m_nSliceCount+(nLeftHeight>0?1:0))] )
	NULLReturnRet( m_paXScreenCaptureSlice , -1 )  
	NEWRECORD( m_paXScreenSliceYV12,new XSCREEN_SLICE_YV12[(m_nSliceCount+(nLeftHeight>0?1:0))] )
	NULLReturnRet( m_paXScreenSliceYV12 , -1 )  

	int i=0;
	for (i=0;i<m_nSliceCount;i++)
	{
		m_paXScreenCaptureSlice[i].Connect(m_hDC?m_hDC:m_hDDC,m_hNullDC,m_nX,m_nY+i*m_nSliceHeight,m_nCX,m_nSliceHeight,m_nBitCount);
		m_paXScreenSliceYV12[i].pYV12[0]=pY;
		m_paXScreenSliceYV12[i].pYV12[1]=pV;
		m_paXScreenSliceYV12[i].pYV12[2]=pU;
		m_paXScreenSliceYV12[i].width=m_nCX;
		m_paXScreenSliceYV12[i].height=m_nSliceHeight;

		pY+=m_paXScreenSliceYV12[i].width*m_paXScreenSliceYV12[i].height;
		pU+=(m_paXScreenSliceYV12[i].width*m_paXScreenSliceYV12[i].height>>2);
		pV+=(m_paXScreenSliceYV12[i].width*m_paXScreenSliceYV12[i].height>>2);
	}

	if (nLeftHeight>0)
	{
		m_paXScreenCaptureSlice[i].Connect(m_hDC?m_hDC:m_hDDC,m_hNullDC,m_nX,m_nY+m_nSliceCount*m_nSliceHeight,m_nCX,nLeftHeight,m_nBitCount);
		m_paXScreenSliceYV12[i].pYV12[0]=pY;
		m_paXScreenSliceYV12[i].pYV12[1]=pV;
		m_paXScreenSliceYV12[i].pYV12[2]=pU;
		m_paXScreenSliceYV12[i].width=m_nCX;
		m_paXScreenSliceYV12[i].height=nLeftHeight;
		m_nSliceCount++;
	}

	return 0;
}

int XScreenCapture::ResetMem(void)
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
			m_paXScreenSliceYV12[i].pYV12[0]=pY;
			m_paXScreenSliceYV12[i].pYV12[1]=pU;
			m_paXScreenSliceYV12[i].pYV12[2]=pV;

			pY+=m_paXScreenSliceYV12[i].width*m_paXScreenSliceYV12[i].height;
			pU+=(m_paXScreenSliceYV12[i].width*m_paXScreenSliceYV12[i].height>>2);
			pV+=(m_paXScreenSliceYV12[i].width*m_paXScreenSliceYV12[i].height>>2);
		}

		if (nLeftHeight>0)
		{
			m_paXScreenSliceYV12[i].pYV12[0]=pY;
			m_paXScreenSliceYV12[i].pYV12[1]=pU;
			m_paXScreenSliceYV12[i].pYV12[2]=pV;
			m_nSliceCount++;
		}
	}

	return (m_pYV12Buffer!=NULL?0:-1);
}

void XScreenCapture::ClearMem(void)
{
	if (m_paXScreenCaptureSlice)
	{
		for (int i=0;i<m_nSliceCount;i++)
		{
			m_paXScreenCaptureSlice[i].ReleaseConnections();
		}

		delete[]m_paXScreenCaptureSlice;
		m_paXScreenCaptureSlice=NULL;
	}
	if (m_paXScreenSliceYV12)
	{
		delete[]m_paXScreenSliceYV12;
		m_paXScreenSliceYV12=NULL;
	}

	if (m_pYV12Buffer)
	{
		FREEMSG(m_pYV12Buffer);
		m_pYV12Buffer=NULL;
	}
}

int XScreenCapture::Reset(int x,int y,int cx,int cy,int nFrameRate,int nBitCount)
{
	if (cx<=32 && cy<=32)
	{
		cx=32;
		cy=32;
	}
	//if (nFrameRate<=0)
	//{
	//	m_nSkipFrameCount=1;
	//}
	//else
	//{
	//	if (m_nSkipFrameCount!=0)
	//	{
	//		m_nSkipFrameCount=1;
	//	}
	//}

	bool bChanged=false;
	if (nBitCount<=16)
	{
		nBitCount=16;
	}
	else if (nBitCount<=24)
	{
		nBitCount=24;
	}
	else 
	{
		nBitCount=32;
	}

	if (nBitCount!=m_nBitCount)
	{
		bChanged=true;
	}

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

	if (cx!=m_nCX)
	{
		bChanged=true;
	}
	if (cy!=m_nCY)
	{
		bChanged=true;
	}

	if (bChanged)
	{
		m_nSkipFrameCount=5;
	}

	unsigned long ulInterval=0;
	if (nFrameRate<=0)
	{
		ulInterval=0;
		nFrameRate=1;
	}
	else
	{
		if (nFrameRate>=30)
		{
			nFrameRate=30;
		}
		ulInterval=1000/nFrameRate-2;
	}
	m_nFrameRate=nFrameRate;

	if (ulInterval!=m_ulFrameInterval)
	{
		m_ulFrameInterval=ulInterval;
	}

	m_nX=x;
	m_nY=y;

	if (bChanged)
	{
		if (m_bRunning)
		{
			m_bRunning=false;
			SetEvent(m_evFrameInterval);
			WaitForStop();
		}

		m_nBitCount=nBitCount;
		m_nCX=cx;
		m_nCY=cy;

		ClearDisplay();

		ClearMem();

		if (!InitDisplay())
		{
			return -1;
		}

		if (0!=InitMem())
		{
			return -1;
		}

		ResetEvent(m_evFrameInterval);
		m_bRunning=true;
		if (!StartThread())
		{
			return -1;
		}
	}

	return 0;
}

bool XScreenCapture::InitDisplay(void)
{
	ClearDisplay();

	if (m_hDC==NULL)
	{
		// Get the Device Context for the Entire Display
		m_hDDC = CreateDC(_T("DISPLAY"),NULL,NULL,NULL);
		if (!m_hDDC)
			return false;
	}

	// Get a DC to Get the DIB From and Remap the System Palette
	m_hNullDC = GetDC(NULL);
	if (!m_hNullDC)
		return false;

	return true;
}

void XScreenCapture::ClearDisplay(void)
{
	// Delete the Created Device Context
	if (m_hDDC)
	{
		DeleteDC(m_hDDC);
		m_hDDC=NULL;
	}

	// Delete the Null DC
	if (m_hNullDC)
	{
		ReleaseDC(NULL,m_hNullDC);
		m_hNullDC=NULL;
	}
}

int XScreenCapture::Move(int x,int y)
{
	m_nX=x;
	m_nY=y;
	return 0;
}

int XScreenCapture::GetRect(int& x,int& y,int& cx,int& cy)
{
	x=m_nX;
	y=m_nY;
	cx=m_nCX;
	cy=m_nCY;
	return 0;
}

int XScreenCapture::EnableCursor(bool bEnable)
{
	m_bEnableCursor=bEnable;
	for (int i=0;i<m_nSliceCount;i++)
	{
		m_paXScreenCaptureSlice[i].EnableCursor(m_bEnableCursor);
	}
	return 0;
}

int XScreenCapture::RequestFrame(void)
{
	m_bRequestFrame=true;
	return 0;
}



void XScreenCapture::ThreadProcMain(void)
{
	while (m_bRunning)
	{
		if (m_ulFrameInterval==0 && !m_bRequestFrame)
		{
			WaitForSingleObject(m_evFrameInterval,100);
			continue;
		}

		if (0==ResetMem())
		{
			if (m_paXScreenCaptureSlice[0].GetLeft()!=m_nX ||
				m_paXScreenCaptureSlice[0].GetTop()!=m_nY)
			{
				for (int i=0;i<m_nSliceCount;i++)
				{
					m_paXScreenCaptureSlice[i].Move(m_nX,m_nY+i*m_nSliceHeight);
				}
			}

			{
				char*pData=NULL;
				int nLen=0;
				for (int i=0;i<m_nSliceCount;i++)
				{
					m_paXScreenCaptureSlice[i].Capture(&pData,nLen);
				}
			}

			{
				int nRet=-1;
				unsigned char*pData=NULL;
				for (int i=0;i<m_nSliceCount;i++)
				{
					pData=(unsigned char*)m_paXScreenCaptureSlice[i].GetData();
					switch (m_nBitCount)
					{
					case 16:
						nRet=RGB565_TO_YV12((unsigned char*)pData,m_paXScreenSliceYV12[i].pYV12,m_paXScreenSliceYV12[i].width,m_paXScreenSliceYV12[i].height);
						break;
					case 24:
						nRet=RGB24_TO_YV12((unsigned char*)pData,m_paXScreenSliceYV12[i].pYV12,m_paXScreenSliceYV12[i].width,m_paXScreenSliceYV12[i].height);
						break;
					case 32:
						nRet=RGB32_TO_YV12((unsigned char*)pData,m_paXScreenSliceYV12[i].pYV12,m_paXScreenSliceYV12[i].width,m_paXScreenSliceYV12[i].height);
						break;
					}
				}

				if (nRet!=0)
				{
					if (m_ulFrameInterval==0)
					{
						m_bRequestFrame=false;
					}
					else
					{
						WaitForSingleObject(m_evFrameInterval,m_ulFrameInterval);
					}
					continue;
				}
			}

			NEWRECORD3( XImage*,pXImage,new XImage(m_pYV12Buffer,m_nYV12Size) )
			NULLRecord( pXImage )  
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
			pXImage->SetFrameRate(m_nFrameRate);

			if (m_bEnableOSD && m_strOSD.GetLength()>0)
			{
				m_OSD.ProcessYUV420(m_pYV12Buffer,m_nCX,m_nCY);
			}

			if (m_nSkipFrameCount>0)
			{
				m_nSkipFrameCount--;

				pXImage->Release();
				pXImage=NULL;

				XSleep(20);

				continue;
			}

			SendImage(pXImage);

			if (m_nScreenIndex>=0 && m_nWndIndex>=0)
			{
				XAutoLock l(m_csImage);
				if (m_pImage!=NULL)
				{
					m_pImage->Release();
					m_pImage=NULL;
				}
				m_pImage=pXImage;
				m_pImage->AddRef();
			}

			pXImage->Release();
			pXImage=NULL;
		}

		if (m_ulFrameInterval==0)
		{
			m_bRequestFrame=false;
		}
		else
		{
			WaitForSingleObject(m_evFrameInterval,m_ulFrameInterval);
		}
	}
}


int XScreenCapture::AddImageSink(VIDEC_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void XScreenCapture::RemoveImageSink(VIDEC_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

void XScreenCapture::EnableOSD(bool bEnable)
{
	m_bEnableOSD=bEnable;
}

bool XScreenCapture::IsOSDEnable(void)
{
	return m_bEnableOSD;
}

void XScreenCapture::SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft)
{
	m_strOSD=cszText;

	m_strOSD.TrimRight();
	m_strOSD.TrimRight(_T("\r\n"));
	m_nAlign=nAlign;
	m_nTop=nTop;
	m_nLeft=nLeft;
	m_OSD.SetTitle(m_strOSD,m_crText,m_nAlign,m_nTop,m_nLeft);
}

void XScreenCapture::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	m_fontOSD=fontOSD;
	m_crText=crText;
	m_crBk=crBk;

	m_OSD.SetTitle(m_strOSD,m_crText,m_nAlign,m_nTop,m_nLeft);
}


/******************************************************************************
* SetScreenWnd
*描述：设置所在屏幕窗口位置
*输入：	nScreenIndex	-屏幕号
		nWndIndex		-窗口号
		nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
*输出：无
*返回值：无
*****************************************************************************/
void XScreenCapture::SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	ImageInput::SetScreenIndex(nScreenIndex);
	ImageInput::SetWndIndex(nWndIndex);
	ImageInput::SetFullScreen(nFullScreen);
}

VIDEC_Image*XScreenCapture::GetImage(void)
{
	XAutoLock l(m_csImage);
	if (m_pImage!=NULL)
	{
		m_pImage->AddRef();
	}
	return m_pImage;
}
