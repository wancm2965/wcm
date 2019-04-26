#pragma once
#include "win_impl_base.hpp"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Function.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Member.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Room.h"
#include "IPlugin/Plugin_WB_FrameDlg/IPlugin_WB_FrameDlg.h"
#include "MenuBarDlg.h"

class CDlgMainFrame 
	: public WindowImplBase
	, public IPlugin_U7_MM_Function_Observer
	, public IPlugin_U7_MM_Member_Observer
	, public IPlugin_U7_MM_Room_Observer
	, public IPlugin_WB_FrameDlg_Observer
{
public:
	CDlgMainFrame(void);
	~CDlgMainFrame(void);
	//继承WindowImplBase接口
public:
	virtual void Init();
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LPCTSTR GetWindowClassName() const;
	virtual CString GetSkinFile();

	//继承IPlugin_U7_MM_Function_Observer接口
	virtual void OnBroadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,const char* strCardName,int nChannelType,const MMS_MEMBER_TYPE& memberType);
	virtual void OnCloseBroadcast(const char* strMemberID,int nCardIndex,int nChannelType);
	virtual void OnTouchScreenAuthorize(bool bAuthorize);

	//继承IPlugin_U7_MM_Room_Observer接口
	virtual void OnEnterRoom(unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType);
		/* 收到退出会议室事件
		* nErrCode返回登录结果值
		* 0: 表示正常退出会议室
		* 1: 表示所有者退出会议室
		* 2: 表示被踢出会议室
		* 3: 会议成员帐号在该服务器上不存在或该服务器没有加载会议模块 */
	virtual void OnEnterRoomReady();
	virtual void OnExitRoom(unsigned int nErrCode);

public:
	//继承IPlugin_WB_FrameDlg_Observer接口
	virtual void ProcessWBToLocalData( char* pNotifyData );//接收电子白板内部发出来的信令
public:
	bool Connect();
	bool ReleaseConnect();

	void OnExitInternet();//网络退回到单机
	
	void ShowMenuBarDlg();
protected:
	virtual void Notify(TNotifyUI& msg);
	void OnCoUITimer( TNotifyUI& msg );
	//击中判断
	//virtual bool BInRegion( const POINT &pt ) const;
private:

//	CSkinNotifyTransitionToNet* m_pSkinNotifyTransitionToNet;
	bool					m_bTouchScreenAuthorize;
	unsigned short		m_usMemberType; //用户在会议中的身份 
	bool					m_bTakeDataToNet; //是否要发送数据到网络
	bool					m_bAcceptBroadCast;//是否接受到广播消息

	bool                   m_bIsExitRoomCurTime;//是否退出会议wang

	std::string             m_strUserMemberID;//登录用户ID
	
	
	CMenuBarDlg*            m_pMenuBarDlg;
}; 

CDlgMainFrame* GetDlgMainFrame_(void);
