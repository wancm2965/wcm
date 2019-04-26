// UCC7-����Ƶ���Ź���ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7_AV_Player_Observer
	{
	public:
		virtual ~IPlugin_U7_AV_Player_Observer(){};

	public:
		//��Ƶ���ݻص�����
		virtual void OnPlayChanAudioData(unsigned char*pData,int nLen,bool&bLoop) = 0;
		virtual void OnPlayChanAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp) = 0;
		//��Ƶ���ݻص�����
		//nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
		virtual void OnPlayChanVideoData(unsigned char*pData,int nLen,int nKeyFrame) = 0;
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
		virtual void Close(void) = 0;

		virtual void RecvAudio(bool bRecv) = 0;
		virtual void RecvVideo(bool bRecv) = 0;

		virtual int GetAudioLevel(void) = 0;

		// ֡�ʿ��� 0��ȫ֡�� 1����֡�� 2���Զ�֡��
		virtual void SetFrameRateControlMode(int nMode) = 0;
		// �������� 0������ 1������ 2������
		virtual void SetFrameTypeControlMode(int nMode) = 0;

		// ������Ƶ����
		// ����ֵnBrightness[0-255 128] �Աȶ�nContrast[0-127 64] ���Ͷ�nSaturation[0-127 64] ɫ��nHue[0-255 0] ٤��nGamma[1-400 100] ��nSharpen[0-127 0]
		// nGammaR[1-400 100] nGammaG[1-400 100] nGammaB[1-400 100]
		virtual void SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,int nGammaR,int nGammaG,int nGammaB)=0;

		virtual void PlayAudioData(const void* pData, int nLen) = 0;
		virtual void PlayVideoData(const void* pData, int nLen) = 0;

		// ������Ƶ��ʾģʽ 0����� 1��ȫ��
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;

		//���浱ǰͼ��BMP�ļ�
		virtual bool CaptureToBMPFile(const char* cszFile) = 0;

		// ��ȡ��Ƶ�ֱ���
		virtual bool GetVideoSize(int& nVideoWidth, int& nVideoHeight) = 0;
		/* ��ȡ����Ƶ�İ���ͳ������
		ulTotalPackets		-�ܹ������˶��ٸ���
		ulLostPackets		-�ܹ������˶��ٸ���
		AvgPacketLossRate	-ƽ��������
		CurPacketLossRate	-��ǰ������
		nBitrate			-������bps��*/
		virtual int GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;
		virtual int GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;
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
	};
}
