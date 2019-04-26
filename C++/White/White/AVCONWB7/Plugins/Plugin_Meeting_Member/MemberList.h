#ifndef _MEMBER_LIST_H__
#define _MEMBER_LIST_H__
#pragma once
#include "win_impl_base.hpp"
#include "MMComFunction.h"
#include "DataRoomMember.h"
#include "IPlugin/Plugin_WB_FrameDlg/IPlugin_WB_FrameDlg.h"
#include "MemberListTip.h"
#define WM_MPSHOWDIAL			10023
#define WM_DEL_MEMBER_ITEM		10024
#define WM_DEL_SEARCH_ITEM		10025
enum _HIDDEN_DIRECTION   //���ط���
{
	//DIR_LEFT,          //��������
	DIR_TOP,           //��������
	DIR_RIGHT          //��������
};
#define TIMER_MOVEINWINDOW_MEMBER	WM_USER + 5000	//�ƽ����ڶ�ʱ��
#define TIMER_MOVEOUTWINDOW_MEMBER	TIMER_MOVEINWINDOW_MEMBER + 1		//�Ƴ����ڶ�ʱ��

using namespace AVCONPlugin;

class CDlgMemberList
	: public WindowImplBase
	, IDataRoomMember
{
public:
	CDlgMemberList(void);
	~CDlgMemberList(void);
public:
	// �̳�WindowImplBase�ӿ�
	virtual void				Init();
	virtual LRESULT		OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT		OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT		OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT		HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LPCTSTR		GetWindowClassName() const;
	virtual CString			GetSkinFile();
	virtual void				OnFinalMessage(HWND hWnd);
protected:
	virtual void Notify(TNotifyUI& msg);

public:
	// �̳�IDataRoomMember�ӿ�
	//�ڻ�״̬�ı�
	virtual void OnMeetMemStatus(string strMemberID, unsigned char ucStatus,  unsigned short usMemberType);

	//����ʾ�����û�
	void OnlyShowOnlineUser();

	//����label�ؼ�����ͼƬ
	BOOL SetItemStatusPic(CControlUI* pControlUI, CString strSubCtrlName, CString strLogo);
	//������Ա�ڵ�label�ؼ�ͼƬ���ı�
	void SetMemberItemLableText(CControlUI* pListElement, CString strSubCtrlName,CString strLogo);

public:
	bool AddOneMemberInfo(MEMEBER_INFO&MemberInfo);

	// �Զ����ش���
	void HiddenWindow();
	// �Զ���ʾ����
	void ReShowWindow();
protected:
	void LoadRootTree();
public:
	//�����̼߳����б�
	void InitMemberInfo();
	void ThreadInitMemberInfo();
	static unsigned int __stdcall ThreadFunc(void* param);
	void ProcessTipMessage();
private:
	CDialogBuilder				m_dlgBuilder;
	CTreeUI*						m_pMemberTree;		//��Ա�б���
	CTreeItemUI*				m_pTreeElement ;
	std::string						m_strMemberID;		// ���浱ǰѡ�п��ڵ����ԱID
	MAP_NODE					m_mapNodeGroup;		//���������ڵ�����
	MAP_NODE					m_mapNodeMember;	//������Ա�ڵ㡢MP�ڵ�����
	bool								m_bStartThread;
	//�û�Ա�б�����Զ���������
	int							m_nHiddenDir;			// �����������Զ�����ʱ�ķ���0����1���ϣ�2����
	bool						m_bTrack;				// �����Ƿ�����
	bool						m_bFloat;				// �����Ƿ�ͣ��
	CMemberTip*   m_pMemberListTip;
};

#endif
