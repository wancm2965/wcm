#ifndef __DH264PLAYER_H__
#define __DH264PLAYER_H__

#include "VIDEC_DH264Player.h"

class DH264Player 
{
public:
	DH264Player(int nIndex,YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser);
	virtual ~DH264Player(void);
public:
	int Open(void);
	void Close(void);
	int DecodePacket(void*pPacketData,int nPacketSize);

	int GetIndex(void);

	void OnImageData(char*pData,long nLen,int nWidth,int nHeight,long nFrameRate);
protected:
	int								m_nIndex;
	YUV_IMAGE_CALLBACK				m_yuv_image_cb;
	void*							m_pUser;

	unsigned long					m_nLastTimestamp;
	long							m_nLastFrameRate;
	unsigned long					m_nFrameInterval;
};


#endif