//VIDECStreamDecoder.h
#ifndef __VIDECSTREAMDECODER_H__
#define __VIDECSTREAMDECODER_H__

#include "VIDEC_Codec.h"
#include "VIDEC_Decoder.h"
#include "VIDEC_StreamDecoder.h"
#include "VIDEC_convert.h"
#include "../VIDEC_API/XImage.h"


class VIDECStreamDecoder 
	: public VIDEC_StreamDecoder
	, public VIDEC_DecoderCallback
{
public:
	VIDECStreamDecoder(void);
	virtual ~VIDECStreamDecoder(void);
public:
	int Open(void);
	void Close(void);
	int Decode(void*pPacketData,int nPacketSize,void**ppYUVData,int&nWidth,int&nHeight);
protected:
	void OnVIDEC_DecoderCallbackFrame(VIDEC_Decoder*pDecoderFrame,VIDEC_Image*pImage);
protected:
	VIDEC_CODEC_TYPE				m_codecType;

	bool							m_bDoubleField;
	unsigned short					m_nVirtualWidth;
	unsigned short					m_nVirtualHeight;

	unsigned short					m_usLastSeqNoDecode;
	bool							m_bGotLastSeqNoDecode;
	bool							m_bEvenFieldFirst;


	unsigned short					m_nPacketWidth;
	unsigned short					m_nPacketHeight;

	unsigned long					m_ulFrameTimestamp;


	bool							m_bGotEvenKeyFrameDecode;
	bool							m_bGotEvenKeyFrame0;
	bool							m_bGotEvenKeyFrame1;
	bool							m_bGotEvenKeyFrame2;
	bool							m_bGotEvenKeyFrame3;


#ifdef _DEMO
	unsigned long					m_ulFirstTimestamp;
#endif

	VIDEC_Resize*					m_pResize;
	unsigned char*					m_pResizedBuf;
	int								m_nResizedWidth;
	int								m_nResizedHeight;

	XImage*							m_pXImage;

	bool							m_bGotKeyFrame;
	VIDEC_Decoder*				m_pDecoderFrame;
};

#endif
