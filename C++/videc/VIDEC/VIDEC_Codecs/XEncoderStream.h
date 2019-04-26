//XEncoderStream.h
#ifndef __XENCODERSTREAM_H__
#define __XENCODERSTREAM_H__

#include "VIDEC_Codec.h"
#include "VIDEC_Encoder.h"
#include "XAutoLock.h"
#include "XListPtr.h"
#include "XThreadBase.h"
#include "VIDEC_Header.h"



class XEncoderStream
	: public VIDEC_EncoderStream
	, public VIDEC_EncoderCallback
	, public XThreadBase
{
public:
	XEncoderStream(VIDEC_EncoderStreamCallback&rCallback,bool bRTPPacket);
	virtual~XEncoderStream(void);
public:
	int Open(void);
	void Close(void);
	int EncodeFrame(VIDEC_Image*pImage,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType,VIDEC_CODEC_TYPE codecType);
	void RequestKeyFrame(void);
	int  GetEncoderType(void);
protected:
	int DoEncodeFrame(VIDEC_Image*pImage,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType,VIDEC_CODEC_TYPE codecType);
	
	void OnVIDEC_EncoderCallbackFramePacket(void*pPacketData,int nPacketLen,bool bHardwareEncoding);
	void OnVIDEC_EncoderCallbackRTPPacket(void*pPacketData,int nPacketLen);

	void ThreadProcMain(void);

protected:
	typedef struct FRAME{
		FRAME(VIDEC_Image*pImage,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType,VIDEC_CODEC_TYPE codecType)
			:m_pImage(pImage)

			,m_nBitrate(nBitrate)
			,m_nMinQP(nMinQP)
			,m_nMaxQP(nMaxQP)
			,m_nBitrateControlType(nBitrateControlType)
			,m_codecType(codecType)
		{
			if (m_pImage!=NULL)
			{
				m_pImage->AddRef();
			}
		}
		~FRAME(void)
		{
			if (m_pImage!=NULL)
			{
				m_pImage->Release();
				m_pImage=NULL;
			}
		}

		int					m_nBitrate;
		int					m_nMinQP;
		int					m_nMaxQP;
		int					m_nBitrateControlType;

		VIDEC_CODEC_TYPE	m_codecType;
		VIDEC_Image*		m_pImage;
	}FRAME,*PFRAME;
protected:
	VIDEC_EncoderStreamCallback&	m_rCallback;

	int								m_nWidth;
	int								m_nHeight;
	VIDEC_CODEC_TYPE				m_codecType;
	int								m_nVirtualWidth;
	int								m_nVirtualHeight;
	bool							m_bDoubleField;

	int								m_nFrameRate;
	int								m_nEncodeErrorCount;
	int								m_nBitrate;
	int								m_nMinQP;
	int								m_nMaxQP;
	int								m_nBitrateControlType;

	bool							m_bRequestKeyFrame;
	int								m_nNetworkBitrate;

	VIDEC_Encoder*					m_pEncoderFrame;

	XListPtr						m_ListFrames;
	XCritSec						m_csListFrames;

	bool							m_bRunning;
	bool							m_bRTPPacket;

#ifdef _DEMO
	unsigned long					m_ulFirstTimestamp;
#endif
	int								m_nEncoderType;
	unsigned long					m_ulLastQKeyFrameTime;
};


#endif