//CapChanDev.h
#ifndef __CAPCHANDEV_H__
#define __CAPCHANDEV_H__

#include "CapChan.h"
#include <VIDEC/VIDEC_AH2KDev.h>
#include <VIDEC/VIDEC_CameraDev.h>
#include <VIDEC/VIDEC_AH400Dev.h>
#include <VIDEC/VIDEC_DVCDev.h>
#include <VIDEC/VIDEC_Screen.h>
#include <VIDEC/VIDEC_MediaFile.h>
#include <VIDEC/VIDEC_AVIWriter.h>
#include <VIDEC/VIDEC_VQD.h>

#include <AUDEC/AUDEC_Engine.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <NETEC/NETEC_DVC.h>

#include "XAutoLock.h"
#include "AudioCapChanMgr.h"

//Add WebRTC by zys 20160929
#include <WEBRTC/X86/my_voice_engine_interface.h>
#include "WEBRTC/X86/audio_mixer_api.h"
using namespace voice_engine;

class CapChanDev
	: public IMXEC_CapChanDev
	, public CapChan
	, public VIDEC_StreamSink
	, public VIDEC_ImageSink
	, public VIDEC_CapChanCallback
	, public AUDEC_StreamOut
	, public AUDEC_RecordSamples
	, public VIDEC_MediaFileCallback
	, public AudioCapChanUI
	, public NETEC_DVCCallback
	, public VIDEC_VQDCallback
	, public Transport              //Add WebRTC by zys 20160802
	, public AudioMixer_Callback
{
public:
	CapChanDev(IMXEC_CapChanCallback&rCallback);
	virtual~CapChanDev(void);
public:
	//��ͨ��
	virtual int Open(HWND hwndParent,int nOnlyPreview);
	//�ر�ͨ��
	virtual void Close(void);

	virtual void EnablePreview(int nPreview);

	//ȡ��Ƶ����
	virtual HWND GetVideoWnd(void);
	//ȡ��Ƶ���
	virtual int GetVideoWidth(void);
	//ȡ��Ƶ�߶�
	virtual int GetVideoHeight(void);

	//same as CWnd::IsWindowVisible
	virtual bool IsWindowVisible(void);
	//same as CWnd::SetWindowPos
	virtual bool SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags); 
	//���ֵ�ǰͼ��BMP�ļ�
	virtual bool CaptureToBMPFile(const char*cszFile);


	/******************************************************************************
	* SetScreenWnd
	*����������������Ļ����λ��
	*���룺	nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen);

	virtual void SetVideoWndBk(const char* szBkImage = NULL, int nVideoWnBkR = -1, int nVideoWnBkG = -1, int nVideoWnBkB = -1);

	virtual int SetVolume(int nVolume);
	virtual int GetVolume();
	virtual int GetLevel(int&nLevel);

	virtual unsigned long GetAudioID(void);
	virtual unsigned long GetVideoID(void);

	//����Ƶ�豸
	virtual int OpenVideoDev(int nDevID,int nWidth,int nHeight,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub,const char*cszDVCIP,unsigned short nDVCPort,const char*cszDVCPassword,int nSubWidth,int nSubHeight,int nSubBitrate,int nQSubWidth,int nQSubHeight,int nQSubBitrate);
	virtual int OpenVideoDevActual(int nDevID,int nWidth,int nHeight,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub,int nSubWidth,int nSubHeight,int nSubBitrate,int nQSubWidth,int nQSubHeight,int nQSubBitrate);
	virtual int OpenVideoDev(int x,int y,int cx,int cy,int nBitCount,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub,HDC hDC,int nSubWidth,int nSubHeight,int nSubBitrate,int nQSubWidth,int nQSubHeight,int nQSubBitrate);
	virtual int OpenVideoDev(const char*cszMediaFile,int nLoop,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub,int nSubWidth,int nSubHeight,int nSubBitrate,int nQSubWidth,int nQSubHeight,int nQSubBitrate);
	//nScreenType[0:������Ƶ���,1:��Ļģ�������ϣ�����+Զ��+�װ�+�����ĵ���]
	virtual int OpenVideoDev(int nScreenIndex,int nScreenType,int nWidth,int nHeight,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nSub,int nQSub,int nSubWidth,int nSubHeight,int nSubBitrate,int nQSubWidth,int nQSubHeight,int nQSubBitrate);

	/******************************************************************************
	* IsVideoDevOpen
	*��������ȡ�Ƿ��Ѿ�����Ƶ�豸
	*���룺��
	*�����	��
	*����ֵ��Ϊtrueʱ�Ѿ�����Ƶ�豸������û�д�
	*****************************************************************************/
	virtual bool IsVideoDevOpen(void);

	//�ر���Ƶ�豸
	virtual void CloseVideoDev(void);
 	
	/******************************************************************************
	* SetForceEncodeVideo
	*�����������Ƿ�ǿ��ѹ����Ƶ��¼��ʱ���߱��ػ��ز���ʱ������Ϊ��0������ʱ��������Ϊ0���Խ�ʡ��Դ��
	*���룺nForceEncodeVideo	-�Ƿ�ǿ��ѹ����Ƶ��0:���˽��յ�ʱ��ѹ����Ƶ,��0���κ�ʱ��ѹ����Ƶ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetForceEncodeVideo(int nForceEncodeVideo);
 	
	/******************************************************************************
	* SetForceEncodeAudio
	*�����������Ƿ�ǿ��ѹ����Ƶ��¼��ʱ���߱��ػ��ز���ʱ������Ϊ��0������ʱ��������Ϊ0���Խ�ʡ��Դ��
	*���룺nForceEncodeAudio	-�Ƿ�ǿ��ѹ����Ƶ��0:���˽��յ�ʱ��ѹ����Ƶ,��0���κ�ʱ��ѹ����Ƶ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetForceEncodeAudio(int nForceEncodeAudio);	

	/******************************************************************************
	* RequestKeyFrame
	*����������ؼ�֡
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void RequestKeyFrame(void);

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
	virtual int GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	virtual void SetInvert(int nInvert);
	virtual int GetInvert(void);
	virtual void SetDenoise(int nDenoise);
	virtual int GetDenoise(void);

	//�������ڲ���������
	virtual int SetupMask(RECT *RectList, int iRectCount);
	//ֹͣ����
	virtual int StopMask(void);
	virtual int StartMotionDetect(void);
	virtual int StopMotionDetect(void);
	virtual int SetupMotionDetect(int iGrade/*�����ȵȼ�0-6*/,RECT *RectList, int iRectCount);
	virtual bool GetMotionImageSize(int&nWidth,int&nHeight);
 	
	//��ʾOSD
	virtual void EnableOSD(bool bEnable);
	//�Ƿ���ʾOSD 
	virtual bool IsOSDEnable(void);
	//����OSD 
	virtual void SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT
	//����OSD����,�ֵ���ɫ�ͱ���ɫ
	virtual void SetOSDFont(LOGFONTA logfont,COLORREF crText,COLORREF crBk);
	//������Ƶ����
	virtual int SetVideoParam(int nBrightness/*����ֵ0-255*/, int nContrast/*�Աȶ�0-127*/, int nSaturation/*���Ͷ�0-127*/, int nHue/*ɫ��0-255*/,int nGamma/*٤��1-400*/,int nSharpen/*��0-127*/,bool bSharpenLocalOnly);

	//int nGammaR[1-400 100]
	//int nGammaG[1-400 100]
	//int nGammaB[1-400 100]
	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);
	virtual void SetLevelFixer(int nLevelFixer);
	virtual int GetLevelFixer(void);
	
	virtual bool HasVideoAdjustWnd(void);
	virtual void ShowAdjustVideoWnd(void);
	virtual void ShowVideoInputWnd(void);
	virtual bool HasVideoInputWnd(void);
	virtual int TVTunerPutChannel(long lChannel);


	//����Ƶͨ��
	//nDevID:��Ƶ�ɼ��豸�ţ�-2�Ӳɼ����вɼ���Ƶ��>=-1�������вɼ���Ƶ
	//CodecType:��������
	virtual int OpenAudio(int nDevID,X_AUDIO_CODEC_ID CodecType);

	/******************************************************************************
	* IsAudioOpen
	*��������ȡ�Ƿ��Ѿ�����Ƶ
	*���룺��
	*�����	nDevID		-��Ƶ�ɼ��豸�ţ�-2�Ӳɼ����вɼ���Ƶ��>=-1�������вɼ���Ƶ
			CodecType	-��������
	*����ֵ��Ϊtrueʱ�Ѿ�����Ƶ������û�д�
	*****************************************************************************/
	virtual bool IsAudioOpen(int&nDevID,X_AUDIO_CODEC_ID&CodecType);

	//�ر���Ƶͨ��
	virtual void CloseAudio(void);

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
	virtual int GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	virtual unsigned long GetDuration(void);
	virtual unsigned long GetPosition(void);
	virtual void Seek(unsigned long ulSeconds);
	virtual void Pause(void);
	virtual void Play(void);
	virtual void SetMute(bool bMute=true);
	virtual bool IsMute();

	/******************************************************************************
	* StartRecordToAVI
	*��������ʼ����¼��
	*���룺	cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordToAVI(const char*cszPathName,int nStandardFormat=1, int nCacheTimeMS = 10000);
	
	/******************************************************************************
	* StopRecordToAVI
	*������ֹͣ����¼��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordToAVI(void);

	virtual void SetRecording(int nFlagRecording);

	virtual void SetSpeakerMute(int nMute);

	virtual void OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource);
	virtual void OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource);
	virtual void OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);
	virtual void OnReceivedSubStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);
	virtual void OnReceivedAudioStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen);

	virtual void OnVIDEC_CapChanCallbackPreEncode(VIDEC_CapChan*pCapChan,int&nSkipFlag);

	virtual void OnNETEC_DVCCallbackVideoFrameData(unsigned char*pFrameData,int nFrameLen);

	virtual bool Write(const void*data,int len);
	virtual void OnRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);
	virtual void OnAudioCapChanUIAudioData(unsigned char*pData,int nLen,unsigned long nRefCount,bool&bLoop);
	virtual void OnAudioCapChanUIRtpData(unsigned char*pData,int nLen);
	virtual void OnAudioCapChanUIRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);
	virtual void OnAudioCapChanUIGetForceEncode(int&nEncode);
	
	virtual void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource){};
	virtual void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource){};
	//pData:IYUV��ʽ������,Y+U+V
	virtual void OnReceivedImage(VIDEC_Image*pImage){};
	virtual void OnReceivedPreSend(int& nSend){};
	//pResultList[i]==0,û�ж��������ڶ�
	virtual void OnReceivedMotionDetect(int*pResultList,int nResultCount);
	virtual void OnReceivedAudioSamples(short*pSamples,int nSamples,int nSampleRate);

	virtual void OnVIDEC_MediaFileCallbackPositionChanged(VIDEC_MediaFile*pMediaFile,unsigned long ulSeconds);
	virtual void OnVIDEC_MediaFileCallbackEnd(VIDEC_MediaFile*pMediaFile);
	virtual void OnVIDEC_MediaFileCallbackPCMData(VIDEC_MediaFile*pMediaFile,unsigned char*pPCMData,int nLen,unsigned long ulTimestamp);

	virtual void OnTimer(void);

	virtual void OnVIDEC_VQDCallbackResult(unsigned long nDetectResult);

	/******************************************************************************
	* SendPTZ
	*����������PTZ��������̨�������
	*���룺	nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SendPTZ (PTZ_ACTION nPTZAction,int nSpeed);

	virtual int GetEncoderType(void);
	virtual int GetVideoShowType(void);
	virtual int GetVideoShowFrameRate(void);
	virtual int GetVideoCaptureFrameRate(void);
	virtual int GetVideoEncodeFrameRate(void);
	virtual int GetVideoSendFrameRate(void);

	virtual VIDEC_Image*GetImage(void);

	int GetMixerScreenIndex(void)
	{
		if (m_pMixerScreen!=NULL)
		{
			return m_nMixerScreenIndex;
		}
		return -1;
	}
	void ApplyMixerScreenIndexChanged(int nMixerScreenIndex);

	int GetDVCStatus(void);
	int DVCSendData(const char*pData,int nLen);

	int HasVideoSignal(void){return (m_bVideoSignal?1:0);};

	virtual int StartVQD(unsigned long nDetectFlags);
	virtual void StopVQD(void);
protected:
	typedef enum {
		VCT_UNKOWN=0,
		VCT_CAMERA,
		VCT_AH,
		VCT_HIK,
		VCT_COUNT
	}VIDEO_CARD_TYPE;

	XCritSec				m_csDVC;
	NETEC_DVC*				m_pDVC;
	std::string				m_strDVCIP;
	unsigned short			m_nDVCPort;
	std::string				m_strDVCPassword;

	VIDEC_CapDev*			m_pCapDev;
	VIDEC_Screen*			m_pScreen;
	VIDEC_MediaFile*		m_pMediaFile;
	unsigned long			m_nLastOpenCapDevTime;
	std::string				m_strMediaFilePathName;

	VIDEC_CapChan*			m_pMainVidCapChan;
	VIDEC_CapChan*			m_pSubVidCapChan;
	VIDEC_CapChan*			m_pQSubVidCapChan;
	bool					m_bGotSubFrame;
	bool					m_bGotQSubFrame;

	VIDEO_CARD_TYPE			m_nVideoCardType;

	XCritSec				m_csAudCapChanExternal;
	AUDEC_CapChanExternal*	m_pAudCapChanExternal;
	int						m_nAudioCapDevID;
	X_AUDIO_CODEC_ID		m_nAudioCodecID;

	XCritSec				m_csAVIWriter;
	VIDEC_AVIWriter*		m_pAVIWriter;

	int						m_nForceEncodeVideo;
	int						m_nForceEncodeAudio;

	bool					m_bVideoDevIsOpen;
	int						m_nScreenIndex;
	int						m_nWndIndex;

	int						m_nDevID;
	int						m_nWidth;
	int						m_nHeight;
	int						m_nFrameRate;
	int						m_nBitrate;
	int						m_nBitrateControlType;
	VIDEC_CODEC_TYPE		m_nCodecType;
	int						m_nSub;
	int						m_nQSub;
	int						m_nSubWidth;
	int						m_nSubHeight;
	int						m_nSubBitrate;
	int						m_nQSubWidth;
	int						m_nQSubHeight;
	int						m_nQSubBitrate;

	int						m_nInvert;
	int						m_nDenoise;
	int						m_nLevelFixer;
	bool					m_bEnableOSD;
	std::string				m_strOSDText;
	int						m_nOSDAlign;
	int						m_nTop;
	int						m_nLeft;
	LOGFONTA				m_logfontOSD;
	COLORREF				m_crOSDText;
	COLORREF				m_crOSDBk;
	int						m_nBrightness;
	int						m_nContrast;
	int						m_nSaturation;
	int						m_nHue;
	int						m_nGamma;
	int						m_nSharpen;
	bool					m_bSharpenLocalOnly;
	int						m_nGammaR;
	int						m_nGammaG;
	int						m_nGammaB;


	VIDEC_MPScreen*			m_pMixerScreen;
	int						m_nMixerScreenIndex;
	int						m_nMixerScreenWidth;
	int						m_nMixerScreenHeight;
	int						m_nMixerScreenFrameRate;

	bool					m_bVideoSignal;
	int						m_nNewVideoSignal;

	VIDEC_VQD*				m_pVQD;

	unsigned long			m_nLastGotKeyFrameTS;
	int						m_nFlagRecording;

	//Add WebRTC by zys 20160929
	MyVoiceEngineInterface* m_pWebRTCAudio;
	bool                    m_bMute;

public:
	//
	static bool ms_bAudioRecordAll;

protected:
	virtual bool SendRtp(const uint8_t* packet, size_t length, const PacketOptions& options);
	virtual bool SendRtcp(const uint8_t* packet, size_t length);


public:
	virtual int OnMixedData (int out_channel, const char* rtp_data, int len);

	AudioMixerAPI* m_pAudioMixer;
	int m_nAudioMixOut;
	int m_nAudioMixIn;
	XCritSec				m_csAudioMixer;

};


#endif
