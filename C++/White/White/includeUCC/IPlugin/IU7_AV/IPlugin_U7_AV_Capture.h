// UCC7-����Ƶ�ɼ�����ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	typedef enum _CAP_TYPE
	{
		CAP_TYPE_AV		= 0,
		CAP_TYPE_SCREEN,
		CAP_TYPE_MEDIA,
		CAP_TYPE_COMPOUND,
		CAP_TYPE_IPCAM,
	}CAP_TYPE;

	typedef enum _VIDEC_CODEC
	{
		CODEC_H263P		= 0,
		CODEC_H264,
		CODEC_H264_SVC,
	}VIDEC_CODEC;

	class IPlugin_U7_AV_Capture_Observer
	{
	public:
		virtual ~IPlugin_U7_AV_Capture_Observer(){};

	public:
		//��Ƶ���ݻص�����
		virtual void OnCapChanAudioData(unsigned char*pData,int nLen,bool&bLoop){};
		virtual void OnCapChanAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
		//��Ƶ���ݻص�����
		//nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
		virtual void OnCapChanVideoData(unsigned char*pData,int nLen,int nKeyFrame){};

		virtual void OnCapChanFilePositionChanged(unsigned long ulSeconds){};
		virtual void OnCapChanFilePause(void){};
		virtual void OnCapChanFilePlay(void){};
		virtual void OnCapChanFileEnd(void){};
		virtual void OnCapChanRecvdCtrlData(void*pData,int nLen){};

		virtual void OnCapChanAudioIDChanged(unsigned long nAudioID){};
		virtual void OnCapChanVideoDevOpen(void){};

		virtual void OnCapChanCallbackFilePositionChanged(unsigned long ulSeconds){};
		virtual void OnCapChanCallbackFilePause(void){};
		virtual void OnCapChanCallbackFilePlay(void){};
		virtual void OnCapChanCallbackFileEnd(void){};
		virtual void OnCapChanCallbackRecvdCtrlData(void*pData,int nLen){};
	};

	class IPlugin_U7_AV_Capture
	{
	public:
		virtual ~IPlugin_U7_AV_Capture(){};

	public:
		virtual void AddObserver(IPlugin_U7_AV_Capture_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_AV_Capture_Observer* pObserver) = 0;

		virtual HWND GetHWnd(void) = 0;
		virtual int GetCardIndex(void) = 0;

		// ����Ƶ�豸
		virtual int OpenVideo(int nDevID, int nSizeIndex, int nVideoFrame, int nBitrate, int nBitrateCtrlType, VIDEC_CODEC CodecType, int nSub, int nQSub,bool bOpenOSD = true) = 0;
		//virtual int OpenVideo(int x, int y, int cx, int cy, int nBitCount, int nVideoFrame, int nBitrate, int nBitrateCtrlType, VIDEC_CODEC CodecType, int nSub, int nQSub) = 0;
		virtual int OpenVideo(const char*cszMediaFile, int nLoop, int nBitrate, int nBitrateCtrlType, VIDEC_CODEC CodecType, int nSub, int nQSub) = 0;
		//virtual int OpenVideo(int nScreenIndex, int nScreenType, int nWidth, int nHeight, int nVideoFrame, int nBitrate, int nBitrateCtrlType, VIDEC_CODEC CodecType, int nSub, int nQSub) = 0;
		//virtual int OpenVideo(int nSizeIndex,int nVideoFrame,int nBitrate,int nBitrateCtrlType,int nSub,int nQSub) = 0;
		// �ر���Ƶ
		virtual void CloseVideo(void) = 0;

		// ����Ƶ
		// nDevID:��Ƶ�ɼ��豸�ţ�-2�Ӳɼ����вɼ���Ƶ��>=-1�������вɼ���Ƶ
		virtual int OpenAudio(int nDevID) = 0;
		// �ر���Ƶ
		virtual void CloseAudio(void) = 0;

		virtual bool IsVideoOpen(void) = 0;
		virtual bool IsAudioOpen(void) = 0;

		// ǿ�����ñ���
		virtual void SetForceEncode(bool bForce) = 0;

		// ������Ƶ��ʾģʽ 0����� 1��ȫ��
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;

		// ������Ƶ����
		// ����ֵnBrightness[0-255 128] �Աȶ�nContrast[0-127 64] ���Ͷ�nSaturation[0-127 64] ɫ��nHue[0-255 0] ٤��nGamma[1-400 100] ��nSharpen[0-127 0]
		// nGammaR[1-400 100] nGammaG[1-400 100] nGammaB[1-400 100]
		virtual void SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,int nGammaR,int nGammaG,int nGammaB)=0;

		virtual unsigned long GetAudioID(void) = 0;
		virtual unsigned long GetVideoID(void) = 0;

		// ��ȡ��Ƶ�ֱ���
		virtual bool GetVideoSize(int& nVideoWidth, int& nVideoHeight) = 0;
		// ��ȡ��Ƶ֡��
		virtual int GetVideoFrame(void) = 0;
		/* ��ȡ����Ƶ�İ���ͳ������
		ulTotalPackets		-�ܹ������˶��ٸ���
		ulLostPackets		-�ܹ������˶��ٸ���
		AvgPacketLossRate	-ƽ��������
		CurPacketLossRate	-��ǰ������
		nBitrate			-������bps��*/
		virtual int GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;
		virtual int GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;

		//��������
		virtual void SetVolume(int nVolume) = 0;
		//ȡ��ǰ����ֵ
		virtual int GetLevel(void) = 0;

		//���浱ǰͼ��BMP�ļ�
		virtual bool CaptureToBMPFile(const char* cszFile) = 0;

		// ý�岥����ؽӿ�
		virtual unsigned long GetDuration(void) = 0;
		virtual unsigned long GetPosition(void) = 0;
		virtual void Seek(unsigned long ulSeconds) = 0;
		virtual void Pause(void) = 0;
		virtual void Play(void) = 0;
		virtual void SetMute(bool bMute = true) = 0;

		//����ߵ�
		virtual void SetInvert(int nInvert) = 0;
		virtual int GetInvert(void) = 0;

		virtual void SetChanName(const char*cszChanName) = 0;
		virtual void SetMemberName(const char*cszMemberName) = 0;
		virtual void ApplyOSD() = 0;
	};

	class IPlugin_U7_AV_CaptureMgr : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_AV_CaptureMgr(){};

	public:
		virtual IPlugin_U7_AV_Capture* CreateU7AVCapture(CAP_TYPE nType, int nCardIndex) = 0;
		virtual IPlugin_U7_AV_Capture* GetU7AVCapture(int nCardIndex) = 0;
		virtual IPlugin_U7_AV_Capture* GetU7AVCapture(HWND hWnd) = 0;
		virtual void DestroyU7AVCapture(int nCardIndex) = 0;
		virtual void DestroyU7AVCapture(void) = 0;

		//ȡ�豸����
		virtual int GetDevCount(void) = 0;
		//ȡ�豸����
		virtual int GetDevName(int nDevID,char*szName,int nMaxCount) = 0;

		//ȡ�豸����
		virtual int GetDevType(int nDevID,int&nDevType) = 0;

		//�г���ʽ�ĸ���
		virtual int GetFormatCount(int nDevType) = 0;
		//ȡ��ʽ����
		virtual int GetFormatName(int nDevType,int nIndex,char*szName,int nMaxCount) = 0;
		//ȡ��ʽ��С
		virtual int GetFormatSize(int nDevType,int nIndex,int&nWidth,int&nHeight) = 0;

		//ȡ��Ƶ�豸����
		virtual int GetMicDevCount(void) = 0;
		//ȡ��Ƶ�豸����
		virtual int GetMicDevName(int nDev,char desc[]) = 0;

		virtual int GetSpeakerNumDevs(void) = 0;

		//0:ok,<0:error
		virtual int GetSpeakerDevDesc(int dev,char desc[]) = 0;

		// ȡ��Ƶ����
		virtual const AUDIOSET_INFO& GetAudioSet(int nCardIndex) = 0;
		// ������Ƶ
		virtual void SetAudioSet(int nCardIndex, const AUDIOSET_INFO& audioset) = 0;

		// ȡ��Ƶ����
		virtual const VIDEOSET_INFO& GetVideoSet(int nCardIndex) = 0;
		// ������Ƶ
		virtual void SetVideoSet(int nCardIndex, const VIDEOSET_INFO& videoset) = 0;

		// ������Ƶ��ʾģʽ 0����� 1��ȫ��
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;
	};
}
