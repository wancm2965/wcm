#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "ISystem/ISystem.h"
#include "IPlugin\IU7_MM\U7_MMSDefine.h"
#include "IPlugin\IU7_MM\IPlugin_U7_MM_Room.h"
#include "IPlugin\IU7_IM\IPlugin_U7_IM_Friend.h"
using namespace AVCONPlugin;

// CWBMeetingListDlg �Ի���

class CWBMeetingListDlg : public CDialog, public IPlugin_U7_MM_Room_Observer
{
	DECLARE_DYNAMIC(CWBMeetingListDlg)

public:
	CWBMeetingListDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBMeetingListDlg();
	virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);

// �Ի�������
	enum { IDD = IDD_MEETINGLIST_DLG };

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	CTreeCtrl m_menberlist;
	CTreeCtrl m_meetingListTree;
	HTREEITEM m_currentmeeting;//��ǰѡ�л�����
	CImageList m_meetingAvatarLst;//������ǰ���ͼ������
	CString strSelectMeetingName;//��ǰѡ�еĻ����ҵ�����
	PMMS_ROOM m_pRoom;	 //��ǰ����ķ�����Ϣ
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

	// �̳��� IPlugin_U7_MM_Room_Observer �ӿ�
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


	/*��������Ա���˳�*/
	virtual void OnStopPersonalRoom(const char* strRoomID){};
	/*��ȡָ�������ҵ�һ�����߳�Ա*/
	virtual void OnGetAppointRoomOnlineMember(const char* strRoomID,const char* strMemberID) {};
	/*��ȡָ�������ҵ�һ�����ߺϳ��ն˻�������ͨ����Ϣ*/
	virtual void OnGetAppointRoomOnlineMemberInfo(const char* strDomain,const char* strRoomID,const char* strMemberID,
		unsigned int unCardIndex,unsigned long ulAudioID,unsigned long ulVideoID,
		const char* strMCUID,const char* strMCUAddr,unsigned short usMCUPort,
		const char* strNodeID,const char* strNatAddr,const char* strLocalAddr,unsigned short usLocalPort,const char* strTag) {};
	/*֪ͨ���˻��鷢���˻����Ѿ���ʼ(��һ����Ա�Ѿ���¼����)*/
	virtual void OnNotifyPersonalAdminMeetingStart(const char* strRoomID) {};



	virtual void OnRoomListEnd(void);
	virtual void OnEnterRoom(unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType);
	virtual void OnExitRoom(unsigned int nErrCode);
	virtual void OnEnterRoomReady(); 
	afx_msg void OnDestroy();
};
