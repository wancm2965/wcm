//MediaCall.h
#ifndef __MEDIACALL_H__
#define __MEDIACALL_H__

#include <string>
#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_Header.h>
#include <VIDEC/VIDEC_CameraDev.h>
#include <VIDEC/VIDEC_AH2KDev.h>
#include <VIDEC/VIDEC_AH400Dev.h>
#include <VIDEC/VIDEC_AVIWriter.h>
#include <AUDEC/AUDEC_Engine.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <AUDEC/AUDEC_Header.h>
#include <AUDEC/AUDEC_Conference.h>
#include <IMXEC/IMXEC_Call.h>

#include <NETEC/NETEC_MediaSender.h>
#include <NETEC/NETEC_Node.h>
#include <NETEC/NETEC_RTP2Frame.h>
#include <NETEC/NETEC_RTPJBuf.h>
#include <IMXEC/IMXEC_MediaChannel.h>

//Add WebRTC by zys 20160802
#include <WEBRTC/X86/my_voice_engine_interface.h>
using namespace voice_engine;

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#include <MPVideo/MPVideo.h>
#include <MPVideo/MPVideo_Transcode.h>

#endif


#include "CapChanRTP.h"
#include "PlayChanRTP.h"
#include "XBitrate.h"
#include "XAutoLock.h"

class MediaCall
	: public CapChanRTPCallback
	, public PlayChanRTPCallback
	, public VIDEC_CapChanCallback
	, public NETEC_MediaSenderCallback
	, public NETEC_RTP2FrameCallback
	, public NETEC_RTPJBufCallback
	, public IMXEC_PlayChanCallback
	, public AUDEC_DTMFCallback		//add by Jan for DTMF [20140730]
	, public VIDEC_WndCallback
	, public Transport              //Add WebRTC by zys 20160802
{
public:
	MediaCall(void);
	virtual ~MediaCall(void);
public:
	virtual void Close(void);
	/******************************************************************************
	* SetLocalMainVideoWnd
	*���������ñ���������Ƶ��ʾ�Ĵ���
	*���룺	hWnd			-��������Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetLocalMainVideoWnd(HWND hWnd);

	/******************************************************************************
	* SetLocalMainVideoWndPos
	*���������ñ���������Ƶ��ʾ�Ĵ���λ��
	*���룺	x				-���ڵ����λ��
			y				-���ڵ��ϱ�λ��
			cx				-���ڵĿ��
			cy				-���ڵĸ߶�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetLocalMainVideoWndPos(int x,int y,int cx,int cy);

	/******************************************************************************
	* SetLocalMainVideoParam
	*���������ñ���������Ƶ����
	*���룺	nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetLocalMainVideoEnableOSD
	*���������ñ���������Ƶ�Ƿ�����OSD
	*���룺	cszCallID		-���б�ʶ
			nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoEnableOSD(int nEnable);

	/******************************************************************************
	* SetLocalMainVideoOSDText
	*���������ñ���������ƵOSD����
	*���룺	cszCallID		-���б�ʶ
			cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoOSDText(const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetLocalMainVideoOSDFont
	*���������ñ���������ƵOSD����,�ֵ���ɫ�ͱ���ɫ
	*���룺	logfont			-OSD����
			crText			-�ֵ���ɫ
			crBk			-����ɫ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoOSDFont(LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

	/******************************************************************************
	* GetLocalMainVideoInfo
	*��������ȡ����������Ƶ��Ϣ
	*���룺	��
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd,int &nFrameRate);

	/******************************************************************************
	* GetLocalMainVideoPacketStatistics
	*��������ȡ��������Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetRemoteMainVideoWnd
	*����������Զ��������Ƶ��ʾ�Ĵ���
	*���룺	��
			hWnd			-Զ������Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetRemoteMainVideoWnd(HWND hWnd);

	/******************************************************************************
	* SetRemoteMainVideoWndPos
	*����������Զ��������Ƶ��ʾ�Ĵ���λ��
	*���룺	x				-���ڵ����λ��
			y				-���ڵ��ϱ�λ��
			cx				-���ڵĿ��
			cy				-���ڵĸ߶�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetRemoteMainVideoWndPos(int x,int y,int cx,int cy);

	/******************************************************************************
	* SetRemoteMainVideoParam
	*����������Զ��������Ƶ����
	*���룺	nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen);

	/******************************************************************************
	* GetRemoteMainVideoInfo
	*��������ȡԶ��������Ƶ��Ϣ
	*���룺	��
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd,int &nFrameRate);

	/******************************************************************************
	* GetRemoteMainVideoPacketStatistics
	*��������ȡԶ������Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetSecondVideoWnd
	*���������ø�����Ƶ��ʾ�Ĵ���
	*���룺��
			hWnd			-Զ�̸�����Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetSecondVideoWnd(HWND hWnd);

	/******************************************************************************
	* SetSecondVideoWndPos
	*���������ø�����Ƶ��ʾ�Ĵ���λ��
	*���룺	x				-���ڵ����λ��
			y				-���ڵ��ϱ�λ��
			cx				-���ڵĿ��
			cy				-���ڵĸ߶�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetSecondVideoWndPos(int x,int y,int cx,int cy);

	/******************************************************************************
	* SetSecondVideoParam
	*���������ø�����Ƶ����
	*���룺	nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetSecondVideoEnableOSD
	*���������ø�����Ƶ�Ƿ�����OSD
	*���룺	nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoEnableOSD(int nEnable);

	/******************************************************************************
	* SetSecondVideoOSDText
	*���������ø�����ƵOSD����
	*���룺	cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoOSDText(const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetSecondVideoOSDFont
	*���������ø�����ƵOSD����,�ֵ���ɫ�ͱ���ɫ
	*���룺	logfont			-OSD����
			crText			-�ֵ���ɫ
			crBk			-����ɫ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoOSDFont(LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

	/******************************************************************************
	* GetSecondVideoInfo
	*��������ȡ������Ƶ��Ϣ
	*���룺	��
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetSecondVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);
	
	/******************************************************************************
	* GetSecondVideoPacketStatistics
	*��������ȡ������Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetSecondVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	virtual int OpenLocalMainVideoCapDev(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType=0,int nNetworkType=VIDEC_NETWORK_PRIVATE);
	virtual void OnRequestLocalMainVideoKeyFrame(void);
	virtual void CloseLocalMainVideoCapDev(void);

	/******************************************************************************
	* GetLocalAudioLevel
	*��������ȡ������ƵLevel��ʵ��������С��
	*���룺	��
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalAudioLevel(int&nLevel);

	/******************************************************************************
	* SetLocalAudioVolume
	*��������ȡ������Ƶ����
	*���룺	nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalAudioVolume(int nVolume);

	/******************************************************************************
	* SetLocalAudioMute
	*��������ȡ������Ƶ�Ƿ���
	*���룺	nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalAudioMute(int nMute);

	/******************************************************************************
	* GetLocalAudioCodecID
	*��������ȡ������Ƶ������Ϣ
	*���룺	��
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalAudioCodecID(X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetLocalAudioPacketStatistics
	*��������ȡ������Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	virtual int OpenLocalAudioCapDev(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType);
	virtual void CloseLocalAudioCapDev(void);

	//virtual int OpenRemoteMainPlayChan(unsigned long nAudioID,unsigned long nVideoID,const std::string&strPeerLocalIP,unsigned short nPeerLocalPort);
	virtual int OpenRemoteMainPlayChan(unsigned long nAudioID,unsigned long nVideoID,const std::string&strPeerLocalIP,unsigned short nPeerLocalPort,const std::string&strPeerMCUID,unsigned long nEnableMulticast);
	virtual int OpenRemoteSecondPlayChan(unsigned long nSecondVideoID,const std::string&strPeerLocalIP,unsigned short nPeerLocalPort,const std::string&strPeerMCUID,unsigned long nEnableMulticast);
	virtual void CloseRemoteSecondPlayChanMedia();

	virtual int OpenRemoteMainVideoPlayChan(VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType);
	virtual void OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen);
	virtual void CloseRemoteMainVideoPlayChan();

	/******************************************************************************
	* GetRemoteAudioLevel
	*��������ȡԶ����ƵLevel��ʵ��������С��
	*���룺	��
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteAudioLevel(int&nLevel);

	/******************************************************************************
	* SetRemoteAudioVolume
	*��������ȡԶ����Ƶ����
	*���룺	nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteAudioVolume(int nVolume);

	/******************************************************************************
	* SetRemoteAudioMute
	*��������ȡԶ����Ƶ�Ƿ���
	*���룺	nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteAudioMute(int nMute);

	/******************************************************************************
	* GetRemoteAudioCodecID
	*��������ȡԶ����Ƶ����
	*���룺	��
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteAudioCodecID (X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetRemoteAudioPacketStatistics
	*��������ȡԶ����Ƶ�İ���ͳ������
	*���룺	��
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	virtual int OpenRemoteAudioPlayChan(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType);
	virtual void OnRecvdRemoteAudioRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdRemoteAudioRTPPacketJBuf(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void CloseRemoteAudioPlayChan(void);

	virtual int OpenLocalSecondVideoCapDev(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nNetworkType=VIDEC_NETWORK_PRIVATE);
	virtual void OnRequestSecondVideoKeyFrame(void);
	virtual void CloseLocalSecondVideoCapDev(void);

	virtual int OpenRemoteSecondVideoPlayChan(VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType);
	virtual void OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen);
	virtual void CloseRemoteSecondVideoPlayChan(void);

	/******************************************************************************
	* StartRecordLocalMainToAVI
	*��������ʼ��������¼��
	*���룺	cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordLocalMainToAVI(const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordLocalMainToAVI
	*������ֹͣ��������¼��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordLocalMainToAVI(void);

	/******************************************************************************
	* StartRecordRemoteMainToAVI
	*��������ʼԶ������¼��
	*���룺	cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordRemoteMainToAVI(const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordRemoteMainToAVI
	*������ֹͣԶ������¼��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordRemoteMainToAVI(void);

	/******************************************************************************
	* StartRecordSecondToAVI
	*��������ʼ����¼��
	*���룺	cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordSecondToAVI(const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordSecondToAVI
	*������ֹͣ����¼��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordSecondToAVI(void);

	/******************************************************************************
	* GetMainAudioID
	*��������ȡ������Ƶ��ʶ
	*���룺 ��
	*����� nMainAudioID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetMainAudioID(unsigned long&nMainAudioID);
	virtual int GetLocalMainAudioID(unsigned long&nMainAudioID);

	/******************************************************************************
	* GetMainVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 ��
	*����� nMainVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetMainVideoID(unsigned long&nMainVideoID);
	virtual int GetLocalMainVideoID(unsigned long&nMainVideoID);

	/******************************************************************************
	* GetSecondVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 ��
	*����� nSecondVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetSecondVideoID(unsigned long&nSecondVideoID);
	virtual int GetLocalSecondVideoID(unsigned long&nSecondVideoID);

	/******************************************************************************
	* SetMainVideoScreenIndex
	*����������������Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetMainVideoScreenIndex(int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenIndex
	*���������ø�����Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetSecondVideoScreenIndex(int nScreenIndex);


	/******************************************************************************
	* SetSecondVideoScreenWnd
	*���������ø�����Ƶ������Ļ����λ��
	*���룺	nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetRemoteVideoScreenWnd
	*����������Զ����Ƶ������Ļ����λ��
	*���룺	nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetLocalRecordCallback
	*���������ñ���¼��ص�
	*���룺	pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalRecordCallback(IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetRemoteRecordCallback
	*����������Զ��¼��ص�
	*���룺pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteRecordCallback(IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetSecondRecordCallback
	*���������ø���¼��ص�
	*���룺	pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondRecordCallback(IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SaveLocalMainVideoToBMPFile
	*���������汾��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveLocalMainVideoToBMPFile(const char*cszPathName);

	/******************************************************************************
	* SaveRemoteMainVideoToBMPFile
	*����������Զ��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveRemoteMainVideoToBMPFile(const char*cszPathName);

	/******************************************************************************
	* SaveSecondVideoToBMPFile
	*���������渨����Ƶ��BMPͼƬ�ļ�
	*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveSecondVideoToBMPFile(const char*cszPathName);

	/******************************************************************************
	* StartMixingWithWaveFile
	*��������ʼ�Ժ��н���WAVE�ļ��������Է�������WAVE�ļ����ŵ�����
	*���룺	cszPathName		-WAVE�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int StartMixingWithWaveFile(const char*cszPathName);

	/******************************************************************************
	* StopMixingWithWaveFile
	*������ֹͣ�Ժ��н���WAVE�ļ�����
	*���룺��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int StopMixingWithWaveFile(void);

	virtual int GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir){nDir=IMXEC_CALL_DIR_UNKOWN;return -1;}

	virtual void OnNETEC_MediaSenderCallbackRecvdPTZ(void*pData,int nLen);
	virtual void OnNETEC_MediaSenderCallbackRecvdCtrl(void*pData,int nLen);
	//nSkipFlag,1:������֡��ѹ����0:ѹ����ǰ֡
	virtual void OnVIDEC_CapChanCallbackPreEncode(VIDEC_CapChan*pCapChan,int&nSkipFlag);

	virtual int RequestMainVideoKeyFrame(void)=0;
	virtual int RequestSecondVideoKeyFrame(void)=0;

	virtual void ExcludePlayChan(AUDEC_PlayChan*pPlayChan);

	virtual void SendAudio(char*pRTPPacketData,int nRTPPacketLen)=0;
	virtual void SendMainVideo(char*pRTPPacketData,int nRTPPacketLen)=0;
	virtual void SendSecondVideo(char*pRTPPacketData,int nRTPPacketLen)=0;

	virtual void OnRecvDTMF(char dtmf);	//add by Jan for DTMF [20140730]

protected:
	virtual void OnCapChanRTPCallbackSendVideo(CapChanRTP*pCapChanRTP,void*pPacketData,int nPacketLen);
	virtual void OnCapChanRTPCallbackSendAudio(CapChanRTP*pCapChanRTP,void*pPacketData,int nPacketLen);
	virtual void OnPlayChanRTPCallbackVideoPacket(PlayChanRTP*pPlayChanRTP,void*pPacketData,int nPacketLen);
	virtual void OnNETEC_RTP2FrameCallbackFramePacket(NETEC_RTP2Frame*pRTP2Frame,unsigned char*pPacketData,int nPacketLen,int nKeyFrame);
	virtual void OnNETEC_RTPJBufCallbackRTPPacket(NETEC_RTPJBuf*pRTPJBuf,unsigned char*pRTPPacketData,int32_t nRTPPacketLen,int32_t nPayloadType,int32_t nRTPHeaderLen,unsigned short nSeqNo,uint32_t nTimestamp);

	//��Ƶ���ݻص�����
	virtual void OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop);
	virtual void OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
	//��Ƶ���ݻص�����
	//nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
	virtual void OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode);

	virtual bool HasAVCONTransport(void){return false;};

	//Add WebRTC by zys 20160802
	virtual bool SendRtp(const uint8_t* packet, size_t length, const PacketOptions& options);
	virtual bool SendRtcp(const uint8_t* packet, size_t length);

	//���AUDECͷ
	virtual int SetAudecHeader(unsigned char* pData,void const* pSrcData,size_t nlen);

	//AUDECͷתRTPͷ
	virtual int AudecHeaderToRTPHeader(unsigned char* pRTPData,void const* pAudecData,size_t nAudecLen);

protected:

	HWND					m_hLocalMainVideoWnd;
	VIDEC_Wnd*				m_pLocalMainVideoWnd;
	VIDEC_CapDev*			m_pLocalMainVideoCapDev;
	VIDEC_CapChanRTP*		m_pLocalMainVideoCapChan;
	VIDEC_CODEC_TYPE		m_nLocalMainVideoCodecType;
	CapChanRTP				m_LocalMainCapChanRTP;
	int						m_nLocalMainVideoBrightness;
	int						m_nLocalMainVideoContrast;
	int						m_nLocalMainVideoSaturation;
	int						m_nLocalMainVideoHue;
	int						m_nLocalMainVideoGamma;
	int						m_nLocalMainVideoSharpen;
	bool					m_bLocalMainVideoSharpenLocalOnly;

	int						m_nLocalMainVideoEnableOSD;
	std::string				m_strLocalMainVideoOSD;
	int						m_nLocalMainVideoAlign;
	COLORREF				m_crLocalMainVideoText;
	COLORREF				m_crLocalMainVideoBk;
	LOGFONTA				m_fontLocalMainVideoOSD;
	XBitrate				m_XBitrateLocalMainVideo;

	HWND					m_hRemoteMainVideoWnd;
	VIDEC_Wnd*				m_pRemoteMainVideoWnd;
	VIDEC_PlayChanRTP*		m_pRemoteMainVideoPlayChan;
	VIDEC_CODEC_TYPE		m_nRemoteMainVideoCodecType;
	int						m_nRemoteMainVideoBrightness;
	int						m_nRemoteMainVideoContrast;
	int						m_nRemoteMainVideoSaturation;
	int						m_nRemoteMainVideoHue;
	int						m_nRemoteMainVideoGamma;
	int						m_nRemoteMainVideoSharpen;
	XBitrate				m_XBitrateRemoteMainVideo;

	HWND					m_hSecondVideoWnd;
	VIDEC_CapDev*			m_pSecondVideoCapDev;
	VIDEC_CapChanRTP*		m_pSecondVideoCapChan;
	VIDEC_Wnd*				m_pSecondVideoWnd;
	VIDEC_PlayChanRTP*		m_pSecondVideoPlayChan;
	VIDEC_CapChan*			m_pSecondVideoTranscoder;
	VIDEC_CapChan*			m_pMainVideoTranscoder;
	VIDEC_CODEC_TYPE		m_nSecondVideoCodecType;
	CapChanRTP				m_LocalSecondCapChanRTP;
	int						m_nSecondVideoBrightness;
	int						m_nSecondVideoContrast;
	int						m_nSecondVideoSaturation;
	int						m_nSecondVideoHue;
	int						m_nSecondVideoGamma;
	int						m_nSecondVideoSharpen;
	bool					m_bSecondVideoSharpenLocalOnly;
	int						m_nSecondVideoEnableOSD;
	std::string				m_strSecondVideoOSD;
	int						m_nSecondVideoAlign;
	COLORREF				m_crSecondVideoText;
	COLORREF				m_crSecondVideoBk;
	LOGFONTA				m_fontSecondVideoOSD;
	XBitrate				m_XBitrateSecondVideo;

	AUDEC_CapChan*			m_pLocalAudioCapChan;
	unsigned char*			m_pLocalAudioCapChanBuf;
	int						m_nLocalAudioCapChanBufLen;
	unsigned long			m_ulLocalAudioCapChanRTPTimestamp;
	int						m_nLocalAudioCapChanRTPPayloadType;
	X_AUDIO_CODEC_ID		m_nLocalAudioCodecID;
	int						m_nLocalAudioMute;
	int						m_nLocalAudioVolume;

	AUDEC_PlayChan*			m_pRemoteAudioPlayChan;
	X_AUDIO_CODEC_ID		m_nRemoteAudioPlayChanCodecID;
	unsigned char*			m_pRemoteAudioPlayChanBuf;
	int						m_nRemoteAudioPlayChanBufLen;
	int						m_nRemoteAudioMute;
	int						m_nRemoteAudioVolume;
	int						m_nRemoteAudioPayloadType;

	int						m_nMainVideoScreenIndex;
	int						m_nSecondVideoScreenIndex;
	VIDEC_MPScreen*			m_pMainVideoMPScreen;
	VIDEC_MPScreen*			m_pSecondVideoMPScreen;

	AUDEC_Mixer*			m_pAudioMixer;
	AUDEC_PlayChan*			m_pExcludePlayChan;

	NETEC_MediaSender*		m_pMediaSenderRemoteMain;
	NETEC_MediaSender*		m_pMediaSenderRemoteSecond;
	NETEC_RTP2Frame*		m_pRTP2FrameRemoteMain;
	NETEC_RTP2Frame*		m_pRTP2FrameRemoteSecond;
	unsigned short			m_nSeqNoRemoteMain;
	unsigned short			m_nSeqNoRemoteSecond;

	NETEC_MediaSender*		m_pMediaSenderLocalMain;
	NETEC_RTP2Frame*		m_pRTP2FrameLocalMain;
	unsigned short			m_nSeqNoLocalMain;

	NETEC_MediaSender*		m_pMediaSenderLocalSecond;
	NETEC_RTP2Frame*		m_pRTP2FrameLocalSecond;
	unsigned short			m_nSeqNoLocalSecond;

	PlayChanRTP				m_RemoteMainPlayChanRTP;
	PlayChanRTP				m_RemoteSecondPlayChanRTP;

	std::string				m_strSecondRecordPathName;
	int						m_nSecondRecordIndex;

	XCritSec				m_csLocalRecordCallback;
	IMXEC_RecordCallback*	m_pLocalRecordCallback;
	XCritSec				m_csRemoteRecordCallback;
	IMXEC_RecordCallback*	m_pRemoteRecordCallback;
	XCritSec				m_csSecondRecordCallback;
	IMXEC_RecordCallback*	m_pSecondRecordCallback;


	NETEC_RTPJBuf*			m_pRTPJBufMainVideo;
	NETEC_RTPJBuf*			m_pRTPJBufSecondVideo;
	NETEC_RTPJBuf*			m_pRTPJBufAudio;

	IMXEC_PlayChan*			m_pRemoteMainPlayChan;
	IMXEC_PlayChan*			m_pRemoteSecondPlayChan;
	bool					m_bRecording;

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

	MPVideo_Transcode*		m_pTranscodeRemoteMain;
	MPVideo_Transcode*		m_pTranscodeRemoteSecond;
#endif

	//Add WebRTC by zys 20160929
	MyVoiceEngineInterface* m_pWebRTCAudio;
	int m_nWebRTCPlayChan;

	int m_nRTPTimestamp;
	int m_nRTPSeqNo;
	int m_nAudioSeqNo;

public:
	//notice netec send ptz 
	int MediaSendCtrl(void*pData,int nLen);
	int MediaSendPTZ(void*pData,int nLen);
	virtual void OnNETEC_RecvdPTZ(void*pData,int nLen) {};
	virtual void OnNETEC_RecvdCtrl(void*pData,int nLen) {};
	virtual void OnAudioRecvDTMF(char dtmf) {};//add by Jan for DTMF [20140730]
	int ReSetMainAudioCapBitrate(int nBitrate);
	int ReSetMainVideoCapBitrate(int nBitrate);
	int ReSetSecondVideoCapBitrate(int nBitrate);

	virtual int OnNETECRequestMainVideoKeyFrame(void)=0;
	virtual int OnNETECRequestSecondVideoKeyFrame(void)=0;

	int m_nMainVideoFlowCCBitRate;          //�����յ����������� ��λ100 bit/s Ĭ��Ϊ1
	int m_nDualVideoFlowCCBitRate;          //�����յ����������� ��λ100 bit/s Ĭ��Ϊ1
	int m_nAudioFlowCCBitRate;              //�����յ����������� ��λ100 bit/s Ĭ��Ϊ1
	//XCritSec		m_csVideoSender;        //����

};


#endif