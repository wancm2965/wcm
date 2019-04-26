#ifndef __AH3KDEV_H__
#define __AH3KDEV_H__

#include "AH3KAPI.h"
#include <windows.h>
#include <winioctl.h>

#include "AH3K.h"
#include "XThreadBase.h"

#include <setupapi.h>
#pragma comment(lib,"setupapi.lib")

class AH3KDev  
	: public XThreadBase
{
public:
	AH3KDev(void);
	virtual ~AH3KDev(void);
public:
	int Open(int nDevID,AH3K_VIDEO_CB video_cb,AH3K_AUDIO_CB audio_cb,const void*pUserData);
	void Close(void);
	int SetParams(int nScaleType,int nWidth,int nHeight,int nFrameRate);
	void OnCapturedAudio(short*pSamples,int nSamples,int nSampleRate);
protected:
	void ThreadProcMain(void);
protected:
	bool			m_bRunning;
	OVERLAPPED		m_Overlapped;
	unsigned char*	m_pBuffer;
	int				m_nBufLen;
	int				m_nDevID;
	int				m_nChanID;


	unsigned char	m_nScaleType;	//0:F 1:I 2:Q



	int				m_nChannelIndex;
	int				m_nChannelType;

	HANDLE			m_hDevice;
	AH3K_VIDEO_CB	m_video_cb;
	AH3K_AUDIO_CB	m_audio_cb;
	const void*		m_pUserData;

	int				m_nPhyicalDevID;
};



#endif