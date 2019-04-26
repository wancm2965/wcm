	//========================================================================
//����������Ƶ����
//���ܣ����Ÿ�������Ƶ
//���ߣ�����
//ʱ�䣺2008-12-17
//˵����
//--------------------------------------------------------------------------
#pragma once

#include <string>
#include "HPDLG_Base.h"
#include <VIDECVQD/VIDECVQDAPI.h>

typedef enum
{
	AVSTATUS_TYPE_NONE		= -1,
	AVSTATUS_TYPE_CONNECT,
	AVSTATUS_TYPE_LOAD,
	AVSTATUS_TYPE_OUTPUT,
	AVSTATUS_TYPE_FAILED,
}HPAV_AVSTATUS_TYPE;

typedef enum
{
	HPAV_SHOW_FILL_WND		= 0,
	HPAV_SHOW_FULL_VIDEO,
}HPAV_VIDEO_SHOW_TYPE;

typedef enum
{
	HPAV_CNNTYPE_NONE		= 0,
	HPAV_CNNTYPE_P2P,
	HPAV_CNNTYPE_HTTP,
	HPAV_CNNTYPE_SOCKS5,
	HPAV_CNNTYPE_TUNNEL,
	HPAV_CNNTYPE_TCP,
	HPAV_CNNTYPE_UDP,
	HPAV_CNNTYPE_RTP,
}HPAV_CNN_TYPE;

#define WM_CHILE_ADDED	(WM_USER + 4321)

class HPAV_API HPAV_PlayerNotify
{
public:
	HPAV_PlayerNotify(){};
	~HPAV_PlayerNotify(){};
public:
	/*	�������ܣ��Ҽ�������ǰ����
		��    ����	strPlayerID��		PlayerID
		�� �� ֵ��*/
	virtual void HPAV_PlayerNotifyRButtonDown(const std::string &strPlayerID) = 0;

	/*	�������ܣ��Ҽ�������ǰ����
		��    ����	strPlayerID��		PlayerID
					bIsRecord��			�Ƿ���¼��
		�� �� ֵ��*/
	virtual void HPAV_PlayerNotifyOnRecord(const std::string &strPlayerID, bool bIsRecord) = 0;

	/* �������ܣ���Ƶ�����Ϣ�ص� */

	virtual void HPAV_PlayerNotifyOnAVDiagnosisInfo(const std::string &strMemberID,const std::string &strFileTime,const std::string &strFileName,unsigned long nResult) = 0;
	
	virtual void HPAV_StartRemoteRecord(const std::string strChannelID,int nChnindex) = 0;
	virtual void HPAV_StopRemoteRecord(const std::string strChannelID,int nChnindex) = 0;
};

class HPAV_API HPAV_Player
{
public:
	HPAV_Player(void){};
	virtual~ HPAV_Player(void){};

public:
	static HPAV_Player* Create(HPAV_PlayerNotify& rNotify, const std::string &strPlayerID, const std::string& memberid, int nCardID, CWnd* pParent);

	virtual void Destroy() = 0;

	/*	�õ����Ŵ��ھ�� */
	virtual CWnd* GetWnd() = 0;

	/*	����PlayerName��ʾ�������� */
	virtual void SetPlayerTitle(CString playertitle) = 0;
	/*	��ʾ/���� ������ */
	virtual void ShowTitle(bool bShow) = 0;
	/*	��ʾ/���� ������ */
	virtual void ShowTitle() = 0;
	/*	�ı�HPAV_Player��IDֵ */
	virtual void ChangePlayerID(const std::string& strNewID, const std::string& memberid, int nCardID) = 0;

	/*	����ͨ����ַ�Ͷ˿ں� */
	virtual void SetChannelAddPort(std::string channeladdr, unsigned short channelport) = 0;
	/*	���� */
	virtual bool Open(unsigned long nAudChanID,unsigned long nVidChanID,const std::string strPeerNodeID,const std::string strPeerNATIP,
		const std::string strPeerLocalIP,unsigned short nPeerLocalPort,const std::string strPeerMCUID,const std::string strPeerMCUIP,unsigned short nPeerMCUPort) = 0;
	/*	������Ƶ */
	virtual void ReceiveAudio(bool bEnable) = 0;
	/*	������Ƶ */
	virtual void ReceiveVideo(bool bEnable) = 0;
	/*	�������� */
	virtual void ReceiveMainVideo(void) = 0;
	/*	�������� */
	virtual void ReceiveSubVideo(void) = 0;
	/*	���ո��� */
	virtual void ReceiveQSubVideo(void) = 0;
	/*	��Ƶ�ж�ʱ�����Ƿ������һ֡ */
	virtual void KeepLastFrame(bool bIsKeep) = 0;

	/*	��ȡ��ǰ����״̬ */
	virtual void GetConnectStatus(bool &bAudio, bool &bVideo, int &nVideoStream) = 0;
	/*	��ȡ��ǰ��Ƶ״̬ */
	virtual HPAV_AVSTATUS_TYPE GetAudioStatus() = 0;

	/*	�õ��������ͼ�IP��ַ��ID�� */
	virtual void GetNetType(std::string& mcuid, std::string& mcuaddr, unsigned long &ulAudioID, unsigned long &ulVideoID) = 0;
	/*	�õ���Ƶ�������� */
	virtual HPAV_CNN_TYPE GetVideoCnnType() = 0;
	/*	�õ���Ƶ�������� */
	virtual HPAV_CNN_TYPE GetAudioCnnType() = 0;

	/*	�õ�memberid��cardid */
	virtual std::string GetPlayerID(std::string &memberid, unsigned long &cardid) = 0;
	virtual void SetRoundSource(const std::string& strSourceID,unsigned long nSourcecardid) = 0;
	virtual void GetRoundSource(std::string& strSourceID,unsigned long& nSourcecardid) = 0;
	/* �õ�����Ƶ���õ��ܴ��� */
	virtual unsigned long GetBandwidth(void) = 0;
	/* �õ�����Ƶ������Ϣ */
	virtual void GetAVBitrate(unsigned long &ulAudioBitrate, double &ulAudioCurPacketLossRate, unsigned long &ulVideoBitrate, double &ulVideoCurPacketLossRate) = 0;
	/* �õ���Ƶ��С */
	virtual void GetAVSize(unsigned int &uiVideoWidth, unsigned int &uiVideoHeight) = 0; 
	/* �õ�������ֵ */
	virtual int GetAudioLevel(void) = 0;

	/* ����ץ��·��������·���� */
	virtual void SetCapturePath(std::string strCapturePath) = 0;
	/* ץ��ͼ�� */
	virtual bool CapturePicture() = 0;
	/* ����ץ�� */
	virtual void SeriateCapture() = 0;

	/* �������� */
	virtual void SetVolume(int iVolume) = 0;
	/* ��ʾ��Ƶ���öԻ��� */
	virtual void ShowVideoParamDlg() = 0;

	/* ����¼�񱣴�·�� */
	virtual void SetRecordPath(std::string strRecordPath, std::string strLocalMemberID) = 0;
	/* ��ʼ¼�� */
	virtual bool StartRecord() = 0;
	/* ֹͣ¼�� */
	virtual void StopRecord() = 0;
	/* �Ƿ�����¼�� */
	virtual bool IsRecording() = 0;

	/* �������ݿ�·�� */
	virtual void SetDBPath(std::string strDBPath) = 0;

	/* ������Ƶ��䴰����ʾ */
	virtual void VRFillWnd() = 0;
	/* ������Ƶ����ȫ����ʾ */
	virtual void VRFullVideo() = 0;
	/* ��ȡ��Ƶ��ʾ��ʽ */
	virtual HPAV_VIDEO_SHOW_TYPE GetVideoShowMode() = 0;

	/*	���ò��Ŵ����Ƿ�Enable */
	virtual void SetPlayerDlgEnable(BOOL bEnable) = 0;

	//�Ƿ���ʾ������ʾ
	virtual void ShowConnectTip(bool bShow=false) = 0;
	/* ��ʼ��� */
	virtual int StartVQD(unsigned long nDetectFlags=VQDS_SIGNALLOST|VQDS_COLORCAST|VQDS_BLUR|VQDS_BRIGHT|VQDS_FREEZE|VQDS_NOISE|VQDS_JITTER) = 0;
	/* ֹͣ��� */
	virtual void StopVQD(void) = 0;

	virtual int ChangeAVInfo(unsigned long nAudChanID,unsigned long nVidChanID,std::string strPeerNodeID,std::string strPeerNATIP,std::string strPeerLocalIP,
		unsigned short nPeerLocalPort,std::string strPeerMCUID,std::string strPeerMCUIP,unsigned short nPeerMCUPort) = 0;
	
	//Զ��¼��
	virtual void SetRemoteRecordDlg(bool isRecording){};
	virtual bool IsRemoteRecord(){return false;};
	virtual void SetRemoteRecordState(bool bRecording){};
};
