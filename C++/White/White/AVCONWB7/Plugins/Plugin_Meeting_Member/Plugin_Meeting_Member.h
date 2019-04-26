#ifndef _PLUGIN_MEETING_MEMBER_H__
#define _PLUGIN_MEETING_MEMBER_H__
#include "stdafx.h"
#include "IPlugin/Plugin_Meeting_Member/IPlugin_Meeting_Member.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Member.h"
#include "IPlugin/Plugin_WB_FrameDlg/IPlugin_WB_FrameDlg.h"
#include "MemberList.h"
namespace AVCONPlugin
{
	class CPlugin_Meeting_Member 
		: public IPlugin_Meeting_Member
		, public IPlugin_U7_MM_Member_Observer
	{
	public:
		CPlugin_Meeting_Member();
		~CPlugin_Meeting_Member();
	public:
		// 获取插件名称
		virtual std::string Name();
		// 获取插件功能点
		virtual std::string ExtendPoint();
		// 初始化插件
		virtual bool InitPlugin();
		// 插件退出处理
		virtual bool ClosePlugin();
	public:
		//继承IPlugin_Meeting_Member
		virtual void ShowUI(bool bShow);
		virtual void ResleseResource();

		//继承IPlugin_U7_MM_Member_Observer
		//收到会议分组数据
		virtual void OnRoomGroupItem(const MMS_GROUP_EX& item);
		//接收会议分组数据结束
		virtual void OnRoomGroupItemEnd(void);
		//收到会议人员数据
		virtual void OnRoomMemberItem(const MMS_MEMBER_EX& item);
		//接收会议人员数据结束
		virtual void OnRoomMemberItemEnd(void);
		//接收会议通道数据结束
		virtual void OnMemberCardItemEnd(void);
		//当人员和MP节点进会、退会时的处理
		virtual void OnMemberStatus(const char* strGroupID,const char* strMemberID,unsigned short usMemberType,unsigned char ucStatus);
	protected:
		virtual void DestroyUI();
	public:
		virtual void ProcessTipMessage();
	private:
		CDlgMemberList* m_pMemberList;
	};
}
#endif