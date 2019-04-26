#include "stdafx.h"
#include <string>
#include <list>
#include "DVCDev.h"
#include "DVCAPI.h"
#include <VIDEC/VIDEC_CodecDef.h>


typedef struct tagDVC_DEV
{
	std::string		strIP;
	std::string		strPassword;
	unsigned short  nPort;
}DVC_DEV;

typedef std::list<DVC_DEV>	MapDVCDev;

static MapDVCDev g_MapDVCDev;

int GetDevInfoByID(int nDevID,std::string&strIP,unsigned short&nPort,std::string&strPassword)
{
	int nIndex=0;
	MapDVCDev::iterator iter=g_MapDVCDev.begin();
	while (iter!=g_MapDVCDev.end())
	{
		if (nIndex==nDevID)
		{
			strIP=(*iter).strIP;
			nPort=(*iter).nPort;
			strPassword=(*iter).strPassword;
			return 0;
		}
		++iter;
		nIndex++;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DVCDev::DVCDev(void)
{
	m_video_cb=NULL;
	m_pUserData=NULL;
	m_pDVC=NULL;

	m_nFrameRate=30;
	m_nBitrate=1500;
	m_nBitrateControlType=0;
	m_nResolution=6;
}

DVCDev::~DVCDev(void)
{

}

int DVCDev::Open(int nDevID,DVC_VIDEO_CB video_cb,const void*pUserData)
{
	m_video_cb=video_cb;
	m_pUserData=pUserData;

	std::string		strIP;
	std::string		strPassword;
	unsigned short  nPort;

	if (0!=GetDevInfoByID(nDevID,strIP,nPort,strPassword))
	{
		return -1;
	}

	NETEC_DVC*pDVC=NETEC_DVC::Create(*this);
	if (pDVC)
	{
		if (0!=pDVC->Open(strIP.c_str(),nPort,strPassword.c_str()))
		{
			pDVC->Close();
			delete pDVC;
			pDVC=NULL;
		}
		else
		{
			XAutoLock l(m_csDVC);
			m_pDVC=pDVC;
			m_pDVC->SetVideoParams(VIDEC_CODEC_H264,m_nBitrate,m_nBitrateControlType,m_nResolution,m_nFrameRate,0);
		}
	}

	return 0;
}


void DVCDev::Close(void)
{
	NETEC_DVC*pDVC=m_pDVC;
	{
		XAutoLock l(m_csDVC);
		m_pDVC=NULL;
	}

	if (pDVC)
	{
		pDVC->Close();
		delete pDVC;
		pDVC=NULL;
	}
}

int DVCDev::SetParams(int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution)
{
	if (nFrameRate<=1)
		nFrameRate=1;
	else if (nFrameRate<=5)
		nFrameRate=5;
	else if (nFrameRate<=10)
		nFrameRate=10;
	else if (nFrameRate<=15)
		nFrameRate=15;
	else 
		nFrameRate=30;

	if (nBitrate<=32)
		nBitrate=32;
	else if (nBitrate<=64)
		nBitrate=64;
	else if (nBitrate<=128)
		nBitrate=128;
	else if (nBitrate<=256)
		nBitrate=256;
	else if (nBitrate<=384)
		nBitrate=384;
	else if (nBitrate<=512)
		nBitrate=512;
	else if (nBitrate<=768)
		nBitrate=768;
	else if (nBitrate<=1024)
		nBitrate=1024;
	else if (nBitrate<=1536)
		nBitrate=1536;
	else if (nBitrate<=2048)
		nBitrate=2048;
	else if (nBitrate<=3072)
		nBitrate=3072;
	else if (nBitrate<=4096)
		nBitrate=4096;
	else if (nBitrate<=3072)
		nBitrate=3072;
	else if (nBitrate<=5120)
		nBitrate=5120;
	else
		nBitrate=6144;

	if (nResolution<0)
		nResolution=0;
	else if (nResolution>7)
		nResolution=7;
	
	m_nFrameRate=nFrameRate;
	m_nBitrate=nBitrate;
	m_nBitrateControlType=nBitrateControlType;
	m_nResolution=nResolution;
	
	if (m_pDVC)
	{
		m_pDVC->SetVideoParams(VIDEC_CODEC_H264,m_nBitrate,m_nBitrateControlType,m_nResolution,m_nFrameRate,0);
	}

	return 0;
}

int DVCDev::GetStatus(void)
{
	//if (m_pDVC)
	//{
	//	switch (m_pDVC->GetErrorCode())
	//	{
	//	default:
	//		break;
	//	}
	//}
	return 0;
}

int DVCDev::RequestKeyFrame(void)
{
	if (m_pDVC)
	{
		m_pDVC->RequestKeyFrame();
	}
	return 0;
}

int DVCDev::SendData(const char*pData,int nLen)
{
	if (m_pDVC)
	{
		m_pDVC->SendData(pData,nLen);
	}
	return 0;
}

void DVCDev::OnNETEC_DVCCallbackVideoFrameData(unsigned char*pFrameData,int nFrameLen)
{
	if (m_video_cb)
		m_video_cb(m_pUserData,pFrameData,nFrameLen);
}

int DVC_SetIP(const char*cszIP,unsigned short nPort,const char*cszPassword)
{
	std::string strIP="";
	std::string strPassword="";
	if (cszIP)
		strIP=cszIP;

	if (cszPassword)
		strPassword=cszPassword;

	if (nPort && strIP.size())
	{
		DVC_DEV tDev;
		tDev.strIP=strIP;
		tDev.strPassword=strPassword;
		tDev.nPort=nPort;

		MapDVCDev::iterator iter;
		for (iter=g_MapDVCDev.begin();iter!=g_MapDVCDev.end();++iter)
		{
			if (strIP==(*iter).strIP)
			{
				(*iter).strIP=strIP;
				(*iter).nPort=nPort;
				(*iter).strPassword=strPassword;
				return 0;
			}
		}

		g_MapDVCDev.push_back(tDev);
	}
	else
	{
		MapDVCDev::iterator iter;
		for (iter=g_MapDVCDev.begin();iter!=g_MapDVCDev.end();++iter)
		{
			if (strIP==(*iter).strIP)
			{
				g_MapDVCDev.erase(iter);
				return 0;
			}
		}
	}

	return 0;
}


int DVC_GetDevCount(void)
{
	return g_MapDVCDev.size();
}

int DVC_GetDevName(int nDevID,char*szName,int nMaxCount)
{
	if (nDevID<0 || nDevID >= DVC_GetDevCount())
		return -1;

	char szTemp[128];
	size_t size;
	char szType[128]="";

	int nIndex=0;
	MapDVCDev::iterator iter;
	for (iter=g_MapDVCDev.begin();iter!=g_MapDVCDev.end();++iter)
	{
		if (nIndex==nDevID)
		{
			sprintf(szTemp, "DVC - %s",(*iter).strIP.c_str());
			break;
		}
		nIndex++;
	}

	size = strlen(szTemp) + 1;

	if (szName != NULL)
	{
		if (nMaxCount>=size)
		{
			strcpy(szName,szTemp);
		}
		else
		{
			strncpy(szName,szTemp,nMaxCount);
		}
	}

	return size;
}



void*DVC_Open(int nDevID,DVC_VIDEO_CB video_cb,const void*pUserData)
{
	DVCDev*pDVCDev=new DVCDev();
	if (0!=pDVCDev->Open(nDevID,video_cb,pUserData))
	{
		pDVCDev->Close();
		delete pDVCDev;
		pDVCDev=NULL;
	}
	return pDVCDev;
}

void DVC_Close(void*pHandle)
{
	DVCDev*pDVCDev=(DVCDev*)pHandle;
	if (pDVCDev)
	{
		pDVCDev->Close();
		delete pDVCDev;
		pDVCDev=NULL;
	}
}

int DVC_SetParams(void*pHandle,int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution)
{
	int nRet=-1;
	DVCDev*pDVCDev=(DVCDev*)pHandle;
	if (pDVCDev)
	{
		nRet=pDVCDev->SetParams(nFrameRate,nBitrate,nBitrateControlType,nResolution);
	}

	return nRet;
}

int DVC_GetStatus(void*pHandle)
{
	int nRet=-1;
	DVCDev*pDVCDev=(DVCDev*)pHandle;
	if (pDVCDev)
	{
		nRet=pDVCDev->GetStatus();
	}

	return nRet;
}

int DVC_RequestKeyFrame(void*pHandle)
{
	int nRet=-1;
	DVCDev*pDVCDev=(DVCDev*)pHandle;
	if (pDVCDev)
	{
		nRet=pDVCDev->RequestKeyFrame();
	}

	return nRet;
}

int DVC_SendData(void*pHandle,const char*pData,int nLen)
{
	int nRet=-1;
	DVCDev*pDVCDev=(DVCDev*)pHandle;
	if (pDVCDev)
	{
		nRet=pDVCDev->SendData(pData,nLen);
	}

	return nRet;
}
