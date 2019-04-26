#include "MPVIDEO_AH2KDev.h"
#include "AH2KDevMgr.h"
#include "AH2KDev.h"
#include "XAutoLock.h"
#include "AVCONProduct.h"

#include <map>


#ifdef SUPPORT_AH2K

#define DEFAULT_FRAME_RATE	15
#define DEFAULT_WIDTH		0
#define DEFAULT_HEIGHT		0

typedef struct AH2KDevInst
{
	unsigned int	nDevIndex;
	int				nWidth;
	int				nHeight;
	int				nFrameRate;
	bool			bDenoise;

	AH2KDev*		pAH2KDev;
}AH2KDevInst;

typedef std::map<unsigned int,AH2KDevInst>	MapAH2KDevInst;

static XCritSec			g_csMapAH2KDevInst;
static MapAH2KDevInst		g_MapAH2KDevInst;
int						g_nAH2KDevMgrRefCount=0;

extern DWORD GetUsedByProductType(void);


typedef struct tagAH2K_DEV_FORMAT
{
	int		nWidth;
	int		nHeight;
}AH2K_DEV_FORMAT;

static AH2K_DEV_FORMAT g_AH2KDevFormat[]={
	{0,0},
	{176,144},
	{320,180},
	{320,240},
	{352,288},
	{640,360},
	{640,480},
	{640,720},
	{704,576},
	{800,600},
	{960,540},
	{960,1080},
	{1024,768},
	{1280,720},
	{1280,768},
	{1280,960},
	{1280,1024},
	{1360,768},
	{1440,1080},
	{1920,540},
	{1920,1080},
};

static AH2K_DEV_FORMAT g_AH2KDevFormatRS[]={
	{0,0},
	{176,144},
	{320,180},
	{320,240},
	{352,288},
	{640,360},
	{640,480},
	{640,720},
	{704,576},
	{800,600},
	{960,540},
	{1024,768},
	{1280,720},
};

int AH2KDevMgr::GetFormatCount(void)
{
	if (GetUsedByProductType()==AVCON_PRODUCT_RS ||
		GetUsedByProductType()==AVCON_PRODUCT_WJ ||
		//GetUsedByProductType()==AVCON_PRODUCT_NFH ||
		GetUsedByProductType()==AVCON_PRODUCT_PICC)
	{
		return sizeof(g_AH2KDevFormatRS)/sizeof(AH2K_DEV_FORMAT);
	}
	else
	{
		return sizeof(g_AH2KDevFormat)/sizeof(AH2K_DEV_FORMAT);
	}
}

int AH2KDevMgr::GetFormatName(int nIndex,char*szName,int nMaxCount)
{
	if (nIndex<0)
		return -1;

	if (nIndex>=AH2KDevMgr::GetFormatCount())
	{
		return -1;
	}

	if (nIndex==0)
	{
		strcpy(szName,"Auto");
		return 0;
	}

	if (GetUsedByProductType()==AVCON_PRODUCT_RS ||
		GetUsedByProductType()==AVCON_PRODUCT_WJ ||
		//GetUsedByProductType()==AVCON_PRODUCT_NFH ||
		GetUsedByProductType()==AVCON_PRODUCT_PICC )
	{
		if (g_AH2KDevFormatRS[nIndex].nWidth==1920 && g_AH2KDevFormatRS[nIndex].nHeight==540)
		{
			strcpy(szName,"1920 X 1080 I");
		}
		else if (g_AH2KDevFormatRS[nIndex].nWidth==1920 && g_AH2KDevFormatRS[nIndex].nHeight==1080)
		{
			strcpy(szName,"1920 X 1080 P");
		}
		else
		{
			sprintf(szName,"%u X %u",g_AH2KDevFormatRS[nIndex].nWidth,g_AH2KDevFormatRS[nIndex].nHeight);
		}
	}
	else
	{
		if (g_AH2KDevFormat[nIndex].nWidth==1920 && g_AH2KDevFormat[nIndex].nHeight==540)
		{
			strcpy(szName,"1920 X 1080 I");
		}
		else if (g_AH2KDevFormat[nIndex].nWidth==1920 && g_AH2KDevFormat[nIndex].nHeight==1080)
		{
			strcpy(szName,"1920 X 1080 P");
		}
		else
		{
			sprintf(szName,"%u X %u",g_AH2KDevFormat[nIndex].nWidth,g_AH2KDevFormat[nIndex].nHeight);
		}
	}

	return 0;
}

int AH2KDevMgr::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	nWidth=0;
	nHeight=0;

	if (nIndex<0)
		return -1;

	int nCount=AH2KDevMgr::GetFormatCount();
	if (nIndex>=nCount)
	{
		return -1;
	}

	if (GetUsedByProductType()==AVCON_PRODUCT_RS ||
		GetUsedByProductType()==AVCON_PRODUCT_WJ ||
		//GetUsedByProductType()==AVCON_PRODUCT_NFH ||
		GetUsedByProductType()==AVCON_PRODUCT_PICC)
	{
		if (nIndex==0)
		{
			nIndex=nCount-1;
		}
	
		nWidth=g_AH2KDevFormatRS[nIndex].nWidth;
		nHeight=g_AH2KDevFormatRS[nIndex].nHeight;
	}
	else
	{
		nWidth=g_AH2KDevFormat[nIndex].nWidth;
		nHeight=g_AH2KDevFormat[nIndex].nHeight;
	}

	return 0;
}

bool AH2KDevMgr::IsValidFormatSize(int&nWidth,int&nHeight)
{
	int nCount=AH2KDevMgr::GetFormatCount();
	for (int i=0;i<nCount;i++)
	{
		if (GetUsedByProductType()==AVCON_PRODUCT_RS ||
			GetUsedByProductType()==AVCON_PRODUCT_WJ ||
			//GetUsedByProductType()==AVCON_PRODUCT_NFH ||
			GetUsedByProductType()==AVCON_PRODUCT_PICC )
		{
			if (nWidth==g_AH2KDevFormatRS[i].nWidth &&
				nHeight==g_AH2KDevFormatRS[i].nHeight)
			{
				return true;
			}
		}
		else
		{
			if (nWidth==g_AH2KDevFormat[i].nWidth &&
				nHeight==g_AH2KDevFormat[i].nHeight)
			{
				return true;
			}
		}
	}

	nWidth=0;
	nHeight=0;
	return false;
}

int AH2KDevMgr::Init(void)
{
	XAutoLock l(g_csMapAH2KDevInst);

	if (g_nAH2KDevMgrRefCount<=0)
	{
		g_MapAH2KDevInst.clear();
		MPVIDEO_AH2K::Init();
	}
	g_nAH2KDevMgrRefCount++;

	return 0;
}

void AH2KDevMgr::Terminate(void)
{
	XAutoLock l(g_csMapAH2KDevInst);
	
	g_nAH2KDevMgrRefCount--;
	if (g_nAH2KDevMgrRefCount<=0)
	{
		g_MapAH2KDevInst.clear();
		MPVIDEO_AH2K::Terminate();
	}
}

int AH2KDevMgr::GetDevCount(void)
{
	int nDevCount=0;
	nDevCount+=MPVIDEO_AH2K::GetDevCount();
	return nDevCount;
}

int AH2KDevMgr::SetDevCap(unsigned int nDevIndex,int nWidth,int nHeight,int nFrameRate,bool bDenoise)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDevMgr::IsValidFormatSize(nWidth,nHeight);

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		if (iter->second.nWidth!=nWidth ||
			iter->second.nHeight!=nHeight ||
			iter->second.nFrameRate!=nFrameRate ||
			iter->second.bDenoise!=bDenoise)
		{
			iter->second.nWidth=nWidth;
			iter->second.nHeight=nHeight;
			iter->second.nFrameRate=nFrameRate;
			iter->second.bDenoise=bDenoise;

			if (iter->second.pAH2KDev!=NULL)
			{
				iter->second.pAH2KDev->Reset(nWidth,nHeight,nFrameRate,bDenoise);
			}
		}

	}
	else
	{
		AH2KDevInst tAH2KDevInst;
		tAH2KDevInst.nDevIndex=nDevIndex;
		tAH2KDevInst.nFrameRate=nFrameRate;
		tAH2KDevInst.bDenoise=bDenoise;
		tAH2KDevInst.nWidth=nWidth;
		tAH2KDevInst.nHeight=nHeight;
		tAH2KDevInst.pAH2KDev=NULL;
		g_MapAH2KDevInst[nDevIndex]=tAH2KDevInst;
	}

	return 0;
}
 
 
int AH2KDevMgr::SetVideoParam(unsigned int nDevIndex,int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;

		if (pAH2KDev!=NULL)
		{
			return pAH2KDev->SetVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
		}
	}

	return -1;
}

int AH2KDevMgr::SetVideoGammaRGB(unsigned int nDevIndex,int nGammaR,int nGammaG,int nGammaB)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;

		if (pAH2KDev!=NULL)
		{
			return pAH2KDev->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
		}
	}

	return -1;
}

int AH2KDevMgr::AddStreamSink(unsigned int nDevIndex,MPVIDEO_StreamSink*pStreamSink)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		if (iter->second.pAH2KDev==NULL)
		{
			unsigned int nDevCount=0;
			nDevCount=MPVIDEO_AH2K::GetDevCount();
			if (nDevIndex<nDevCount)
			{
				pAH2KDev=new AH2KDev(nDevIndex);

				if (0!=pAH2KDev->Open(iter->second.nWidth,iter->second.nHeight,iter->second.nFrameRate))
				{
					pAH2KDev->Close();
					delete pAH2KDev;
					pAH2KDev=NULL;
					return -1;
				}

				iter->second.pAH2KDev=pAH2KDev;
				pAH2KDev=pAH2KDev;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			pAH2KDev=iter->second.pAH2KDev;
		}
	}
	else
	{
		unsigned int nDevCount=0;
		nDevCount=MPVIDEO_AH2K::GetDevCount();
		if (nDevIndex<nDevCount)
		{
			pAH2KDev=new AH2KDev(nDevIndex);
			AH2KDevInst tAH2KDevInst;
			tAH2KDevInst.nDevIndex=nDevIndex;
			tAH2KDevInst.nFrameRate=DEFAULT_FRAME_RATE;
			tAH2KDevInst.nWidth=DEFAULT_WIDTH;
			tAH2KDevInst.nHeight=DEFAULT_HEIGHT;

			if (0!=pAH2KDev->Open(tAH2KDevInst.nWidth,tAH2KDevInst.nHeight,tAH2KDevInst.nFrameRate))
			{
				pAH2KDev->Close();
				delete pAH2KDev;
				pAH2KDev=NULL;
				return -1;
			}

			tAH2KDevInst.pAH2KDev=pAH2KDev;
			g_MapAH2KDevInst[nDevIndex]=tAH2KDevInst;
		}
		else
		{
			return -1;
		}
	}

	pAH2KDev->AddStreamSink(pStreamSink);

	return 0;
}

void AH2KDevMgr::RemoveStreamSink(unsigned int nDevIndex,MPVIDEO_StreamSink*pStreamSink)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;

		if (pAH2KDev!=NULL)
		{
			pAH2KDev->RemoveStreamSink(pStreamSink);
			if (pAH2KDev->CanBeClosed())
			{
				iter->second.pAH2KDev=NULL;

				pAH2KDev->Close();
				delete pAH2KDev;
				pAH2KDev=NULL;
			}
		}
	}
}

int AH2KDevMgr::StartVideoStream(unsigned int nDevIndex,int nBitrate,int nQuant,int qmin,int qmax)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;

		if (pAH2KDev!=NULL)
		{
			return pAH2KDev->StartVideoStream(nBitrate,nQuant,qmin,qmax);
		}
	}

	return -1;
}

int AH2KDevMgr::RequestKeyFrame(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;

		if (pAH2KDev!=NULL)
		{
			return pAH2KDev->RequestKeyFrame();
		}
	}

	return -1;
}

int AH2KDevMgr::StopVideoStream(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;

		if (pAH2KDev!=NULL)
		{
			return pAH2KDev->StopVideoStream();
		}
	}

	return -1;
}

int AH2KDevMgr::GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount)
{
	unsigned int nDevCount=0;
	nDevCount=MPVIDEO_AH2K::GetDevCount();
	if (nDevIndex<nDevCount)
	{
		return MPVIDEO_AH2K::GetDevName(nDevIndex,szName,nMaxCount);
	}

	return -1;
}


int AH2KDevMgr::GetDevType(unsigned int nDevIndex,MPVIDEO_DEV_TYPE&nDevType)
{
	unsigned int nDevCount=0;
	nDevCount=MPVIDEO_AH2K::GetDevCount();
	if (nDevIndex<nDevCount)
	{
		return MPVIDEO_AH2K::GetDevType(nDevIndex,nDevType);
	}

	return -1;
}

bool AH2KDevMgr::HasAudioInput(unsigned int nDevIndex)
{
	unsigned int nDevCount=0;
	nDevCount=MPVIDEO_AH2K::GetDevCount();
	if (nDevIndex<nDevCount)
	{
		return MPVIDEO_AH2K::HasAudioInput(nDevIndex);
	}
	return false;
}

bool AH2KDevMgr::SupportHardwareEncoding(unsigned int nDevIndex)
{
	unsigned int nDevCount=0;
	nDevCount=MPVIDEO_AH2K::GetDevCount();
	if (nDevIndex<nDevCount)
	{
		return MPVIDEO_AH2K::SupportHardwareEncoding(nDevIndex);
	}
	return false;
}

int AH2KDevMgr::AddImageSink(unsigned int nDevIndex,MPVIDEO_ImageSink*pImageSink)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		if (iter->second.pAH2KDev==NULL)
		{
			unsigned int nDevCount=0;
			nDevCount=MPVIDEO_AH2K::GetDevCount();
			if (nDevIndex<nDevCount)
			{
				pAH2KDev=new AH2KDev(nDevIndex);

				if (0!=pAH2KDev->Open(iter->second.nWidth,iter->second.nHeight,iter->second.nFrameRate,iter->second.bDenoise))
				{
					pAH2KDev->Close();
					delete pAH2KDev;
					pAH2KDev=NULL;
					return -1;
				}

				iter->second.pAH2KDev=pAH2KDev;
				pAH2KDev=pAH2KDev;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			pAH2KDev=iter->second.pAH2KDev;
		}
	}
	else
	{
		unsigned int nDevCount=0;
		nDevCount=MPVIDEO_AH2K::GetDevCount();
		if (nDevIndex<nDevCount)
		{
			pAH2KDev=new AH2KDev(nDevIndex);
			AH2KDevInst tAH2KDevInst;
			tAH2KDevInst.nDevIndex=nDevIndex;
			tAH2KDevInst.nFrameRate=DEFAULT_FRAME_RATE;
			tAH2KDevInst.nWidth=DEFAULT_WIDTH;
			tAH2KDevInst.nHeight=DEFAULT_HEIGHT;
			tAH2KDevInst.bDenoise=true;

			if (0!=pAH2KDev->Open(tAH2KDevInst.nWidth,tAH2KDevInst.nHeight,tAH2KDevInst.nFrameRate,tAH2KDevInst.bDenoise))
			{
				pAH2KDev->Close();
				delete pAH2KDev;
				pAH2KDev=NULL;
				return -1;
			}

			tAH2KDevInst.pAH2KDev=pAH2KDev;
			g_MapAH2KDevInst[nDevIndex]=tAH2KDevInst;
		}
		else
		{
			return -1;
		}
	}

	pAH2KDev->AddImageSink(pImageSink);

	return 0;
}

void AH2KDevMgr::RemoveImageSink(unsigned int nDevIndex,MPVIDEO_ImageSink*pImageSink)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;

		if (pAH2KDev!=NULL)
		{
			pAH2KDev->RemoveImageSink(pImageSink);
			if (pAH2KDev->CanBeClosed())
			{
				iter->second.pAH2KDev=NULL;

				pAH2KDev->Close();
				delete pAH2KDev;
				pAH2KDev=NULL;
			}
		}
	}
}

void AH2KDevMgr::SetInvert(unsigned int nDevIndex,int nInvert)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;
		pAH2KDev->SetInvert(nInvert);
	}
}

int AH2KDevMgr::GetInvert(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;
		return pAH2KDev->GetInvert();
	}

	return -1;
}

void AH2KDevMgr::SetDenoise(unsigned int nDevIndex,int nDenoise)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		iter->second.bDenoise=(nDenoise!=0);
		pAH2KDev=iter->second.pAH2KDev;
		pAH2KDev->SetDenoise(nDenoise);
	}
}

int AH2KDevMgr::GetDenoise(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;
		return pAH2KDev->GetDenoise();
	}

	return -1;
}

void AH2KDevMgr::SetLevelFixer(unsigned int nDevIndex,int nLevelFixer)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;
		pAH2KDev->SetLevelFixer(nLevelFixer);
	}
}

int AH2KDevMgr::GetLevelFixer(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;
		return pAH2KDev->GetLevelFixer();
	}

	return -1;
}

	
void AH2KDevMgr::EnableOSD(unsigned int nDevIndex,bool bEnable)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;
		pAH2KDev->EnableOSD(bEnable);
	}
}

bool AH2KDevMgr::IsOSDEnable(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;
		return pAH2KDev->IsOSDEnable();
	}

	return false;
}

void AH2KDevMgr::SetOSDText(unsigned int nDevIndex,const char*cszText,int nAlign)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;
		pAH2KDev->SetOSDText(cszText,nAlign);
	}
}

void AH2KDevMgr::SetOSDFont(unsigned int nDevIndex,LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	XAutoLock l(g_csMapAH2KDevInst);

	AH2KDev*pAH2KDev=NULL;

	MapAH2KDevInst::iterator iter=g_MapAH2KDevInst.find(nDevIndex);
	if (iter!=g_MapAH2KDevInst.end())
	{
		pAH2KDev=iter->second.pAH2KDev;
		pAH2KDev->SetOSDFont(fontOSD,crText,crBk);
	}
}


#endif