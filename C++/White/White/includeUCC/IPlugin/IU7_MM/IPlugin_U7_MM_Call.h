// UCC7-MM������й���ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "../../IPlugin/IU7_MM/IPlugin_U7_MM_Function.h"

namespace AVCONPlugin
{
	class IPlugin_U7_MM_Call_Observer
	{
	public:
		virtual ~IPlugin_U7_MM_Call_Observer(){};

	public:
		// �յ�ĳ�˵ĺ�������
		virtual void OnCallRequest(const char* strUserID) = 0;
		// �յ�ĳ�˷���Ķ��˺�������
		virtual void OnCallRequest(const char* strUserID, std::list<std::string> lstMemberID) = 0;
		// �յ�ĳ�˴���ǰ�û����͵���������
		virtual void OnProcessCallRequest(const char* strUserID, bool bAgree) = 0;
		// �յ�ĳ�˹ҶϺ��е���Ϣ
		virtual void OnHangupCall(const char* strUserID) = 0;

		virtual void OnEnterCallMeeting(int nErrorCode) = 0;
		virtual void OnExitCallMeeting(int nErrorCode) = 0;

		virtual void OnUserEnterMeeting(const char* strUserID) = 0;
		virtual void OnUserExitMeeting(const char* strUserID) = 0;

		virtual void OnRecvAV(const char* strUserID,HWND hWnd) = 0;
		virtual void OnCloseAV(const char* strUserID) = 0;
		virtual void OnRecvChatMessage(const char* strUserID,const char* strHead,const char* strBody,const char* strFormat) = 0;
		virtual void OnBroadcast(int nScreenID,int nWindowID,const std::string& strMemberID,int nCardIndex,int nChannelType) = 0;
		virtual void OnCloseBroadcast(const std::string& strMemberID, int nCardIndex,int nChannelType) = 0;
		virtual void OnReady()=0;
	};

	// UCC7-MM������й���ʵ�ֲ��
	class IPlugin_U7_MM_Call : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_MM_Call(){};

	public:
		virtual void AddObserver(IPlugin_U7_MM_Call_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_MM_Call_Observer* pObserver) = 0;

		// ��ĳ�˷���һ����������
		virtual void CallRequest(const char* strUserID) = 0;
		// �����˷���һ����������
		virtual void CallRequest(const std::list<std::string>& lstMemberID) = 0;
		// ����һ����������
		virtual void ProcessCallRequest(const char* strUserID, bool bAgree) = 0;
		// �Ҷ���ĳ�˵ĺ���
		virtual void HangupCall(const char* strUserID) = 0;

		virtual void EnterCallMeeting(void) = 0;
		virtual void ExitCallMeeting(void) = 0;
		virtual void InviteUser(const char* strUserID) = 0;

		virtual std::string GetCallRoomID(void) = 0;
		virtual void SetCallCamer(int nCardIndex) = 0;
		virtual void Broadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,AVCONPlugin::THPAVStream nAVType)=0;
		virtual void CloseBroadcast(const char* strMemberID,int nCardIndex,AVCONPlugin::THPAVStream nAVType)=0;
		virtual bool IsOwnerInCallMetting()=0;
		virtual bool IsCallMeeting()=0;
		virtual unsigned long PublishLocalCard(int nCardIndex)=0;
	};
}
