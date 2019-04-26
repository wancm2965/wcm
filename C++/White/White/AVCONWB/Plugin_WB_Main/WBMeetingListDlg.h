#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "ISystem/ISystem.h"
#include "IPlugin\IU7_MM\U7_MMSDefine.h"
#include "IPlugin\IU7_MM\IPlugin_U7_MM_Room.h"
#include "IPlugin\IU7_IM\IPlugin_U7_IM_Friend.h"
using namespace AVCONPlugin;

// CWBMeetingListDlg 对话框

class CWBMeetingListDlg : public CDialog, public IPlugin_U7_MM_Room_Observer
{
	DECLARE_DYNAMIC(CWBMeetingListDlg)

public:
	CWBMeetingListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBMeetingListDlg();
	virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);

// 对话框数据
	enum { IDD = IDD_MEETINGLIST_DLG };

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CTreeCtrl m_menberlist;
	CTreeCtrl m_meetingListTree;
	HTREEITEM m_currentmeeting;//当前选中会议项
	CImageList m_meetingAvatarLst;//会议名前面的图标链表
	CString strSelectMeetingName;//当前选中的会议室的名字
	PMMS_ROOM m_pRoom;	 //当前会议的房间信息
	HTREEITEM defaultMeetingLstRoot;
	HTREEITEM advanceMeetingLstRoot;

	CWnd * m_pParentWnd;

public:
	void InitMeetingList();
	void InsertMember(CString strMemberName);
	HTREEITEM *GetCurrentMeeting();
	PMMS_ROOM GetCurrentMeetingInfo();
	virtual BOOL OnInitDialog();
	CString *GetSelectMeetingName();
	afx_msg void OnTvnSelchangedMeetinglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkMeetinglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// 继承自 IPlugin_U7_MM_Room_Observer 接口
	virtual void OnGetPersonalRoom(const char* strRoomID,const char* strGroupID,const char* strMeetingName,bool bHaveFrame,unsigned short usStatus){};
	virtual void OnGetPersonalRoomEnd(void) {};
	virtual void OnGetPersonalRoomGroup(const char* strRoomID,const MMS_GROUP& group) {};
	virtual void OnGetPersonalRoomGroupEnd(const char* strRoomID) {};
	virtual void OnGetPersonalRoomMember(const char* strRoomID,const char* strGroupID,const char* strMemberID,unsigned short usMemberType) {};
	virtual void OnGetPersonalRoomMemberEnd(const char* strRoomID) {};
	virtual void OnAddPersonalRoom(const char* strRoomID,const char* strGroupID,const char* strMeetingName,const char* strOwnerID) {};
	virtual void OnDelPersonalRoom(const char* strRoomID) {};
	virtual void OnRenamePersonalRoom(const char* strRoomID,const char* strMeetingName) {};
	virtual void OnAddPersonalRoomMember(const char* strRoomID,const char* strGroupID,const char* strMemberID,unsigned short usMemberType) {};
	virtual void OnSetPersonalRoomMemberType(const char* strRoomID,const char* strMemberID,unsigned short usMemberType) {};
	virtual void OnDelPersonalRoomMember(const char* strRoomID,const char* strMemberID) {};
	virtual void OnAddPersonalRoomGroup(const MMS_GROUP& group) {};
	virtual void OnAddPersonalRoomGroupEnd(const char* strRoomID) {};


	/*会议中人员都退出*/
	virtual void OnStopPersonalRoom(const char* strRoomID){};
	/*获取指定会议室的一个在线成员*/
	virtual void OnGetAppointRoomOnlineMember(const char* strRoomID,const char* strMemberID) {};
	/*获取指定会议室的一个在线合成终端或主持人通道信息*/
	virtual void OnGetAppointRoomOnlineMemberInfo(const char* strDomain,const char* strRoomID,const char* strMemberID,
		unsigned int unCardIndex,unsigned long ulAudioID,unsigned long ulVideoID,
		const char* strMCUID,const char* strMCUAddr,unsigned short usMCUPort,
		const char* strNodeID,const char* strNatAddr,const char* strLocalAddr,unsigned short usLocalPort,const char* strTag) {};
	/*通知个人会议发起人会议已经开始(第一个成员已经登录会议)*/
	virtual void OnNotifyPersonalAdminMeetingStart(const char* strRoomID) {};



	virtual void OnRoomListEnd(void);
	virtual void OnEnterRoom(unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType);
	virtual void OnExitRoom(unsigned int nErrCode);
	virtual void OnEnterRoomReady(); 
	afx_msg void OnDestroy();
};
