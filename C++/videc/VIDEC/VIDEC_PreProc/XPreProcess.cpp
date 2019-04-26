#include "stdafx.h"
#include "XPreProcess.h"
#include "VIDEC_convert.h"
#include "XUtil.h"
#include "../VIDEC_csp/fastmemcpy.h"
#include "../SubTitle/SubTitle.h"
#include "AVMPlayer.h"

#include "ContrastEnhance.h"

IXPreProcess*CreateIXPreProcess(IXPreProcessNotify&rIXPreProcessNotify)
{
	NEWRECORD3( XPreProcess*,pXPreProcess,new XPreProcess(rIXPreProcessNotify) )
	NULLReturnRet( pXPreProcess , pXPreProcess )  
	if (!pXPreProcess->Connect())
	{
		pXPreProcess->ReleaseConnections();
		delete pXPreProcess;
		pXPreProcess=NULL;
	}
	return pXPreProcess;
}


inline void InvertImage(int nWidth,int nHeight,unsigned char*pSrc,unsigned char*pDst)
{
	unsigned char*pSrcTemp=pSrc;
	unsigned char*pDstTemp=pDst+nWidth*nHeight-1;

	for (int i=0;i<nHeight;i++)
	{
		for (int j=0;j<nWidth;j++)
		{
			*pDstTemp--=*pSrcTemp++;
		}
	}
}

XPreProcess::XPreProcess(IXPreProcessNotify&rIXPreProcessNotify)
:m_rIXPreProcessNotify(rIXPreProcessNotify)

,m_nWidth(0)
,m_nHeight(0)
,m_pBuffer(NULL)

,m_nYSize(0)
,m_nUVSize(0)

,m_bI420Enable(false)
,m_bInvertEnable(false)

,m_bDenoiseEnable(false)

,m_bInterlacedEnable(false)
,m_bDeinterlacedEnable(false)
,m_bDoubleFrameRate(false)

,m_pDenoise(NULL)
,m_pDeinterlace(NULL)

,m_bEnableOSD(false)
,m_strOSD(_T(""))
,m_nAlign(0)
,m_nTop(-1)
,m_nLeft(-1)
{
	m_pRectList=NULL;
	m_iRectCount=0;
	m_nBufLen=0;
	m_pFrameBuf=NULL;

	m_pPictProp=NULL;

	m_nHue=VIDEC_GetDefaultHue();
	m_nSaturation=VIDEC_GetDefaultSaturation();
	m_nGamma=VIDEC_GetDefaultGamma();
	m_nGammaR=VIDEC_GetDefaultGamma();
	m_nGammaG=VIDEC_GetDefaultGamma();
	m_nGammaB=VIDEC_GetDefaultGamma();
	m_nSharpen=VIDEC_GetDefaultSharpen();

	m_nContrast=128;
	m_nBrightness=0;

	m_bResetPictProp=false;
	m_bDoPictProp=false;

	ZeroMemory(&m_fontOSD, sizeof(m_fontOSD));	
	m_fontOSD.lfWeight = FW_HEAVY;//FW_BOLD;
	strcpy(m_fontOSD.lfFaceName, _T("Tahoma"));
	m_fontOSD.lfHeight = -16;
	m_fontOSD.lfQuality = NONANTIALIASED_QUALITY;

	m_crText=(RGB(0,0,255));
	m_crBk=(RGB(255,255,255));


	m_bEnableImageEnhance=false;
	m_pContrastEnhance=NULL;
	m_nContrastEnhanceAlpha=1;
}

XPreProcess::~XPreProcess(void)
{
	if (m_pRectList)
	{
		FREEMSG(m_pRectList);
		m_pRectList=NULL;
	}
	if (m_pFrameBuf!=NULL)
	{
		FREEMSG(m_pFrameBuf);
		m_pFrameBuf=NULL;
	}
}


bool XPreProcess::Connect(void)
{
	return true;
}

void XPreProcess::ReleaseConnections(void)
{
	if (m_pBuffer)
	{
		FREEMSG(m_pBuffer);
		m_pBuffer=NULL;
	}

	if (m_pDenoise)
	{
		m_pDenoise->Close();
		delete m_pDenoise;
		m_pDenoise=NULL;
	}

	if (m_pDeinterlace)
	{
		m_pDeinterlace->ReleaseConnections();
		delete m_pDeinterlace;
		m_pDeinterlace=NULL;
	}

	if (m_pPictProp!=NULL)
	{
		m_pPictProp->Close();
		delete m_pPictProp;
		m_pPictProp=NULL;
	}

#ifdef __ENABLE_CONTRAST_ENHANCE__
	if (m_pContrastEnhance)
	{
		ContrastEnhanceClose(m_pContrastEnhance);
		m_pContrastEnhance=NULL;
	}
#endif
}

void XPreProcess::PreProcess(VIDEC_Image*pImage)
{
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
	{
		return ;
	}

	if (m_nWidth!=pXImage->GetActualWidth() ||
		m_nHeight!=pXImage->GetActualHeight())
	{
		m_nWidth=pXImage->GetActualWidth();
		m_nHeight=pXImage->GetActualHeight();

		m_nYSize=m_nWidth*m_nHeight;
		m_nUVSize=m_nYSize/4;
		m_naStride[0]=m_nWidth;
		m_naStride[1]=m_nWidth>>1;
		m_naStride[2]=m_nWidth>>1;

		if (m_pBuffer)
		{
			FREEMSG(m_pBuffer);
			m_pBuffer=NULL;
		}

		if (m_pDenoise)
		{
			m_pDenoise->Close();
			delete m_pDenoise;
			m_pDenoise=NULL;
		}

		if (m_pDeinterlace)
		{
			m_pDeinterlace->ReleaseConnections();
			delete m_pDeinterlace;
			m_pDeinterlace=NULL;
		}

#ifdef __ENABLE_CONTRAST_ENHANCE__
		if (m_pContrastEnhance)
		{
			ContrastEnhanceClose(m_pContrastEnhance);
			m_pContrastEnhance=NULL;
		}
#endif

		if (m_bDoPictProp)
		{
			m_bResetPictProp=true;
		}

		if (m_pPictProp!=NULL)
		{
			m_pPictProp->Close();
			delete m_pPictProp;
			m_pPictProp=NULL;
		}

		//m_pDenoise=VIDEC_Denoise::Create();
		//if (m_pDenoise!=NULL)
		//{
		//	if (0!=m_pDenoise->Open(m_nWidth,m_nHeight))
		//	{
		//		m_pDenoise->Close();
		//		delete m_pDenoise;
		//		m_pDenoise=NULL;
		//	}
		//}


		m_pPictProp=VIDEC_PictProp::Create();
		if (m_pPictProp!=NULL)
		{
			if (m_pPictProp->Open(m_nWidth,m_nHeight)!=0)
			{
				m_pPictProp->Close();
				delete m_pPictProp;
				m_pPictProp=NULL;
			}
			else
			{
			}
		}

#ifdef __ENABLE_CONTRAST_ENHANCE__
		//if (m_nWidth>=640 && m_nHeight>=480)
		{
			m_pContrastEnhance=ContrastEnhanceOpen(m_nWidth,m_nHeight);
		}
#endif
	}

	if (m_bDeinterlacedEnable && m_pDeinterlace==NULL)
	{
		m_pDeinterlace=CreateIXDeinterlace(m_nWidth,m_nHeight);
	}

	if (m_bDenoiseEnable && m_pDenoise==NULL)
	{
		m_pDenoise=VIDEC_Denoise::Create();
		if (m_pDenoise!=NULL)
		{
			if (0!=m_pDenoise->Open(m_nWidth,m_nHeight))
			{
				m_pDenoise->Close();
				delete m_pDenoise;
				m_pDenoise=NULL;
			}
		}
	}

	if (m_bResetPictProp && m_pPictProp!=NULL)
	{
		m_bResetPictProp=false;
		m_pPictProp->SetSaturation(m_nSaturation);
		m_pPictProp->SetBrightness(m_nBrightness);
		m_pPictProp->SetContrast(m_nContrast);
		m_pPictProp->SetHue(m_nHue);
		m_pPictProp->SetGamma(m_nGamma);
		m_pPictProp->SetGammaRGB(m_nGammaR,m_nGammaG,m_nGammaB);
		m_pPictProp->SetSharpen(m_nSharpen);
	}

	unsigned char*pData=(unsigned char*)pXImage->GetData();
	int nLen=pXImage->GetDataLen();

	unsigned char*pFiltered=pData;
	bool bProcessedI420=false;

	if ((m_bEnableImageEnhance && m_pContrastEnhance) ||
		m_bI420Enable || 
		m_bInvertEnable || 
		(m_bDenoiseEnable && m_pDenoise!=NULL) || 
		(m_bDeinterlacedEnable && m_pDeinterlace!=NULL && m_nHeight>288 && m_bDoubleFrameRate==false) ||
		(m_pPictProp!=NULL && m_pPictProp->NeedProcess()))
	{
		if (m_pBuffer==NULL)
		{
			m_pBuffer=(unsigned char*)MALLOCMSG(nLen*1.25);
			if (m_pBuffer==NULL)
			{
				OutputData(pXImage);
				return;
			}
		}

#define XSWAP(x,y,type)	{type t;t=x;x=y;y=t;}
		unsigned char*pSrc[3];
		pSrc[0]=pData;
		pSrc[1]=pSrc[0]+m_nYSize;
		pSrc[2]=pSrc[1]+m_nUVSize;
		if (m_bI420Enable)
		{
			XSWAP(pSrc[1],pSrc[2],unsigned char*)
		}

		unsigned char*pDst[3];
		pDst[0]=m_pBuffer;
		pDst[1]=pDst[0]+m_nYSize;
		pDst[2]=pDst[1]+m_nUVSize;

		if (m_pPictProp!=NULL && m_pPictProp->NeedProcess())
		{
			m_pPictProp->DoProcess(pSrc,pDst);
			XSWAP(pSrc[0],pDst[0],unsigned char*);
			XSWAP(pSrc[1],pDst[1],unsigned char*);
			XSWAP(pSrc[2],pDst[2],unsigned char*);

			pFiltered=(pFiltered==pData?m_pBuffer:pData);

			if (bProcessedI420==false && m_bI420Enable)
			{
				bProcessedI420=true;
				XSWAP(pSrc[1],pSrc[2],unsigned char*)
			}
		}

		if ((m_bEnableImageEnhance && m_pContrastEnhance) ||
			m_bInvertEnable || 
			(m_bDenoiseEnable && m_pDenoise!=NULL) ||
			(m_bDeinterlacedEnable && m_pDeinterlace!=NULL && m_nHeight>288 && m_bDoubleFrameRate==false))
		{
			if (m_bInvertEnable)
			{
				if (pXImage->GetDoubleField())
				{
					pXImage->SetEvenFieldFirst(!pXImage->GetEvenFieldFirst());
				}
				InvertImage(m_nWidth,m_nHeight,pSrc[0],pDst[0]);
				InvertImage(m_nWidth>>1,m_nHeight>>1,pSrc[1],pDst[1]);
				InvertImage(m_nWidth>>1,m_nHeight>>1,pSrc[2],pDst[2]);

				XSWAP(pSrc[0],pDst[0],unsigned char*);
				XSWAP(pSrc[1],pDst[1],unsigned char*);
				XSWAP(pSrc[2],pDst[2],unsigned char*);

				pFiltered=(pFiltered==pData?m_pBuffer:pData);

				if (bProcessedI420==false && m_bI420Enable)
				{
					bProcessedI420=true;
					XSWAP(pSrc[1],pSrc[2],unsigned char*)
				}
			}

			if (m_bDenoiseEnable && m_pDenoise!=NULL)
			{
				if (0==m_pDenoise->Denoise(pSrc,pDst))
				{
					XSWAP(pSrc[0],pDst[0],unsigned char*);
					XSWAP(pSrc[1],pDst[1],unsigned char*);
					XSWAP(pSrc[2],pDst[2],unsigned char*);

					pFiltered=(pFiltered==pData?m_pBuffer:pData);
					
					if (bProcessedI420==false && m_bI420Enable)
					{
						bProcessedI420=true;
						XSWAP(pSrc[1],pSrc[2],unsigned char*)
					}
				}
			}


			if (m_bDeinterlacedEnable && m_pDeinterlace!=NULL && m_nHeight>288 && m_bDoubleFrameRate==false)
			{
				if (m_pDeinterlace->Deinterlace(pSrc,m_naStride,pDst,m_naStride,m_nWidth,m_nHeight))
				{
					XSWAP(pSrc[0],pDst[0],unsigned char*);
					XSWAP(pSrc[1],pDst[1],unsigned char*);
					XSWAP(pSrc[2],pDst[2],unsigned char*);

					pFiltered=(pFiltered==pData?m_pBuffer:pData);
					
					if (bProcessedI420==false && m_bI420Enable)
					{
						bProcessedI420=true;
						XSWAP(pSrc[1],pSrc[2],unsigned char*)
					}
				}
			}

#ifdef __ENABLE_CONTRAST_ENHANCE__
			if (m_bEnableImageEnhance && m_pContrastEnhance)
			{
				ContrastEnhanceProcess(m_pContrastEnhance,pSrc[0],m_nContrastEnhanceAlpha);
			}
#endif
		}
		else
		{
			fastmemcpy(pDst[0],pSrc[0],m_nYSize);
			fastmemcpy(pDst[1],pSrc[1],m_nUVSize);
			fastmemcpy(pDst[2],pSrc[2],m_nUVSize);

			pFiltered=(pFiltered==pData?m_pBuffer:pData);
		}

		if (pFiltered==pData)
		{
			OutputData(pXImage);
		}
		else
		{
			m_pBuffer=NULL;

			NEWRECORD3( XImage*,pXImageNew,new XImage(pFiltered,nLen) )
			NULLRecord( pXImageNew )  
			pXImageNew->AddRef();

			pXImageNew->SetActualWidth(pXImage->GetActualWidth());
			pXImageNew->SetActualHeight(pXImage->GetActualHeight());
			pXImageNew->SetVirtualWidth(pXImage->GetVirtualWidth());
			pXImageNew->SetVirtualHeight(pXImage->GetVirtualHeight());
			pXImageNew->SetEvenFieldFirst(pXImage->GetEvenFieldFirst());
			pXImageNew->SetDoubleField(pXImage->GetDoubleField());
			pXImageNew->SetEvenField(pXImage->GetDoubleField());
			pXImageNew->SetOddField(pXImage->GetDoubleField());
			pXImageNew->SetTimestamp(pXImage->GetTimestamp());
			pXImageNew->SetTimestampOffset(pXImage->GetTimestampOffset());
			pXImageNew->SetFrameRate(pXImage->GetFrameRate());
			pXImageNew->SetHardware(pXImage->GetHardware());
			pXImageNew->SetVideoSignal(pXImage->GetVideoSignal());

			OutputData(pXImageNew);

			pXImageNew->Release();
			pXImageNew=NULL;
		}
	}
	else
	{
		OutputData(pXImage);
	}
}

void XPreProcess::EnableI420(bool bEnable)
{
	m_bI420Enable=bEnable;
}

bool XPreProcess::IsI420Enable(void)
{
	return m_bI420Enable;
}

void XPreProcess::EnableInvert(bool bEnable)
{
	m_bInvertEnable=bEnable;
}

bool XPreProcess::IsInvertEnable(void)
{
	return m_bInvertEnable;
}

void XPreProcess::EnableDenoise(bool bEnable)
{
	m_bDenoiseEnable=bEnable;
}

bool XPreProcess::IsDenoiseEnable(void)
{
	return m_bDenoiseEnable;
}

void XPreProcess::EnableInterlaced(bool bEnable)
{
	m_bInterlacedEnable=bEnable;
}

bool XPreProcess::IsInterlacedEnable(void)
{
	return m_bInterlacedEnable;
}

void XPreProcess::EnableDeinterlaced(bool bEnable,bool bDoubleFrameRate)
{
	m_bDeinterlacedEnable=bEnable;
	m_bDoubleFrameRate=bDoubleFrameRate;
}

bool XPreProcess::IsDeinterlacedEnable(void)
{
	return m_bDeinterlacedEnable;
}

//设置遮掩并启动遮掩
bool XPreProcess::SetupMask(RECT *RectList, int iRectCount)
{
	{
		XAutoLock l(m_csRectList);
		if (m_pRectList!=NULL)
		{
			FREEMSG(m_pRectList);
			m_pRectList=NULL;
			m_iRectCount=0;
		}
		m_iRectCount=iRectCount;
		if (RectList==NULL ||
			iRectCount<=0)
		{
			return false;
		}

		m_pRectList=(RECT*)MALLOCMSG(m_iRectCount*sizeof(RECT));
		if (m_pRectList==NULL)
		{
			return false;
		}
		fastmemcpy(m_pRectList,RectList,m_iRectCount*sizeof(RECT));
	}

	return true;
}

//停止遮掩
bool XPreProcess::StopMask(void)
{
	{
		XAutoLock l(m_csRectList);

		if (m_pRectList!=NULL)
		{
			FREEMSG(m_pRectList);
			m_pRectList=NULL;
			m_iRectCount=0;
		}
	}
	return true;
}


void XPreProcess::OutputData(XImage*pXImage)
{
	int nLen=pXImage->GetDataLen();
	unsigned char*pData=(unsigned char*)pXImage->GetData();
	if (m_bDeinterlacedEnable && m_bDoubleFrameRate && !pXImage->GetDoubleField() && pXImage->GetVirtualHeight()>288)
	{
		if (m_pFrameBuf==NULL || m_nBufLen<nLen)
		{
			if (m_pFrameBuf!=NULL)
			{
				FREEMSG(m_pFrameBuf);
				m_pFrameBuf=NULL;
			}
			m_nBufLen=nLen;
			m_pFrameBuf=(unsigned char*)MALLOCMSG(m_nBufLen);
		}

		if (m_pFrameBuf!=NULL)
		{
			int nActualWidth=pXImage->GetActualWidth();
			int nActualHeight=pXImage->GetActualHeight();
			int nHalfEncWidth=(nActualWidth>>1);
			int nHalfEncHeight=(nActualHeight>>1);

			unsigned char*pSrcY=pData;
			unsigned char*pSrcU=pSrcY+nActualWidth*nActualHeight;
			unsigned char*pSrcV=pSrcU+nActualWidth*nActualHeight/4;

			unsigned char*pDstEvenY=m_pFrameBuf;
			unsigned char*pDstEvenU=pDstEvenY+nActualWidth*nActualHeight;
			unsigned char*pDstEvenV=pDstEvenU+nActualWidth*nActualHeight/4;

			unsigned char*pDstOddY=pDstEvenY+nActualWidth*nHalfEncHeight;
			unsigned char*pDstOddU=pDstEvenU+nHalfEncWidth*nHalfEncHeight/2;
			unsigned char*pDstOddV=pDstEvenV+nHalfEncWidth*nHalfEncHeight/2;


			for (int i=0;i<(nActualHeight>>2);i++)
			{
				fastmemcpy(pDstEvenY,pSrcY,nActualWidth);
				pDstEvenY+=nActualWidth;
				pSrcY+=nActualWidth;

				fastmemcpy(pDstOddY,pSrcY,nActualWidth);
				pDstOddY+=nActualWidth;
				pSrcY+=nActualWidth;

				fastmemcpy(pDstEvenY,pSrcY,nActualWidth);
				pDstEvenY+=nActualWidth;
				pSrcY+=nActualWidth;

				fastmemcpy(pDstOddY,pSrcY,nActualWidth);
				pDstOddY+=nActualWidth;
				pSrcY+=nActualWidth;

				fastmemcpy(pDstEvenU,pSrcU,nHalfEncWidth);
				pDstEvenU+=nHalfEncWidth;
				pSrcU+=nHalfEncWidth;

				fastmemcpy(pDstOddU,pSrcU,nHalfEncWidth);
				pDstOddU+=nHalfEncWidth;
				pSrcU+=nHalfEncWidth;

				fastmemcpy(pDstEvenV,pSrcV,nHalfEncWidth);
				pDstEvenV+=nHalfEncWidth;
				pSrcV+=nHalfEncWidth;

				fastmemcpy(pDstOddV,pSrcV,nHalfEncWidth);
				pDstOddV+=nHalfEncWidth;
				pSrcV+=nHalfEncWidth;
			}


			NEWRECORD3( XImage*,pXImageNew,new XImage(m_pFrameBuf,nLen) )
			NULLReturn( pXImageNew )  
			pXImageNew->AddRef();
			m_pFrameBuf=NULL;

			pXImageNew->SetActualWidth(pXImage->GetActualWidth());
			pXImageNew->SetActualHeight(pXImage->GetActualHeight());
			pXImageNew->SetVirtualWidth(pXImage->GetVirtualWidth());
			pXImageNew->SetVirtualHeight(pXImage->GetVirtualHeight());

			bool bEvenFieldFirst=pXImage->GetEvenFieldFirst();
			if (m_bInvertEnable)
			{
				if (bEvenFieldFirst)
				{
					bEvenFieldFirst=false;
				}
				else
				{
					bEvenFieldFirst=true;
				}
			}

			pXImageNew->SetEvenFieldFirst(bEvenFieldFirst);
			pXImageNew->SetDoubleField(true);
			pXImageNew->SetEvenField(true);
			pXImageNew->SetOddField(true);
			pXImageNew->SetTimestamp(pXImage->GetTimestamp());
			pXImageNew->SetTimestampOffset(pXImage->GetTimestampOffset());
			pXImageNew->SetFrameRate(pXImage->GetFrameRate());
			pXImageNew->SetHardware(pXImage->GetHardware());
			pXImageNew->SetVideoSignal(pXImage->GetVideoSignal());

	
			if (m_bEnableOSD && m_strOSD.GetLength()>0)
			{
				m_OSD.ProcessYUV420((unsigned char*)pXImageNew->GetData(),pXImageNew->GetActualWidth(),pXImageNew->GetActualHeight());
			}

			m_rIXPreProcessNotify.OnIXPreProcessNotifyOutputData(pXImageNew);

			pXImageNew->Release();
			pXImageNew=NULL;

			return ;
		}
	}

	{
		unsigned char*pData=(unsigned char*)pXImage->GetData();
		int nLen=pXImage->GetDataLen();
		if (m_bInterlacedEnable && !pXImage->GetDoubleField())
		{
			bool bFielded=((pXImage->GetActualHeight()>>1)==pXImage->GetVirtualHeight());
			if (!bFielded)
			{
				Frame2Field(pData,pData,pXImage->GetActualWidth(),pXImage->GetActualHeight());
				pXImage->SetActualHeight(pXImage->GetActualHeight()>>1);
				pXImage->SetDataLen(nLen>>1);
			}
		}


		{
			XAutoLock l(m_csRectList);

			if (m_pRectList!=NULL)
			{
				for (int i=0;i<m_iRectCount;i++)
				{
					int x=m_pRectList[i].left*pXImage->GetActualWidth()/704;
					x+=(x%2);
					int y=m_pRectList[i].top*pXImage->GetActualHeight()/576;
					y+=(y%2);
					int cx=(m_pRectList[i].right-m_pRectList[i].left)*pXImage->GetActualWidth()/704;
					cx-=(cx%2);
					int cy=(m_pRectList[i].bottom-m_pRectList[i].top)*pXImage->GetActualHeight()/576;
					cy-=(cy%2);

					unsigned char*pTempY=pData;
					unsigned char*pTempU=pTempY+pXImage->GetActualWidth()*pXImage->GetActualHeight();
					unsigned char*pTempV=pTempU+((pXImage->GetActualWidth()*pXImage->GetActualHeight())>>2);

					int nHalfWidth=pXImage->GetActualWidth()>>1;
					int nHalfCX=cx>>1;

					pTempY+=y*pXImage->GetActualWidth()+x;
					pTempU+=(y>>1)*(nHalfWidth)+(x>>1);
					pTempV+=(y>>1)*(nHalfWidth)+(x>>1);
					for (int j=0;j<(cy>>1);j++)
					{
						memset(pTempY,0,cx);
						pTempY+=pXImage->GetActualWidth();
						memset(pTempY,0,cx);
						pTempY+=pXImage->GetActualWidth();

						memset(pTempU,127,nHalfCX);
						pTempU+=nHalfWidth;
						memset(pTempV,127,nHalfCX);
						pTempV+=nHalfWidth;
					}
				}
			}
		}
		
		if (m_bEnableOSD && m_strOSD.GetLength()>0)
		{
			m_OSD.ProcessYUV420((unsigned char*)pXImage->GetData(),pXImage->GetActualWidth(),pXImage->GetActualHeight());
		}

		m_rIXPreProcessNotify.OnIXPreProcessNotifyOutputData(pXImage);
	}
}

void XPreProcess::SetPictProp(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	m_nContrast=nContrast*2;
	m_nBrightness=(nBrightness-128)*2;
	m_nHue=nHue;
	m_nSaturation=nSaturation;
	m_nGamma=nGamma;
	m_nSharpen=nSharpen;
	m_bResetPictProp=true;
	m_bDoPictProp=true;

	if (m_nSharpen)
	{
		if (nSharpen>10)
			m_nContrastEnhanceAlpha=10;
		else if (nSharpen<1)
			m_nContrastEnhanceAlpha=1;
		else
			m_nContrastEnhanceAlpha=nSharpen;

		m_nSharpen=0;
		m_bEnableImageEnhance=true;
	}
	else
	{
		m_nContrastEnhanceAlpha=0;
		m_bEnableImageEnhance=false;
	}

	//char szDebug[1024];
	//sprintf(szDebug,"%u nSharpen=%d m_nContrastEnhanceAlpha=%d m_bEnableImageEnhance=%d\n",(unsigned long)this,nSharpen,m_nContrastEnhanceAlpha,m_bEnableImageEnhance);
	//DebugOutput(szDebug);
}

void XPreProcess::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	m_nGammaR=nGammaR;
	m_nGammaG=nGammaG;
	m_nGammaB=nGammaB;
	m_bResetPictProp=true;
	m_bDoPictProp=true;
}

void XPreProcess::EnableOSD(bool bEnable)
{
	m_bEnableOSD=bEnable;
}

bool XPreProcess::IsOSDEnable(void)
{
	return m_bEnableOSD;
}

void XPreProcess::SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft)
{
	m_strOSD=cszText;

	m_strOSD.TrimRight();
	m_strOSD.TrimRight(_T("\r\n"));
	m_nAlign=nAlign;
	m_nTop=nTop;
	m_nLeft=nLeft;
	m_OSD.SetTitle(m_strOSD,m_crText,m_nAlign,m_nTop,m_nLeft);
}

void XPreProcess::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	m_fontOSD=fontOSD;
	m_crText=crText;
	m_crBk=crBk;

	m_OSD.SetTitle(m_strOSD,m_crText,m_nAlign,m_nTop,m_nLeft);
}