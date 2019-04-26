//IMXCallMgr.h
#ifndef __IMXCALLMGR_H__
#define __IMXCALLMGR_H__

#include <IMXEC/IMXEC_IMX.h>
#include <IMXEC/IMXEC_MediaChannel.h>
#include <NETEC/NETEC_Call.h>
#include "IMXCall.h"
#include "XBufferPool.h"
#include "StrPacket.h"

#include <string>
#include <map>

class IMXCallMgr
	: public IMXEC_CapChanCallback
	, public NETEC_Call
	, public XBufferPool
{
public:
	IMXCallMgr(void);
	virtual~IMXCallMgr(void);
public:
	static IMXCallMgr&Instance(void);
public:
	/******************************************************************************
	* SetUserID
	*�����������û���ʶ
	*���룺 cszUserID		-�û���ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetUserID(const char*cszUserID);

	/******************************************************************************
	* SetUserName
	*�����������û�����
	*���룺 cszUserName		-�û�����
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetUserName(const char*cszUserName);

	/******************************************************************************
	* SetAudioDevParam
	*����������������Ƶ�豸����
	*���룺 nAudioCodecID		-��Ƶ����
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID);

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
	virtual int SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType);

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
	*����ֵ����
	*****************************************************************************/
	virtual void Stop(void);

	/******************************************************************************
	* MakeCall
	*�������������
	*���룺	cszUserID		-�Է����û�ID��IP
	*�������
	*����ֵ�����б�ʶ
	*****************************************************************************/
	virtual const char*MakeCall(const char*cszUserID);

	/******************************************************************************
	* AcceptCall
	*���������ܺ�������
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int AcceptCall(const char*cszCallID);

	/******************************************************************************
	* HangupCall
	*�������ҶϺ���
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int HangupCall(const char*cszCallID);



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
	virtual int GetLocalMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);

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
	* SaveLocalMainVideoToBMPFile
	*���������汾��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveLocalMainVideoToBMPFile(const char*cszPathName);


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
	* GetLocalMainVideoInfo
	*��������ȡ����������Ƶ��Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);

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
	* GetRemoteMainVideoInfo
	*��������ȡԶ��������Ƶ��Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);

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

	/******************************************************************************
	* GetSecondVideoInfo
	*��������ȡ������Ƶ��Ϣ
	*���룺	cszCallID		-���б�ʶ
	*�����	nWidth			-ͼ����
			nHeight			-ͼ��߶�
			nVideoCodecType	-��������
			hVideoWnd		-���ھ��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);

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

	void OnTimer(UINT nIDEvent);
protected:
	virtual void OnNETEC_CallRingOut(const char*cszPeerUserID,const char*cszPeerUserName,const char*cszCallID,const char*cszCallType);
	virtual void OnNETEC_CallRingIn(const char*cszPeerUserID,const char*cszPeerUserName,const char*cszPeerSDP,const char*cszCallID,const char*cszCallType);
	virtual void OnNETEC_CallEstablished(const char*cszPeerUserID,const char*cszPeerUserName,const char*cszPeerSDP,const char*cszCallID,const char*cszCallType);
	virtual void OnNETEC_CallDestroyed(const char*cszPeerUserID,const char*cszPeerUserName,const char*cszCallID,const char*cszCallType);
protected:
	std::string			m_strUserID;
	std::string			m_strUserName;

	IMXEC_CapChanDev*	m_pMainCapChanDev;

	typedef std::map<std::string,IMXCall*>	MapIMXCall;
	MapIMXCall			m_MapIMXCall;
	UINT32				m_nIDEventTimer;

	X_AUDIO_CODEC_ID	m_nAudioCodecID;
	int					m_nMainVideoDevID;
	int					m_nMainVideoFrameRate;
	int					m_nMainVideoBitrate;
	VIDEC_CODEC_TYPE	m_nMainVideoCodecType;
	VIDEO_FORMAT_TYPE	m_nMainVideoFormatType;
};

#endif
