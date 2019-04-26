#ifndef _HPMEETING_H
#define _HPMEETING_H

#define TYPE_ZR				0				    //������ 1:Ϊ������

#define NEW_RECORD_VERSION	"AVCON6.0.1"		//�µ�¼���汾����   

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

// Ȩ��λ����
typedef enum PRIVILEGE_BIT
{
	PRIVILEGE_BIT_SIP		= 1,	//SIP����
	PRIVILEGE_BIT_SMS		= 2,	//SMS����
	PRIVILEGE_BIT_MMS		= 3,	//�������(WEB)
	PRIVILEGE_BIT_MON		= 4,	//��ص㲥(WEB)
	PRIVILEGE_BIT_TEM		= 5,	//ģ������
	PRIVILEGE_BIT_FAX		= 6,	//�շ�����
	PRIVILEGE_BIT_MSG		= 7,	//�����㲥��Ϣ
	PRIVILEGE_BIT_BAD		= 8,	//��������
	PRIVILEGE_BIT_MAP		= 9,	//��ͼ�༭
	PRIVILEGE_BIT_MAT		= 10,	//�������
	PRIVILEGE_BIT_LOS		= 11,	//����Ȩ��
	PRIVILEGE_BIT_INFO		= 14,	//�޸ĸ�������
	PRIVILEGE_BIT_ORG		= 17,	//��ʾ����Ȩ��;
	PRIVILEGE_BIT_VOD		= 21,	//�ϴ�¼��㲥Ȩ��;
	PRIVILEGE_BIT_MEET_MON	= 23,	//������Ȩ��
	PRIVILEGE_BIT_REC		= 24,	//Զ��¼��
};


enum {
	CON_AFFICHE = 0,				//���鹫��
	CON_TITLE,						//�������
	CON_CALLING,					//�������
	CON_MASS,						//������
	LOCK_CON,						//��������
	SOUND_BESTIR,					//��������
	CON_SYNC,						//ͬ��ģʽ
	CON_DESYNC,						//����ģʽ
	CON_INDEPENDENCE,				//����ģʽ
	CON_LABIAL_SOUND,				//����ͬ��
	LAND_TIMER_TAXIS,				//��¼ʱ������
	NAME_TAXIS,						//��������
	TYPE_TAXIS,						//��Ա��������
	SET_CON_AFFICHE,				//���ù���
	SET_CON_TITLE,					//���ñ���
	SET_CON_HAND,					//���û������
	ALLOW_DISCUSS,					//��������
	SCREEN_CONTROL,					//����������Ļ����
	NOTE_INFORM,					//����֪ͨ
	CON_IN_PHASE,					//����ͬ��
	IN_PHASE_CHOOSE_1,				//ѡ 1 ��
	IN_PHASE_CHOOSE_2,				//ѡ 2 ��
	IN_PHASE_CHOOSE_3,				//ѡ 3 ��
	IN_PHASE_CHOOSE_4,				//ѡ 4 ��
	SCREEN_CHANGE,					//����
	CONF_KINESCOPE,					//����¼��
	CONF_RECORD_REMOTE,				//Զ��¼��
	CONF_RECORD_REMOTE_SET,			//Զ��¼���ʽ
	CONF_AVDIO,						//��Ƶ
	CONF_DOC,						//�ĵ�
	FULL_SCREEN,					//ȫ��
	SMALL_SCRIP,					//Сֽ��
	SHOWVIDEOTITLE,					//��Ƶ����
	CLOSE_AUDIO_MANAGE,				// ���� [3/23/2011 %x.t%]
	CONF_MEDIA,
	CONF_SIPCALL,					//SIP����
};

//�ص�
enum {
	REV_ALLOW_KINESCOPY = 0,		//�������¼��
	REV_ALLOW_YT_CONTROL,			//���������̨
	REV_ALLOW_SPEAK,				//�������뷢��
	REV_ALLOW_WB,					//������Ӱװ�
	REV_ALLOW_CONTROL,				//��������Ա����
	REC_CON_LABIAL_SOUND,			//����ͬ��
	REV_ALLOW_FLOOT,				//����ı両������
	REV_CON_AFFICHE,				//���鹫��
	REV_CON_TITLE,					//�������
	REV_CON_HAND,					//�������
	REV_CON_CALLING,				//�������
	REV_CON_MASS,					//������
	REV_LOCK_CON,					//��������
	REV_SOUND_BESTIR,				//��������
	REV_NAME_TAXIS,					//��������
	REV_LAND_TIMER_TAXIS,			//��¼ʱ������
	REV_TYPE_TAXIS,					//��Ա��������
	REV_CON_SYNC,					//ͬ��ģʽ
	REV_CON_DESYNC,					//����ģʽ
	REV_CON_INDEPENDENCE,			//����ģʽ
	REV_IN_PHASE_CHOOSE_1,			//ѡ 1 ��
	REV_IN_PHASE_CHOOSE_2,			//ѡ 2 ��
	REV_IN_PHASE_CHOOSE_3,			//ѡ 3 ��
	REV_IN_PHASE_CHOOSE_4,			//ѡ 4 ��
	REV_ALLOW_DISCUSS,				//��������
	REV_SCREEN_CONTROL,				//����������Ļ����
	REV_SCREEN_SHARE_RIGHT,			//��Ļ����Ȩ��
	REV_CONF_KINESCOPE,				//����¼��
	REV_CONF_RECORD_REMOTE,			//Զ��¼��
	REV_CONF_RECORD_REMOTE_SET,		//Զ��¼���ʽ
	REV_CONF_AVDIO,					//��Ƶ
	REV_CONF_DOC,					//�ĵ���ť״̬
	REV_CONF_DOC_RIGHT,				//�ĵ�Ȩ��
	REV_FULL_SCREEN,				//ȫ��
	REV_SMALL_SCRIP,				//Сֽ��
	REV_YT_CTRL,					//��̨����
	REV_H323_CTRL,					//H323����
	REV_SHOW_AVINFO,				//��Ƶ��Ϣ
	REV_CLOSE_AUDIO_MANAGE,			// ���� [3/24/2011 %x.t%]
};

enum{
	STREAM_TYPE_AUTO = 0,
	STREAM_TYPE_HIGH,
	STREAM_TYPE_MID,
	STREAM_TYPE_LOW,
};

#define TERM_TYPE_PC		"PC"		//PC�汾
#define TERM_TYPE_TERMINAL	"Terminal"	//�ն˰汾
#define TERM_TYPE_RECORD    "record"	//Զ��¼�����
#define TERM_TYPE_H323MP	"h323mp"	//H323MP

#define SELFGROUP_HOST_ENTER     "host_enter" 
#define SELFGROUP_HOST_STOP		 "host_stop"

class IHPMeetingNotify
{
public:
	virtual void SetCurRoomStatus(_ROOM_STATUS nStatus)=0; //0:�ޣ�1���ڻ��飬2����AView;
	virtual _ROOM_STATUS GetCurRoomStatus(void)=0; //0:�ޣ�1���ڻ��飬2����AView;

	virtual void OnStartHPMCNotify(const std::string& strDomain,
		const std::string& strRoomID,
		const std::string& strPassword,
		const std::string& strMemberID,
		const std::string& strMemberName,
		const std::string& strInviteeGroupID,
		bool bPresider=false,
		const std::string& strUpdateTime="",
		const std::string& strIPType="")=0;//������ؿͻ��˻ص�
	virtual void SetHPMCbtnStatus(bool bBtnStatus)=0;//��ؿͻ��˰�ť�ص�
	virtual void OnRejectRequestMsg(CString strMsg)=0;
	virtual void OnInviteIntoMeeting(const std::string& strDomain,const std::string& strRoomID,const std::string& strRoomName,const std::string& strInviterNodeID,const std::string& strInviterID, const std::string& strPassword, const std::string& strInviterName,int nInviteAdmOrderID,int nBeInviteAdmOrderID)=0;
	virtual void OnRejectInvitation(const std::string& inviteeid,const std::string& strinviteName)=0;
	virtual std::string GetLocalMemberID(void)=0;
//    virtual std::string GetServerTime() = 0;
	virtual void OnEnterRoom(unsigned int errcode, const std::string& errText = "")=0;
	virtual void OnExitRoom(void)=0;
	virtual int GetConnectStatus()=0;
	virtual bool IsWebMeeting()=0;
	virtual void OnHiteAvfonDlg(bool bHide = true) = 0;   // �������ʱ���� AVfoneDlg �Ի��򴰿�  Add Hhj 2008/06/28
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
	
	virtual void OnConferenceOptions(int nShowUsrPage = -1) = 0;  //����ѡ�� -1 ����,1Ϊ��ʾѡ��H323����ҳ
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
	virtual bool IsFullScreen() = 0;//�Ƿ�ȫ��
	virtual void Request_Stop_Open(const std::string& sType) = 0;
	virtual void ExitMeeting() = 0;

	virtual void ShowPopTip(CString strText, CString strUserID, UINT nType = 1, DWORD dwTimeToShow = 1000)=0;
	//���¹�����ʹ��	
	//���ӵ�ͼ
	virtual void AddMapInfoInMeeting(const std::string& strGroupID, const std::string& strMapMD5)=0;
	virtual void UpdateMapInfoInMeeting(const std::string& strGroupID, const std::string& strMapMD5)=0;
	virtual void DeleteMapInfoInMeeting(const std::string& strGroupID)=0;
	virtual void GetMapInfoInMeeting(const std::string& strGroupID)=0;
	virtual void AddBzInfoInMeeting(const std::string& strGroupID,const std::string& strTreeID, const std::string& strBzID)=0;
	virtual void DeleteBzInfoInMeeting(const std::string& strGroupID,const std::string& strTreeID)=0;
	virtual void GetBzInfoInMeeting(const std::string& strGroupID)=0; 
	virtual void GetLoginAddressInMeeting(CString& strAddress,unsigned short& urPort)=0;

	//��Ϣ��
	virtual void OnMeetingMemberMsg(string groupId,string sFromId,string sCmdType,string roomid ="") = 0;

    virtual void OnApplyBtnStatus(int nButton, bool bEnable) = 0;
    virtual void OnUpdateMemberBaseItem() = 0;
    virtual void OnShowMainWindow(bool show,bool bFromExe = false) = 0;
	virtual void OnStartEnterMeetingRoom(void)=0;
    virtual void OnResizeMemberTree(int x, int y, int cx, int cy) = 0;
    virtual RECT GetInitially_MemberTreeRect() const = 0;
	virtual RECT GetInitially_TempletRect() const = 0;
    //added by arrow.                end
	//����
	virtual void SetMeetingTitle(string sTitle,string sFontName,DWORD corlorRef)=0;
	virtual void ShowMeetingTile(bool bShow) = 0;
	//����
	virtual void SetMeetingAffiche(string sAffiche,string sFontName,DWORD corlorRef)=0;
	virtual void ShowMeetingAffiche(bool bShow)=0;
	virtual void ShowFriendTab() = 0;
	
	virtual CWnd* GetMeetMainWnd() = 0;
	virtual CWnd* GetIMWnd() = 0;

	virtual void Mon_OnData(KCmdUserDataPacket& packet) = 0;	//���ؼ������
	virtual void SendTalkUserData(const std::string strChannelID, KCmdUserDataPacket& packet,bool bBrocast = false)=0;

    // ControlerSysEvent {{

	virtual void AppendVODSvrMenu(CMenu* pMenu,bool bPopupMenu,UINT menu_uploadvod_start) = 0;
	virtual void processVODUpload(int nIndex) = 0;	

    //�˵�
    virtual void OnControlMenu() = 0;
    //����
    virtual void OnControlFriend() = 0;
    //�������
    virtual void OnControlConference() = 0;
    //��Ƶ����
    virtual void OnControlVideoAdjust() = 0;
    //��Ƶ����
    virtual void OnControlAudioAdjust() = 0;
    //ѡ��
    virtual void OnControlOptions() = 0;
    //��Զ
    //virtual void OnControlZoomOut() = 0;
    //����
    //virtual void OnControlZoomIn() = 0;
    //��������
    virtual void OnControlNetworkSetting() = 0;
    //�˳�ϵͳ
    virtual void OnControlQuitSystem() = 0;
    //����
    virtual void OnCallPhone() = 0;
    //�Ҷ�
    virtual void OnShutPhone() = 0;
    //MIC UP
    virtual void OnMicVolumeUp() = 0;
    //MIC DOWN
    virtual void OnMicVolumeDown() = 0;
	// IM ������Ƶ�����򵼵���(����Ļص�)
	virtual void RsetIMAVdioSetting(void) = 0;

	//��ȡ��������
	virtual std::string GetUserName(const std::string& strMemberID)=0;
	//��ȡ��Ա��Ϣ
	virtual IMS_CONTACT_ITEM OnGetUserInfo(string userID) = 0;
   // }}
	virtual void ClearMonData(void)=0;
	virtual void GetMonData(bool bRefresh = false) = 0;		//�����������ȡ��������
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
	virtual void TMCtrlNotify(bool bIsOpen)=0;//ģ��������ص�
	virtual void SetMonRight(bool bRight) = 0;
	virtual void MenuFileLogout() = 0;

	//����Ϊ��װAVCONMS�����ӽӿ�
	virtual void OnRoomInsert(const std::string& strRoomID)=0;
	virtual void OnRoomDelete(const std::string& strRoomID)=0;
	virtual void OnGroupInsert(const std::string& strRoomID,const std::string& strGroupID)=0;
	virtual void OnMemberInsert(const std::string& strRoomID,const std::string& strMemberID)=0;
	//end
	virtual bool GetPriv(int privilegebit)=0;

	//����ѡ��ToolBar
	virtual void ShowStreamType(int iType) = 0;
	//end
	virtual void ShowFrameRateControlMode(int nMode) = 0;
	virtual void SetToolBarTopMost(bool bTopMost) = 0;
	virtual void MCUReConnect(bool bLocalData) = 0;
	virtual void MeetingVideoChat(string userid, string nodeID) = 0;
	virtual CString GetAVCONHomePage(void) = 0;

	virtual bool IsInHPMC(void) = 0;
	virtual void ProcessVideoMixSet() = 0;

	//voip�й�
	virtual bool IsHandupVoipCall() = 0;

	//״̬�����
	virtual void OnMeetingLocalAVShow(bool bShow) = 0;
	virtual void OnShowHideIM(const CRect& rc) = 0;
	virtual std::string GetSipCode(void)=0;
	virtual void OnShowMcuSetDlg() = 0;

	virtual CString GetMemberInfoURL() = 0;
	virtual void OnAVSetHide() = 0;
	virtual void OnGetUploadPlan(bool bAutoUpload, MMS_RECPLAN_LST& listRecPlan) = 0;

	//¼����̲��㱨��//*---
	virtual void OnStartRecAlart(bool bStart = true) = 0;
	virtual void OnIDlgConMainNotifyExit(bool bneedrefresh = true) = 0;
	//ˢ�»������
	virtual void RefreshConfTitle(void) = 0;

	//����ǽ�������
	virtual void ProTVCInitTreeMember(const std::string& strRoomID,const std::string& strRommName,const unsigned short& uRoomType)=0;
	//�Զ����������ť
	virtual void AutoClickChangeScreen() = 0;
	//���û���Ϊ����
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
	//���ӵ�ͼ���
	virtual void OnMapInfo(const std::string& strGroupID, const std::string& strMapMD5)=0;
	virtual void OnBzIDInfo(const std::string& strGroupID, MAP_BZ_LST listMAPBZ)=0;
	virtual void OnAECTest()=0;
	virtual void OnStartHPMC()=0;//������ؿͻ���
	virtual BOOL IsTempletePictureInPicture(int nTempleteID,  CRect *pRectBig, CRect *pRectSmall) = 0;
	virtual void OnIMOptionsChange() = 0;			
	virtual void AllMemberExitMeeting(void) = 0;	
	virtual void RunDlgToolBarRespond(UINT nFlags, bool bP2Pcmd=false) = 0;			// ���� DlgToolBar ����Ӧ
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
	virtual bool IsMyAuditor()=0; //������
	virtual std::string GetRoomPassword(const std::string& roomid) = 0;
	virtual std::string GetCurRoomID() = 0;
	virtual std::string GetCurRoomName() = 0;
	virtual bool IsRoomGroupSynInfoEnd()=0; 
	virtual void CheckUploadFile() = 0;  //����Ƿ�����Ҫ�ϴ����ļ�
	virtual void GetUploadPlan() = 0;   //��ȡ¼��ƻ�
	virtual void GetStorageServer() = 0; //��ȡ¼���������ַ
	virtual void OnRcvP2PData(const std::string&strNodeID,KCmdPacket& packet) = 0;

	virtual void SomebodyOnLine(const std::string& memberid) = 0;

	//ע�⣡�������½ӿ�ĿǰרΪCMeeting����Ϊ�ͻ��˽ӿڷ��񣡣�����
	virtual void InviteMemberIntoMeeting(const std::string& roomid) = 0;
	virtual void InviteAllOfflineMemIntoMeeting(const std::string& roomid ,bool bFilter) = 0;
	virtual void CloseSubWindow() = 0;  	
	virtual void ClearTempRoomData() = 0; 
	virtual void ClearTempGroupMemberData() = 0; 
	virtual void SetMyExit(bool bMyExit) = 0;
	virtual void CloseMsgWnd() = 0;
	virtual void OnConferenceOptions() = 0;  //����ѡ��
	virtual void OnH323Setting() = 0;
	virtual void ShowMeetingMsg(CString strTitle,CString strContent,int nShowType = 0/*0�а�ť��1�ް�ť*/,CString strCmdType = _T(""),ULONG ulSessionID = -1,int nTimeType = -1,int nCloseTime = -1)=0;
	//virtual void AddUserForInviting(const std::string& userid) = 0;
	//ע�⣡����end

	virtual std::string GetLocalMemberID(void)=0;

	virtual bool CreateHPMediaChannel(CString strMediaFile, int nScreenIndex = -1, int nFreeWndIndex = -1)=0;
	virtual bool IsHPMediaChannelExist(void)=0;
	virtual void DestroyLocalHPMediaChannel(bool bLoot = false)=0;

	virtual bool CreateHPXSSEChannel(void)=0;
	virtual bool IsHPXSSEChannelExist(void)=0;
	virtual bool IsMemberHPXSSEChannelExist(const std::string strMemberID, int& nTMID, int& nWndIndex)=0;
	virtual bool IsHPXSSEChannelLocalVisible(const std::string strMemberID, int pTMID, int pWndIndex)=0;	// �ж���Ļ�������Ƿ���ʾ Add  Han Huajie    2008-3-10
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
    virtual void VideoFmtPresetting() = 0;  //Ԥ���� Dialog
    virtual void ApplyVideoFmtPresettting(bool bApplyMySet) = 0;    //Ӧ��Ԥ����
    virtual void ApplyWndProp(bool bApply) = 0;    //Ӧ�ô�������
    virtual void ShowHPTMDialog() = 0;	//����ģ������

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
	
	//0x01:�ܾ���������
	//0x02:����������ʾ��Ϣ
	//0x04:�Զ�����
	//0x10:����ڻ����У����Զ��˳�������뱻����Ļ���

	//ͬ��/�ܾ���������
	virtual void AcceptRequestSpeaker(string roomid,string groupId,string sToId,bool bAnswer = true)=0;
	virtual void RejectRequestSpeaker(string roomid,string groupId,string sToId)=0;

	//ͬ��/�ܾ���������
	virtual void AcceptRequestQuiz(string roomid,string groupId,string sToId)=0;
	virtual void RejectRequestQuiz(string roomid,string groupId,string sToId)=0;

	//ֻ�ж�������Ƿ�Ϊ�����ߣ������ǰ�������ˣ�����FALSE
	virtual bool  IsMySpeaker() =0;
	//
	virtual unsigned long GetActiveTM() = 0;
	virtual RECT GetPrimaryScreenRect() const  = 0;
    
	virtual long GetChannelPacketLen() = 0;
	virtual long GetAVRPlayerDataLen() = 0;

	virtual bool IsShowMeetingTitle() =0;
	virtual bool IsShowMeetingAffiche() =0;

	virtual void UpDateMeetingMember(MMS_ROOM metRoomItem,std::vector<std::string> vecNewMembers ,std::vector<std::string> vecOldMembers,const std::string& strGroupId="") = 0;
    //Ȩ�޿���
    virtual void SetPrivilegeCode(unsigned long nCode) = 0;
    virtual unsigned long GetPrivilegeCode() = 0;
    virtual bool IsEnabledPrivilege( unsigned long nCode ) = 0;
	virtual void MonUserData(KCmdUserDataPacket& packet)=0;	//���ͼ������

    virtual void RequestSpeak(bool cancel_request = false) = 0;   //���뷢��
    virtual void RequestQuestion(bool cancel_request = false) = 0;  //��������
	virtual void TempCtrl()=0;				//ģ�������
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
	virtual void SetIMAVdioSetting(std::string memberid, BOOL bIMAVSET = TRUE, unsigned long ulChannelID = 0) = 0;		// IM ������Ƶ�����򵼵��� bIMAVSET�Ƿ�IM�е���

	virtual std::string GetMemberName(const std::string& memberid)=0;	//�õ������Ա����

	//  [11/21/2008 n.x.s] begin
	//�����Դ�ص�
	virtual void OnGetGroupInfoEnd(const std::string& strDomain)=0;
	virtual void OnChannelInfoEnd(const std::string& strDomain)=0;
	virtual void OnChannelInfo(const CHANNELINFO& item, bool bTreeSort = true)=0;
	virtual void OnDevChannelChange(const std::string& strDevID,const std::string& strChannelID,unsigned long ulVideoID,unsigned long ulAudioID)=0;
	virtual void OnUpdateDevName(const std::string& strDevID,const std::string& strName, unsigned long ulGroupID)=0;
	virtual void OnUpdateChannelName(const std::string& strChannelID,const std::string& strName, unsigned long ulGroupID)=0;

	//��Ա״̬�仯֪ͨ
	virtual void OnOrgUserStatus(const std::string& strUserID,const std::string& strStatus,bool bFirst,const std::string& strNodeID,const std::string& strNatAddr,const std::string& strLocalAddr,unsigned short usLocalPort)=0;

	//����ǽ����
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
	//����Ϊ��װAVCONMS�����ӽӿ�
	///* �ڻ����б��д��������� */
	//virtual void InsertRoom(const std::string& strRoomName, const std::string& strPassword, const std::string& strDesc)=0;
	///* �ڻ����б���ɾ�������� */
	//virtual void DeleteRoom(const std::string& strRoomID)=0;
	///* �ڻ����б�����ӻ����� */
	//virtual void InsertGroup(const std::string& strRoomID, const std::string& strParentGroupID, const std::string& strGroupName)=0;
	///* �ڻ����б�����ӻ����Ա */
	//virtual void InsertMember(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID,
	//	const std::string& strMemberName, int mtMemberType)=0;
	/* ��¼������ */
	virtual void Login(const std::string& strRoomID, const std::string& strPassword, const std::string& strMemberID,
		const std::string& strMemberName, const std::string& strInviteeGroupID, int mtMemberType)=0;
	/* ����m_strPreRoomID */
	virtual void SetPreRoomID(const std::string& strRoomID)=0;
	//end

	//����ѡ��ToolBar
	virtual void SetStreamType(int iType) = 0;	//iTypeΪ-1ʱ���������ļ���ȡ������
	//end
	virtual void SetFrameRateControlMode(int nMode) = 0;	//nModeΪ-1ʱ���������ļ���ȡ������
	virtual void SetMCUItem(MMS_MCU* pMCU) = 0;
	virtual void ShowPTZControl() = 0;	//��ʾ/������̨����̨
	//End

	virtual void SetAVCONType(int type) = 0;
	virtual void OnRequestIntoMeetingData(KCmdUserDataPacket& packet) = 0;

	virtual void OnMixVideoDateChanged() = 0;
	virtual void OnIMLoginDone(BOOL bDone) = 0;			// ֪ͨ����IM�����Ѿ���¼��ɣ����Խ��н���������
	
	//״̬�����
	virtual bool IsConferencePlayer() = 0;
	virtual void ShowMeetingLocalAV(bool bShow) = 0;
	virtual void ProcessLocalVolumeCapture(bool bCapture) = 0;
	virtual void OnControlShowHideIM() = 0;
	virtual void ClearRecPlanData() = 0;	//ɾ������¼��ƻ�����
	virtual void RecordRemoteChanged() = 0;  //¼���ʽ�ı�
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
	virtual int GetScreenCount() = 0;		//�õ���Ļ������
	virtual void SetLocalMicStatus(bool bOpen)=0;		//�����м�¼��ǰMIC״̬

	virtual void ShowWBTools(void)=0;	
	//added by luoning 2011-4-1
	virtual void NotifyToCheckTouchScreen(bool bCheck) = 0;
	//end

	virtual void ShowCloseAudioBar() = 0;
	virtual bool IsBindRecordServer(void)=0;
	////////////////////IDeviceClientNotify//////////////////////////////////////
	/*
	��¼ʧ�ܻص�
	nErrCode:�������
	strError:��������
	*/
	virtual void OnDeviceClientLogin(int nErrCode, CString strError)=0;
	/*
	�豸���߳ɹ��ص�
	�ڴ˽���ͨ������
	allow_count:��������ͨ������
	*/
	virtual void OnDeviceOnline(int allow_count)=0;
	/*
	�豸����֪ͨ
	*/
	virtual void OnDeviceOffline(void)=0;
	/*
	ͨ�����߳ɹ��ص�
	*/
	virtual void OnChannelOnline(void)=0;
	/*
	ͨ������֪ͨ
	*/
	virtual void OnChannelOffline(void)=0;

	virtual void ConferenceDeviceLogout(void)=0;

	//��ȡ��ǰ��������
	virtual MMS_ROOM_TYPE GetCurRoomType(void)=0;

	//��ȡ��ǰ������
	virtual std::string GetCurRoomDomain(void)=0;	

	//��ʼ�����б�
	virtual void InitMeetMonDomain(void)=0;
	virtual void InitMeetMonRoomList(const std::string& strDomain)=0;

	virtual void OnP2P(const std::string& strDevID, KCmdUserDataPacket& packet)=0;
	virtual void OnYTControl(KCmdPacket& rCmdPacket)=0;
	virtual void ReSetFullScreenToolBarStatus(UINT nType, int nFlag)=0;
	//Զ��¼��
	virtual void SetRemoteRecordPos(void) = 0;
	virtual void EnablePreview(int nPreview)=0;

	//��������ͨ��
	virtual void InitLocalDev()=0;

	//IMͨ������
	virtual void IMChannelSet()=0;
	virtual int GetCurrChannelCount()=0;
	virtual void UpdateChannelName()=0;

	//�Ƿ���¼��Ȩ��
	virtual BOOL IsHavePrivRec(void) = 0;
};

AFX_EXT_CLASS IHPMeeting* CreateHPMeeting(IHPMeetingNotify& rNotify,CWnd* pWnd);

#endif
