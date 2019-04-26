#include "H264Player.h"
//#include "../VIDEC_Codecs/VIDECUDAWrapper.h"
#include "../VIDEC_Codecs/VIDECYNCWrapper.h"
#include "XUtil.h"

int H264SupportPlayer(void)
{
	return SyncSupportPlayer();
	//return (SyncSupportPlayer() || cudaSupportPlayer());
}

int H264Init(void)
{
	int nRet=0;
	SyncInit();
	//cudaInit();

	return nRet;
}

void H264Terminate(void)
{
	SyncTerminate();
	//cudaTerminate();
}

void H264PlayerSetMaxCount(unsigned int nMaxPlayer)
{
	//if (cudaSupportPlayer())
	//{
	//	cudaPlayerSetMaxCount(nMaxPlayer);
	//	SyncPlayerSetMaxCount(0);
	//}
	//else 
	if (SyncSupportPlayer())
	{
		SyncPlayerSetMaxCount(nMaxPlayer);
	}
}

int H264PlayerTry(void*hPlayer,unsigned long nWidth,unsigned long nHeight)
{
	//if (cudaSupportPlayer())
	//{
	//	if (0==cudaPlayerTry(hPlayer,nWidth,nHeight))
	//	{
	//		return 0;
	//	}
	//}
	if (SyncSupportPlayer())
	{
		return SyncPlayerTry(hPlayer,nWidth,nHeight);
	}
	return -1;
}



H264Player::H264Player(void)
{
	m_hPlayer=NULL;
	m_bCUDA=true;
}

H264Player::~H264Player(void)
{

}

int H264Player::Open(YNCD3D_CB_DC cbdc,void*pUserData,HWND hWnd, int nWidth, int nHeight)
{
	//if (cudaSupportPlayer())
	//{
	//	m_hPlayer=cudaPlayerOpen(hWnd,nWidth,nHeight);
	//	if (m_hPlayer!=NULL)
	//	{
	//		m_bCUDA=true;
	//		return 0;
	//	}
	//}

	m_bCUDA=false;
	if (SyncSupportPlayer())
	{
		m_hPlayer = SyncPlayerOpen(cbdc,pUserData,hWnd,nWidth,nHeight);
		if (m_hPlayer!=NULL)
		{
			return 0;
		}
	}

	return -1;
}

void H264Player::Close(void)
{
	if (m_hPlayer)
	{
		//if (m_bCUDA)
		//{
		//	cudaPlayerClose(m_hPlayer);
		//}
		//else
		{
			SyncPlayerClose(m_hPlayer);
		}
		m_hPlayer=NULL;
	}
}

int H264Player::PlayFrame(const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int&nFlagReset)
{
	if (!m_hPlayer)
	{
		return -1;
	}

	//if (m_bCUDA)
	//{
	//	return cudaPlayerPlayFrame(m_hPlayer, pFrameData, nFrameLen, nFrameRate,nFlagReset);
	//}

	return SyncPlayerPlayFrame(m_hPlayer, pFrameData, nFrameLen, nFrameRate,nFlagReset);
}

int H264Player::GetWidth(void)
{
	if (!m_hPlayer)
	{
		return 1280;
	}

	//if (m_bCUDA)
	//{
	//	return cudaPlayerGetWidth(m_hPlayer);
	//}

	return SyncPlayerGetWidth(m_hPlayer);
}

int H264Player::GetHeight(void)
{
	if (!m_hPlayer)
	{
		return 720;
	}

	//if (m_bCUDA)
	//{
	//	return cudaPlayerGetHeight(m_hPlayer);
	//}

	return SyncPlayerGetHeight(m_hPlayer);
}

int H264Player::Resize(void)
{
	if (!m_hPlayer)
	{
		return -1;
	}

	//if (m_bCUDA)
	//{
	//	return cudaPlayerResize(m_hPlayer);
	//}

	return SyncPlayerResize(m_hPlayer);
}

int H264Player::GetFrameRate(void)
{
	if (!m_hPlayer)
	{
		return 0;
	}
	return SyncPlayerGetFrameRate(m_hPlayer);
}

int H264Player::GetShowType(void)
{
	if (!m_hPlayer)
	{
		return 0;
	}
	return SyncPlayerGetShowType(m_hPlayer);
}

int H264Player::SkipDisplay(int nFlagSkipDisplay)
{
	if (!m_hPlayer)
	{
		return -1;
	}

	//if (m_bCUDA)
	//{
	//	return cudaPlayerSkipDisplay(m_hPlayer,nFlagSkipDisplay);
	//}

	return SyncPlayerSkipDisplay(m_hPlayer,nFlagSkipDisplay);
}

void*H264Player::GetPlayerHandle(void)
{
	return m_hPlayer;
}

bool H264Player::CaptureToBMPFile(const char*cszFile)
{
	if (!m_hPlayer)
	{
		return false;
	}

	return SyncPlayerCaptureToBMPFile(m_hPlayer,cszFile);
}