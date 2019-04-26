#pragma once

//��������
typedef enum{
	PT_NONE=0,			//�޴���ֱ������
	PT_HTTP_PROXY,		//HTTP����
	PT_SOCK5,			//SOCK5����
	PT_HTTP_TUNNEL		//HTTP���
}PROXY_TYPE;

//����Э������
typedef enum{
	PT_TCP=0,			//TCP����Э��
	PT_UDP,				//UDP����Э��
	PT_RTP,				//RTP����Э��
}PROTOCOL_TYPE;

//��������
typedef enum{
	ROOM_TYPE_WORKGROUP=0,		//���������
	ROOM_TYPE_NORMAL,			//��׼����
	ROOM_TYPE_ADVANCE,			//�߼�����
}ROOM_TYPE;

//�����Ա����
typedef enum{
	MEMBER_TYPE_ATTENDEE=0,		//�����
	MEMBER_TYPE_PRESIDER,		//������
}MEMBER_TYPE;

//��¼���ش���������
typedef enum{
	LOGIN_SUCCESS=0,			//0: �ɹ���¼������
	LOGIN_PWD_ERROR,			//1: ������֤ʧ��
	LOGIN_ALREADY_LOGIN,		//2: �û��Ѿ��������
	LOGIN_MORETHAN_SIX,			//3: �����Ѿ�������������
	LOGIN_NOFEAT_SVR,			//4: ����û���ʵ��ķ�����
	LOGIN_OWNER_LEFT,			//5: �����������뿪����
	LOGIN_MEM_TYPE_ERROR,		//6: ��¼�����Ͳ���ȷ
	LOGIN_ROOM_LOCKED,			//7: �����Ѿ�����
	LOGIN_ROOM_TYPE_ERROR,		//8: �������Ͳ���ȷ
	LOGIN_SVR_ERROR=99,			//99:�����ڲ���������
}LOGIN_ERRCODE_TYPE;

//�˳����鷵�ش���������
typedef enum{
	LOGOUT_SUCCESS=0,			//0: ��ʾ�����˳�������
	LOGOUT_OWNER_LEFT,			//1: ��ʾ�������˳�������
	LOGOUT_KICKOUT,				//2: ��ʾ���߳�������
}LOGOUT_ERRCODE_TYPE;


//=======================================================================================
// IAVCONMS �ඨ��
//=======================================================================================
class AFX_EXT_CLASS IAVCONMS
{
public:
	IAVCONMS(void){};
	virtual ~IAVCONMS(void){};

public:
	/* �������� */
	virtual bool Connect(const std::string& strServerAddr,	//������IP��ַ(����������ַ)
                         unsigned short usServerPort		//�������˿�
						 )=0;

	/* �Ͽ����� */
	virtual void Disconnect(void)=0;

	/* ��������ͨѶЭ������ */
	virtual void SetSessionProtocolType(PROTOCOL_TYPE type)=0;

	/* ������Ƶ����Э������ */
	virtual void SetAudioProtocolType(PROTOCOL_TYPE type)=0;

	/* ������Ƶ����Э������ */
	virtual void SetVideoProtocolType(PROTOCOL_TYPE type)=0;

	/* ���ô������� */
	virtual void SetProxyType(PROXY_TYPE pt)=0;

	/* ���ô����û� */
	virtual void SetProxyUser(const char* cszUser)=0;

	/* ���ô������� */
	virtual void SetProxyPassword(const char* cszPassword)=0;

	/* ���ô�������� */
	virtual void SetProxyHost(const char* cszHost)=0;
	virtual void SetProxyPort(unsigned short usPort)=0;

	/* �ڻ����б��д��������� */
	virtual void InsertRoom(const std::string& strRoomName,		//�����Ҵ������Զ���Ļ�������
							ROOM_TYPE rtRoomType,				//��������
							const std::string& strPassword,		//�������룬�����Ա�������ʱ��Ҫ��֤���
							const std::string& strDesc			//����������Ϣ
							)=0;

	/* �ڻ����б���ɾ�������� */
	virtual void DeleteRoom(const std::string& strRoomID)=0;

	/* �ڻ����б�����ӻ����� */
	virtual void InsertGroup(	const std::string& strRoomID,			//������ID
								const std::string& strParentGroupID,	//���ڵ���ID
								const std::string& strGroupName			//����������
								)=0;

	/* �ڻ����б�����ӻ����Ա */
	virtual void InsertMember(	const std::string& strRoomID,			//������ID
								const std::string& strGroupID,			//������ID�����Ϊ�գ��򽫳�Ա��ӵ�����
								const std::string& strMemberID,			//�����ԱID
								const std::string& strMemberName,		//�����Ա����
								MEMBER_TYPE mtMemberType				//��Ա����
								)=0;

	/* ��¼������ */
	virtual void Login(	const std::string& strRoomID,					//������ID
						const std::string& strPassword,					//��������
						const std::string& strMemberID,					//�����ԱID
						const std::string& strMemberName,				//�����Ա����
						const std::string& strInviteeGroupID,			//���Ϊ�������ߣ�����Ҫȷ������ID
						MEMBER_TYPE mtMemberType=MEMBER_TYPE_ATTENDEE	//���Ϊ�������ߣ�����Ҫȷ����Ա����
						)=0;

	/* �˳������� */
	virtual void Logout(void)=0;
};


//=======================================================================================
// IAVCONMSEvent �¼��ඨ��
//=======================================================================================
class AFX_EXT_CLASS IAVCONMSEvent
{
public:
	IAVCONMSEvent(void){};
	virtual ~IAVCONMSEvent(void){};

public:
	/* ���ӷ�����ʧ�� */
	virtual void OnConnectFailed(void)=0;

	/* �����ӵ������� */
	virtual void OnConnected(const std::string& strNatAddr)=0;

	/* ������������������ѶϿ� */
	virtual void OnDisconnected(void)=0;

	/* ���ش����Ļ�����ID */
	virtual void OnRoomInsert(const std::string& strRoomID)=0;

	/* ���ر�ɾ���Ļ�����ID */
	virtual void OnRoomDelete(const std::string& strRoomID)=0;

	/* ���ش�������ID���������ID */
	virtual void OnGroupInsert(const std::string& strRoomID,const std::string& strGroupID)=0;

	/* ���ش����ĳ�ԱID���������ID */
	virtual void OnMemberInsert(const std::string& strRoomID,const std::string& strMemberID)=0;

	/* �յ���¼�������¼� */
	virtual void OnLogin(LOGIN_ERRCODE_TYPE nErrCode)=0;

	/* �յ��˳��������¼� */
	virtual void OnLogout(LOGOUT_ERRCODE_TYPE nErrCode)=0;
};


//=======================================================================================
// IAVCONMS ʵ��
//=======================================================================================
AFX_EXT_CLASS IAVCONMS* CreateAVCONMSImpl(IAVCONMSEvent& rAVCONMSEvent);
AFX_EXT_CLASS void DestoryAVCONMSImpl(void);
AFX_EXT_CLASS IAVCONMS* GetAVCONMSImpl(void);