//========================================================================
//��������������Ƶ�ɼ���
//���ܣ��ɼ���������Ƶ
//���ߣ�����
//ʱ�䣺2008-2-26
//˵����
//--------------------------------------------------------------------------
#pragma once

#include "../../HPLoadData/HPLoadiniData.h"
/*
typedef struct
{
	bool				HPImageNoise;
	int 				HPVideoSize;
	int 				nFrameRate;
	int 				nQuality;
	VIDEC_CODEC_TYPE	codecType;
	int 				nBitrateControlType;
	bool				bEnableSub;
	int 				nSubScaleType;
	int 				nSubQuality;
}HPAV_VIDEO_CAPTURE_TYPE;
*/

class HPAV_API HPAV_ChannelLocalNotify
{
public:
	HPAV_ChannelLocalNotify(){};
	~HPAV_ChannelLocalNotify(){};
public:
	/*	�������ܣ��رյ�ǰ����
		��    ����	pWindow��		��ǰ���ڵ�ָ��
					ulSessionID��	���
		�� �� ֵ��*/
	//virtual void HPAV_PlayerNotifyClose(HPAV_Player* pWindow, unsigned long ulSessionID) = 0;

	/*	�������ܣ��ر����д���
		��    ����	pWindow��		��ǰ���ڵ�ָ��
					ulSessionID��	���
		�� �� ֵ��*/
	//virtual void HPAV_PlayerNotifyCloseAll(HPAV_Player* pWindow, unsigned long ulSessionID) = 0;
};

class HPAV_API HPAV_ChannelLocal
{
public:
	HPAV_ChannelLocal(void){};
	virtual ~ HPAV_ChannelLocal(void){};

public:
	virtual void OpenAudioCapture(int nDevID, X_AUDIO_CODEC_ID CodecType) = 0;
	virtual void CloseAudioCapture() = 0;
	virtual void OpenVideoCapture(int nDevID, int nSizeIndex, int nFrameRate, int nBitrate, int nBitrateControlType, VIDEC_CODEC_TYPE CodecType, int nSub, int nQSub) = 0;
	virtual void CloseVideoCapture() = 0;

	//��ȡ������ֵ������200����ȡһ�Σ�
	virtual int  GetAudioLevel(void) = 0;
	//��ȡռ�ô���ֵ
	virtual unsigned long GetBandwidth(void) = 0;
	//��ȡ����Ƶ����ֵ
	virtual void GetAVBitrate(unsigned long& ulAudioBitrate, unsigned long& ulVideoBitrate) = 0;
	//��ȡ��Ƶ��С
	virtual void GetAVSize(unsigned short& uWidth, unsigned short& uHeight) = 0;
	//��ȡ��������Ƶͨ��ID
	virtual void GetChannelLocalID(unsigned long& ulAudioChannel, unsigned long& ulVideoChannel) = 0;

	/* ����¼�񱣴�·�� */
	virtual void SetRecordPath(std::string strRecordPath, std::string strLocalMemberID) = 0;
	/* ��ʼ¼�� */
	virtual bool StartRecord() = 0;
	/* ֹͣ¼�� */
	virtual void StopRecord() = 0;
	/* �Ƿ�����¼�� */
	virtual bool IsRecording() = 0;

	/* Ӧ����Ƶ��Ļ */
	virtual void ApplyOSD(BOOL bUseOSD, int iVODLocation, std::string strText) = 0;
	virtual void SetVODFormat(CHARFORMAT cfVOD) = 0;
	virtual CHARFORMAT GetVODFormat(void) = 0;
	virtual void SetVODBackColor(COLORREF clrBack) = 0;
	virtual COLORREF GetVODBackColor(void) = 0;

	virtual HWND GetSafeHwnd() const = 0;
	virtual BOOL ShowWindow(int nCmdShow) = 0;
	virtual BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) = 0;
	virtual void GetWindowRect(LPRECT lpRect) const = 0;
	virtual bool IsWindowVisible(void) = 0;
};


class HPAV_API HPAV_ChannelLocalMgrNotify
{
public:
	HPAV_ChannelLocalMgrNotify(){};
	~HPAV_ChannelLocalMgrNotify(){};
public:
	/*	�������ܣ��رյ�ǰ����
		��    ����	pWindow��		��ǰ���ڵ�ָ��
					ulSessionID��	���
		�� �� ֵ��*/
	//virtual void HPAV_PlayerNotifyClose(HPAV_Player* pWindow, unsigned long ulSessionID) = 0;

	/*	�������ܣ��ر����д���
		��    ����	pWindow��		��ǰ���ڵ�ָ��
					ulSessionID��	���
		�� �� ֵ��*/
	//virtual void HPAV_PlayerNotifyCloseAll(HPAV_Player* pWindow, unsigned long ulSessionID) = 0;
};

class HPAV_API HPAV_ChannelLocalMgr
{
public:
	HPAV_ChannelLocalMgr(void){};
	virtual~ HPAV_ChannelLocalMgr(void){};

public:
	/*	�������ܣ�����CHPAVChannelLocalMgr����
		��    ����
		�� �� ֵ��*/
	virtual void Destory(void) = 0;

	/*	�������ܣ�����MCU
		��    ����
		�� �� ֵ��*/
	virtual void SetMCU(std::string mcuid, std::string mcuaddr, unsigned short mcuport, std::string mcutype) = 0;

	/*	�������ܣ���������ͨ��
		��    ����	ulChannelIndex��	ͨ�����
					ulAudioChannel��	��Ƶͨ��
					ulVideoChannel��	��Ƶͨ��
					ulVideoChannelSub��	��Ƶ����ͨ��
		�� �� ֵ��*/
	virtual HPAV_ChannelLocal* CreateHPAV_ChannelLocal(unsigned long ulChannelIndex,CWnd* pParentWnd) = 0;

	/*	�������ܣ�������ضԽ�ͨ��
		��    ����
		�� �� ֵ��*/
	virtual HPAV_ChannelLocal* CreateMonitorAudioChannel(int nDevID, X_AUDIO_CODEC_ID CodecType) = 0;

	/*	�������ܣ���ȡ��������Ƶͨ��
		��    ����	ulChannelIndex��	ͨ�����
		�� �� ֵ��*/
	virtual HPAV_ChannelLocal* GetHPAV_ChannelLocal(unsigned long ulChannelIndex) = 0;

	/*	�������ܣ���ȡ��������Ƶͨ��
		��    ����
		�� �� ֵ��*/
	virtual HPAV_ChannelLocal* GetHPAV_MonitorAudioChannel() = 0;

	/*	�������ܣ���ȡ��������Ƶռ�õ��ܴ���
		��    ����
		�� �� ֵ��*/
	virtual unsigned long GetChannelLocal_Bandwidth() = 0;
};

HPAV_API HPAV_ChannelLocalMgr* CreateHPAV_ChannelLocalMgr(HPAV_ChannelLocalMgrNotify& rNotify, unsigned long ulSessionID = 0);
HPAV_API void DestoryHPAV_ChannelLocalMgr(void);
HPAV_API HPAV_ChannelLocalMgr* GetHPAV_ChannelLocalMgr(void);
