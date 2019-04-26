#include "stdafx.h"

#include "DH264Player.h"
#include "XAutoLock.h"


#include "../VIDEC_DH264/dhplay.h"
#pragma comment(lib, "../VIDEC_DH264/dhplay.lib")



#define DH264_DECODER_BASE_INDEX		50
#define DH264_DECODER_MAX_COUNT		50

static DH264Player*	g_apDH264Player[DH264_DECODER_MAX_COUNT];
static XCritSec		g_csArrayDH264Player;

void CALLBACK DH264PlayerCallback(long nPort,char * pBuf,long nSize, FRAME_INFO * pFrameInfo,long nReserved1,long nReserved2);

class DH264PlayerManager
{
public:
	DH264PlayerManager(void)
	{
		XAutoLock l(g_csArrayDH264Player);
		for (int i=0;i<DH264_DECODER_MAX_COUNT;i++)
		{
			g_apDH264Player[i]=NULL;
		}
	};
	virtual~DH264PlayerManager(void)
	{
	}
public:
	static DH264Player*CreateDH264Player(YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser)
	{
		XAutoLock l(g_csArrayDH264Player);
		for (int i=0;i<DH264_DECODER_MAX_COUNT;i++)
		{
			if (g_apDH264Player[i]==NULL)
			{
				DH264Player*pDH264Player=new DH264Player(i,yuv_image_cb,pUser);
				g_apDH264Player[i]=pDH264Player;
				return pDH264Player;
			}
		}

		return NULL;
	}
	static void FreeDH264Player(DH264Player*pDH264Player)
	{
		XAutoLock l(g_csArrayDH264Player);
		int nIndex=pDH264Player->GetIndex();
		if (g_apDH264Player[nIndex]==pDH264Player)
		{
			g_apDH264Player[nIndex]=NULL;
		}
	}
};

static DH264PlayerManager g_DH264PlayerManager;


DH264Player::DH264Player(int nIndex,YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser)
:m_nIndex(nIndex+DH264_DECODER_BASE_INDEX)
,m_yuv_image_cb(yuv_image_cb)
,m_pUser(pUser)
,m_nLastTimestamp(0)
,m_nLastFrameRate(25)
{
	m_nFrameInterval=1000/m_nLastFrameRate;
}

DH264Player::~DH264Player(void)
{
	DH264PlayerManager::FreeDH264Player(this);
}

int DH264Player::Open(void)
{
	::PLAY_OpenStreamEx(m_nIndex,NULL,0,2048000);
	::PLAY_SetDecCallBack(m_nIndex,DH264PlayerCallback);

	::PLAY_SetStreamOpenMode(m_nIndex,STREAME_REALTIME);
	::PLAY_Play(m_nIndex,NULL);

	return 0;
}

void DH264Player::Close(void)
{
	::PLAY_Stop(m_nIndex);
	::PLAY_CloseStreamEx(m_nIndex);
}

int DH264Player::DecodePacket(void*pPacketData,int nPacketSize)
{
	int nRet=::PLAY_InputVideoData(m_nIndex,(unsigned char*)pPacketData,nPacketSize);
	return 0;
}

int DH264Player::GetIndex(void)
{
	return m_nIndex-DH264_DECODER_BASE_INDEX;
}

void DH264Player::OnImageData(char*pData,long nLen,int nWidth,int nHeight,long nFrameRate)
{
	if (nFrameRate!=m_nLastFrameRate)
	{
		m_nLastFrameRate=nFrameRate;
		if (m_nLastFrameRate<=0)
			m_nLastFrameRate=25;
		m_nFrameInterval=1000/m_nLastFrameRate;
	}

	m_nLastTimestamp+=m_nFrameInterval;

	m_yuv_image_cb(m_pUser,(unsigned char*)pData,(int)nLen,nWidth,nHeight,m_nLastTimestamp);
}

void CALLBACK DH264PlayerCallback(long nPort,char * pBuf,long nSize, FRAME_INFO * pFrameInfo,long nReserved1,long nReserved2)
{
	XAutoLock l(g_csArrayDH264Player);
	long ulDecoderIndex=nPort-DH264_DECODER_BASE_INDEX;
	if (g_apDH264Player[ulDecoderIndex])
	{
		if(pFrameInfo->nType == T_YV12 && nSize>0)
		{
			//char szDebug[1024];
			//static long nStaticStamp=0;
			//sprintf(szDebug,"nStamp = %d Delta=%d\n",pFrameInfo->nStamp,pFrameInfo->nStamp-nStaticStamp);
			//nStaticStamp=pFrameInfo->nStamp;
			//OutputDebugString(szDebug);

			g_apDH264Player[ulDecoderIndex]->OnImageData(pBuf,nSize,pFrameInfo->nWidth,pFrameInfo->nHeight,pFrameInfo->nFrameRate);
		}
	}
}

VIDEC_DH264PLAYER_API void* VIDEC_DH264PlayerOpen(YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser)
{
	DH264Player*pDH264Player=DH264PlayerManager::CreateDH264Player(yuv_image_cb,pUser);
	if (pDH264Player!=NULL)
	{
		if (0!=pDH264Player->Open())
		{
			pDH264Player->Close();
			delete pDH264Player;
			pDH264Player=NULL;
		}
	}
	return pDH264Player;
}

VIDEC_DH264PLAYER_API void VIDEC_DH264PlayerInputData(void*pHandle,unsigned char*pData,int nLen)
{
	DH264Player*pDH264Player=(DH264Player*)pHandle;
	if (pDH264Player!=NULL)
	{
		pDH264Player->DecodePacket(pData,nLen);
	}
}

VIDEC_DH264PLAYER_API void VIDEC_DH264PlayerClose(void*pHandle)
{
	DH264Player*pDH264Player=(DH264Player*)pHandle;
	if (pDH264Player!=NULL)
	{
		pDH264Player->Close();
		delete pDH264Player;
		pDH264Player=NULL;
	}
}


