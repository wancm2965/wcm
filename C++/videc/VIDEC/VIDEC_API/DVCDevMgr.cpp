#include "VIDEC_DVCDev.h"
#include "DVCDevMgr.h"
#include "DVCDev.h"
#include "XAutoLock.h"
#include "AVCONProduct.h"

#include <map>


#ifdef SUPPORT_DVC

#define DEFAULT_FRAME_RATE	30
#define DEFAULT_WIDTH		0
#define DEFAULT_HEIGHT		0

typedef struct DVCDevInst
{
	unsigned int	nDevIndex;
	int				nFrameRate;
	int				nBitrateControlType;
	int				nBitrate;
	int				nResolusion;

	DVCDev*		pDVCDev;
}DVCDevInst;

typedef std::map<unsigned int,DVCDevInst>	MapDVCDevInst;

static XCritSec			g_csMapDVCDevInst;
static MapDVCDevInst		g_MapDVCDevInst;
int						g_nDVCDevMgrRefCount=0;



typedef struct tagDVC_DEV_FORMAT
{
	int		nWidth;
	int		nHeight;
}DVC_DEV_FORMAT;

static DVC_DEV_FORMAT g_XDVCDevFormat[]={
	{176,144},
	{320,240},
	{352,288},
	{640,480},
	{704,576},
	{960,720},
	{1280,720},
	{1600,1200},
	{1920,1080},
};

int DVCDevMgr::GetFormatCount(void)
{
	return sizeof(g_XDVCDevFormat)/sizeof(DVC_DEV_FORMAT);
}

int DVCDevMgr::GetFormatName(int nIndex,char*szName,int nMaxCount)
{
	if (nIndex<0)
		return -1;

	if (nIndex>=DVCDevMgr::GetFormatCount())
	{
		return -1;
	}

	sprintf(szName,"%u X %u",g_XDVCDevFormat[nIndex].nWidth,g_XDVCDevFormat[nIndex].nHeight);

	return 0;
}

int DVCDevMgr::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	nWidth=0;
	nHeight=0;

	if (nIndex<0)
		return -1;

	int nCount=DVCDevMgr::GetFormatCount();
	if (nIndex>=nCount)
	{
		return -1;
	}

	nWidth=g_XDVCDevFormat[nIndex].nWidth;
	nHeight=g_XDVCDevFormat[nIndex].nHeight;

	return 0;
}

bool DVCDevMgr::IsValidFormatSize(int&nWidth,int&nHeight)
{
	int nCount=DVCDevMgr::GetFormatCount();
	for (int i=0;i<nCount;i++)
	{
		if (nWidth==g_XDVCDevFormat[i].nWidth &&
			nHeight==g_XDVCDevFormat[i].nHeight)
		{
			return true;
		}
	}

	nWidth=0;
	nHeight=0;
	return false;
}


int DVCDevMgr::Init(void)
{
	XAutoLock l(g_csMapDVCDevInst);

	if (g_nDVCDevMgrRefCount<=0)
	{
		g_MapDVCDevInst.clear();
		VIDEC_DVC::Init();
	}
	g_nDVCDevMgrRefCount++;

	return 0;
}

void DVCDevMgr::Terminate(void)
{
	XAutoLock l(g_csMapDVCDevInst);
	
	g_nDVCDevMgrRefCount--;
	if (g_nDVCDevMgrRefCount<=0)
	{
		g_MapDVCDevInst.clear();
		VIDEC_DVC::Terminate();
	}
}

int DVCDevMgr::GetDevCount(void)
{
	int nDevCount=0;
	nDevCount+=VIDEC_DVC::GetDevCount();
	return nDevCount;
}

int DVCDevMgr::SetDevCap(unsigned int nDevIndex,int nFrameRate,int nBitrate,int nBitrateControlType,int nResolusion)
{
	XAutoLock l(g_csMapDVCDevInst);


	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		if (iter->second.nBitrate!=nBitrate ||
			iter->second.nBitrateControlType!=nBitrateControlType ||
			iter->second.nResolusion!=nResolusion ||
			iter->second.nFrameRate!=nFrameRate)
		{
			iter->second.nBitrate=nBitrate;
			iter->second.nBitrateControlType=nBitrateControlType;
			iter->second.nResolusion=nResolusion;
			iter->second.nFrameRate=nFrameRate;

			if (iter->second.pDVCDev!=NULL)
			{
				iter->second.pDVCDev->Reset(nFrameRate,nBitrate,nBitrateControlType,nResolusion);
			}
		}

	}
	else
	{
		DVCDevInst tDVCDevInst;
		tDVCDevInst.nDevIndex=nDevIndex;
		tDVCDevInst.nFrameRate=nFrameRate;
		tDVCDevInst.nBitrate=nBitrate;
		tDVCDevInst.nBitrateControlType=nBitrateControlType;
		tDVCDevInst.nResolusion=nResolusion;
		tDVCDevInst.pDVCDev=NULL;
		g_MapDVCDevInst[nDevIndex]=tDVCDevInst;
	}

	return 0;
}
 
 
int DVCDevMgr::SetVideoParam(unsigned int nDevIndex,int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;

		if (pDVCDev!=NULL)
		{
			return pDVCDev->SetVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
		}
	}

	return -1;
}

int DVCDevMgr::SetVideoGammaRGB(unsigned int nDevIndex,int nGammaR,int nGammaG,int nGammaB)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;

		if (pDVCDev!=NULL)
		{
			return pDVCDev->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
		}
	}

	return -1;
}

int DVCDevMgr::AddStreamSink(unsigned int nDevIndex,VIDEC_StreamSink*pStreamSink)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		if (iter->second.pDVCDev==NULL)
		{
			unsigned int nDevCount=0;
			nDevCount=VIDEC_DVC::GetDevCount();
			if (nDevIndex<nDevCount)
			{
				NEWRECORD( pDVCDev,new DVCDev(nDevIndex) )
NULLReturnRet( pDVCDev , -1 )  
				if (0!=pDVCDev->Open(iter->second.nFrameRate,iter->second.nBitrate,iter->second.nBitrateControlType,iter->second.nResolusion))
				{
					pDVCDev->Close();
					delete pDVCDev;
					pDVCDev=NULL;
					return -1;
				}

				iter->second.pDVCDev=pDVCDev;
				pDVCDev=pDVCDev;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			pDVCDev=iter->second.pDVCDev;
		}
	}
	else
	{
		unsigned int nDevCount=0;
		nDevCount=VIDEC_DVC::GetDevCount();
		if (nDevIndex<nDevCount)
		{
			NEWRECORD( pDVCDev,new DVCDev(nDevIndex) )
			NULLReturnRet( pDVCDev , -1 )  
			DVCDevInst tDVCDevInst;
			tDVCDevInst.nDevIndex=nDevIndex;
			tDVCDevInst.nFrameRate=DEFAULT_FRAME_RATE;
			tDVCDevInst.nBitrate=1500;
			tDVCDevInst.nBitrateControlType=0;
			tDVCDevInst.nResolusion=6;

			if (0!=pDVCDev->Open(tDVCDevInst.nFrameRate,tDVCDevInst.nBitrate,tDVCDevInst.nBitrateControlType,tDVCDevInst.nResolusion))
			{
				pDVCDev->Close();
				delete pDVCDev;
				pDVCDev=NULL;
				return -1;
			}

			tDVCDevInst.pDVCDev=pDVCDev;
			g_MapDVCDevInst[nDevIndex]=tDVCDevInst;
		}
		else
		{
			return -1;
		}
	}

	pDVCDev->AddStreamSink(pStreamSink);

	return 0;
}

void DVCDevMgr::RemoveStreamSink(unsigned int nDevIndex,VIDEC_StreamSink*pStreamSink)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;

		if (pDVCDev!=NULL)
		{
			pDVCDev->RemoveStreamSink(pStreamSink);
			if (pDVCDev->CanBeClosed())
			{
				iter->second.pDVCDev=NULL;

				pDVCDev->Close();
				delete pDVCDev;
				pDVCDev=NULL;
			}
		}
	}
}


int DVCDevMgr::RequestKeyFrame(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;

		if (pDVCDev!=NULL)
		{
			return pDVCDev->RequestKeyFrame();
		}
	}

	return -1;
}

int DVCDevMgr::SendData(unsigned int nDevIndex,const char*pData,int nLen)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;

		if (pDVCDev!=NULL)
		{
			return pDVCDev->SendData(pData,nLen);
		}
	}

	return -1;
}


int DVCDevMgr::GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount)
{
	unsigned int nDevCount=0;
	nDevCount=VIDEC_DVC::GetDevCount();
	if (nDevIndex<nDevCount)
	{
		return VIDEC_DVC::GetDevName(nDevIndex,szName,nMaxCount);
	}

	return -1;
}


int DVCDevMgr::GetDevType(unsigned int nDevIndex,VIDEC_DEV_TYPE&nDevType)
{
	unsigned int nDevCount=0;
	nDevCount=VIDEC_DVC::GetDevCount();
	if (nDevIndex<nDevCount)
	{
		return VIDEC_DVC::GetDevType(nDevIndex,nDevType);
	}

	return -1;
}

bool DVCDevMgr::HasAudioInput(unsigned int nDevIndex)
{
	unsigned int nDevCount=0;
	nDevCount=VIDEC_DVC::GetDevCount();
	if (nDevIndex<nDevCount)
	{
		return VIDEC_DVC::HasAudioInput(nDevIndex);
	}
	return false;
}

bool DVCDevMgr::SupportHardwareEncoding(unsigned int nDevIndex)
{
	unsigned int nDevCount=0;
	nDevCount=VIDEC_DVC::GetDevCount();
	if (nDevIndex<nDevCount)
	{
		return VIDEC_DVC::SupportHardwareEncoding(nDevIndex);
	}
	return false;
}

int DVCDevMgr::AddImageSink(unsigned int nDevIndex,VIDEC_ImageSink*pImageSink)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		if (iter->second.pDVCDev==NULL)
		{
			unsigned int nDevCount=0;
			nDevCount=VIDEC_DVC::GetDevCount();
			if (nDevIndex<nDevCount)
			{
				NEWRECORD( pDVCDev,new DVCDev(nDevIndex) )
NULLReturnRet( pDVCDev , -1 )  
				if (0!=pDVCDev->Open(iter->second.nFrameRate,iter->second.nBitrate,iter->second.nBitrateControlType,iter->second.nResolusion))
				{
					pDVCDev->Close();
					delete pDVCDev;
					pDVCDev=NULL;
					return -1;
				}

				iter->second.pDVCDev=pDVCDev;
				pDVCDev=pDVCDev;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			pDVCDev=iter->second.pDVCDev;
		}
	}
	else
	{
		unsigned int nDevCount=0;
		nDevCount=VIDEC_DVC::GetDevCount();
		if (nDevIndex<nDevCount)
		{
			NEWRECORD( pDVCDev ,new DVCDev(nDevIndex) )
			NULLReturnRet( pDVCDev , -1 )  
			DVCDevInst tDVCDevInst;
			tDVCDevInst.nDevIndex=nDevIndex;
			tDVCDevInst.nFrameRate=DEFAULT_FRAME_RATE;
			tDVCDevInst.nBitrate=1500;
			tDVCDevInst.nBitrateControlType=0;
			tDVCDevInst.nResolusion=6;

			if (0!=pDVCDev->Open(tDVCDevInst.nFrameRate,tDVCDevInst.nBitrate,tDVCDevInst.nBitrateControlType,tDVCDevInst.nResolusion))
			{
				pDVCDev->Close();
				delete pDVCDev;
				pDVCDev=NULL;
				return -1;
			}

			tDVCDevInst.pDVCDev=pDVCDev;
			g_MapDVCDevInst[nDevIndex]=tDVCDevInst;
		}
		else
		{
			return -1;
		}
	}

	pDVCDev->AddImageSink(pImageSink);

	return 0;
}

void DVCDevMgr::RemoveImageSink(unsigned int nDevIndex,VIDEC_ImageSink*pImageSink)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;

		if (pDVCDev!=NULL)
		{
			pDVCDev->RemoveImageSink(pImageSink);
			if (pDVCDev->CanBeClosed())
			{
				iter->second.pDVCDev=NULL;

				pDVCDev->Close();
				delete pDVCDev;
				pDVCDev=NULL;
			}
		}
	}
}

void DVCDevMgr::SetInvert(unsigned int nDevIndex,int nInvert)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;
		pDVCDev->SetInvert(nInvert);
	}
}

int DVCDevMgr::GetInvert(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;
		return pDVCDev->GetInvert();
	}

	return -1;
}

void DVCDevMgr::SetDenoise(unsigned int nDevIndex,int nDenoise)
{

}

int DVCDevMgr::GetDenoise(unsigned int nDevIndex)
{
	return 0;
}

void DVCDevMgr::SetLevelFixer(unsigned int nDevIndex,int nLevelFixer)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;
		pDVCDev->SetLevelFixer(nLevelFixer);
	}
}

int DVCDevMgr::GetLevelFixer(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;
		return pDVCDev->GetLevelFixer();
	}

	return -1;
}

	
void DVCDevMgr::EnableOSD(unsigned int nDevIndex,bool bEnable)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;
		pDVCDev->EnableOSD(bEnable);
	}
}

bool DVCDevMgr::IsOSDEnable(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;
		return pDVCDev->IsOSDEnable();
	}

	return false;
}

void DVCDevMgr::SetOSDText(unsigned int nDevIndex,const char*cszText,int nAlign,int nTop,int nLeft)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;
		pDVCDev->SetOSDText(cszText,nAlign,nTop,nLeft);
	}
}

void DVCDevMgr::SetOSDFont(unsigned int nDevIndex,LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	XAutoLock l(g_csMapDVCDevInst);

	DVCDev*pDVCDev=NULL;

	MapDVCDevInst::iterator iter=g_MapDVCDevInst.find(nDevIndex);
	if (iter!=g_MapDVCDevInst.end())
	{
		pDVCDev=iter->second.pDVCDev;
		pDVCDev->SetOSDFont(fontOSD,crText,crBk);
	}
}


#endif