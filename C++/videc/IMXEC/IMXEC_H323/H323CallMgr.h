#ifndef __H323CALLMGR_H__
#define __H323CALLMGR_H__

#include <string>
#include <map>
#include "H323Call.h"
#include "H323APIType.h"
#include "../DefineHeader/H323DefineHeader.h"
#include "CallMgr.h"

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
#include "XThreadBase.h"

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
#include "H323A7DAPCall.h"
#else 
#include "H323DAPCall.h"
#endif

#else
#include "H323MediaCall.h"
#endif

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#include <AVCONMP/IMXEC_H323.h>
#include <AVCONMP/IMXEC_Call.h>
#include <AVCONMP/AdapterHeaderDefine.h>

#else

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
#include <IMXEC/IMXEC_DAPH323.h>
#else
#include <IMXEC/IMXEC_H323.h>
#endif

#include <IMXEC/IMXEC_Call.h>
#include <IMXEC/AdapterHeaderDefine.h>

#endif


#include "XAutoLock.h"
#include "CallCapability.h"
#include "XBufferPool.h"
#include "StrPacket.h"
#include "H235Mgr.h"

#include "H323RegUser.h"



/////////////////////////////////////////////////////////////////////////

//���н�������DNS IP��
#define MAX_DNSIP_NUM   10

struct DNSIPItem
{
char strDNS[128];
char strIP[16];
};

struct DNSIP
{
	unsigned int	m_nNum;	//��Ч���ݸ���
	DNSIPItem		m_Array[MAX_DNSIP_NUM];

	DNSIP()
	{
		m_nNum = 0;
	}
};

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

class H323CallMgr 
	: public XBufferPool
	, public XThreadBase

#else

class H323CallMgr 
	: public XBufferPool

#endif

{
public:
	H323CallMgr();
	~H323CallMgr();
public:
	static H323CallMgr&Instance(void);
public:
	/******************************************************************************
	* SetH323ID
	*����������ע���ʺ�
	*���룺	cszH323ID		-ע���ʺ�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetH323ID(const char*cszH323ID);

	/******************************************************************************
	* SetTelNumber
	*���������õ绰����
	*���룺	cszTelNumber		-�绰����
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetTelNumber(const char*cszTelNumber);

	/******************************************************************************
	* SetRegPassword
	*����������ע������
	*���룺	cszRegPassword	-ע���ʺ�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetRegPassword(const char*cszRegPassword);

	/******************************************************************************
	* SetUserName
	*�����������û�����
	*���룺	cszUserName		-�û�����
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetUserName(const char*cszUserName);

	/******************************************************************************
	* SetRegHost
	*�����������û�����
	*���룺	cszHost			-ע�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetRegHost(const char*cszHost);

	/******************************************************************************
	* Register
	*������ע��
	*���룺nType			-ע�����͡�0:�Զ�����GK����������0:ע�ᵽָ����GK��������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int Register(int nType);

	/******************************************************************************
	* UnRegister
	*������ע��
	*���룺��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int UnRegister(void);

	/******************************************************************************
	* GetRegState
	*��������ȡע��״̬
	*���룺��
	*�����	nRegState			-ע��״̬
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetRegState(IMXEC_H323::REG_STATE&nRegState);

	/******************************************************************************
	* SetH323ListenPort
	*���������������˿�
	*���룺	nH323ListenPort			-�����˿�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetH323ListenPort(unsigned short nH323ListenPort);

	/******************************************************************************
	* SetLocalIP
	*����������ע���ʺ�
	*���룺	cszH323ID		-ע���ʺ�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetLocalIP(const char*cszLocalIP);

	/******************************************************************************
	* SetRTPBasePort
	*���������������˿�
	*���룺	nRTPBasePort			-RTP��ʼ�˿�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetRTPBasePort(unsigned short nRTPBasePort);

	/******************************************************************************
	* SetSupportH239
	*�����������Ƿ�֧��H239
	*���룺nSupportH239		-�Ƿ�֧��H239
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetSupportH239(int nSupportH239);
	int GetSupportH239(void){return m_nSupportH239;};

	int GetMainVideoDevID(void){return m_nMainVideoDevID;}; 
	int GetMainVideoDevFrameRate(void){return m_nMainVideoDevFrameRate;};
	VIDEC_CODEC_TYPE GetMainVideoCodecType(void){return m_nMainVideoCodecType;};
	VIDEO_FORMAT_TYPE GetMainVideoFormatType(void){return m_nMainVideoFormatType;};
	int GetSecondVideoDevID(void){return m_nSecondVideoDevID;}; 
	int GetSecondVideoDevFrameRate(void){return m_nSecondVideoDevFrameRate;};
	VIDEC_CODEC_TYPE GetSecondVideoCodecType(void){return m_nSecondVideoCodecType;};
	VIDEO_FORMAT_TYPE GetSecondVideoFormatType(void){return m_nSecondVideoFormatType;};
    int GetMainVideoNetworkType(void){return m_nMainVideoNetworkType;};
    int GetSecondVideoNetworkType(void){return m_nSecondVideoNetworkType;};

	/******************************************************************************
	* SetAudioDevParam
	*����������������Ƶ�豸����
	*���룺 nAudioCodecID		-��Ƶ����
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID);
	X_AUDIO_CODEC_ID GetAudioCodecID(void){return m_nAudioCodecID;};

	/******************************************************************************
	* SetMainVideoDevParam
	*����������������Ƶ�豸����
	*���룺 nDevID				-�豸��ʶ
			nFrameRate			-֡��
			nBitrate			-����
			nVideoCodecType		-��Ƶ����
			nVideoFormatType	-��Ƶ��ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType=VIDEC_NETWORK_PRIVATE);

	/******************************************************************************
	* SetSecondVideoDevParam
	*���������ø�����Ƶ�豸����
	*���룺 nDevID				-�豸��ʶ
			nFrameRate			-֡��
			nBitrate			-����
			nVideoCodecType		-��Ƶ����
			nVideoFormatType	-��Ƶ��ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType=VIDEC_NETWORK_PRIVATE);

	const std::string GetUserName(void){return m_strUserName;};
	unsigned short GetRTPBasePort(void);
	HAPP GetHApp(void){return m_hApp;};
	unsigned long GetLocalIPAddress(const std::string&strCallLocalIP);

	/******************************************************************************
	* MakeCall
	*�������������
	*���룺	cszUserID		-�Է����û�ID��IP
	*�������
	*����ֵ�����б�ʶ
	*****************************************************************************/
	const char*MakeCall(const char*cszUserID);

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


#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )

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

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


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
	virtual int GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

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


#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )

#else


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


#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)



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
	virtual int GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

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

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )

#else


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

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


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
	virtual int GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

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

	virtual int GetAudioMaxBitrate(void);
	virtual int GetMainVideoMaxBitrate(void);
	virtual int GetSecondVideoMaxBitrate(void);

	virtual int Start(void);//����
	virtual void Stop(void);			//ֹͣ


	//============ �ص����� ================
	//===ע��״̬�¼�
	virtual void OnRegistrationState(bool bState);

	//===����״̬�仯�¼�
	virtual void OnCallRingIn(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName);
	virtual void OnCallRingOut(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName);

	virtual void OnCallEstablished(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName);
	virtual void OnCallDestroyed(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName);

	virtual void OnCallSecondVideoStart(const std::string&strCallID);
	virtual void OnCallSecondVideoStop(const std::string&strCallID);
	virtual void OnCallLocalMainVideoStart(const std::string&strCallID);
	virtual void OnCallLocalMainVideoStop(const std::string&strCallID);

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2))
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

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
	virtual int StartRecordLocalMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordLocalMainToAVI
	*������ֹͣ��������¼��
	*���룺cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordLocalMainToAVI(const char*cszCallID);

	/******************************************************************************
	* StartRecordRemoteMainToAVI
	*��������ʼԶ������¼��
	*���룺	cszCallID			-���б�ʶ
			cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordRemoteMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordRemoteMainToAVI
	*������ֹͣԶ������¼��
	*���룺cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordRemoteMainToAVI(const char*cszCallID);

	/******************************************************************************
	* StartRecordSecondToAVI
	*��������ʼ����¼��
	*���룺	cszCallID			-���б�ʶ
			cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordSecondToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordSecondToAVI
	*������ֹͣ����¼��
	*���룺cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordSecondToAVI(const char*cszCallID);



#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


	/******************************************************************************
	* GetMainAudioID
	*��������ȡ������Ƶ��ʶ
	*���룺 cszCallID		-���б�ʶ
	*����� nMainAudioID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID);

	/******************************************************************************
	* GetMainVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 cszCallID		-���б�ʶ
	*����� nMainVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID);

	/******************************************************************************
	* GetSecondVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 cszCallID		-���б�ʶ
	*����� nSecondVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID);

	/******************************************************************************
	* SetMainVideoScreenIndex
	*����������������Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenIndex
	*���������ø�����Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex);


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
	virtual void SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

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
	virtual void SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

	//added by jiangdingfeng 2013-09-25 ����I֡����
	/******************************************************************************
	* RequestMainVideoKeyFrame
	*��������������I֡
	*���룺cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void RequestMainVideoKeyFrame(const char*cszCallID);
	/******************************************************************************
	* RequestSecondVideoKeyFrame
	*������������I֡
	*���룺cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void RequestSecondVideoKeyFrame(const char*cszCallID);

	/******************************************************************************
	* SetLocalMainVideoSource
	*���������ñ�������ƵԴ
	*���룺cszCallID		  -���б�ʶ
	       nMainVideoDevID    -�����豸ID
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoSource(const char*cszCallID, int nMainVideoDevID);
	/******************************************************************************
	* SetLocalSecondVideoSource
	*���������ñ��ظ���ƵԴ
	*���룺cszCallID		  -���б�ʶ
		   nSecondVideoDevID  -�����豸ID
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalSecondVideoSource(const char*cszCallID, int nSecondVideoDevID);


#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )

#else



	/******************************************************************************
	* SetLocalRecordCallback
	*���������ñ���¼��ص�
	*���룺	cszCallID		-���б�ʶ
			pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetRemoteRecordCallback
	*����������Զ��¼��ص�
	*���룺	cszCallID		-���б�ʶ
			pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetSecondRecordCallback
	*���������ø���¼��ص�
	*���룺	cszCallID		-���б�ʶ
			pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

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


#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)



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
	* SendDTMF
	*����������DTMF
	*���룺	cszCallID		-���б�ʶ
			nDTMF			-DTMF
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendDTMF(const char*cszCallID,const char nDTMF);

	/******************************************************************************
	* GetSecondVideoDirection
	*��������ȡ�����ķ���
	*���룺	cszCallID		-���б�ʶ
	*����� nDir			-�����ķ���
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir);
	
	bool ExcludePlayChan(unsigned long nAudioID,AUDEC_PlayChan*pPlayChan);

	void DoCheckPTZAction(void);

public:
	bool					m_bRegisteredToGK;
	int						m_nUseGK;
	HAPP					m_hApp;
	CallCapability			m_CallCapability;

public:
	bool HasCall();
	void AddCall(H323MediaCall*pH323MediaCall);
	void RemoveCall(H323MediaCall*pH323MediaCall);
	virtual void OnTimer(UINT32 nIDEvent);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	virtual void OnTimerDtmf(UINT32 nIDEvent);

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

public:
	typedef std::map<std::string,H323MediaCall*>	MapH323MediaCall;

	XCritSec				m_csMapH323MediaCall;					//������������
	MapH323MediaCall		m_MapH323MediaCall;						//�������Ĳ�����

private:

	XCritSec				m_csMapTempH323MediaCall;				//�̲߳��������
	MapH323MediaCall		m_MapTempH323MediaCall;					//�߳����Ĳ�����
	int						m_nStarted;
	std::string				m_strEmpty;
//test
public:
	std::string				m_strH323ID;							//H323ID;
	std::string				m_strTelNumber;							//E164;
	std::string				m_strUserName;
	std::string				m_strRegHost;
	std::string				m_strRegPassword;
	unsigned short			m_nH323ListenPort;
	unsigned short			m_nRTPBasePort;
	std::string				m_strLocalIP;

	//MC��ַ������
	std::string				m_strPeerMCUAddr;
	std::string				m_strConfID;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	UINT32					m_nIDDtmfTimer;

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


	UINT32					m_nIDEventTimer;

	int						m_nAudioMaxBitrate;
	X_AUDIO_CODEC_ID		m_nAudioCodecID;
	int						m_nMainVideoDevID;
	int						m_nMainVideoDevFrameRate;
	int						m_nMainVideoMaxBitrate;
	VIDEC_CODEC_TYPE		m_nMainVideoCodecType;
	VIDEO_FORMAT_TYPE		m_nMainVideoFormatType;
    int                     m_nMainVideoNetworkType;


	int						m_nSupportH239;
	int						m_nSecondVideoDevID;
	int						m_nSecondVideoDevFrameRate;
	int						m_nSecondVideoMaxBitrate;
	VIDEC_CODEC_TYPE		m_nSecondVideoCodecType;
	VIDEO_FORMAT_TYPE		m_nSecondVideoFormatType;
    int                     m_nSecondVideoNetworkType;

			
	int						m_nH245PortFrom;
	int						m_nH245PortTo;


private:
	void RegisterGK();

public:

	EncryptionCapability        m_EncryptionCapability;					//�ӽ���������
	int							m_nH235MediaUse;						//0 ��ʹ�� 1��ѡʹ�� 2ǿ��ʹ��H235ý��������
	int							m_nH235AuthUse;							//0 ��ʹ�� 1ʹ��
	int							m_nUseAlogrithmId;						//��ѡ�����㷨
	H235Mgr						m_h235Mgr;

	bool						m_bCheckIncoming;						//�Ƿ�Խ��յ��ĺ�����Ϣ���м����֤
	char						m_strPassword[64];						//��֤����
	char						m_strSenderId[256];						//���ͱ�ʶ�����˱�ʶ
	char						m_strGeneralId[256];					//���ձ�ʶ���Զ˱�ʶ
	bool						m_bH235AnnexI;                          //annex D I ��Ϣ�����Լ�������֤
	bool						m_bH235AnnexIA;                         //annex D IA ��������֤
	bool						m_bH235AnnexII;							//annex E II ���ù�Կ/˽Կ�Ե�����ǩ��
	bool						m_bH235AnnexIII;						//annex E III �˵�����֤
	bool						m_bH235AnnexIV;							//annex F ���IV ���ת�ӵİ�ȫ�� ����˹��I�͹��II
	bool						m_bUserPrimes;							//ý�������� ʹ��Ԥ���������������ɹ�����Կ
	bool						m_bH235MediaDH512;						//ý�������� ʹ��DH512����7�ֽڹ�����Կ ����ѡ�����DH���ɹ�����Կ
	bool						m_bH235MediaDH1024;						//ý�������� ʹ��DH1024����21�ֽڹ�����Կ
	bool						m_bH235MediaDH1536;						//ý�������� ʹ��DH1536����21�ֽڹ�����Կ

	int							m_nMediaEncrypSpecified;				//0 Ϊ��ʹ��ָ������(�Զ�Э��) 1 Ϊʹ��ָ������
	char						m_strMediaEncrypPWD[64];				//ʹ��ָ������ʱ���õļ�������

public:

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
	* SetH235MediaSecurityEncryptionPWD
	*������	����ý������������
	*���룺	type 0 Ϊ��ʹ��ָ������(�Զ�Э��) 1 Ϊʹ��ָ������
			encrypPwd ʹ��ָ������ʱ���õļ������� ����С��64�ֽ�
	*****************************************************************************/
	void SetH235MediaSecurityEncryptionPWD(int type=0,char * encrypPwd=NULL);

	//H235SecuritySettingInitialize
	void H235SecuritySettingInitialize();

	//ȡ�ӽ����㷨���� ����ISO OID 
	//return >0 - successful, =0 - failure.
	int GetEncDecAlgorithmTypeFromOID(EncDecAlgorithmID &id,unsigned char * pData,int dataLen);
	//return >0 - successful, =0 - failure.
	int GetEncDecAlgorithmOIDFromType(EncDecAlgorithmID algId,unsigned char * pData,int &dataLen);

	//H235 call ��ʼ��
	void H235SecurityCallInit(H323Call *pH323MediaCall);
	void H235SecurityCallEnd(H323Call *pH323MediaCall);

	//����H235��� startǰ����
	void SetH235AuthData();


	//����H235ѡ�� 0 ��ʹ�� 1ʹ��
	void SetH235SecurityAuthOption(int option);

	//����ý��������ѡ�� 0 ��ʹ�� 1��ѡʹ�� 2ǿ��ʹ��H235ý��������
	void SetH235MediaSecurityOption(int option);

	//����ý�������ȼ����㷨
	void SetH235MediaEncryptionAlogrithmFirst(int alogrimId);


	//����H235��֤��Ϣ senderId ���ͱ�ʶ�����˱�ʶ generalId���ձ�ʶ���Զ˱�ʶ password ��֤����
	//������senderId�����Է�������generalID ������generalID�����Է�������senderId
	void SetH235SecurityAuth(char * senderId,char * generalId,char * password);

	//���ÿ���״̬ Ԥ���������Ƿ�����ý����ܹ�����ԿЭ��
	void SetH235SecurityUserPrimeEnable(bool enable);
	//���öԽ��պ��е���Ϣ������֤���״̬
	void SetH235SecurityCheckIncomingMsgEnable(bool enable);

	//���ð�ȫ��֤ģʽ����״̬ ������Ϊ���п���
	//annex D I ��Ϣ�����Լ�������֤ ���ÿ���״̬
	void SetH235SecurityAnnexIEnable(bool enable);

	//annex D IA ��������֤ ���ÿ���״̬
	void SetH235SecurityAnnexIAEnable(bool enable);

	//annex E II ���ù�Կ/˽Կ�Ե�����ǩ�� ���ÿ���״̬
	void SetH235SecurityAnnexIIEnable(bool enable);

	//annex E III �˵�����֤ ���ÿ���״̬
	void SetH235SecurityAnnexIIIEnable(bool enable);

	//annex F ���IV �������ת�ӵİ�ȫ�� ����˹��I�͹��II ����״̬
	void SetH235SecurityAnnexIVEnable(bool enable);

	//ý�������� ʹ�ó���Ϊ512 DH����7�ֽڹ�����Կ ����ѡ�����DH���ɹ�����Կ
	//������DES���ܵ�ʹ��7�ֽ����ڵ�������ܵ��㷨
	void SetH235SecurityMediaDH512Enable(bool enable);

	//ý�������� ʹ�ó���Ϊ1024 DH����21�ֽڹ�����Կ ����DES 3DES���ܵ�
	void SetH235SecurityMediaDH1024Enable(bool enable);

	//ý�������� ʹ�ó���Ϊ1536 DH����21�ֽڹ�����Կ ����DES 3DES���ܵ�
	void SetH235SecurityMediaDH1536Enable(bool enable);


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	//H239 �������ƿ��Ƽ������շ�20121212����

public:
	XCritSec				m_csMapH239MediaCall;						//������ӦMAP������
	MapH323MediaCall		m_MapH239MediaCall;							//������ӦMAP
	bool					m_bTokenUseState;							//��������״̬ trueΪ��ʹ�ã�falseΪ����ʹ��
	H323MediaCall*			m_pKeepH323MediaCall;						//��¼����ӵ����
	H323MediaCall*			m_pRequestH323MediaCall;					//��¼����������
	//20130322
	H323MediaCall*			m_pVitualH323MediaCall;						//�����ն� ����MP��������

public:
	//�����������״̬������CALLָ����� ����trueΪ��ʹ�ã�falseΪ����ʹ��
	bool CheckH239TokenState(H323MediaCall*pH323MediaCall);

	//������ʹ��״̬������CALLָ�������bState trueΪ��ʹ�ã�falseΪ����ʹ��
	void SetH239TokenState(H323MediaCall*pH323MediaCall,bool bState);

	//�����еĻ���������ʹ��״̬��bState trueΪ��ʹ�ã�falseΪ����ʹ��
	void SetAllH239TokenState(bool bState);

	//���������ӦMAP�������CALLָ�����
	void AddH239MediaCallItem(H323MediaCall*pH323MediaCall);

	//ɾ��������ӦMAP�������CALLָ�����
	void DelH239MediaCallItem(H323MediaCall*pH323MediaCall);

	//���������ӦMAP������CALLָ�����
	void RemoveH239MediaCallAll(H323MediaCall*pH323MediaCall);

	//�㲥�������������з�����ӵ���ߣ�����CALLָ�����
	void BroadGenericRequest(H323MediaCall*pH323MediaCall);
	//�㲥�������������з�����ӵ���ߣ�����CALLָ�����
	void OnBroadGenericRequest(H323MediaCall*pH323MediaCall);

	void SendGenericResponse(H323MediaCall*pH323MediaCall,bool bAccept);

	//�㲥����ȡ�������з�����ӵ���ߣ�����CALLָ�����
	void BroadGenericCommand(H323MediaCall*pH323MediaCall);

	void BroadGenericIndication(H323MediaCall*pH323MediaCall);

	//�㲥���������з�����ӵ���ߣ�����CALLָ�����
	void BroadSecondVideoRTPPacket(H323MediaCall*pH323MediaCall,unsigned char*pRTPPacketData,int nRTPPacketLen);

	//�յ��������󣬴���CALLָ����� typeΪ1 ʱ����Ӧ,Ĭ�Ϸ�,  0ʱ����(���յ�ָʾ��Ͳ��跢��Ӧ)
	void OnRecvGenericRequest(H323MediaCall*pH323MediaCall, int terLabel, int chID, int symBreaking,int type=1);

	//�յ�����ȡ��������CALLָ�����
	void OnRecvGenericCommand(H323MediaCall*pH323MediaCall, int terLabel, int chID);

	//�յ�������Ӧ������CALLָ�����
	void OnRecvGenericResponse(H323MediaCall*pH323MediaCall, int response, int terLabel, int chID);

	void OnRecvGenericIndication(H323MediaCall*pH323MediaCall, int terLabel, int chID);

	//�յ�����������CALLָ�����
	void OnRecvdRemoteSecondVideoRTPPacket(H323MediaCall*pH323MediaCall,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

	//���ɾ������ӵ�������
	void DelH239TokenKeeper(H323MediaCall*pH323MediaCall);

	//�ҶϺ�����������״̬ɾ��������ӦMAP����
	void CheckDelH239MediaCallItem(H323MediaCall*pH323MediaCall);
	//��ͨ������������״̬������������
	void CheckSendGenericRequest(H323MediaCall*pH323MediaCall);

	//�����������״̬������ConferenceID���� ����trueΪ��ʹ�ã�falseΪ����ʹ��
	bool CheckH239TokenState(std::string & strConfID);

	//������ʹ��״̬������ConferenceID������bState trueΪ��ʹ�ã�falseΪ����ʹ��
	void SetH239TokenState(std::string & strConfID,bool bState);

	//�㲥�������������з�����ӵ���ߣ�����ConferenceID����
	void BroadGenericCommand(std::string & strConfID);

	//�㲥�������������з�����ӵ���ߣ�����ConferenceID����
	void BroadGenericRequest(std::string & strConfID);
	//�㲥�������������з�����ӵ���ߣ�����ConferenceID����
	void OnBroadGenericRequest(std::string & strConfID);

	//���������ӦMAP������ConferenceID����
	void RemoveH239MediaCallAll(std::string & strConfID);

	//�յ�����MP����������ʹ����Ϣ������ConferenceID����,MPID����
	void OnRecvMPGenericRequest(std::string strConfID,std::string strMPID);

	//�յ�����MP���������Ʋ���ʹ����Ϣ������ConferenceID����,MPID����
	void OnRecvMPGenericCommand(std::string strConfID,std::string strMPID);

	//�յ�����MP�������ٹ㲥�����з�����ӵ���ߣ�����ConferenceID����
	void OnRecvMPSecondVideoRTPPacket(std::string strConfID,unsigned char*pRTPPacketData,int nRTPPacketLen);

	//�ҶϺ�����б��Ƿ�Ϊ�� Ϊ���������Ʋ�����״̬������CALLָ�����
	void CheckSetH239TokenState(H323MediaCall*pH323MediaCall);


public:
	bool m_bH239ForceGetToken;
	bool m_bH239ForceGetResp;
	int  m_nH239ControlAgent;
	int  m_nH239MsgType;
	int  m_nH239MediaSend;
	bool m_bH239HasToken;
	bool m_bH239UseToken;
	bool m_bResponseAccept;
	bool m_bMPUseToken;							//MP��������
	bool m_bTermianlUseToken;					//��ͨ�ն���������

public:
	void InitH239VirtualCallState(void);
	void GetH239TokenForce(void);
	int  RecvExternalH239Media(H323MediaCall*pH323MediaCall,char*pRTPPacketData,int nRTPPacketLen);
	int  SendInternalH239Media(H323MediaCall*pH323MediaCall,char*pRTPPacketData,int nRTPPacketLen);
	int  OnH239TokenAssign(int nDir);
	int  OnH239TokenRelease(int nDir);
	void RecordH239MsgType(int nType){m_nH239MsgType=nType;};
	void SetH239MediaStatus(int nStatus){m_nH239MediaSend=nStatus;};
	void SetH239TokenUseState(bool bState){m_bH239UseToken=bState;};
	void SetH239TokenOwnState(bool bState){m_bH239HasToken=bState;};
	void SetH239ControlAgent(int nType){m_nH239ControlAgent=nType;};
	int  GetRecordH239MsgType(void){return m_nH239MsgType;};
	int  GetH239MediaStatus(void){return m_nH239MediaSend;};
	bool GetH239TokenUseState(void){return m_bH239UseToken;};
	bool GetH239TokenOwnState(void){return m_bH239HasToken;};
	int  GetH239ControlAgent(void){return m_nH239ControlAgent;};

public:
	//�յ��������󣬴���CALLָ�����
	void OnRecvH239GenericRequest(H323MediaCall*pH323MediaCall, int terLabel, int chID, int symBreaking);
	//�յ�����ȡ��������CALLָ�����
	void OnRecvH239GenericCommand(H323MediaCall*pH323MediaCall, int terLabel, int chID);
	//�յ�������Ӧ������CALLָ�����
	void OnRecvH239GenericResponse(H323MediaCall*pH323MediaCall, int response, int terLabel, int chID);
	void OnRecvH239GenericIndication(H323MediaCall*pH323MediaCall, int terLabel, int chID);

	void SendH239GenericRequest(H323MediaCall*pH323MediaCall);
	void SendH239GenericResponse(H323MediaCall*pH323MediaCall, bool bAccept);
	void SendH239GenericCommand(H323MediaCall*pH323MediaCall);
	void SendH239GenericIndication(H323MediaCall*pH323MediaCall);

	//���MP��ɫ��������״̬������CALLָ����� ����trueΪ��ʹ�ã�falseΪ����ʹ��
	bool CheckMPAgentTokenState(H323MediaCall*pH323MediaCall);
	//�����ͨ�ն˽�ɫ��������״̬������CALLָ����� ����trueΪ��ʹ�ã�falseΪ����ʹ��
	bool CheckTerminalAgentTokenState(H323MediaCall*pH323MediaCall);
	//��MP��ɫ����ʹ��״̬������CALLָ�������bState trueΪ��ʹ�ã�falseΪ����ʹ��
	void SetMPAgentTokenState(H323MediaCall*pH323MediaCall,bool bState);
	//����ͨ��ɫ����ʹ��״̬������CALLָ�������bState trueΪ��ʹ�ã�falseΪ����ʹ��
	void SetTerminalAgentTokenState(H323MediaCall*pH323MediaCall,bool bState);


#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

public:
	typedef std::map<int,H323RegUser*>	MapH323RegUser;
	unsigned long			m_nLastTimestampThread;								//�߳��м�ʱ
	unsigned long			m_nLastTimestampTimer;								//OnTimer�м�ʱ
	unsigned long			m_nLastTimestampCKACT;								//��ʱ���3��,��DoCheckPTZAction
	int						m_nRegGKTimestamp;									//�˺�ע��GK��ʱɨ����ʱ�� s Ĭ��2s

	MapH323RegUser			m_MapH323RegUser;									//ע���˺ű�
	XCritSec				m_csH323RegUser;									//ע���˺ű����

	unsigned short			GetH245BasePort(void);
	int						GetMasterSlaveValue(void);
	void					OutPutStackLog(int nLogType,const char * cszLog,char * szCallID=NULL);
	void					OutPutCapContent(const char*cszCallID,int nCapType,const char * cszContent);
	int						GetDAPCapPrintEnable(void);
	int						InsideRegisterToGK(cmRASTransaction rasTransact,const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword);
	void					TimerRegisterToGK(const char*cszHost=NULL,int type=0);	//��ʱע�������˺� type is 0 default,type is 1 restart

//add adapter
public:
	std::string								m_strProductID;
	std::string								m_strProductVersionID;
	int										m_nT35Code;
	int										m_nT35Extension;
	int										m_nManufacturerCode;
	int										m_nEndpontType;						//��ͨH323�ն���Ϊ0,H323MCU����Ϊ1
	int										m_nCapPrint;						//�����������Ϣ���� 0����ӡ 1��ӡ
	int										m_nMasterSlaveValue;				//MCU����Ϊ240��190 ,��ͨ�ն�����Ϊ50
	int										m_nGateKeeperPort;
	int										m_nAutoManualReg;					//0�Զ�ע��,1�ֶ�ע��
	cmTransportAddress						m_TransportAddr;
	bool									m_nFindGKAddr;
	bool									m_bRunning;
	std::string								m_strMCUID;

protected:
	virtual void							ThreadProcMain(void);

public:

	/******************************************************************************
	*�������趨ע�ᵽGK����
	*���룺nType	-ע������ �Զ�����GK��������Ϊ0,ע�ᵽָ����GK��������Ϊ��0ֵ
	*****************************************************************************/
	int SetTypeDAPRegisterToGK(int nType=1);

	////�����Զ�ע�ỹ���ֶ�ע�� 0�Զ�ע��,1�ֶ�ע��
	//int SetTypeDAPAutoManualRegGK(int nType=0);

	/******************************************************************************
	*������ע�ᵽGK ע���ʺź͵绰������������һ�� �趨�Զ�����GKʱcszHost����Ϊ""��NULL
	*���룺	cszH323ID			-ע���ʺ�
	        cszTelNumber		-�绰����
			cszHost				-ע���GK��������ַ
			cszRegPassword		-ע���˺�����
	*����ֵ���ɹ�����>=0[account id]��ʧ�ܷ���-1
	*****************************************************************************/
	int DAPRegisterToGK(const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword=NULL,int natFwOption=0);

	/******************************************************************************
	* UnRegisterToGK
	*��������GK��ע���˺�
	*���룺	nAccountID			-ע���ʺ�ID
	*����ֵ�����óɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int UnDAPRegisterToGK(int nAccountID);

	int SetDAPH245RangePort(unsigned short nFromPort,unsigned short nToPort);

	int SetDAPProductID(const char*cszProductID);

	int SetDAPProductVersionID(const char*cszVersionID);

	int SetDAPT35CountryCode(int nT35Code=88);

	int SetDAPT35Extension(int nT35Extension=0);

	int SetDAPManufacturerCode(int nManufacturerCode=1);

	//��ͨH323�ն���Ϊ0,H323MCU����Ϊ1
	int SetDAPH323EndPointType(int nType=0);

	int SendDAPPTZControl (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	int SetDAPCapPrintEnable(int nPrint=0);

	//MCU����Ϊ240��190 ,��ͨ�ն�����Ϊ50
	int SetDAPMasterSlaveValue(int nValue=50);

	int SetDAPLocalMainVideoParam(const char*cszCallID,int nFrameRate,int nBitrate);

	int SetDAPLocalSecondVideoParam(const char*cszCallID,int nFrameRate,int nBitrate);
	int SetH323CallSupportH239(const char*cszCallID,int nSupportH239);

	void SetUserNonstandardInfo(H323MediaCall *pH323MediaCall,cmCallState_e state,ConfInfo *pConfInfo=NULL,char * cszPacket=NULL);

	void GetCallUserNonstandardInfo(H323MediaCall *pH323MediaCall,ConfInfo *pConfInfo,char * cszPacket);

	int DAPStart(void);
	int DAPStop(void);

	void RegGKSetAutoManual(int type=0);					//0�Զ�ע��,1�ֶ�ע��

	int InsideStart(int fromPort,int toPort);

	const char* InsideMakeCall(const char*cszUserID,int nBandWidth=0,char * cszPacket=NULL,ConfInfo *pConfInfo=NULL);
	int InsideAcceptCall(const char*cszCallID,char * cszPacket=NULL,ConfInfo *pConfInfo=NULL);

	int DAPHangupCall(const char*cszCallID);

	int DAPGatekeeperRequest();

	/******************************************************************************
	*�������������
	*���룺	cszUserID		-�Է����û�ID��IP
			nBandWidth		-�ܴ���ֵ kbps
			cszPacket		-�Ǳ�׼��Ϣ����
	*****************************************************************************/
	const char* DAPMakeCall(const char*cszUserID,int nBandWidth,char * cszPacket,ConfInfo *pConfInfo);

	/******************************************************************************
	* AcceptCall
	*���������ܺ�������
	*���룺	cszCallID		-���б�ʶ
			cszPacket		-�Ǳ�׼��Ϣ����
	*****************************************************************************/
	int DAPAcceptCall(const char*cszCallID,char * cszPacket,ConfInfo *pConfInfo);


	/******************************************************************************
	* SetLocalMainVideoH264Level
	*���������ñ���������ƵH264����ʹ�õ����Levelֵ ���������ں��м���������ʱ������ʹ�ô�Levelֵ
	*���룺	cszCallID		-���б�ʶ
			nLevel			-��������ƵLevelֵ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetLocalMainVideoH264Level(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* SetLocalSecondVideoH264Level
	*���������ñ��ظ�����ƵH264����ʹ�õ����Levelֵ  ���������ں��м���������ʱ������ʹ�ô�Levelֵ
	*���룺	cszCallID		-���б�ʶ
			nLevel			-���ظ�����ƵLevelֵ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetLocalSecondVideoH264Level(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* SetLocalMainVideoH264HPLevel
	*���������ñ���������ƵH264HighProfile����ʹ�õ����Levelֵ  ���������ں��м���������ʱ������ʹ�ô�Levelֵ
	*���룺	cszCallID		-���б�ʶ
			nLevel			-��������ƵLevelֵ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetLocalMainVideoH264HPLevel(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* SetLocalSecondVideoH264HPLevel
	*���������ñ��ظ�����ƵH264HighProfile����ʹ�õ����Levelֵ  ���������ں��м���������ʱ������ʹ�ô�Levelֵ
	*���룺	cszCallID		-���б�ʶ
			nLevel			-���ظ�����ƵLevelֵ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetLocalSecondVideoH264HPLevel(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* SetCallRTPMediaMulticast 
	*����������RTP ý�����鲥 ���������鲥IP��������鲥
	*���룺	cszCallID		-���б�ʶ
			cszMulticastIP	-�鲥IP
			ttl				-�鲥���ݰ���TTL(����ʱ��)
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetCallRTPMediaMulticast(const char*cszCallID,const char*cszMulticastIP,int ttl=1);


	/******************************************************************************
	*������������Ƶý����
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			RTPPacketData		-RTP��
			PacketDataLen		-RTP������
	*****************************************************************************/
	int DAPSendCallAudio(HDAPAPPCALL hdapCall,const char*cszCallID,char* RTPPacketData, int PacketDataLen);

	/******************************************************************************
	*����������������Ƶý����
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			RTPPacketData		-RTP��
			PacketDataLen		-RTP������
	*****************************************************************************/
	int DAPSendCallMainVideo(HDAPAPPCALL hdapCall,const char*cszCallID,char* RTPPacketData, int PacketDataLen);

	/******************************************************************************
	*���������͸�����Ƶý����
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			RTPPacketData		-RTP��
			PacketDataLen		-RTP������
	*****************************************************************************/
	int DAPSendCallDualVideo(HDAPAPPCALL hdapCall,const char*cszCallID,char* RTPPacketData, int PacketDataLen);

	/******************************************************************************
	*����������PTZ��������̨�������
	*���룺	cszCallID		-���б�ʶ
			nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*****************************************************************************/
	int DAPSendPTZControl (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	/******************************************************************************
	*����������DTMF���β��Ű���ֵ ע��Ҳ��������ʵ��{ AUDEC_GetEngine().AUDEC_PlayDTMF(dtmf);} �ڲ�ʵ�ֲ��ٵ�AUDEC_GetEngine
	*���룺	cszCallID			-���б�ʶ
			dtmf				-����ֵ
	*****************************************************************************/
	int DAPSendCallDTMF(const char*cszCallID,const char dtmf);

	/******************************************************************************
	*��������Զ˷���H245�Ǳ���Ϣ
	*���룺	cszCallID		-���б�ʶ
			strPacket		-�Ǳ�׼��Ϣ����
	*****************************************************************************/
	int DAPSendCallH245NonstandardData(const char*cszCallID,std::string strPacket);

	/******************************************************************************
	* SendCallH245ControlMessage
	*����������H245������Ϣ ����flowControlIndication H239������� �����Ϣ
	*���룺	
			nMsgType			-������Ϣ����
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int DAPSendCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen);


	/******************************************************************************
	* SendRequestMainVideoKeyFrame
	*����������������Ƶ�ؼ�֡������Ϣ
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendRequestMainVideoKeyFrame(const char*cszCallID);

	/******************************************************************************
	* SendRequestSecondVideoKeyFrame
	*���������͸�����Ƶ�ؼ�֡������Ϣ
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendRequestSecondVideoKeyFrame(const char*cszCallID);

	/******************************************************************************
	* SendCallChannelMessage
	*�����������߼�ͨ����Ϣ ����FlowControlCommand videoFastUpdateGOB videoFastUpdateMB
	*���룺	cszCallID			-���б�ʶ
			nMsgType			-������Ϣ����
			cszPacket			-��Ϣ�� ����ý�������ֶ� �շ������ֶ�
			nPacketLen			-��Ϣ������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendCallChannelMessage(const char*cszCallID,H245ChanMessageType nMsgType,const char * cszPacket,int nPacketLen);

	/******************************************************************************
	* SendRASMessage
	*����������RAS��Ϣ nMsgType �ض���
	*���룺	nMsgType			-��Ϣ����
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SendRASMessage(H225RASMessageType nMsgType,const char * cszPacket,int nPacketLen);

	/******************************************************************************
	* GetCallQ931TransportAddr
	*��������ȡ����Q931ͨ��IP��ַ��Ϣ
	*���룺	cszCallID			-���б�ʶ
			szAddr				-IP��ַ�ַ���Buffer
			nPort				-ͨ�Ŷ˿�
			nLen				-IP��ַ�ַ�������
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetCallQ931TransportAddr(const char*cszCallID,char * szAddr,int &nPort,int &nLen);

	/******************************************************************************
	* GetCallH245TransportAddr
	*��������ȡ����H245ͨ��IP��ַ��Ϣ
	*���룺	cszCallID			-���б�ʶ
			szAddr				-IP��ַ�ַ���Buffer
			nPort				-ͨ�Ŷ˿�
			nLen				-IP��ַ�ַ�������
	*����� ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetCallH245TransportAddr(const char*cszCallID,char * szAddr,int &nPort,int &nLen);

	void SetMCUID(const char*cszMCUID);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	int OpenDualVideoOutChannel(const char*cszCallID);

	int OpenFECCOutChannel(const char*cszCallID);

	int CloseDualVideoOutChannel(const char*cszCallID);

	int CloseFECCOutChannel(const char*cszCallID);

	int AudioRecvChannelAnswer(const char*cszCallID);

	int MainVideoRecvChannelAnswer(const char*cszCallID);

	int DualVideoRecvChannelAnswer(const char*cszCallID);

	int FECCRecvChannelAnswer(const char*cszCallID);

	int AudioRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason);

	int MainVideoRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason);

	int DualVideoRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason);

	int FECCRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason);

#endif

	int DAPGetCallSupportSecondVideo(const char*cszCallID);

public:

	/******************************************************************************
	//*�������յ��Զ���Ƶ����ͨ�����¼�
	//*���룺	strCallID			-���б�ʶ
	//		AudioCodecID		-��ƵCodecID
	//		nPayloadType		-PayloadType
	//*****************************************************************************/
	virtual void OnDAP_CallAudioRecvChannelOpen(const std::string& strCallID,X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType);

	///******************************************************************************
	//*�������յ��Զ�������Ƶ����ͨ�����¼�
	//*���룺	strCallID			-���б�ʶ
	//		codecType			-��ƵCodecID
	//		nBitrate			-����
	//		nPayloadType		-PayloadType
	//		width				-�ֱ��ʿ�������� ��ѡ
	//		nHeight				-�ֱ��ʸ߶������� ��ѡ
	//		nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
	//*****************************************************************************/
	virtual void OnDAP_CallMainVideoRecvChannelOpen(const std::string& strCallID,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,int nWidth=0,int nHeight=0,int nLevel=0);

	///******************************************************************************
	//*�������յ��Զ˸�����Ƶ����ͨ�����¼�
	//*���룺	strCallID			-���б�ʶ
	//		codecType			-��ƵCodecID
	//		nBitrate			-����
	//		nPayloadType		-PayloadType
	//		width				-�ֱ��ʿ�������� ��ѡ
	//		nHeight				-�ֱ��ʸ߶������� ��ѡ
	//		nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
	//*****************************************************************************/
	virtual void OnDAP_CallSecondVideoRecvChannelOpen(const std::string& strCallID,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,int nWidth=0,int nHeight=0,int nLevel=0);

	///******************************************************************************
	//* OnDAP_CallFECCRecvChannelOpen
	//*�������յ��Զ�FECC���Ʒ���ͨ�����¼�
	//*���룺	strCallID			-���б�ʶ
	//			nPayloadType		-PayloadType
	//*****************************************************************************/
	virtual void OnDAP_CallFECCRecvChannelOpen(const std::string& strCallID,int nPayloadType);

	/******************************************************************************
	*��������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
			AudioCodecID		-��ƵCodecID
			nPayloadType		-PayloadType
			ip					-���˽���rtp ip
			port				-���˽���rtp port
	*****************************************************************************/
	virtual void OnDAP_CallAudioRecvChannelConnected(const std::string& strCallID,X_AUDIO_CODEC_ID AudioCodecID
		,int nPayloadType,unsigned long ip, int port);

	/******************************************************************************
	*��������Ƶ����ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*****************************************************************************/
	virtual void OnDAP_CallAudioRecvChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
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
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoRecvChannelConnected(const std::string& strCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth=0,int nHeight=0,int nLevel=0);

	/******************************************************************************
	*������������Ƶ����ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoRecvChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
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
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoRecvChannelConnected(const std::string& strCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth=0,int nHeight=0,int nLevel=0);

	/******************************************************************************
	*������������Ƶ����ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoRecvChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
	*������FECC���ƽ���ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
			nPayloadType		-PayloadType
			ip					-���˽���rtp ip
			port				-���˽���rtp port
	*****************************************************************************/
	virtual void OnDAP_CallFECCRecvChannelConnected(const std::string& strCallID ,int nPayloadType,unsigned long ip
		, int port);

	/******************************************************************************
	*������FECC���ƽ���ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*****************************************************************************/
	virtual void OnDAP_CallFECCRecvChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
	*��������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			AudioCodecID		-��ƵCodecID
			nPayloadType		-PayloadType
			ip					-�Զ�rtp ip
			port				-�Զ�rtp port
	*****************************************************************************/
	virtual void OnDAP_CallAudioSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID ,X_AUDIO_CODEC_ID AudioCodecID
		,int nPayloadType,unsigned long ip, int port);

	/******************************************************************************
	*��������Ƶ����ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*****************************************************************************/
	virtual void OnDAP_CallAudioSendChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
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
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip,int port,int nWidth,int nHeight,int nLevel = 0);

	/******************************************************************************
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoSendChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
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
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth,int nHeight,int nLevel = 0);
	
	/******************************************************************************
	*������������Ƶ����ͨ�����ӳɹ��¼�
	*���룺	cszCallID			-���б�ʶ
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoSendChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
	*������FECC���Ʒ���ͨ�����ӳɹ��¼�
	*���룺	HDAPAPPCALL hdapCall ʵ��call���
			cszCallID			-���б�ʶ
			ip					-�Զ�rtp ip
			port				-�Զ�rtp port
			nPayloadType		-PayloadType
	*****************************************************************************/
	virtual void OnDAP_CallFECCSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID,int nPayloadType
		,unsigned long ip, int port);

	/******************************************************************************
	* OnDAP_CallFECCSendChannelDestroyed
	*������FECC���Ʒ���ͨ���ر��¼�
	*���룺	cszCallID			-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallFECCSendChannelDestroyed(const std::string& strCallID);



	/******************************************************************************
	*���������յ���Ƶý�����ص�
	*���룺	cszCallID			-���б�ʶ
			pRTPPacketData		-RTP��
			nRTPPacketLen		-RTP������
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP��ͷ����
			usSequence			-RTP������
			ulTimestamp			-RTP��ʱ���
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteAudioRTPPacket(const char*cszCallID,X_AUDIO_CODEC_ID codecID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

	/******************************************************************************
	*���������յ�������Ƶý�����ص�
	*���룺	cszCallID			-���б�ʶ
			pRTPPacketData		-RTP��
			nRTPPacketLen		-RTP������
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP��ͷ����
			usSequence			-RTP������
			ulTimestamp			-RTP��ʱ���
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteMainVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	
	/******************************************************************************
	*���������յ�������Ƶý�����ص�
	*���룺	cszCallID			-���б�ʶ
			pRTPPacketData		-RTP��
			nRTPPacketLen		-RTP������
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP��ͷ����
			usSequence			-RTP������
			ulTimestamp			-RTP��ʱ���
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteSecondVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

	/******************************************************************************
	*���������յ�������Ƶ�ؼ�֡������Ϣ
	*���룺	cszCallID			-���б�ʶ
	*****************************************************************************/
	virtual void OnDAP_RecvMainVideoRequestKeyFrame(const char*cszCallID);

	/******************************************************************************
	*���������յ�������Ƶ�ؼ�֡������Ϣ
	*���룺	cszCallID			-���б�ʶ
	*****************************************************************************/
	virtual void OnDAP_RecvSecondVideoRequestKeyFrame(const char*cszCallID);

	/******************************************************************************
	*���������յ�H245������Ϣ�ص� ����flowControlIndication H239������� �����Ϣ
	*���룺	cszCallID			-���б�ʶ
			nMsgType			-������Ϣ����
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*****************************************************************************/
	virtual void OnDAP_RecvCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen);

	/******************************************************************************
	*���������յ������߼�ͨ����Ϣ�ص� ����FlowControlCommand videoFastUpdateGOB videoFastUpdateMB
	*���룺	cszCallID			-���б�ʶ
			nMsgType			-������Ϣ����
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*****************************************************************************/
	virtual void OnDAP_RecvCallChannelMessage(const char*cszCallID,H245ChanMessageType nMsgType,const char * cszPacket,int nPacketLen);

	/******************************************************************************
	*���������յ�RAS��Ϣ�ص� �ҽ�RAS��Ϣ
	*���룺	nMsgType			-��Ϣ����
			cszPacket			-��Ϣ��
			nPacketLen			-��Ϣ������
	*****************************************************************************/
	virtual void OnDAP_RecvRASMessage(H225RASMessageType nMsgType,const char * cszPacket,int nPacketLen);

	/******************************************************************************
	*������Э��ջ��־����ص�
	*���룺	nLogType			-��־���� 0 ������־ 1 ������־ 
			cszLog				-��־����
			nLen				-��־���ݳ���
	*****************************************************************************/
	virtual void OnDAP_OutPutStackLog(int nLogType,const char * cszLog,int nLen);

	/******************************************************************************
	*��������������ͨ��������������������ص� ��������ÿ���
	*���룺	cszCallID			-���б�ʶ
			nCapType			-������������ 0 Զ������������ 1 �������������� 2 Զ��ͨ��ͨ�������� 3 ����ͨ������������
			cszContent			-��������
			nLen				-�������ݳ���
	*****************************************************************************/
	virtual void OnDAP_OutPutCapContent(const char*cszCallID,int nCapType,const char * cszContent);

	/******************************************************************************
	*���������յ�PTZ�¼������Է�������̨��������ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*****************************************************************************/
	virtual void OnDAP_CallRecvdPTZControl(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);


	/******************************************************************************
	*�����������¼������ն˺��б�����ʱ��ϵͳ����ô��¼�
	*���룺	strCallID		-���б�ʶ
			strUserID		-�û���ʶ���û�IP
			strUserName		-�û�����
			strPacket		-�Ǳ�׼��Ϣ����
			strTelNum		-ע�����غ��� ������ָʾ����� δע������ʱΪ�մ�
	*****************************************************************************/
	virtual void OnDAP_CallRingIn(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,const std::string& strE164ID,const std::string& strPacket,std::string & strTelNum);

	/******************************************************************************
	*�����������¼�������������гɹ���ϵͳ����ô��¼�
	*���룺	strCallID		-���б�ʶ
			strUserID		-�û���ʶ���û�IP
			strUserName		-�û�����
	*****************************************************************************/
	virtual void OnDAP_CallRingOut(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName);

	/******************************************************************************
	*���������н����¼�����˫������ͨ���󡾱���������Զ�˵ĺ��������Զ�˽����˱����ĺ�������ϵͳ����ô��¼�
	*���룺	strCallID		-���б�ʶ
			strUserID		-�û���ʶ���û�IP
			strUserName		-�û�����
			strPacket		-�Զ˷Ǳ�׼��Ϣ����
	*****************************************************************************/
	virtual void OnDAP_CallEstablished(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,const std::string& strPacket);


	/******************************************************************************
	*���������������¼�����ͨ�������󡾱����Ҷϻ�Զ�˹Ҷϡ�ϵͳ����ô��¼�
	*���룺	strCallID		-���б�ʶ
			strUserID		-�û���ʶ���û�IP
			strUserName		-�û�����
			strReason		-����ԭ��
	*****************************************************************************/
	virtual void OnDAP_CallDestroyed(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,IMXEC_CALL_END_REASON nReason,const std::string& strReason );

	/******************************************************************************
	* OnDAP_CallH235CapSet
	*��������������������к��н���ʱ��ϵͳ������OnCallRingIn���ô��¼�, ���ں����������Ƿ�����ý��������
	*���룺	cszCallID		-���б�ʶ
			cszE164			-���е�E164�� û��ֵʱΪ�մ�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnDAP_CallH235CapSet(H323MediaCall*pH323MediaCall,const char * cszE164,ConfMediaSet & h235Set);


	/******************************************************************************
	*���������յ����ն˷���H245�Ǳ���Ϣʱϵͳ����ô��¼�
	*���룺	strCallID		-���б�ʶ
			strPacket		-�Ǳ�׼��Ϣ����
	*****************************************************************************/
	virtual void OnDAP_RecvNonstandardData(const std::string& strCallID,const std::string& strPacket);


	/******************************************************************************
	*������H323ע��ɹ��¼�
	*���룺	cszH323ID			-ע���ʺ�
			cszTelNumber		-�绰����
	*****************************************************************************/
	virtual void OnDAP_H323RegistrationSuccess(const char*cszH323ID,const char*cszTelNumber);

	/******************************************************************************
	*������H323ע��ʧ���¼�
	*���룺	cszH323ID			-ע���ʺ�
			cszTelNumber		-�绰����
			cszReason			-ʧ��ԭ��
	*****************************************************************************/
	virtual void OnDAP_H323RegistrationError(const char*cszH323ID,const char*cszTelNumber,char* cszReason=NULL);


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)


	/******************************************************************************
	* AddCapAudioAll
	*�����������Ƶ������������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioAll(const char*cszCallID);

	/******************************************************************************
	* RemoveCapAudioAll
	*�������Ƴ���Ƶ������������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int RemoveCapAudioAll(const char*cszCallID);

	/******************************************************************************
	* AddCapG711A_64K ע,��ý�������������������������
	*�����������ƵG711A������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG711A_64K(const char*cszCallID);

	/******************************************************************************
	* AddCapG711U_64K
	*�����������ƵG711U������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG711U_64K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG722_64K
	*�����������ƵG722-64K������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG722_64K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG7221_24K
	*�����������ƵG7221-24K������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG7221_24K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG7221_32K
	*�����������ƵG7221-32K������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG7221_32K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG7221C_24K
	*�����������ƵG7221C-24K������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG7221C_24K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG7221C_32K
	*�����������ƵG7221C-32K������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG7221C_32K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG7221C_48K
	*�����������ƵG7221C-48K������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG7221C_48K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG729A
	*�����������ƵG729A������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG729A(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG719_32K
	*�����������ƵG719-32K������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG719_32K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG719_48K
	*�����������ƵG719-48K������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG719_48K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG719_64K
	*�����������ƵG719_64K������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapAudioG719_64K(const char*cszCallID);

	/******************************************************************************
	* AddCapVideoAll
	*���������������Ƶ������������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoAll(const char*cszCallID);

	/******************************************************************************
	* RemoveCapVideoAll
	*�������Ƴ�������Ƶ������������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int RemoveCapVideoAll(const char*cszCallID);

	/******************************************************************************
	* AddCapH263
	*���������������ƵH263������
	*���룺	cszCallID		-���б�ʶ
			nHasQCIF		-ʹ��QCIF�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasCIF			-ʹ��CIF�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHas4CIF		-ʹ��4CIF�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH263(const char*cszCallID,int nHasQCIF=0,int nHasCIF=0,int nHas4CIF=0);

	/******************************************************************************
	* AddCapVideoH263Plus
	*���������������ƵH263+������
	*���룺	cszCallID		-���б�ʶ
			nHasVGA			-ʹ��VGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSVGA		-ʹ��SVGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasXGA			-ʹ��XGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSXGA		-ʹ��SXGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHas720P		-ʹ��720P�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH263Plus(const char*cszCallID,int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapVideoH263PlusPlus
	*���������������ƵH263++������
	*���룺	cszCallID		-���б�ʶ
			nHasVGA			-ʹ��VGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSVGA		-ʹ��SVGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasXGA			-ʹ��XGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSXGA		-ʹ��SXGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHas720P		-ʹ��720P�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH263PlusPlus(const char*cszCallID,int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapVideoH264QCIF
	*���������������Ƶ��H264QCIF������
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH264QCIF(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264CIF
	*���������������Ƶ��H264CIF������
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH264CIF(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapVideoH2644CIF
	*���������������Ƶ��H264 4CIF������
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH2644CIF(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264720P
	*���������������Ƶ��H264720P������
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH264720P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapVideoH2641080P
	*���������������Ƶ��H2641080P������
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH2641080P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264HP720P
	*���������������Ƶ��H264HighProfile������,Ĭ�����720P
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨��������Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH264HP720P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264HP1080P
	*���������������Ƶ��H264HighProfile������,Ĭ�����1080P
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨��������Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapVideoH264HP1080P(const char*cszCallID,int nLevel=0);


	/******************************************************************************
	* AddCapDualVideoAll
	*��������Ӹ�����Ƶ������������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoAll(const char*cszCallID);

	/******************************************************************************
	* RemoveCapDualVideoAll
	*�������Ƴ�������Ƶ������������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int RemoveCapDualVideoAll(const char*cszCallID);

	/******************************************************************************
	* AddCapDualVideoH263
	*��������Ӹ�����ƵH263������
	*���룺	cszCallID		-���б�ʶ
			nHasQCIF		-ʹ��QCIF�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasCIF			-ʹ��CIF�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHas4CIF		-ʹ��4CIF�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH263(const char*cszCallID,int nHasQCIF=0,int nHasCIF=0,int nHas4CIF=0);

	/******************************************************************************
	* AddCapDualVideoH263Plus
	*��������Ӹ�����ƵH263+������
	*���룺	cszCallID		-���б�ʶ
			nHasVGA			-ʹ��VGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSVGA		-ʹ��SVGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasXGA			-ʹ��XGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSXGA		-ʹ��SXGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHas720P		-ʹ��720P�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH263Plus(const char*cszCallID,int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapDualVideoH263PlusPlus
	*��������Ӹ�����ƵH263++������
	*���룺	cszCallID		-���б�ʶ
			nHasVGA			-ʹ��VGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSVGA		-ʹ��SVGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasXGA			-ʹ��XGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHasSXGA		-ʹ��SXGA�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
			nHas720P		-ʹ��720P�ֱ��� ��ʹ������Ϊ0��ʹ������Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH263PlusPlus(const char*cszCallID,int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapDualVideoH264QCIF
	*��������Ӹ�����Ƶ��H264QCIF������
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH264QCIF(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264CIF
	*��������Ӹ�����Ƶ��H264CIF������
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH264CIF(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH2644CIF
	*��������Ӹ�����Ƶ��H264 4CIF������
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH2644CIF(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264720P
	*��������Ӹ�����Ƶ��H264720P������
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH264720P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH2641080P
	*��������Ӹ�����Ƶ��H2641080P������
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨�˷ֱ��ʶ�Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH2641080P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264HP720P
	*��������Ӹ�����Ƶ��H264HighProfile������,Ĭ�����720P
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨��������Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH264HP720P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264HP1080P
	*��������Ӹ�����Ƶ��H264HighProfile������,Ĭ�����1080P
	*���룺	cszCallID	-���б�ʶ
			nLevel		-�趨��������Ӧ��H264levelֵ,��nLevel����0ʱʹ�ô�ֵ,��ѡ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapDualVideoH264HP1080P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* RemoveCapAll
	*�������Ƴ�����ý�����͵�����������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int RemoveCapAll(const char*cszCallID);

	/******************************************************************************
	* RemoveCapFECCDataAll
	*�������Ƴ�FECC��������������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int RemoveCapFECCDataAll(const char*cszCallID);

	/******************************************************************************
	* AddCapFECCData
	*���������FECC����������
	*���룺	cszCallID	-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int AddCapFECCData(const char*cszCallID);
#endif

private:
	void SetDataBufferChannelDestroyed(const std::string& strCallID,std::string strCMD,std::string strMedia);
	void SetDataBufferChannelConnectedVD(const std::string& strCallID ,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth,int nHeight,int nLevel,std::string strCMD,std::string strMedia,HDAPAPPCALL hdapCall=NULL);



#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

public:
	void StartVideoToCall(const char*cszCallID,unsigned long nVideoID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
	void StopVideoToCall(const char*cszCallID);
	void StartH239ToCall(const char*cszCallID,unsigned long nVideoID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
	void StopH239ToCall(const char*cszCallID);
	void StartAudioToCall(const char*cszCallID,unsigned long nAudioID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
	void StopAudioToCall(const char*cszCallID);

#endif

public:
	unsigned long	m_nLastSendTripTimestamp;
	std::string		m_strProName;
	int				m_nRegAccountNum;
private:
	void DoCheckRoundTrip(void);
	void TimerSendRoundTripDalay(H323MediaCall * pH323MediaCall,int type=1);
	bool TimerCheckRoundTripDalay(H323MediaCall* pH323MediaCall,int type=1);  ////0 terminal use, 1 mp use or mcu use

public:
	int HangupCallPointer(H323MediaCall*pH323MediaCall,int type =1); //type 1 is check media hungup , 0 is terminal ras URQ hungup ,2 �������ٲ��ϹҶ�
	H323RegUser* FindH323RegAccount(std::string strID);
	int AddH323RegAccount(cmRASTransaction rasTransact,const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword,int natFwOption);
	void RemoveH323RegAccount(int nLineID);

	//A7 add
public:
	virtual void OnMainVideoFlowControlCommand(const std::string&strCallID, unsigned int nBitRate);
	virtual void OnSecondVideoFlowControlCommand(const std::string&strCallID, unsigned int nBitRate);
	// ����Զ˷���˫��
	void AllowToSendSecondVideo(const char*cszCallID, bool bAllow);
	// ����������
	void SendFlowControlCommand(const char*cszCallID, IMXEC_CALL_MEDIA_TYPE emMediaType, int nMaximumBitRate);
	//�ն���,�Ƿ�����H460 NAT��͸, 0 not use ,1 use,default 0
	void SetH460NATFwOption(int option);
	void SendChannelKeepAlive(void);
	void HungupAll(void);
	void OnDAP_CallDestroyed(H323MediaCall*pH323MediaCall);

public:

	HH323FWNATMGR		m_hFwNat;					//add for 460client
	int					m_nUseTerminalFWNAT;		//�ն���,�Ƿ�����H460 NAT��͸, 0 not use ,1 use,default 0
	int					m_nUseMCUFWNAT;				//MCU��,�Ƿ�Ĭ�Ϸ��ʹ򶴰�,  0 not use ,1 use,default 1
	int					m_nSendKeepliveSeconds;		//NATͨ����ʱ����ʱ�� Ĭ��10��
	unsigned long		m_nSendKeepliveTimestamp;
	int					m_nRegStatusTimestamp;		//�ն��� �˺�ע����ʱ���ʱ
	int					m_nRegGKPort;		        //�ն���
	int					m_nTimeToLive;		        //�ն��� s
	bool				m_bRegToGKStateOld;         //�ն���
	int                 m_nRASCastPort;
	int                 m_nRASLocalPort;
	std::string         m_strLastGKHost;            //MCU�� �����ϴ����õ�GK��ַ
	int 				m_nSendURQ;                 //�ն��� ����Э��ջ����Ʋ�����URQ�Ĵ���
	int 				m_nMaxCallNum;              //MCU��  ������������
	//int 				m_nIPCPort;
	int                 m_nDoCheckPTZCount;
	int                 m_nCheckNetCardDisable;     //����������ߺ�Ĵ���
	HWND                m_hTestWnd;                 //use test
	std::string         m_strH225LogicalKA;         //H460 H225ͨ������
	int                 m_bRoundTripDiscMaxCount;   //�ҶϺ���û�յ��ػ�������Ϣ��������

public:

	MediaPortRelease    m_MediaPortRelease;
	int                 m_nREGRejectReason;         //�ն��� ����ע��ʧ��ԭ����

	void SaveMediaReleasePort(unsigned short rtpPort);
	void OnCallH245CapChanged (const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,unsigned int nMediaType);

	//��ȡH323ID �ն��� ��ȡ������H323ID���ȣ����򷵻�NULL
	int GetCallH323ID(const char*cszCallID ,char * szH323ID,int nMaxLen);

	//��ȡE164ID �ն��� ��ȡ������E164ID���ȣ����򷵻�NULL
	int GetCallE164ID(const char*cszCallID ,char * szE164ID,int nMaxLen);

	//����H323ע����ض˿�
	int SetH323RASPort(unsigned short nLocalPort,unsigned short nRemotePort,unsigned short nCastPort);

	void H323SettingFromINI();
	void DAPCheckRasModule(const char*cszHost);
	//�ն��� STOP������Ʒ���URQ��һ��
	RvBool CheckRASURQSend();
	void SetMediaPortArrayEmpty();
	bool H323CallMgr::TimerCheckNetCardDisable(H323MediaCall * pH323MediaCall,int type=1);

	//1016 add
	DNSIP m_DNSIP;
	bool IsDNS(const char * strDNS);
	bool GetDNSIPPart1(const char * strDNS,char * DNSIP,int & nLen);
	bool GetDNSIP(const char * strDNS,char * DNSIP,int & nLen);
	void SaveToDNSIP(char * strDNS,char * strIP);
	bool FindDNSIP(char * strDNS,char * strIP);
	//�ص��Ҷ�״̬ʱ�ټ�����ǲ�����Ϊ�����ղ���ý����ԭ��Ҷϵ�
	bool DisconnectCheckRoundTripDalay(H323MediaCall * pH323MediaCall);

	void  OnCallRTPChanged(const std::string& strCallID,H323Channel * pChannel,unsigned int nType);
	//���¼�����������H323�����������߳��е��û����ڴ�й©���
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) 
#else 
	DWORD m_dwThreadID;
#endif

	bool  m_bThreadH323Attach;
	int   m_nCallerRate;            //���к������� ����MCU���������������
	int   m_nTerminalCapsType;      //1 use terminal caps ,2 use mcu confcaps
	int   m_nUseMediaNetRTP;        //1 use medianet rtp  ,0 not use 
	int   m_nSendFlowCC;            //1 send flowControlCommand  ,0 not send 
	bool  m_bNETECNodeToGK;         //NETEC node connect to gk NETEC MCU
	int   m_nNETECNodeCount;        //NETECNode Create Count

	void  OnCallKeyFrameRequest(const std::string& strCallID,unsigned nType);  //nType 1 main 2 dualvideo

public:
	void  AddQ931CallPartyNumber(HCALL hsCall,const char * cszUserID,cmCallParam callParam,char * cszPartyNumber);
	int   GetH323IDAlias(char * szH323ID,cmAlias & termAlias,char * bmpStr);
	int   GetE164IDAlias(char * szTelNumber,cmAlias & termAlias,char * bmpStr);

	bool  CheckMapCallEmpty(void);
	//mcu use ,support dtmf ,���������Ļ�������
	int   DapChanegCallCaps(const char*cszCallID,ConfInfo *pConfInfo);
	void  SetMCUInfo();
	//gk mcu use 20150409 
	void  NETECStopToGK();
	void  NETECStartToGK(void);
	int   SetBandWidth1(ConfInfo * pConInfo);
	int   SetBandWidth2(ConfInfo * pConInfo);

};




#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


#if (_MSC_VER && _MSC_VER<=1200) || (__BORLANDC__) 
#define __FUNCTION__ ""
#endif

//sprintf(strDebugMsg,"File %s,Func %s,Line %d .\n",__FILE__,__FUNCTION__,__LINE__); \
//sprintf(strDebugMsg,"File %s,Func %s,Line %d,Debug %s .\n",__FILE__,__FUNCTION__,__LINE__,(char *)strDebugINFO); \

#if 1
#define PrintDebugString() \
{ \
	char strDebugMsg[1024]; \
	sprintf(strDebugMsg,"Func %s,Line %d .\n",__FUNCTION__,__LINE__); \
	OutputDebugString(strDebugMsg); \
	LogFile(NULL,NULL,"","",strDebugMsg); \
}

#define PrintDebugStringInfo(strDebugINFO) \
{ \
	char strDebugMsg[1024]; \
	sprintf(strDebugMsg,"Func %s,Line %d,Debug %s .\n",__FUNCTION__,__LINE__,(char *)strDebugINFO); \
	OutputDebugString(strDebugMsg); \
	LogFile(NULL,NULL,"","",strDebugMsg); \
}

#define PrintDebugStringInfo2(strDebugINFO1,strDebugINFO2) \
{ \
	char strDebugMsg[1024]; \
	sprintf(strDebugMsg,"Func %s,Line %d,Debug %s,%s .\n",__FUNCTION__,__LINE__,(char *)strDebugINFO1,(char *)strDebugINFO2); \
	OutputDebugString(strDebugMsg); \
	LogFile(NULL,NULL,"","",strDebugMsg); \
}

#define PrintDebugStringInfo3(strDebugINFO1,strDebugINFO2,strDebugINFO3) \
{ \
	char strDebugMsg[1024]; \
	sprintf(strDebugMsg,"Func %s,Line %d,Debug %s,%s,%s .\n",__FUNCTION__,__LINE__,(char *)strDebugINFO1,(char *)strDebugINFO2,(char *)strDebugINFO3); \
	OutputDebugString(strDebugMsg); \
	LogFile(NULL,NULL,"","",strDebugMsg); \
}

#else

#define PrintDebugString()

#define PrintDebugStringInfo(strDebugINFO) \
{ \
}

#define PrintDebugStringInfo2(strDebugINFO1,strDebugINFO2) \
{ \
}

#define PrintDebugStringInfo3(strDebugINFO1,strDebugINFO2,strDebugINFO3) \
{ \
}

#endif	//#if 1

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


#endif	//#ifndef __H323CallMgr_H__
