// UCC7-����Ƶ�ɼ�����ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "U7_AVDefine.h"

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

		virtual void OnCapChanRecordStart(void){};

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

		// ��Ƶ����
		virtual void SetVideoSet(const VIDEOSET_INFO& videosetInfo) = 0;
		virtual void GetVideoSet(VIDEOSET_INFO& videosetInfo) = 0;

		// ����Ƶ�豸
		// ��Ӧ����Ƶ������������
		virtual int OpenVideo(int nVideoSizeIndex,int nWidth,int nHeight,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC videoCodec,int nSub,int nQSub, int nSubWidth = 0, int nSubHeight = 0, int nSubBitrate = 0, int nQSubWidth = 0, int nQSubHeight = 0, int nQSubBitrate = 0) = 0;
		// �������ж�ȡ��Ƶ��������
		virtual int OpenVideo(int nSub, int nQSub) = 0;
		virtual int OpenVideo(int x, int y, int cx, int cy, int nSub, int nQSub, int nSubWidth = 0, int nSubHeight = 0, int nSubBitrate = 0, int nQSubWidth = 0, int nQSubHeight = 0, int nQSubBitrate = 0, HDC hDC = NULL) = 0;
		virtual int OpenVideo(const char* cszMediaFile, int nLoop, int nSub, int nQSub, int nSubWidth = 0, int nSubHeight = 0, int nSubBitrate = 0, int nQSubWidth = 0, int nQSubHeight = 0, int nQSubBitrate = 0) = 0;
		virtual int OpenVideo(const char* cszIP, int nPort, const char* cszPassword, int nSubWidth = 0, int nSubHeight = 0, int nSubBitrate = 0, int nQSubWidth = 0, int nQSubHeight = 0, int nQSubBitrate = 0) = 0;
		// �ر���Ƶ
		virtual void CloseVideo(void) = 0;

		// ��Ƶ����
		virtual void SetAudioSet(int nDevID, int nAudioCodec) = 0;
		virtual void GetAudioSet(int& nDevID, int& nAudioCodec) = 0;

		// ����Ƶ
		virtual int OpenAudio(void) = 0;
		// �ر���Ƶ
		virtual void CloseAudio(void) = 0;

		virtual bool IsVideoOpen(void) = 0;
		virtual bool IsAudioOpen(void) = 0;

		// ǿ�����ñ���
		virtual void SetForceEncode(bool bForce) = 0;

		// ������Ƶ��ʾģʽ 0����� 1��ȫ��
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;
		// ��ȡ��Ƶ��ʾģʽ 0����� 1��ȫ��
		virtual int GetVideoShowMode(void) = 0;

		// ������Ƶ����
		// ����ֵnBrightness[0-255 128] �Աȶ�nContrast[0-127 64] ���Ͷ�nSaturation[0-127 64] ɫ��nHue[0-255 0] ٤��nGamma[1-400 100] ��nSharpen[0-127 0]
		// �񻯽�Ӧ�õ�����bSharpenLocalOnly[Ĭ��ֵΪtrue]
		// nGammaR[1-400 100] nGammaG[1-400 100] nGammaB[1-400 100]
		// �����ȵ���bAjustAmp[Ĭ��ֵΪfalse] Ϊtrueʱ����ԭ��ֵ�Ļ����ϼ��ϴ���Ĳ���ֵ
		virtual void SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly,
			int nGammaR,int nGammaG,int nGammaB,bool bAjustAmp) = 0;
		virtual void GetVideoParam(int& nBrightness, int& nContrast, int& nSaturation, int& nHue, int& nGamma,int& nSharpen, bool& bSharpenLocalOnly,
			int& nGammaR, int& nGammaG, int& nGammaB) = 0;

		virtual bool HasVideoAdjustWnd(void)=0;
		virtual void ShowAdjustVideoWnd(void)=0;

		virtual unsigned long GetAudioID(void) = 0;
		virtual unsigned long GetVideoID(void) = 0;
		virtual CAP_TYPE GetCapType(void) = 0;

		// ��ȡ��Ƶ�ֱ���
		virtual bool GetVideoSize(int& nVideoWidth, int& nVideoHeight) = 0;

		virtual bool GetAutoVideoSize(int& nVideoWidth, int& nVideoHeight) = 0;
		// ��ȡ��Ƶ֡��
		virtual int GetVideoFrame(void) = 0;
		//��ȡ��Ƶ����ֵ
		virtual int GetVideoBitrate(void) = 0;

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

		// ����ؼ�֡
		virtual void RequestKeyFrame() = 0;

		// ֧�ָ�ʽ��avms��avi��mkv��mp4
		virtual void StartRecord(const char* cszPathName) = 0;
		virtual void StopRecord(void) = 0;

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

		// �����������̨����
		virtual int DVCSendData(const char*pData,int nLen) = 0;

		// �����Ƿ���ʾOSD
		virtual void EnableOSD(bool bEnable) = 0;
		// ��ȡ�Ƿ���ʾOSD
		virtual bool IsOSDEnable(void) = 0;

		// ����OSD���ı�
		// Align==0 �����Ͻ� 1:�����½�,2:�����м�,3:�����м�,4:�����Ͻ�,5:�����½�
		virtual void SetOSDText(const char*cszText,int nAlign = 0) = 0;
		// ��ȡOSD���ı�
		virtual void GetOSDText(std::string& strText, int& nAlign) = 0;

		// ����OSD������
		virtual void SetOSDFont(const LOGFONTA& logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255)) = 0;
		// ��ȡOSD������
		virtual void GetOSDFont(LOGFONTA& logfont,COLORREF& crText,COLORREF& crBk) = 0;
		virtual int OpenCompoundVideo(int nScreenIndex, int nScreenType, int nWidth, int nHeight, int nFrameRate, int nBitrate, int nBitrateControlType, VIDEC_CODEC_TYPE CodecType, int nSub, int nQSub) = 0;
		
		virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen) = 0;
		//�����Ƿ����������Ƶ
		virtual void SetAVOpenbAllow(bool bAllowOpenAV) = 0;

		//�����ֱ��ʼ�����
		//��������Ϊ0��ʾ�����и���
		//nBitreate������Ϊ256��512��
		virtual void Lock(int nWidth, int nHeight, int nBitreate) = 0;
		virtual void UnLock(bool bResolution, bool bBitreate) = 0;
	};

	class IPlugin_U7_AV_CaptureMgr : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_AV_CaptureMgr(){};

	public:
		virtual IPlugin_U7_AV_Capture* CreateU7AVCapture(CAP_TYPE nType, int nDevID) = 0;
		virtual IPlugin_U7_AV_Capture* GetU7AVCapture(int nDevID) = 0;
		virtual IPlugin_U7_AV_Capture* GetU7AVCaptureByChnID(int nChannelID) = 0;
		virtual IPlugin_U7_AV_Capture* GetU7AVCapture(HWND hWnd) = 0;
		virtual void DestroyU7AVCapture(int nDevID) = 0;
		virtual void DestroyU7AVCapture(void) = 0;

		// ����ĳͨ���Ŀ�������CPU������
		virtual IPlugin_U7_AV_Capture* CreateDevCopy(CAP_TYPE nType, int nDevID, unsigned long ulCopyIndex) = 0;
		virtual IPlugin_U7_AV_Capture* GetDevCopy(unsigned long ulCopyIndex) = 0;
		virtual void DestroyCardCopy(unsigned long ulCopyIndex) = 0;

		// ���÷�����Ƶ
		virtual void EnableVideoSend(bool bEnable) = 0;
		virtual bool IsEnableVideoSend(void) = 0;

		// ��ȡ��Ƶͨ������
		virtual int GetChannelCount(void) = 0;
		// ��ȡ��Ƶͨ����Ӧ���豸
		virtual int GetChannelDev(int nChannelID) = 0;

		// ȡ�豸����
		virtual int GetDevCount(void) = 0;
		// ȡ�豸����
		virtual int GetDevName(int nDevID,char*szName,int nMaxCount) = 0;

		// ȡ�豸����
		virtual int GetDevType(int nDevID,int&nDevType) = 0;

		// �г���ʽ�ĸ���
		virtual int GetFormatCount(int nDevType) = 0;
		// ȡ��ʽ����
		virtual int GetFormatName(int nDevType,int nIndex,char*szName,int nMaxCount) = 0;
		// ȡ��ʽ��С
		virtual int GetFormatSize(int nDevType,int nIndex,int&nWidth,int&nHeight) = 0;

		// ȡ��Ƶ�豸����
		virtual int GetMicDevCount(void) = 0;
		// ȡ��Ƶ�豸����
		virtual int GetMicDevName(int nDev,char desc[]) = 0;

		virtual int GetSpeakerNumDevs(void) = 0;

		// 0:ok,<0:error
		virtual int GetSpeakerDevDesc(int dev,char desc[]) = 0;

		// ȡ��Ƶ����
		virtual AUDIOSET_INFO GetAudioSet(int nCardIndex) = 0;
		// ������Ƶ
		virtual void SetAudioSet(int nCardIndex, const AUDIOSET_INFO& audioset) = 0;

		// ȡ��Ƶ����
		virtual VIDEOSET_INFO GetVideoSet(int nCardIndex) = 0;
		// ������Ƶ
		virtual void SetVideoSet(int nCardIndex, const VIDEOSET_INFO& videoset) = 0;

		// ������Ƶ�����ַ���
		virtual void EncodeVideoSet(const VIDEOSET_INFO& videoset, std::string& strCodec, std::string& strSize
			, std::string& strFrameRate, std::string& strBitrate, std::string& strNetType) = 0;
		// ����Ӧ����Ƶ����
		// nSizeΪ�ֱ���������nSzWidth��nSzHeight�ֱ�Ϊ��ʵ�ֱ��ʿ���
		// nStreamΪ����������nBitrateΪ��Ƶ��ʵ����ֵ��
		virtual void DecodeSetVideoSet(int nDevID, int nCodec, int nSize, int nSzWidth, int nSzHeight
			, int nFrameRate, int nStream, int nBitrate, int nNetType, bool bDenoise) = 0;

		// ������Ƶ��ʾģʽ 0����� 1��ȫ��
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;
		// ��ȡ��Ƶ��ʾģʽ 0����� 1��ȫ��
		virtual int GetVideoShowMode(void) = 0;

		// ��ȡ��ǰ�����ϴ��ܴ���
		virtual unsigned long GetUploadTotalBitrate(void) = 0;

		virtual int SetScreenPos(int nScreenIndex,int x,int y,int cx,int cy) = 0;

		virtual int SetScreenWndCount(int nScreenIndex,int nWndCount) = 0;

		virtual int SetScreenWndPos(int nScreenIndex,int nWndIndex,int x,int y,int cx,int cy) = 0;
	};
}
