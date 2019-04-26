#include "CapChanRTP.h"

CapChanRTP::CapChanRTP(CapChanRTPCallback&rCallback)
: m_rCallback(rCallback)
, m_pAVIWriter(NULL)
{

}

CapChanRTP::~CapChanRTP(void)
{
	StopRecordToAVI();
}
	
void CapChanRTP::OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)
{
	m_rCallback.OnCapChanRTPCallbackSendVideo(this,(void*)pPacketData,nPacketLen);
}

bool CapChanRTP::Write(const void*data,int len)
{
	m_rCallback.OnCapChanRTPCallbackSendAudio(this,(void*)data,len);

	return true;
}

void CapChanRTP::OnRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)
{
	XAutoLock l(m_csAVIWriter);
	if (m_pAVIWriter!=NULL)
	{
		m_pAVIWriter->WriteAudio((unsigned char*)pSamples,nSamples<<1,nSampleRate,nTimestamp);
	}
}

void CapChanRTP::OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
{
	XAutoLock l(m_csAVIWriter);
	if (m_pAVIWriter!=NULL)
	{
		m_pAVIWriter->WriteVideo(pData,nLen);
	}
}

/******************************************************************************
* StartRecordToAVI
*描述：开始录像
*输入：	cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int CapChanRTP::StartRecordToAVI(const char*cszPathName,int nStandardFormat)
{
	StopRecordToAVI();

	VIDEC_AVIWriter*pAVIWriter=VIDEC_AVIWriter::Create();
	if (pAVIWriter==NULL)
	{
		return -1;
	}
	if (0!=pAVIWriter->Open(cszPathName,nStandardFormat))
	{
		pAVIWriter->Close();
		delete pAVIWriter;
		pAVIWriter=NULL;

		return -1;
	}

	{
		XAutoLock l(m_csAVIWriter);
		m_pAVIWriter=pAVIWriter;
	}

	return 0;
}

/******************************************************************************
* StopRecordToAVI
*描述：停止录像
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void CapChanRTP::StopRecordToAVI(void)
{
	VIDEC_AVIWriter*pAVIWriter=m_pAVIWriter;

	{
		XAutoLock l(m_csAVIWriter);
		m_pAVIWriter=NULL;
	}

	if (pAVIWriter!=NULL)
	{
		pAVIWriter->Close();
		delete pAVIWriter;
		pAVIWriter=NULL;
	}
}
