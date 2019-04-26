#include "SDDevMgr.h"
#include "SDDev.h"
#include "SDDevCamera.h"
#include "XAutoLock.h"

#include <map>

#define DEFAULT_FRAME_RATE	15
#define DEFAULT_WIDTH		352
#define DEFAULT_HEIGHT		288

typedef struct SDDevInst
{
	unsigned int	nDevIndex;
	int				nWidth;
	int				nHeight;
	int				nFrameRate;
	bool			bAutoFrameRate;

	SDDev*			pSDDev;
}SDDevInst;

typedef std::map<unsigned int,SDDevInst>	MapSDDevInst;

static XCritSec			g_csMapSDDevInst;
static MapSDDevInst		g_MapSDDevInst;
int						g_nSDDevMgrRefCount=0;




typedef struct tagCAMERA_DEV_FORMAT
{
	int		nWidth;
	int		nHeight;
}CAMERA_DEV_FORMAT;

static CAMERA_DEV_FORMAT g_SDDevFormat[]={
	{176,144},
	{320,240},
	{352,288},
	{640,480},
	{704,576},
};

int SDDevMgr::GetFormatCount(void)
{
	return sizeof(g_SDDevFormat)/sizeof(CAMERA_DEV_FORMAT);
}

int SDDevMgr::GetFormatName(int nIndex,char*szName)
{
	if (nIndex<0)
		return -1;

	if (nIndex>=SDDevMgr::GetFormatCount())
	{
		return -1;
	}

	sprintf(szName,"%u X %u",g_SDDevFormat[nIndex].nWidth,g_SDDevFormat[nIndex].nHeight);

	return 0;
}

int SDDevMgr::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	nWidth=0;
	nHeight=0;

	if (nIndex<0)
		return -1;

	int nCount=SDDevMgr::GetFormatCount();
	if (nIndex>=nCount)
	{
		return -1;
	}

	nWidth=g_SDDevFormat[nIndex].nWidth;
	nHeight=g_SDDevFormat[nIndex].nHeight;

	return 0;
}

bool SDDevMgr::IsValidFormatSize(int&nWidth,int&nHeight)
{
	int nCount=SDDevMgr::GetFormatCount();
	for (int i=0;i<nCount;i++)
	{
		if (nWidth==g_SDDevFormat[i].nWidth &&
			nHeight==g_SDDevFormat[i].nHeight)
		{
			return true;
		}
	}

	nWidth=0;
	nHeight=0;
	return false;
}

int SDDevMgr::Init(void)
{
	XAutoLock l(g_csMapSDDevInst);

	if (g_nSDDevMgrRefCount<=0)
	{
		g_MapSDDevInst.clear();
	}
	g_nSDDevMgrRefCount++;

	return 0;
}

void SDDevMgr::Terminate(void)
{
	XAutoLock l(g_csMapSDDevInst);
	
	g_nSDDevMgrRefCount--;
	if (g_nSDDevMgrRefCount<=0)
	{
		g_MapSDDevInst.clear();
	}
}

int SDDevMgr::GetDevCount(void)
{
	int nDevCount=0;
	nDevCount+=VIDEC_Camera::GetDeviceCount();
	return nDevCount;
}

int SDDevMgr::SetDevCap(unsigned int nDevIndex,int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate)
{
	XAutoLock l(g_csMapSDDevInst);

	SDDevMgr::IsValidFormatSize(nWidth,nHeight);

	if (nFrameRate<1)
		nFrameRate=1;
	else if (nFrameRate>30)
		nFrameRate=30;

	MapSDDevInst::iterator iter=g_MapSDDevInst.find(nDevIndex);
	if (iter!=g_MapSDDevInst.end())
	{
		if (iter->second.nWidth!=nWidth ||
			iter->second.nHeight!=nHeight ||
			iter->second.nFrameRate!=nFrameRate)
		{
			iter->second.nWidth=nWidth;
			iter->second.nHeight=nHeight;
			iter->second.nFrameRate=nFrameRate;

			if (iter->second.pSDDev!=NULL)
			{
				iter->second.pSDDev->SetDevCap(nWidth,nHeight,nFrameRate,bAutoFrameRate);
			}
		}

	}
	else
	{
		SDDevInst tSDDevInst;
		tSDDevInst.nDevIndex=nDevIndex;
		tSDDevInst.nFrameRate=nFrameRate;
		tSDDevInst.nWidth=nHeight;
		tSDDevInst.nHeight=nWidth;
		tSDDevInst.pSDDev=NULL;
		g_MapSDDevInst[nDevIndex]=tSDDevInst;
	}

	return 0;
}

int SDDevMgr::GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount)
{
	//unsigned int nDevCount=0;
	//nDevCount=VIDEC_Camera::GetDeviceCount();
	//if (nDevIndex<nDevCount)
	//{
	//	sprintf(szName,"AVCON AH2000 %u",nDevIndex);
	//	return 0;
	//}

	return -1;
}

int SDDevMgr::AddOutputImage(unsigned int nDevIndex,VIDEC_OutputImage*pOutputImage)
{
	XAutoLock l(g_csMapSDDevInst);

	SDDev*pSDDev=NULL;

	MapSDDevInst::iterator iter=g_MapSDDevInst.find(nDevIndex);
	if (iter!=g_MapSDDevInst.end())
	{
		if (iter->second.pSDDev==NULL)
		{
			unsigned int nDevCount=0;
			nDevCount=VIDEC_Camera::GetDeviceCount();
			if (nDevIndex<nDevCount)
			{
				SDDevCamera*pCamera=new SDDevCamera(nDevIndex);

				if (0!=pCamera->Open(iter->second.nWidth,iter->second.nHeight,iter->second.nFrameRate,iter->second.bAutoFrameRate))
				{
					pCamera->Close();
					delete pCamera;
					pCamera=NULL;
					return -1;
				}

				iter->second.pSDDev=pCamera;
				pSDDev=pCamera;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			pSDDev=iter->second.pSDDev;
		}
	}
	else
	{
		unsigned int nDevCount=0;
		nDevCount=VIDEC_Camera::GetDeviceCount();
		if (nDevIndex<nDevCount)
		{
			SDDevCamera*pSDDevCamera=new SDDevCamera(nDevIndex);
			SDDevInst tSDDevInst;
			tSDDevInst.nDevIndex=nDevIndex;
			tSDDevInst.nFrameRate=DEFAULT_FRAME_RATE;
			tSDDevInst.nWidth=DEFAULT_WIDTH;
			tSDDevInst.nHeight=DEFAULT_HEIGHT;
			tSDDevInst.bAutoFrameRate=false;

			if (0!=pSDDevCamera->Open(tSDDevInst.nWidth,tSDDevInst.nHeight,tSDDevInst.nFrameRate,tSDDevInst.bAutoFrameRate))
			{
				pSDDevCamera->Close();
				delete pSDDevCamera;
				pSDDevCamera=NULL;
				return -1;
			}

			tSDDevInst.pSDDev=pSDDevCamera;
			g_MapSDDevInst[nDevIndex]=tSDDevInst;
			pSDDev=pSDDevCamera;
		}
		else
		{
			return -1;
		}
	}

	pSDDev->AddOutputImage(pOutputImage);

	return 0;
}

void SDDevMgr::RemoveOutputImage(unsigned int nDevIndex,VIDEC_OutputImage*pOutputImage)
{
	XAutoLock l(g_csMapSDDevInst);

	SDDev*pSDDev=NULL;

	MapSDDevInst::iterator iter=g_MapSDDevInst.find(nDevIndex);
	if (iter!=g_MapSDDevInst.end())
	{
		pSDDev=iter->second.pSDDev;
		if (pSDDev!=NULL)
		{
			pSDDev->RemoveOutputImage(pOutputImage);
			if (pSDDev->CanBeClosed())
			{
				iter->second.pSDDev=NULL;

				pSDDev->Close();
				delete pSDDev;
				pSDDev=NULL;
			}
		}
	}
}

int SDDevMgr::AddOutputStream(unsigned int nDevIndex,VIDEC_OutputStream*pOutputStream)
{
	XAutoLock l(g_csMapSDDevInst);

	SDDev*pSDDev=NULL;

	MapSDDevInst::iterator iter=g_MapSDDevInst.find(nDevIndex);
	if (iter!=g_MapSDDevInst.end())
	{
		if (iter->second.pSDDev==NULL)
		{
			unsigned int nDevCount=0;
			nDevCount=VIDEC_Camera::GetDeviceCount();
			if (nDevIndex<nDevCount)
			{
				SDDevCamera*pCamera=new SDDevCamera(nDevIndex);

				if (0!=pCamera->Open(iter->second.nWidth,iter->second.nHeight,iter->second.nFrameRate,iter->second.bAutoFrameRate))
				{
					pCamera->Close();
					delete pCamera;
					pCamera=NULL;
					return -1;
				}

				iter->second.pSDDev=pCamera;
				pSDDev=pCamera;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			pSDDev=iter->second.pSDDev;
		}
	}
	else
	{
		unsigned int nDevCount=0;
		nDevCount=VIDEC_Camera::GetDeviceCount();
		if (nDevIndex<nDevCount)
		{
			SDDevCamera*pSDDevCamera=new SDDevCamera(nDevIndex);
			SDDevInst tSDDevInst;
			tSDDevInst.nDevIndex=nDevIndex;
			tSDDevInst.nFrameRate=DEFAULT_FRAME_RATE;
			tSDDevInst.nWidth=DEFAULT_WIDTH;
			tSDDevInst.nHeight=DEFAULT_HEIGHT;
			tSDDevInst.bAutoFrameRate=false;

			if (0!=pSDDevCamera->Open(tSDDevInst.nWidth,tSDDevInst.nHeight,tSDDevInst.nFrameRate,tSDDevInst.bAutoFrameRate))
			{
				pSDDevCamera->Close();
				delete pSDDevCamera;
				pSDDevCamera=NULL;
				return -1;
			}

			tSDDevInst.pSDDev=pSDDevCamera;
			g_MapSDDevInst[nDevIndex]=tSDDevInst;
			pSDDev=pSDDevCamera;
		}
		else
		{
			return -1;
		}
	}

	pSDDev->AddOutputStream(pOutputStream);

	return 0;
}

void SDDevMgr::RemoveOutputStream(unsigned int nDevIndex,VIDEC_OutputStream*pOutputStream)
{
	XAutoLock l(g_csMapSDDevInst);

	SDDev*pSDDev=NULL;

	MapSDDevInst::iterator iter=g_MapSDDevInst.find(nDevIndex);
	if (iter!=g_MapSDDevInst.end())
	{
		pSDDev=iter->second.pSDDev;

		if (pSDDev!=NULL)
		{
			pSDDev->RemoveOutputStream(pOutputStream);
			if (pSDDev->CanBeClosed())
			{
				iter->second.pSDDev=NULL;

				pSDDev->Close();
				delete pSDDev;
				pSDDev=NULL;
			}
		}
	}
}
