
#pragma once


//========================================================================
//类名：通道对讲窗口资源类
//功能：显示对讲通道、码流等信息，调整对讲音量大小
//作者：戴磊
//时间：2008-09-27
//说明：通过设置回调函数地址实现参数回调
//--------------------------------------------------------------------------


#define TALK_STATUS_CONNECT		0
#define TALK_STATUS_LOAD		1
#define TALK_STATUS_OUTPUT		2
#define TALK_STATUS_FAILED		3
#define TALK_STATUS_CALLING		4

#ifndef _HPDLG_TALK_TYPE
#define _HPDLG_TALK_TYPE
enum HPMC_TALK_TYPE
{
	HPMC_TALK_TYPE_NOR,
	HPMC_TALK_TYPE_TEST,
};
#endif

class AFX_EXT_CLASS HPDLG_TalkViewNotify
{
public:
	HPDLG_TalkViewNotify(){};
	~HPDLG_TalkViewNotify(){};
public:
	/*
	函数功能：关闭对讲窗口回调事件
	参    数：strMemberID	对讲人ID
	strDevID	设备ID
	nCloseType 挂断类型 0主动挂断代表结束 1:拒绝挂断 2:60秒无音频自动挂断
	返 回 值：无
	*/
	virtual void CloseTalkViewNotify(std::string strMemberID, std::string strDevID,int nCloseType = 0)=0;

	/*
	函数功能：调整声音回调事件
	参    数：strMemberID	对讲人ID
	strDevID	设备ID
	nVolumePos	声音调节
	返 回 值：无
	*/
	virtual void SetVolNotify(std::string strMemberID, std::string m_strDevID, int nVolumePos)=0;

	/*
	函数功能：取得码流和丢包率
	参    数：uiAudBitrate		音频码流
	AudioLostCountRate	丢包率
	返 回 值：无
	*/
	virtual void GetCodeText(UINT& uiAudBitrate, double& AudioLostCountRate)=0;

	/*
	函数功能：取得音感参数
	参    数：nAudioLevel	音感
	返 回 值：无
	*/
	virtual void GetAudioLevel(int& nAudioLevel)=0;

	/*
	函数功能：设置对讲状态
	参    数：nStatus		对讲状态
	返 回 值：无
	*/
	virtual void GetTalkStatus(int& nStatus)=0;

	//强制中止对讲后，重新开启监控对讲
	virtual void ReStartTalk(string strDevId) = 0;

	//被呼叫模式使用	
	virtual void RecTalkViewNotify(std::string strMemberID, std::string strDevID){};
};

class AFX_EXT_CLASS HPDLG_TalkView
{
public:
	HPDLG_TalkView(void);
	virtual~ HPDLG_TalkView(void);

	/* 函数：创建及显示对话框
	* pParent, 父窗口句柄 
	* bModal, 是否以模态显示*/

	bool Create(HPDLG_TalkViewNotify& rNotify, CWnd* pParent);
	void ShowWindow(int nCmdShow);

	/* 强制关闭对话框*/
	void Destroy();

public:
	//设置对讲设备名称(对话框上显示)
	void SetDevName(CString strDevName);

	//设置对讲设备ID(关闭对讲用)
	void SetDevid(std::string strDevid);

	//设置对讲人ID(组P2P包用)
	void SetMemberID(std::string strMemberid);

	/*设置当前音量值
	* nAudioLevel		音量值*/
	void SetVolumn(int nAudioLevel);
	/*设置当前码流信息
	* uiAudBitrate			音频码流
	* uiAudioLostCount		丢包率*/
	void SetCodeInfo(UINT uiAudBitrate, UINT uiAudioLostCount);

	void SetTalkType(const HPMC_TALK_TYPE& eTalkType);
	/*对讲连接成功*/
	void SetTalkConnected(void);

	/*设置当前对讲状态
	* iStatus			状态值
	*		TALK_STATUS_CONNECT		0		正在连接
	*		TALK_STATUS_LOAD		1		正在载入音频
	*		TALK_STATUS_OUTPUT		2		正在通话
	*		TALK_STATUS_FAILED		3		连接失败
	*		TALK_STATUS_CALLING		4		正在呼叫
	*		TALK_STATUS_BUSY		5		遇忙*/
	void SetTalkStatus(int iStatus);
	void SetTalkBusy(CString csUserName);
	/*设置模式创建之前调用0:为主动呼叫模式1:为被呼叫模式*/
	void SetMode(int nMode);

private:
	void*		m_hWnd;
	bool		m_bWndCreated;
	BOOL		m_bCheckMuteText;
	HPMC_TALK_TYPE		m_eTalkType;
	int			m_nMode;

	
};

