#include "XCameraDevMgr.h"
#include "XCameraDev.h"
#include "MPVIDEO_CameraDev.h"
#include "XAutoLock.h"
#include "AVCONProduct.h"
#include <MPVideo/MPVIDEO_AH2K.h>

#include <map>

#define DEFAULT_FRAME_RATE	15
#define DEFAULT_WIDTH		352
#define DEFAULT_HEIGHT		288

typedef struct XCameraDevInst
{
	unsigned int		nDevIndex;
	int					nWidth;
	int					nHeight;
	int					nFrameRate;
	bool				bDefaultFormat;

	XCameraDev*		pXCameraDev;
}XCameraDevInst;

typedef std::map<unsigned int,XCameraDevInst>	MapXCameraDevInst;

static XCritSec			g_csMapXCameraDevInst;
static MapXCameraDevInst		g_MapXCameraDevInst;
int						g_nXCameraDevMgrRefCount=0;



extern DWORD GetUsedByProductType(void);
extern bool GetOnlyUSBCamera(void);


typedef struct tagCAMERA_DEV_FORMAT
{
	int		nWidth;
	int		nHeight;
}CAMERA_DEV_FORMAT;

static CAMERA_DEV_FORMAT g_XCameraDevFormat[]={
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

int XCameraDevMgr::GetFormatCount(void)
{
	//int nAH2KDevCount=MPVIDEO_AH2K::GetDevCount();
	//if (nAH2KDevCount==0 && (GetUsedByProductType()==AVCON_PRODUCT_RS || GetUsedByProductType()==AVCON_PRODUCT_XF_D || GetOnlyUSBCamera()))
	//{
	//	return sizeof(g_XCameraDevFormat)/sizeof(CAMERA_DEV_FORMAT)-4;
	//}
	{
		return sizeof(g_XCameraDevFormat)/sizeof(CAMERA_DEV_FORMAT);
	}
}

int XCameraDevMgr::GetFormatName(int nIndex,char*szName,int nMaxCount)
{
	if (nIndex<0)
		return -1;

	if (nIndex>=XCameraDevMgr::GetFormatCount())
	{
		return -1;
	}

	//if (nIndex==0)
	//{
	//	strcpy(szName,"Auto");
	//	return 0;
	//}

	sprintf(szName,"%u X %u",g_XCameraDevFormat[nIndex].nWidth,g_XCameraDevFormat[nIndex].nHeight);

	return 0;
}

int XCameraDevMgr::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	nWidth=0;
	nHeight=0;

	if (nIndex<0)
		return -1;

	int nCount=XCameraDevMgr::GetFormatCount();
	if (nIndex>=nCount)
	{
		return -1;
	}

	nWidth=g_XCameraDevFormat[nIndex].nWidth;
	nHeight=g_XCameraDevFormat[nIndex].nHeight;

	return 0;
}

bool XCameraDevMgr::IsValidFormatSize(int&nWidth,int&nHeight)
{
	int nCount=XCameraDevMgr::GetFormatCount();
	for (int i=0;i<nCount;i++)
	{
		if (nWidth==g_XCameraDevFormat[i].nWidth &&
			nHeight==g_XCameraDevFormat[i].nHeight)
		{
			return true;
		}
	}

	nWidth=0;
	nHeight=0;
	return false;
}

int XCameraDevMgr::Init(void)
{
	XAutoLock l(g_csMapXCameraDevInst);

	if (g_nXCameraDevMgrRefCount<=0)
	{
		g_MapXCameraDevInst.clear();
	}
	g_nXCameraDevMgrRefCount++;

	return 0;
}

void XCameraDevMgr::Terminate(void)
{
	XAutoLock l(g_csMapXCameraDevInst);
	
	g_nXCameraDevMgrRefCount--;
	if (g_nXCameraDevMgrRefCount<=0)
	{
		g_MapXCameraDevInst.clear();
	}
}

int XCameraDevMgr::GetDevCount(void)
{
	int nDevCount=0;
	nDevCount+=MPVIDEO_Camera::GetDeviceCount();
	return nDevCount;
}

int XCameraDevMgr::SetDevCap(unsigned int nDevIndex,int nWidth,int nHeight,int nFrameRate,bool bDefaultFormat)
{
	XAutoLock l(g_csMapXCameraDevInst);

	if (nFrameRate<1)
		nFrameRate=1;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		if (iter->second.nWidth!=nWidth ||
			iter->second.nHeight!=nHeight ||
			iter->second.nFrameRate!=nFrameRate ||
			iter->second.bDefaultFormat!=bDefaultFormat )
		{
			iter->second.nWidth=nWidth;
			iter->second.nHeight=nHeight;
			iter->second.nFrameRate=nFrameRate;
			iter->second.bDefaultFormat=bDefaultFormat;

			if (iter->second.pXCameraDev!=NULL)
			{
				iter->second.pXCameraDev->Reset(nWidth,nHeight,nFrameRate,bDefaultFormat);
			}
		}

	}
	else
	{
		XCameraDevInst tXCameraDevInst;
		tXCameraDevInst.nDevIndex=nDevIndex;
		tXCameraDevInst.nFrameRate=nFrameRate;
		tXCameraDevInst.bDefaultFormat=bDefaultFormat;
		tXCameraDevInst.nWidth=nWidth;
		tXCameraDevInst.nHeight=nHeight;
		tXCameraDevInst.pXCameraDev=NULL;
		g_MapXCameraDevInst[nDevIndex]=tXCameraDevInst;
	}

	return 0;
}

 
int XCameraDevMgr::SetVideoParam(unsigned int nDevIndex,int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;

		if (pXCameraDev!=NULL)
		{
			return pXCameraDev->SetVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
		}
	}

	return -1;
}

int XCameraDevMgr::SetVideoGammaRGB(unsigned int nDevIndex,int nGammaR,int nGammaG,int nGammaB)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;

		if (pXCameraDev!=NULL)
		{
			return pXCameraDev->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
		}
	}

	return -1;
}

int XCameraDevMgr::GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount)
{
	unsigned int nDevCount=0;
	nDevCount=MPVIDEO_Camera::GetDeviceCount();
	if (nDevIndex<nDevCount)
	{
		char szTemp[1024]="";
		if (0!=MPVIDEO_Camera::GetDeviceName(nDevIndex,szTemp))
		{
			return -1;
		}


		if (nMaxCount<=0)
			return -1;

		if (szName==NULL)
		{
			return strlen(szTemp)+1;
		}

		if (nDevCount>1)
		{
			char szIndex[128]="";
			sprintf(szIndex,"%u",nDevIndex);
			strcpy(szTemp+strlen(szTemp),szIndex);
		}

		if (nMaxCount>=strlen(szTemp)+1)
		{
			strcpy(szName,szTemp);
		}
		else
		{
			strncpy(szName,szTemp,nMaxCount);
		}

		return strlen(szName)+1;
	}

	return -1;
}


int XCameraDevMgr::AddImageSink(unsigned int nDevIndex,MPVIDEO_ImageSink*pImageSink)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		if (iter->second.pXCameraDev==NULL)
		{
			unsigned int nDevCount=0;
			nDevCount=MPVIDEO_Camera::GetDeviceCount();
			if (nDevIndex<nDevCount)
			{
				pXCameraDev=new XCameraDev(nDevIndex);

				if (0!=pXCameraDev->Open(iter->second.nWidth,iter->second.nHeight,iter->second.nFrameRate,iter->second.bDefaultFormat))
				{
					pXCameraDev->Close();
					delete pXCameraDev;
					pXCameraDev=NULL;
					return -1;
				}

				iter->second.pXCameraDev=pXCameraDev;
				pXCameraDev=pXCameraDev;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			pXCameraDev=iter->second.pXCameraDev;
		}
	}
	else
	{
		unsigned int nDevCount=0;
		nDevCount=MPVIDEO_Camera::GetDeviceCount();
		if (nDevIndex<nDevCount)
		{
			pXCameraDev=new XCameraDev(nDevIndex);
			XCameraDevInst tXCameraDevInst;
			tXCameraDevInst.nDevIndex=nDevIndex;
			tXCameraDevInst.nFrameRate=DEFAULT_FRAME_RATE;
			tXCameraDevInst.nWidth=DEFAULT_WIDTH;
			tXCameraDevInst.nHeight=DEFAULT_HEIGHT;
			tXCameraDevInst.bDefaultFormat=true;

			if (0!=pXCameraDev->Open(tXCameraDevInst.nWidth,tXCameraDevInst.nHeight,tXCameraDevInst.nFrameRate,tXCameraDevInst.bDefaultFormat))
			{
				pXCameraDev->Close();
				delete pXCameraDev;
				pXCameraDev=NULL;
				return -1;
			}

			tXCameraDevInst.pXCameraDev=pXCameraDev;
			g_MapXCameraDevInst[nDevIndex]=tXCameraDevInst;
		}
		else
		{
			return -1;
		}
	}

	pXCameraDev->AddImageSink(pImageSink);

	return 0;
}

void XCameraDevMgr::RemoveImageSink(unsigned int nDevIndex,MPVIDEO_ImageSink*pImageSink)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;

		if (pXCameraDev!=NULL)
		{
			pXCameraDev->RemoveImageSink(pImageSink);
			if (pXCameraDev->CanBeClosed())
			{
				iter->second.pXCameraDev=NULL;

				pXCameraDev->Close();
				delete pXCameraDev;
				pXCameraDev=NULL;
			}
		}
	}
}

void XCameraDevMgr::ShowAdjustVideoWnd(unsigned int nDevIndex,HWND hWnd)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		pXCameraDev->ShowAdjustVideoWnd(hWnd);
	}
}

void XCameraDevMgr::ShowVideoInputWnd(unsigned int nDevIndex,HWND hWnd)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		pXCameraDev->ShowVideoInputWnd(hWnd);
	}
}

bool XCameraDevMgr::HasVideoInputWnd(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		return pXCameraDev->HasVideoInputWnd();
	}
	return false;
}

int XCameraDevMgr::TVTunerPutChannel(unsigned int nDevIndex,long lChannel)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		return pXCameraDev->TVTunerPutChannel(lChannel);
	}

	return -1;
}

void XCameraDevMgr::SetInvert(unsigned int nDevIndex,int nInvert)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		pXCameraDev->SetInvert(nInvert);
	}
}

int XCameraDevMgr::GetInvert(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		return pXCameraDev->GetInvert();
	}

	return -1;
}

void XCameraDevMgr::SetDenoise(unsigned int nDevIndex,int nDenoise)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		pXCameraDev->SetDenoise(nDenoise);
	}
}

int XCameraDevMgr::GetDenoise(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		return pXCameraDev->GetDenoise();
	}

	return -1;
}

void XCameraDevMgr::SetLevelFixer(unsigned int nDevIndex,int nLevelFixer)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		pXCameraDev->SetLevelFixer(nLevelFixer);
	}
}

int XCameraDevMgr::GetLevelFixer(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		return pXCameraDev->GetLevelFixer();
	}

	return -1;
}	

void XCameraDevMgr::EnableOSD(unsigned int nDevIndex,bool bEnable)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		pXCameraDev->EnableOSD(bEnable);
	}
}

bool XCameraDevMgr::IsOSDEnable(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		return pXCameraDev->IsOSDEnable();
	}

	return false;
}

void XCameraDevMgr::SetOSDText(unsigned int nDevIndex,const char*cszText,int nAlign)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		pXCameraDev->SetOSDText(cszText,nAlign);
	}
}

void XCameraDevMgr::SetOSDFont(unsigned int nDevIndex,LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		pXCameraDev->SetOSDFont(fontOSD,crText,crBk);
	}
}


//设置遮掩并启动遮掩
int XCameraDevMgr::SetupMask(unsigned int nDevIndex,RECT *RectList, int iRectCount)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		return pXCameraDev->SetupMask(RectList,iRectCount);
	}

	return -1;
}

//停止遮掩
int XCameraDevMgr::StopMask(unsigned int nDevIndex)
{
	XAutoLock l(g_csMapXCameraDevInst);

	XCameraDev*pXCameraDev=NULL;

	MapXCameraDevInst::iterator iter=g_MapXCameraDevInst.find(nDevIndex);
	if (iter!=g_MapXCameraDevInst.end())
	{
		pXCameraDev=iter->second.pXCameraDev;
		return pXCameraDev->StopMask();
	}

	return -1;
}



//列出可裁剪的格式的个数
int MPVIDEO_CameraDev::GetFormatCount(void)
{
	return XCameraDevMgr::GetFormatCount();
}
	
//取格式名称
int MPVIDEO_CameraDev::GetFormatName(int nIndex,char*szName,int nMaxCount)
{
	return XCameraDevMgr::GetFormatName(nIndex,szName,nMaxCount);
}

//取格式大小
int MPVIDEO_CameraDev::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	return XCameraDevMgr::GetFormatSize(nIndex,nWidth,nHeight);
}
