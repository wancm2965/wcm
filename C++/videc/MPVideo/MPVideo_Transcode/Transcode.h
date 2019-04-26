//Transcode.h
#ifndef __TRANSCODE_H__
#define __TRANSCODE_H__

#include <MPVideo/MPVideo_Transcode.h>
#include <MPVideo/MPVideo_Decoder.h>
#include <MPVideo/MPVideo_Encoder.h>

class Transcode
	: public MPVideo_Transcode
	, public MPVideo_DecoderStreamCallback
	, public MPVideo_EncoderStreamCallback
{
public:
	Transcode(MPVideo_TranscodeCallback&rCallback);
	virtual~Transcode(void);
public:
	virtual int Open(int nBitrate,int nBitrateControlType);
	virtual void Close(void);
	virtual int TranscodePacketData(void*pPacketData,int nPacketLen);
	virtual void RequestKeyFrame(void);
	virtual void SetSkipFrameCount(int nSkipFrameCount);
protected:
	virtual void OnMPVideo_DecoderStreamCallbackFrame(MPVideo_Image*pImage);
	virtual void OnMPVideo_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight);
	virtual void OnMPVideo_EncoderStreamCallbackRTPPacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight){};
protected:
	MPVideo_TranscodeCallback&	m_rCallback;
	MPVideo_DecoderStream*		m_pDecoderStream;
	MPVideo_EncoderStream*		m_pEncoderStream;
	int							m_nBitrate;
	int							m_nBitrateControlType;
	int							m_nQMin;
	int							m_nQMax;

	int							m_nSkipFrameCount;
	int							m_nSkipFrameIndex;
};

#endif
