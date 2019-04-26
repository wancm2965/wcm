// UCC7-MM���鹦��ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	// ����Ƶ���Ͷ���
	typedef enum _THPAVStream{
		HPAV_TYPE_NONE		= 0,
		HPAV_TYPE_VIDEO		= 100,	// ��Ƶ
		HPAV_TYPE_AUDIO,			// ��Ƶ
		HPAV_TYPE_AV,				// ����Ƶ
	}THPAVStream;

	class IPlugin_U7_MM_Function_Observer
	{
	public:
		virtual ~IPlugin_U7_MM_Function_Observer(){};

	public:
		virtual void OnBroadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,int nChannelType){};
		virtual void OnCloseBroadcast(const char* strMemberID,int nCardIndex,int nChannelType){};
		virtual void OnRecvChatMessage( const std::string& strMemberID,const std::string& strHead,const std::string& strBody,const std::string& strFormat ){};

		/* �϶����� */
		virtual void OnDragWindow(int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID){};

		/* �ı�ģ�� */
		virtual void OnSetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber){};

		virtual void OnSetCurrentScreenID(int nCurScrID){}; 

		virtual void OnSetFullScreen(const std::string& strMemberID,int nCardIndex,int nScreenID,int nWindowID,int nFullType){};

		/* �������� */
		virtual void OnSetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY){};

		virtual void OnSetTempPresider(std::string& strMemberID,bool bSet){};

		/*��������*/
		virtual void OnLockMeet(bool bLock){};

		// ������Ȩ�ص�
		virtual void OnTouchScreenAuthorize(bool bAuthorize){};

		//�Խ�
		virtual void OnChAudioStart(std::string strFromUserID, std::string strUserData){};

		// ͶƱ
		/* ԤͶƱ�ص�
		 * strMemberID: ͶƱ������
		 * strVoteID:	ͶƱ��ʶ
		 * bAllow:		�Ƿ�����ͶƱ�������ǰ�������Ѿ�������һ��ͶƱ�����ֵΪfalse */
		virtual void OnPrepVote(const std::string& strMemberID,const std::string& strVoteID,bool bAllow){};
		
		/* ��ʼͶƱ�ص�
		 * strVoteID:	ͶƱ��ʶ
		 * strTopic:	ͶƱ����
		 * bType:		ͶƱ���ͣ�����ͶƱΪtrue��������Ϊfalse
		 * ulTime:		ͶƱʱ����Ϊ����AVCONͳһ��ʹ�ú��뵥λ
		 * bMulti:		�Ƿ������ѡ
		 * strItems:	ѡ���б� */
		 virtual void OnStartVote(const std::string& strVoteID,const std::string& strTopic,bool bType,unsigned long ulTime,bool bMulti,MMS_VOTE_LST& strItems){};
		
		 /* ֹͣͶƱ�ص�
		 * strVoteID:	ͶƱ��ʶ
		 * strEndType:	ͶƱ������ʽ���ֶ����Զ���"hand" �� "auto"��
		 * nSum:		ӦͶƱ����
		 * nAnswer:		��Ӧ����
		 * strItems:	ѡ���б� */
		virtual void OnStopVote(const std::string& strVoteID,const std::string& strEndType,int nSum,int nAnswer,MMS_VOTE_LST& strItems){};

		/* ��ʼ���֣������˶����յ� ulID�����ξ��ֵ�ID; ulTime:����ʱ�� */
		virtual void OnStartHandle(unsigned long ulID, unsigned long ulTime){};
		/* ֹͣ���֣������˶����յ� ulID�����ξ��ֵ�ID; usTotal:������; usCount:��������*/
		virtual void OnStopHandle(unsigned long ulID, unsigned short usTotal, unsigned short usCount){};

		/* ��ʼ����,�����˶����յ� strMemberID:����������ID */
		virtual void OnCalling(const std::string& strMemberID, unsigned int usTime){};
		virtual void OnCloseCalling(const std::string& strMemberID){};
	};

	// UCC7-MM���鹦��ʵ�ֲ��
	class IPlugin_U7_MM_Function : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_MM_Function(){};

	public:
		virtual void AddObserver(IPlugin_U7_MM_Function_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_MM_Function_Observer* pObserver) = 0;

		// ���ذװ塢�ĵ��������۵�ChannelID
		virtual unsigned long PublishLocalCard(int nCardIndex) = 0;
		// ������Ƶͨ��ID�ı�ʱ�ٴη���
		virtual void SetCardAudioItem(int nCardIndex,unsigned long ulAudioID) = 0;

		virtual void Broadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,AVCONPlugin::THPAVStream nAVType) = 0;
		virtual void CloseBroadcast(const char* strMemberID,int nCardIndex,AVCONPlugin::THPAVStream nAVType) = 0;

		/* ������ʱ������
		* bSet: true=����;false=ȡ�� */
		virtual void SetTempPresider(std::string& strMemberID,bool bPresider) = 0;

		virtual void SendMMMessage(const std::string& strMemberID,const std::string& strBody,const std::string& strFormat)= 0;

		/* �л����� */
		virtual void DragWindow(int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID) = 0;

		/*���õ�ǰ������*/
		virtual void SetActive(int nScreenID) = 0 ;

		/* ���û���ģ�� */
		virtual void SetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber) = 0;

		virtual void SetFullScreen(const std::string& strMemberID, int nCardIndex, int nScreenID, int nWindowID, int nFullType) = 0;
		/* ���ø�������λ��
		* x = ���ر��� * 100
		* y = ���ر��� * 100
		* cx= ��ȱ��� * 100
		* cy= �߶ȱ��� * 100 */
		virtual void SetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY) = 0;

		virtual void ModifyCardName(const std::string& strMemberID, int nCardIndex, const std::string& strCardName) = 0;//lml

		virtual void LockMeet(bool bLock) = 0;

		// ��ȡ������Ȩ
		virtual void GetTouchScreenAuthorize(void) = 0;
		// �ͷŴ�����Ȩ
		virtual void ReleaseTouchScreenAuthorize(void) = 0;

		virtual void BeginTalk(const std::string strChannelID, int nCurrentTalkType, bool bForce=false) = 0;

		/****************************************************************
		 * ����ͶƱ����
		 ****************************************************************/

		/* ֻ��λ��root��������˲ſ��Է���ͶƱ,��������в��ܷ���
		 * ԤͶƱ */
		virtual void PrepVote(void) = 0;

		/* ����ͶƱ 
		 * strVoteID:	ͶƱ��ʶ
		 * strTopic:	ͶƱ����
		 * bType:		ͶƱ���ͣ�����ͶƱΪtrue��������Ϊfalse
		 * ulTime:		ͶƱʱ����Ϊ����AVCONͳһ��ʹ�ú��뵥λ
		 * bMulti:		�Ƿ������ѡ
		 * strItems:	ѡ���б�*/
		virtual void StartVote(const std::string& strVoteID, const std::string& strTopic,
			bool bType, unsigned long ulTime, bool bMulti, MMS_VOTE_LST& strItems) = 0;

		/* ����ͶƱ 
		 * strVoteID:	ͶƱ��ʶID
		 * strEndType:	ͶƱ������ʽ���ֶ����Զ���"hand" �� "auto"��*/
		virtual void StopVote(const std::string& strVoteID, const std::string& strEndType) = 0;

		/* ͶƱ
		 * strVoteID:	ͶƱ��ʶID
		 * strItems:	�б��д���ҪͶƱ��ѡ������Ȩ��������б��� */
		virtual void Vote(const std::string& strVoteID, MMS_VOTE_LST& strItems) = 0;

		/****************************************************************
		 * ������ֺ���
		 ****************************************************************/

		/* ��ʼ����:�����˵��� */
		virtual void StartHandle(unsigned long ulTime) = 0;

		/* ����:�����˵��� */
		virtual void Handle(unsigned long ulID, bool bAgree) = 0;

		/* ֹͣ����:�����˵��� */
		virtual void StopHandle(unsigned long ulID) = 0;

		/****************************************************************
		 * �����������
		 ****************************************************************/

		/* ��ʼ����:�����˵��� */
		virtual void Calling(unsigned int usTime) = 0;

		/* �رյ���:�����˵��� */
		virtual void CloseCalling() = 0;

		/* �����������ȷ��ͼƬ:�������˵��� */
		virtual void CallingInConference(const std::string& strFile) = 0;
	};
}
