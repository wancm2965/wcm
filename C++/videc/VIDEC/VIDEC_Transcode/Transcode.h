//Transcode.h
#ifndef __TRANSCODE_H__
#define __TRANSCODE_H__

#include <VIDEC/VIDEC_Transcode.h>
#include <VIDEC/VIDEC_Decoder.h>
#include <VIDEC/VIDEC_Encoder.h>

class Transcode
	: public VIDEC_Transcode
	, public VIDEC_DecoderStreamCallback
	, public VIDEC_EncoderStreamCallback
{
public:
	Transcode(VIDEC_TranscodeCallback&rCallback);
	virtual~Transcode(void);
public:
	virtual int Open(int nBitrate,int nBitrateControlType);
	virtual void Close(void);
	virtual int TranscodePacketData(void*pPacketData,int nPacketLen);
	virtual void RequestKeyFrame(void);
protected:
	virtual void OnVIDEC_DecoderStreamCallbackFrame(VIDEC_Image*pImage);
	virtual void OnVIDEC_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight);
	virtual void OnVIDEC_EncoderStreamCallbackRTPPacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight){};
protected:
	VIDEC_TranscodeCallback&	m_rCallback;
	VIDEC_DecoderStream*		m_pDecoderStream;
	VIDEC_EncoderStream*		m_pEncoderStream;
	int							m_nBitrate;
	int							m_nBitrateControlType;
	int							m_nQMin;
	int							m_nQMax;
};

#endif
