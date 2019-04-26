//CallMgr.h
#ifndef __CALLMGR_H__
#define __CALLMGR_H__

#include <string>
#include <map>
#include <vector>
#include <IMXEC/IMXEC_Call.h>
#include <NETEC/XAutoLock.h>
#include <IMXEC/IMXEC_VideoSetting.h>
#include <AUDEC/AUDEC_Conference.h>
#include <IMXEC/AdapterHeaderDefine.h>
#include "ScreenCapChanRTP.h"
#include "XAutoLock.h"
#include "IPProcess.h"

//media release rtp port save array 20140718 add
#define MAX_PORTRLEASE_NUM  128*6
struct PortReleaseItem
{
	unsigned short m_nRtpPort;
};

struct MediaPortRelease
{
	int                m_nCount;    //��Ч���ݸ���
	PortReleaseItem    m_Array[MAX_PORTRLEASE_NUM];
	MediaPortRelease()
	{
		m_nCount = 0;
	}
};

class CallMgr
{
public:
	CallMgr(void);
	virtual ~CallMgr(void);
public:
	static CallMgr&Instance(void);
	int    m_nUseNETECSend;         //ʹ��NETEC˽��Э�鴫��ý��������Ϊ1 ��������Ϊ0 Ĭ��0 
	int    m_nUseCheckMedia;        //ʹ��ý������ʱ�������Ϊ1 ��������Ϊ0 Ĭ��0
	int    m_nUseAjustMediaRate;    //ʹ��ý����������Ӧ��������Ϊ1 ��������Ϊ0 Ĭ��0
	XCritSec m_csMapCall;		    //������������
	int    m_nIPCPort;
	int    m_nResStand;             //0 ���� 1 ���� 2 ȫ���� Ĭ��99ȫ����
	IPItemSave    m_AllLocalIPSave; //����IP��ַ�б�
	RouteItemSave m_AddrRoute;      //����������·�ɱ�
	std::vector<std::string> m_vecBlackIP; //ip�������б�
	bool  m_bUseBlackIPList;        //�Ƿ�ʹ��ip�������б�
	bool IsBlackListIP(const char* strIP);           //�Ƿ��Ǻ�����ip

public:
	void AddIMXEC_Call(IMXEC_Call*pIMXEC_Call);
	void RemoveIMXEC_Call(IMXEC_Call*pIMXEC_Call);

	/******************************************************************************
	* MakeCall
	*�������������
	*���룺	cszUserID		-�Է����û�ID��IP
			cszProtocol		-����Э�飬"H323","SIP","IMXEC"
	*�������
	*����ֵ�����б�ʶ
	*****************************************************************************/
	const char*MakeCall(const char*cszUserID,const char*cszProtocol);

	/******************************************************************************
	* AcceptCall
	*���������ܺ�������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AcceptCall(const char*cszCallID);

	/******************************************************************************
	* HangupCall
	*�������ҶϺ���
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int HangupCall(const char*cszCallID);

	/******************************************************************************
	* SendPTZ
	*����������PTZ��������̨�������
	*���룺	cszCallID		-���б�ʶ
			nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	/******************************************************************************
	* StartSecondVideo
	*��������ʼ����
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int StartSecondVideo (const char*cszCallID);

	/******************************************************************************
	* StopSecondVideo
	*������ֹͣ����
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int StopSecondVideo (const char*cszCallID);

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#else

	/******************************************************************************
	* SetLocalMainVideoWnd
	*���������ñ���������Ƶ��ʾ�Ĵ���
	*���룺	cszCallID		-���б�ʶ
			hWnd			-��������Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetLocalMainVideoWnd(const char*cszCallID,HWND hWnd);

	/******************************************************************************
	* SetLocalMainVideoWndPos
	*���������ñ���������Ƶ��ʾ�Ĵ���λ��
	*���룺	cszCallID		-���б�ʶ
			x				-���ڵ����λ��
			y				-���ڵ��ϱ�λ��
			cx				-���ڵĿ��
			cy				-���ڵĸ߶�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetLocalMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

	/******************************************************************************
	* SetLocalMainVideoParam
	*���������ñ���������Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetLocalMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetLocalMainVideoEnableOSD
	*���������ñ���������Ƶ�Ƿ�����OSD
	*���룺	cszCallID		-���б�ʶ
			nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetLocalMainVideoEnableOSD(const char*cszCallID,int nEnable);

	/******************************************************************************
	* SetLocalMainVideoOSDText
	*���������ñ���������ƵOSD����
	*���룺	cszCallID		-���б�ʶ
			cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetLocalMainVideoOSDText(const char*cszCallID,const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetLocalMainVideoOSDFont
	*���������ñ���������ƵOSD����,�ֵ���ɫ�ͱ���ɫ
	*���룺	cszCallID		-���б�ʶ
			logfont			-OSD����
			crText			-�ֵ���ɫ
			crBk			-����ɫ
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetLocalMainVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));


	/******************************************************************************
	* SetRemoteMainVideoWnd
	*����������Զ��������Ƶ��ʾ�Ĵ���
	*���룺	cszCallID		-���б�ʶ
			hWnd			-Զ������Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetRemoteMainVideoWnd(const char*cszCallID,HWND hWnd);

	/******************************************************************************
	* SetRemoteMainVideoWndPos
	*����������Զ��������Ƶ��ʾ�Ĵ���λ��
	*���룺	cszCallID		-���б�ʶ
			x				-���ڵ����λ��
			y				-���ڵ��ϱ�λ��
			cx				-���ڵĿ��
			cy				-���ڵĸ߶�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetRemoteMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

	/******************************************************************************
	* SetRemoteMainVideoParam
	*����������Զ��������Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetRemoteMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen);

	/******************************************************************************
	* GetRemoteMainVideoPacketStatistics
	*��������ȡԶ������Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetSecondVideoWnd
	*���������ø�����Ƶ��ʾ�Ĵ���
	*���룺	cszCallID		-���б�ʶ
			hWnd			-Զ�̸�����Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetSecondVideoWnd(const char*cszCallID,HWND hWnd);

	/******************************************************************************
	* SetSecondVideoWndPos
	*���������ø�����Ƶ��ʾ�Ĵ���λ��
	*���룺	cszCallID		-���б�ʶ
			x				-���ڵ����λ��
			y				-���ڵ��ϱ�λ��
			cx				-���ڵĿ��
			cy				-���ڵĸ߶�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetSecondVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

	/******************************************************************************
	* SetSecondVideoParam
	*���������ø�����Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nBrightness		-����ֵ0-255
			nContrast		-�Աȶ�0-127
			nSaturation		-���Ͷ�0-127
			nHue			-ɫ��0-255
			nGamma			-٤��1-400
			nSharpen		-��0-127
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetSecondVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetSecondVideoEnableOSD
	*���������ø�����Ƶ�Ƿ�����OSD
	*���룺	cszCallID		-���б�ʶ
			nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetSecondVideoEnableOSD(const char*cszCallID,int nEnable);

	/******************************************************************************
	* SetSecondVideoOSDText
	*���������ø�����ƵOSD����
	*���룺	cszCallID		-���б�ʶ
			cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetSecondVideoOSDText(const char*cszCallID,const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetSecondVideoOSDFont
	*���������ø�����ƵOSD����,�ֵ���ɫ�ͱ���ɫ
	*���룺	cszCallID		-���б�ʶ
			logfont			-OSD����
			crText			-�ֵ���ɫ
			crBk			-����ɫ
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetSecondVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

#endif

	/******************************************************************************
	* GetLocalMainVideoInfo
	*��������ȡ����������Ƶ��Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			nFrame          -ͼ��֡��
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

	/******************************************************************************
	* GetLocalMainVideoPacketStatistics
	*��������ȡ��������Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetRemoteMainVideoInfo
	*��������ȡԶ��������Ƶ��Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			nFrame          -ͼ��֡��
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

	/******************************************************************************
	* GetSecondVideoInfo
	*��������ȡ������Ƶ��Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			nFrame          -ͼ��֡��
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

	/******************************************************************************
	* GetSecondVideoPacketStatistics
	*��������ȡ������Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetLocalAudioLevel
	*��������ȡ������ƵLevel��ʵ��������С��
	*���룺	cszCallID		-���б�ʶ
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetLocalAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetLocalAudioVolume
	*��������ȡ������Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	void SetLocalAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetLocalAudioMute
	*��������ȡ������Ƶ�Ƿ���
	*���룺	cszCallID		-���б�ʶ
			nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	void SetLocalAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetLocalAudioCodecID
	*��������ȡ������Ƶ������Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetLocalAudioPacketStatistics
	*��������ȡ������Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetRemoteAudioLevel
	*��������ȡԶ����ƵLevel��ʵ��������С��
	*���룺	cszCallID		-���б�ʶ
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetRemoteAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetRemoteAudioVolume
	*��������ȡԶ����Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	void SetRemoteAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetRemoteAudioMute
	*��������ȡԶ����Ƶ�Ƿ���
	*���룺	cszCallID		-���б�ʶ
			nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	void SetRemoteAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetRemoteAudioCodecID
	*��������ȡԶ����Ƶ����
	*���룺	cszCallID		-���б�ʶ
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetRemoteAudioPacketStatistics
	*��������ȡԶ����Ƶ�İ���ͳ������
	*���룺	cszCallID			-���б�ʶ
	*����� ulTotalPackets		-�ܹ������˶��ٸ���
			ulLostPackets		-�ܹ������˶��ٸ���
			AvgPacketLossRate	-ƽ��������
			CurPacketLossRate	-��ǰ������
			nBitrate			-������bps��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetCallSupportSecondVideo
	*��������ȡ�����Ƿ�֧�ָ�����H239��
	*���룺	cszCallID		-���б�ʶ
	*�����	��
	*����ֵ��֧�ַ��ط�0����֧�ַ���0
	*****************************************************************************/
	int GetCallSupportSecondVideo(const char*cszCallID);

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#else

	/******************************************************************************
	* StartRecordLocalMainToAVI
	*��������ʼ��������¼��
	*���룺	cszCallID			-���б�ʶ
			cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int StartRecordLocalMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordLocalMainToAVI
	*������ֹͣ��������¼��
	*���룺cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void StopRecordLocalMainToAVI(const char*cszCallID);

	/******************************************************************************
	* StartRecordRemoteMainToAVI
	*��������ʼԶ������¼��
	*���룺	cszCallID			-���б�ʶ
			cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int StartRecordRemoteMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordRemoteMainToAVI
	*������ֹͣԶ������¼��
	*���룺cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void StopRecordRemoteMainToAVI(const char*cszCallID);

	/******************************************************************************
	* StartRecordSecondToAVI
	*��������ʼ����¼��
	*���룺	cszCallID			-���б�ʶ
			cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int StartRecordSecondToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordSecondToAVI
	*������ֹͣ����¼��
	*���룺cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void StopRecordSecondToAVI(const char*cszCallID);

#endif

public:
	void OnCallRingIn(const std::string&strCallID,const std::string&strUserID,const std::string&strUserName,const std::string&strProto);
	void OnCallRingOut(const std::string&strCallID,const std::string&strUserID,const std::string&strUserName);
	void OnCallEstablished(const std::string&strCallID,const std::string&strUserID,const std::string&strUserName);
	void OnCallDestroyed(const std::string&strCallID,const std::string&strUserID,const std::string&strUserName,IMXEC_CALL_END_REASON nReason ,int callNum=0);

	/******************************************************************************
	* OnCallRecvdPTZ
	*���������յ�PTZ�¼������Է�������̨��������ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnCallRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	//add by Jan for DTMF [20140730]
	/******************************************************************************
	* OnCallRecvdDTMF
	*���������յ�DTMF�¼������Է�����DTMF����ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			szDtmf			-DTMF����
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnCallRecvdDTMF(const char*cszCallID,char dtmf);
	//end [20140730]

	/******************************************************************************
	* OnCallSecondVideoStart
	*������������ʼ�¼�����������ʼʱ��ϵͳ����ô��¼�
	*���룺	strCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnCallSecondVideoStart(const std::string&strCallID);

	/******************************************************************************
	* OnCallSecondVideoStop
	*����������ֹͣ�¼���������ֹͣʱ��ϵͳ����ô��¼�
	*���룺	strCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnCallSecondVideoStop(const std::string&strCallID);

	/******************************************************************************
	* OnSIPVirtualLineRegistrationSuccess
	*������SIPע��ɹ��¼�
	*���룺	nLineID		-��·��ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnSIPVirtualLineRegistrationSuccess(int nLineID);

	/******************************************************************************
	* OnSIPVirtualLineRegistrationError
	*������SIPע��ʧ���¼�
	*���룺	nLineID			-��·��ʶ
			cszErrorCode	-������
	*�������
	*����ֵ����
	*****************************************************************************/
	 void OnSIPVirtualLineRegistrationError(int nLineID,const char*cszErrorCode);

	/******************************************************************************
	* OnH323RegisterOK
	*������H323ע��ɹ��¼�
	*���룺cszGKHost ע�ᵽ��GK��ַ port ע�ᵽ��GK�˿�
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnH323RegisterOK(const char*cszGKHost,int port);

	/******************************************************************************
	* OnH323RegisterFailed
	*������H323ע��ʧ���¼�
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnH323RegisterFailed(void);

	/******************************************************************************
	* SetCallType
	*���������ú�������
	*���룺	nCallType		-��������
	*�����	��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetCallType(IMXEC_CALL_TYPE nCallType);

	/******************************************************************************
	* GetCallType
	*��������ȡ��������
	*���룺	��
	*�����	nCallType		-��������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetCallType(IMXEC_CALL_TYPE&nCallType);

	/******************************************************************************
	* GetMainAudioID
	*��������ȡ������Ƶ��ʶ
	*���룺 cszCallID		-���б�ʶ
	*����� nMainAudioID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID);

	/******************************************************************************
	* GetMainVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 cszCallID		-���б�ʶ
	*����� nMainVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID);

	/******************************************************************************
	* GetSecondVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 cszCallID		-���б�ʶ
	*����� nSecondVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID);

	/******************************************************************************
	* SetMainVideoScreenIndex
	*����������������Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenIndex
	*���������ø�����Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenWnd
	*���������ø�����Ƶ������Ļ����λ��
	*���룺	cszCallID		-���б�ʶ
			nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetRemoteVideoScreenWnd
	*����������Զ����Ƶ������Ļ����λ��
	*���룺	cszCallID		-���б�ʶ
			nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#else

	/******************************************************************************
	* SetLocalRecordCallback
	*���������ñ���¼��ص�
	*���룺	cszCallID		-���б�ʶ
			pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetLocalRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetRemoteRecordCallback
	*����������Զ��¼��ص�
	*���룺	cszCallID		-���б�ʶ
			pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetRemoteRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetSecondRecordCallback
	*���������ø���¼��ص�
	*���룺	cszCallID		-���б�ʶ
			pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetSecondRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SaveLocalMainVideoToBMPFile
	*���������汾��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszCallID		-���б�ʶ
			cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SaveLocalMainVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SaveRemoteMainVideoToBMPFile
	*����������Զ��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszCallID		-���б�ʶ
			cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SaveRemoteMainVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SaveSecondVideoToBMPFile
	*���������渨����Ƶ��BMPͼƬ�ļ�
	*���룺	cszCallID		-���б�ʶ
			cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SaveSecondVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* HotDisplayVideo
	*��������Ƶ��ʾ֧����ʾ���Ȳ�νӿ�
	*���룺	cszCallID		    -���б�ʶ
			hWndLocal			-��������Ƶ��ʾ�Ĵ��ھ�� ��û��������ΪNULL
			hWndRemote			-Զ������Ƶ��ʾ�Ĵ��ھ�� ��û��������ΪNULL
			hWndSecond         	-Զ�̸�����Ƶ��ʾ�Ĵ��ھ�� ��û��������ΪNULL
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int HotDisplayVideo(const char*cszCallID,HWND hWndLocal,HWND hWndRemote,HWND hWndSecond);

#endif

	/******************************************************************************
	* SendDTMF
	*����������DTMF
	*���룺	cszCallID		-���б�ʶ
			nDTMF			-DTMF
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendDTMF(const char*cszCallID,const char nDTMF);

	/******************************************************************************
	* StartMixingWithWaveFile
	*��������ʼ�Ժ��н���WAVE�ļ��������Է�������WAVE�ļ����ŵ�����
	*���룺	cszCallID		-���б�ʶ
			cszPathName		-WAVE�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int StartMixingWithWaveFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* StopMixingWithWaveFile
	*������ֹͣ�Ժ��н���WAVE�ļ�����
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int StopMixingWithWaveFile(const char*cszCallID);

	/******************************************************************************
	* SetAudioOnly
	*�����������Ƿ�ֻ֧����Ƶ
	*���룺	nAudioOnly		-�Ƿ�ֻ֧����Ƶ��0��֧����Ƶ��Ƶ����0��ֻ֧����Ƶ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetAudioOnly(int nAudioOnly);
	int GetAudioOnly(void);

	/******************************************************************************
	* GetCallProtocol
	*��������ȡ����Э��
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ����غ���Э�飬ʧ�ܷ��ؿմ�""
	*****************************************************************************/
	const char*GetCallProtocol(const char*cszCallID);

	/******************************************************************************
	* GetHasVideo
	*��������ȡ�Ƿ�ֻ����Ƶ
	*���룺cszCallID		-���б�ʶ
	*�������
	*����ֵ���������Ƶ����ô���ط�0�����򷵻�0
	*****************************************************************************/
	int GetHasVideo(const char*cszCallID);

	/******************************************************************************
	* SetEnableVideoTranscoding
	*�����������Ƿ�������Ƶת��
	*���룺nEnable		-�Ƿ�������Ƶת�롾0����������Ƶת�룬��0��������Ƶת�롿
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetEnableVideoTranscoding(int nEnable);

	/******************************************************************************
	* GetEnableVideoTranscoding
	*��������ȡ�Ƿ�������Ƶת��
	*���룺��
	*�������
	*����ֵ�����������Ƶת�룬��ô���ط�0�����򷵻�0
	*****************************************************************************/
	int GetEnableVideoTranscoding(void);

	/******************************************************************************
	* GetSecondVideoDirection
	*��������ȡ�����ķ���
	*���룺	cszCallID		-���б�ʶ
	*����� nDir			-�����ķ���
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir);
	bool ExcludePlayChan(unsigned long nAudioID,AUDEC_PlayChan*pPlayChan);


	unsigned short GetRTPBasePort(void);
	void SetRTPBasePort(unsigned short nRTPBasePort);

	int OpenLocalMainVideoCapDev(MediaCall*pMediaCall,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType);
	void OnRequestLocalMainVideoKeyFrame(MediaCall*pMediaCall);
	void CloseLocalMainVideoCapDev(MediaCall*pMediaCall);
	
	int OpenLocalSecondVideoCapDev(MediaCall*pMediaCall,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate);
	void OnRequestSecondVideoKeyFrame(MediaCall*pMediaCall);
	void CloseLocalSecondVideoCapDev(MediaCall*pMediaCall);


	//H235 Media use

	/******************************************************************************
	* GetH235AuthCheckState
	*��������ȡH235��֤���״̬ ����ȡ������֤��Ϊ����ʱ����֤���
	*���룺	cszCallID		  -���б�ʶ
	*����� nState -��֤���״̬
			nLength -������֤ʧ��ԭ���ַ�������
	*����ֵ���ɹ�������֤ʧ��ԭ��ʧ�ܷ��ؿ�
	*****************************************************************************/
	const char* GetH235AuthCheckState(const char*cszCallID,IMXEC_CALL_H235AUTH_STATE &nState,int &nLength);


	/******************************************************************************
	* OnChannelRecvMediaEncryption
	*����������ý��ͨ���Ѽ���״̬֪ͨ�����򿪼��ܵĽ���ý��ͨ��ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	nMediaType		-ý������
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnChannelRecvMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType);

	/******************************************************************************
	* OnChannelSendMediaEncryption
	*����������ý��ͨ���Ѽ���״̬֪ͨ�����򿪼��ܵķ���ý��ͨ��ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	nMediaType		-ý������
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnChannelSendMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType);



	/******************************************************************************
	* SetExternalIP
	*��������������ӳ��IP
	*���룺cszExternalIP		-����ӳ��IP
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetExternalIP(const char*cszExternalIP);
	const char*GetExternalIP(void);
	unsigned long GetExternalIPAddress(void);


	/******************************************************************************
	* OnDAP_RecvCallH245ControlMessage
	*���������յ�H245������Ϣ�ص� ����flowControlIndication H239������� �����Ϣ
	*���룺	cszCallID			-���б�ʶ
			nMsgType			-������Ϣ����
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_RecvCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen);

	//added by jiangdingfeng 2013-09-25 ����I֡����
	/******************************************************************************
	* RequestMainVideoKeyFrame
	*��������������I֡
	*���룺cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void RequestMainVideoKeyFrame(const char*cszCallID);
	/******************************************************************************
	* RequestSecondVideoKeyFrame
	*������������I֡
	*���룺cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void RequestSecondVideoKeyFrame(const char*cszCallID);

	/******************************************************************************
	* HasActiveCall
	*�������Ƿ��к���
	*���룺��
	*�������
	*����ֵ��BOOL
	*****************************************************************************/
	bool HasActiveCall();

	/******************************************************************************
	* SetLocalMainVideoSource
	*���������ñ�������ƵԴ
	*���룺cszCallID		  -���б�ʶ
	       nMainVideoDevID    -�����豸ID
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetLocalMainVideoSource(const char*cszCallID, int nMainVideoDevID);
	/******************************************************************************
	* SetLocalSecondVideoSource
	*���������ñ��ظ���ƵԴ
	*���룺cszCallID		  -���б�ʶ
		   nSecondVideoDevID  -�����豸ID
	*�������
	*����ֵ����
	*****************************************************************************/
	void SetLocalSecondVideoSource(const char*cszCallID, int nSecondVideoDevID);

	//add use to send private msg
	/******************************************************************************
	*����:	��Զ˷����Զ���Ǳ���Ϣ
	*����:	cszCallID		-���б�ʶ
			cszContent		-�Զ���Ǳ�׼��Ϣ����
			nContentLen		-�Զ���Ǳ�׼��Ϣ����
			cszMimeType		-�Զ���Ǳ���Ϣ�������� ��ΪXML ֵ��Ϊapplication/xml,��Ϊһ���ַ�������ΪNULL��""
	*****************************************************************************/
	int SendCallCustomMsg(const char*cszCallID,const char * cszContent,int nContentLen,const char * cszMimeType);

	/******************************************************************************
	* OnDAP_CallRecvNonstandardData
	*���������յ����ն˷���H245�Ǳ���Ϣʱϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszPacket		-�Ǳ�׼��Ϣ����
			nPacketLen		-�Ǳ�׼��Ϣ����
			cszMimeType		-Ŀǰ��SIPʹ�� ��Ϣ�������� ��application/xml
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_RecvNonstandardData(const char*cszCallID,const char * cszPacket,int nPacketLen,const char * cszMimeType);

	//����ͬʱֻ����һ��call�ں���
	bool IsCanCall();
	void SetCallNumZero();
	void SetCanCallNot();

	/******************************************************************************
	* GetCallH323ID
	*��������ȡH323ID
	*���룺	cszCallID		-���б�ʶ
            szH323ID        -H323ID Buf
			nMaxLen         -Buf ��󳤶�
	*�����	��
	*����ֵ����ȡ������H323ID���ȣ����򷵻�NULL
	*****************************************************************************/
	int GetCallH323ID(const char*cszCallID ,char * szH323ID,int nMaxLen);
	/******************************************************************************
	* GetCallE164ID
	*��������ȡE164ID
	*���룺	cszCallID		-���б�ʶ
            szE164ID        -E164ID Buf
			nMaxLen         -Buf ��󳤶�
	*�����	��
	*����ֵ����ȡ������E164ID���ȣ����򷵻�NULL
	*****************************************************************************/
	int GetCallE164ID(const char*cszCallID ,char * szE164ID,int nMaxLen);

	//Add U6 U7 use
	/******************************************************************************
	* OnDAP_RecvRemoteAudioRTPPacket
	*���������յ���Ƶý�����ص�
	*���룺	cszCallID			-���б�ʶ
	        codecID             -��Ƶ��������
			pRTPPacketData		-RTP��
			nRTPPacketLen		-RTP������
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP��ͷ����
			usSequence			-RTP������
			ulTimestamp			-RTP��ʱ���
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_RecvRemoteAudioRTPPacket(const char*cszCallID,X_AUDIO_CODEC_ID codecID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

	/******************************************************************************
	* OnDAP_RecvRemoteMainVideoRTPPacket
	*���������յ�������Ƶý�����ص�
	*���룺	cszCallID			-���б�ʶ
	        codecType           -��Ƶ��������
			pRTPPacketData		-RTP��
			nRTPPacketLen		-RTP������
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP��ͷ����
			usSequence			-RTP������
			ulTimestamp			-RTP��ʱ���
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_RecvRemoteMainVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	
	/******************************************************************************
	* OnDAP_RecvRemoteSecondVideoRTPPacket
	*���������յ�������Ƶý�����ص�
	*���룺	cszCallID			-���б�ʶ
	        codecType           -��Ƶ��������
			pRTPPacketData		-RTP��
			nRTPPacketLen		-RTP������
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP��ͷ����
			usSequence			-RTP������
			ulTimestamp			-RTP��ʱ���
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_RecvRemoteSecondVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);



	/******************************************************************************
	* OnPeerEntityInfo
	*���������ضԶ�ʵ����Ϣ
	*���룺	pCallParam			-CallParamSet ��Ϣ��
	*�������
	*����ֵ����
	*****************************************************************************/
    void OnPeerEntityInfo(const CallParamSet * pCallParam);


    /******************************************************************************
    * SetNetecTransEnable
    *�������Ƿ�ʹ��NETEC˽�д���
    *���룺	cszCallID			-���б�ʶ
            nEnable             -�Ƿ�ʹ��,1Ϊʹ��,0Ϊ��ʹ��
    *�������
    *����ֵ����
    *****************************************************************************/
    void SetNetecTransEnable(const char*cszCallID,int nEnable);

protected:
	typedef std::map<IMXEC_Call*,IMXEC_Call*>		MapIMXEC_Call;
	typedef std::map<std::string,std::string>		MapCallID;

	typedef std::map<std::string,ScreenCapChanRTP*>	MapScreenCapChanRTP;

protected:
	std::string				m_strEmpty;
	XCritSec				m_csMapIMXEC_Call;
	MapIMXEC_Call			m_MapIMXEC_Call;

	IMXEC_Call*				m_pIMXEC_CallExclusive;

	MapCallID				m_MapCallID;
	IMXEC_CALL_TYPE			m_nCallType;


	int						m_nAudioOnly;

	unsigned short			m_nRTPBasePort;



	int						m_nEnableVideoTranscoding;


	int						m_nMainVideoScreenIndex;
	int						m_nSecondVideoScreenIndex;

	XCritSec				m_csMapScreenCapChanRTPMain;
	MapScreenCapChanRTP		m_MapScreenCapChanRTPMain;

	XCritSec				m_csMapScreenCapChanRTPSecond;
	MapScreenCapChanRTP		m_MapScreenCapChanRTPSecond;

	std::string				m_strExternalIP;
	unsigned long			m_nExternalIPAddress;
	int						m_nCallNum;				//��ǰ�����call��

public:
	void SaveMediaReleasePort(MediaPortRelease & MediaPortRelease,unsigned short rtpPort);
	unsigned short GetMediaReleasePort(MediaPortRelease & MediaPortRelease);




	/******************************************************************************
	* OnDAP_CallAudioRecvChannelConnected
	*��������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
			AudioCodecID		-��ƵCodecID
			nPayloadType		-PayloadType
			ip					-���˽���rtp ip
			port				-���˽���rtp port
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallAudioRecvChannelConnected(const char*cszCallID,X_AUDIO_CODEC_ID AudioCodecID
		,int nPayloadType,unsigned long ip, int port);

	/******************************************************************************
	* OnDAP_CallAudioRecvChannelConnected
	*��������Ƶ����ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallAudioRecvChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallMainVideoRecvChannelConnected
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
			codecType			-��ƵCodecID
			nBitrate			-����
			nPayloadType		-PayloadType
			width				-�ֱ��ʿ�������� ��ѡ
			nHeight				-�ֱ��ʸ߶������� ��ѡ
			ip					-���˽���rtp ip
			port				-���˽���rtp port
			nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallMainVideoRecvChannelConnected(const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth=0,int nHeight=0,*/int nLevel=0);

	/******************************************************************************
	* OnDAP_CallMainVideoRecvChannelDestroyed
	*������������Ƶ����ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallMainVideoRecvChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallSecondVideoRecvChannelConnected
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
			codecType			-��ƵCodecID
			nBitrate			-����
			nPayloadType		-PayloadType
			width				-�ֱ��ʿ�������� ��ѡ
			nHeight				-�ֱ��ʸ߶������� ��ѡ
			ip					-���˽���rtp ip
			port				-���˽���rtp port
			nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallSecondVideoRecvChannelConnected(const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth=0,int nHeight=0,*/int nLevel=0);

	/******************************************************************************
	* OnDAP_CallSecondVideoRecvChannelDestroyed
	*������������Ƶ����ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallSecondVideoRecvChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallFECCRecvChannelConnected
	*������FECC���ƽ���ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
			nPayloadType		-PayloadType
			ip					-���˽���rtp ip
			port				-���˽���rtp port
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallFECCRecvChannelConnected(const char*cszCallID ,int nPayloadType,unsigned long ip
		, int port);

	/******************************************************************************
	* OnDAP_CallFECCRecvChannelDestroyed
	*������FECC���ƽ���ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallFECCRecvChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallAudioSendChannelConnected
	*��������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			AudioCodecID		-��ƵCodecID
			nPayloadType		-PayloadType
			ip					-�Զ�rtp ip
			port				-�Զ�rtp port
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallAudioSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,X_AUDIO_CODEC_ID AudioCodecID
		,int nPayloadType,unsigned long ip, int port);

	/******************************************************************************
	* OnDAP_CallAudioSendChannelDestroyed
	*��������Ƶ����ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallAudioSendChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallMainVideoSendChannelConnected
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			codecType			-��ƵCodecID
			nBitrate			-����
			nPayloadType		-PayloadType
			width				-�ֱ��ʿ�������� ��ѡ
			nHeight				-�ֱ��ʸ߶������� ��ѡ
			nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
			ip					-�Զ�rtp ip
			port				-�Զ�rtp port
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallMainVideoSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip,int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth,int nHeight,*/int nLevel = 0);

	/******************************************************************************
	* OnDAP_CallMainVideoSendChannelDestroyed
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallMainVideoSendChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallSecondVideoSendChannelConnected
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			codecType			-��ƵCodecID
			nBitrate			-����
			nPayloadType		-PayloadType
			width				-�ֱ��ʿ�������� ��ѡ
			nHeight				-�ֱ��ʸ߶������� ��ѡ
			nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
			ip					-�Զ�rtp ip
			port				-�Զ�rtp port
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallSecondVideoSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth,int nHeight,*/int nLevel = 0);
	
	/******************************************************************************
	* OnDAP_CallSecondVideoSendChannelDestroyed
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallSecondVideoSendChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallFECCSendChannelConnected
	*������FECC���Ʒ���ͨ�����ӳɹ��¼�
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			ip					-�Զ�rtp ip
			port				-�Զ�rtp port
			nPayloadType		-PayloadType
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallFECCSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID,int nPayloadType
		,unsigned long ip, int port);

	/******************************************************************************
	* OnDAP_CallFECCSendChannelDestroyed
	*������FECC���Ʒ���ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	void OnDAP_CallFECCSendChannelDestroyed(const char*cszCallID);

	void CheckMapCallEmpty(void);

	bool GetOneSaveLocalIP(std::string & strLocalIP);
	bool GetSameToDestIP(std::string & strLocalIP,std::string & strDestIP);

#ifdef _TIANZHU_CACHE_
	/******************************************************************************
	* SetRemoteVideoCache
	*����������Զ���Ӳ��ŵĻ���֡��
	*���룺	cszCallID			-���б�ʶ
	        nCacheFrameCount	-�����֡����0 - 60��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetRemoteVideoCache(const char*cszCallID, int nCacheFrameCount);
#endif

};

#endif
