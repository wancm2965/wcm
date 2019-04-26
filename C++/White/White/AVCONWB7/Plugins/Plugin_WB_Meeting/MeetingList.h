#pragma once
#include "win_impl_base.hpp"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Room.h"
#include "MeetingTip.h"
enum _HIDDEN_DIRECTION   //隐藏方向
{
	//DIR_LEFT,          //向左隐藏
	DIR_TOP,           //向上隐藏
	DIR_RIGHT          //向右隐藏
};
#define TIMER_MOVEINWINDOW	WM_USER + 4000	//移进窗口定时器
#define TIMER_MOVEOUTWINDOW	TIMER_MOVEINWINDOW + 1		//移出窗口定时器
#define TIMER_REFRESHMEETINGLIST          TIMER_MOVEINWINDOW + 2//刷新会议列表
using namespace AVCONPlugin;


namespace AVCONPlugin
{

	class CMeetingList
		: public WindowImplBase
		, public IPlugin_U7_MM_Room_Observer
	{
	public:
		CMeetingList(void);
		~CMeetingList(void);

	public:
		//////////////////////////////////////////////////////////////////////////
		// 继承WindowImplBase接口
		virtual void Init();
		virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LPCTSTR GetWindowClassName() const;
		virtual CString GetSkinFile();
		virtual CControlUI* CreateControl(LPCTSTR pstrClass);
		virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		//////////////////////////////////////////////////////////////////////////
		
		// 自动隐藏窗口
		void HiddenWindow();
		// 自动显示窗口
		void ReShowWindow();

	public:
		//继承实现IPlugin_U7_MM_Room_Observer
		virtual void OnRoomListEnd(void);
		virtual void OnEnterRoom(unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType);
		virtual void OnExitRoom(unsigned int nErrCode);
		virtual void OnRoomListItem(const std::string& strRoomID,const std::string& strRoomName,const std::string& strRoomPwd, unsigned short usRoomType,const std::string& strDesc,int nMemberSum,int nOnlineSum);
		virtual void OnMMData(void){};
		virtual void OnEnterRoomReady();

//////////////////////////////////////////////////////////////////////////

		void AddMeetingList();
		void RemoveAllMemList();
		//基于CoUI的消息机制，当鼠标移动至列表某一项的子对象按钮时，该按钮会接受一个MouseEnter消息
		//列表项会接受一个MouseLeave消息，导致列表项的高亮背景消失，该函数用于解决此问题
		bool ProcessListItemBtnEvent(void* p);
	protected:
		virtual void Notify(TNotifyUI& msg);

		void EnterMeetingRoom(PMMS_ROOM pMMSRoom);
		void AddMeetItem(PMMS_ROOM pRoomListItem);
		void SetControlFontSize(CLabelUI *pLabel, unsigned int nFontSize);
	public:
		void ProcessTipMessage();
	private:
		CMeetingTip *m_pMeetingTip;
		void InitLocalLang();
		CDialogBuilder		m_dlgBuilder;
		CTreeUI*			m_pMemberTree;
		bool					m_bLogout;

		bool					m_bTouchScreenAuthorize;//触摸是否支持
		//让会员列表框有自动伸缩功能
		int							m_nHiddenDir;			// 定义主窗口自动隐藏时的方向，0向左，1向上，2向右
		bool						m_bTrack;				// 定义是否收缩
		bool						m_bFloat;				// 定义是否停靠
		
	};

}
CMeetingList* GetMeetingList(void);