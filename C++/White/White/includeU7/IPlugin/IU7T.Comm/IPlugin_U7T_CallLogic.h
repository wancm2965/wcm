#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	// UCC7-T�����߼�ͨ�ýӿ�
	class IPlugin_U7T_CallLogic : virtual public IPlugin
	{
	public:
		virtual void InitCallLogic(void) = 0;		// ��ʼ�������߼�
		virtual void UnInitCallLogic(void) = 0;	// ����ʼ�������߼�

		// ����㲥�߼������յ��㲥����󣬽���ģ��ǰ���ø÷���
		virtual bool DealBroadcast(int& nScreenID, int& nWindowID, const std::string& strMemberID, int nCardIndex, int nChannelType) = 0;
		// ����رչ㲥�߼������յ��رչ㲥���������رմ��ں���ø÷���
		virtual bool DealCloseBroadcast(const std::string& strMemberID, int nCardIndex, int nChannelType) = 0;

		virtual int GetDigitalCard() = 0;
		virtual int GetTMMaxCount(void) = 0;
		virtual int GetMaxCount(void) = 0;	// ��ȡ�������������ʱ��תΪ����ģʽ
		virtual int GetAutoBroadcastCount(void) = 0;
		virtual bool IsAllowChangeMeeting(void) {return true;};
	};
}
