//PlayChanRTP.h
#ifndef __PLAYCHANRTP_H__
#define __PLAYCHANRTP_H__

#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_AVIWriter.h>
#include <AUDEC/AUDEC_Engine.h>
#include "../IMXEC_Util/XAutoLock.h"


class PlayChanRTP;
class PlayChanRTPCallback
{
public:
	PlayChanRTPCallback(void){};
	virtual~PlayChanRTPCallback(void){};
public:
	virtual void OnPlayChanRTPCallbackVideoPacket(PlayChanRTP*pPlayChanRTP,void*pPacketData,int nPacketLen)=0;
};

class PlayChanRTP
	: public VIDEC_StreamSink
	, public AUDEC_RecordSamples
{
public:
	PlayChanRTP(PlayChanRTPCallback&rCallback);
	virtual~PlayChanRTP(void);
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
	virtual void OnRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);
protected:
	PlayChanRTPCallback&	m_rCallback;
	XCritSec				m_csAVIWriter;
	VIDEC_AVIWriter*		m_pAVIWriter;
};

#endif
