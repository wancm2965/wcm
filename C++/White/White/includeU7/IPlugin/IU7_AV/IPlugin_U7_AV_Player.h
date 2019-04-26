// UCC7-����Ƶ���Ź���ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "AVCONSys/AUDEC/AUDEC_CodecID.h"

namespace AVCONPlugin
{
	class IPlugin_U7_AV_Player_Observer
	{
	public:
		virtual ~IPlugin_U7_AV_Player_Observer(){};

	public:
		//��Ƶ���ݻص�����
		virtual void OnPlayChanAudioData(unsigned char*pData,int nLen,bool&bLoop){};
		virtual void OnPlayChanAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
		//��Ƶ���ݻص�����
		//nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
		virtual void OnPlayChanVideoData(unsigned char*pData,int nLen,int nKeyFrame){};

		virtual void OnPlayChanRecordStart(){};
	};

	class IPlugin_U7_AV_Player
	{
	public:
		virtual ~IPlugin_U7_AV_Player(){};

	public:
		virtual void AddObserver(IPlugin_U7_AV_Player_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_AV_Player_Observer* pObserver) = 0;

		virtual HWND GetHWnd(void) = 0;
		virtual const char* GetMemberID(void) = 0;
		virtual int GetCardIndex(void) = 0;

		virtual int Open(bool bLocalPlay) = 0;
		virtual int Open(unsigned long nAudChanID, unsigned long nVidChanID, const char*cszPeerNodeID, 
			const char*cszPeerNATIP, unsigned short nPeerNATPort, const char*cszPeerLocalIP, unsigned short nPeerLocalPort, 
			const char*cszPeerMCUID, const char*cszPeerMCUIP, unsigned short nPeerMCUPort, int nForceUseMCU = 0) = 0;
		virtual void Close(void) = 0;

		virtual void RecvAudio(bool bRecv) = 0;
		virtual void RecvVideo(bool bRecv) = 0;

		virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;

		virtual int SetVolume(int nVolume) = 0;
		virtual int GetAudioLevel(void) = 0;

		// ֡�ʿ��� 0��ȫ֡�� 1����֡�� 2���Զ�֡��
		virtual void SetFrameRateControlMode(int nMode) = 0;
		// ��ȡ��ǰ֡��
		virtual int GetFrameRateControlMode() = 0;

		// �������� 0������ 1������ 2������
		virtual void SetFrameTypeControlMode(int nMode) = 0;

		// ��ȡ��ǰ����
		virtual int GetFrameTypeControlMode() = 0;

		// ������Ƶ����
		// ����ֵnBrightness[0-255 128] �Աȶ�nContrast[0-127 64] ���Ͷ�nSaturation[0-127 64] ɫ��nHue[0-255 0] ٤��nGamma[1-400 100] ��nSharpen[0-127 0]
		// �񻯽�Ӧ�õ�����bSharpenLocalOnly[Ĭ��ֵΪtrue]
		// nGammaR[1-400 100] nGammaG[1-400 100] nGammaB[1-400 100]
		// �����ȵ���bAjustAmp[Ĭ��ֵΪfalse] Ϊtrueʱ����ԭ��ֵ�Ļ����ϼ��ϴ���Ĳ���ֵ
		virtual void SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly,
			int nGammaR,int nGammaG,int nGammaB,bool bAjustAmp) = 0;
		virtual void GetVideoParam(int& nBrightness, int& nContrast, int& nSaturation, int& nHue, int& nGamma,int& nSharpen, bool& bSharpenLocalOnly,
			int& nGammaR, int& nGammaG, int& nGammaB) = 0;

		virtual void PlayAudioData(const void* pData, int nLen) = 0;
		virtual void PlayVideoData(const void* pData, int nLen) = 0;

		// ������Ƶ��ʾģʽ 0����� 1��ȫ��
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;
		// ��ȡ��Ƶ��ʾģʽ 0����� 1��ȫ��
		virtual int GetVideoShowMode(void) = 0;

		//���浱ǰͼ��BMP�ļ�
		virtual bool CaptureToBMPFile(const char* cszFile) = 0;

		// ����ؼ�֡
		virtual void RequestKeyFrame() = 0;

		// ¼��֧�ָ�ʽ��avms��avi��mkv��mp4
		virtual bool StartRecord(const char* cszPathName) = 0;
		virtual void StopRecord(void) = 0;

		// ��ȡ��Ƶ�ֱ���
		virtual bool GetVideoSize(int& nVideoWidth, int& nVideoHeight) = 0;

		/* ��ȡ����Ƶ�İ���ͳ������
		ulTotalPackets		-�ܹ������˶��ٸ���
		ulLostPackets		-�ܹ������˶��ٸ���
		AvgPacketLossRate	-ƽ��������
		CurPacketLossRate	-��ǰ������
		nBitrate			-������bps��*/
		virtual int GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate) = 0;
		virtual int GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate) = 0;

		//���������л�
		virtual void SetSreamLocked(bool bStreamLocked) = 0;
		virtual bool GetStreamLocked() =0;

		// ��ȡ������Ϣ
		virtual bool GetMemberCardInfo(unsigned long &audiocid, unsigned long &videocid, std::string &nodeid, std::string &nataddr, std::string &localaddr, unsigned short &localport, std::string &mcuid, std::string &mcuaddr, unsigned short &mcuport) = 0;
		virtual bool GetMonChannelInfo(unsigned long &audiocid, unsigned long &videocid, std::string &nodeid, std::string &nataddr, std::string &localaddr, unsigned short &localport, std::string &mcuid, std::string &mcuaddr, unsigned short &mcuport) = 0;

		virtual int StartMixingAudio(X_AUDIO_CODEC_ID nCodecID)=0;
		virtual void StopMixingAudio(void)=0;
		virtual int GetMixingAudioID(unsigned long&nAudioID)=0;
		//�����Ƶͨ��ID
		virtual unsigned long GetAudioID(void) = 0;
		//���Ϳ�������
		virtual void SendCtrlData(void* pData,int nLen) = 0;
		//�Ƿ�����򿪽�������Ƶ
		virtual void SetAllowRecvAV(bool bAllow) = 0;
	};

	class IPlugin_U7_AV_PlayerMgr : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_AV_PlayerMgr(){};

	public:
		virtual IPlugin_U7_AV_Player* CreateU7AVPlayer(const char* strMemberID, int nCardIndex) = 0;
		virtual IPlugin_U7_AV_Player* GetU7AVPlayer(const char* strMemberID, int nCardIndex) = 0;
		virtual IPlugin_U7_AV_Player* GetU7AVPlayer(HWND hWnd) = 0;
		virtual void DestroyU7AVPlayer(const char* strMemberID, int nCardIndex) = 0;
		virtual void DestroyU7AVPlayer(void) = 0;

		// ֡�ʿ��� 0��ȫ֡�� 1����֡�� 2���Զ�֡��
		virtual void SetFrameRateControlMode(int nMode) = 0;
		// �������� 0������ 1������ 2������
		virtual void SetFrameTypeControlMode(int nMode) = 0;

		// ������Ƶ��ʾģʽ 0����� 1��ȫ��
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;
		// ��ȡ��Ƶ��ʾģʽ 0����� 1��ȫ��
		virtual int GetVideoShowMode(void) = 0;

		// ����\ֹͣ ����¼��ʱ����
		virtual void SetConferenceRecording(int nFlagConferenceRecording) = 0;

		// ��������Player��Ƶ����
		// ����ֵnBrightness[0-255 128] �Աȶ�nContrast[0-127 64] ���Ͷ�nSaturation[0-127 64] ɫ��nHue[0-255 0] ٤��nGamma[1-400 100] ��nSharpen[0-127 0]
		// �񻯽�Ӧ�õ�����bSharpenLocalOnly[Ĭ��ֵΪtrue]
		// nGammaR[1-400 100] nGammaG[1-400 100] nGammaB[1-400 100]
		// �����ȵ���bAjustAmp[Ĭ��ֵΪfalse] Ϊtrueʱ����ԭ��ֵ�Ļ����ϼ��ϴ���Ĳ���ֵ
		virtual void SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly,
			int nGammaR,int nGammaG,int nGammaB,bool bAjustAmp) = 0;

		// ��ȡ��ǰ���������ܴ���
		virtual unsigned long GetDownloadTotalBitrate(void) = 0;

		// ǿ�ƽ�����Ƶ
		virtual void SetForcibleReceiving(bool bForcible) = 0;

		virtual int SetScreenWndPos(int nScreenIndex,int nWndIndex,int x,int y,int cx,int cy) = 0;

		virtual void ChangeOwner(HWND hWnd, const char* strMemberID, int nCardIndex) = 0;

		//���ܣ�������ѭ����
		virtual void SetRRParamter(const char* cmd, const char* strParam) = 0;
	};
}
