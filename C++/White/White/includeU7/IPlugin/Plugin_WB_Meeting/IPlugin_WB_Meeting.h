#pragma once
#ifndef _IPLUGIN_WB_MEETING_H__
#define _IPLUGIN_WB_MEETING_H__

#include "ISystem/IPlugin.h"
namespace AVCONPlugin
{
	class IPlugin_WB_Meeting_Card //根据广播卡号类型创建相应的广播窗口
	{
	public:
		virtual HWND GetCardWnd(const char* szMemberId,int nCardIndex) = 0;
	};
	class IPlugin_WB_Meeting_Observer
	{
	public:
		~IPlugin_WB_Meeting_Observer();
	public:
		virtual void OnExitMeeting(void){};
		virtual void OnExitMeeting(int nError){};
		virtual void OnEnterRoomReady(){};
		virtual void OnEnterRoom(unsigned int nErrCode,const char* strDomainID,const char* strRoomID,const char* strRoomName,unsigned short usRoomType,unsigned short usMemberType){};
		void OnBroadcast( int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,const char* strCardName,int nChannelType ){};
		void OnCloseBroadcast(const char* strMemberID,int nCardIndex,int nChannelType){};
	};
	class IPlugin_WB_Meeting : public IPlugin
	{
	public:
		virtual ~IPlugin_WB_Meeting(){};
		//会议列表插件的接口
	public: 
		virtual void ShowUIMeetingList(bool bShow) = 0;
		virtual void AddObserver(IPlugin_WB_Meeting_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_WB_Meeting_Observer* pObserver) = 0;
		
		virtual void RegCard(int nCardIndex,IPlugin_WB_Meeting_Card* pCard) = 0;
		virtual void UnRegCard(int nCardIndex) = 0;
		virtual std::string GetRoomName() = 0;
		
		virtual void AVConf_Enter(const std::string& strDomain,const std::string& strRoomID,const std::string& strPassword) = 0; //进会议
		// 主动退回
		virtual void ExitMeeting(void) = 0;
		virtual void OnExitMeeting(void) = 0;
		// 广播
		virtual void Broadcast(int nScreenID,int nWindowID,const std::string& strMemberID,int nCardIndex,int nChannelType) = 0;
		// 关闭广播，bDestroy 关闭广播时是否销毁这个窗口，有可能关闭这个广播的时候
		// 并不希望销毁它，则将其设为false
		virtual void CloseBroadcast(const std::string& strMemberID,int nCardIndex,int nChannelType, bool bDestroy = true) = 0;
		virtual void ProcessTipMessage() = 0;
	};
}
#endif
