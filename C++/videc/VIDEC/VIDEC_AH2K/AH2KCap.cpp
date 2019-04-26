#include "AH2KCap.h"
#include "XUtil.h"
#include <emmintrin.h>
#include <VIDEC/VIDEC.h>
#include "../VIDEC_FRE/VIDECFREAPI.h"

#ifdef USE_AH2K_STATIC_MEMORY
#include "VCapMemMang.h"
#endif

#ifdef USE_AH2K_STATIC_MEMORY
VIDEC_MemMang** g_pMemManage = NULL;
#endif

VIDEC_AH2K*VIDEC_AH2K::Create(VIDEC_AH2KCallback&rCallback)
{
	//return new AH2KCap(rCallback);
	NEWRECORDret3(  VIDEC_AH2K* ,p , new AH2KCap(rCallback) ) 
}


void* AH2K_VideoCallback(const void*pUserData,unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp,unsigned long nHeaderLen,unsigned long nInputFrameType)
{
	AH2KCap*pAH2KCap=(AH2KCap*)pUserData;
	if (pAH2KCap)
	{
		if (nFrameRate<=0)
		{
			nFrameRate=25;
		}
		return pAH2KCap->OnVideoCallback(pFrameData,nFrameLen,nWidth,nHeight,nFrameRate,nTimestamp,nHeaderLen,nInputFrameType);
	}

	return pFrameData;
}

void AH2K_AudioCallback(const void*pUserData,short*pSamples,int nSamples,int nSampleRate,unsigned long nTimestamp)
{
	AH2KCap*pAH2KCap=(AH2KCap*)pUserData;
	if (pAH2KCap)
	{
		pAH2KCap->OnAudioCallback(pSamples,nSamples,nSampleRate,nTimestamp);
	}
}

VIDEC_API void* AH2K_Malloc(size_t size)
{
	return MALLOCMSG(size);
}

VIDEC_API void AH2K_Free(void*ptr)
{
	FREEMSG(ptr);
}



AH2KCap::AH2KCap(VIDEC_AH2KCallback&rCallback)
:m_rCallback(rCallback)
,m_pHandle(NULL)
,m_pXImage(NULL)
,m_pResize(NULL)
,m_nScaleType(0)
,m_nDevID(-1)
{
	m_nActualWidth=0;
	m_nActualHeight=0;
	m_nVirtualWidth=0;
	m_nVirtualHeight=0;

	m_nImageSize=0;

	m_pTempBuf=NULL;
	m_nBufLen=0;
	m_usDstWidth=0;
	m_usDstHeight=0;
	m_nFrameCount=0;
	m_nFrameRate=0;
	m_nLastTS=XGetTimestamp();

#ifdef USE_AH2K_STATIC_MEMORY
	if (g_pMemManage == NULL)
	{
		int nCount = AH2KGetDevCount();
		NEWRECORD( g_pMemManage , new VIDEC_MemMang* [nCount] );
		NULLReturn( g_pMemManage ) 

			for (int i = 0; i < nCount; i++)
			{
				g_pMemManage[i] = VIDEC_MemMang::Create();
				if (!g_pMemManage[i])
					DebugOutput( logcoreerror ,"AH2KCap malloc failed\n");
			}
	}
#endif
}

AH2KCap::~AH2KCap(void)
{

}

int AH2KCap::Open(int nDevID,int nWidth,int nHeight,int nFrameRate)
{
	DebugOutput(logcapture,"AH2KCap::Open\n");

	m_pHandle=AH2KOpen(nDevID,AH2K_VideoCallback,AH2K_AudioCallback,this);
	if (m_pHandle==NULL)
	{
		DebugOutput( logcoreexcep ,"AH2KOpen Failed return \n");
		return -1;
	}
	
	m_nDevID = nDevID;
	m_nFrameRate=nFrameRate;
	if (nFrameRate <= 1)
		nFrameRate = 1;
	else if (nFrameRate > 30)
	{
		nFrameRate = 100;
	}

	if ((nWidth==1920 && nHeight==1080) ||
		(nWidth==0 && nHeight==0))
	{
		m_nScaleType=0;
	}
	else if ((nWidth<=960 || nHeight<=540) && nWidth!=1920 && nHeight!=1080)
	{
		m_nScaleType=2;
	}
	//else
	//{
	//	m_nScaleType=1;
	//}

	bool b1080I=false;

	bool bIsHD=false;
	bIsHD=AH2KIsHD(nDevID);
	if (bIsHD)
	{
		m_pResize=VIDEC_Resize::Create();
		if (m_pResize->Open()!=0)
		{
			m_pResize->Close();
			delete m_pResize;
			m_pResize=NULL;
		}

		if (m_pResize!=NULL)
		{
			if (nWidth!=0)
			{
				if (nWidth==1920 && nHeight==540)
				{
					b1080I=true;
					nHeight=1080;
					m_usDstWidth=nWidth;
					m_usDstHeight=nHeight;
				}
				else
				{
					double dblRatio=(double)nHeight/(double)nWidth;
					m_usDstWidth=nWidth;
					m_usDstHeight=nHeight;

					nWidth=1080.00/dblRatio;
					nHeight=1080;

					if (nWidth%32)
					{
						nWidth+=(32-(nWidth%32));
					}
				}
			}
		}
	}

	m_nBufLen=1920*1080*3/2+1920*1080;
	if (b1080I)
	{
		m_nScaleType=1;
	}

	AH2KSetParams(m_pHandle,m_nScaleType,nWidth,nHeight,nFrameRate);

	return 0;
}

void AH2KCap::Close(void)
{
	DebugOutput(logcapture,"AH2KCap::Close\n");

	if (m_pHandle)
	{
		AH2KClose(m_pHandle);

		m_pHandle=NULL;
	}

	if (m_pXImage != NULL)
	{
		m_pXImage->Release();
		m_pXImage = NULL;
	}

	if (m_pResize!=NULL)
	{
		m_pResize->Close();
		delete m_pResize;
		m_pResize=NULL;
	}

	if (m_pTempBuf)
	{
		FREEMSG(m_pTempBuf);
		m_pTempBuf = NULL;
	}
}

void* AH2KCap::OnVideoCallback(unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp,unsigned long nHeaderLen,unsigned long nInputFrameType)
{
	if (FRE_GetDebugLevel() > 0)
	{
		unsigned long nCurTS=XGetTimestamp();
		m_nFrameCount++;
		if (nCurTS-m_nLastTS>=60000)
		{
			char szDebug[1024];
			sprintf(szDebug,"AH2KCap::OnVideoCallback nFrameRate=%u nFrameCount=%u ActualWidth=%u ActualHeight=%u this=%u\n",nFrameRate,m_nFrameCount,nWidth,nHeight,this);
			DebugOutput( logthread ,szDebug);
			m_nFrameCount=0;
			m_nLastTS=nCurTS;
		}
	}

	int nSend = 0;
	m_rCallback.OnVIDEC_AH2KCallbackPreSendImage(nSend);
	if (nSend==0)
	{
		return pFrameData;
	}

	//nInputFrameType=INPUT_FRAME_TYPE_MERGED;
	//char szDebug[1024];
	//sprintf(szDebug,"OnVideoCallback nInputFrameType=%u\n",nInputFrameType);
	//DebugOutput(szDebug);

#ifdef USE_AH2K_STATIC_MEMORY
	unsigned char* pMem = NULL;
	if (m_nDevID >= 0 && g_pMemManage[m_nDevID])
	{
		pMem = g_pMemManage[m_nDevID]->GetMemory();				//该块处理是为了获得没有使用的内存，最好的做法是丢掉前面的帧，但由于该帧实际是在XVideoDisplay里的线程里，
																//处理起来改动的地方较多，因此暂时只能直接丢，最优的方法是丢XVideoDisplay里的m_listImage的成员，再丢AH2KDev里的
		if (!pMem)												//m_listImage的成员
		{
			//OutputDebugString("drop one cap frame");
			return pFrameData;
		}

		g_pMemManage[m_nDevID]->SetMemNotUse(pMem);
	}

	if (pMem)
	{
		memcpy(pMem+1, pFrameData, nFrameLen);
		nHeaderLen = 0;
		pFrameData = pMem+1;
	}
#endif

	if (nInputFrameType & INPUT_FRAME_TYPE_MERGED)
	{
		if (nFrameRate<=30 && nFrameRate>=25)
		{
			nFrameRate=(nFrameRate<<1);
		}
		int nWidth2=(nWidth<<1);
		int nHalfHeight=(nHeight>>1);
		int nHalfWidth=(nWidth>>1);
		int nYLen=nWidth*nHeight;
		int nHalfYLen=nWidth*nHalfHeight;
		int nUVLen=(nYLen>>2);
		int nHalfUVLen=(nHalfYLen>>2);
		nFrameLen=(nFrameLen>>1);
		nHeight=nHalfHeight;

		unsigned char*pFieldFrameData=(unsigned char*)MALLOCMSG(nWidth*nHalfHeight*3);
		if(pFieldFrameData ==NULL )
		{
			DebugOutput( logcoreexcep ,"Malloc Failed. OnVideoCallback. \n");
			return pFrameData;
		}
		unsigned char*pDstY=pFieldFrameData;
		unsigned char*pDstU=pFieldFrameData+nHalfYLen;
		unsigned char*pDstV=pDstU+nHalfUVLen;

		unsigned char*pSrcY=pFrameData;
		unsigned char*pSrcU=pFrameData+nYLen;
		unsigned char*pSrcV=pSrcU+nUVLen;

		for (int i=0;i<nHeight;++i)
		{
			memcpy(pDstY,pSrcY,nWidth);
			pDstY+=nWidth;
			pSrcY+=nWidth2;
			memcpy(pDstY,pSrcY,nWidth);
			pDstY+=nWidth;
			pSrcY+=nWidth2;

			memcpy(pDstU,pSrcU,nHalfWidth);
			pDstU+=nHalfWidth;
			pSrcU+=nWidth;
			memcpy(pDstV,pSrcV,nHalfWidth);
			pDstV+=nHalfWidth;
			pSrcV+=nWidth;
		}

		pFieldFrameData=(unsigned char*)OnVideoFrameData(pFieldFrameData,nFrameLen,nWidth,nHeight,nFrameRate,nTimestamp,0,nInputFrameType);

		if (m_nFrameRate > 30)
		{
			if (pFieldFrameData==NULL)
			{
				pFieldFrameData=(unsigned char*)MALLOCMSG(nWidth*nHalfHeight*3);		
				if(pFieldFrameData ==NULL )
				{
					DebugOutput( logcoreexcep ,"Malloc Failed 2. OnVideoCallback. \n");
					return pFrameData;
				}
			}

			pSrcY=pFrameData;
			pSrcU=pFrameData+nYLen;
			pSrcV=pSrcU+nUVLen;

			pDstY=pFieldFrameData;
			pDstU=pFieldFrameData+nHalfYLen;
			pDstV=pDstU+nHalfUVLen;

			pSrcY+=nWidth;
			pSrcU+=nHalfWidth;
			pSrcV+=nHalfWidth;

			for (int i=0;i<nHeight;++i)
			{
				memcpy(pDstY,pSrcY,nWidth);
				pDstY+=nWidth;
				pSrcY+=nWidth2;
				memcpy(pDstY,pSrcY,nWidth);
				pDstY+=nWidth;
				pSrcY+=nWidth2;

				memcpy(pDstU,pSrcU,nHalfWidth);
				pDstU+=nHalfWidth;
				pSrcU+=nWidth;

				memcpy(pDstV,pSrcV,nHalfWidth);
				pDstV+=nHalfWidth;
				pSrcV+=nWidth;
			}
			pFieldFrameData=(unsigned char*)OnVideoFrameData(pFieldFrameData,nFrameLen,nWidth,nHeight,nFrameRate,nTimestamp,0,nInputFrameType);
		}
	
		if (pFieldFrameData)
		{
			FREEMSG(pFieldFrameData);
			pFieldFrameData=NULL;
		}
		return pFrameData;
	}


	return OnVideoFrameData(pFrameData,nFrameLen,nWidth,nHeight,nFrameRate,nTimestamp,nHeaderLen,nInputFrameType);
}

void*AH2KCap::OnVideoFrameData(unsigned char*pFrameData,int nFrameLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp,unsigned long nHeaderLen,unsigned long nInputFrameType)
{
	unsigned int nActualWidth=m_usDstWidth;
	unsigned int nActualHeight=m_usDstHeight;
	unsigned int nVirtualWidth=m_usDstWidth;
	unsigned int nVirtualHeight=m_usDstHeight;

	bool bAlreadyFieldHeight = (nWidth > (nHeight << 1));
	bool bAlreadyFieldWidth = (nWidth < nHeight);

	int nWidth_X_Height=nWidth * nHeight;

	if (nInputFrameType & INPUT_FRAME_TYPE_422)
	{
		int nHeightLoop=(nHeight>>1);
		int nWidthLoop=(nWidth>>4);
		unsigned char*pSrcUV=pFrameData+nWidth_X_Height;
		unsigned char*pDstUV=pSrcUV;

		for( int i = 0; i < nHeightLoop; i++ )
		{
			for( int j = 0; j < nWidthLoop; j++ )
			{
				*( ( __m128i * ) pDstUV ) = _mm_avg_epu8( *( ( __m128i * ) pSrcUV ), *( ( __m128i * )  ( pSrcUV + nWidth ) ) );
				pSrcUV += 16;
				pDstUV += 16;
			}
			pSrcUV += nWidth;
		}
	}
	else if (nInputFrameType & INPUT_FRAME_TYPE_I)
	{

	}


	unsigned long ulTimestamp = XGetTimestamp();

	PBYTE pBufY = pFrameData;
	PBYTE pBufU = pBufY + nWidth_X_Height;
	PBYTE pBufV = pBufU + nWidth_X_Height / 4;

	unsigned char *pCapYUV = pBufY;
	bool bResize=false;

	if (m_usDstWidth>=nWidth && m_usDstHeight>=nHeight)
	{
		bResize=false;
	}
	else
	{
		bResize=true;
	}


	if (bResize && (((m_usDstWidth!=0 && m_usDstWidth!=(nWidth*(bAlreadyFieldWidth?2:1))) ||
		(m_usDstHeight!=0 && m_usDstHeight!=(nHeight*(bAlreadyFieldHeight?2:1)))
		)))
	{
		int nTempDstHeight = (bAlreadyFieldHeight?(m_usDstHeight>>1):m_usDstHeight);
		int nTempDstWidth  = (bAlreadyFieldWidth?(m_usDstWidth>>1):m_usDstWidth);

		double dblRatioDst=(double)m_usDstWidth/(double)nTempDstHeight;
		double dblRatioOrg=(double)nWidth/(double)nHeight;
		double dblDelta=dblRatioDst-dblRatioOrg;
		{
			if (m_pTempBuf==NULL)
			{
				m_pTempBuf=(PBYTE)MALLOCMSG(m_nBufLen);
				if (m_pTempBuf==NULL)
					return pFrameData;
			}

			if (nHeight==540 && bAlreadyFieldHeight)
			{
				nTempDstHeight<<=1;
			}

			//if (nTempDstHeight>nHeight || (nHeight==1080 && nTempDstHeight>=720))
			//{
			//	nActualHeight=nHeight;
			//	nTempDstHeight=nHeight;
			//}
			//else
			{
				nActualHeight=nTempDstHeight;
			}

			nActualWidth=nTempDstWidth;

			if (nWidth!=nTempDstWidth || nHeight!=nTempDstHeight)
			{
				unsigned int nSrcYBufferSize = nWidth * nHeight;
				unsigned int nSrcUVBufferSize = nSrcYBufferSize >> 2;

				unsigned char*src[3];
				src[0]= pCapYUV;
				src[1] = src[0] + nSrcYBufferSize;
				src[2] = src[1] + nSrcUVBufferSize;

				unsigned int nDstYBufferSize = nTempDstWidth*nTempDstHeight;
				unsigned int nDstUVBufferSize = nDstYBufferSize >> 2;

				unsigned char*dst[3];
				dst[0] = m_pTempBuf;
				dst[1] = dst[0] + nDstYBufferSize;
				dst[2] = dst[1] + nDstUVBufferSize;

				if (0==m_pResize->Resize(src,nWidth,nHeight,dst,nTempDstWidth,nTempDstHeight))
				{
					nWidth=nTempDstWidth;
					nHeight=nTempDstHeight;
					pCapYUV=m_pTempBuf;
				}
			}
		}
	}
	else
	{
		if (bAlreadyFieldHeight)
		{
			nActualWidth=nWidth;
			nActualHeight=nHeight;
			nVirtualWidth=nWidth;
			nVirtualHeight=nHeight<<1;
		}
		else if (bAlreadyFieldWidth)
		{
			nActualWidth=nWidth;
			nActualHeight=nHeight;

			if ((nWidth==480 && nHeight==540))
			{
				nVirtualWidth=m_usDstWidth;
				nVirtualHeight=m_usDstHeight;
			}
			else
			{
				if ((nWidth==960 && nHeight==1080) ||
					(nWidth==640 && nHeight==720))
				{
					nVirtualWidth=nWidth;
				}
				else
				{
					nVirtualWidth=nWidth<<1;
				}
				nVirtualHeight=nHeight;
			}
		}
		else
		{
			nActualWidth=nWidth;
			nActualHeight=nHeight;
			//nVirtualWidth=nWidth;
			//nVirtualHeight=nHeight;
		}
	}

	if (m_nActualWidth!=nActualWidth ||
		m_nActualHeight!=nActualHeight)
	{
		m_nActualWidth=nActualWidth;
		m_nActualHeight=nActualHeight;
		m_nImageSize=m_nActualWidth*m_nActualHeight*3/2;
	}

	if (m_nVirtualWidth!=nVirtualWidth ||
		m_nVirtualHeight!=nVirtualHeight)
	{
		m_nVirtualWidth=nVirtualWidth;
		m_nVirtualHeight=nVirtualHeight;
	}

	if (m_pXImage!=NULL)
	{
		m_pXImage->Release();
		m_pXImage=NULL;
	}

	if (pCapYUV==m_pTempBuf)
	{
		nHeaderLen=0;
		m_pTempBuf=NULL;
	}
	else 
	{
		pFrameData=NULL;
	}

XImage*pXImage;

#ifdef USE_AH2K_STATIC_MEMORY
	NEWRECORD( pXImage , new XImage(pCapYUV, m_nImageSize, nHeaderLen, (int)1) );
#else
	NEWRECORD( pXImage ,new XImage(pCapYUV,m_nImageSize,nHeaderLen) );
#endif

NULLReturnRet( pXImage , pFrameData ) 

	pXImage->AddRef();

	pXImage->SetActualWidth(m_nActualWidth);
	pXImage->SetActualHeight(m_nActualHeight);
	pXImage->SetVirtualWidth(m_nVirtualWidth);
	pXImage->SetVirtualHeight(m_nVirtualHeight);
	pXImage->SetDoubleField(false);
	pXImage->SetEvenField(false);
	pXImage->SetOddField(false);
	pXImage->SetTimestamp(ulTimestamp);
	pXImage->SetHardware(true);

	pXImage->SetVideoSignal(AH2KHasVideoSignal(m_pHandle)!=0);

	//if (nFrameRate>=50 && (nInputFrameType & INPUT_FRAME_TYPE_MERGED))
	//{
	//	nFrameRate>>=1;
	//}

	pXImage->SetFrameRate(nFrameRate);

	m_rCallback.OnVIDEC_AH2KCallbackImage(pXImage);

	m_pXImage=pXImage;

	return pFrameData;
}


void AH2KCap::OnAudioCallback(short*pSamples,int nSamples,int nSampleRate,unsigned long nTimestamp)
{
	m_rCallback.OnVIDEC_AH2KCallbackAudioData(pSamples,nSamples,nSampleRate);
}
 

int VIDEC_AH2K::Init(void)
{
	AH2K_init();

	AH2KSetMemory(AH2K_Malloc,AH2K_Free);

	return 0;
}

void VIDEC_AH2K::Terminate(void)
{
	AH2K_terminate();
}

int VIDEC_AH2K::GetDevCount(void)
{
	return AH2KGetDevCount();
}

int VIDEC_AH2K::GetDevName(int nDevID,char*szName,int nMaxCount)
{
	return AH2KGetDevName(nDevID,szName,nMaxCount);
}

int VIDEC_AH2K::GetDevType(int nDevID,VIDEC_DEV_TYPE&nDevType)
{
	if (AH2KIsHD(nDevID))
	{
		nDevType=VIDEC_DEV_HD;
	}
	else
	{
		nDevType=VIDEC_DEV_SD;
	}

	return 0;
}

bool VIDEC_AH2K::HasAudioInput(int nDevID)
{
	return (AH2KHasAudio(nDevID)!=0);
}

bool VIDEC_AH2K::SupportHardwareEncoding(int nDevID)
{
	return false;
}
