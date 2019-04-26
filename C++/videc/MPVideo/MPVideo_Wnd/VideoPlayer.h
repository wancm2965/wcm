//VideoPlayer.h
#ifndef __VIDEOPLAYER_H__
#define __VIDEOPLAYER_H__

#include <MPVideo/MPVideo_Codec.h>
#include "XAutoLock.h"
#include "XListPtr.h"

#include <MPVideo/MPVideo_Player.h>
#include "../MPVideo_Codecs/XDecoderStreamBase.h"
#include "../MPVideo_MP/ImageInput.h"

class VideoPlayer 
	: public XDecoderStreamBase
	, public MPVideo_Player
	, public ImageInput
{
public:
	VideoPlayer(MPVideo_PlayerCallback&rCallback);
	virtual ~VideoPlayer(void);
public:
	int Open(unsigned long*lpnVideoTimestamp,unsigned long*lpnAudioTimestamp);
	void Close(void);

	
	void SetSynFlag(int nSynFlag);

	int PlayData(void*pPacketData,int nPacketSize);
	void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen);

protected:
	virtual void OnDecoderStreamCallbackFrame(MPVideo_Image*pImage);
	bool IsPending(void);
	virtual void OnPreDecodingFrame(unsigned long nFrameTimestamp,unsigned long nPaintTimestamp);
	virtual int DoDecodePacket(void*pPacketData,int nPacketSize);

	virtual MPVideo_Image*GetImage(void);
protected:
	MPVideo_PlayerCallback&			m_rCallback;

	unsigned long*					m_lpnVideoTimestamp;
	unsigned long*					m_lpnAudioTimestamp;
	int								m_nSynFlag;
	unsigned long					m_nLastAudioTimestamp;
	unsigned long					m_nLastUpdateAudioTimestamp;
	unsigned long					m_nNewFrameTimestamp;

	unsigned long					m_nLastPaintedTimestamp;
	unsigned long					m_nLastFrameTimestamp;

	XCritSec						m_csListImage;
	XListPtr						m_listImage;
	MPVideo_Image*					m_pImage;
};

#endif
