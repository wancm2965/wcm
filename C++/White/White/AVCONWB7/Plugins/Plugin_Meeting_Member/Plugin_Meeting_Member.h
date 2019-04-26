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
		// ��ȡ�������
		virtual std::string Name();
		// ��ȡ������ܵ�
		virtual std::string ExtendPoint();
		// ��ʼ�����
		virtual bool InitPlugin();
		// ����˳�����
		virtual bool ClosePlugin();
	public:
		//�̳�IPlugin_Meeting_Member
		virtual void ShowUI(bool bShow);
		virtual void ResleseResource();

		//�̳�IPlugin_U7_MM_Member_Observer
		//�յ������������
		virtual void OnRoomGroupItem(const MMS_GROUP_EX& item);
		//���ջ���������ݽ���
		virtual void OnRoomGroupItemEnd(void);
		//�յ�������Ա����
		virtual void OnRoomMemberItem(const MMS_MEMBER_EX& item);
		//���ջ�����Ա���ݽ���
		virtual void OnRoomMemberItemEnd(void);
		//���ջ���ͨ�����ݽ���
		virtual void OnMemberCardItemEnd(void);
		//����Ա��MP�ڵ���ᡢ�˻�ʱ�Ĵ���
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