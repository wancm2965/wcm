#pragma once
#include "stdafx.h"
#include "IPlugin/Plugin_WB_Meeting/IPlugin_WB_Meeting.h"
#include "MeetingList.h"

namespace AVCONPlugin
{
	typedef std::map<int,IPlugin_WB_Meeting_Card*>MAP_CARD;
	typedef MAP_CARD::iterator IT_MAP_CARD;
	
	class CPlugin_WB_Meeting 
		: public IPlugin_WB_Meeting
	{
	public:
		CPlugin_WB_Meeting();
		~CPlugin_WB_Meeting();
		//重写
	public:
		// 获取插件名称
		virtual std::string Name();
		// 获取插件功能点
		virtual std::string ExtendPoint();
		// 初始化插件
		virtual bool InitPlugin();
		// 插件退出处理
		virtual bool ClosePlugin();
		
		virtual void ShowUIMeetingList(bool bShow);
		
		//继承IPlugin_WB_Meeting接口
		virtual void AddObserver(IPlugin_WB_Meeting_Observer* pObserver);
		virtual void DelObserver(IPlugin_WB_Meeting_Observer* pObserver);
		
		virtual void RegCard(int nCardIndex,IPlugin_WB_Meeting_Card* pCard);
		virtual void UnRegCard(int nCardIndex);
		
		virtual std::string GetRoomName();

		virtual void AVConf_Enter(const std::string& strDomain,const std::string& strRoomID,const std::string& strPassword); //进会议
		// 主动退回
		virtual void ExitMeeting(void);
		virtual void OnExitMeeting(void);
		// 广播
		virtual void Broadcast(int nScreenID,int nWindowID,const std::string& strMemberID,int nCardIndex,int nChannelType);
		// 关闭广播，bDestroy 关闭广播时是否销毁这个窗口，有可能关闭这个广播的时候
		// 并不希望销毁它，则将其设为false
		virtual void CloseBroadcast(const std::string& strMemberID,int nCardIndex,int nChannelType, bool bDestroy = true);
	public:
		virtual void ProcessTipMessage();
	private:
		std::list<IPlugin_WB_Meeting_Observer*> m_lstObserver;
		MAP_CARD m_mapCard;
		CMeetingList* m_pMeetingList;
	};
}
CPlugin_WB_Meeting* GetPluginWBMeeting();

