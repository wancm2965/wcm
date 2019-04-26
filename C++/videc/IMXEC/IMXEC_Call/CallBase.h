//CallBase.h
#ifndef __CALLBASE_H__
#define __CALLBASE_H__

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
#include <IMXEC/IMXEC_VideoSetting.h>

#include <NETEC/NETEC_MediaSender.h>
#include <NETEC/NETEC_Node.h>

#include "CapChanRTP.h"
#include "PlayChanRTP.h"
#include "XBitrate.h"



class CallBase
{
public:
	CallBase(void){};
	virtual ~CallBase(void){};
public:
	virtual void Close(void)=0;
	/******************************************************************************
	* SetLocalMainVideoWnd
	*���������ñ���������Ƶ��ʾ�Ĵ���
	*���룺	hWnd			-��������Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetLocalMainVideoWnd(HWND hWnd)=0;

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
	virtual int SetLocalMainVideoWndPos(int x,int y,int cx,int cy)=0;

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
	virtual void SetLocalMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)=0;

	/******************************************************************************
	* SetLocalMainVideoEnableOSD
	*���������ñ���������Ƶ�Ƿ�����OSD
	*���룺	cszCallID		-���б�ʶ
			nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoEnableOSD(int nEnable)=0;

	/******************************************************************************
	* SetLocalMainVideoOSDText
	*���������ñ���������ƵOSD����
	*���룺	cszCallID		-���б�ʶ
			cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoOSDText(const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT)=0;//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetLocalMainVideoOSDFont
	*���������ñ���������ƵOSD����,�ֵ���ɫ�ͱ���ɫ
	*���룺	logfont			-OSD����
			crText			-�ֵ���ɫ
			crBk			-����ɫ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalMainVideoOSDFont(LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255))=0;

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
	virtual int GetLocalMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)=0;

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
	virtual int GetLocalMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;

	/******************************************************************************
	* SetRemoteMainVideoWnd
	*����������Զ��������Ƶ��ʾ�Ĵ���
	*���룺	��
			hWnd			-Զ������Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetRemoteMainVideoWnd(HWND hWnd)=0;

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
	virtual int SetRemoteMainVideoWndPos(int x,int y,int cx,int cy)=0;

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
	virtual void SetRemoteMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)=0;

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
	virtual int GetRemoteMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)=0;

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
	virtual int GetRemoteMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;

	/******************************************************************************
	* SetSecondVideoWnd
	*���������ø�����Ƶ��ʾ�Ĵ���
	*���룺��
			hWnd			-Զ�̸�����Ƶ��ʾ�Ĵ��ھ��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetSecondVideoWnd(HWND hWnd)=0;

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
	virtual int SetSecondVideoWndPos(int x,int y,int cx,int cy)=0;

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
	virtual void SetSecondVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)=0;

	/******************************************************************************
	* SetSecondVideoEnableOSD
	*���������ø�����Ƶ�Ƿ�����OSD
	*���룺	nEnable			-�Ƿ�����OSD
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoEnableOSD(int nEnable)=0;

	/******************************************************************************
	* SetSecondVideoOSDText
	*���������ø�����ƵOSD����
	*���룺	cszText			-OSD����
			nAlign			-���뷽ʽ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoOSDText(const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT)=0;//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetSecondVideoOSDFont
	*���������ø�����ƵOSD����,�ֵ���ɫ�ͱ���ɫ
	*���룺	logfont			-OSD����
			crText			-�ֵ���ɫ
			crBk			-����ɫ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoOSDFont(LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255))=0;

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
	virtual int GetSecondVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd)=0;
	
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
	virtual int GetSecondVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;


	/******************************************************************************
	* GetLocalAudioLevel
	*��������ȡ������ƵLevel��ʵ��������С��
	*���룺	��
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalAudioLevel(int&nLevel)=0;

	/******************************************************************************
	* SetLocalAudioVolume
	*��������ȡ������Ƶ����
	*���룺	nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalAudioVolume(int nVolume)=0;

	/******************************************************************************
	* SetLocalAudioMute
	*��������ȡ������Ƶ�Ƿ���
	*���룺	nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalAudioMute(int nMute)=0;

	/******************************************************************************
	* GetLocalAudioCodecID
	*��������ȡ������Ƶ������Ϣ
	*���룺	��
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetLocalAudioCodecID(X_AUDIO_CODEC_ID &nAudioCodecID)=0;

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
	virtual int GetLocalAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;

	/******************************************************************************
	* GetRemoteAudioLevel
	*��������ȡԶ����ƵLevel��ʵ��������С��
	*���룺	��
	*�����	nLevel			-ʵ��������С��0-10��
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteAudioLevel(int&nLevel)=0;

	/******************************************************************************
	* SetRemoteAudioVolume
	*��������ȡԶ����Ƶ����
	*���룺	nVolume			-����
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteAudioVolume(int nVolume)=0;

	/******************************************************************************
	* SetRemoteAudioMute
	*��������ȡԶ����Ƶ�Ƿ���
	*���룺	nMute			-�Ƿ���
	*�����	��
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteAudioMute(int nMute)=0;

	/******************************************************************************
	* GetRemoteAudioCodecID
	*��������ȡԶ����Ƶ����
	*���룺	��
	*�����	nAudioCodecID	-����ID
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetRemoteAudioCodecID (X_AUDIO_CODEC_ID &nAudioCodecID)=0;

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
	virtual int GetRemoteAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;

	/******************************************************************************
	* StartRecordLocalMainToAVI
	*��������ʼ��������¼��
	*���룺	cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordLocalMainToAVI(const char*cszPathName,int nStandardFormat=1)=0;
	
	/******************************************************************************
	* StopRecordLocalMainToAVI
	*������ֹͣ��������¼��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordLocalMainToAVI(void)=0;

	/******************************************************************************
	* StartRecordRemoteMainToAVI
	*��������ʼԶ������¼��
	*���룺	cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordRemoteMainToAVI(const char*cszPathName,int nStandardFormat=1)=0;
	
	/******************************************************************************
	* StopRecordRemoteMainToAVI
	*������ֹͣԶ������¼��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordRemoteMainToAVI(void)=0;

	/******************************************************************************
	* StartRecordSecondToAVI
	*��������ʼ����¼��
	*���룺	cszPathName			-¼���ļ�����
			nStandardFormat		-�Ƿ�¼�Ƴɱ�׼�ļ���ʽ
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int StartRecordSecondToAVI(const char*cszPathName,int nStandardFormat=1)=0;
	
	/******************************************************************************
	* StopRecordSecondToAVI
	*������ֹͣ����¼��
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void StopRecordSecondToAVI(void)=0;

	/******************************************************************************
	* GetMainAudioID
	*��������ȡ������Ƶ��ʶ
	*���룺 ��
	*����� nMainAudioID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetMainAudioID(unsigned long&nMainAudioID)=0;

	/******************************************************************************
	* GetMainVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 ��
	*����� nMainVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetMainVideoID(unsigned long&nMainVideoID)=0;

	/******************************************************************************
	* GetSecondVideoID
	*��������ȡ������Ƶ��ʶ
	*���룺 ��
	*����� nSecondVideoID	-������Ƶ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int GetSecondVideoID(unsigned long&nSecondVideoID)=0;

	/******************************************************************************
	* SetMainVideoScreenIndex
	*����������������Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetMainVideoScreenIndex(int nScreenIndex)=0;

	/******************************************************************************
	* SetSecondVideoScreenIndex
	*���������ø�����Ƶͼ���������
	*���룺 cszCallID		-���б�ʶ
			nScreenIndex	-�������
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SetSecondVideoScreenIndex(int nScreenIndex)=0;


	/******************************************************************************
	* SetSecondVideoScreenWnd
	*���������ø�����Ƶ������Ļ����λ��
	*���룺	nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;

	/******************************************************************************
	* SetRemoteVideoScreenWnd
	*����������Զ����Ƶ������Ļ����λ��
	*���룺	nScreenIndex	-��Ļ��
			nWndIndex		-���ں�
			nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;

	/******************************************************************************
	* SetLocalRecordCallback
	*���������ñ���¼��ص�
	*���룺	pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetLocalRecordCallback(IMXEC_RecordCallback*pRecordCallback)=0;

	/******************************************************************************
	* SetRemoteRecordCallback
	*����������Զ��¼��ص�
	*���룺pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetRemoteRecordCallback(IMXEC_RecordCallback*pRecordCallback)=0;

	/******************************************************************************
	* SetSecondRecordCallback
	*���������ø���¼��ص�
	*���룺	pRecordCallback	-¼��ص�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void SetSecondRecordCallback(IMXEC_RecordCallback*pRecordCallback)=0;

	/******************************************************************************
	* SaveLocalMainVideoToBMPFile
	*���������汾��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveLocalMainVideoToBMPFile(const char*cszPathName)=0;

	/******************************************************************************
	* SaveRemoteMainVideoToBMPFile
	*����������Զ��������Ƶ��BMPͼƬ�ļ�
	*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveRemoteMainVideoToBMPFile(const char*cszPathName)=0;

	/******************************************************************************
	* SaveSecondVideoToBMPFile
	*���������渨����Ƶ��BMPͼƬ�ļ�
	*���룺	cszPathName		-ͼƬ�ļ�ȫ·��
	*�������
	*����ֵ���ɹ�����0��ʧ�ܷ���-1
	*****************************************************************************/
	virtual int SaveSecondVideoToBMPFile(const char*cszPathName)=0;
};

void GetVideoSizeByH264Level(int nH264Level,VIDEO_FORMAT_TYPE nVideoFormatType,int&nWidth,int&nHeight);

#endif