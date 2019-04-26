// UCC7-IM��¼����ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"

//-------------------------------------------
// �Խ���Ϣ
typedef struct _REC_AUDIO_DATA
{
	unsigned long		ulAudiOID;
	std::string			strNodeid;
	std::string			strNataddr;
	std::string			strLocaladdr;
	unsigned short		usLocalport;
	std::string			strMcuid;
	std::string			strMcuaddr;
	unsigned short		usMcuport;
	std::string			strFromMemberID;
	int					nMode;			//1:�������:0��غ���
	std::string			strDevID;
	std::string			strChanID;
	std::string			strUserData;
}REC_AUDIO_DATA,*PREC_AUDIO_DATA;

namespace AVCONPlugin
{
	class IPlugin_U7_IM_Talk_Observer
	{
	public:
		virtual ~IPlugin_U7_IM_Talk_Observer(){};

	public:
		virtual void OnTalkData(const std::string &strChannelID, const std::string &strResult) = 0;
		virtual void OnTalkRequest(CString strMemberID){};
		virtual void OnTalkAccept(CString strMemberID, bool bAccept){};

		/*	�������ܣ������ص�
		��    ����bDev���Ƿ��豸
		strMemberID��"FINISH"������ "BUSY"��æµ
		strReason������ԭ��
		�� �� ֵ��*/
		virtual void OnEndTalk(bool bDev, CString strMemberID, const std::string &strReason = "FINISH"){};
	};

	// UCC7-IM��¼����ʵ�ֲ��
	class IPlugin_U7_IM_Talk: public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_Talk(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_Talk_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_Talk_Observer* pObserver) = 0;

		virtual void BeginTalk(const std::string& strChannelID, int nCurrentTalkType, bool bForce=false) = 0;
		virtual bool IsInTalking(const std::string& strDevID) = 0;
		virtual void TalkAccept(const std::string& strDevID, bool bAccept) = 0;
		
		// nCloseType 1:�����߹Ҷ� 2���������߹Ҷ�
		virtual void EndTalk(/*std::string strMemberID, */const std::string& strDevID, int nCloseType) = 0;
		virtual std::string GetRoomId() = 0;
		virtual PREC_AUDIO_DATA GetCardItem(const char* strMemberID, int nCardIndex) = 0;
	};
}
