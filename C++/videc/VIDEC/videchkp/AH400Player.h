#ifndef __AH400PLAYER_H__
#define __AH400PLAYER_H__

#include "VIDEC_AH400Player.h"



class AH400Player 
{
public:
	AH400Player(int nIndex,YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser);
	virtual ~AH400Player(void);
public:
	int Open(HIK_TYPE type);
	void Close(void);
	int DecodePacket(void*pPacketData,int nPacketSize);

	int GetIndex(void);

	void OnImageData(char*pData,long nLen,int nWidth,int nHeight,long nStamp);
protected:
	int								m_nIndex;
	YUV_IMAGE_CALLBACK				m_yuv_image_cb;
	void*							m_pUser;
};


#endif