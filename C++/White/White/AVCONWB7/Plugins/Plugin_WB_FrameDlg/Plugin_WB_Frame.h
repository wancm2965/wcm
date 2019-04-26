#pragma once
#include "IPlugin/Plugin_WB_FrameDlg/IPlugin_WB_FrameDlg.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Member.h"
#include "IPlugin/Plugin_WB_Meeting/IPlugin_WB_Meeting.h"
#include "CWBPersonalDlg.h"
#include "HPWBModel.h"
namespace AVCONPlugin
{
	class CPlugin_WB_FrameDlg
	: public IPlugin_WB_FrameDlg
	, public IPlugin_U7_MM_Member_Observer
	, public IPlugin_WB_Meeting_Card
	{
	public:
		CPlugin_WB_FrameDlg(void);
		~CPlugin_WB_FrameDlg(void);

		//继承自IPlugin接口 
	public:
		// 获取插件名称
		virtual std::string Name();
		// 获取插件功能点
		virtual std::string ExtendPoint();
		// 初始化插件
		virtual bool InitPlugin();
		// 插件退出处理
		virtual bool ClosePlugin();
		
		//继承自IPlugin_WB_Meeting_Card接口
	public:
		//对于单机版来说，该函数在这里的主要功能是连通网络实现网络同步功能
		virtual HWND GetCardWnd(const char* szMemberId,int nCardIndex);
		
		//继承自IPlugin_U7_WhiteBoard接口
	public:
		virtual void AddObserver(IPlugin_WB_FrameDlg_Observer* pObserver);
		virtual void DelObserver(IPlugin_WB_FrameDlg_Observer* pObserver);
		
		virtual HWND GetHwnd(void);
		virtual HWND CreateWhiteBoard(void);
		virtual BOOL SetWindowPos(HWND hWndInsertAfter,int X,int Y,int cx,int cy,UINT uFlags);
		virtual void DestroyWhiteBoard(void);
		virtual bool Connect(void);
		virtual void DisConnect(void);

		virtual void RequestAllObj(void);
		virtual void EnableTouch(bool bTouchScreen);
		virtual void SetLocalUserID(char *strUserID);
		virtual void MemberOffline(char *strUserID);
		virtual void Move(int x,int y,int cx,int cy,UINT nFlag);
		virtual void ProcessWbData(const char *pData, int nLen);
		virtual void GetUserID(std::string &strUser);
		virtual void SetLocalUserName(CString str);
		virtual void SetWBAVLock(bool block);
		
	public:
		//继承IPlugin_U7_MM_Member_Observer接口
		virtual void OnMemberStatus(const char* strGroupID,const char* strMemberID,unsigned short usMemberType,unsigned char ucStatus);
		
		virtual void OpenResource();
	private:
		CWBPersonalDlg* m_pWhiteBoardDlg;
		bool m_bConnect;	
		std::string m_strUserID;	
	};
}