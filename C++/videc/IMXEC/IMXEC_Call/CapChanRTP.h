//CapChanRTP.h
#ifndef __CAPCHANRTP_H__
#define __CAPCHANRTP_H__

#include <string>
#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_CameraDev.h>
#include <VIDEC/VIDEC_AH2KDev.h>
#include <VIDEC/VIDEC_AH400Dev.h>
#include <VIDEC/VIDEC_AVIWriter.h>
#include <AUDEC/AUDEC_Engine.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <AUDEC/AUDEC_Header.h>
#include "RTPHeader.h"
#include "../IMXEC_Util/XAutoLock.h"

class CapChanRTP;
class CapChanRTPCallback
{
public:
	CapChanRTPCallback(void){};
	virtual~CapChanRTPCallback(void){};
public:
	virtual void OnCapChanRTPCallbackSendVideo(CapChanRTP*pCapChanRTP,void*pPacketData,int nPacketLen)=0;
	virtual void OnCapChanRTPCallbackSendAudio(CapChanRTP*pCapChanRTP,void*pPacketData,int nPacketLen)=0;
};

class CapChanRTP
	: public VIDEC_CapChanRTPCallback
	, public AUDEC_StreamOut
	, public AUDEC_RecordSamples
	, public VIDEC_StreamSink
{
public:
	CapChanRTP(CapChanRTPCallback&rCallback);
	virtual~CapChanRTP(void);
public:
	/******************************************************************************
	* StartRecordToAVI
	*��������ʼ¼��
	*���룺	cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordToAVI(const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordToAVI
	*������ֹͣ¼��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordToAVI(void);
protected:
	virtual void OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource){};
	virtual void OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource){};
	virtual void OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame=false,int nWidth=0,int nHeight=0,int nHeaderLen=0);
	virtual void OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen);
	virtual bool Write(const void*data,int len);
	virtual void OnRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);
protected:
	CapChanRTPCallback&		m_rCallback;
	XCritSec				m_csAVIWriter;
	VIDEC_AVIWriter*		m_pAVIWriter;
};

#endif
