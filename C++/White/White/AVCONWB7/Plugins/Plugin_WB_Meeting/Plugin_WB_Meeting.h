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
		//��д
	public:
		// ��ȡ�������
		virtual std::string Name();
		// ��ȡ������ܵ�
		virtual std::string ExtendPoint();
		// ��ʼ�����
		virtual bool InitPlugin();
		// ����˳�����
		virtual bool ClosePlugin();
		
		virtual void ShowUIMeetingList(bool bShow);
		
		//�̳�IPlugin_WB_Meeting�ӿ�
		virtual void AddObserver(IPlugin_WB_Meeting_Observer* pObserver);
		virtual void DelObserver(IPlugin_WB_Meeting_Observer* pObserver);
		
		virtual void RegCard(int nCardIndex,IPlugin_WB_Meeting_Card* pCard);
		virtual void UnRegCard(int nCardIndex);
		
		virtual std::string GetRoomName();

		virtual void AVConf_Enter(const std::string& strDomain,const std::string& strRoomID,const std::string& strPassword); //������
		// �����˻�
		virtual void ExitMeeting(void);
		virtual void OnExitMeeting(void);
		// �㲥
		virtual void Broadcast(int nScreenID,int nWindowID,const std::string& strMemberID,int nCardIndex,int nChannelType);
		// �رչ㲥��bDestroy �رչ㲥ʱ�Ƿ�����������ڣ��п��ܹر�����㲥��ʱ��
		// ����ϣ����������������Ϊfalse
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

