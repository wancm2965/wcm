 
#include "VIDEC_AH400Dev.h"
#include "AH400DevMgr.h"
#include "AH400Dev.h"
#include "XAutoLock.h"

#include <map>


#ifdef SUPPORT_AH400

#define DEFAULT_FRAME_RATE	15
#define DEFAULT_WIDTH		352
#define DEFAULT_HEIGHT		288

typedef struct AH400DevInst
{
	unsigned int		nDevIndex;
	int					nWidth;
	int					nHeight;
	int					nFrameRate;

	AH400Dev*			pAH400Dev;
}AH400DevInst;

typedef std::map<unsigned int,AH400DevInst>	MapAH400DevInst;

static XCritSec			g_csMapAH400DevInst;
static MapAH400DevInst	g_MapAH400DevInst;
int						g_nAH400DevMgrRefCount=0;




typedef struct tagAH400_DEV_FORMAT
{
	int		nWidth;
	int		nHeight;
}AH400_DEV_FORMAT;

static AH400_DEV_FORMAT g_AH400DevFormat[]={
	{0,0},
	{176,144},
	{320,240},
	{352,288},
	{640,480},
	{704,576},
};

int AH400DevMgr::GetFormatCount(void)
{
	return sizeof(g_AH400DevFormat)/sizeof(AH400_DEV_FORMAT);
}

int AH400DevMgr::GetFormatName(int nIndex,char*szName,int nMaxCount)
{
	if (nIndex<0)
		return -1;

	if (nIndex>=AH400DevMgr::GetFormatCount())
	{
		return -1;
	}

	if (nIndex==0)
	{
		strcpy(szName,"Auto");
		return 0;
	}

	sprintf(szName,"%u X %u",g_AH400DevFormat[nIndex].nWidth,g_AH400DevFormat[nIndex].nHeight);

	return 0;
}

int AH400DevMgr::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	nWidth=0;
	nHeight=0;

	if (nIndex<0)
		return -1;

	int nCount=AH400DevMgr::GetFormatCount();
	if (nIndex>=nCount)
	{
		return -1;
	}

	nWidth=g_AH400DevFormat[nIndex].nWidth;
	nHeight=g_AH400DevFormat[nIndex].nHeight;

	return 0;
}

bool AH400DevMgr::IsValidFormatSize(int&nWidth,int&nHeight)
{
	int nCount=AH400DevMgr::GetFormatCount();
	for (int i=0;i<nCount;i++)
	{
		if (nWidth==g_AH400DevFormat[i].nWidth &&
			nHeight==g_AH400DevFormat[i].nHeight)
		{
			return true;
		}
	}

	nWidth=0;
	nHeight=0;
	return false;
}

int AH400DevMgr::Init(void)
{
	XAutoLock l(g_csMapAH400DevInst);

	if (g_nAH400DevMgrRefCount<=0)
	{
		g_MapAH400DevInst.clear();
		VIDEC_AH400::Init();
	}
	g_nAH400DevMgrRefCount++;

	return 0;
}

void AH400DevMgr::Terminate(void)
{
	XAutoLock l(g_csMapAH400DevInst);
	
	g_nAH400DevMgrRefCount--;
	if (g_nAH400DevMgrRefCount<=0)
	{
		g_MapAH400DevInst.clear();
		VIDEC_AH400::Terminate();
	}
}

int AH400DevMgr::GetDevCount(void)
{
	int nDevCount=0;
	nDevCount+=VIDEC_AH400::GetDevCount();
	return nDevCount;
}

int AH400DevMgr::SetDevCap(unsigned int nDevIndex,int nWidth,int nHeight,int nFrameRate)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400DevMgr::IsValidFormatSize(nWidth,nHeight);

	if (nFrameRate<1)
		nFrameRate=1;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		if (iter->second.nWidth!=nWidth ||
			iter->second.nHeight!=nHeight ||
			iter->second.nFrameRate!=nFrameRate)
		{
			iter->second.nWidth=nWidth;
			iter->second.nHeight=nHeight;
			iter->second.nFrameRate=nFrameRate;

			if (iter->second.pAH400Dev!=NULL)
			{
				iter->second.pAH400Dev->Reset(nWidth,nHeight,nFrameRate);
			}
		}

	}
	else
	{
		AH400DevInst tAH400DevInst;
		tAH400DevInst.nDevIndex=nDevIndex;
		tAH400DevInst.nFrameRate=nFrameRate;
		tAH400DevInst.nWidth=nWidth;
		tAH400DevInst.nHeight=nHeight;
		tAH400DevInst.pAH400Dev=NULL;
		g_MapAH400DevInst[nDevIndex]=tAH400DevInst;
	}

	return 0;
}
 
int AH400DevMgr::SetVideoParam(unsigned int nDevIndex,int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;

		if (pAH400Dev!=NULL)
		{
			return pAH400Dev->SetVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
		}
	}

	return -1;
}

int AH400DevMgr::SetVideoGammaRGB(unsigned int nDevIndex,int nGammaR,int nGammaG,int nGammaB)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;

		if (pAH400Dev!=NULL)
		{
			return pAH400Dev->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
		}
	}

	return -1;
}

int AH400DevMgr::GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount)
{
	unsigned int nDevCount=0;
	nDevCount=VIDEC_AH400::GetDevCount();
	if (nDevIndex<nDevCount)
	{
		return VIDEC_AH400::GetDevName(nDevIndex,szName,nMaxCount);
	}

	return -1;
}


int AH400DevMgr::AddImageSink(unsigned int nDevIndex,VIDEC_ImageSink*pImageSink)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		if (iter->second.pAH400Dev==NULL)
		{
			unsigned int nDevCount=0;
			nDevCount=VIDEC_AH400::GetDevCount();
			if (nDevIndex<nDevCount)
			{
				NEWRECORD( pAH400Dev , new AH400Dev(nDevIndex) );
NULLReturnRet( pAH400Dev  , -1)  
				if (0!=pAH400Dev->Open(iter->second.nWidth,iter->second.nHeight,iter->second.nFrameRate))
				{
					pAH400Dev->Close();
					delete pAH400Dev;
					pAH400Dev=NULL;
					return -1;
				}

				iter->second.pAH400Dev=pAH400Dev;
				pAH400Dev=pAH400Dev;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			pAH400Dev=iter->second.pAH400Dev;
		}
	}
	else
	{
		unsigned int nDevCount=0;
		nDevCount=VIDEC_AH400::GetDevCount();
		if (nDevIndex<nDevCount)
		{
			NEWRECORD( pAH400Dev , new AH400Dev(nDevIndex) );
NULLReturnRet( pAH400Dev  , -1)  
			AH400DevInst tAH400DevInst;
			tAH400DevInst.nDevIndex=nDevIndex;
			tAH400DevInst.nFrameRate=DEFAULT_FRAME_RATE;
			tAH400DevInst.nWidth=DEFAULT_WIDTH;
			tAH400DevInst.nHeight=DEFAULT_HEIGHT;

			if (0!=pAH400Dev->Open(tAH400DevInst.nWidth,tAH400DevInst.nHeight,tAH400DevInst.nFrameRate))
			{
				pAH400Dev->Close();
				delete pAH400Dev;
				pAH400Dev=NULL;
				return -1;
			}

			tAH400DevInst.pAH400Dev=pAH400Dev;
			g_MapAH400DevInst[nDevIndex]=tAH400DevInst;
		}
		else
		{
			return -1;
		}
	}

	pAH400Dev->AddImageSink(pImageSink);

	return 0;
}

void AH400DevMgr::RemoveImageSink(unsigned int nDevIndex,VIDEC_ImageSink*pImageSink)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;

		if (pAH400Dev!=NULL)
		{
			pAH400Dev->RemoveImageSink(pImageSink);
			if (pAH400Dev->CanBeClosed())
			{
				iter->second.pAH400Dev=NULL;

				pAH400Dev->Close();
				delete pAH400Dev;
				pAH400Dev=NULL;
			}
		}
	}
}


int AH400DevMgr::AddStreamSink(unsigned int nDevIndex,VIDEC_StreamSink*pStreamSink)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		if (iter->second.pAH400Dev==NULL)
		{
			unsigned int nDevCount=0;
			nDevCount=VIDEC_AH400::GetDevCount();
			if (nDevIndex<nDevCount)
			{
				NEWRECORD( pAH400Dev , new AH400Dev(nDevIndex) );
NULLReturnRet( pAH400Dev  , -1)  
				if (0!=pAH400Dev->Open(iter->second.nWidth,iter->second.nHeight,iter->second.nFrameRate))
				{
					pAH400Dev->Close();
					delete pAH400Dev;
					pAH400Dev=NULL;
					return -1;
				}

				iter->second.pAH400Dev=pAH400Dev;
				pAH400Dev=pAH400Dev;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			pAH400Dev=iter->second.pAH400Dev;
		}
	}
	else
	{
		unsigned int nDevCount=0;
		nDevCount=VIDEC_AH400::GetDevCount();
		if (nDevIndex<nDevCount)
		{
			NEWRECORD( pAH400Dev , new AH400Dev(nDevIndex) );
NULLReturnRet( pAH400Dev  , -1)  
			AH400DevInst tAH400DevInst;
			tAH400DevInst.nDevIndex=nDevIndex;
			tAH400DevInst.nFrameRate=DEFAULT_FRAME_RATE;
			tAH400DevInst.nWidth=DEFAULT_WIDTH;
			tAH400DevInst.nHeight=DEFAULT_HEIGHT;

			if (0!=pAH400Dev->Open(tAH400DevInst.nWidth,tAH400DevInst.nHeight,tAH400DevInst.nFrameRate))
			{
				pAH400Dev->Close();
				delete pAH400Dev;
				pAH400Dev=NULL;
				return -1;
			}

			tAH400DevInst.pAH400Dev=pAH400Dev;
			g_MapAH400DevInst[nDevIndex]=tAH400DevInst;
		}
		else
		{
			return -1;
		}
	}

	pAH400Dev->AddStreamSink(pStreamSink);

	return 0;
}

void AH400DevMgr::RemoveStreamSink(unsigned int nDevIndex,VIDEC_StreamSink*pStreamSink)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;

		if (pAH400Dev!=NULL)
		{
			pAH400Dev->RemoveStreamSink(pStreamSink);
			if (pAH400Dev->CanBeClosed())
			{
				iter->second.pAH400Dev=NULL;

				pAH400Dev->Close();
				delete pAH400Dev;
				pAH400Dev=NULL;
			}
		}
	}
}

int AH400DevMgr::StartVideoStream(unsigned int nDevIndex,int nBitrate,int nQuant,int qmin,int qmax,int nKeyFrameInterval,bool bEnableSub,int nSubScaleType,int nSubBitrate,int nSubQuant,int nSubQMin,int nSubQMax)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;

		if (pAH400Dev!=NULL)
		{
			return pAH400Dev->StartVideoStream(nBitrate,nQuant,qmin,qmax,nKeyFrameInterval,bEnableSub,nSubScaleType,nSubBitrate,nSubQuant,nSubQMin,nSubQMax);
		}
	}
	return -1;
}

int AH400DevMgr::StopVideoStream(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;

		if (pAH400Dev!=NULL)
		{
			return pAH400Dev->StopVideoStream();
		}
	}
	return -1;
}

int AH400DevMgr::RequestKeyFrame(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;

		if (pAH400Dev!=NULL)
		{
			return pAH400Dev->RequestKeyFrame();
		}
	}
	return -1;
}

int AH400DevMgr::RequestSubKeyFrame(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;

		if (pAH400Dev!=NULL)
		{
			return pAH400Dev->RequestSubKeyFrame();
		}
	}
	return -1;
}

void AH400DevMgr::SetInvert(unsigned int nDevIndex,int nInvert)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		pAH400Dev->SetInvert(nInvert);
	}
}

int AH400DevMgr::GetInvert(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		return pAH400Dev->GetInvert();
	}

	return -1;
}

void AH400DevMgr::SetDenoise(unsigned int nDevIndex,int nDenoise)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		pAH400Dev->SetDenoise(nDenoise);
	}
}

int AH400DevMgr::GetDenoise(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		return pAH400Dev->GetDenoise();
	}

	return -1;
}

void AH400DevMgr::SetLevelFixer(unsigned int nDevIndex,int nLevelFixer)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		pAH400Dev->SetLevelFixer(nLevelFixer);
	}
}

int AH400DevMgr::GetLevelFixer(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		return pAH400Dev->GetLevelFixer();
	}

	return -1;
}

//设置遮掩并启动遮掩
int AH400DevMgr::SetupMask(unsigned int nDevIndex,RECT *RectList, int iRectCount)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		return pAH400Dev->SetupMask(RectList,iRectCount);
	}

	return -1;
}

//停止遮掩
int AH400DevMgr::StopMask(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		return pAH400Dev->StopMask();
	}

	return -1;
}

int AH400DevMgr::StartMotionDetect(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		return pAH400Dev->StartMotionDetect();
	}

	return -1;
}

int AH400DevMgr::StopMotionDetect(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		return pAH400Dev->StopMotionDetect();
	}

	return -1;
}

int AH400DevMgr::SetupMotionDetect(unsigned int nDevIndex,int iGrade/*灵敏度等级0-6*/,RECT *RectList, int iRectCount)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		return pAH400Dev->SetupMotionDetect(iGrade,RectList,iRectCount);
	}

	return -1;
}
	
void AH400DevMgr::EnableOSD(unsigned int nDevIndex,bool bEnable)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		pAH400Dev->EnableOSD(bEnable);
	}
}

bool AH400DevMgr::IsOSDEnable(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		return pAH400Dev->IsOSDEnable();
	}

	return false;
}

void AH400DevMgr::SetOSDText(unsigned int nDevIndex,const char*cszText,int nAlign,int nTop,int nLeft)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		pAH400Dev->SetOSDText(cszText,nAlign,nTop,nLeft);
	}
}

void AH400DevMgr::SetOSDFont(unsigned int nDevIndex,LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	XAutoLock l(g_csMapAH400DevInst);

	AH400Dev*pAH400Dev=NULL;

	MapAH400DevInst::iterator iter=g_MapAH400DevInst.find(nDevIndex);
	if (iter!=g_MapAH400DevInst.end())
	{
		pAH400Dev=iter->second.pAH400Dev;
		pAH400Dev->SetOSDFont(fontOSD,crText,crBk);
	}
}


#endif

