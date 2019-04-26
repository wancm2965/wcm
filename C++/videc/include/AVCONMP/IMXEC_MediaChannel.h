//IMXEC_MediaChannel.h
#ifndef __IMXEC_MEDIACHANNEL_H__
#define __IMXEC_MEDIACHANNEL_H__

#include <AVCONMP/IMXEC_Export.h>
#include <AVCONMP/IMXEC_Call.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <NETEC/NETEC.h>

AVCONMP_API int AVCONMP_Init(int nScreenCount=4/*�ϳ�������*/);

AVCONMP_API void AVCONMP_Terminate(void);

class AVCONMP_CapChanCallback
{
public:
	AVCONMP_CapChanCallback(void){};
	virtual~AVCONMP_CapChanCallback(void){};
public:
	//��Ƶ���ݻص�����
	virtual void OnAVCONMP_CapChanCallbackAudioData(unsigned char*pData,int nLen,bool&bLoop){};
	//��Ƶ���ݻص�����nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
	virtual void OnAVCONMP_CapChanCallbackVideoData(unsigned char*pData,int nLen,int nKeyFrame){};
	virtual void OnAVCONMP_CapChanCallbackAudioIDChanged(unsigned long nAudioID){};
};

class AVCONMP_API AVCONMP_CapChanDev
{
public:
	AVCONMP_CapChanDev(void){};
	virtual~AVCONMP_CapChanDev(void){};
public:
	virtual int Open(int nScreenIndex)=0;
	virtual void Close(void)=0;
	virtual int GetScreenIndex(void)=0;
	virtual int SetScreenPos(int x,int y,int cx,int cy)=0;
	virtual int SetScreenWndCount(int nWndCount)=0;
	virtual int SetScreenWndPos(int nWndIndex,int x,int y,int cx,int cy)=0;
	virtual int OpenVideoDev(int nWidth,int nHeight,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub)=0;
	virtual int OpenAudio(X_AUDIO_CODEC_ID CodecType)=0;	
	virtual unsigned long GetAudioID(void)=0;
	virtual unsigned long GetVideoID(void)=0;
	
	virtual int SetForceEncodeVideo(int nForceEncodeVideo)=0;
	virtual int GetForceEncodeVideo(void) = 0;
	virtual void RequestKeyFrame(void)=0;
	
	/*����������������������ķֱ�����������С�������õ�ǰ�����ڵ���OpenVideoDev�ӿ�֮ǰ���ã�����OpenVideoDev�е�int nSub��int nQSub ����Ϊ1����������ô˽ӿ���������������������������1/4��1/9�ı�����С*/
	virtual int SetOtherCodec(int nSubWidth,int nSubHeight,int nSubBitrate,int nQSubWidth,int nQSubHeight,int nQSubBitrate)=0;

	static AVCONMP_CapChanDev*Create(AVCONMP_CapChanCallback&rCallback);
};


class AVCONMP_PlayChanCallback
{
public:
	AVCONMP_PlayChanCallback(void){};
	virtual~AVCONMP_PlayChanCallback(void){};
public:
	//��Ƶ���ݻص�����
	virtual void OnAVCONMP_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop)=0;
	virtual void OnAVCONMP_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)=0;
	//��Ƶ���ݻص�����
	//nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
	virtual void OnAVCONMP_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode)=0;

	//�޷�����Ļص���nHardwareError������0ʱ����ʾ���볬����Χ��nHardwareError����0ʱ����ʾ�ָ�������
	//�˺���Ϊ�߳��лص������ĺ�������������ʱ��תΪWindows��Ϣ
	virtual void OnAVCONMP_PlayChanCallbackHardwareError(int nHardwareError){};
};

class AVCONMP_API AVCONMP_PlayChan
{
public:
	AVCONMP_PlayChan(void){};
	virtual~AVCONMP_PlayChan(void){};
public:
	virtual int Open(unsigned long nAudChanID,unsigned long nVidChanID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU=0)=0;

	virtual int Open(void)=0;

	virtual void Close(void)=0;

	//ȡ��Ƶ���
	virtual int GetVideoWidth(void)=0;
	//ȡ��Ƶ�߶�
	virtual int GetVideoHeight(void)=0;

	/******************************************************************************
	* SetScreenWnd
	*����������������Ļ����λ��
	*���룺	nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;

	virtual int SetVolume(int nVolume)=0;
	virtual int GetLevel(int&nLevel)=0;

	virtual int StartAudio(void)=0;
	virtual int EnableAudio(int nEnable/*0:�����գ�1:����*/)=0;
	virtual int SetAudioID(unsigned long nAudioID)=0;
	virtual int StopAudio(void)=0;
	virtual int StartVideo(void)=0;
	virtual int EnableVideo(int nEnable/*0:�����գ�1:����*/)=0;
	virtual int StopVideo(void)=0;
	virtual int SetAudioCast(bool bAudioCast)=0;
	virtual int SetVideoCast(bool bVideoCast)=0;

	/******************************************************************************
	* RequestKeyFrame
	*����������ؼ�֡
	*���룺	��
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int RequestKeyFrame(void)=0;

	/******************************************************************************
	* SetFrameRateControlMode
	*����������֡�ʿ���ģʽ
	*���룺	nMode			-֡�ʿ���ģʽ
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetFrameRateControlMode(FRAME_RATE_CONTROL_MODE nMode)=0;

	/******************************************************************************
	* SetFrameTypeControlMode
	*����������֡���Ϳ���ģʽ
	*���룺	nMode			-֡���Ϳ���ģʽ
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE nMode)=0;

	/******************************************************************************
	* GetAudioPacketStatistics
	*��������ȡ��Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;

	/******************************************************************************
	* GetVideoPacketStatistics
	*��������ȡ��Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;

	/******************************************************************************
	* PlayAudioData
	*������������Ƶ����
	*���룺	pData			-����ָ��
			nLen			-���ݳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int PlayAudioData(const void*pData,int nLen)=0;

	/******************************************************************************
	* PlayVideoData
	*������������Ƶ����
	*���룺	pData			-����ָ��
			nLen			-���ݳ���
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int PlayVideoData(const void*pData,int nLen)=0;

	/******************************************************************************
	* SetSynFlag
	*������������Ƶ��Ƶͬ�����
	*���룺	nSynFlag		-��Ƶ��Ƶͬ����ǡ�0����Ƶ��Ƶ��ͬ������0����Ƶ��Ƶͬ����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSynFlag(int nSynFlag)=0;



	static AVCONMP_PlayChan*Create(AVCONMP_PlayChanCallback&rCallback);

	static unsigned long GetTotalBytes(void);
	static int GetBitrate(void);
	static void ClearTotalBytes(void);
	static void SetForcibleReceiving(bool bForcible);
	static bool GetForcibleReceiving(void);

	virtual int StartMixingAudio(X_AUDIO_CODEC_ID nCodecID)=0;
	virtual void StopMixingAudio(void)=0;
	virtual int GetMixingAudioID(unsigned long&nAudioID)=0;
};


#endif
