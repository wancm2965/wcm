#include <IMXEC/IMXEC_Call.h>
#include "CallMgr.h"

IMXEC_Call::IMXEC_Call(void)
:m_nExclusive(0)
{

}

IMXEC_Call::~IMXEC_Call(void)
{
}

int IMXEC_Call::GetExclusive(void)
{
	return m_nExclusive;
}

/******************************************************************************
* Start
*��������������
*���룺��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::Start(int nExclusive)
{
	OutputDebugString("IMXEC_Call::Start ---------------------------------------------------\n");
	m_nExclusive=nExclusive;
	CallMgr::Instance().AddIMXEC_Call(this);
	return 0;
}

/******************************************************************************
* Stop
*������ֹͣ����
*���룺��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
void IMXEC_Call::Stop(void)
{
	CallMgr::Instance().RemoveIMXEC_Call(this);
	OutputDebugString("IMXEC_Call::Stop ---------------------------------------------------\n");

}


/******************************************************************************
* MakeCall
*�������������
*���룺	cszUserID		-�Է����û�ID��IP
		cszProtocol		-����Э�飬"H323","SIP","IMXEC"
*�������
*����ֵ�����б�ʶ
*****************************************************************************/
const char*IMXEC_Call::MakeCall(const char*cszUserID,const char*cszProtocol)
{
	return CallMgr::Instance().MakeCall(cszUserID,cszProtocol);
}


/******************************************************************************
* AcceptCall
*���������ܺ�������
*���룺	cszCallID		-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::AcceptCall(const char*cszCallID)
{
	return CallMgr::Instance().AcceptCall(cszCallID);
}

/******************************************************************************
* HangupCall
*�������ҶϺ���
*���룺	cszCallID		-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::HangupCall(const char*cszCallID)
{
	return CallMgr::Instance().HangupCall(cszCallID);
}


/******************************************************************************
* SendPTZ
*����������PTZ��������̨�������
*���룺	cszCallID		-���б�ʶ
		nPTZAction		-��̨����
		nSpeed			-�������ٶ�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
	return CallMgr::Instance().SendPTZ(cszCallID,nPTZAction,nSpeed);
}


/******************************************************************************
* StartSecondVideo
*��������ʼ����
*���룺	cszCallID		-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::StartSecondVideo (const char*cszCallID)
{
	return CallMgr::Instance().StartSecondVideo(cszCallID);
}

/******************************************************************************
* StopSecondVideo
*������ֹͣ����
*���룺	cszCallID		-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::StopSecondVideo (const char*cszCallID)
{
	return CallMgr::Instance().StopSecondVideo(cszCallID);
}


/******************************************************************************
* SetLocalMainVideoWnd
*���������ñ���������Ƶ��ʾ�Ĵ���
*���룺	cszCallID		-���б�ʶ
		hWnd			-��������Ƶ��ʾ�Ĵ��ھ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::SetLocalMainVideoWnd(const char*cszCallID,HWND hWnd)
{
	return CallMgr::Instance().SetLocalMainVideoWnd(cszCallID,hWnd);
}

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
int IMXEC_Call::SetLocalMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	return CallMgr::Instance().SetLocalMainVideoWndPos(cszCallID,x,y,cx,cy);
}

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
void IMXEC_Call::SetLocalMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	CallMgr::Instance().SetLocalMainVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
}

/******************************************************************************
* SetLocalMainVideoEnableOSD
*���������ñ���������Ƶ�Ƿ�����OSD
*���룺	cszCallID		-���б�ʶ
		nEnable			-�Ƿ�����OSD
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetLocalMainVideoEnableOSD(const char*cszCallID,int nEnable)
{
	CallMgr::Instance().SetLocalMainVideoEnableOSD(cszCallID,nEnable);
}

/******************************************************************************
* SetLocalMainVideoOSDText
*���������ñ���������ƵOSD����
*���룺	cszCallID		-���б�ʶ
		cszText			-OSD����
		nAlign			-���뷽ʽ
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetLocalMainVideoOSDText(const char*cszCallID,const char*cszText,int nAlign)
{
	CallMgr::Instance().SetLocalMainVideoOSDText(cszCallID,cszText,nAlign);
}

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
void IMXEC_Call::SetLocalMainVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	CallMgr::Instance().SetLocalMainVideoOSDFont(cszCallID,logfont,crText,crBk);
}

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
int IMXEC_Call::GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	return CallMgr::Instance().GetLocalMainVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
}

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
int IMXEC_Call::GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return CallMgr::Instance().GetLocalMainVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}

/******************************************************************************
* SetRemoteMainVideoWnd
*����������Զ��������Ƶ��ʾ�Ĵ���
*���룺	cszCallID		-���б�ʶ
		hWnd			-Զ������Ƶ��ʾ�Ĵ��ھ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::SetRemoteMainVideoWnd(const char*cszCallID,HWND hWnd)
{
	return CallMgr::Instance().SetRemoteMainVideoWnd(cszCallID,hWnd);
}

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
int IMXEC_Call::SetRemoteMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	return CallMgr::Instance().SetRemoteMainVideoWndPos(cszCallID,x,y,cx,cy);
}

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
void IMXEC_Call::SetRemoteMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)
{
	CallMgr::Instance().SetRemoteMainVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
}

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
int IMXEC_Call::GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	return CallMgr::Instance().GetRemoteMainVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
}

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
int IMXEC_Call::GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return CallMgr::Instance().GetRemoteMainVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}

/******************************************************************************
* SetSecondVideoWnd
*���������ø�����Ƶ��ʾ�Ĵ���
*���룺	cszCallID		-���б�ʶ
		hWnd			-Զ�̸�����Ƶ��ʾ�Ĵ��ھ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::SetSecondVideoWnd(const char*cszCallID,HWND hWnd)
{
	return CallMgr::Instance().SetSecondVideoWnd(cszCallID,hWnd);
}

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
int IMXEC_Call::SetSecondVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	return CallMgr::Instance().SetSecondVideoWndPos(cszCallID,x,y,cx,cy);
}

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
void IMXEC_Call::SetSecondVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	CallMgr::Instance().SetSecondVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
}

/******************************************************************************
* SetSecondVideoEnableOSD
*���������ø�����Ƶ�Ƿ�����OSD
*���룺	cszCallID		-���б�ʶ
		nEnable			-�Ƿ�����OSD
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetSecondVideoEnableOSD(const char*cszCallID,int nEnable)
{
	CallMgr::Instance().SetSecondVideoEnableOSD(cszCallID,nEnable);
}

/******************************************************************************
* SetSecondVideoOSDText
*���������ø�����ƵOSD����
*���룺	cszCallID		-���б�ʶ
		cszText			-OSD����
		nAlign			-���뷽ʽ
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetSecondVideoOSDText(const char*cszCallID,const char*cszText,int nAlign)
{
	CallMgr::Instance().SetSecondVideoOSDText(cszCallID,cszText,nAlign);
}

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
void IMXEC_Call::SetSecondVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	CallMgr::Instance().SetSecondVideoOSDFont(cszCallID,logfont,crText,crBk);
}

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
int IMXEC_Call::GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	return CallMgr::Instance().GetSecondVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
}

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
int IMXEC_Call::GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return CallMgr::Instance().GetSecondVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}

/******************************************************************************
* GetLocalAudioLevel
*��������ȡ������ƵLevel��ʵ��������С��
*���룺	cszCallID		-���б�ʶ
*�����	nLevel			-ʵ��������С��0-10��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::GetLocalAudioLevel(const char*cszCallID,int&nLevel)
{
	return CallMgr::Instance().GetLocalAudioLevel(cszCallID,nLevel);
}

/******************************************************************************
* SetLocalAudioVolume
*��������ȡ������Ƶ����
*���룺	cszCallID		-���б�ʶ
		nVolume			-����
*�����	��
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetLocalAudioVolume(const char*cszCallID,int nVolume)
{
	CallMgr::Instance().SetLocalAudioVolume(cszCallID,nVolume);
}

/******************************************************************************
* SetLocalAudioMute
*��������ȡ������Ƶ�Ƿ���
*���룺	cszCallID		-���б�ʶ
		nMute			-�Ƿ���
*�����	��
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetLocalAudioMute(const char*cszCallID,int nMute)
{
	CallMgr::Instance().SetLocalAudioMute(cszCallID,nMute);
}

/******************************************************************************
* GetLocalAudioCodecID
*��������ȡ������Ƶ������Ϣ
*���룺	cszCallID		-���б�ʶ
*�����	nAudioCodecID	-����ID
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID)
{
	return CallMgr::Instance().GetLocalAudioCodecID(cszCallID,nAudioCodecID);
}

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
int IMXEC_Call::GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return CallMgr::Instance().GetLocalAudioPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}

/******************************************************************************
* GetRemoteAudioLevel
*��������ȡԶ����ƵLevel��ʵ��������С��
*���룺	cszCallID		-���б�ʶ
*�����	nLevel			-ʵ��������С��0-10��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::GetRemoteAudioLevel(const char*cszCallID,int&nLevel)
{
	return CallMgr::Instance().GetRemoteAudioLevel(cszCallID,nLevel);
}

/******************************************************************************
* SetRemoteAudioVolume
*��������ȡԶ����Ƶ����
*���룺	cszCallID		-���б�ʶ
		nVolume			-����
*�����	��
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetRemoteAudioVolume(const char*cszCallID,int nVolume)
{
	CallMgr::Instance().SetRemoteAudioVolume(cszCallID,nVolume);
}

/******************************************************************************
* SetRemoteAudioMute
*��������ȡԶ����Ƶ�Ƿ���
*���룺	cszCallID		-���б�ʶ
		nMute			-�Ƿ���
*�����	��
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetRemoteAudioMute(const char*cszCallID,int nMute)
{
	CallMgr::Instance().SetRemoteAudioMute(cszCallID,nMute);
}

/******************************************************************************
* GetRemoteAudioCodecID
*��������ȡԶ����Ƶ����
*���룺	cszCallID		-���б�ʶ
*�����	nAudioCodecID	-����ID
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID)
{
	return CallMgr::Instance().GetRemoteAudioCodecID(cszCallID,nAudioCodecID);
}

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
int IMXEC_Call::GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return CallMgr::Instance().GetRemoteAudioPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}

/******************************************************************************
* SetCallType
*���������ú�������
*���룺	nCallType		-��������
*�����	��
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::SetCallType(IMXEC_CALL_TYPE nCallType)
{
	return CallMgr::Instance().SetCallType(nCallType);
}

/******************************************************************************
* GetCallType
*��������ȡ��������
*���룺	��
*�����	nCallType		-��������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::GetCallType(IMXEC_CALL_TYPE&nCallType)
{
	return CallMgr::Instance().GetCallType(nCallType);
}

/******************************************************************************
* GetCallSupportSecondVideo
*��������ȡ�����Ƿ�֧�ָ�����H239��
*���룺	cszCallID		-���б�ʶ
*�����	��
*����ֵ��֧�ַ��ط�0����֧�ַ���0
*****************************************************************************/
int IMXEC_Call::GetCallSupportSecondVideo(const char*cszCallID)
{
	return CallMgr::Instance().GetCallSupportSecondVideo(cszCallID);
}



/******************************************************************************
* StartRecordLocalMainToAVI
*��������ʼ��������¼��
*���룺	cszCallID			-���б�ʶ
		cszPathName			-¼���ļ�����
		nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::StartRecordLocalMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	return CallMgr::Instance().StartRecordLocalMainToAVI(cszCallID,cszPathName,nStandardFormat);
}

/******************************************************************************
* StopRecordLocalMainToAVI
*������ֹͣ��������¼��
*���룺cszCallID			-���б�ʶ
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::StopRecordLocalMainToAVI(const char*cszCallID)
{
	CallMgr::Instance().StopRecordLocalMainToAVI(cszCallID);
}

/******************************************************************************
* StartRecordRemoteMainToAVI
*��������ʼԶ������¼��
*���룺	cszCallID			-���б�ʶ
		cszPathName			-¼���ļ�����
		nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::StartRecordRemoteMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	return CallMgr::Instance().StartRecordRemoteMainToAVI(cszCallID,cszPathName,nStandardFormat);
}


/******************************************************************************
* StopRecordRemoteMainToAVI
*������ֹͣԶ������¼��
*���룺cszCallID			-���б�ʶ
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::StopRecordRemoteMainToAVI(const char*cszCallID)
{
	CallMgr::Instance().StopRecordRemoteMainToAVI(cszCallID);
}

/******************************************************************************
* StartRecordSecondToAVI
*��������ʼ����¼��
*���룺	cszCallID			-���б�ʶ
		cszPathName			-¼���ļ�����
		nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::StartRecordSecondToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	return CallMgr::Instance().StartRecordSecondToAVI(cszCallID,cszPathName,nStandardFormat);
}


/******************************************************************************
* StopRecordSecondToAVI
*������ֹͣ����¼��
*���룺cszCallID			-���б�ʶ
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::StopRecordSecondToAVI(const char*cszCallID)
{
	return CallMgr::Instance().StopRecordSecondToAVI(cszCallID);
}

/******************************************************************************
* GetMainAudioID
*��������ȡ������Ƶ��ʶ
*���룺 cszCallID		-���б�ʶ
*����� nMainAudioID	-������Ƶ��ʶ
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID)
{
	return CallMgr::Instance().GetMainAudioID(cszCallID,nMainAudioID);
}

/******************************************************************************
* GetMainVideoID
*��������ȡ������Ƶ��ʶ
*���룺 cszCallID		-���б�ʶ
*����� nMainVideoID	-������Ƶ��ʶ
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID)
{
	return CallMgr::Instance().GetMainVideoID(cszCallID,nMainVideoID);
}

/******************************************************************************
* GetSecondVideoID
*��������ȡ������Ƶ��ʶ
*���룺 cszCallID		-���б�ʶ
*����� nSecondVideoID	-������Ƶ��ʶ
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID)
{
	return CallMgr::Instance().GetSecondVideoID(cszCallID,nSecondVideoID);
}

/******************************************************************************
* SetMainVideoScreenIndex
*����������������Ƶͼ���������
*���룺 cszCallID		-���б�ʶ
		nScreenIndex	-�������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex)
{
	return CallMgr::Instance().SetMainVideoScreenIndex(cszCallID,nScreenIndex);
}

/******************************************************************************
* SetSecondVideoScreenIndex
*���������ø�����Ƶͼ���������
*���룺 cszCallID		-���б�ʶ
		nScreenIndex	-�������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex)
{
	return CallMgr::Instance().SetSecondVideoScreenIndex(cszCallID,nScreenIndex);
}

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
void IMXEC_Call::SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen)
{
	CallMgr::Instance().SetSecondVideoScreenWnd(cszCallID,nScreenIndex,nWndIndex,nFullScreen);
}

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
void IMXEC_Call::SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen)
{
	CallMgr::Instance().SetRemoteVideoScreenWnd(cszCallID,nScreenIndex,nWndIndex,nFullScreen);
}

/******************************************************************************
* SetLocalRecordCallback
*���������ñ���¼��ص�
*���룺	cszCallID		-���б�ʶ
		pRecordCallback	-¼��ص�
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetLocalRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	CallMgr::Instance().SetLocalRecordCallback(cszCallID,pRecordCallback);
}

/******************************************************************************
* SetRemoteRecordCallback
*����������Զ��¼��ص�
*���룺	cszCallID		-���б�ʶ
		pRecordCallback	-¼��ص�
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetRemoteRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	CallMgr::Instance().SetRemoteRecordCallback(cszCallID,pRecordCallback);
}

/******************************************************************************
* SetSecondRecordCallback
*���������ø���¼��ص�
*���룺	cszCallID		-���б�ʶ
		pRecordCallback	-¼��ص�
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetSecondRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	CallMgr::Instance().SetSecondRecordCallback(cszCallID,pRecordCallback);
}

/******************************************************************************
* SaveLocalMainVideoToBMPFile
*���������汾��������Ƶ��BMPͼƬ�ļ�
*���룺	cszCallID		-���б�ʶ
		cszPathName		-ͼƬ�ļ�ȫ·��
*�������
*����ֵ����
*****************************************************************************/
int IMXEC_Call::SaveLocalMainVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	return CallMgr::Instance().SaveLocalMainVideoToBMPFile(cszCallID,cszPathName);
}

/******************************************************************************
* SaveRemoteMainVideoToBMPFile
*����������Զ��������Ƶ��BMPͼƬ�ļ�
*���룺	cszCallID		-���б�ʶ
		cszPathName		-ͼƬ�ļ�ȫ·��
*�������
*����ֵ����
*****************************************************************************/
int IMXEC_Call::SaveRemoteMainVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	return CallMgr::Instance().SaveRemoteMainVideoToBMPFile(cszCallID,cszPathName);
}

/******************************************************************************
* SaveSecondVideoToBMPFile
*���������渨����Ƶ��BMPͼƬ�ļ�
*���룺	cszCallID		-���б�ʶ
		cszPathName		-ͼƬ�ļ�ȫ·��
*�������
*����ֵ����
*****************************************************************************/
int IMXEC_Call::SaveSecondVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	return CallMgr::Instance().SaveSecondVideoToBMPFile(cszCallID,cszPathName);
}

/******************************************************************************
* SendDTMF
*����������DTMF
*���룺	cszCallID		-���б�ʶ
		nDTMF			-DTMF
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::SendDTMF(const char*cszCallID,const char nDTMF)
{
	return CallMgr::Instance().SendDTMF(cszCallID,nDTMF);
}

/******************************************************************************
* StartMixingWithWaveFile
*��������ʼ�Ժ��н���WAVE�ļ��������Է�������WAVE�ļ����ŵ�����
*���룺	cszCallID		-���б�ʶ
		cszPathName		-WAVE�ļ�ȫ·��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::StartMixingWithWaveFile(const char*cszCallID,const char*cszPathName)
{
	return CallMgr::Instance().StartMixingWithWaveFile(cszCallID,cszPathName);
}

/******************************************************************************
* StopMixingWithWaveFile
*������ֹͣ�Ժ��н���WAVE�ļ�����
*���룺	cszCallID		-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::StopMixingWithWaveFile(const char*cszCallID)
{
	return CallMgr::Instance().StopMixingWithWaveFile(cszCallID);
}

/******************************************************************************
* SetAudioOnly
*�����������Ƿ�ֻ֧����Ƶ
*���룺	nAudioOnly		-�Ƿ�ֻ֧����Ƶ��0��֧����Ƶ��Ƶ����0��ֻ֧����Ƶ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::SetAudioOnly(int nAudioOnly)
{
	return CallMgr::Instance().SetAudioOnly(nAudioOnly);
}

/******************************************************************************
* GetCallProtocol
*��������ȡ����Э��
*���룺	cszCallID		-���б�ʶ
*�������
*����ֵ���ɹ����غ���Э�飬ʧ�ܷ��ؿմ�""
*****************************************************************************/
const char*IMXEC_Call::GetCallProtocol(const char*cszCallID)
{
	return CallMgr::Instance().GetCallProtocol(cszCallID);
}

/******************************************************************************
* GetHasVideo
*��������ȡ�Ƿ�ֻ����Ƶ
*���룺cszCallID		-���б�ʶ
*�������
*����ֵ���������Ƶ����ô���ط�0�����򷵻�0
*****************************************************************************/
int IMXEC_Call::GetHasVideo(const char*cszCallID)
{
	return CallMgr::Instance().GetHasVideo(cszCallID);
}

/******************************************************************************
* SetEnableVideoTranscoding
*�����������Ƿ�������Ƶת��
*���룺nEnable		-�Ƿ�������Ƶת�롾0����������Ƶת�룬��0��������Ƶת�롿
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetEnableVideoTranscoding(int nEnable)
{
	CallMgr::Instance().SetEnableVideoTranscoding(nEnable);
}

/******************************************************************************
* GetEnableVideoTranscoding
*��������ȡ�Ƿ�������Ƶת��
*���룺��
*�������
*����ֵ�����������Ƶת�룬��ô���ط�0�����򷵻�0
*****************************************************************************/
int IMXEC_Call::GetEnableVideoTranscoding(void)
{
	return CallMgr::Instance().GetEnableVideoTranscoding();
}

/******************************************************************************
* GetSecondVideoDirection
*��������ȡ�����ķ���
*���룺	cszCallID		-���б�ʶ
*����� nDir			-�����ķ���
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir)
{
	return CallMgr::Instance().GetSecondVideoDirection(cszCallID,nDir);
}




//H235 Media use

/******************************************************************************
* GetH235AuthCheckState
*��������ȡH235��֤���״̬ ����ȡ������֤��Ϊ����ʱ����֤���
*���룺	cszCallID		  -���б�ʶ
*����� nState -��֤���״̬
nLength -������֤ʧ��ԭ���ַ�������
*����ֵ���ɹ�������֤ʧ��ԭ��ʧ�ܷ��ؿ�
*****************************************************************************/

const char* IMXEC_Call::GetH235AuthCheckState(const char*cszCallID,IMXEC_CALL_H235AUTH_STATE &nState,int &nLength)
{
	return CallMgr::Instance().GetH235AuthCheckState(cszCallID,nState,nLength);
}


/******************************************************************************
* SetExternalIP
*��������������ӳ��IP
*���룺cszExternalIP		-����ӳ��IP
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetExternalIP(const char*cszExternalIP)
{
	CallMgr::Instance().SetExternalIP(cszExternalIP);
}


/******************************************************************************
* RequestMainVideoKeyFrame
*��������������I֡
*���룺cszCallID		-���б�ʶ
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::RequestMainVideoKeyFrame(const char*cszCallID)
{
	CallMgr::Instance().RequestMainVideoKeyFrame(cszCallID);
}

/******************************************************************************
* RequestSecondVideoKeyFrame
*������������I֡
*���룺cszCallID		-���б�ʶ
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::RequestSecondVideoKeyFrame(const char*cszCallID)
{
	CallMgr::Instance().RequestSecondVideoKeyFrame(cszCallID);
}

/******************************************************************************
* SetLocalMainVideoSource
*���������ñ�������ƵԴ
*���룺cszCallID		  -���б�ʶ
nMainVideoDevID    -�����豸ID
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetLocalMainVideoSource(const char*cszCallID, int nMainVideoDevID)
{
	CallMgr::Instance().SetLocalMainVideoSource(cszCallID, nMainVideoDevID);
}
/******************************************************************************
* SetLocalSecondVideoSource
*���������ñ��ظ���ƵԴ
*���룺cszCallID		  -���б�ʶ
nSecondVideoDevID  -�����豸ID
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetLocalSecondVideoSource(const char*cszCallID, int nSecondVideoDevID)
{
	CallMgr::Instance().SetLocalSecondVideoSource(cszCallID, nSecondVideoDevID);
}

/******************************************************************************
*����:	��Զ˷����Զ���Ǳ���Ϣ
*����:	cszCallID		-���б�ʶ
cszContent		-�Զ���Ǳ�׼��Ϣ����
nContentLen		-�Զ���Ǳ�׼��Ϣ����
cszMimeType		-�Զ���Ǳ���Ϣ�������� ��ΪXML ֵ��Ϊapplication/xml,��Ϊһ���ַ�������ΪNULL
*****************************************************************************/
int IMXEC_Call::SendCallCustomMsg(const char*cszCallID,const char * cszContent,int nContentLen,const char * cszMimeType)
{
	return CallMgr::Instance().SendCallCustomMsg(cszCallID,cszContent,nContentLen,cszMimeType);
}

/******************************************************************************
* HotDisplayVideo
*��������Ƶ��ʾ֧����ʾ���Ȳ�νӿ�
*���룺	cszCallID		    -���б�ʶ
hWndLocal			-��������Ƶ��ʾ�Ĵ��ھ��
hWndRemote			-Զ������Ƶ��ʾ�Ĵ��ھ��
hWndSecond         	-Զ�̸�����Ƶ��ʾ�Ĵ��ھ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_Call::HotDisplayVideo(const char*cszCallID,HWND hWndLocal,HWND hWndRemote,HWND hWndSecond)
{
	return CallMgr::Instance().HotDisplayVideo(cszCallID,hWndLocal,hWndRemote,hWndSecond);
}

/******************************************************************************
* GetCallH323ID
*��������ȡH323ID
*���룺	cszCallID		-���б�ʶ
szH323ID        -H323ID Buf
nMaxLen         -Buf ��󳤶�
*�����	��
*����ֵ����ȡ������H323ID���ȣ����򷵻�NULL
*****************************************************************************/
int IMXEC_Call::GetCallH323ID(const char*cszCallID ,char * szH323ID,int nMaxLen)
{
	return CallMgr::Instance().GetCallH323ID(cszCallID ,szH323ID,nMaxLen);
}

/******************************************************************************
* GetCallE164ID
*��������ȡE164ID
*���룺	cszCallID		-���б�ʶ
szE164ID        -E164ID Buf
nMaxLen         -Buf ��󳤶�
*�����	��
*����ֵ����ȡ������E164ID���ȣ����򷵻�NULL
*****************************************************************************/

int IMXEC_Call::GetCallE164ID(const char*cszCallID ,char * szE164ID,int nMaxLen)
{
	return CallMgr::Instance().GetCallE164ID(cszCallID ,szE164ID,nMaxLen);
}

/******************************************************************************
* SetNetecTransEnable
*�������Ƿ�ʹ��NETEC˽�д���
*���룺	cszCallID			-���б�ʶ
        nEnable             -�Ƿ�ʹ��,1Ϊʹ��,0Ϊ��ʹ��
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetNetecTransEnable(const char*cszCallID,int nEnable)
{
    CallMgr::Instance().SetNetecTransEnable(cszCallID,nEnable);
}

/******************************************************************************
* SetResolutionStand
*�����������ն�֧�ֵ����ֱ�������
*���룺	nResStand			-���ֱ�������
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_Call::SetResolutionStand(ResStandard nResStand)
{
    CallMgr::Instance().m_nResStand =nResStand;
}


#ifdef _TIANZHU_CACHE_
int IMXEC_Call::SetRemoteVideoCache(const char*cszCallID, int nCacheFrameCount)
{
	return CallMgr::Instance().SetRemoteVideoCache(cszCallID,nCacheFrameCount);
}
#endif