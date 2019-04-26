#pragma once

#define AVCON_SERVER_VERSION		"7.2.1.1457_T"
#define AVCON_CMD_VERSION			0x0020

//////////////////////////////////////////////////////////////////////////////////////
//									SYS									s			//
//////////////////////////////////////////////////////////////////////////////////////

#define IO_CONNECT_TYPE_CLIENT_SVC			100
#define IO_CONNECT_TYPE_CLIENT_NFS			103		//�����ϰ汾
#define IO_CONNECT_TYPE_CLIENT_MGW			104
#define IO_CONNECT_TYPE_CLIENT_VRS			105		//���¼���ϴ�
#define IO_CONNECT_TYPE_CLIENT_PLT			106		//���ƽ̨
#define IO_CONNECT_TYPE_CLIENT_NDU          107
#define IO_CONNECT_TYPE_CLIENT_NDD          108
#define IO_CONNECT_TYPE_CLIENT_NDI          109

#define IO_CONNECT_TYPE_MCU_ROUTER			200
#define IO_CONNECT_TYPE_NFS_ROUTER			201

#define SERVER_TYPE_MCU						300
#define SERVER_TYPE_MTS						301
#define SERVER_TYPE_MCU_MTS					302
#define SERVER_TYPE_GW						303
#define SERVER_TYPE_3GC						304
#define SERVER_TYPE_3GM						305
//#define SERVER_TYPE_MP					306
#define SERVER_TYPE_GW_TR2					307
#define SERVER_TYPE_TRS						308		//ת�������
#define SERVER_TYPE_TPS						309		//Э��ת��������
#define SERVER_TYPE_TPS						309

#define IMS_AGENT_ID						400
#define MMS_AGENT_ID						401
#define MON_AGENT_ID						402
#define SMS_AGENT_ID						403
#define FAX_AGENT_ID						404
#define SIP_AGENT_ID						405
#define NDS_AGENT_ID						406
#define GPS_AGENT_ID						407
#define SYNC_AGENT_ID						408
#define REC_AGENT_ID                        409
#define CTS_AGENT_ID						410	//����AGENT����
#define VDS_AGENT_ID						411	//����VDS
#define CRS_AGENT_ID						412	//����CRS

//---------------------------------------------------------
// �ļ����䳣��
#define MAX_FILE_PACKET			65536 + 64
#define MAX_FILE_DATA_PACKET	65536
#define MAX_CMD_PACKET			32768 * 3

//---------------------------------------------------------
//���ֹ��ܹ㲥��������λ��ֵ
#define DIGITAL_SCREENID		999
#define DIGITAL_WINDOWID		999

//////////////////////////////////////////////////////////////////////////////////////
//									IMS												//
//////////////////////////////////////////////////////////////////////////////////////

// ��֤����
typedef enum _IMS_USER_AUTHTYPE
{
	IMS_AUTHTYPE_DB			= 0,		// �������ݿ���֤
	IMS_AUTHTYPE_LDAP		= 1,		// LDAP��֤
    IMS_AUTHTYPE_RADIUS		= 2,		// RADIUS��֤
    IMS_AUTHTYPE_GUEST      = 4,        // ���
	IMS_AUTHTYPE_SX			= 5,		// 
	IMS_AUTHTYPE_THIRDPART	= 99,		// ��������֤
};

//////////////////////////////////////////////////////////////////////////////////////
//									MMS												//
//////////////////////////////////////////////////////////////////////////////////////
#include "RIGHT.h" // ����Ȩ�޿��ƣ� ���ڸĶ��ϴ󣬶�����һ�������ļ�

// �б�
typedef enum _MMS_LIST
{
	MET_LIST_ROLOCUTOR	= 1,	// ����
	MET_LIST_QUESTIONER,		// ����
};

//-------------------------------------
// ͨ������
typedef enum _MMS_CHLTYPE
{
	MMS_CHLTYPE_VID = 1,	// ��Ƶͨ��
	MMS_CHLTYPE_AUD,		// ��Ƶͨ��
	MMS_CHLTYPE_DOC,		// �ĵ�ͨ��
	MMS_CHLTYPE_CHT,		// ����ͨ��
	MMS_CHLTYPE_WHB,		// �װ�ͨ��
	MMS_CHLTYPE_DTS,		// �ļ�����ͨ��
};

//---------------------------------------------------------------------------------------
// Ⱥ�����߹���
//---------------------------------------------------------------------------------------
typedef enum _IMS_TEAMTYPE
{
	IMS_TEAM_NONE = 0,		//none
	IMS_TEAM_PIC,			//Ⱥ��ͼƬ
	IMS_TEAM_FILE,			//Ⱥ���ļ�
};

typedef enum _KDXF_ACTIONTYPE
{
	KDXF_LOGIN = 0,		        //��¼
	KDXF_LOGOUT,			    //�ǳ�
	KDXF_ACCESS_CLASS,			//�������
	KDXF_QUIT_CLASS,            //�˳�����
	KDXF_START_BROADCAST,       //��ʼ�㲥
	KDXF_STOP_BROADCAST,        //ֹͣ�㲥
	KDXF_SWITCH_WINDOW,         //�л�����
	KDXF_ZOOM_IN,               //�Ŵ�
	KDXF_ZOOM_OUT,              //��С
};
//-------------------------------------
// ͨ���ṹ
typedef struct _MMS_CHANNEL
{
	unsigned int	channeltype;
	unsigned long	channelid;
	unsigned long	codetype;
	_MMS_CHANNEL()
	{
		channeltype=0;
		channelid=0;
		codetype=0;
	};
}MMS_CHANNEL,*PMMS_CHANNEL;

#include <list>
typedef std::list<PMMS_CHANNEL> MMS_CHANNEL_LST;

//---------------------------------------------------------
// NODEID MAP
//---------------------------------------------------------
#include <map>
//nodeid,index
typedef std::map<std::string,int> NODEID_MAP;

//=======================================================================================
// �������
//=======================================================================================
#define	ERR_NO							00000		//��ȷ

//---------------------------------------------------------------------------------------
// ϵͳ���������������
//---------------------------------------------------------------------------------------
#define	ERR_SYS_PROTOCOL				10000		//ͨѶЭ��汾��ƥ��
#define	ERR_SYS_UNKNOWN					10001		//δ֪����
#define	ERR_SYS_COMMAND					10002		//Э���������
#define	ERR_SYS_DATABASE				10003		//���ݿ����
#define	ERR_SYS_NETWORK					10004		//����Ͽ�(����ͨ·�Ͽ�)
#define	ERR_SYS_SERVERTYPE				10005		//���������ʹ���,��¼���������͵ķ�����
#define	ERR_SYS_LICEXPIRED              10006       //��������Ȩ����
//---------------------------------------------------------------------------------------
// ��ʱͨѶ��������
//---------------------------------------------------------------------------------------
#define	ERR_IMS_PROTOCOL				20000		//ͨѶЭ��汾��ƥ��
#define	ERR_IMS_INVALIDACCOUNT			20001		//��Ч���˻�(ϵͳû�и��˻�ID)
#define	ERR_IMS_INVALIDPASSWORD			20002		//��Ч������
#define	ERR_IMS_DUPLICATELOGIN			20003		//���˻��Ѿ���¼,�����ظ���¼
#define	ERR_IMS_REQUESTRELOGIN			20004		//������Ҫ��ʹ�û�õ��µ�ַ���µ�¼
#define	ERR_IMS_KICKOUT					20005		//���߳�(���˻��������ط��ѵ�¼,ǿ�Ʊ����ʺ��˳�)
#define ERR_IMS_MODULELIMIT				20006		//��������������(����˼�ʱͨѶģ����Ȩû������)
#define ERR_IMS_CONNECTLIMIT			20007		//���ӷ�������������û�������
#define ERR_IMS_ACCOUNTLIMIT			20008		//�û���¼��������û��ʺ�������
#define ERR_IMS_SERVER					20009		//�û��ʺ��ڸ÷������ϲ����ڻ�÷�����û�м����ʺ���֤ģ��
#define ERR_IMS_SERVERLIMIT				20010		//��������ַ�޶�,���û������ڴ˷������ϵ�¼
#define ERR_IMS_UNKNOWNAUTHTYPE			20011		//δ֪���ʻ���֤ģʽ(NOT:DB/LDAP/RADIUS)
#define ERR_IMS_FORBIDDEN				20012		//���˻�������
#define ERR_IMS_CLIENTAUTH				20013		//����ͻ�����֤
#define ERR_IMS_SERIALNOERROR			20014		//���кŰ󶨴���
#define ERR_IMS_INVALIDRECORD			20015		//��¼��������ʻ�����
#define ERR_IMS_ADDDISCUSSMEMBER		20016		//����������Ա����(������������)
#define ERR_IMS_WEBSERVICEDISCONNECT	20017		//WebService����ʧ��
#define ERR_IMS_OTHERSERVERDISCONNECT	20018		//����������������ʧ��
#define	ERR_IMS_INVALIDIDENTIFYID		20019		//�ն�ʶ�������
#define	ERR_IMS_3GLIMIT					20020		//3G�û���¼��������û��ʺ�������
#define	ERR_IMS_INVALIDTERM				20021		//���ն��ʻ����ʹ���
#define	ERR_IMS_AUTHFAILD				20022		//��֤ʧ��
#define ERR_IMS_PLATFORMLIMIT			20023		//������ƽ̨�޶�,���ն˲����ڴ˷������ϵ�¼


//---------------------------------------------------------------------------------------
// ����ͨѶ��������
//---------------------------------------------------------------------------------------
#define ERR_MMS_PROTOCOL				30000		//ͨѶЭ��汾��ƥ��
#define	ERR_MMS_INVALIDPASSWORD			30001		//��Ч�Ļ�������
#define	ERR_MMS_ALREADYLOGIN			30002		//���˻��Ѿ��������
#define	ERR_MMS_WORKGROUPLIMITS			30003		//�����Ѿ�������������
#define	ERR_MMS_NOMCU					30004		//����û���ʵ���MCU������
#define	ERR_MMS_OWNERLEAVED				30005		//�����������뿪����
#define	ERR_MMS_ROOMLOCKED				30006		//�����Ѿ�����
#define	ERR_MMS_INVALIDROOMTYPE			30007		//���������Ͳ���ȷ
#define	ERR_MMS_INVALIDMEMBERTYPE		30008		//��¼�����Ͳ���ȷ
#define	ERR_MMS_LOSSINVALID				30009		//���𱨰��Ų�����
#define	ERR_MMS_LOSSAGAIN				30010		//���𱨰����ѱ������˵Ǽ�
#define	ERR_MMS_LOSSHAVEEND				30011		//���𱨰����Ѿ��������
#define ERR_MMS_MODULELIMIT				30012		//��������������(ϵͳ����ģ����Ȩû������)
#define ERR_MMS_CONNECTLIMIT			30013		//ϵͳ�����������Ա������
#define ERR_MMS_HAVETV					30014		//����ǽ�豸���Ѵ���
#define ERR_MMS_INVALIDTV				30015		//����Ч�ĵ���ǽ�豸��
#define ERR_MMS_HAVETVMATRIX			30016		//����ǽ�����豸���Ѵ���
#define ERR_MMS_INVALIDTVMATRIX			30017		//����Ч�ĵ���ǽ�����豸��
#define ERR_MMS_SERVER					30018		//����������Ͽ�����
#define ERR_MMS_AUDITORLIMIT			30019		//���������������������
#define ERR_MMS_NONCONSOLE				30020		//���ն˲��ǻ������̨����
#define ERR_MMS_TOUCHSCREENLIMIT		30021		//���Ӱװ���Ȩ����
#define ERR_MMS_GROUP					30022		//�������������
#define ERR_MMS_SYNCPARAMETER			30023       //����ͬ��ʱ�Ķ�������δ�ҵ����������Ļ������
#define ERR_MMS_DUPLICATEMPS			30024       //�Ѿ�������ͬ���͵�MPS
#define ERR_MMS_UNAVAILABLE				30025       //����������MPS����
#define ERR_MMS_NOAVAILABLEMPS			30026       //û�п��е�MPS
#define ERR_MMS_APPLYMEMORYFAILED		30027       //�����ڴ�ʧ��
#define ERR_MMS_INVITEMPSSUCCESS		30028       //����MPS�ɹ�
#define ERR_MMS_LOGINSTOPEDROOM			30029		//���黹û���ٿ�
#define ERR_MMS_NOAVAILABLEROOM         30030       //û�к��ʵĻ�����
#define ERR_MMS_ROLLCALLING				30031		//�������ڽ�����(ͬһ��������ͬʱ��ֻ�ܷ���һ�ε�������)
#define ERR_MMS_VOTEING					30032		//ͶƱ������(ͬһ��������ͬʱ��ֻ�ܷ���һ��ͶƱ����)
#define ERR_MMS_HANDLING				30033		//���ֽ�����(ͬһ��������ͬʱ��ֻ�ܷ���һ�ξ��ֲ���)
#define ERR_MMS_LIMITEONECONSOLE		30034		//����������һ���������������
#define ERR_MMS_DIGITALCODETIMEOUT		30035		//���ֹ��ܽ����������ѹ���
#define ERR_MMS_SETTEMERR				30036		//ģ�����ô���
#define ERR_MMS_SETSWICHAUTTEMERR		30037		//�ֶ��л��Զ�ģ��ʱ��㲥��ͻ�Ĵ���
//#define ERR_MMS_SETAUTTEMERR			30038		//�Զ��л��ֶ����ֶ��л��Զ�ģ�����
#define ERR_MMS_RRGOUPRUNNING			30039		//�˻�������ѭ����������ѭ
#define	ERR_MMS_STARTRRFAILED			30040		//��ʼ��ѭʧ��
#define ERR_MMS_STOPRRFAILED			30041		//ֹͣ��ѭʧ��
#define ERR_MMS_NORRGOUPRUNNING			30042		//�˻�������ѭ����������ѭ

#define ERR_MMS_TRSCHLEXIST				30043		//�������ת��ͨ���Ѿ���ת����
#define ERR_MMS_NOAVAILABLETRSERVER		30044		//û�п��е�ת�������
#define ERR_MMS_TRSNOTFINDCHLINF		30045		//�������ת��ͨ����Ϣ����(δ����,�����lid�����)
//---------------------------------------------------------------------------------------
// ������������
//---------------------------------------------------------------------------------------
#define ERR_SMS_PROTOCOL				40000		//ͨѶЭ��汾��ƥ��
#define	ERR_SMS_GATEWAY					40001		//�������ط���ʧ��
#define	ERR_SMS_INVALIDSEARIALNO		40002		//���к���Ϊ�ջ���Ч
#define	ERR_SMS_NETWORK					40003		//���ӵ��������ط����������
#define	ERR_SMS_OTHERS					40004		//���ӵ��������ط�����������
#define	ERR_SMS_ENTRYLIMITS				40005		//�ֻ�����Ϊ�ջ��߳���1000��
#define	ERR_SMS_SPLITCHAR				40006		//�ֻ�����ָ���Ų���ȷ
#define	ERR_SMS_ERRORNUMBER				40007		//�����ֻ����벻��ȷ����ɾ���������ֻ����뱻����
#define	ERR_SMS_CONTENTOVERFLOW			40008		//��������Ϊ�ջ򳬳�(70������)
#define	ERR_SMS_BILLING					40009		//�������ؼƷ�ʧ��
#define	ERR_SMS_UNKNOWN					40010		//δ֪����
#define ERR_SMS_MODULELIMIT				40011		//��������������(ϵͳ����ģ����Ȩû������)
#define ERR_SMS_INVALIDTYPE				40012		//��������δ����
#define ERR_SMS_EMPTYCODE				40013		//���ͺŻ���պ�Ϊ��

//---------------------------------------------------------------------------------------
// �������������
//---------------------------------------------------------------------------------------
#define ERR_MON_PROTOCOL				50000		//ͨѶЭ��汾��ƥ��
#define ERR_MON_REUSESERIAL				50001		//���к��ѱ������ʻ���
#define	ERR_MON_INVALIDACCOUNT			50002		//��Ч�ļ���豸(������)�˻�(ϵͳû�и��˻�ID)
#define	ERR_MON_INVALIDPASSWORD			50003		//��Ч������
#define	ERR_MON_INVALIDSERIAL			50004		//��Ч�����к�
#define	ERR_MON_ALREADYLOGIN			50005		//�Ѿ���¼
#define ERR_MON_INVALIDTYPE				50006		//�Ƿ����ն�����
#define ERR_MON_MODULELIMIT				50007		//��������������(ϵͳ���ģ����Ȩû������)
#define ERR_MON_CONNECTLIMIT			50008		//ϵͳ�������ͨ��������
#define ERR_MON_MGWCOUNTLIMIT			50009		//�����豸�������������
#define ERR_MON_MOBCOUNTLIMIT			50010		//�ƶ��豸�������������

//---------------------------------------------------------------------------------------
// �����ļ���������
//---------------------------------------------------------------------------------------
#define ERR_NDS_PROTOCOL				60000		//ͨѶЭ��汾��ƥ��
#define ERR_NDS_MCUDISKFULL				60001		//MCU������
#define ERR_NDS_RECEIVERSPACEFULL		60002		//�����߿ռ���
#define ERR_NDS_MD5		                60003		//MD5��֤ʧ��
#define ERR_NDS_FILENOTEXIST		    60004		//�ļ�������
#define ERR_NDS_FILEISUPLOADING		    60005		//�ļ������ϴ���
#define ERR_NDS_FILEISDOWNLOADING		60006		//�ļ�����������
#define ERR_NDS_NETERROR                60007       //�������
#define ERR_NDS_DELFAILED				60008		//�ļ�ɾ��ʧ��

//----------------------------------------------------------------------------------------
// ¼��������������
//----------------------------------------------------------------------------------------
#define ERR_REC_NOSERVER				70000		//û�п��õ�¼�������
#define ERR_REC_SVRRES_BUSY				70001		//¼��������æ
#define ERR_REC_SVR_EXCEPTION			70002		//¼���������쳣
#define ERR_REC_SERVER_INVALIDTYPE		70003		//��Ч���˻�����
#define ERR_REC_SERVER_CANNOT_STOP		70004		//�޷�ֹͣ¼��
#define ERR_REC_STOP					70005		//ֹͣ¼��
#define ERR_REC_CUT						70006		//�и���Ƶ
#define ERR_REC_NO_CONF					70007		//����û��¼��
#define ERR_REC_CONF_CUT				70008		//����¼��ָ�
#define ERR_REC_CUT_FINISH				70009		//�и���Ƶ���(��Ҫ��Ե�¼)
#define ERR_REC_SEVER_UNKNOWN			70499		//δ֪����(����)

//---------------------------------------------------------------------------------------
// ת����������
//---------------------------------------------------------------------------------------
#define ERR_TRS_COUNT_LIMIT				70501		//ͨѶЭ��汾��ƥ��

//----------------------------------------------------------------------------------------
// ����ҵ��������
//----------------------------------------------------------------------------------------
#define ERR_CTS_MEDIC_ROOMNONEXIST		80001		//�޴�������
#define ERR_CTS_MEDIC_UNBINDFAILED		80002		//�����ҽ����ʧ��
#define ERR_CTS_MEDIC_BINDFAILED		80003		//�����Ұ�ʧ��
#define ERR_CTS_MEDIC_PATIENTADDED		80004		//�����Ѿ����
#define ERR_CTS_MEDIC_ADDPATIENTFAILED	80005		//��Ӳ�����Ϣʧ��
#define ERR_CTS_MEDIC_PATIENTNOTEXIST	80006		//������Ϣ������
#define ERR_CTS_MEDIC_NORECORD			80007		//û�в�ѯ��������¼
