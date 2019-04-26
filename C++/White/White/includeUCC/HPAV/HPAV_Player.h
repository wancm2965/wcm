	//========================================================================
//类名：音视频播放
//功能：播放各种音视频
//作者：戴磊
//时间：2008-12-17
//说明：
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
	/*	函数功能：右键单击当前窗口
		参    数：	strPlayerID：		PlayerID
		返 回 值：*/
	virtual void HPAV_PlayerNotifyRButtonDown(const std::string &strPlayerID) = 0;

	/*	函数功能：右键单击当前窗口
		参    数：	strPlayerID：		PlayerID
					bIsRecord：			是否在录像
		返 回 值：*/
	virtual void HPAV_PlayerNotifyOnRecord(const std::string &strPlayerID, bool bIsRecord) = 0;

	/* 函数功能：视频诊断信息回调 */

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

	/*	得到播放窗口句柄 */
	virtual CWnd* GetWnd() = 0;

	/*	设置PlayerName显示标题栏用 */
	virtual void SetPlayerTitle(CString playertitle) = 0;
	/*	显示/隐藏 标题栏 */
	virtual void ShowTitle(bool bShow) = 0;
	/*	显示/隐藏 标题栏 */
	virtual void ShowTitle() = 0;
	/*	改变HPAV_Player的ID值 */
	virtual void ChangePlayerID(const std::string& strNewID, const std::string& memberid, int nCardID) = 0;

	/*	设置通道地址和端口号 */
	virtual void SetChannelAddPort(std::string channeladdr, unsigned short channelport) = 0;
	/*	连接 */
	virtual bool Open(unsigned long nAudChanID,unsigned long nVidChanID,const std::string strPeerNodeID,const std::string strPeerNATIP,
		const std::string strPeerLocalIP,unsigned short nPeerLocalPort,const std::string strPeerMCUID,const std::string strPeerMCUIP,unsigned short nPeerMCUPort) = 0;
	/*	接收音频 */
	virtual void ReceiveAudio(bool bEnable) = 0;
	/*	接收视频 */
	virtual void ReceiveVideo(bool bEnable) = 0;
	/*	接收主流 */
	virtual void ReceiveMainVideo(void) = 0;
	/*	接收子流 */
	virtual void ReceiveSubVideo(void) = 0;
	/*	接收辅流 */
	virtual void ReceiveQSubVideo(void) = 0;
	/*	视频中断时画面是否保留最后一帧 */
	virtual void KeepLastFrame(bool bIsKeep) = 0;

	/*	获取当前连接状态 */
	virtual void GetConnectStatus(bool &bAudio, bool &bVideo, int &nVideoStream) = 0;
	/*	获取当前音频状态 */
	virtual HPAV_AVSTATUS_TYPE GetAudioStatus() = 0;

	/*	得到网络类型及IP地址、ID号 */
	virtual void GetNetType(std::string& mcuid, std::string& mcuaddr, unsigned long &ulAudioID, unsigned long &ulVideoID) = 0;
	/*	得到视频连接类型 */
	virtual HPAV_CNN_TYPE GetVideoCnnType() = 0;
	/*	得到音频连接类型 */
	virtual HPAV_CNN_TYPE GetAudioCnnType() = 0;

	/*	得到memberid、cardid */
	virtual std::string GetPlayerID(std::string &memberid, unsigned long &cardid) = 0;
	virtual void SetRoundSource(const std::string& strSourceID,unsigned long nSourcecardid) = 0;
	virtual void GetRoundSource(std::string& strSourceID,unsigned long& nSourcecardid) = 0;
	/* 得到音视频所用的总带宽 */
	virtual unsigned long GetBandwidth(void) = 0;
	/* 得到音视频码流信息 */
	virtual void GetAVBitrate(unsigned long &ulAudioBitrate, double &ulAudioCurPacketLossRate, unsigned long &ulVideoBitrate, double &ulVideoCurPacketLossRate) = 0;
	/* 得到视频大小 */
	virtual void GetAVSize(unsigned int &uiVideoWidth, unsigned int &uiVideoHeight) = 0; 
	/* 得到音量感值 */
	virtual int GetAudioLevel(void) = 0;

	/* 设置抓拍路径（绝对路径） */
	virtual void SetCapturePath(std::string strCapturePath) = 0;
	/* 抓拍图像 */
	virtual bool CapturePicture() = 0;
	/* 连续抓拍 */
	virtual void SeriateCapture() = 0;

	/* 设置音量 */
	virtual void SetVolume(int iVolume) = 0;
	/* 显示视频设置对话框 */
	virtual void ShowVideoParamDlg() = 0;

	/* 设置录像保存路径 */
	virtual void SetRecordPath(std::string strRecordPath, std::string strLocalMemberID) = 0;
	/* 开始录像 */
	virtual bool StartRecord() = 0;
	/* 停止录像 */
	virtual void StopRecord() = 0;
	/* 是否正在录像 */
	virtual bool IsRecording() = 0;

	/* 设置数据库路径 */
	virtual void SetDBPath(std::string strDBPath) = 0;

	/* 设置视频填充窗口显示 */
	virtual void VRFillWnd() = 0;
	/* 设置视频比例全景显示 */
	virtual void VRFullVideo() = 0;
	/* 获取视频显示方式 */
	virtual HPAV_VIDEO_SHOW_TYPE GetVideoShowMode() = 0;

	/*	设置播放窗口是否Enable */
	virtual void SetPlayerDlgEnable(BOOL bEnable) = 0;

	//是否显示连接提示
	virtual void ShowConnectTip(bool bShow=false) = 0;
	/* 开始诊断 */
	virtual int StartVQD(unsigned long nDetectFlags=VQDS_SIGNALLOST|VQDS_COLORCAST|VQDS_BLUR|VQDS_BRIGHT|VQDS_FREEZE|VQDS_NOISE|VQDS_JITTER) = 0;
	/* 停止诊断 */
	virtual void StopVQD(void) = 0;

	virtual int ChangeAVInfo(unsigned long nAudChanID,unsigned long nVidChanID,std::string strPeerNodeID,std::string strPeerNATIP,std::string strPeerLocalIP,
		unsigned short nPeerLocalPort,std::string strPeerMCUID,std::string strPeerMCUIP,unsigned short nPeerMCUPort) = 0;
	
	//远程录像
	virtual void SetRemoteRecordDlg(bool isRecording){};
	virtual bool IsRemoteRecord(){return false;};
	virtual void SetRemoteRecordState(bool bRecording){};
};
