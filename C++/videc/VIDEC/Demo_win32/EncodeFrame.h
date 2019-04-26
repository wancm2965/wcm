//
#ifndef __ENCODEFRAME_H__
#define __ENCODEFRAME_H__

#include "stdafx.h"
#include <string.h>
#include "XThreadBase.h"
#include "XCritSec.h"
#include "XListPtr.h"
#include "XAutoLock.h"
#include <NETEC_Node.h>
#include <NETEC_Core.h>
#include <NETEC_MediaSender.h>
#include "DlgVideoParam.h"
#include <VIDEC_Header.h>
#include "../../VIDEC\VIDEC_Codecs\AVCONH264Header.h"
#include <HPAVC_Capture.h>
#include "../HPAVC/HPAVC_Common/include/sample_utils.h"
#include "VIDEC_Codec.h"

class EncodeFrame
	: public XThreadBase
	, public NETEC_MediaSenderCallback
	//, public HPAVC_CaptureCallback
	, public VIDEC_EncoderCallback
{
public:
	EncodeFrame(void);
	virtual~EncodeFrame(void);
public:
	void Open(int nFrameRate,int nBitrate);
	void Close(void);

	virtual void ThreadProcMain(void);
	virtual void OnHPAVC_CaptureCallbackPacketData(void*pData,int nLen,int nKeyFrame,int nWidth,int nHeight);

	virtual void OnVIDEC_EncoderCallbackFramePacket(void*pPacketData,int nPacketLen,bool bHardwareEncoding);
	virtual void OnVIDEC_EncoderCallbackRTPPacket(void*pPacketData,int nPacketSize){};

	void GetAVParameters(unsigned long& ulAudioID,unsigned long& ulVideoID);
	void EncodeData(unsigned char*pData,int nLen);
	void EncodeImage(VIDEC_Image* pImage);
	void SetVideoParam(Video_Param param);

	void ReSetFrameRate(void);
	int GetVideoSendFrameRate(void);
public:
	int						m_nEncoderType;

protected:
	XListPtr				m_listImage;
	XCritSec				m_csListImage;
	bool					m_bWaitStop;

	unsigned char*			m_pPacketBuf;
	int						m_nFrameRate;
	int						m_nBitrate;
	unsigned long			m_nSeqNoRemoteMain;

	VIDEC_Encoder*			m_pEncoderFrame;
	//HPAVC_Capture*			m_pHPAVC_Capture;

	NETEC_MediaSender*		m_pIAVSender;
	unsigned long			m_nAudChanID;
	unsigned long			m_nVidChanID;
	std::string				m_strNoidID;
	std::string				m_strMCUID;
	std::string				m_strMCUIP;

	int						m_nSendFrameRate;
	unsigned long			m_ulLastCountFrameTimestamp;
	unsigned long			m_nSendFrameRateCount;

};	


#endif