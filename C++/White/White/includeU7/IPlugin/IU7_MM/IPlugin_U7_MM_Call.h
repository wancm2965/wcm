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

		// �յ�ĳ�ڵ㷢��Ķ��˺�������
		virtual void OnCallRequest(
			const char* strNodeID,
			const char* strUserID, 
			const char* strUserOrgID,
			const char* strUserDptID,
			const char* strUserName,
			long lUserData){};

		// �յ�ĳ�˴���ǰ�û����͵���������
		virtual void OnProcessCallRequest(
			const char* strNodeID,
			const char* strUserID, 
			const char* strUserOrgID,
			const char* strUserDptID,
			const char* strUserName,
			const char* strRoomID, 
			bool bAgree,
			const char* strReason,
			long lUserData){};

		// �յ�ĳ�˹ҶϺ��е���Ϣ
		virtual void OnHangupCall(
			const char* strNodeID, 
			const char* strUserID, 
			const char* strUserOrgID,
			const char* strUserDptID,
			const char* strUserName,
			int	nCallID,
			long lUserData){};

		virtual void OnEnterCallMeeting(int nErrorCode) = 0;
		virtual void OnExitCallMeeting(int nErrorCode) = 0;
		virtual void OnUserEnterMeeting(const char* strUserID) = 0;
		virtual void OnUserExitMeeting(const char* strUserID) = 0;
		virtual void OnRecvAV(const char* strUserID,HWND hWnd) = 0;
		virtual void OnCloseAV(const char* strUserID) = 0;
		virtual void OnRecvChatMessage(const char* strUserID,const char* strHead,const char* strBody,const char* strFormat) = 0;
		virtual void OnBroadcast(int nScreenID,int nWindowID,const std::string& strMemberID,int nCardIndex,const std::string& strCardName,int nChannelType) = 0;
		virtual void OnCloseBroadcast(const std::string& strMemberID, int nCardIndex,int nChannelType) = 0;
		virtual void OnReady()=0;
		virtual void OnNotifyP2PCheck(const std::string& strNodeid, const std::string& strUserid, const std::string& strUserName) = 0;
	};

	typedef struct _TREQUEST_INFO
	{
		std::string strNodeID;
		std::string strUserID;
	}TREQUESTINFO;

	// UCC7-MM������й���ʵ�ֲ��
	class IPlugin_U7_MM_Call : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_MM_Call(){};

	public:
		virtual void AddObserver(IPlugin_U7_MM_Call_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_MM_Call_Observer* pObserver) = 0;

		// �����˷���һ����������
		virtual void CallRequest(const std::list<std::string>& lstMemberID, long lUserData = 0) = 0;
		virtual void CallRequest(const std::list<TREQUESTINFO>& lstRequestInfo, long lUserData = 0) = 0;

		// ����һ����������
		virtual void ProcessCallRequest(const char* strUserID, const char* strRoomID, bool bAgree, const char* strReason = "",long lUserData = 0) = 0;
		virtual void ProcessCallRequest(const char* strNodeID, const char* strUserID, const char* strRoomID, bool bAgree, const char* strReason = "", long lUserData = 0) = 0;

		// �Ҷ���ĳ�˵ĺ���
		virtual void HangupCall(const char* strUserID, int nCall, bool bAutoTime = false) = 0;
		virtual void HangupCallByNodeID(const char* strNodeID, int nCall, bool bAutoTime = false) = 0;

		// �����˻�����
		virtual void EnterCallMeeting(void) = 0;
		virtual void ExitCallMeeting(bool bAllLogout) = 0;
		virtual void InviteUser(const char* strUserID) = 0;

		// ����תΪ���飬֪ͨ�ײ�
		virtual void Change2Meeting(void) = 0;

		// ���л������ԡ�����
		virtual std::string GetCallRoomID(void) = 0;
		virtual void SetCallCamer(int nCardIndex) = 0;
		virtual void Broadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,AVCONPlugin::THPAVStream nAVType)=0;
		virtual void CloseBroadcast(const char* strMemberID,int nCardIndex,AVCONPlugin::THPAVStream nAVType)=0;
		virtual bool IsOwnerInCallMetting()=0;
		virtual bool IsCallMeeting()=0;
		virtual unsigned long PublishLocalCard(int nCardIndex) = 0;
		virtual void SetCallRoomID(const std::string& strRoomID) = 0;
	};
}
