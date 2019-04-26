#ifndef _HPMEETING_H
#define _HPMEETING_H

#define TYPE_ZR				0				    //中人新 1:为中人新

#define NEW_RECORD_VERSION	"AVCON6.0.1"		//新的录像库版本定义   

#define LEN_MEET_NAME		20
#define LEN_MEET_DESC		100
#define LEN_MEET_PASSWORD	20
#define DROP_DEV_LIST_TO_VIEW 0x12			//REQ20071112

#define HAVE_KMONCLIENT
#include "KMONDefine.h"
#include "StructDefine.h"
#include "ConferencePlayer/IAVSPlayerNotify.h"
#include "KIMSDefine.h"
#include "KMMSDefine.h"
#include "DeviceClient.h"
#include <imxec/IMXEC_MediaChannel.h>
#include "AVCONDefine.h"

// 权限位命令
typedef enum PRIVILEGE_BIT
{
	PRIVILEGE_BIT_SIP		= 1,	//SIP呼叫
	PRIVILEGE_BIT_SMS		= 2,	//SMS短信
	PRIVILEGE_BIT_MMS		= 3,	//会议管理(WEB)
	PRIVILEGE_BIT_MON		= 4,	//监控点播(WEB)
	PRIVILEGE_BIT_TEM		= 5,	//模板设置
	PRIVILEGE_BIT_FAX		= 6,	//收发传真
	PRIVILEGE_BIT_MSG		= 7,	//机构广播消息
	PRIVILEGE_BIT_BAD		= 8,	//发布公告
	PRIVILEGE_BIT_MAP		= 9,	//地图编辑
	PRIVILEGE_BIT_MAT		= 10,	//矩阵控制
	PRIVILEGE_BIT_LOS		= 11,	//定损权限
	PRIVILEGE_BIT_INFO		= 14,	//修改个人设置
	PRIVILEGE_BIT_ORG		= 17,	//显示机构权限;
	PRIVILEGE_BIT_VOD		= 21,	//上传录像点播权限;
	PRIVILEGE_BIT_MEET_MON	= 23,	//会议监控权限
	PRIVILEGE_BIT_REC		= 24,	//远程录像
};


enum {
	CON_AFFICHE = 0,				//会议公告
	CON_TITLE,						//会议标题
	CON_CALLING,					//会议点名
	CON_MASS,						//批操作
	LOCK_CON,						//锁定会议
	SOUND_BESTIR,					//语音激励
	CON_SYNC,						//同步模式
	CON_DESYNC,						//独立模式
	CON_INDEPENDENCE,				//主控模式
	CON_LABIAL_SOUND,				//唇音同步
	LAND_TIMER_TAXIS,				//登录时间排序
	NAME_TAXIS,						//名称排序
	TYPE_TAXIS,						//人员类型排序
	SET_CON_AFFICHE,				//设置公告
	SET_CON_TITLE,					//设置标题
	SET_CON_HAND,					//设置会议举手
	ALLOW_DISCUSS,					//允许讨论
	SCREEN_CONTROL,					//允许申请屏幕控制
	NOTE_INFORM,					//短信通知
	CON_IN_PHASE,					//会议同步
	IN_PHASE_CHOOSE_1,				//选 1 屏
	IN_PHASE_CHOOSE_2,				//选 2 屏
	IN_PHASE_CHOOSE_3,				//选 3 屏
	IN_PHASE_CHOOSE_4,				//选 4 屏
	SCREEN_CHANGE,					//切屏
	CONF_KINESCOPE,					//会议录像
	CONF_RECORD_REMOTE,				//远程录像
	CONF_RECORD_REMOTE_SET,			//远程录像格式
	CONF_AVDIO,						//视频
	CONF_DOC,						//文档
	FULL_SCREEN,					//全屏
	SMALL_SCRIP,					//小纸条
	SHOWVIDEOTITLE,					//视频标题
	CLOSE_AUDIO_MANAGE,				// 闭音 [3/23/2011 %x.t%]
	CONF_MEDIA,
	CONF_SIPCALL,					//SIP呼叫
};

//回调
enum {
	REV_ALLOW_KINESCOPY = 0,		//允许会议录像
	REV_ALLOW_YT_CONTROL,			//允许控制云台
	REV_ALLOW_SPEAK,				//允许申请发言
	REV_ALLOW_WB,					//允许电子白板
	REV_ALLOW_CONTROL,				//允许监控人员控制
	REC_CON_LABIAL_SOUND,			//唇音同步
	REV_ALLOW_FLOOT,				//允许改变浮动窗口
	REV_CON_AFFICHE,				//会议公告
	REV_CON_TITLE,					//会议标题
	REV_CON_HAND,					//会议举手
	REV_CON_CALLING,				//会议点名
	REV_CON_MASS,					//批操作
	REV_LOCK_CON,					//锁定会议
	REV_SOUND_BESTIR,				//语音激励
	REV_NAME_TAXIS,					//名称排序
	REV_LAND_TIMER_TAXIS,			//登录时间排序
	REV_TYPE_TAXIS,					//人员类型排序
	REV_CON_SYNC,					//同步模式
	REV_CON_DESYNC,					//独立模式
	REV_CON_INDEPENDENCE,			//主控模式
	REV_IN_PHASE_CHOOSE_1,			//选 1 屏
	REV_IN_PHASE_CHOOSE_2,			//选 2 屏
	REV_IN_PHASE_CHOOSE_3,			//选 3 屏
	REV_IN_PHASE_CHOOSE_4,			//选 4 屏
	REV_ALLOW_DISCUSS,				//允许讨论
	REV_SCREEN_CONTROL,				//允许申请屏幕控制
	REV_SCREEN_SHARE_RIGHT,			//屏幕控制权限
	REV_CONF_KINESCOPE,				//会议录像
	REV_CONF_RECORD_REMOTE,			//远程录像
	REV_CONF_RECORD_REMOTE_SET,		//远程录像格式
	REV_CONF_AVDIO,					//视频
	REV_CONF_DOC,					//文档按钮状态
	REV_CONF_DOC_RIGHT,				//文档权限
	REV_FULL_SCREEN,				//全屏
	REV_SMALL_SCRIP,				//小纸条
	REV_YT_CTRL,					//云台控制
	REV_H323_CTRL,					//H323控制
	REV_SHOW_AVINFO,				//视频信息
	REV_CLOSE_AUDIO_MANAGE,			// 闭音 [3/24/2011 %x.t%]
};

enum{
	STREAM_TYPE_AUTO = 0,
	STREAM_TYPE_HIGH,
	STREAM_TYPE_MID,
	STREAM_TYPE_LOW,
};

#define TERM_TYPE_PC		"PC"		//PC版本
#define TERM_TYPE_TERMINAL	"Terminal"	//终端版本
#define TERM_TYPE_RECORD    "record"	//远程录像服务
#define TERM_TYPE_H323MP	"h323mp"	//H323MP

#define SELFGROUP_HOST_ENTER     "host_enter" 
#define SELFGROUP_HOST_STOP		 "host_stop"

class IHPMeetingNotify
{
public:
	virtual void SetCurRoomStatus(_ROOM_STATUS nStatus)=0; //0:无，1：在会议，2：在AView;
	virtual _ROOM_STATUS GetCurRoomStatus(void)=0; //0:无，1：在会议，2：在AView;

	virtual void OnStartHPMCNotify(const std::string& strDomain,
		const std::string& strRoomID,
		const std::string& strPassword,
		const std::string& strMemberID,
		const std::string& strMemberName,
		const std::string& strInviteeGroupID,
		bool bPresider=false,
		const std::string& strUpdateTime="",
		const std::string& strIPType="")=0;//启动监控客户端回调
	virtual void SetHPMCbtnStatus(bool bBtnStatus)=0;//监控客户端按钮回调
	virtual void OnRejectRequestMsg(CString strMsg)=0;
	virtual void OnInviteIntoMeeting(const std::string& strDomain,const std::string& strRoomID,const std::string& strRoomName,const std::string& strInviterNodeID,const std::string& strInviterID, const std::string& strPassword, const std::string& strInviterName,int nInviteAdmOrderID,int nBeInviteAdmOrderID)=0;
	virtual void OnRejectInvitation(const std::string& inviteeid,const std::string& strinviteName)=0;
	virtual std::string GetLocalMemberID(void)=0;
//    virtual std::string GetServerTime() = 0;
	virtual void OnEnterRoom(unsigned int errcode, const std::string& errText = "")=0;
	virtual void OnExitRoom(void)=0;
	virtual int GetConnectStatus()=0;
	virtual bool IsWebMeeting()=0;
	virtual void OnHiteAvfonDlg(bool bHide = true) = 0;   // 进入会议时隐藏 AVfoneDlg 对话框窗口  Add Hhj 2008/06/28
	virtual void OnMemberOffline(const std::string& memberid,const std::string& membername)=0;
	virtual void OnKickOut(void)=0;
	virtual void OnXSSEResizeWndNotify(int nTMID, int nWndIndex, bool bEnabled)=0;

	virtual string GetTVCMemberRoom()=0;
	virtual void SetTVCMemberRoom(const string& strRoomID)=0;
	
	virtual std::string GetLocalUserName(void)=0;

	virtual void MemberInMeeting( const std::string& memberid ) = 0;
	virtual void MemberOutMeeting( const std::string& memberid ) = 0;

	virtual void MeetingModifyPwd(PMMS_ROOM pRoomItem) = 0;
	virtual void MeetingMember(MMS_ROOM metRoomItem, std::vector<std::string> vecOldMember, const std::vector<std::string> &hide_members, const std::string strGroupId="") = 0;
	virtual void InviteMemberIntoMeeting(const std::string& meetingtype, std::vector<std::string>& vecCurMember) = 0;

	virtual void MemberEnough() = 0;
	virtual void OnRequestExitMeeting(bool bKickOut = false)=0;
	
	virtual void OnConferenceOptions(int nShowUsrPage = -1) = 0;  //会议选项 -1 正常,1为显示选中H323设置页
	virtual void GetOrgContent(const std::string& orgid) = 0;
	virtual void GetDipartmentAndUser(const std::string& did,const std::string& oid) = 0;

    //added by arrow.              begin
    virtual void TreeMemberRequest_HPTM() = 0;
    virtual void TreeMemberRequest_Option() = 0;
    virtual void TreeMemberRequest_WB() = 0;
    virtual void TreeMemberRequest_ScreenShare() = 0;
    virtual void TreeMemberRequest_Discuss() = 0;
    virtual void TreeMemberRequest_MediaShare() = 0;
    virtual void TreeMemberRequest_VideoFmtPresetting() = 0;
    virtual void TreeMemberRequest_InviteMember() = 0;
    virtual void TreeMemberRequest_InviteAllMember() = 0;
    virtual void TreeMemberRequest_QuitMeeting() = 0;
	virtual void TreeMemberRequest_ReSetState(std::string strMemberID = "", UINT nType = 0, int nFlag = 0) = 0;
	virtual void AddSysMsg(CString msg) = 0;
	virtual void SetConfStatus(CString status) = 0;
	virtual void ReSetConfTitle(std::string membername,std::string roomname,int membersum,int onlinesum) = 0;
	virtual void SetFullTemplate(bool bFull, bool bP2Pcmd=false) = 0;
	virtual bool IsFullScreen() = 0;//是否全屏
	virtual void Request_Stop_Open(const std::string& sType) = 0;
	virtual void ExitMeeting() = 0;

	virtual void ShowPopTip(CString strText, CString strUserID, UINT nType = 1, DWORD dwTimeToShow = 1000)=0;
	//以下供会议使用	
	//电子地图
	virtual void AddMapInfoInMeeting(const std::string& strGroupID, const std::string& strMapMD5)=0;
	virtual void UpdateMapInfoInMeeting(const std::string& strGroupID, const std::string& strMapMD5)=0;
	virtual void DeleteMapInfoInMeeting(const std::string& strGroupID)=0;
	virtual void GetMapInfoInMeeting(const std::string& strGroupID)=0;
	virtual void AddBzInfoInMeeting(const std::string& strGroupID,const std::string& strTreeID, const std::string& strBzID)=0;
	virtual void DeleteBzInfoInMeeting(const std::string& strGroupID,const std::string& strTreeID)=0;
	virtual void GetBzInfoInMeeting(const std::string& strGroupID)=0; 
	virtual void GetLoginAddressInMeeting(CString& strAddress,unsigned short& urPort)=0;

	//消息框
	virtual void OnMeetingMemberMsg(string groupId,string sFromId,string sCmdType,string roomid ="") = 0;

    virtual void OnApplyBtnStatus(int nButton, bool bEnable) = 0;
    virtual void OnUpdateMemberBaseItem() = 0;
    virtual void OnShowMainWindow(bool show,bool bFromExe = false) = 0;
	virtual void OnStartEnterMeetingRoom(void)=0;
    virtual void OnResizeMemberTree(int x, int y, int cx, int cy) = 0;
    virtual RECT GetInitially_MemberTreeRect() const = 0;
	virtual RECT GetInitially_TempletRect() const = 0;
    //added by arrow.                end
	//标题
	virtual void SetMeetingTitle(string sTitle,string sFontName,DWORD corlorRef)=0;
	virtual void ShowMeetingTile(bool bShow) = 0;
	//公告
	virtual void SetMeetingAffiche(string sAffiche,string sFontName,DWORD corlorRef)=0;
	virtual void ShowMeetingAffiche(bool bShow)=0;
	virtual void ShowFriendTab() = 0;
	
	virtual CWnd* GetMeetMainWnd() = 0;
	virtual CWnd* GetIMWnd() = 0;

	virtual void Mon_OnData(KCmdUserDataPacket& packet) = 0;	//返回监控数据
	virtual void SendTalkUserData(const std::string strChannelID, KCmdUserDataPacket& packet,bool bBrocast = false)=0;

    // ControlerSysEvent {{

	virtual void AppendVODSvrMenu(CMenu* pMenu,bool bPopupMenu,UINT menu_uploadvod_start) = 0;
	virtual void processVODUpload(int nIndex) = 0;	

    //菜单
    virtual void OnControlMenu() = 0;
    //好友
    virtual void OnControlFriend() = 0;
    //进入会议
    virtual void OnControlConference() = 0;
    //视频调节
    virtual void OnControlVideoAdjust() = 0;
    //音频调节
    virtual void OnControlAudioAdjust() = 0;
    //选项
    virtual void OnControlOptions() = 0;
    //拉远
    //virtual void OnControlZoomOut() = 0;
    //拉进
    //virtual void OnControlZoomIn() = 0;
    //网络设置
    virtual void OnControlNetworkSetting() = 0;
    //退出系统
    virtual void OnControlQuitSystem() = 0;
    //呼叫
    virtual void OnCallPhone() = 0;
    //挂断
    virtual void OnShutPhone() = 0;
    //MIC UP
    virtual void OnMicVolumeUp() = 0;
    //MIC DOWN
    virtual void OnMicVolumeDown() = 0;
	// IM 的音视频调节向导调用(结果的回调)
	virtual void RsetIMAVdioSetting(void) = 0;

	//获取好友名称
	virtual std::string GetUserName(const std::string& strMemberID)=0;
	//获取人员信息
	virtual IMS_CONTACT_ITEM OnGetUserInfo(string userID) = 0;
   // }}
	virtual void ClearMonData(void)=0;
	virtual void GetMonData(bool bRefresh = false) = 0;		//向服务器发送取数据命令
	virtual bool IsGetMonData(void) = 0;
	//virtual MapMonGroupItem GetMonGroupItemMap(void)=0;	
	virtual MapMonGroupItem     GetMonGroupItemMap(const std::string& strDomain)=0;
	virtual MapMonDomainGroupItem GetMonDomainGroupItemMap()=0;
	//virtual MapMonChannelItem GetMonChannelItemMap(void)=0;
	virtual MapMonChannelItem GetMonChannelItemMap(const std::string& strDomain)=0;
	virtual MapMonDomainChannelItem GetMonDomainChannelItemMap()=0;
	virtual GROUPINFO*	GetMonGroupItem(const std::string& groupid)=0;
	virtual GROUPINFO*	GetMonDevItem(const std::string& devid)=0;
	virtual CHANNELINFO* GetMonChannelItem(const std::string& channelid) = 0;
	virtual void SetMonChannelItem(const CHANNELINFO& item) = 0;

	virtual std::string GetMonChannelBindMember(const std::string& devid) = 0;
	virtual void TMCtrlNotify(bool bIsOpen)=0;//模板控制器回调
	virtual void SetMonRight(bool bRight) = 0;
	virtual void MenuFileLogout() = 0;

	//以下为封装AVCONMS所增加接口
	virtual void OnRoomInsert(const std::string& strRoomID)=0;
	virtual void OnRoomDelete(const std::string& strRoomID)=0;
	virtual void OnGroupInsert(const std::string& strRoomID,const std::string& strGroupID)=0;
	virtual void OnMemberInsert(const std::string& strRoomID,const std::string& strMemberID)=0;
	//end
	virtual bool GetPriv(int privilegebit)=0;

	//码流选择ToolBar
	virtual void ShowStreamType(int iType) = 0;
	//end
	virtual void ShowFrameRateControlMode(int nMode) = 0;
	virtual void SetToolBarTopMost(bool bTopMost) = 0;
	virtual void MCUReConnect(bool bLocalData) = 0;
	virtual void MeetingVideoChat(string userid, string nodeID) = 0;
	virtual CString GetAVCONHomePage(void) = 0;

	virtual bool IsInHPMC(void) = 0;
	virtual void ProcessVideoMixSet() = 0;

	//voip有关
	virtual bool IsHandupVoipCall() = 0;

	//状态栏相关
	virtual void OnMeetingLocalAVShow(bool bShow) = 0;
	virtual void OnShowHideIM(const CRect& rc) = 0;
	virtual std::string GetSipCode(void)=0;
	virtual void OnShowMcuSetDlg() = 0;

	virtual CString GetMemberInfoURL() = 0;
	virtual void OnAVSetHide() = 0;
	virtual void OnGetUploadPlan(bool bAutoUpload, MMS_RECPLAN_LST& listRecPlan) = 0;

	//录像磁盘不足报警//*---
	virtual void OnStartRecAlart(bool bStart = true) = 0;
	virtual void OnIDlgConMainNotifyExit(bool bneedrefresh = true) = 0;
	//刷新会议标题
	virtual void RefreshConfTitle(void) = 0;

	//电视墙三期添加
	virtual void ProTVCInitTreeMember(const std::string& strRoomID,const std::string& strRommName,const unsigned short& uRoomType)=0;
	//自动点击切屏按钮
	virtual void AutoClickChangeScreen() = 0;
	//设置会议为焦点
	virtual void SetConFocus() = 0;
	virtual IDeviceClient* GetDeviceClientSession(void)=0;
	virtual IMXEC_AudioMixerCapChan* GetAudioMixerCapChan()=0;
	virtual BOOL IsUseSubCodec()=0;
	virtual bool IsBindRecordServer(void)=0;

	virtual void DeviceLogin(void)=0;
	virtual std::string GetLoginDevID()=0;
	virtual void LoginSelfMeeting(std::string strDomain, std::string strRoomID, std::string strGroupID)=0;

	virtual DOMAININFOLIST_EX& GetLocalDomainList(void)=0;
	virtual MAP_MMS_ROOM& GetAppointDomainRoomList(void)=0;
	virtual void ChangeRoom(const std::string& strRoomID)=0;
	virtual void SendGetLocalDomainList(void)=0;
	virtual void SendGetAppointDomainRoomList(void)=0;
	virtual unsigned long OnGetMixAudioID(void) = 0;
	virtual BOOL IsDeviceAlreadyLogin(void)=0;
	virtual void OnSelectScreen(UINT nType, int nFlag)=0;
};

#define SETISBEGGINNER_CHAT			1
#define SETISBEGGINNER_WB			2
#define SETISBEGGINNER_BCDESKTOP	3
#define SETISBEGGINNER_BCVEDIO		4
#define SETISBEGGINNER_FILESEND     5
class IHPMeeting
{
public:
	//电子地图相关
	virtual void OnMapInfo(const std::string& strGroupID, const std::string& strMapMD5)=0;
	virtual void OnBzIDInfo(const std::string& strGroupID, MAP_BZ_LST listMAPBZ)=0;
	virtual void OnAECTest()=0;
	virtual void OnStartHPMC()=0;//启动监控客户端
	virtual BOOL IsTempletePictureInPicture(int nTempleteID,  CRect *pRectBig, CRect *pRectSmall) = 0;
	virtual void OnIMOptionsChange() = 0;			
	virtual void AllMemberExitMeeting(void) = 0;	
	virtual void RunDlgToolBarRespond(UINT nFlags, bool bP2Pcmd=false) = 0;			// 运行 DlgToolBar 的响应
	virtual void SetSyncBtnRect(CRect rc) = 0;
	virtual void UpdateMeetingLocalUserInfo( void ) = 0; 		
	virtual void SetAVConMainCWnd(void *pAVConMain) = 0;
	virtual void ReleaseConnections(void)=0;
	virtual void EnterMeetingMode(void)=0;
	virtual void ExitMeeting(bool bneedrefresh = true)=0;
	virtual void ResizeDlgMeetingList(CWnd* pWnd,int x,int y,int cx,int cy,bool bShow)=0;
	virtual void ResizeDlgMonitorMeetingList(CWnd* pWnd,int x,int y,int cx,int cy,bool bShow)=0;
	virtual void ResizeDlgMember(CWnd* pWnd,int x,int y,int cx,int cy,bool bShow)=0;
	virtual void InviteUserIntoMeeting(const std::string& userid, bool bPresider = false, bool bStartRecord = false)=0;	
	virtual void RequestIntoMeeting(const std::string& userid,const std::string nodeid,const std::string& username)=0;
	virtual void AcceptInvitation(std::string strDomain,std::string roomid,std::string password,std::string strGroupID="", bool bPresider = false)=0;
	virtual void RejectInvitation(const std::string& strDomain,const std::string& strRoomID,const std::string& strInviterNodeID) = 0;
	virtual void ResizeMainHPTM(CWnd* pWnd,int x,int y,int cx,int cy,bool bShow) = 0;
	virtual void ResizeDlgLocalVideoMgr(CWnd* pWnd,int x,int y,int cx,int cy,bool bShow)=0;
	virtual bool GetEnterRoom(void)=0;
	virtual bool GetCurRoomIDAndPW(std::string& roomid,std::string& password) = 0;
	virtual void SetTMType(int nType, int nSubType, unsigned long ulScreenIndex, int nWinCount, bool bSync = true)=0;
	virtual bool IsMyHost()=0;
	virtual bool IsMyAuditor()=0; //旁听者
	virtual std::string GetRoomPassword(const std::string& roomid) = 0;
	virtual std::string GetCurRoomID() = 0;
	virtual std::string GetCurRoomName() = 0;
	virtual bool IsRoomGroupSynInfoEnd()=0; 
	virtual void CheckUploadFile() = 0;  //检查是否有需要上传的文件
	virtual void GetUploadPlan() = 0;   //获取录像计划
	virtual void GetStorageServer() = 0; //获取录像服务器地址
	virtual void OnRcvP2PData(const std::string&strNodeID,KCmdPacket& packet) = 0;

	virtual void SomebodyOnLine(const std::string& memberid) = 0;

	//注意！！！以下接口目前专为CMeeting服务！为客户端接口服务！！！！
	virtual void InviteMemberIntoMeeting(const std::string& roomid) = 0;
	virtual void InviteAllOfflineMemIntoMeeting(const std::string& roomid ,bool bFilter) = 0;
	virtual void CloseSubWindow() = 0;  	
	virtual void ClearTempRoomData() = 0; 
	virtual void ClearTempGroupMemberData() = 0; 
	virtual void SetMyExit(bool bMyExit) = 0;
	virtual void CloseMsgWnd() = 0;
	virtual void OnConferenceOptions() = 0;  //会议选项
	virtual void OnH323Setting() = 0;
	virtual void ShowMeetingMsg(CString strTitle,CString strContent,int nShowType = 0/*0有按钮，1无按钮*/,CString strCmdType = _T(""),ULONG ulSessionID = -1,int nTimeType = -1,int nCloseTime = -1)=0;
	//virtual void AddUserForInviting(const std::string& userid) = 0;
	//注意！！！end

	virtual std::string GetLocalMemberID(void)=0;

	virtual bool CreateHPMediaChannel(CString strMediaFile, int nScreenIndex = -1, int nFreeWndIndex = -1)=0;
	virtual bool IsHPMediaChannelExist(void)=0;
	virtual void DestroyLocalHPMediaChannel(bool bLoot = false)=0;

	virtual bool CreateHPXSSEChannel(void)=0;
	virtual bool IsHPXSSEChannelExist(void)=0;
	virtual bool IsMemberHPXSSEChannelExist(const std::string strMemberID, int& nTMID, int& nWndIndex)=0;
	virtual bool IsHPXSSEChannelLocalVisible(const std::string strMemberID, int pTMID, int pWndIndex)=0;	// 判断屏幕共享窗口是否显示 Add  Han Huajie    2008-3-10
	virtual void SetLocalHPXSSEChannelInvisible() = 0;
	virtual bool IsLocalMediaChannelExist(int& nTMID, int& nSubWndID) = 0;
	virtual bool IsLocalMPType(void)=0;
	virtual void DestroyHPXSSEChannel(void)=0;
	virtual bool GetXSSERect(int nTMID, int nWndIndex, RECT& rcRgn)=0;

	virtual bool CreateHPChat(int nScreenIndex = -1, int nFreeWndIndex = -1)=0;
	virtual bool CreateHPMap(int nScreenIndex = -1, int nFreeWndIndex = -1)=0;
	virtual void DestroyHPChat(bool bLoot = false)=0;
	virtual bool IsHPChatExist(void)=0;
	virtual void DestroyHPMap(bool bLoot = false)=0;
	//added by yangzubin 09.22


	virtual bool CreateHPFileSend(void)=0;
	virtual void DestroyHPFileSend(bool bLoot = false)=0;
	virtual bool IsHPFileSendExist(void)=0;
	virtual void ShowHPFileSend(bool bShow)=0;
	//added by luoning  09.03.29

	virtual bool ShowWBWnd(int nScreenIndex = -1, int nFreeWndIndex = -1) = 0;
	virtual void ProcessMap(int nAcction) = 0;
	virtual bool IsWBExist() = 0;
	virtual bool IsMapExist() = 0; 
	virtual void CloseWBWnd(bool bLoot = false) = 0;
	//virtual void CloseWBWndForRemote() = 0;
	virtual void SendUserData(KCmdUserDataPacket& packet, bool bBroadCast = false) = 0;
	virtual void OnTalkUserData(KCmdUserDataPacket& packet) =0;

    //added by arrow  {{
    virtual void VideoFmtPresetting() = 0;  //预设置 Dialog
    virtual void ApplyVideoFmtPresettting(bool bApplyMySet) = 0;    //应用预设置
    virtual void ApplyWndProp(bool bApply) = 0;    //应用窗口属性
    virtual void ShowHPTMDialog() = 0;	//会议模板设置

	virtual void ShowAddMeetingDlg() = 0;
	virtual	void OnMeetinglistEnterRoom() = 0;
	virtual	void OnMeetinglistDeleteRoom() = 0;
	virtual	void OnMeetinglistRoomSet() = 0;

	virtual bool StartEnterMeetingRoom(void)=0;
    virtual void OnShowMainConferenceWnd(bool bShow) = 0;
	virtual void ShowVoteDialog() = 0;
    //}}
	virtual void OnTreeRootMenu(long x,long y) = 0;
	virtual void OnShowVideoModeMenu() = 0;
	virtual void ShowHandDialog() = 0;
	virtual void StopHand() = 0;
	virtual void OnCmdbarKickOut() = 0;
	virtual bool IsMeetingSelected() = 0;
	virtual bool IsInMeeting() = 0;

	virtual void SetIsBeginner(int nwhat, bool isbeginner) = 0;
	virtual HWND GetMeetingListWnd() = 0;
	
	//0x01:拒绝所有邀请
	//0x02:弹出邀请提示信息
	//0x04:自动进入
	//0x10:如果在会议中，将自动退出会议进入被邀请的会议

	//同意/拒绝发言请求
	virtual void AcceptRequestSpeaker(string roomid,string groupId,string sToId,bool bAnswer = true)=0;
	virtual void RejectRequestSpeaker(string roomid,string groupId,string sToId)=0;

	//同意/拒绝提问请求
	virtual void AcceptRequestQuiz(string roomid,string groupId,string sToId)=0;
	virtual void RejectRequestQuiz(string roomid,string groupId,string sToId)=0;

	//只判断与会者是否为发言者，如果当前是主持人，返回FALSE
	virtual bool  IsMySpeaker() =0;
	//
	virtual unsigned long GetActiveTM() = 0;
	virtual RECT GetPrimaryScreenRect() const  = 0;
    
	virtual long GetChannelPacketLen() = 0;
	virtual long GetAVRPlayerDataLen() = 0;

	virtual bool IsShowMeetingTitle() =0;
	virtual bool IsShowMeetingAffiche() =0;

	virtual void UpDateMeetingMember(MMS_ROOM metRoomItem,std::vector<std::string> vecNewMembers ,std::vector<std::string> vecOldMembers,const std::string& strGroupId="") = 0;
    //权限控制
    virtual void SetPrivilegeCode(unsigned long nCode) = 0;
    virtual unsigned long GetPrivilegeCode() = 0;
    virtual bool IsEnabledPrivilege( unsigned long nCode ) = 0;
	virtual void MonUserData(KCmdUserDataPacket& packet)=0;	//发送监控数据

    virtual void RequestSpeak(bool cancel_request = false) = 0;   //申请发言
    virtual void RequestQuestion(bool cancel_request = false) = 0;  //申请提问
	virtual void TempCtrl()=0;				//模板控制器
	virtual void OnScreenControlOnKeyDown(MSG* pMsg) = 0;
	virtual void OnScreenControlMouseWheel(MSG* pMsg) = 0;
	virtual void OnSetAllWindowUnSelect() = 0; 
	virtual void OpenScreenShare(string strMemberID,int nScreenIndex = -1, int nFreeWndIndex = -1) = 0; 
    virtual bool IsWindowVisible(const std::string &control_id) = 0;
    virtual void OnShowQuestionList(bool show, const CRect &rc) = 0;
	virtual void SetConferencePlayerNotify(IAVSPlayerNotify *pIAVSPlayerNotify)= 0;
	virtual bool GetConferenceRecordFileName(CString& strAVCFile)= 0;

	virtual void CreateControlConferenceMeeting() =0;
	virtual void DestroyControlConferenceMeeting() =0;

	virtual HWND GetChatDlgHwnd() = 0;
	virtual int GetMeetingActiveTMid() = 0;
	virtual BOOL IsScreenRectVisible(int nIndex) = 0;
	virtual void SetIMAVdioSetting(std::string memberid, BOOL bIMAVSET = TRUE, unsigned long ulChannelID = 0) = 0;		// IM 的音视频调节向导调用 bIMAVSET是否IM中吊起

	virtual std::string GetMemberName(const std::string& memberid)=0;	//得到会议成员名称

	//  [11/21/2008 n.x.s] begin
	//监控资源回调
	virtual void OnGetGroupInfoEnd(const std::string& strDomain)=0;
	virtual void OnChannelInfoEnd(const std::string& strDomain)=0;
	virtual void OnChannelInfo(const CHANNELINFO& item, bool bTreeSort = true)=0;
	virtual void OnDevChannelChange(const std::string& strDevID,const std::string& strChannelID,unsigned long ulVideoID,unsigned long ulAudioID)=0;
	virtual void OnUpdateDevName(const std::string& strDevID,const std::string& strName, unsigned long ulGroupID)=0;
	virtual void OnUpdateChannelName(const std::string& strChannelID,const std::string& strName, unsigned long ulGroupID)=0;

	//人员状态变化通知
	virtual void OnOrgUserStatus(const std::string& strUserID,const std::string& strStatus,bool bFirst,const std::string& strNodeID,const std::string& strNatAddr,const std::string& strLocalAddr,unsigned short usLocalPort)=0;

	//电视墙三期
	virtual void ProStartLoginMeeting_TVC()=0;


	virtual void ProcessMonAlertData(const std::string strChannelID,unsigned short usType,unsigned short usSubtype)=0;

	virtual void SetClartyTilte(BOOL bClartyTilte) = 0;
	virtual void ModificationMeetingLocalUserInfo(void) = 0;
	virtual void ReSetAllAVDelayTimeAndPause(void) = 0;
	virtual void ScreenControlSet(int nScreenControl) = 0;

	virtual void SetNoteInform(const std::string& m_strNoteSet) = 0;
	virtual void SetDefContent(BOOL bDefaultContent) =  0;
	virtual void SetRadioNoteMode(int nRadioNoteMode) = 0;
	virtual std::string GetNoteInform() = 0;
	virtual void ShowMessageBox(CString strText,CString strTextCaption) = 0;
	virtual void OptiongCallingSet() = 0;
	virtual bool IsConferCalling() = 0;
	virtual void OnChangeTitleScreen() = 0;
	virtual void OnChangeAfficheScreen() = 0;
	virtual void SetControlerDlgPos() = 0;
	virtual void SetTitlePos(int pos) = 0;
	virtual void SetAffichePos(int pos) = 0;
	virtual void SetToolDlgPos(int pos) = 0;
	virtual void SetNowScreenPos(int pos) = 0;
	virtual int  GetToolDlgPos() = 0;
	virtual int  GetNowScreenPos() = 0;	
	//end

	// for debug[12/28/2009 nxs]
	//以下为封装AVCONMS所增加接口
	///* 在会议列表中创建会议室 */
	//virtual void InsertRoom(const std::string& strRoomName, const std::string& strPassword, const std::string& strDesc)=0;
	///* 在会议列表中删除会议室 */
	//virtual void DeleteRoom(const std::string& strRoomID)=0;
	///* 在会议列表中添加会议组 */
	//virtual void InsertGroup(const std::string& strRoomID, const std::string& strParentGroupID, const std::string& strGroupName)=0;
	///* 在会议列表中添加会议成员 */
	//virtual void InsertMember(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID,
	//	const std::string& strMemberName, int mtMemberType)=0;
	/* 登录会议室 */
	virtual void Login(const std::string& strRoomID, const std::string& strPassword, const std::string& strMemberID,
		const std::string& strMemberName, const std::string& strInviteeGroupID, int mtMemberType)=0;
	/* 设置m_strPreRoomID */
	virtual void SetPreRoomID(const std::string& strRoomID)=0;
	//end

	//码流选择ToolBar
	virtual void SetStreamType(int iType) = 0;	//iType为-1时，从配置文件读取来设置
	//end
	virtual void SetFrameRateControlMode(int nMode) = 0;	//nMode为-1时，从配置文件读取来设置
	virtual void SetMCUItem(MMS_MCU* pMCU) = 0;
	virtual void ShowPTZControl() = 0;	//显示/隐藏云台控制台
	//End

	virtual void SetAVCONType(int type) = 0;
	virtual void OnRequestIntoMeetingData(KCmdUserDataPacket& packet) = 0;

	virtual void OnMixVideoDateChanged() = 0;
	virtual void OnIMLoginDone(BOOL bDone) = 0;			// 通知会议IM界面已经登录完成，可以进行进入会议操作
	
	//状态栏相关
	virtual bool IsConferencePlayer() = 0;
	virtual void ShowMeetingLocalAV(bool bShow) = 0;
	virtual void ProcessLocalVolumeCapture(bool bCapture) = 0;
	virtual void OnControlShowHideIM() = 0;
	virtual void ClearRecPlanData() = 0;	//删除本地录像计划数据
	virtual void RecordRemoteChanged() = 0;  //录像格式改变
	virtual CString GetStorageServerIP() = 0;
	virtual CString GetStorageServerPort() = 0;

	//*****-
	virtual void TempletControlerDlg_CheckWnd_ShowWindow(int nCmdShow,BOOL bIsDoModle)=0;
	virtual int  TempletControlerDlg_CheckWnd_AddCWnd(CWnd * pWnd)=0;
	
	//*----
	virtual std::list<PMMS_ROOM> MyGetRoomList()=0; 
	virtual bool MyEnterRoom(PMMS_ROOM pSetItem, string strTempGrpId = "", bool bPresider = false)=0;
    virtual list<PMMS_ROOM_TEMP> GetTmpRooms()=0;
	virtual void MyCopyTempRoomToAdvanceRoom(PMMS_ROOM pNewItem,const MMS_ROOM_TEMP& item)=0;
	virtual bool GetCurSubGroupStatus()=0;
	virtual int GetScreenCount() = 0;		//得到屏幕的数量
	virtual void SetLocalMicStatus(bool bOpen)=0;		//会议中记录当前MIC状态

	virtual void ShowWBTools(void)=0;	
	//added by luoning 2011-4-1
	virtual void NotifyToCheckTouchScreen(bool bCheck) = 0;
	//end

	virtual void ShowCloseAudioBar() = 0;
	virtual bool IsBindRecordServer(void)=0;
	////////////////////IDeviceClientNotify//////////////////////////////////////
	/*
	登录失败回调
	nErrCode:错误代码
	strError:错误内容
	*/
	virtual void OnDeviceClientLogin(int nErrCode, CString strError)=0;
	/*
	设备上线成功回调
	在此进行通道发布
	allow_count:允许发布的通道个数
	*/
	virtual void OnDeviceOnline(int allow_count)=0;
	/*
	设备下线通知
	*/
	virtual void OnDeviceOffline(void)=0;
	/*
	通道上线成功回调
	*/
	virtual void OnChannelOnline(void)=0;
	/*
	通道下线通知
	*/
	virtual void OnChannelOffline(void)=0;

	virtual void ConferenceDeviceLogout(void)=0;

	//获取当前会议类型
	virtual MMS_ROOM_TYPE GetCurRoomType(void)=0;

	//获取当前会议域
	virtual std::string GetCurRoomDomain(void)=0;	

	//初始化域列表
	virtual void InitMeetMonDomain(void)=0;
	virtual void InitMeetMonRoomList(const std::string& strDomain)=0;

	virtual void OnP2P(const std::string& strDevID, KCmdUserDataPacket& packet)=0;
	virtual void OnYTControl(KCmdPacket& rCmdPacket)=0;
	virtual void ReSetFullScreenToolBarStatus(UINT nType, int nFlag)=0;
	//远程录像
	virtual void SetRemoteRecordPos(void) = 0;
	virtual void EnablePreview(int nPreview)=0;

	//创建本地通道
	virtual void InitLocalDev()=0;

	//IM通道设置
	virtual void IMChannelSet()=0;
	virtual int GetCurrChannelCount()=0;
	virtual void UpdateChannelName()=0;

	//是否有录播权限
	virtual BOOL IsHavePrivRec(void) = 0;
};

AFX_EXT_CLASS IHPMeeting* CreateHPMeeting(IHPMeetingNotify& rNotify,CWnd* pWnd);

#endif
