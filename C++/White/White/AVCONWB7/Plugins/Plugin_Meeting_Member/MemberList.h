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
enum _HIDDEN_DIRECTION   //隐藏方向
{
	//DIR_LEFT,          //向左隐藏
	DIR_TOP,           //向上隐藏
	DIR_RIGHT          //向右隐藏
};
#define TIMER_MOVEINWINDOW_MEMBER	WM_USER + 5000	//移进窗口定时器
#define TIMER_MOVEOUTWINDOW_MEMBER	TIMER_MOVEINWINDOW_MEMBER + 1		//移出窗口定时器

using namespace AVCONPlugin;

class CDlgMemberList
	: public WindowImplBase
	, IDataRoomMember
{
public:
	CDlgMemberList(void);
	~CDlgMemberList(void);
public:
	// 继承WindowImplBase接口
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
	// 继承IDataRoomMember接口
	//在会状态改变
	virtual void OnMeetMemStatus(string strMemberID, unsigned char ucStatus,  unsigned short usMemberType);

	//仅显示在线用户
	void OnlyShowOnlineUser();

	//设置label控件背景图片
	BOOL SetItemStatusPic(CControlUI* pControlUI, CString strSubCtrlName, CString strLogo);
	//设置人员节点label控件图片和文本
	void SetMemberItemLableText(CControlUI* pListElement, CString strSubCtrlName,CString strLogo);

public:
	bool AddOneMemberInfo(MEMEBER_INFO&MemberInfo);

	// 自动隐藏窗口
	void HiddenWindow();
	// 自动显示窗口
	void ReShowWindow();
protected:
	void LoadRootTree();
public:
	//开辟线程加载列表
	void InitMemberInfo();
	void ThreadInitMemberInfo();
	static unsigned int __stdcall ThreadFunc(void* param);
	void ProcessTipMessage();
private:
	CDialogBuilder				m_dlgBuilder;
	CTreeUI*						m_pMemberTree;		//人员列表树
	CTreeItemUI*				m_pTreeElement ;
	std::string						m_strMemberID;		// 保存当前选中卡节点的人员ID
	MAP_NODE					m_mapNodeGroup;		//保存会议组节点数据
	MAP_NODE					m_mapNodeMember;	//保存人员节点、MP节点数据
	bool								m_bStartThread;
	//让会员列表框有自动伸缩功能
	int							m_nHiddenDir;			// 定义主窗口自动隐藏时的方向，0向左，1向上，2向右
	bool						m_bTrack;				// 定义是否收缩
	bool						m_bFloat;				// 定义是否停靠
	CMemberTip*   m_pMemberListTip;
};

#endif
