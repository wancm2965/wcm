// UCC7-MM���鹦��ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "U7_MMSDefine.h"
#include "AVCONSys/KXNCP.h"

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
		virtual void OnBroadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,const char* strCardName,int nChannelType,const MMS_MEMBER_TYPE& memberType, const std::string& strTag){};
		virtual void OnCloseBroadcast(const char* strMemberID,int nCardIndex,int nChannelType){};
		virtual void OnRecvChatMessage( const std::string& strMemberID,const std::string& strHead,const std::string& strBody,const std::string& strFormat ){};

		/* �϶����� */
		virtual void OnDragWindow(int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID){};

		virtual void OnRoomDigitalComp(const _MMS_CHLTYPE& channelType){};
		/* �ı�ģ�� */
		virtual void OnSetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber){};

		virtual void OnSetCurrentScreenID(int nCurScrID){}; 

		virtual void OnSetFullScreen(int nScreenID,int nWindowID,int nFullType){};

		virtual void OnTellFullScreen(int nCardIndex){};

		/* �������� */
		virtual void OnSetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY){};

		virtual void OnSetTempPresider(const std::string& strMemberID,bool bSet){};

		/*��������*/
		virtual void OnSetRight(int nRight, bool bEnable, const std::string& strMemberID){};

		// ������Ȩ�ص�
		virtual void OnTouchScreenAuthorize(bool bAuthorize){};

		/* �����ɾ�������˻ص� */
		// strMemberID��������ID
		// bRequest��trueΪ���룬falseΪɾ��
		virtual void OnSetQuestioner(const std::string& strMemberID,bool bRequest){};

		/* ���ӻ�ȡ�����������˻ص� */
		// strMemberID��������ID
		// bEnable���Ƿ񵥽�������
		virtual void OnEnableQuestioner(const std::string& strMemberID,bool bEnable){};

		// �������
		virtual void OnAnnexData(int nType,const std::string& strData,const std::string& strFormat){};
		virtual void OnEnableAnnex(int nType,bool bEnable){};

		//�Խ�
		virtual void OnChAudioStart(std::string strFromUserID, std::string strUserData){};

		virtual void OnGetAlarmLogInfo( const AVCON_MON_ALARMLOG& alarmLoginfo ){};
		/* ��ʼͶƱ�ص�
		 * strVoteID:	ͶƱ��ʶ
		 * strInitiator: ������
		 * strTopic:	ͶƱ����
		 * bType:		ͶƱ���ͣ�����ͶƱΪtrue��������Ϊfalse
		 * ulTime:		ͶƱʱ����Ϊ����AVCONͳһ��ʹ�ú��뵥λ
		 * bMulti:		�Ƿ������ѡ
		 * strItems:	ѡ���б� */
		 virtual void OnStartVote(const std::string& strVoteID,const std::string& strInitiator,const std::string& strTopic,bool bType,unsigned long ulTime,bool bMulti,MMS_VOTE_LST& strItems){};

		/* ͶƱ����ص�
		 * nErrCode:	������ */
		 virtual void OnStartVoteErr(unsigned int nErrCode){};

		 /* ֹͣͶƱ�ص�
		 * strVoteID:	ͶƱ��ʶ
		 * strEndType:	ͶƱ������ʽ���ֶ����Զ���"hand" �� "auto"��
		 * nSum:		ӦͶƱ����
		 * nAnswer:		��Ӧ����
		 * strItems:	ѡ���б� */
		virtual void OnStopVote(const std::string& strVoteID,const std::string& strEndType,int nSum,int nAnswer,MMS_VOTE_LST& strItems){};

		/* ��ʼ���֣������˶����յ� ulID�����ξ��ֵ�ID; ulTime:����ʱ�� */
		virtual void OnStartHandle(unsigned long ulID, unsigned long ulTime){};
		/* ���ִ���nErrCode:������ */
		virtual void OnStartHandleErr(unsigned int nErrCode){};
		/* ֹͣ���֣������˶����յ� ulID�����ξ��ֵ�ID; usTotal:������; usCount:��������*/
		virtual void OnStopHandle(unsigned long ulID, unsigned short usTotal, unsigned short usCount){};

		/* ��ʼ����,�����˶����յ� strMemberID:����������ID */
		virtual void OnCalling(const std::string& strMemberID, unsigned int usTime){};
		virtual void OnCallingErr(unsigned int nErrCode){};
		virtual void OnCloseCalling(const std::string& strMemberID){};

		/* ģ��Ԥ�ò����ص� */
		virtual void OnSetScreenAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute){};
		/* ���û��鴰������
		* strAllAttribute���д��ڵ����Դ�
		* strCurAttribute��ǰ���õ����Դ���Ϊ�գ���Ӧ��strAllAttribute��ֵ*/
		virtual void OnSetWindowsAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute=""){};


		/* ��Ƶ�ɼ����� */
		virtual void OnSetCaptureLock(const std::string& strMemberID, int nDevID, bool bLock){};

		/* �Զ����� */
		virtual void OnSetAutomaticCall(int nEndTime, int nIntervalTime){};

		virtual void OnSetWndStatusInLive(const std::string& strParameter){};

		virtual void OnShowMeetingRemind(bool bShowMeetingRemind){};

		// ���û������
		virtual void OnSetMeetingParameter(const std::string& strGroupID,const std::string& strKeyword, const std::string& strParameter, const std::string& strAdminID){};
		// ɾ���������
		virtual void OnDelMeetingParameter(const std::string& strGroupID,const std::string& strKeyword){};

		//�˻ص�δ�ͻ����Լ��ֶ����ã��ڻ����У��Լ�����Ƶ���㲥�ˣ���ʱ��Ƶ���ñ��ı�Ӧ���ֶ����ô˻ص���
		//Ŀ����֪ͨ�����������Լ����������ñ��ı䣬Ӧ�õ�����Ƶ���ܷ�ʽ����Ҫ��TR2���յ�����Ϣ�������������ܣ�Ԥ��С���ڽ��������
		//nStream, int nResWidth, int nResHeightΪ������ֵ�ͷֱ���
		virtual void OnSetHighStreamResolution(const std::string& strMemberID, int nCardIndex, int nStream, int nResWidth, int nResHeight, bool bOpenCalc = true){};
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
		virtual unsigned long PublishLocalCard(int nCardIndex, int nDevID = -1) = 0;

		// ������Ƶͨ��ID�ı�ʱ�ٴη���
		virtual void SetCardAudioItem(int nCardIndex,unsigned long ulAudioID) = 0;

		virtual void Broadcast(int nScreenID,int nWindowID,const char* strMemberID,int nCardIndex,AVCONPlugin::THPAVStream nAVType,bool bMutex = true, const std::string& strTag = "CON_SYNC") = 0;
		virtual void CloseBroadcast(const char* strMemberID,int nCardIndex,AVCONPlugin::THPAVStream nAVType) = 0;

		/* ������ʱ������
		* bSet: true=����;false=ȡ�� */
		virtual void SetTempPresider(std::string& strMemberID,bool bPresider) = 0;

		virtual void SendMMMessage(const std::string& strMemberID,const std::string& strBody,const std::string& strFormat)= 0;

		/* �л����� */
		virtual void DragWindow(int nFromCardID, int nToCardID, const std::string& strFromMemberID, const std::string& strToFromMemberID, int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID) = 0;

		/*���õ�ǰ������*/
		virtual void SetActive(int nScreenID) = 0 ;

		/* ���û���ģ�� */
		virtual void SetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber) = 0;

		/* ���ô���ȫ����������nScreenID ģ��ţ�nWindowID ���ںţ�nFullType �Ƿ�ȫ����
		* lpszMemberID ͨ���������ߣ�nCardIndex ͨ���ţ�
		* ˵������nFullTypeΪ0����ȡ��ȫ��ʱ������lpszMemberID��nCardIndex�����壬��ȱʡ��
		*		��nFullTypeΪ1�������ô���ȫ��ʱ������ָ��ͨ�������ߵ�ID�Լ�ͨ���ţ�
				�����ͨ��û�������ߣ������ۺͰװ壩����ô����lpszMemberID��ȱʡ�� */
		virtual void SetFullScreen(int nScreenID, int nWindowID, int nFullType, const char* lpszMemberID = "", int nCardIndex = 0) = 0;
		virtual void TellFullScreen(const std::string& strMemberID, int nCardIndex) = 0;
		/* ���ø�������λ��
		* x = ���ر��� * 100
		* y = ���ر��� * 100
		* cx= ��ȱ��� * 100
		* cy= �߶ȱ��� * 100 */
		virtual void SetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY) = 0;

		/* ��ȡ��ͬ����Ϣ */
		virtual void GetGroupSynInfo(const std::string& strGroupID) = 0;

		virtual void ModifyCardName(const std::string& strMemberID, int nCardIndex, const std::string& strCardName) = 0;//lml

		virtual void SetRight(int nRight, bool bEnable, const std::string& strMemberID="") = 0;

		// ��ȡ������Ȩ
		virtual void GetTouchScreenAuthorize(void) = 0;
		// �ͷŴ�����Ȩ
		virtual void ReleaseTouchScreenAuthorize(void) = 0;

		virtual void BeginTalk(const std::string &strChannelID, int nCurrentTalkType, bool bForce=false) = 0;

		/* �����ɾ�������� */
		// strMemberIDΪ�������������ʣ�strMemberID��Ϊ����ɾ��������
		virtual void SetQuestioner(const std::string& strMemberID = "") = 0;

		/* ���ӻ�ȡ������������*/
		// strMemberID ��������ID
		// bEnable���Ƿ񵥽�
		virtual void EnableQuestioner(const std::string& strMemberID, bool bEnable) = 0; 

		/****************************************************************
		 * ������⡢���溯��
		 ****************************************************************/

		/* �������� */
		virtual void SetAnnexData(int nType,const std::string& strData,const std::string& strFormat) = 0;

		/* ���� */
		virtual void EnableAnnex(int nType,bool bEnable=true) = 0;

		/* 	��ȡ���桢��������	*/
		virtual void GetAnnexData(void) = 0;

		/****************************************************************
		 * ����ͶƱ����
		 ****************************************************************/

		/* ֻ��λ��root��������˲ſ��Է���ͶƱ,��������в��ܷ���
		 * ԤͶƱ */
		virtual void PrepVote(void) = 0;

		/* ����ͶƱ
		 * strTopic:	ͶƱ����
		 * bType:		ͶƱ���ͣ�����ͶƱΪtrue��������Ϊfalse
		 * ulTime:		ͶƱʱ����Ϊ����AVCONͳһ��ʹ�ú��뵥λ
		 * bMulti:		�Ƿ������ѡ
		 * strItems:	ѡ���б�*/
		virtual void StartVote(const std::string& strTopic, bool bType, unsigned long ulTime, bool bMulti, MMS_VOTE_LST& strItems) = 0;

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
		/* ���û���������
		 * strAllAttribute�����������Դ�
		 * strCurAttribute��ǰ���õ����Դ���Ϊ�գ���Ӧ��strAllAttribute��ֵ*/
		virtual void SetScreenAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="") = 0;
		
		/* ���û��鴰������
		* strAllAttribute���д��ڵ����Դ�
		* strCurAttribute��ǰ���õ����Դ���Ϊ�գ���Ӧ��strAllAttribute��ֵ*/
		virtual void SetWindowsAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="") = 0;

		/*���� MPS*/
		virtual void StartMP(void)=0;
		virtual void Start3GC(void)=0;
		/*���� MPS*/
		virtual void StopMPS(const std::string& strMPSUserID)=0;

		/* ��Ƶ�ɼ����� */
		virtual void SetCaptureLock(const std::string& strMemberID, int nDevID, bool bLock) = 0;

		// �����Զ�����
		virtual void SetAutomaticCall(int nEndTime, int nIntervalTime) = 0;

		virtual void SetWndStatusInLive(const std::string& strWndStatusInLive) = 0;

		//�ֶ�����֪ͨ�Լ��ĸ������Լ��ֱ��ʱ��ı䣡�ڻ����У��Լ�����Ƶ���㲥�ˣ���ʱ��Ƶ���ñ��ı�Ӧ���ֶ����ô˻ص���
		//Ŀ����֪ͨ�����������Լ����������ñ��ı䣬Ӧ�õ�����Ƶ���ܷ�ʽ����Ҫ��TR2���յ�����Ϣ�������������ܣ�Ԥ��С���ڽ��������
		//nStream, int nResWidth, int nResHeightΪ������ֵ�ͷֱ���
		virtual bool SetHighStreamResolution(const std::string& strMemberID, int nCardIndex, int nStream, int nResWidth, int nResHeight, bool bOpenCalc = true) = 0;
		
		//���ͻ�������֪ͨ�����ˣ���Ƶ���������Լ��ֱ��ʱ��ֶ��ı䣨������TR2�����ڵ���������
		virtual void SetStreamResolutionNotice(int nCardIndex, int nStream, int nResWidth, int nResHeight) = 0;

		// �Ƿ���ʾ��������
		virtual void ShowMeetingRemind(bool bShow = true) = 0;

		// ���û������
		virtual void SetMeetingParameter(const std::string& strGroupID, const std::string& strKeyword, const std::string& strParameter, MMS_SYNCOBJECT synObject=SYNCOBJ_ALL, const std::string& strSynAppoint="", MMS_PRMAVLBSTTS prmAvlbStts=PAS_MANUAL) = 0;
		// ɾ���������
		virtual void DelMeetingParameter(const std::string& strGroupID, const std::string& strKeyword, MMS_SYNCOBJECT synObject=SYNCOBJ_ALL, const std::string& strSynAppoint="", MMS_PRMAVLBSTTS prmAvlbStts=PAS_MANUAL) = 0;

		//���ۿͷ�StrServiceID:�ͷ�ID,usSatisfaction:����ȣ�0:�������ۣ�1:�����⣬2:�������⣬3:���⣬4:�ǳ����⣩��usServiceTime:����ʱ������λ�룩
		virtual	void JudgeService(const std::string& strServiceID,const unsigned short& usSatisfaction=0,const unsigned long& ulServiceTime=0) = 0;

		// �ϴ��º��ܽ� 
		// business_type:ҵ������
		// business_first_level:һ��ҵ������
		// business_second_level:����ҵ������
		// strDepart:�ͷ��Լ����ڵĲ���
		// starttime:�ͷ���ʼ�绰����ʼʱ��
		// endtime:�ͷ������绰����ʱ��
		virtual void UploadServiceSumary(const int& business_type, const int& business_first_level, const int& business_second_level, const std::string& strDepart = "", const unsigned long& ulTalkTime = 0) = 0;
	};
}
