#ifndef __SIPCALLMGR_H__
#define __SIPCALLMGR_H__

#include "../DefineHeader/SIPDefineHeader.h"
#include "SIPUser.h"
#include "XThreadBase.h"
#include "pjsip.h" 
#include "SIPCallCapability.h"
#include "XBufferPool.h"
#include "StrPacket.h"
#include "CallMgr.h"

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#include "SIPDAPCall.h"
#else
#include "SIPMediaCall.h"
#endif

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1)

#include <AVCONMP/IMXEC_SIP.h>
#include <AVCONMP/IMXEC_Call.h>

#else

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
#include <IMXEC/IMXEC_DAPSIP.h>
#else
#include <IMXEC/IMXEC_SIP.h>
#endif

#include <IMXEC/IMXEC_Call.h>
#endif


#include <vector>
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>





#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

class SIPCallMgr 
	: public XBufferPool
	, public XThreadBase

#else
class SIPCallMgr 
	: public XBufferPool

#endif

{
public:
	SIPCallMgr(void);
	virtual ~SIPCallMgr(void);
public:
	static SIPCallMgr&Instance(void);

public:
	int m_nLogModuleEnabled ;

public:
	//���SIP�ı���Ϣ
	void StackDisplayMsg(char * msg,int type=0);//type=1 system msg
	SipConfigStruct					m_sipConfig;
	SIPCallCapability				m_sdpCap;
	XCritSec						m_csSdpCap;

	SIPMediaCall*  GetSIPMediaCallFromCallID(const char*cszCallID);
	std::string GetHostAddress(void);
	//use to ip call, strHost ������δע����� IP����ʱ
	SIPMediaCall* CreateSIPMediaCall(SIPUser * pSIPUser,std::string callID,std::string strHost) ;

	void OnUserIdRegistration(int nAccountId,int status,const std::string&strErrorCode);
	SIPUser * GetSIPUserFromAccountID(int accountID) ;

public:
	/******************************************************************************
	* AddVirtualLine
	*���������������·
	*���룺	cszUserID		-�û��ʺ�
			cszUserName		-�û�����
			cszPassword		-�û�����
			cszHost			-SIP���������
	*�������
	*����ֵ���ɹ�����>=0[line id]��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int AddVirtualLine(const char*cszUserID,const char*cszUserName,const char*cszPassword,const char*cszSIPProxy,const char*cszAuthUserID,const char*cszDomain);

	/******************************************************************************
	* RemoveVirtualLine
	*������ɾ��������·
	*���룺	nLineID			-��·��ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void RemoveVirtualLine(int nLineID);

	virtual int SetCurrentVirtualLine(int nLineID);
	virtual int GetCurrentVirtualLine(void);
	virtual bool HasVirtualLineRegistrationSuccess(void);
	virtual int IsVirtualLineRegistrationSuccess(int nLineID);
	virtual int IsCurrentVirtualLineRegistrationSuccess(void);
	virtual int IsVirtualLineRegistrationError(int nLineID);
	virtual int IsCurrentVirtualLineRegistrationError(void);
	virtual const char*GetVirtualLineRegistrationErrorCode(int nLineID);
	virtual const char*GetCurrentVirtualLineRegistrationErrorCode(void);

	/******************************************************************************
	* SetSIPListenPort
	*���������������˿�
	*���룺	nSIPListenPort			-�����˿�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetSIPListenPort(unsigned short nSIPListenPort);
	virtual unsigned short GetSIPListenPort(void);

	/******************************************************************************
	* SetLocalIP
	*����������ע���ʺ�
	*���룺	cszSIPID		-ע���ʺ�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetLocalIP(const char*cszLocalIP);
	std::string m_strLocalIP;
	std::string GetLocalIP();
	/******************************************************************************
	* SetRTPBasePort
	*���������������˿�
	*���룺	nRTPBasePort			-RTP��ʼ�˿�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetRTPBasePort(unsigned short nRTPBasePort);
	virtual unsigned short GetRTPBasePort(void);

	/******************************************************************************
	* SetAudioDevParam
	*����������������Ƶ�豸����
	*���룺 nAudioCodecID		-��Ƶ����
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID);
	virtual X_AUDIO_CODEC_ID GetAudioCodecID(void){return m_nAudioCodecID;};

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
	virtual int SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType=VIDEC_NETWORK_PRIVATE);
	virtual int GetMainVideoDevID(void){return m_nMainVideoDevID;};
	virtual int GetMainVideoFrameRate(void){return m_nMainVideoFrameRate;};
	virtual int GetMainVideoBitrate(void){return m_nMainVideoBitrate;};
	virtual VIDEC_CODEC_TYPE GetMainVideoCodecType(void){return m_nMainVideoCodecType;};
	virtual VIDEO_FORMAT_TYPE GetMainVideoFormatType(void){return m_nMainVideoFormatType;};
    virtual int GetMainVideoNetworkType(void){return m_nMainVideoNetworkType;};


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
	virtual int SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType=VIDEC_NETWORK_PRIVATE);
	virtual int GetSecondVideoDevID(void){return m_nSecondVideoDevID;}; 
	virtual int GetSecondVideoDevFrameRate(void){return m_nSecondVideoDevFrameRate;};
	virtual int GetSecondVideoMaxBitrate(void){return m_nSecondVideoMaxBitrate;};
	virtual VIDEC_CODEC_TYPE GetSecondVideoCodecType(void){return m_nSecondVideoCodecType;};
	virtual VIDEO_FORMAT_TYPE GetSecondVideoFormatType(void){return m_nSecondVideoFormatType;};
    virtual int GetSecondVideoNetworkType(void){return m_nSecondVideoNetworkType;};

	/******************************************************************************
	* SetSupportH239
	*�����������Ƿ�֧��H239
	*���룺nSupportH239		-�Ƿ�֧��H239
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetSupportH239(int nSupportH239);
	int GetSupportH239(void){return m_nSupportH239;};
	int						m_nSupportH239;

	/******************************************************************************
	* GetSecondVideoDirection
	*��������ȡ�����ķ���
	*���룺	cszCallID		-���б�ʶ
	*����� nDir			-�����ķ���
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir);
	
	/******************************************************************************
	* Start
	*����������SIP����
	*���룺��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int Start(void);

	/******************************************************************************
	* Stop
	*������ֹͣSIP����
	*���룺��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int Stop(void);


	virtual bool GetCallSDP(const std::string&strCallID,std::string&strLocalIP,unsigned short&nLocalAudioPort,unsigned short&nLocalVideoPort);
	virtual void OnMediaUpdate(const std::string&strCallID,const std::string&strRemoteIP,unsigned short nRemoteAudioPort,int nAudioPayloadType,const std::string&strAudioPayloadName,int nAudioSampleRate,X_AUDIO_CODEC_ID AudioCodecID,unsigned short nRemoteVideoPort,int nVideoPayloadType,const std::string&strVideoPayloadName,int nVideoBitrate,int nVideoWidth,int nVideoHeight);


	/******************************************************************************
	* MakeCall
	*�������������
	*���룺	cszUserID		-�Է����û�ID��IP
	*�������
	*����ֵ�����б�ʶ
	*****************************************************************************/
	virtual const char*MakeCall(const char*cszUserID,ConfInfo *pConfInfo=NULL);

	void AddCall(SIPMediaCall*pSIPMediaCall);

	/******************************************************************************
	* AcceptCall
	*���������ܺ�������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int AcceptCall(const char*cszCallID,ConfInfo *pConfInfo=NULL);

	/******************************************************************************
	* HangupCall
	*�������ҶϺ���
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int HangupCall(const char*cszCallID);

	/******************************************************************************
	* SendPTZ
	*����������PTZ��������̨�������
	*���룺	cszCallID		-���б�ʶ
			nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	/******************************************************************************
	* StartSecondVideo
	*��������ʼ����
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartSecondVideo (const char*cszCallID);

	/******************************************************************************
	* StopSecondVideo
	*������ֹͣ����
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StopSecondVideo (const char*cszCallID);

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

	/******************************************************************************
	* SetLocalMainVideoWnd
	*���������ñ���������Ƶ��ʾ�Ĵ���
	*���룺	cszCallID		-���б�ʶ
			hWnd			-��������Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetLocalMainVideoWnd(const char*cszCallID,HWND hWnd);

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
	virtual int SetLocalMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

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
	virtual void SetLocalMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetLocalMainVideoEnableOSD
	*���������ñ���������Ƶ�Ƿ�����OSD
	*���룺	cszCallID		-���б�ʶ
			nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoEnableOSD(const char*cszCallID,int nEnable);

	/******************************************************************************
	* SetLocalMainVideoOSDText
	*���������ñ���������ƵOSD����
	*���룺	cszCallID		-���б�ʶ
			cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoOSDText(const char*cszCallID,const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

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
	virtual void SetLocalMainVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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
	virtual int GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);


#if defined(HAS_SIP_AVCON_MP) && ( (HAS_SIP_AVCON_MP == 1) || (HAS_SIP_AVCON_MP == 2) )

#else

	/******************************************************************************
	* SetRemoteMainVideoWnd
	*����������Զ��������Ƶ��ʾ�Ĵ���
	*���룺	cszCallID		-���б�ʶ
			hWnd			-Զ������Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetRemoteMainVideoWnd(const char*cszCallID,HWND hWnd);

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
	virtual int SetRemoteMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

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
	virtual void SetRemoteMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen);

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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
	virtual int GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

	/******************************************************************************
	* SetSecondVideoWnd
	*���������ø�����Ƶ��ʾ�Ĵ���
	*���룺	cszCallID		-���б�ʶ
			hWnd			-Զ�̸�����Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetSecondVideoWnd(const char*cszCallID,HWND hWnd);

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
	virtual int SetSecondVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

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
	virtual void SetSecondVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetSecondVideoEnableOSD
	*���������ø�����Ƶ�Ƿ�����OSD
	*���룺	cszCallID		-���б�ʶ
			nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoEnableOSD(const char*cszCallID,int nEnable);

	/******************************************************************************
	* SetSecondVideoOSDText
	*���������ø�����ƵOSD����
	*���룺	cszCallID		-���б�ʶ
			cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoOSDText(const char*cszCallID,const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

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
	virtual void SetSecondVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));


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

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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
	virtual int GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetLocalAudioLevel
	*��������ȡ������ƵLevel��ʵ��������С��
	*���룺	cszCallID		-���б�ʶ
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetLocalAudioVolume
	*��������ȡ������Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetLocalAudioMute
	*��������ȡ������Ƶ�Ƿ���
	*���룺	cszCallID		-���б�ʶ
			nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetLocalAudioCodecID
	*��������ȡ������Ƶ������Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

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
	virtual int GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetRemoteAudioLevel
	*��������ȡԶ����ƵLevel��ʵ��������С��
	*���룺	cszCallID		-���б�ʶ
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetRemoteAudioVolume
	*��������ȡԶ����Ƶ����
	*���룺	cszCallID		-���б�ʶ
			nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetRemoteAudioMute
	*��������ȡԶ����Ƶ�Ƿ���
	*���룺	cszCallID		-���б�ʶ
			nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetRemoteAudioCodecID
	*��������ȡԶ����Ƶ����
	*���룺	cszCallID		-���б�ʶ
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

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
	virtual int GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetCallSupportSecondVideo
	*��������ȡ�����Ƿ�֧�ָ�����H239��
	*���룺	cszCallID		-���б�ʶ
	*�����	��
	*����ֵ��֧�ַ��ط�0����֧�ַ���0
	*****************************************************************************/
	virtual int GetCallSupportSecondVideo(const char*cszCallID);

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

	/******************************************************************************
	* SaveLocalMainVideoToBMPFile
	*���������汾��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszCallID		-���б�ʶ
			cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveLocalMainVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SaveRemoteMainVideoToBMPFile
	*����������Զ��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszCallID		-���б�ʶ
			cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveRemoteMainVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SaveSecondVideoToBMPFile
	*���������渨����Ƶ��BMPͼƬ�ļ�
	*���룺	cszCallID		-���б�ʶ
			cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveSecondVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SendDTMF
	*����������DTMF
	*���룺	cszCallID		-���б�ʶ
			nDTMF			-DTMF
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SendDTMF(const char*cszCallID,const char nDTMF);

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
	* GetHasVideo
	*��������ȡ�Ƿ�ֻ����Ƶ
	*���룺cszCallID		-���б�ʶ
	*�������
	*����ֵ���������Ƶ����ô���ط�0�����򷵻�0
	*****************************************************************************/
	int GetHasVideo(const char*cszCallID);
	
	bool ExcludePlayChan(unsigned long nAudioID,AUDEC_PlayChan*pPlayChan);
public:
	virtual void OnRegistrationSuccess(int nLineID,std::string strUserID);
	virtual void OnRegistrationError(int nLineID,const std::string&strErrorCode,std::string strUserID);
	virtual void OnSIPMediaCallCreated (SIPMediaCall*pSIPMediaCall);
	virtual void OnCallRingIn (const std::string& strCallID);
	virtual void OnCallRingOut (const std::string& strCallID);
	virtual void OnCallEstablished (const std::string& strCallID);
	virtual void OnCallDestroyed (const std::string& strCallID,IMXEC_CALL_END_REASON discReason);
	virtual void OnTimer(UINT32 nIDEvent);

	virtual void OnCallSecondVideoStart(const std::string&strCallID,int type=0);
	virtual void OnCallSecondVideoStop(const std::string&strCallID,int type=0);
	
	void OnCallBFCPTransportChange(SIPCall*pSIPMediaCall);

	//���յ�SDPЭ�̺����´�codec
	void OnCallSDPNegotiate(SIPCall*pSIPMediaCall);

	//�����յ��ĶԷ�������ý�������뱾�˴򿪵Ľ���CODEC��ͬʱ����PTƥ�����´򿪽���CODEC
	//�Զ˷�����ý���������Ա���PT����RTP���������ptΪ���յ�ý�����ݵ�PalyloadType
	bool GetAudioLocalCodecTypeFromPalyloadType(X_AUDIO_CODEC_ID &type,int pt);
	bool GetVideoLocalCodecTypeFromPalyloadType(VIDEC_CODEC_TYPE &type,int pt);

	bool GetDualVideoLocalCodecTypeFromPalyloadType(VIDEC_CODEC_TYPE &type,int pt);
	bool GetDataLocalCodecTypeFromPalyloadType(DATA_CODEC_ID &type,int pt);

	/******************************************************************************
	* OnRecvHangupCall
	*���������յ��Զ˹ҶϺ���
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int OnRecvHangupCall(const char*cszCallID);

	//added by jiangdingfeng 2013-11-18 �ж��Ƿ��к���
	bool HasCall();

protected:
	typedef std::map<int,SIPUser*>	MapSIPUser;

	XCritSec			m_csMapSIPUser;
	MapSIPUser			m_MapSIPUser;
	XCritSec			m_csMapThrSIPUser;
	MapSIPUser			m_MapThrSIPUser;
	int					m_H264PT;
	int					m_H263PPT;
	int					m_H263PPPT;
	int					m_H224PT;

	int					m_H264SVCPT;
	int					m_DualH264PT;
	int					m_DualH264SVCPT;
	int					m_DualH263PPT;
	int					m_DualH263PPPT;

	int					m_G7221_24k_PT;
	int					m_G7221_32k_PT;
	int					m_G7221C_24k_PT;
	int					m_G7221C_32k_PT;
	int					m_G7221C_48k_PT;
	bool				m_bRunning;
	friend class SIPUser;
	typedef std::map<std::string,SIPMediaCall*>	MapSIPMediaCall;

	XCritSec			m_csMapSIPMediaCall;					//������������
	MapSIPMediaCall		m_MapSIPMediaCall;						//�������Ĳ�����

	XCritSec			m_csMapThrSIPMediaCall;					//�̲߳��������
	MapSIPMediaCall		m_MapThrSIPMediaCall;					//�߳����Ĳ�����

	std::string			m_strEmpty;
	int					m_nCurrentLineID;
	unsigned short		m_nRTPBasePort;
	unsigned short		m_nSIPListenPort;
	unsigned int		m_nTimerID;

	X_AUDIO_CODEC_ID	m_nAudioCodecID;
	int					m_nMainVideoDevID;
	int					m_nMainVideoFrameRate;
	VIDEC_CODEC_TYPE	m_nMainVideoCodecType;
	//VIDEO_FORMAT_TYPE	m_nMainVideoFormatType;
    int                 m_nMainVideoNetworkType;

	int						m_nSecondVideoDevID;
	int						m_nSecondVideoDevFrameRate;
	VIDEC_CODEC_TYPE		m_nSecondVideoCodecType;
	//VIDEO_FORMAT_TYPE		m_nSecondVideoFormatType;
    int                     m_nSecondVideoNetworkType;

public:
	unsigned long			m_nActualSIPListenPort;
	int					    m_nMainVideoBitrate;
	int						m_nSecondVideoMaxBitrate;

public:
	unsigned int			m_nRequestConfId;					//local bfcp info
	unsigned short			m_nRequestUserId;					//local bfcp info
	unsigned short			m_nRequestFloorId;					//local bfcp info
	unsigned short			m_nRequestPriority;					//local bfcp info
	EBFCPTransportType		m_nRequestTransportType;			//local bfcp info
	EBFCPFloorControlType	m_nRequestFloorControlType;			//local bfcp info
	EBFCPSetupType			m_nRequestSetupType;	

	unsigned int			m_nRequestConfIdCalled;					//Called local bfcp info
	unsigned short			m_nRequestUserIdCalled;					//Called local bfcp info
	unsigned short			m_nRequestFloorIdCalled;			    //Called local bfcp info
	EBFCPFloorControlType	m_nRequestFloorControlTypeCalled;		//Called local bfcp info
	EBFCPSetupType			m_nRequestSetupTypeCalled;	

	int						m_nStarted;

	//set bfcp local info
	void SetBFCPUserId(int userId);
	void SetBFCPConfId(int confId);
	void SetBFCPFloorId(int floorId);
	void SetBFCPRequestPriority(unsigned short priority);
	void SetBFCPSetupType(EBFCPSetupType type);
	void SetBFCPFloorControlType(EBFCPFloorControlType type);
	void SetBFCPTransportType(EBFCPTransportType type);
	//keep connect
	void KeepConnectedSendBfcpInfo();
	//set sip bfcp sdp local info
	void SetSDPBFCPLocalInfo(SIPCallCapability & sdpCap,int nDir=IMXEC_CALL_DIR_OUT);
	//set sip bfcp call local info
	void SetSIPCallBFCPLocalInfo(SIPMediaCall* pSIPMediaCall,int nDir=IMXEC_CALL_DIR_OUT);
	//set sip bfcp net local info
	void SetSIPBFCPLocalInfo(SIP_BFCP * pSipBfcp,int nDir=IMXEC_CALL_DIR_OUT);
	void SIPCallMgr::SipSetting();

	/******************************************************************************
	* SetSIPTransportType
	*����������SIP���������
	*���룺type			-����
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetSIPTransportType(IMXEC_SIP::TRANS_TYPE type);

	//0 udp 1 tcp 2 tls 
	int GetSIPTransportType();

	//ͨ��uri���ҵ�SIPUser
	SIPUser * GetSIPUserFromSipUri(pjsip_uri *uri,pjsip_transport * transport);
	//�ȴ�SIP�˺�ע�����
	void WaitSIPUserUnRegComplete();
	void WaitSIPUserDelete(SIPUser *pSIPUser);

	/******************************************************************************
	* SetICETransportCount
	*���������ô�����ÿ��ý����ICEͨ����
	*���룺	nCount		-ÿ��ý����ICEͨ����(����ֻ��RTP����Ϊ1���绹��Ҫ��RTCP����Ϊ2)
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetICETransportCount(int nCount);
	/******************************************************************************
	* SetICEEnable
	*����������ICEӦ��״̬
	*���룺	bEnable		-ICEӦ��״̬ trueʹ�ã�false��ʹ�ã�Ĭ��false
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetICEEnable(bool bEnable);
	/******************************************************************************
	* SetTURNLoginAuth
	*���������õ�¼TURN�������˻�
	*���룺	cszTurnUser		-TURN�������˻���
	cszTurnPWD		-TURN�������˻�����
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetTURNLoginAuth(const char*cszTurnUser,const char*cszTurnPWD);
	/******************************************************************************
	* SetTURNServerIPPort
	*����������TURN��������ַ,Ҳ����Ϊ������ַ
	*���룺	cszTurnIPPort		-TURN��������ַ ����IP:Port
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetTURNServerIPPort(const char*cszTurnIPPort);
	/******************************************************************************
	* SetSTUNServerIPPort
	*����������STUN��������ַ,Ҳ����Ϊ������ַ
	*���룺	cszStunIPPort		-STUN��������ַ ����IP:Port
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetSTUNServerIPPort(const char*cszStunIPPort);
	/******************************************************************************
	* SetTurnTransportType
	*����������TURN����ͨ������
	*���룺	nType		-TURN����ͨ������ 17 udp, 6 tcp, 255 tls, ����ɲο�pj_turn_tp_type
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetTurnTransportType(int nType);
	/******************************************************************************
	* SetDNSServerIP
	*����������DNS��������ַ ���������ЧDNS�Ͳ�Ҫ����
	*���룺	cszDNSIP		-����DNS��������ַ ����IP
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetDNSServerIP(const char*cszDNSIP);
	/******************************************************************************
	* SetDNSServerIP2
	*����������DNS��������ַ2  ���������ЧDNS�Ͳ�Ҫ����
	*���룺	cszDNSIP2		-����DNS��������ַ ����IP
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetDNSServerIP2(const char*cszDNSIP2);

	/******************************************************************************
	* GetICEInstanceConfig
	*��������ȡICE CONFIG
	*���룺��
	*�������
	*����ֵ������ICE CONFIG *
	*****************************************************************************/
	pj_ice_strans_cfg * GetICEInstanceConfig();

	/******************************************************************************
	* GetICEMediaTrans
	*������ȡICEý��ͨ��,���С�ڵ���1�򴴽�3*m_nICETransportCount�� 
	       ����ȡ����ʹ�ý�������Զ��ر�,������������ر�
	*���룺��
	*�������
	*����ֵ���ɹ�����ICEͨ��ָ�룬ʧ�ܷ���NULL
	*****************************************************************************/
	pjmedia_transport* GetICEMediaTrans();

	/******************************************************************************
	* GetMediaEndpoint
	*������ȡMediaEndpoint
	*���룺��
	*�������
	*����ֵ���ɹ�����MediaEndpointָ�룬ʧ�ܷ���NULL
	*****************************************************************************/
	pjmedia_endpt * GetMediaEndpoint();

	//ICEЭ�̽���ص�
	void OnICENegotiationComplete(pjmedia_transport *tp,pj_ice_strans_op op,pj_status_t status);

private:
	/******************************************************************************
	* ICEInstanceInit
	*��������ʼ��ICEӦ�ã�ע����SipInit����û����SipInit��������
	*���룺	��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int ICEInstanceInit();

	/******************************************************************************
	* ICEInstanceExit
	*������ICEӦ���˳�
	*���룺	��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int ICEInstanceExit();

	/******************************************************************************
	* CreateMediaEndpoint
	*����������MediaEndpoint
	*���룺	��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int CreateMediaEndpoint();

	/******************************************************************************
	* CloseMediaEndpoint
	*�������ͷ�MediaEndpoint
	*���룺	��
	*�������
	*����ֵ����
	*****************************************************************************/
	void CloseMediaEndpoint();

	/******************************************************************************
	* CreateICEMediaTrans
	*����������ICEͨ��
	*���룺 nCount��ʼ�������ICEͨ����,Ĭ��3*m_nICETransportCount
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int CreateICEMediaTrans(int nCount);

	/******************************************************************************
	* GetICEMediaTrans
	*������ȡICEý��ͨ��,���С�ڵ���1�򴴽�3*m_nICETransportCount�� 
	       ����ȡ����ʹ�ý�������Զ��ر�,������������ر�
	*���룺 pIceTransȡ����ICEͨ��ָ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int GetICEMediaTrans(pjmedia_transport** pIceTrans);

	//ɾ��ָ����pjmedia_transport
	int DestroyICEMediaTrans(pjmedia_transport* pMediaTransport);
	//ɾ������
	int DestroyAllICEMediaTrans();
	//ICE�߳�
	static int ICEWorkerThread(void *unused);
	//ICE�̺߳���ִ����
	int ICEHandleEvents(unsigned max_msec, unsigned *p_count);
	//����NAT������ص�����
	void SetNatDetectCallback();

	void DoCheckMediaStop(void);

	//SIPע��ʧ�ܺ�ʱע�� ʹ��m_nSIPRegFailTimestamp���ʱ�䶨ʱע��
	void TimerRegAccountCheck(unsigned long nTimestamp);

public:
	//����SIP��δע�������IP����
	SIPUser * m_pIPCallSIPUser;
	IMXEC_SIP::TRANS_TYPE	m_nSIPTransType;				//SIP���������
	char					m_strUserAgentName[256];
	int						m_nICETransportCount;			//ÿ��ý����ICEͨ����
	std::string				m_strStunSrv;					//STUN��������ַ ����IP:Port
	std::string				m_strTurnSrv;					//TURN��������ַ ����IP:Port
	std::string				m_strTurnUser;					//TURN�������˻���
	std::string				m_strTurnPWD;					//TURN�������˻�����
	std::string				m_strDNSSrv;
	std::string				m_strDNSSrv2;
	bool					m_bICEEnable;					//ICEӦ��״̬ trueʹ�ã�false��ʹ�ã�Ĭ��false
	pj_thread_t *			m_pICEThread;					//ICE�߳�
	bool					m_bICEThreadQuitFlag;			//ICE�߳��˳���־

private:
	pj_ice_strans_cfg		m_ICEConfig;					//ICE CONFIG
	pj_caching_pool			m_ICECachingPool;
	int						m_nTurnTPType;					//TURN����ͨ������ �ο�pj_turn_tp_type
	int						m_nICEInit;						//default 0
	pj_pool_t *				m_pICEPool;
	pjmedia_endpt *			m_pMediaEndpoint;					
	pj_dns_resolver	*		m_pDNSResolver;					//DNS resolver
	typedef std::vector<pjmedia_transport*>	MapSIPICE;
	XCritSec				m_csMapSIPMediaICETrans;
	MapSIPICE				m_MapSIPMediaICETrans;			//ice pjmedia_transport
	int						m_nMaxICESocketCount;			//׼���������ICE SOCKET��,ע Ĭ��6*10·

public:
	unsigned						m_nOutboundProxyCount;						//Number of outbound proxies in the  m_OutboundProxyArray array.
	char							m_strOutboundProxy[8][256];					//outbound proxies
	pjsip_route_hdr					m_OutboundProxyRoute;						//List of outbound proxies
	pjsip_route_hdr					m_RouteHdr;									//Complete route set inc. outbnd.
	int								m_nForcelrParam;							//�˿���request line����ʹ��outbound uri
	int								m_nUseRegReqLine;							//reg request line use new addr
	int								m_nUseInvReqLine;							//call request line use new addr
	std::string						m_strRegReqLineAddr;						//request line new addr
	std::string						m_strInvReqLineAddr;						//request line new addr
	std::string						m_strDateTime;
	int								m_nUseDualVideoToken;						//use token interactive is 1,else is 0
	int								m_nSDPOrignIPConverse;						//SDP orign ip is converse ,not converse is 0 ,converse is 1
	int								m_nSendSubscribre;							//call sip mcu Send Subscribre is 1,else is 0
	int								m_nSendPicFastUpdate;						// Send Picture Fast Update is 1,not Send is 0
	int								m_nSendMultipartSDP;						//Send Multipart SDP set 1 ,not set 0
	//unsigned long					m_nSIPRegFailLastTimestamp;
	unsigned long					m_nSIPRegFailTimerTimestamp;				//ע��ʧ�ܺ��ٴ�ע����ʱ�� ms Ĭ��2*60*1000
	int								m_nSIPREGFail;								//ע��ʧ��״̬ 1Ϊʧ��״̬ 2�ɹ�״̬
	int								m_nUseDualVideoTokenType;					//0 not use , 1 use to zhongxing ,
	char							m_strConfCentralNum[256];					//conference central number

public:
	void SetCapabilityParam(SIPCall * pSIPCall,SIPCallCapability & sdpCap,int nSupportDual,std::string strLocalIP,int nLocalAudioPort,int nLocalVideoPort,int nLocalDualVideoPort,int nLocalBFCPTransPort,int nDir=IMXEC_CALL_DIR_OUT);

protected:
	virtual void							ThreadProcMain(void);
	int										m_nCapPrint;						//�����������Ϣ���� 0����ӡ 1��ӡ
	int										m_nRegExpires;
	int										m_nSIPLogLevel;
	char									m_strSIPLogFile[256];

private:
	void SetDataBufferChannelDestroyed(const std::string& strCallID,std::string strCMD,std::string strMedia);
	void SetDataBufferChannelConnectedVD(const std::string& strCallID ,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth,int nHeight,int nLevel,std::string strCMD,std::string strMedia,HDAPAPPCALL hdapCall=NULL);


public:

	/******************************************************************************
	* OutPutStackLog
	*������Э��ջ��־���
	*���룺	nLogType			-��־����  0 ������־ 1 ������־ 
			cszLog				-��־����
			nLen				-��־���ݳ���
	*����� ��
	*����ֵ����
	*****************************************************************************/
	void					OutPutStackLog(int nLogType,const char * cszLog,char * szCallID=NULL);

	/******************************************************************************
	* OutPutCapContent
	*����������������������� ��������ÿ���
	*���룺	cszCallID			-���б�ʶ
			nCapType			-������������ 0 Զ������������ 1 �������������� 2 Զ��ͨ��ͨ�������� 3 ����ͨ������������
			cszContent			-��������
			nLen				-�������ݳ���
	*����� ��
	*����ֵ����
	*****************************************************************************/
	void					OutPutCapContent(const char*cszCallID,int nCapType,const char * cszContent);

	int						GetDAPCapPrintEnable(void);

	/******************************************************************************
	*����������DTMF���β��Ű���ֵ ע��Ҳ��������ʵ��{ AUDEC_GetEngine().AUDEC_PlayDTMF(dtmf);} �ڲ�ʵ�ֲ��ٵ�AUDEC_GetEngine
	*���룺	cszCallID			-���б�ʶ
			dtmf				-����ֵ
	*****************************************************************************/
	int DAPSendCallDTMF(const char*cszCallID,const char dtmf);

	/******************************************************************************
	*��������Զ˷����Զ���Ǳ���Ϣ
	*���룺	cszCallID		-���б�ʶ
			cszContent		-�Զ���Ǳ�׼��Ϣ����
			cszMimeType		-�Զ���Ǳ���Ϣ�������� ��application/xml
	*****************************************************************************/
	int DAPSendCallCustomMsg(const char*cszCallID,const char * cszContent,const char * cszMimeType);

	/******************************************************************************
	*���������͸�������������Ϣ
	*���룺	cszCallID		-���б�ʶ
			nType			-��������������Ϣ���� 1 ���� 2 ��Ӧ 3 ָʾ 4 ȡ��
			transactionId	-����ID  
			userId			-�û�ID  
			floorId			-����������ID 
			status			-״̬ �緢����Ӧʱ 0��ʾ�ܾ� 1��ʾ����
	*****************************************************************************/
	int SendCallDualVideoControl(const char*cszCallID,int nType,int transactionId=0,int userId=0,int floorId=0,int status=0);

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
	* SetAgentName
	*����������Ӧ��ʵ���� userAgent
	*���룺	cszAgent		-Ӧ��ʵ����
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetAgentName(const char*cszAgent);

	/******************************************************************************
	* SetCapPrintEnable
	*���������ô�ӡ���˼�Զ��������������Ϣ����,Ĭ�ϲ���ӡ
	*���룺	nPrint		-����ӡ��Ϊ0,��ӡ����Ϊ1
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetDAPCapPrintEnable(int nPrint=0);

	/******************************************************************************
	* SetRegisterTimer
	*����������SIPע����ʱ�� 
	*���룺	regTimer		-ע��ɹ����ٴ�ע��ˢ��ʱ�� ��λ,��
			regFailTimer	-ע��ʧ�ܺ��ٴ�ע����ʱ�� ��λ,��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetRegisterTimer(int regTimer,int regFailTimer);

	/******************************************************************************
	* SetSIPLogFile
	*����������SIPЭ��ջ��¼��־������־�ļ���
	*���룺	logLevel		-��־���� 0,fatal error 1,error 2,warning 3,info 4,debug 5,trace 6,more detailed trace
			cszLogFileName	-��־�ļ���
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	int SetSIPLogFile(int logLevel,const char*cszLogFileName);

	int SetDAPLocalMainVideoParam(const char*cszCallID,int nFrameRate,int nBitrate);
	int SetDAPLocalSecondVideoParam(const char*cszCallID,int nFrameRate,int nBitrate);
	int SetSIPCallSupportH239(const char*cszCallID,int nSupportH239);

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
	*���������յ���Ƶý�����ص�
	*���룺	cszCallID			-���б�ʶ
			pRTPPacketData		-RTP��
			nRTPPacketLen		-RTP������
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP��ͷ����
			usSequence			-RTP������
			ulTimestamp			-RTP��ʱ���
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteAudioRTPPacket(const char*cszCallID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

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
	virtual void OnDAP_RecvRemoteMainVideoRTPPacket(const char*cszCallID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	
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
	virtual void OnDAP_RecvRemoteSecondVideoRTPPacket(const char*cszCallID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

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
	*������ý��ͨ�����ӳɹ��¼�
	*���룺	type				-1 ý������ 1 audio 2 main video 3 second video 4 fecc data
			sendRecv			-1 send 2 recv
			cszCallID			-���б�ʶ
			codecType			-��ƵCodecID
			nBitrate			-����
			nPayloadType		-PayloadType
			width				-�ֱ��ʿ�������� ��ѡ
			nHeight				-�ֱ��ʸ߶������� ��ѡ
			ip					-���˽���rtp ip
			port				-���˽���rtp port
			nLevel				-�ֱ���ʵ��H264levelֵ ��ѡ
	*****************************************************************************/
	virtual void OnDAP_CallMediaChannelConnected(int type,int sendRecv,HDAPAPPCALL hdapCall,const std::string& strCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth=0,int nHeight=0,int nLevel=0);

	/******************************************************************************
	*������ý��ͨ���ر��¼�
	*���룺	type				-1 ý������ 1 audio 2 main video 3 second video 4 fecc data
			sendRecv			-1 send 2 recv
			cszCallID			-���б�ʶ
	*****************************************************************************/
	virtual void OnDAP_CallMediaChannelDestroyed(int type,int sendRecv,const std::string& strCallID);


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
	*�������յ��Զ˷��͵�SIP INFO���ص���Ϣ
	*���룺	strCallID		-���б�ʶ
			cszContent		-�Զ�����Ϣ����
			cszMimeType		-�Զ����������� ��application/xml
	*****************************************************************************/
	virtual void OnDAP_RecvCallCustomMsg(const std::string& strCallID,const char * cszContent,const char * cszMimeType);

	/******************************************************************************
	*�������յ��Զ˷��͵ĸ�������������Ϣ
	*���룺	strCallID		-���б�ʶ
			nType			-��������������Ϣ���� 1 ���� 2 ��Ӧ 3 ָʾ 4 ȡ��
			transactionId	-����ID  
			userId			-�û�ID  
			floorId			-����������ID 
			status			-״̬ �緢����Ӧʱ 0��ʾ�ܾ� 1��ʾ����
	*****************************************************************************/
	virtual void OnDAP_RecvCallDualVideoControl(const std::string& strCallID,int nType,int transactionId=0,int userId=0,int floorId=0,int status=0);


#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

public:
	void StartVideoToCall(const char*cszCallID,unsigned long nVideoID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
	void StopVideoToCall(const char*cszCallID);
	void StartH239ToCall(const char*cszCallID,unsigned long nVideoID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
	void StopH239ToCall(const char*cszCallID);
	void StartAudioToCall(const char*cszCallID,unsigned long nAudioID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
	void StopAudioToCall(const char*cszCallID);

#endif

	// ����Զ˷���˫��
	void AllowToSendSecondVideo(const char*cszCallID, bool bAllow);
	// ����������
	void SendFlowControlCommand(const char*cszCallID, IMXEC_CALL_MEDIA_TYPE emMediaType, int nMaximumBitRate);
	//���ں�ͨ���ٴ�ͨ��
	void OpenSIPMediaChannel(SIPMediaCall* pSIPMediaCall);
	void HungupAll(int nLineID =-1);

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

	unsigned short GetCallRTPPort(void);

public:
	unsigned long	m_nLastPacketCheckTimestamp;
	unsigned long	m_nLastBFCPCheckTimestamp;
	unsigned long	m_nSIPCheckLastTimestamp;
	int			    m_nProxySendRtp;						// use nat check server proxy send rtp is 1,not Send is 0
	int			    m_nProxyUseFEC;						    // use nat check server proxy send rtp is 1,not Send is 0
	int			    m_nRTPUseType;                          // use medianet rtp is 1,use pjsip rtp is 2 ,dufault is 1
	VIDEO_FORMAT_TYPE       m_nMainVideoFormatType;
	VIDEO_FORMAT_TYPE       m_nSecondVideoFormatType;
	int             m_nSIPServerType;                       //1 is kamailio,2 is sgb sip server,default 0
	int             m_nDualVideoSDPMode;                    //1 is Send first invite Add DualVideo SDP,2 is send second invite DualVideo SDP,default 0
	unsigned long	m_nLastCheckCallNumTS;

	MediaPortRelease m_MediaPortRelease;
	void SaveMediaReleasePort(unsigned short rtpPort);
	void GetTerminalInfo(SIPMediaCall* pSIPMediaCall,pjsip_rx_data *rdata);
	void GetServerInfo(pjsip_rx_data *rdata);
	void SetMediaPortArrayEmpty();
	//�ص��Ҷ�״̬ʱ�ټ�����ǲ�����Ϊ�����ղ���ý����ԭ��Ҷϵ�
	bool DisconnectCheckRoundTripDalay(SIPMediaCall * pSIPMediaCall);

	bool CheckMapCallEmpty(void);

};

#endif
