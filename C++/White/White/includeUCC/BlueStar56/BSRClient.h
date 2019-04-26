/********************************************************************
*	Copyright (C) 2008 - BlueStar. All Rights Reserved
*
*	Date Created:2008-01-01
*	Filename	:BSRClient.h
*	Author		:BlueStar luoyanhua
*	Description	:BlueStar DVRClient SDK interface functions declare
*********************************************************************/
#ifndef __BLUESTAR_BSRCLIENT_INTERFACE_H__
#define __BLUESTAR_BSRCLIENT_INTERFACE_H__

#include <windows.h>

#ifdef __cplusplus
#define BSRCLIENT_API extern "C"
#else
#define BSRCLIENT_API
#endif

#ifndef BSRCLIENT_STDCALL
#define BSRCLIENT_STDCALL __stdcall
#endif

#ifndef BSCP_ENUM_VALUE

//BSCP������
typedef enum
{
	//Э�������(1-31)===================>
	BSCPERR_BSCP_NONE				= 0,		//����ִ�гɹ��޴���
	BSCPERR_BSCP_UNKNOW_CMDTYPE,				//�޷�ʶ�����������
	BSCPERR_BSCP_UNKNOW_CMDSUBTYPE,				//�޷�ʶ�������������
	BSCPERR_BSCP_UNKNOW_CMDOPT,					//�޷�ʶ��������������
	BSCPERR_BSCP_UNKNOW_CMDCODE,				//�޷�ʶ���������
	BSCPERR_BSCP_UNSUPPORT_CMD,					//��֧�ֵ�����
	BSCPERR_BSCP_CMD_DENIED,					//���߱�����ִ��Ȩ��
	BSCPERR_BSCP_CMD_UNLOGIN,					//δ��¼��ִ�и�����
	BSCPERR_BSCP_CMD_LOCKED,					//�������Ѽ���
	BSCPERR_BSCP_CMD_TIMEOUT,					//������ʱ
	BSCPERR_BSCP_POINTER_NULL,					//����ָ��Ϊ��
	BSCPERR_BSCP_INPUTDATA_ERROR,				//��������ݴ���
	BSCPERR_BSCP_CMD_INPUTPARAM,				//�����������(��Ч)
	BSCPERR_BSCP_CMD_INPUTLEN,					//�������ݳ��ȴ���
	BSCPERR_BSCP_CMD_SYSREBOOT,					//ϵͳ��Ҫ��������
	BSCPERR_BSCP_CMD_MAXTRANS,					//�ѳ��������ͨ����
	BSCPERR_BSCP_CMD_CREATETRANS,				//��������ͨ������
	BSCPERR_BSCP_CMD_VERUNSUPPORT,				//�汾��֧�ָ�����
	BSCPERR_BSCP_CMD_DEVUNSUPPORT,				//���豸��֧�ָ�����
	BSCPERR_BSCP_CMD_TRANSNOTEXIST,				//����ͨ��δ����
	BSCPERR_BSCP_CMD_RC5ERR,					//RC5�����������

   //<===================Э�������

   //=====================�¼Ӵ�����
   BSR_ERR_CMD_RECORDWITHD1,              //21.����ʹ��D1����¼�񣬲��ܴ���������CHQ*/
   BSR_ERR_CMD_WAITRECORDWITHD1,          ///*22.����ʹ��D1����Ԥ¼�����ܴ���������CHQ*/
   BSR_ERR_CMD_TRANSWITHD1,               ///*23.����ʹ��D1���ʽ������������䣬���ܴ���������changqing*/
   BSR_ERR_CMD_IDELWITHD1,                ///*24.����ʹ��D1���ʽ��п��б��룬���ܴ���������changqing*/
   BSR_ERR_CMD_TRANS_GOINGON,             ///*25.���ڽ������������䣬�����޸�����������*/

   //�¼Ӵ��������
	

	BSCPERR_BSCP_CMD_INNER			= 32,		//�ڲ�������ʼ���

	//ϵͳ���������(33-79)===================>
	BSCPERR_SYS_TIME_RECORDBACKUP,				//¼����߱���״̬�²�������ʱ��
	BSCPERR_SYS_TIME_MODEERR,					//��Чʱ�����ʱ���ʽ����
	BSCPERR_SYS_SYSBUSYING,					    //ϵͳæ
	//<===================ϵͳ���������

	//�û����������(80-119)===================>
	BSCPERR_USR_INVLADNAME			= 80,		//��Ч���û���
	BSCPERR_USR_INVLADKEY,						//��Ч������
	BSCPERR_USR_LOGININVLAD,					//�û���,������֤��Ч
	BSCPERR_USR_LOGINMAXNUM,					//�ѳ�������û���¼��
	BSCPERR_USR_LOGINLOGINED,					//���û��Ѿ���¼
	BSCPERR_USR_LOGOUTPARAM,					//ע����������
	BSCPERR_USR_INVALDUSRID,					//��Ч���û����
	BSCPERR_USR_LOGOUTED,						//��ǰ�Ѵ��ڵǳ�״̬
	BSCPERR_USR_KICKSELFERR,					//�����߳��Լ�
	BSCPERR_USR_DELADMIN,						//�����޸Ļ���ɾ����������Ա
	BSCPERR_USR_DELCURUSR,						//����ɾ�����޸ĵ�ǰ��¼�û�
	BSCPERR_USR_OVERID,							//��������û����
	BSCPERR_USR_EXIST,							//���û��Ѵ���
	BSCPERR_USR_NOTEXIST,						//���û�������
	BSCPERR_USR_FULL,							//�û�����
	BSCPERR_USR_NOTADMIN,						//û�в���Ȩ��
	//<===================�û����������

	//��־���������(120-159)===================>
	BSCPERR_LOG_NOTEXIST			= 120,		//��־������
	//<===================��־���������

	//¼��/��ʾ/���������(160-199)===================>
	BSCPERR_REC_NOTSETPARA			= 160,		//¼��״̬�������ò���
	BSCPERR_REC_NOTSETPATROL			= 161,		//�طźͶ໭�治��������Ѳ
	//<===================¼��/��ʾ/���������

	//�澯���������(200-239)===================>
	//<===================�澯���������

	//���̴洢��ع��������(240-279)===================>
	BSCPERR_DISK_NOTEXIST			= 240,		//Ӳ�̲�����
	BSCPERR_DISK_OVERNUMBER,					//����¼�����Ӳ����
	BSCPERR_DISK_SLEEPERR,						//����ʧ��
	BSCPERR_DISK_WAKEERR,						//����ʧ��
	BSCPERR_DISK_FORMATERR,						//Ӳ�̸�ʽ������
	BSCPERR_DISK_RECORDMODE,					//¼���̵�ģʽ���ô���
	BSCPERR_DISK_BACKUPRECORDPLAY,				//�ڲ��ű���¼���ʱ���ܶ�Ӳ�̽�������
	BSCPERR_DISK_NOTCHANAGERECORDDISK,			//���ܸ��ĵ�ǰ¼���̵�״̬
	BSCPERR_DISK_NOTCHANAGEBACKUPDISK,			//���ܸ��ĵ�ǰ���ݵ�״̬
	BSCPERR_DISK_DISKMODENUMBERERROR,			//Ӳ��״ֵ̬���벻�Ϸ�
	BSCPERR_DISK_DISKMODEERROR,					//Ӳ�̵�״̬����
	BSCPERR_DISK_DISKISBACKUPDISK,				//�����̲�����Ϊ¼����
	BSCPERR_DISK_SPEEDERR,						//Ӳ���ٶȲ���ʧ��
	BSCPERR_DISK_WRITEERR,						//Ӳ��д����ʧ��
	BSCPERR_DISK_GETTIMERBACKUPERR,				//��ȡ��ʱ����ʧ��
	BSCPERR_DISK_SETTIMERBACKUPERR,				//���ö�ʱ����ʧ��
	BSCPERR_DISK_BACKUPBUSY,					//���ݷ�æ,����ʧ��
	BSCPERR_DISK_NOTFORMAT,						//����û�и�ʽ��
	BSCPERR_DISK_TIMERBACKUP_TIMEEER,			//��ʱ����ʱ�����ô���
	BSCPERR_DISK_TIME_MODEERR,					//ʱ���ʽ���ò���
	BSCPERR_DISK_TIME_STARTBIGEND,				//��ʼʱ����ڽ���ʱ��
	BSCPERR_DISK_CHANNEL_SETERR,				//ͨ�����ô���
	BSCPERR_DISK_FIELD_STARTBIGEND,				//�ļ���ʼID���ڽ���ID
	BSCPERR_DEBUG_TASKINFOERR,					//��ȡ������Ϣʧ��
	BSCPERR_DEBUG_GETDPERR,						//��ȡDP��Ϣʧ��
	BSCPERR_DEBUG_REBOOTTASKERR,				//��������ʧ��	
	BSCPERR_CTRL_SETDVRCTLERR,					//����DVRCTRLʧ��	
	BSCPERR_DISK_OVERCHANNEL,					//����¼�����ͨ����
	BSCPERR_DISK_QUALITYERR,					//���ʴ���
	BSCPERR_USR_MODIPASS,						//��ǰ�û�ֻ���޸�����
	//<===================���̴洢��ع��������

	//������ع��������(280-319)===================>
	BSCPERR_NET_SENDEMAILL			= 280,		//�ʼ�����ʧ��
	//<===================������ع��������

	//���ݴ�����������(320-359)===================>
	BSCPERR_TRANS_RTPCALL			= 320,		//����RTP���� 
	BSCPERR_TRANS_BIGERENDTIME,					//ʱ���ѯʱ,��ʼʱ����ڽ���ʱ��
	BSCPERR_TRANS_BIGERNOWTIME,					//ʱ���ѯʱ,��ʼʱ����ڵ�ǰʱ��
	BSCPERR_TRANS_RECORDTYPE,					//¼�����ʹ���
	BSCPERR_TRANS_CHANNEL,						//ͨ������
	BSCPERR_TRANS_PAGEERR,						//��ҳ��ѯ����ҳ����
	BSCPERR_TRANS_NODISK,						//���̲�����
	BSCPERR_TRANS_NORECORD,						//������¼���
	BSCPERR_TRANS_CHANNELERR,					//ͨ������
	BSCPERR_TRANS_DISKINFOERR,					//������Ϣ����
	BSCPERR_TRANS_BEFOREFILERES,				//=====
	BSCPERR_TRANS_DATATYPE,						//�������ʹ���
	BSCPERR_TRANS_PROTOCOL,						//Э�����
	BSCPERR_TRANS_TIMEOUT,						//��ʱ
	BSCPERR_TRANS_STREAMOVERNUM,				//��Ƶ������
	BSCPERR_TRANS_NO_PERMISSION,				//����������,���������
	BSCPERR_TRANS_ENCODER_FULL,					//����������,����������
	BSCPERR_TRANS_NULL_STREAM,					//����������,������
	BSCPERR_TRANS_ERROR,						//����������,�򿪴���
	BSR_ERR_TRANS_TAGSEARCH_FAIL,				//TAG����ʧ��
	BSR_ERR_TRANS_CHANVIEWERR,					//�ͻ�����ͼ�����޷�Ԥ����ͨ��

	//345
	//<===================���ݴ�����������

	//������ƹ��������(360-399)===================>
	BSCPERR_CTRL_UARTWRITEFPERR		= 360,		//======
	BSCPERR_CTRL_INPUTKEYERR,					//�����ֵʧ��
	BSCPERR_CTRL_SENDKEYERR,					//���ͼ�ֵʧ��
	BSCPERR_CTRL_GETDVRCTLERR,					//��ȡDVRCTRLʧ��
	BSCPERR_CTRL_1604EERR,						//����1604E����
	BSCPERR_CTRL_4004ERR,						//����4004E����
	//<===================������ƹ��������

	//��CPU���ƴ�����(400-439)===================>
	//<===================��CPU���ƴ�����

	//���Դ�����(440-999)===================>
	//<===================���Դ�����

	//���ش�����(1000-1300)===================>
	BSCPERR_SDK_NOINIT 				= 1000,		//û�г�ʼ��
	BSCPERR_SDK_REINIT,							//�ظ���ʼ��
	BSCPERR_SDK_COPYRIGHT,						//��Ȩ����,δ����Ȩ�Ĵ���汾
	BSCPERR_SDK_RELOGIN,						//��ǰ�����ظ���¼
	BSCPERR_SDK_SESSIONEXCEPT,					//��ǰ�Ự�����쳣�Ͽ�
	BSCPERR_SDK_ERRUSERHANDLE,					//��Ч�û����
	BSCPERR_SDK_ERRREALHANDLE,					//��ЧԤ�����
	BSCPERR_SDK_ERRDOWNHANDLE,					//��Ч���ؾ��
	BSCPERR_SDK_ERRUPGRADEHANDLE,				//��Ч�������
	BSCPERR_SDK_ERRVOICEHANDLE,					//��Ч�Խ����
	BSCPERR_SDK_ERRALARMHANDLE,					//��Ч�������
	BSCPERR_SDK_ERRLOGFINDHANDLE,				//��Ч��־��ѯ���
	BSCPERR_SDK_ERRFILEFINDHANDLE,				//��Ч�ļ���ѯ���
	BSCPERR_SDK_ERRWINDOWHANDLE,				//��Ч�Ĵ��ھ��
	BSCPERR_SDK_ALLOCRES,						//��Դ�������
	BSCPERR_SDK_CREATETHREAD,					//�̴߳���ʧ��
	BSCPERR_SDK_INITSOCK,						//SOCKET��ʼ������
	BSCPERR_SDK_CREATESOCKET,					//����SOCKET����
	BSCPERR_SDK_SETSOCKET,						//����SOCKET����
	BSCPERR_SDK_SOCKETLISTEN,					//SOCKET����ʧ��
	BSCPERR_SDK_SOCKETCLOSE,					//SOCKET���ر�
	BSCPERR_SDK_ALARMON,						//�ѿ�����������
	BSCPERR_SDK_ALARMOFF,						//δ������������
	BSCPERR_SDK_PATHERROR,						//·������
	BSCPERR_SDK_CREATEFILE,						//�����ļ�����
	BSCPERR_SDK_READFILE,						//��ȡ�ļ�����

	BSCPERR_SDK_CONNECT				= 1050,		//���ӷ�����ʧ��
	BSCPERR_SDK_SENDDATA,						//���������������ʧ��
	BSCPERR_SDK_RECVDATA,						//�ӷ�������������ʧ��
	BSCPERR_SDK_RECVTIMEOUT,					//�ӷ������������ݳ�ʱ
	BSCPERR_SDK_CMDTIMEOUT,						//����ִ�г�ʱ
	BSCPERR_SDK_NOENOUGHBUF,					//������̫С
	BSCPERR_SDK_ERRORDATA,						//���͵���������(У��ʧ��)

	BSCPERR_SDK_PLAYERERR			= 1100,		//�������д���
	BSCPERR_SDK_OPENSOUND,						//��������ʧ��
	BSCPERR_SDK_AUDIOMODE,						//����ģʽ����
	BSCPERR_SDK_DDRAWNOSUPPORT,					//�����Կ���֧��
	//<===================���ش�����(1000-1300)

	BSCPERR_VALIDFLAG,							//��Ч�������ʶ
	BSCPERR_UNKOWN_ERROR			= 0xFFFF	//δ֪����
}BSRCLIENT_ERRCODE;

//DVR�豸����
typedef enum
{
	DVRTYPE_6001	= 6001,		//��·
	DVRTYPE_6003	= 6003,		//ATM(����3·ͬʱ����,2/4ͨ������,�ӳ���������)
	DVRTYPE_6004	= 6004,		//4·(�п�����ATM��4·)
	DVRTYPE_6008	= 6008,		//8·
	DVRTYPE_6012	= 6012,		//12·
	DVRTYPE_6016	= 6016,		//16·
	DVRTYPE_UNKNOWN	= 6255		//δ֪����
}BSRCLIENT_DVRTYPE;

//�쳣��ϢWPARAMֵ
typedef enum
{
	EXCEPTMSG_CONNECT		= 0x8001,	//�Ự�����쳣,		LPARAMΪhUserHandle
	EXCEPTMSG_SENDDATA		= 0x8002,	//���ݷ����쳣,		LPARAMΪhUserHandle
	EXCEPTMSG_ALARM			= 0x8003,	//�����쳣,			LPARAMΪhUserHandle

	EXCEPTMSG_PREVIEW		= 0x8011,	//����Ԥ���쳣,		LPARAMΪhRealHandle
	EXCEPTMSG_VOICE			= 0x8012,	//�����Խ��쳣,		LPARAMΪhRealHandle
	EXCEPTMSG_PREVALARMINT	= 0x8013,	//����Ԥ��������ͨ���ж�(���ATM),LPARAMΪhRealHandle

	EXCEPTMSG_DOWNLOAD		= 0x8021,	//���������쳣,		LPARAMΪhDownHandle
	EXCEPTMSG_NETPLAYBACK	= 0x8022,	//����ط��쳣,		LPARAMΪhPlayHandle

	EXCEPTMSG_DOWNFINISH	= 0x8031,	//�����������,		LPARAMΪhDownHandle
	EXCEPTMSG_PLAYFINISH	= 0x8032,	//����ط����,		LPARAMΪhPlayHandle

	EXCEPTMSG_UPGRADERROR	= 0x8041,	//���������쳣,		LPARAMΪhUpgradeHandle
	EXCEPTMSG_UPGRADEFINISH	= 0x8042	//�����������,		LPARAMΪhUpgradeHandle
}BSRCLIENT_EXCEPTMSG;

//��ֵ����
typedef enum
{
	MAX_LOGIN_NUM				= 5,		//���ͻ��˵�¼��
	MAX_USERS_NUM				= 10,		//����û���
	MAX_STREAM_NUM				= 18,		//���������
	MAX_CHAN_NUM				= 16,		//���ģ��ͨ����
	MAX_DISK_NUM				= 8,		//���Ӳ����
	MAX_ONEPAGEFILE_SIZE		= 50,		//��ҳ����ļ�����
	MAX_ONEPAGELOG_SIZE			= 50,		//��ҳ�����־����
	MAX_CRUISEDOT_NUM 			= 99,		//���Ѳ�������
	MAX_CHECKPOINTS_NUM			= 16,		//�����Ե����
	MAX_ALARMIN_NUM				= 16,		//��󱨾�������
	MAX_STRINGLEN				= 16,		//����ַ�������
	MAX_ALARMUPLOAD				= 512,		//��󱨾��ϴ�·��
	MOTION_DETECTSEC_NUM		= 4,		//�ƶ�֡����ʱ����
	MULTI_DETECT_NUM			= 4,		//���������

	MAX_CHANNUM_ATM				= 4,		//ATM�����ͨ����
	MAX_DISKNUM_ATM				= 4,		//ATM����������
  FOUR_DISK_MAX   = 4,
  FOUR_CHAN_MAX   = 4,
}BSRCLIENT_MAXVALUEDEF;

//����˿�
typedef enum
{
	BSTP_SERVICE_DEFTRANSPORT	= 3720,		//Ĭ���ֳ���Ƶ����˿�
	BSCP_SERVICE_DEFCTRLPORT	= 3721,		//Ĭ��������ƶ˿�
	BSTP_SERVICE_DEFDATAPORT	= 3722,		//Ĭ�����ݴ���˿�(����������)
	BSCP_SERVICE_DEFALARMPORT	= 2134,		//Ĭ�ϱ��������˿�
	BSCP_SERVICE_DEFAUDIOPORT	= 8000,		//�ɶԽ����ƵĶԽ��˿�
	BSTP_SERVICE_NEWAUDIOPORT	= 3720,		//�¶Խ����ƵĶԽ��˿�(�°汾�����ֳ�����ͳһ)
}BSRCLIENT_DEVICEPORT;

#endif

//��Чֵ����
typedef enum
{
	INVALID_STREAMID	= -2,				//��Ч��ID��
	INVALID_SESSIONID	= -2				//��ЧSESSION��
}BSRCLIENT_INVALIDVALUE;

//�豸��尴��
typedef enum
{
	BSRKEY_RECORD			= 1,		//��ʼ¼��(����1)
	BSRKEY_PLAY				= 2,		//��ʼ����(����2)
	BSRKEY_STOP				= 3,		//ֹͣ¼��/����(����3)
	BSRKEY_FASTBACK			= 4,		//����(����4)
	BSRKEY_FASTFORWARD		= 6,		//���(����5)
	BSRKEY_PAUSE			= 5,		//��ͣ(����6)
	BSRKEY_NEXTFRAME		= 8,		//��֡����(����7)
	BSRKEY_PREVIOUS			= 9,		//��һ��(����8)
	BSRKEY_NEXT				= 16,		//��һ��(����9)
	BSRKEY_TOP				= 7,		//�ص���¼�����(����0)
	BSRKEY_CLEARALARM		= 19,		//�������
	BSRKEY_CHANGEQUALITY	= 20,		//����ѡ��(+)
	BSRKEY_DISPLAY			= 21,		//��Ϣ��ʾ(-)

	BSRKEY_MENU				= 22,		//�˵�
	BSRKEY_LEFT				= 23,		//����-��
	BSRKEY_RIGHT			= 24,		//����-��
	BSRKEY_UP				= 25,		//����-��
	BSRKEY_DOWN				= 32,		//����-��

	BSRKEY_CHAN1			= 33,		//ͨ��1
	BSRKEY_CHAN2			= 34,		//ͨ��2
	BSRKEY_CHAN3			= 35,		//ͨ��3
	BSRKEY_CHAN4			= 36,		//ͨ��4
	BSRKEY_CHAN5			= 37,		//ͨ��5
	BSRKEY_CHAN6			= 38,		//ͨ��6
	BSRKEY_CHAN7			= 39,		//ͨ��7
	BSRKEY_CHAN8			= 40,		//ͨ��8
	BSRKEY_CHAN9			= 41,		//ͨ��9
	BSRKEY_CHAN10			= 42,		//ͨ��10
	BSRKEY_CHAN11			= 43,		//ͨ��11
	BSRKEY_CHAN12			= 44,		//ͨ��12
	BSRKEY_CHAN13			= 45,		//ͨ��13
	BSRKEY_CHAN14			= 46,		//ͨ��14
	BSRKEY_CHAN15			= 47,		//ͨ��15
	BSRKEY_CHAN16			= 48,		//ͨ��16

	BSRKEY_MULTISHOW		= 49,		//��ϻ���
	BSRKEY_TIMER			= 128,		//��ʱ
	BSRKEY_MOTION			= 129,		//�ƶ����
	BSRKEY_DEFENCE			= 130,		//��/����
	BSRKEY_PTZ				= 133,		//��̨����
	BSRKEY_KEYLOCK			= 135,		//������
}BSRCLIENT_KEYBOARD;

//���紫�䷽ʽ,���SDK��֧����ѡ��ʽ���Զ�ת��ΪTCPģʽ
typedef enum 
{
	PROTOCOL_TCPMODE		= 1,		//TCP��ʽ
	PROTOCOL_UNICAST		= 2,		//UDP����(�ݲ�֧��)
	PROTOCOL_MULTICAST		= 3,		//UDP�鲥(�ݲ�֧��)
	PROTOCOL_RTPMODE		= 4,		//RTP��ʽ
	PROTOCOL_RESERVED					//����
}BSRCLIENT_PROTOCOL;

//�ֳ���Ƶ���������(GET/SET)
typedef enum
{
	STREAMPARA_CHANSWITCH	= 1,		//ͨ���л�(1~16; 0-��ǰ�ֳ�; <0 ��ʾ�ֳ��л�����Ӧ�Ķ໭�����)
	STREAMPARA_LEISURE		= 2,		//����ʱ��,0Ϊ����ʱ(0~3600)
	STREAMPARA_DIAGKEEP		= 3,		//�Ự�������Ƿ񱣳�(0-������ 1-����)
	STREAMPARA_BANDWIDTH	= 4,		//����(�ֽ�/��)
	STREAMPARA_SWITCHSTREAM	= 5,		//0-������, 1-������[����ʱ�޲�,��0����]

	STREAMPARA_VCTRL		= 10,		//��Ƶ�Ĵ�/�ر�
	STREAMPARA_VFRAMERATE	= 11,		//��Ƶ֡��(PAL:1-25, NTSC:1-30)
	STREAMPARA_VBITRATE		= 12,		//��Ƶ����(64~4096 KBPS)
	STREAMPARA_VRESOLUTION	= 13,		//��Ƶ�ֱ���(0-CIF, 1-DCIF, 2-1/2D1, 3-2CIF 4-D1)

	STREAMPARA_LVBO			= 14,		//�˲�(0, 1, 2)
	STREAMPARA_IFINTERVAL	= 15,		//I֡���(25-200)
	STREAMPARA_LIANGHUA		= 16,		//����ֵ(2-31)

	STREAMPARA_MAKEKEYFRAME	= 20,		//����ǿ�����ɹؼ�֡[�޲�,��0����]
	STREAMPARA_BRIGHTNESS	= 21,		//��Ƶ����(0~255)
	STREAMPARA_CONTRAST		= 22,		//��Ƶ�Աȶ�(0~255)
	STREAMPARA_HUES			= 23,		//��Ƶɫ��(0~255)

	STREAMPARA_AUDIOCTRL	= 30,		//�����Ĵ�/�ر�(0-�ر� 1-��)
	STREAMPARA_AUDIOSAMPLE	= 31,		//�����Ĳ�����
	STREAMPARA_AUDIORATE	= 32,		//����������

	STREAMPARA_PTZCTRL		= 51,		//��̨PTZ����(��16λΪ����ֵBSRCLIENT_PTZCTRLPARABYLOCAL,��16λΪ0)
	STREAMPARA_PTZPRESET	= 52,		//��̨Ԥ��λ(1-n)
	STREAMPARA_PTZPATH		= 53,		//��̨Ѳ��·��(0-n, 0Ϊֹͣ)
	STREAMPARA_PTZCRUISE	= 54,		//��̨�켣 (0-1)
	STREAMPARA_PTZAUX		= 55,		//��̨����(1�����ƹ�,2ֹͣ�ƹ�,3������ˢ,4ֹͣ��ˢ...)
}BSRCLIENT_STREAMPARA;

//�����Խ����������(GET/SET)
typedef enum
{
	VOICEPARA_IDLETIME		= 2,		//����ʱ��,0Ϊ����ʱ(0~3600)
	VOICEPARA_DIAGKEEP		= 3,		//�Ƿ񱣳ֻỰ(0������,1����)
	VOICEPARA_BITRATE		= 4,		//����(BYTE/S),������ֻ��
	VOICEPARA_SAMPLE		= 5,		//������(KHZ),������ֻ�� 2(2k) | 4(4k) | 8(8k) | 16(16k) | 32(32k)
	VOICEPARA_ENCODE		= 6,		//�����ʽ,������ֻ�� 2(mp2) | 3(mp3) | 9(g.729)
	VOICEPARA_VIDEOON		= 10		//��Ƶ�Ĵ򿪹ر�,0�ر�,1��
}BSRCLIENT_VOICEPARA;

//�����ص���������
typedef enum
{
	REALDATATYPE_SYSHEADER		= 1,	//ϵͳ�ļ�ͷ
	REALDATATYPE_VOICEDATA		= 2,	//�Խ�������(����)
	REALDATATYPE_SCENEDATA		= 3,	//�ֳ�������
	REALDATATYPE_DOWNDATA		= 4,	//����������
}BSRCLIENT_REALDATATYPE;

//���ؽ��ȴ���ֵ
typedef enum
{
	DOWNERRFLAG_TIMEOUT		= 1051,		//���ճ�ʱ
	DOWNERRFLAG_SOCKERR		= 1052,		//SOCKET����
	DOWNERRFLAG_SOCKCLOSE	= 1053,		//SOCKET���Է�ǿ�ƹر�
	DOWNERRFLAG_UNKNOWN		= 1054		//δ֪����
}BSRCLIENT_DOWNERRFLAG;

//DVR��Ϣ�ص�����
typedef enum
{
	UPMSGTYPE_ALARMINFO	= 0x1000,		//�ϴ�������Ϣ
	UPMSGTYPE_TRADEINFO	= 0x2000		//ATMDVR�ϴ�������Ϣ(��δʵ��)
}BSRCLIENT_UPMSGTYPE;

//Զ�̻طſ���
typedef enum
{
	PLAYBACKCTRL_NORMAL			= 0x00,	//��������
	PLAYBACKCTRL_PAUSE,					//��ͣ����
	PLAYBACKCTRL_ONEBYONE,				//��֡����
	PLAYBACKCTRL_1OF2TIMES,				//1/2��(����)
	PLAYBACKCTRL_1OF4TIMES,				//1/4��(����)
	PLAYBACKCTRL_1OF8TIMES,				//1/8��(����)
	PLAYBACKCTRL_1OF16TIMES,			//1/16��(����)
	PLAYBACKCTRL_2TIMES,				//2����(���)
	PLAYBACKCTRL_4TIMES,				//4����(���)
	PLAYBACKCTRL_8TIMES,				//8����(���)
	PLAYBACKCTRL_16TIMES,				//16����(���)
	PLAYBACKCTRL_MAXSPEED,				//������
	PLAYBACKCTRL_IFRAME,				//I֡����
	PLAYBACKCTRL_OPENSOUND,				//������
	PLAYBACKCTRL_CLOSESOUND,			//�ر�����
	PLAYBACKCTRL_SETVOLUME,				//��������(����ֵ0-100֮��)
	PLAYBACKCTRL_SETPLAYPOS,			//�ı��ļ��طŵĽ���(ǧ�ֱ�0-1000)
	PLAYBACKCTRL_GETPLAYPOS,			//��ȡ�ļ��طŵĽ���(ǧ�ֱ�0-1000,-1Ϊ��Ч,>1000Ϊ�쳣,BSRCLIENT_DOWNERRFLAG)
	PLAYBACKCTRL_GETPLAYEDTIME,			//��ȡ��ǰ�Ѿ����ŵ�ʱ��
	PLAYBACKCTRL_GETTOTALTIME,			//��ȡ��ǰ���ŷ�Χ����ʱ�䳤��,��λ��
	PLAYBACKCTRL_GETPLAYEDFRAMES,		//��ȡ��ǰ�Ѿ����ŵ�֡��
	PLAYBACKCTRL_GETBAND,             //��ȡԶ�̲��Ŵ���
	PLAYBACKCTRL_SETBAND,             //����Զ�̲��Ŵ���
	PLAYBACKCTRL_INVALID		= 0xFF,	//��Ч������
}BSRCLIENT_PLAYBACKCTRL;

//�����¼����(���ر�������)
//�������,��Ƶ��ʧ,��Ƶ�ָ�,��Ƶ�ڵ�,�ƶ����,���̴���ȶ�����channel����Ϣ,��Ӧ���Ǿ����ͨ���Ż���̺�
typedef enum
{
	ALARM_EVENT_REPORT_IP		= 0x2112,	//����IP��ַ
	ALARM_EVENT_ALARM_OUT		= 0x2113,	//�������(�������ӵı�������)
	ALARM_EVENT_HDISK_ERROR		= 0x2114,	//���̴���
	ALARM_EVENT_VIDEO_LOST		= 0x2115,	//��Ƶ��ʧ
	ALARM_EVENT_EVERYTHING_OK	= 0x2116,	//��������
	ALARM_EVENT_VIDEO_RESTORE	= 0x2117,	//��Ƶ�ָ�
	ALARM_EVENT_CONTROL_1604E	= 0x2118,	//ר�ſ���1604E
	ALARM_EVENT_CONTROL_4000	= 0x2119,	//ר�ſ���4000ϵ��¼�������
	ALARM_EVENT_CTRL_4004		= 0x211A,	//ר�ſ���4000ϵ���ϵ���̨�豸
	ALARM_EVENT_VIDEO_COVER		= 0x211B,	//��Ƶ�ڵ�
	ALARM_EVENT_MOTION_DETECT	= 0x211C,	//�ƶ����
	ALARM_EVENT_NET_BREAK		= 0x211D,	//�����,6.0������,ͨ��ֵ=0��ʾ����,�����ʾ�ָ�����
	ALARM_EVENT_FUN_BREAK		= 0x211E,	//���ȴ���,6.0������,ͨ��ֵ=0��ʾ����,�����ʾ�ָ�����
	ALARM_EVENT_TIME_ERROR		= 0x211F,	//ʱ���,��ͨ���޹�,6.0������
	ALARM_EVENT_CHANNEL_ERROR	= 0x2120,	//ͨ����,��ͨ���޹�,6.0������
	ALARM_EVENT_DISK_FULL		= 0x2121,	//������,��ͨ���޹�,6.0������
	ALARM_EVENT_INVALID_LINK	= 0x2122,	//��Ч������,��ͨ���޹�,6.0������
	ALARM_EVENT_CHANNEL_CHANGE	= 0x2123,	//ATM����2/4ͨ���л�
	ALARM_EVENT_REQUEST_TALK	= 0x2124,	//DVR����Խ�
	ALARM_EVENT_CARDNO_ALM		= 0x2125,	//���ű���
	ALARM_EVENT_ALG_ALM			= 0x2126	//���ܱ���
}BSRCLIENT_NETALARMEVENT;

//Ѷ��ʵʱ���ص�����

typedef void (CALLBACK *fRealDataXM)( HANDLE hCallHandle, DWORD dwDataType, BYTE* lpHeadBuffer, DWORD dwHeaderSize, 
                                                                        BYTE*lpBuffer,  DWORD dwSize, LPVOID lpVoid );

//�����ص�(�ֳ�ʵʱ/Զ�̻ط�)
typedef void (CALLBACK *CLIENTDATACALLBACKFUN)(HANDLE hCallHandle, DWORD dwDataType, BYTE *lpBuffer, DWORD dwSize, LPVOID lpVoid);

//��ʾ���ھ��HOOK�ص�
typedef void (CALLBACK *CLIENTHWNDHOOKFUN)(HANDLE hRealHandle, HWND *hWnd, LPVOID lpVoid);

//�ַ����ӻص�
typedef void (CALLBACK *CLIENTTITLEDRAWFUN)(HANDLE hRealHandle, HDC hDC, DWORD dwWidth, DWORD dwHeight, LPVOID lpVoid);

//¼�����Ϣ�ص�[����������Ϣ]����(��IP�����豸)
typedef BOOL (CALLBACK *MESSAGECALLBACKFUN)(LONG nCommand, LPCSTR lpszDVRIP, BYTE *lpBuffer, DWORD dwSize, LPVOID lpVoid);

//��Ƶ���ݲɼ��ص�
typedef void (CALLBACK *VOICEDATACALLBACKFUN)(BYTE *lpDataBuffer, DWORD dwSize, DWORD dwType, LPVOID lpVoid);

//��Ƶ�ص�/ע����������
typedef enum
{
	BSRWAVETYPE_PCM		= 0,	//����ǰ��PCM����
	BSRWAVETYPE_ENCODE,			//���������
	BSRWAVETYPE_BOTH			//����(�˲�����������Ƶ�ɼ�ע�ắ��)
}BSRCLIENT_WAVEDATATYPE;

//��Ƶ�ɼ�����
typedef struct tagBSRCLIENT_WAVEPARA
{
	WORD wBits;					//λ
	WORD wChannels;				//����
	DWORD dwSamplesPerSec;		//������
}BSRCLIENT_WAVEPARA, *PBSRCLIENT_WAVEPARA;
//����1700ϵ��DVR��Ƶ�ɼ���ʽ����:
//WORD wBits = 16;
//WAVEFORMATEX wfx = {0};
//wfx.wFormatTag = WAVE_FORMAT_PCM;
//wfx.nChannels = 2;
//wfx.nSamplesPerSec = 16000;
//wfx.nAvgBytesPerSec = wfx.nChannels * wfx.nSamplesPerSec * wBits / 8;
//wfx.nBlockAlign = wBits * wfx.nChannels / 8;
//wfx.wBitsPerSample = wBits;
//wfx.cbSize = 0;

//��Ƶ�ɼ����ݽṹ
typedef struct tagBSRCLIENT_AUDIOWAVEIN
{
	BSRCLIENT_WAVEPARA *lpWFX;				//��Ƶ�ɼ���ʽָ��(ΪNULL��ʾʹ��Ĭ��ֵ)

	VOICEDATACALLBACKFUN lpfnNotEncode;		//����ǰ���ݻص�ָ��
	LPVOID lpVoidNotEncode;					//����ǰ���ݻص�������

	VOICEDATACALLBACKFUN lpfnEncoded;		//��������ݻص�ָ��
	LPVOID lpVoidEncoded;					//��������ݻص�������
}BSRCLIENT_AUDIOWAVEIN, *PBSRCLIENT_AUDIOWAVEIN;
//����1700ϵ��DVR��Ƶ�ɼ���ʽ����:
//WORD wBits = 16;
//WAVEFORMATEX wfx = {0};
//wfx.wFormatTag = WAVE_FORMAT_PCM;
//wfx.nChannels = 2;
//wfx.nSamplesPerSec = 16000;
//wfx.nAvgBytesPerSec = wfx.nChannels * wfx.nSamplesPerSec * wBits / 8;
//wfx.nBlockAlign = wBits * wfx.nChannels / 8;
//wfx.wBitsPerSample = wBits;
//wfx.cbSize = 0;

//DVRʱ�����ڸ�ʽ
typedef struct tagBSCP_DATATIME
{
	USHORT year;			//��
	USHORT month;			//��
	USHORT day;				//��
	USHORT hour;			//ʱ
	USHORT minute;			//��
	USHORT second;			//��
	USHORT week;			//��
	USHORT res;				//����
}BSCP_DATATIME, *PBSCP_DATATIME;

//�����ϴ��ص��ṹ
typedef struct tagBSRCLIENT_ALARMINFO
{
	ULONG nEvent;			//�����¼�,BSRCLIENT_NETALARMEVENT
	ULONG nChannel;			//ͨ����
	ULONG nSrcIP;			//������IP(������)
	ULONG nSerial;			//���к�(¼���������)
	ULONG nYear;			//��
	ULONG nMonth;			//��
	ULONG nDay;				//��
	ULONG nHour;			//ʱ
	ULONG nMinute;			//��
	ULONG nSecond;			//��
	CHAR szBuf[24];			//������Ϣ(����ǿ��ű����������ŵ��ǿ��ŵ�ASSCII��)
}BSRCLIENT_ALARMINFO, *PBSRCLIENT_ALARMINFO;

//��ǰ�������б���
typedef struct tagBSRCLIENT_STREAMLISTITEM
{
	LONG nStreamID;			//��ID
	LONG nSessionID;		//�ỰID
	LONG nChannel;			//ģ��ͨ��
	LONG nProtocol;			//Э�����
	CHAR szSrcIP[16];		//ԴIP
	LONG nSrcPort;			//ԴPORT
	CHAR szDstIP[16];		//Ŀ��IP
	LONG nDstPort;			//Ŀ��PORT
	LONG nStatus;			//��״̬UNUSED(0) | OPEN(1) | START(2) | STOP(3) | CLOSE(4)
	LONG nResolve;			//����
}BSRCLIENT_STREAMLISTITEM, *PBSRCLIENT_STREAMLISTITEM;

//��ǰ�������б�
typedef struct tagBSRCLIENT_STREAMLIST
{
	BSRCLIENT_STREAMLISTITEM item[MAX_STREAM_NUM];
}BSRCLIENT_STREAMLIST, *PBSRCLIENT_STREAMLIST;

//��ϵͳ��Ϣ
typedef struct tagBSCP_NETTRANS_STREMINFO
{
	INT streamId;			//����
	INT sessionid;			//�ỰID
	INT channel;			//ͨ����
	INT switchchannel;		//�л�ͨ����
	INT timeout;			//��ʱ
	INT Iskeep;				//�Ƿ񱣳�
	INT BandWidth;			//����
	INT bit_rate_type;      //��������0:������ 1:������ 3:�����ޱ�����
	INT frame_rate;			//֡��1-25/30
	INT bit_rate;			//��Ƶ���� 64-4096 KBPS
	INT filter;				//�˲�(��,1,2)
	INT Iframe_interval;	//I֡���25-200
	INT quantity;			//���� 2-31
	INT audio_bit_rate;		//��Ƶ����16,32,64 KBPS
	INT media_type;			//��������: 0: �����;1 :��Ƶ��
	INT video_mode;			//ѹ����Ƶ�Ĵ�С 0:CIF 1:DCIF 2:1/2D1 3: 2CIF 4:D1
	INT bright;				//����
	INT contrast;			//�Աȶ�
	INT sature;				//���Ͷ�
	INT states;				//��״̬0-unUsed 1-Open 2-start 3-stop 4-busy
	INT audio;				//0-�ر� 1-��
	INT presetno;			//
	INT cruise;				//
	INT pattern;			//
	INT aux;				//
}BSCP_NETTRANS_STREMINFO, *PBSCP_NETTRANS_STREMINFO;

//��ǰ��ϵͳ��Ϣ�б�
typedef struct tagBSRCLIENT_STREAMINFOS
{
	BSCP_NETTRANS_STREMINFO item[MAX_STREAM_NUM];
}BSRCLIENT_STREAMINFOS, *PBSRCLIENT_STREAMINFOS;

//�ļ��������(¼���ļ����)
typedef enum 
{
	RECORDFILETYPE_TIMER		= 0,		//��ʱ¼��
	RECORDFILETYPE_MOTODETEC	= 1,		//�ƶ����
	RECORDFILETYPE_ALARM		= 2,		//����¼��
	RECORDFILETYPE_MAMUAL		= 3,		//�ֶ�¼��
	RECORDFILETYPE_ALL			= 4,		//ȫ��¼��

	RECORDFILETYPE_CARDNO		= 5			//����¼��(3·ATM֧��,�������Ϊ�������ʶ��,������Ϊ��������)
}BSRCLIENT_RECORDFILETYPE;

//�ļ��������
typedef enum
{
	REMOTESEARCH_GENERAL	= 0,	//�������
	REMOTESEARCH_BYCARD,			//���ż���
	REMOTESEARCH_BYALG,				//���ܼ���
	REMOTESEARCH_INVALID			//��Ч��
}BSRCLIENT_FINDTYPEENUM;

//��չ���������ṹ
typedef struct tagBSRCLIENT_REMOTEFINDEX
{
	LONG nChannel;			//ͨ����(1-N,�ݲ�֧������ͨ��)
	DWORD dwSearchType;		//��ѯ���BSRCLIENT_FINDTYPEENUM
	DWORD dwMajorType;		//�����BSRCLIENT_RECORDFILETYPE
	DWORD dwMinorType;		//�����(���ܲ�ѯʱ,�������BSRALGDECT_ALGDECTTYPE)
	DWORD dwDiskNumber;		//���̺�(1-N,0��ʾ������)
	SYSTEMTIME tBegin;		//��ʼʱ��
	SYSTEMTIME tEnd;		//����ʱ��
	CHAR szCard[32];		//����(���Ų�ѯʱΪ�����ַ���,�����ѯʱ������Ч)
	LONG nPageCount;		//ҳ�����(ȫ�̼���ʱ������Ч)
	LONG nFileCount;		//�ļ������(ȫ�̼���ʱ������Ч)
}BSRCLIENT_REMOTEFINDEX, *PBSRCLIENT_REMOTEFINDEX;

//�ļ���ϸ��Ϣ
typedef struct tagBSCP_NETTRANS_FILEINFO
{
	CHAR name[64];			//�ļ���
	INT type;				//¼������(BSRCLIENT_RECORDFILETYPE)
	UINT time;				//��ʼʱ��(32λtime_t����)
	INT channel;			//����ͨ��1-16
	INT disk;				//��������1-32
	INT cluster;			//������
	UINT period;			//¼��ʱ��
	UINT64 size;			//�ļ���С(Bytes)
}BSCP_NETTRANS_FILEINFO, *PBSCP_NETTRANS_FILEINFO;

//�ļ��б�
typedef struct tagBSRCLIENT_FILELIST
{
	BSCP_NETTRANS_FILEINFO fileinfo[MAX_ONEPAGEFILE_SIZE];	//�ļ���
}BSRCLIENT_FILELIST, *PBSRCLIENT_FILELIST;

//�汾��ʶ
enum 
{
	B61X_TYPE = 0x80000000,//61X����
	B61X_SUBTYPE_SENSITIVE_AUTO = 0x00000001,//61X����ͨ���������Զ�
	B61X_SUBTYPE_OLNY_QCIF	= 0x00000002,//61X����ֻ֧��QCIF
	B61X_SUBTYPE_OLNY_32K = 0x00000004,//61X���Ͳ�֧��˫����
};
typedef struct tabBSRCLIENT_VERFLAG
{
	BOOL bNewAlarm;			//�±����ṹATMĿǰ������,�汾�ƻ�����V6.11֮��Ķ����µı����ṹ
	BOOL bNewAudio;			//�¶Խ�����,V6.10B08D01֮��Ķ����µĶԽ�����
	BOOL bSATAType;			//�Ƿ�SATA����
	BOOL bAlgType;			//�Ƿ�֧������
	BOOL bSubStream;		//�Ƿ�֧��˫����
	BOOL bMultiMask;		//�Ƿ���ڸ�����,V6.10B09�汾��ʼ֧��
	BOOL bArmyIA;       //�Ƿ��Ǿ���ר��DVR
	BOOL bB02IA;        //�Ƿ���B02ר��DVR   
	BOOL b650B02;       //6.50B02D14��֮��汾
	BOOL b61XB0X;       //6.11B01 |6.12B01 |6.12B02 | 6.13b01 | 6.14B01�汾
	BOOL b610B09;       //6.10B09
	BOOL bFourMask;     //"V6.14B01D07" "V6.11B01D22" �Լ�"V6.13B01D15"��֮��汾
	BOOL bConduct;		//6.31B01D12���������Ժ�֧�ִ�ܶ�Ź
  BOOL bIAV640M;
  BOOL bIA_V640;
  BOOL bIA_V640QJ;  //V6.40B01D16֮��汾֧��

	DWORD b61X;			//V6.11B01D33 V6.11B01D33NXX V6.13B01D31 V6.13B01D31NXX V6.14B01D16 V6.14B01D16NXX֧����Ƶ��СCIF QCIF��ѡ��ͨ�������ȿ�ѡ�Զ�

  BOOL bAtm61XX;      //����atm��֧�ֵ�Э��
  BOOL bAtm614X;      //����atm��֧�ֵ�Э��,614X֧�ֵ�Э���612��611����613�汾��һ��
  BOOL bAtm615X;

  BOOL bOutputValid;  //�жϱ������ʱ����Ƿ���Ч;

  BOOL bUseNewDevId;  //�ж��Ƿ�ʹ�����µ��豸��ʶ�ṹ

  BOOL bUseNewDDNS;  //6.30B02D09֮����������������˽���άDDNS����;

  BOOL bV64XX; //V6.4�汾��̨����
  BOOL bSupportQCIF; //
}BSRCLIENT_VERFLAG, *PBSRCLIENT_VERFLAG;

//�豸��Ϣ
typedef struct tagBSRCLIENT_DEVICEINFO
{
	ULONG hardwarever;			//Ӳ���汾��

	ULONG typeandmode;			//�豸����
	ULONG subType;				//�豸������

	ULONG fpStatus;				//¼���״̬
	ULONG quality;				//��ǰ����

	ULONG slaveCPUs;			//��CPU�ĸ���
	ULONG channumber;			//ͨ����
	ULONG videoNumber;			//��Ƶ�������
	ULONG maxDiskNumber;		//���Ӳ�̸���
	ULONG nicNumber;			//������Ŀ
	ULONG uartNumber;			//������Ŀ
	ULONG alarmNumber;			//����������(���ֶ�B09֮��汾��Ч)

	ULONG r_version;			//��_�汾��
	ULONG w_version;			//д_�汾��

	CHAR sysver[40];			//ϵͳ�汾��
	ULONG softwarever;			//����汾��

	ULONG quarter_status;		//��ǰ�໭��״̬

	SYSTEMTIME stTime;			//ϵͳʱ��

	BSRCLIENT_VERFLAG verFlag;	//����汾��ʶ
}BSRCLIENT_DEVICEINFO, *PBSRCLIENT_DEVICEINFO;

//¼���״̬
typedef struct tagBSCP_DVR_DEVICESTAT
{
	UINT fpStatus;							//
	INT netmode;							//
	INT alert;								//
	INT quality;							//����
	INT fs_mode;							//FS�ͺ�
	INT mode;								//
	BSCP_DATATIME net_DateTime;				//
}BSCP_DVR_DEVICESTAT, *PBSCP_DVR_DEVICESTAT;

//¼�����Ϣ
typedef struct tagBSCP_DVR_DEVICEINFO
{
	UCHAR ipaddr[4];						//
	UCHAR netmask[4];						//
	UINT fpStatus;							//
	INT netmode;							//
	INT alert;								//
	INT quality;							//
	INT fs_mode;							//
	INT mode;								//
	BSCP_DATATIME net_DateTime;				//

	ULONG r_ip;								//��ǰ���ӵ�IP��ַ
	INT delay_of_direct_sendto;				//delayֵ
	INT dqlen;								//Data_Qid�е�ǰ����Ϣ����
	INT dskqlen;							//writedisk_qid�е�ǰ����Ϣ����
	INT cqlen;								//Cmd_Qid�е�ǰ����Ϣ����
	INT misqlen;							//Misc_Qid�е�ǰ����Ϣ����
	INT ctrlqlen;							//msgQueue�е�ǰ����Ϣ����
	ULONG buildid;							//Master BuildID
	ULONG slaveId;							//Slave BuildID
	CHAR version[40];						//��ǰ����İ汾��
	CHAR startup_date[80];					//ϵͳ��������
	CHAR m_b_date[48];						//������ı�������
	CHAR m_s_date[48];						//Slave����ı�������
	USHORT uChannel;						//ͨ����Դ״̬
	UCHAR message_timeout;					//Message Timeout
	UCHAR current_disk;						//Current disk for Transfer by Net
	// -1 for HddInUseHandle
	//0 - 7 for disk A, B, C, D

	//ÿһ��bit��ʾһ��channel����Դ���
}BSCP_DVR_DEVICEINFO, *PBSCP_DVR_DEVICEINFO;

//�豸�����
typedef enum
{
	DEVSUBTYPE_UNDEFINE	= 0,	//δ����
	DEVSUBTYPE_RECORDER,		//¼���
	DEVSUBTYPE_1604NE,			//1604NE
	DEVSUBTYPE_DECODER,			//������
	DEVSUBTYPE_TRANSBOX			//�����
}BSRCLIENT_DEVSUBTYPE;

//¼���״̬(��λ��TRUE)
typedef enum
{
	DEVSTATUS_NETERROR		= 0x80000000,		//�������
	DEVSTATUS_FANERROR		= 0x40000000,		//���ȴ���

	DEVSTATUS_INTRANSFER	= 0x00200000,		//���ڴ���
	DEVSTATUS_TRANSFER		= 0x00100000,		//������������

	DEVSTATUS_WINDOWS		= 0x00080000,		//����״̬
	DEVSTATUS_HDDERROR		= 0x00040000,		//Ӳ�̴���
	DEVSTATUS_RECPLAY		= 0x00020000,		//
	DEVSTATUS_MOTION		= 0x00010000,		//�ƶ����

	DEVSTATUS_RECORD		= 0x00008000,		//¼��״̬
	DEVSTATUS_PLAYSTATUS	= 0x00004000,		//����״̬
	DEVSTATUS_STOP			= 0x00002000,		//�ֳ�״̬
	DEVSTATUS_PAUSE			= 0x00001000,		//��ͣ״̬

	DEVSTATUS_FBPLAY		= 0x00000800,		//����
	DEVSTATUS_FFPLAY		= 0x00000400,		//���
	DEVSTATUS_STEP			= 0x00000200,		//��֡
	DEVSTATUS_TOP			= 0x00000100,		//���ο�ʼ

	DEVSTATUS_ALARM			= 0x00000080,		//����¼��
	DEVSTATUS_BALARM		= 0x00000040,		//����ǰ
	DEVSTATUS_TIMER			= 0x00000020,		//��ʱ¼��
	DEVSTATUS_FULL			= 0x00000010,		//������

	DEVSTATUS_BACKUP		= 0x00000008,		//����
	DEVSTATUS_DIALOG		= 0x00000004		//�Ự
}BSRCLIENT_DEVSTATUS;

//�ֳ�Ԥ��/�����Խ�����
typedef struct tagBSRCLIENT_CLIENTINFO
{
	LONG lChannel;					//ͨ����(0-16,0Ϊ��ϻ���,��������ݻ��;������·��,-4/-14/-24/-9/-19/-29��ʾ��N��4�������,��N��9�������)
	BOOL bPullMode;					//ģʽTRUE-����, FALSE-����(ĿǰTCP��֧�ֱ���,RTP��֧������)
	UINT nProtocol;					//����Э������(BSRCLIENT_PROTOCOL)
	HWND hWnd;						//���Ŵ��ڵľ��(���ΪNULL��ʾ���������ݲ�����ͼ��,�����Խ�ʱҲ���Դ�NULL)
	DWORD dwDeviceNum;				//������ʾ�豸,0ΪĬ���豸(Ŀǰ��֧��Ĭ���豸)
	WORD wTransPort;				//DVR�˴���˿�(BSRCLIENT_DEVICEPORT)
	BOOL bSubStream;				//������ѡ��,TRUE������,FALSE������(����豸��֧����������˲���������)
	DWORD dwBufFrameNum;			//����֡��,0����Чֵ��ΪĬ��ֵ5֡,��Ч��Χ(2-50)
	WORD wLocalPort;				//���ض˿�(����������ģʽ,����ģʽ��0����)
	LPCSTR lpszLocalIP;				//����ģʽ����IP(����������ģʽ,����ģʽ��NULL����)
	CLIENTDATACALLBACKFUN lpfnReal;	//ʵʱ�����ص�(������ص�����NULL����)
	LPVOID lpRealVoid;				//�û��ص�������
}BSRCLIENT_CLIENTINFO, *PBSRCLIENT_CLIENTINFO;

//������������
typedef struct tagBSRCLIENT_SUBSTREAMPARAITEM
{
	UCHAR nVideoDIM;				//��Ƶѹ����ʽ,Ŀǰ�̶�Ϊ0-CIF
	UCHAR nBitrateType;				//��������,0-������,1-������,3-����������, Ŀǰ�̶�Ϊ3-����������
	UCHAR nFrameRate;				//֡��1-25,100��������խ������ ��102-1/2,103-1/3��������
	UCHAR nIFrameInterval;			//I֡���25-200
	UCHAR nFilter;                  //�˲�ֵ(0��, 1, 2), 3, 4, 5, 6, 7,���漸�������óɹ�����ʵ��Ч��
	UCHAR nQuality;					//��������,Ŀǰ�̶�Ϊ6
	UCHAR nMediaType;				//��������0-������,1-��Ƶ��
	UCHAR nQuantity;				//����ֵ2-31
	UCHAR nAudioBitrate;			//��Ƶ����0-32,1-64(KBPS)
	UCHAR nRes;						//����λ
	USHORT nVideoBitrate;			//��Ƶ����64-1024(KBPS),DVR��Ĭ��ֵΪ384
}BSRCLIENT_SUBSTREAMPARAITEM, *PBSRCLIENT_SUBSTREAMPARAITEM;

//������������ȡ/����(�ɶ�ĳһͨ����������/��ȡ,Ҳ���Զ�ָ��ͨ��һ��������/��ȡ)
typedef struct tagBSRCLIENT_SUBSTREAMPARA
{
	BSRCLIENT_SUBSTREAMPARAITEM subPara[MAX_CHAN_NUM+1];
	ULONG  nChannel;				//����ͨ��,��λ����(�ӵ͵���1Ϊ������,2-17Ϊ1-16ͨ��)
}BSRCLIENT_SUBSTREAMPARA, *PBSRCLIENT_SUBSTREAMPARA;


//�벶׽�������ӷ�ʽ
typedef enum
{
	CATCHERLINKTYPE_RS232		= 1,
	CATCHERLINKTYPE_RS485		= 2,
	CATCHERLINKTYPE_RS232HUB	= 3,
	CATCHERLINKTYPE_RJ45		= 4
}BSRCLIENT_CARDCATCHERLINKTYPE;

//����Э��
typedef enum
{
	//��׽�����ӷ�ʽΪRJ45ʱ
	CARDPROTOCOL_RJ45_NONE		= 0,	//��
	CARDPROTOCOL_RJ45_XINJIANG	= 1,	//�½�
	CARDPROTOCOL_RJ45_CHENGDU	= 2,	//�ɶ�
	CARDPROTOCOL_RJ45_OTHER		= 3,	//����
	CARDPROTOCOL_RJ45_YINDU		= 4,	//ӡ��
	CARDPROTOCOL_RJ45_NINGXIA	= 5,	//����
	CARDPROTOCOL_RJ45_GELUNBIYA	= 6,	//���ױ���
	CARDPROTOCOL_RJ45_SHANGHAI	= 7,	//�Ϻ�

	//��׽�����ӷ�ʽΪRS232ʱ
	CARDPROTOCOL_RS232_NONE		= 0,	//��
	CARDPROTOCOL_RS232_GDYT		= 1,	//�����ͨ
	CARDPROTOCOL_RS232_RILI		= 2,	//����
	CARDPROTOCOL_RS232_GELUNBIYA= 3,	//���ױ���
	CARDPROTOCOL_RS232_YINDU	= 4,	//ӡ��
	CARDPROTOCOL_RS232_NCR		= 5,	//NCR
	CARDPROTOCOL_RS232_LANTIAN	= 6,	//����
}BSRCLIENT_CARDPROTOCOL;

//ATM���Ų�׽������
typedef struct tagBSRCLIENT_CARDCATCHERPARA
{
	UCHAR nLinkType;				//�벶�������ӷ�ʽBSRCLIENT_CARDCATCHERLINKTYPE
	UCHAR nProtocol;				//����Э��BSRCLIENT_CARDPROTOCOL
	UCHAR nForwards;				//����ת������:0-��,1-����
	UCHAR res;						//����
	UINT nAddr;						//��������ַIP��ַ(�����ֽ���)��RS485��ַ
	UINT nPeriod;					//����ʱ��(¼��ʱ��,��λ��)
}BSRCLIENT_CARDCATCHERPARA, *PBSRCLIENT_CARDCATCHERPARA;

//����������
typedef struct tagBSRCLIENT_ACTIVATORPARA
{
	CHAR nLink;						//��������:1-��������
	CHAR nPort;						//�˿ں�:1-4
	CHAR bEnabled;					//�����״̬:0-����,1-����
	CHAR res;						//����λ
}BSRCLIENT_ACTIVATORPARA, *PBSRCLIENT_ACTIVATORPARA;

//���ڴ��������ȡ����������õ�����
typedef struct tagBSRCLIENT_UARTAGENTPARA
{
	CHAR nlink;						//��������:1-RS232,3-RS485
	CHAR port;						//�˿ں�:1-4
	CHAR bEnabled;					//�����״̬:0-����,1-����
	CHAR nProtocol;					//Э��:1-�ڱ���
}BSRCLIENT_UARTAGENTPARA, *PBSRCLIENT_UARTAGENTPARA;

//�豸�˲�����ȡ/��������
typedef enum
{
	CONFIGCMD_GETSYSGENERAL		= 101,	//��ȡϵͳ�������BSCP_DVR_SYSCFG_GENERAL
	CONFIGCMD_SETSYSGENERAL,			//����ϵͳ�������BSCP_DVR_SYSCFG_GENERAL
	CONFIGCMD_GETSYSDEVID,				//��ȡϵͳ�豸��ʶ����BSCP_DVR_PAGEDEVID
	CONFIGCMD_SETSYSDEVID,				//����ϵͳ�豸��ʶ����BSCP_DVR_PAGEDEVID

  CONFIGCMD_GETSYSDEVIDEX,				//��ȡϵͳ�豸��ʶ����BSCP_DVR_PAGEDEVIDEX
  CONFIGCMD_SETSYSDEVIDEX,				//����ϵͳ�豸��ʶ����BSCP_DVR_PAGEDEVIDEX

	CONFIGCMD_GETDEVICETIME,			//��ȡ�豸��ǰʱ��BSCP_DATATIME
	CONFIGCMD_SETDEVICETIME,			//�����豸��ǰʱ��BSCP_DATATIME

	CONFIGCMD_GETTIMERBAKINFO,			//��ȡ��ʱ������ϢBSCP_DISK_TIMERBACKUP
	CONFIGCMD_SETTIMERBAKINFO,			//���ö�ʱ������ϢBSCP_DISK_TIMERBACKUP

	CONFIGCMD_GETLOGCONFIGIP,			//��ȡ��־����IP BSCP_LOG_NETCFGIP
	CONFIGCMD_SETLOGCONFIGIP,			//������־����IP BSCP_LOG_NETCFGIP

	CONFIGCMD_GETRTPCONFIGINFO,			//��ȡRTP������ϢBSRCLIENT_RTPCONFIGINFO
	CONFIGCMD_SETRTPCONFIGINFO,			//����RTP������ϢBSRCLIENT_RTPCONFIGINFO

	CONFIGCMD_GETNETWORKPARAS,			//��ȡ�������ò���BSCP_NETWORK_NETCFGINFO
	CONFIGCMD_SETNETWORKPARAS,			//�����������ò���BSCP_NETWORK_NETCFGINFO

	CONFIGCMD_GETYTCTRLPARA,			//��ȡ��̨���Ʋ���BSCP_CONTROL_CTRLPARA_LIST
	CONFIGCMD_SETYTCTRLPARA,			//������̨���Ʋ���BSCP_CONTROL_CTRLPARA_LIST

	CONFIGCMD_GETDVRHARDINFOS,			//��ȡDVRӲ����ϢBSCP_DVR_GETHARDINFO
	CONFIGCMD_SETDVRHARDINFOS,			//����DVRӲ����ϢBSCP_DVR_GETHARDINFO

	CONFIGCMD_GETDVRTIMEOUT,			//��ȡDVR�������ʱʱ��BSCP_NETWORK_TIMEOUT
	CONFIGCMD_SETDVRTIMEOUT,			//����DVR�������ʱʱ��BSCP_NETWORK_TIMEOUT

	CONFIGCMD_GETRECGENERAL,			//��ȡ¼�񳣹����BSCP_RECORD_PAGEGENERAL
	CONFIGCMD_SETRECGENERAL,			//����¼�񳣹����BSCP_RECORD_PAGEGENERAL
	CONFIGCMD_GETRECADVANCE,			//��ȡ¼��߼�����BSCP_RECORD_PAGEADVANCE
	CONFIGCMD_SETRECADVANCE,			//����¼��߼�����BSCP_RECORD_PAGEADVANCE

	CONFIGCMD_GETTIMERREC,				//��ȡ��ʱ¼�����BSCP_RECORD_PAGETIMER
	CONFIGCMD_SETTIMERREC,				//���ö�ʱ¼�����BSCP_RECORD_PAGETIMER

	CONFIGCMD_GETMOTION,				//��ȡ�ƶ�������BSCP_RECORD_PAGEMOTION
	CONFIGCMD_SETMOTION,				//�����ƶ�������BSCP_RECORD_PAGEMOTION

	CONFIGCMD_GETSCENECHAN,				//��ȡ�ֳ�ͨ������BSCP_RECORD_PAGESCENCE
	CONFIGCMD_SETSCENECHAN,				//�����ֳ�ͨ������BSCP_RECORD_PAGESCENCE
	CONFIGCMD_GETSCENEPATROL,			//��ȡ�ֳ���Ѳ����BSCP_RECORD_AUTOPATROL
	CONFIGCMD_SETSCENEPATROL,			//�����ֳ���Ѳ����BSCP_RECORD_AUTOPATROL

	//FOR OLD==>
	CONFIGCMD_GETALMDEFENCE,			//��ȡ��������������BSCP_ALARM_PAGEDEFENCE
	CONFIGCMD_SETALMDEFENCE,			//���ñ�������������BSCP_ALARM_PAGEDEFENCE
	CONFIGCMD_GETALMALARMIN,			//��ȡ�����������BSCP_ALARM_PAGEALARMIN
	CONFIGCMD_SETALMALARMIN,			//���ñ����������BSCP_ALARM_PAGEALARMIN
	CONFIGCMD_GETALMRECORD,				//��ȡ����¼�����BSCP_ALARM_PAGERECORD
	CONFIGCMD_SETALMRECORD,				//���ñ���¼�����BSCP_ALARM_PAGERECORD
	CONFIGCMD_GETALMEXCEPT,				//��ȡ�����쳣����BSCP_ALARM_PAGEEXCEPT
	CONFIGCMD_SETALMEXCEPT,				//���ñ����쳣����BSCP_ALARM_PAGEEXCEPT
	CONFIGCMD_GETALMPRERECORD,			//��ȡ����Ԥ¼����BSCP_ALARM_PAGEPRERECORD
	CONFIGCMD_SETALMPRERECORD,			//���ñ���Ԥ¼����BSCP_ALARM_PAGEPRERECORD
	//FOR OLD<==

	//FOR ATM OR NEW==>
	CONFIGCMD_GETALARMZONE_NEW,			//��ȡ������������BSRCLIENT_ALARMZONESET
	CONFIGCMD_SETALARMZONE_NEW,			//���ñ�����������BSRCLIENT_ALARMZONESET
	CONFIGCMD_GETALARMRECORD_NEW,		//��ȡ����¼������BSRCLIENT_ALARMRECORD_NEW
	CONFIGCMD_SETALARMRECORD_NEW,		//���ñ���¼������BSRCLIENT_ALARMRECORD_NEW
	CONFIGCMD_GETALARMALMOUT_NEW,		//��ȡ�����������BSRCLIENT_ALARMOUT_PARA
	CONFIGCMD_SETALARMALMOUT_NEW,		//���ñ����������BSRCLIENT_ALARMOUT_PARA
	//FOR ATM OR NEW<==

	//FOR ATMר��==>
	CONFIGCMD_GETUARTSETDATA_ATM,		//ATM���������û�ȡBSRCLIENT_ATMCTRLPARA_GROUP
	CONFIGCMD_SETUARTSETDATA_ATM,		//ATM��������������BSRCLIENT_ATMCTRLPARA_GROUP
	//FFOR ATMר��<==

	CONFIGCMD_GETSUBSTREAMPARA,			//��ȡ���������ò���BSRCLIENT_SUBSTREAMPARA
	CONFIGCMD_SETSUBSTREAMPARA,			//�������������ò���BSRCLIENT_SUBSTREAMPARA

	//��֧��V6.10B09�汾==>
	CONFIGCMD_GETSCENECHANEX,			//��ȡ�ֳ�ͨ������BSCP_RECORD_PAGESCENCEEX
	CONFIGCMD_SETSCENECHANEX,			//�����ֳ�ͨ������BSCP_RECORD_PAGESCENCEEX

	CONFIGCMD_GETALMEXCEPTEX,			//��ȡ�����쳣����BSCP_ALARM_PAGEEXCEPTEX
	CONFIGCMD_SETALMEXCEPTEX,			//��ȡ�����쳣����BSCP_ALARM_PAGEEXCEPTEX
	//��֧��V6.10B09�汾<==

	//��֧�ִ�������Ϣ�Ļ���==>
	CONFIGCMD_GETALGDECTVIDEOPARA,		//��ȡ��Ƶ���ܲ�����ϢBSRCLIENT_ALGDECTVIDEOPARA
	CONFIGCMD_SETALGDECTVIDEOPARA,		//������Ƶ���ܲ�����ϢBSRCLIENT_ALGDECTVIDEOPARA
	CONFIGCMD_GETALGDECTAUDIOPARA,		//��ȡ��Ƶ���ܲ�����ϢBSRCLIENT_ALGDECTAUDIOPARA
	CONFIGCMD_SETALGDECTAUDIOPARA,		//������Ƶ���ܲ�����ϢBSRCLIENT_ALGDECTAUDIOPARA

	//�°汾����DVR��������
	CONFIGCMD_GETALGDECTVIDEOPARA_NEW,		//��ȡ��Ƶ���ܲ�����ϢBSRCLIENT_ALGDECTVIDEOPARA
	CONFIGCMD_SETALGDECTVIDEOPARA_NEW,		//������Ƶ���ܲ�����ϢBSRCLIENT_ALGDECTVIDEOPARA

	CONFIGCMD_GETALARMZONE_ALG,			//��ȡ������������ALGBSRCLIENT_ALARMZONESET
	CONFIGCMD_SETALARMZONE_ALG,			//���ñ�����������ALGBSRCLIENT_ALARMZONESET
	CONFIGCMD_GETALARMRECORD_ALG,		//��ȡ����¼������ALGBSRCLIENT_ALARMRECORD
	CONFIGCMD_SETALARMRECORD_ALG,		//���ñ���¼������ALGBSRCLIENT_ALARMRECORD
	CONFIGCMD_GETALARMALMOUT_ALG,		//��ȡ�����������ALGBSRCLIENT_ALARMOUT_PARA
	CONFIGCMD_SETALARMALMOUT_ALG,		//���ñ����������ALGBSRCLIENT_ALARMOUT_PARA
	//��֧�ִ�������Ϣ�Ļ���<==

	//��֧��6004AMN
	CONFIGCMD_GETUARTHUBPARA,			//��ȡ����HUB��������BSRCLIENT_UARTHUBPARA
	CONFIGCMD_SETUARTHUBPARA,			//���ô���HUB��������BSRCLIENT_UARTHUBPARA

	//���翨��
	CONFIGCMD_SETCARDPROTOCOL,			//�������翨��Э��BSRCLIENT_NETCARDPROTOCOL
	CONFIGCMD_GETCARDPROTOCOL,			//��ȡ���翨��Э��BSRCLIENT_NETCARDPROTOCOL

	//ATM���Ų�׽�����
	CONFIGCMD_GETCARDCATCHER,			//��ȡATM���Ų�׽������BSRCLIENT_CARDCATCHERPARA
	CONFIGCMD_SETCARDCATCHER,			//����ATM���Ų�׽������BSRCLIENT_CARDCATCHERPARA
	CONFIGCMD_GETACTIVATOR,				//��ȡ����������BSRCLIENT_ACTIVATORPARA
	CONFIGCMD_SETACTIVATOR,				//���ü���������BSRCLIENT_ACTIVATORPARA

	//DVSר������
	CONFIGCMD_DVSGETALARMINZONE,		//��ȡ1644���������������BSCPDVS_ALARMZONEGROUP
	CONFIGCMD_DVSSETALARMINZONE,		//����1644���������������BSCPDVS_ALARMZONEGROUP
	CONFIGCMD_DVSGETALARMOUTRELATE,		//��ȡ1644���������������BSCPDVS_ALARMOUTGROUP
	CONFIGCMD_DVSSETALARMOUTRELATE,		//����1644���������������BSCPDVS_ALARMOUTGROUP
	CONFIGCMD_DVSGETSCENESTATUS,		//��ȡ1644�ֳ��Զ��л�״̬BSRCLIENT_DVS_SCENESWITCHGROUP
	CONFIGCMD_DVSSETSCENESTATUS,		//����1644�ֳ��Զ��л�״̬BSRCLIENT_DVS_SCENESWITCHGROUP
	CONFIGCMD_DVSGETDEVCONFIG,			//��ȡ1644Ӳ��������ϢBSRCLIENT_DVS_DEVCONFIGGROUP
	CONFIGCMD_DVSSETDEVCONFIG,			//����1644Ӳ��������ϢBSRCLIENT_DVS_DEVCONFIGGROUP
	CONFIGCMD_DVSGETNETCTLSTATUS,		//��ȡ1644�������״̬BSRCLIENT_DVS_NETCTLSTATUS
	CONFIGCMD_DVSSETNETCTLSTATUS,		//����1644�������״̬BSRCLIENT_DVS_NETCTLSTATUS

	CONFIGCMD_DVSGET2NDNICANDNVR,		//����104/1644�ڶ�������������¼�������BSRCLIENT_DVS_NICGROUP
	CONFIGCMD_DVSSET2NDNICANDNVR,		//��ȡ104/1644�ڶ�������������¼�������BSRCLIENT_DVS_NICGROUP

	//��Ѷר������
	CONFIGCMD_GETSECBUREAUSTATUS,       //��ȡע��״̬
	CONFIGCMD_GETSECBUREAUCFG,          //��ȡע�����
	CONFIGCMD_SETSECBUREAUCFG,          //����ע�����

	//��Ѷ������

  //���翨������
  CONFIGCMD_GETNETCARDCFG,
  CONFIGCMD_SETNETCARDCFG,


  //����������

  CONFIGCMD_GETNETDETECT,
  CONFIGCMD_SETNETDETECT,

  CONFIGCMD_GETREGISTEREXP, //��ȡ����ע���������
  CONFIGCMD_SETREGISTEREXP, //��������ע���������

  CONFIGCMD_GETREGISTERPARA, //��ȡ����ע�����
  CONFIGCMD_SETREGISTERPARA, //��������ע�����

  CONFIGCMD_GETREGISTERSTANDBY, //��ȡ�������
  CONFIGCMD_SETREGISTERSTANDBY, //�����������

  CONFIGCMD_GETREGISTERSTAT, //��ȡ����ע��״̬

  CONFIGCMD_GETCHNWARNREPORTSTAT, //��ȡͨ�������ϴ���Ϣ
  CONFIGCMD_SETCHNWARNREPORTSTAT, //����ͨ�������ϴ���Ϣ

  CONFIGCMD_MANUALREGISTER, //�ֶ�ע�� 
  CONFIGCMD_MANUALLOGOUT, //�ֶ�ע��

  CONFIGCMD_GETREGISTERAGENT, //��ȡ�ڱ���Э����Ϣ
  CONFIGCMD_SETREGISTERAGENT, //�����ڱ���Э����Ϣ
  
  //����atm���ܻ�ȫ��
  CONFIGCMD_GETATMSCENEOBJ,
  CONFIGCMD_SETATMSCENEOBJ,

}BSRCLIENT_CONFIGCMD;

//�û���ṹ
typedef struct tagBSCP_USER_GROUP
{
	UCHAR UserId;				//�û�id 0~9
	UCHAR grade;				//Ȩ�� 0:��ͨ�û� 1:����Ա 2:����Ա 3:��������Ա
	USHORT resv;				//����
	INT dialogId;				//�Ựsession
}BSCP_USER_GROUP, *PBSCP_USER_GROUP;

//��ǰ��½�û�
typedef struct tagBSCP_USER_CURLOGINUSR 
{
	BSCP_USER_GROUP usrLogin[MAX_LOGIN_NUM];	//
}BSCP_USER_CURLOGINUSR, *PBSCP_USER_CURLOGINUSR;

//�û���ϸ��Ϣ
typedef struct tagBSCP_USER_DETAIL
{
	CHAR username[16];			//�û��� ,���15λ
	CHAR userpass[8];			//���� ,���6λ
	UCHAR uid;					//�û�id 0~9
	UCHAR grade;				//Ȩ�� 0:��ͨ�û� 1:����Ա 2:����Ա 3:��������Ա
	USHORT backup;				//
}BSCP_USER_DETAIL, *PBSCP_USER_DETAIL;

//��ʾ��ǰ�����û�
typedef struct tagBSCP_USER_ALLUSER
{
	BSCP_USER_DETAIL usrAll[MAX_USERS_NUM];	//
}BSCP_USER_ALLUSER, *PBSCP_USER_ALLUSER;

//�Ự��ϸ��Ϣ
typedef struct tagBSRCLIENT_DIALOGDETAIL
{
	LONG stat;				//�Ự״̬
	LONG sock;				//socketid
	LONG sessionid;			//�Ựsessionid
	BOOL blogin;			//�Ƿ��¼
	CHAR szClientIP[16];	//�ͻ���IP��ַ
	LONG netport;			//�ͻ��˶˿�
	LONG uid;				//��¼�û�id
	LONG ugrade;			//��¼�û�Ȩ��
	ULONG idletime;			//����ʱ��
	SYSTEMTIME logintime;	//��¼ʱ��
}BSRCLIENT_DIALOGDETAIL, *PBSRCLIENT_DIALOGDETAIL;

//����ͨ����ϸ��Ϣ
typedef struct tagBSCP_DEBUG_TRANSCHANINFO
{
	INT stat;			//����״̬
	INT sock;			//socketid
	INT trans_id;		//����id
	INT sess_sid;		//��������ĻỰid
	INT sessSockid;		//��������ĻỰsocketid
	INT rcv_timeout;	//���ճ�ʱʱ��
	INT snd_timeout;	//���ͳ�ʱʱ��
	INT resv;			//����1
	UINT64 snd_num;		//���͵��ֽ���
	UINT64 rcv_num;		//���յ����ֽ���
	INT conn_time;		//����ʱ��
	INT resv1;			//����2
}BSCP_DEBUG_TRANSCHANINFO, *PBSCP_DEBUG_TRANSCHANINFO;

//������Ϣ��
typedef struct tagBSRCLIENT_TRANSCHANINFO
{
	BSCP_DEBUG_TRANSCHANINFO TransInfo[MAX_LOGIN_NUM];
}BSRCLIENT_TRANSCHANINFO, *PBSRCLIENT_TRANSCHANINFO;

//��ǰ���лỰ��Ϣ
typedef struct tagBSRCLIENT_DIALOGINFOS
{
	BSRCLIENT_DIALOGDETAIL dialog[MAX_LOGIN_NUM];
}BSRCLIENT_DIALOGINFOS, *PBSRCLIENT_DIALOGINFOS;

//��ǰʵ����Чͨ��
typedef struct tagBSRCLIENT_ACTUALCHANS
{
	BOOL bValid[32];
}BSRCLIENT_ACTUALCHANS, *PBSRCLIENT_ACTUALCHANS;

//ϵͳ��ǰ������Ϣ�ṹ
typedef struct tagBSRCLIENT_DVRLOADINFO
{
	CHAR product[32];		//��Ʒ����
	CHAR version[32];		//��Ʒ�汾 
	CHAR buildtime[32];		//����ʱ�� 
	UINT RealTime;			//ϵͳʱ��(32λtime_t����)
	UINT Runningtime;		//����ʱ��(32λtime_t����)
	INT tasksNum;			//��������
	INT cpuLoad;			//CPU����(%) 
	INT memUsage;			//�ڴ�ʹ��(%)  
}BSRCLIENT_DVRLOADINFO, *PBSRCLIENT_DVRLOADINFO;

//ͨ��״̬
typedef struct tagBSRCLIENT_CHANSTATUS
{
	CHAR type;				//¼������:��(0)|�ֹ�(1)|��ʱ(2)|���(4)|����(8)[λ��]
	CHAR codec;				//¼������ʽ:1(mpeg4)|3(h263)|4(h264)
	CHAR resolution;		//�ֱ���:PAL DCIF(1)|CIF(2)|1/2D1(3)|D1(4) NTSC+128
	CHAR flags;				//bit0:�Ƿ�����Ԥ¼,bit1:�Ƿ��а���
	CHAR videoFormat;		//��Ƶ����
	CHAR audioFormat;		//��Ƶ����
	SHORT res;				//����
	UINT firstClipTime;		//����¼��ʱ��(32λtime_t����)
	UINT lastClipTime;		//����¼��ʱ��(32λtime_t����),�������¼����Ϊϵͳ��ǰʱ��
}BSRCLIENT_CHANSTATUS, *PBSRCLIENT_CHANSTATUS;

//����״̬
typedef struct tagBSRCLIENT_DISKSTATUS
{
	INT status;				//��ǰ״̬,����(0)|�(1)
	UINT totalSpace;		//����������KB(���̼��㷽ʽ1000����)
	UINT freeSpace;			//����ʣ������KB(���̼��㷽ʽ1000����)
	UINT startTime;			//����¼���ʱ��(32λtime_t����)
	UINT endTime;			//����¼���ʱ��(32λtime_t����),�������¼����Ϊϵͳ��ǰʱ��
}BSRCLIENT_DISKSTATUS, *PBSRCLIENT_DISKSTATUS;

//ϵͳ��ǰ����״̬��Ϣ�ṹ(B09����)
typedef struct tagBSRCLIENT_DVRRUNINFO
{
	INT serial;				//��Ʒ�������к�
	CHAR version[16];		//�豸�汾��Ϣ
	UINT localtime;			//�豸ϵͳʱ��(32λtime_t����)
	UINT uptime;			//�豸��������ʼ�������ʱ��
	UCHAR load;				//��ǰcpu����
	UCHAR streams;			//��ǰ��Ƶ����Ŀ
	UCHAR channels;			//ʵ��ͨ����
	UCHAR disks;			//���̸���
	BSRCLIENT_CHANSTATUS channStatus[MAX_CHAN_NUM];
	BSRCLIENT_DISKSTATUS diskStatus[MAX_DISK_NUM];   
}BSRCLIENT_DVRRUNINFO, *PBSRCLIENT_DVRRUNINFO;

//ϵͳ��ǰ����״̬��Ϣ�ṹ(4ͨ�� 4Ӳ��)
typedef struct tagBSRCLIENT_DVRRUNINFO_EX
{
	INT serial;				//��Ʒ�������к�
	CHAR version[16];		//�豸�汾��Ϣ
	UINT localtime;			//�豸ϵͳʱ��(32λtime_t����)
	UINT uptime;			//�豸��������ʼ�������ʱ��
	UCHAR load;				//��ǰcpu����
	UCHAR streams;			//��ǰ��Ƶ����Ŀ
	UCHAR channels;			//ʵ��ͨ����
	UCHAR disks;			//���̸���
	BSRCLIENT_CHANSTATUS channStatus[FOUR_CHAN_MAX];
	BSRCLIENT_DISKSTATUS diskStatus[FOUR_DISK_MAX];   
}BSRCLIENT_DVRRUNINFO_EX, *PBSRCLIENT_DVRRUNINFO_EX;

//����ȴ���ʱ
typedef struct tagBSCP_NETWORK_TIMEOUT
{
	INT nTimeout;			//��ʱʱ��
}BSCP_NETWORK_TIMEOUT, *PBSCP_NETWORK_TIMEOUT;

//��־��ѯ�����
typedef enum
{
	MAJOLOG_ALL		= 0,	//����
	MAJOLOG_OPERATE	= 1,	//����
	MAJOLOG_ALARM	= 2,	//����
	MAJOLOG_EXCEPT	= 3,	//�쳣
	MAJOLOG_ADJTIME	= 4		//ʱ�����
}BSCP_LOGEVENT_MAJOR;

//��־��ѯ�����
typedef enum
{
	MINOR_ALL		= 0		//����
}BSCP_LOGEVENT_MINOR;

//��־��ѯ�����¼�
typedef enum
{
	EVENTLOG_NULL		= 0,	//����

	EVENTLOG_ALARM1		= 1,	//����1
	EVENTLOG_ALARM2		= 2,	//����2
	EVENTLOG_ALARM3		= 3,	//����3
	EVENTLOG_ALARM4		= 4,	//����4
	EVENTLOG_ALARM5		= 5,	//����5
	EVENTLOG_ALARM6		= 6,	//����6
	EVENTLOG_ALARM7		= 7,	//����7
	EVENTLOG_ALARM8		= 8,	//����8
	EVENTLOG_ALARM9		= 9,	//����9
	EVENTLOG_ALARM10	= 10,	//����10
	EVENTLOG_ALARM11	= 11,	//����11
	EVENTLOG_ALARM12	= 12,	//����12
	EVENTLOG_ALARM13	= 13,	//����13
	EVENTLOG_ALARM14	= 14,	//����14
	EVENTLOG_ALARM15	= 15,	//����15
	EVENTLOG_ALARM16	= 16,	//����16

	EVENTLOG_MOTION		= 17,	//���¼��

	EVENTLOG_VLOST1		= 18,	//��Ƶ��ʧ1
	EVENTLOG_VLOST2		= 19,	//��Ƶ��ʧ2
	EVENTLOG_VLOST3		= 20,	//��Ƶ��ʧ3
	EVENTLOG_VLOST4		= 21,	//��Ƶ��ʧ4
	EVENTLOG_VLOST5		= 22,	//��Ƶ��ʧ5
	EVENTLOG_VLOST6		= 23,	//��Ƶ��ʧ6
	EVENTLOG_VLOST7		= 24,	//��Ƶ��ʧ7
	EVENTLOG_VLOST8		= 25,	//��Ƶ��ʧ8
	EVENTLOG_VLOST9		= 26,	//��Ƶ��ʧ9
	EVENTLOG_VLOST10	= 27,	//��Ƶ��ʧ10
	EVENTLOG_VLOST11	= 28,	//��Ƶ��ʧ11
	EVENTLOG_VLOST12	= 29,	//��Ƶ��ʧ12
	EVENTLOG_VLOST13	= 30,	//��Ƶ��ʧ13
	EVENTLOG_VLOST14	= 31,	//��Ƶ��ʧ14
	EVENTLOG_VLOST15	= 32,	//��Ƶ��ʧ15
	EVENTLOG_VLOST16	= 33,	//��Ƶ��ʧ16

	EVENTLOG_ENTERMENU		= 34,	//����˵�
	EVENTLOG_EXITMENU		= 35,	//�˳��˵�
	EVENTLOG_BEGINDEFENCE	= 36,	//����
	EVENTLOG_ENDDEFENCE		= 37,	//����
	EVENTLOG_DISKFORMAT		= 38,	//��ʽ��Ӳ��
	EVENTLOG_RESTOREDEF		= 39,	//��������
	EVENTLOG_SETDATETIME	= 40,	//ʱ����������
	EVENTLOG_SETRECPARA		= 41,	//¼���������
	EVENTLOG_SETVIDEOPARA	= 42,	//��Ƶ��������
	EVENTLOG_SETALARMREC	= 43,	//����¼������
	EVENTLOG_SETCOMMPARA	= 44,	//ͨ������
	EVENTLOG_SETTIMERREC	= 45,	//��ʱ¼������
	EVENTLOG_SETMOTION		= 46,	//�ƶ��������
	EVENTLOG_SETALARMPREREC	= 47,	//����¼������
	EVENTLOG_SETDISKMODE	= 48,	//Ӳ�̹���ģʽ����
	EVENTLOG_SETNETWORK		= 49,	//��������
	EVENTLOG_MANSTARTREC	= 50,	//�ֹ�����¼��
	EVENTLOG_MANSTOPREC		= 51,	//�ֹ�ֹͣ¼��
	EVENTLOG_SETSYSTEMPWD	= 52,	//ϵͳ��������
	EVENTLOG_UPGRADE		= 53,	//���������ɹ�
	EVENTLOG_FINDRECORD		= 54,	//¼�����
	EVENTLOG_BACKUPRECORD	= 55,	//¼�񱸷�
	EVENTLOG_LOCKKEYBOARD	= 56,	//��������
	EVENTLOG_UNLOCKKEYBOARD	= 57,	//���̽���
	EVENTLOG_OVERLAYCARDNO	= 58,	//���ŵ���

	EVENTLOG_POWERON		= 59,	//����
	EVENTLOG_SHUTDOWN		= 60,	//�ػ�
	EVENTLOG_LAWLESSSHUTDOWN= 61,	//�Ƿ��ػ�
	EVENTLOG_LOCALLOGIN		= 62,	//���ص�¼
	EVENTLOG_LOCALLOGOUT	= 63,	//����ע��
	EVENTLOG_LOCALCONFIG	= 64,	//��������
	EVENTLOG_LOCALPLAY		= 65,	//���ػط�
	EVENTLOG_LOCALPLAYBYTIME= 66,	//���ذ�ʱ��ط�
	EVENTLOG_LOCALSTARTREC	= 67,	//��������¼��
	EVENTLOG_LOCALSTOPREC	= 68,	//����ֹͣ¼��
	EVENTLOG_LOCALPTZCTRL	= 69,	//������̨����
	EVENTLOG_LOCALUPGRADE	= 70,	//��������
	EVENTLOG_LOCALBAKBYTIME	= 71,	//���ذ�ʱ�䱸��
	EVENTLOG_LOCALBAKBYFILE	= 72,	//���ذ��ļ�����
	EVENTLOG_BACKCANCEL		= 73,	//������ֹ
	EVENTLOG_BACKCOMPLITE	= 74,	//�������
	EVENTLOG_BAKDISKFULL	= 75,	//��������

	EVENTLOG_REMOTELOGIN			= 76,	//Զ�̵�¼
	EVENTLOG_REMOTELOGOUT			= 77,	//Զ��ע��
	EVENTLOG_REMOTESTARTREC			= 78,	//Զ������¼��
	EVENTLOG_REMOTESTOPREC			= 79,	//Զ��ֹͣ¼��
	EVENTLOG_CREATETRANSCHAN		= 80,	//����͸��ͨ��
	EVENTLOG_DISCONNTRANSCHAN		= 81,	//�Ͽ�͸��ͨ��
	EVENTLOG_REMOTEGETPARA			= 82,	//Զ�̻�ȡ����
	EVENTLOG_REMOTESETPARA			= 83,	//Զ������
	EVENTLOG_REMOTEGETSTAT			= 84,	//Զ�̻�ȡ״̬
	EVENTLOG_REMOTEBEGINDDEFENCE	= 85,	//Զ�̲���
	EVENTLOG_REMOTEENDDEFENCE		= 86,	//Զ�̳���
	EVENTLOG_REMOTEREBOOT			= 87,	//Զ������
	EVENTLOG_STARTVOICETALK			= 88,	//�����Խ���ʼ
	EVENTLOG_STOPVOICETALK			= 89,	//�����Խ�����
	EVENTLOG_REMOTEUPGRADE			= 90,	//Զ������
	EVENTLOG_REMOTEFILEPLAY			= 91,	//Զ���ļ��ط�
	EVENTLOG_REMOTEPLAYBYTIME		= 92,	//Զ�̰�ʱ��ط�
	EVENTLOG_REMOTEPTZCTRL			= 93,	//Զ����̨����
	EVENTLOG_ALARMINPUT				= 94,	//��������
	EVENTLOG_ALARMOUTPUT			= 95,	//�������
	EVENTLOG_BEGINMOTION			= 96,	//�ƶ���⿪ʼ
	EVENTLOG_ENDMOTION				= 97,	//�ƶ�������
	EVENTLOG_VIDEOLOST				= 98,	//��Ƶ��ʧ
	EVENTLOG_COVERALARMBEGIN		= 99,	//�ڵ��澯��ʼ
	EVENTLOG_COVERALARMEND			= 100,	//�ڵ��澯����
	EVENTLOG_DISKERR				= 101,	//Ӳ�̴�
	EVENTLOG_DISKFULL				= 102,	//Ӳ����
	EVENTLOG_NETWORKDISCONN			= 103,	//�����
	EVENTLOG_IPADDRESSCONFLICT		= 104,	//IP��ַ��ͻ
	EVENTLOG_LAWLESSLOCALACCESS		= 105,	//�Ƿ����ط���
	EVENTLOG_DISKFAILUREALARM		= 106,	//Ӳ�̹��ϱ���
	EVENTLOG_FANFAILUREALARM		= 107,	//���ȹ��ϱ���
	EVENTLOG_FANFAILURERECOVER		= 108,	//���ȹ��ϻָ�
	EVENTLOG_MBOARDFAILUREALARM		= 109,	//������ϱ���
	EVENTLOG_CHANFAILUREALARM		= 110,	//ͨ�����ϱ���
	EVENTLOG_TIMEERRALARM			= 111,	//ʱ����󱨾�
	EVENTLOG_ADJUSTTIME				= 112,	//Уʱ
	EVENTLOG_NETTRANSBEGIN			= 113,	//���紫������
	EVENTLOG_NETTRANSEND			= 114,	//���紫��ֹͣ

	EVENTLOG_AUDIOANALYSE			= 115,	//��Ƶ��������
	EVENTLOG_NOFACEANALYSE			= 116,	//����������
	EVENTLOG_PERIMETERPROC			= 117,	//�ܽ籨��
	EVENTLOG_STUMLINEALARM			= 118,	//���߱���
	EVENTLOG_PEPOLECOUNTALARM		= 119,	//����ͳ�Ʊ���
	EVENTLOG_LEFTOBJALARM			= 120,	//�����ﱨ��
	EVENTLOG_LOSSOBJALARM			= 121,	//��ʧ�ﱨ��

	EVENTLOG_INVALIDFLAG					//��Ч�ػ���
};

//¼����ϸ��Ϣ
typedef struct tagBSCP_LOG_RECORDDETAIL
{	
	CHAR strname[16];			//�����û���
	UINT dt;					//�¼�������ʱ��(32λtime_t����)
	UINT beforedt;				//����ǰ��ʱ��(32λtime_t����)
	UINT afterdt;				//�������ʱ��(32λtime_t����)
	UCHAR type;					//�����¼�
	UCHAR diskno;				//����id
	USHORT rev;					//����
	ULONG recordId;				//¼��α��
	ULONG endrecordId;			//¼��α��
	ULONG ChannelId;			//ͨ�����,��λ����	
}BSCP_LOG_RECORDDETAIL, *PBSCP_LOG_RECORDDETAIL;

//��־��Ϣ
typedef struct tagBSCP_DVR_LOG
{
	BSCP_LOG_RECORDDETAIL logs[MAX_ONEPAGELOG_SIZE];
}BSCP_DVR_LOG, *PBSCP_DVR_LOG;

//��־��������IP
typedef struct tagBSCP_LOG_NETCFGIP
{
	CHAR szNetWorkIP[16];		//IP��Ϣ
}BSCP_LOG_NETCFGIP, *PBSCP_LOG_NETCFGIP;

//�������Ϊ0-7֮��(���֧��8��Ӳ��)
#define BSCP_ALLDISK	0xFF	//���д���

//��������Ϣ
typedef struct tagBSCP_DISK_SUBINFO
{
	UCHAR disk;			//�������0 - 7
	UCHAR sleep;		//�Ƿ���˯��״̬1:�� 0:��
	UCHAR exist;		//�����Ƿ����1:���� 0:������
	UCHAR rev;			//����
	USHORT wrev;		//����
	USHORT timeout;		//���̵ĳ�ʱʱ��
	ULONG idle;			//�����Ѿ������˶೤ʱ��
	ULONG sleepTime;	//�Ѿ�˯�ߵ�ʱ��
	ULONG totalSectors;	//��������(����Ϊ������,һ������Ϊ512BYTE)
	UCHAR flag;			//��־λ
	UCHAR Selected;		//Current Selected disk through network
	UCHAR isInUsed;		//���û�����Ϊ�����̵��̷�
	UCHAR bBackup;		//������
	UCHAR bFormat;		//�Ƿ��ʽ��
	UCHAR bRecord;		//¼����
	UCHAR Backup;		//������
	UCHAR waking;		//��������������
}BSCP_DISK_SUBINFO, *PBSCP_DISK_SUBINFO;

//������Ϣ
typedef struct tagBSRCLIENT_DISKINFO
{
	LONG nDiskCount;	//���̸���,����ȡ���д�����Ϣʱ��������
	BSCP_DISK_SUBINFO disk[MAX_DISK_NUM];
}BSRCLIENT_DISKINFO, *PBSRCLIENT_DISKINFO;

//DVR�ļ�ʱ�䳤��ת��
typedef struct tagBSRCLIENT_FILETIME2LEN
{
	BOOL bTime2Len;		//TRUEΪʱ��ת����,FALSEΪ����תʱ��
	UINT stime;			//��ʼʱ��(32λtime_t����)
	UINT etime;			//����ʱ��(32λtime_t����)
	UINT64 bstart;		//��ʼ�ֽ���
	UINT64 blength;		//���ݳ���
}BSRCLIENT_FILETIME2LEN, *PBSRCLIENT_FILETIME2LEN;

//�������
typedef enum
{
	DISKTYPE_WORKDISK		= 0,	//������
	DISKTYPE_RECORDDISK		= 1,	//¼����
	DISKTYPE_BACKDISK		= 2		//������
}BSRCLIENT_DISKTYPE;

//�������Ӳ������
typedef enum
{
	WDISKTYPE_NONE			= 0,	//��
	WDISKTYPE_YES			= 1,	//��
	WDISKTYPE_NO			= 2		//��
}BSRCLIENT_WORKSUBTYPE;

//DVR���ػطſ�������
typedef enum
{
	LOCALPLAYCMD_PLAY		= 0,	//����¼��
	LOCALPLAYCMD_FASTBACK,			//����
	LOCALPLAYCMD_FASTSPEED,			//���
	LOCALPLAYCMD_PAUSE,				//��ͣ
	LOCALPLAYCMD_BACKTOFIRST,		//�ص���¼�����
	LOCALPLAYCMD_SINGLEFRAME,		//��֡����
	LOCALPLAYCMD_PREVIOUS,			//��һ��
	LOCALPLAYCMD_NEXT,				//��һ��
	LOCALPLAYCMD_PREALARM,			//��һ�α���¼��
	LOCALPLAYCMD_NEXTALARM			//��һ�α���¼��
}BSRCLIENT_LOCALPLAYCMD;

//��̨1604��ӿ�������
typedef enum
{
	SELECT_RECORD1604	= 128,		//ѡ��¼���
	CTRL_RECORD1604		= 129,		//����¼���
	ALARM_INPUT1604		= 130,		//��������
	ALARM_OUTPUT1604	= 131,		//�������
	SELECT_PTZ1604		= 132,		//ѡ����̨
	CTRL_PTZ1604		= 133,		//������̨
	CLEAR_ALARMOUT1604	= 134		//�������
}BSRCLIENT_PTZCTRLCMDBY1604;

//��̨����ֱ�ӿ�������
typedef enum
{
	BSRCTRLPTZ_CHAN01LOCAL	= 0xAA00,		//���Ƶ�1ͨ����̨
	BSRCTRLPTZ_CHAN02LOCAL	= 0xAA01,		//���Ƶ�2ͨ����̨
	BSRCTRLPTZ_CHAN03LOCAL	= 0xAA02,		//���Ƶ�3ͨ����̨
	BSRCTRLPTZ_CHAN04LOCAL	= 0xAA03,		//���Ƶ�4ͨ����̨
	BSRCTRLPTZ_CHAN05LOCAL	= 0xAA04,		//���Ƶ�5ͨ����̨
	BSRCTRLPTZ_CHAN06LOCAL	= 0xAA05,		//���Ƶ�6ͨ����̨
	BSRCTRLPTZ_CHAN07LOCAL	= 0xAA06,		//���Ƶ�7ͨ����̨
	BSRCTRLPTZ_CHAN08LOCAL	= 0xAA07,		//���Ƶ�8ͨ����̨
	BSRCTRLPTZ_CHAN09LOCAL	= 0xAA08,		//���Ƶ�9ͨ����̨
	BSRCTRLPTZ_CHAN10LOCAL	= 0xAA09,		//���Ƶ�10ͨ����̨
	BSRCTRLPTZ_CHAN11LOCAL	= 0xAA0A,		//���Ƶ�11ͨ����̨
	BSRCTRLPTZ_CHAN12LOCAL	= 0xAA0B,		//���Ƶ�12ͨ����̨
	BSRCTRLPTZ_CHAN13LOCAL	= 0xAA0C,		//���Ƶ�13ͨ����̨
	BSRCTRLPTZ_CHAN14LOCAL	= 0xAA0D,		//���Ƶ�14ͨ����̨
	BSRCTRLPTZ_CHAN15LOCAL	= 0xAA0E,		//���Ƶ�15ͨ����̨
	BSRCTRLPTZ_CHAN16LOCAL	= 0xAA0F,		//���Ƶ�16ͨ����̨
	BSRSELECT_DVRLOCAL		= 0xF070,		//ѡ��DVR
	BSRSELECT_BAUDLOCAL		= 0xF081,		//ѡ������
	BSRSELECT_PROTOLOCAL	= 0xF083,		//ѡ��Э��
	BSRSELECT_PTZLOCAL		= 0xF084,		//ѡ����̨
	BSRCTRL_PTZLOCAL		= 0xF085,		//������̨(��ǰ��������ͨ��)
	BSRPRESS_SETLOCAL		= 0xF089,		//����Ԥ��λ
	BSRPRESS_GOLOCAL		= 0xF090,		//��תԤ��λ
	BSRCTRL_CUSTOMLOCAL		= 0xF0FE		//�Զ���
}BSRCLIENT_PTZCTRLCMDBYLOCAL;

//���ؿ����������
typedef enum
{
	BSRYTCTRL_ACTION_UP					= 0,	//�����ƶ�
	BSRYTCTRL_ACTION_DOWN				= 1,	//�����ƶ�
	BSRYTCTRL_ACTION_LEFT				= 2,	//�����ƶ�
	BSRYTCTRL_ACTION_RIGHT				= 3,	//�����ƶ�
	BSRYTCTRL_ACTION_CLOSE				= 4,	//��Ȧ��С
	BSRYTCTRL_ACTION_OPEN				= 5,	//��Ȧ�Ŵ�
	BSRYTCTRL_ACTION_NEAR				= 6,	//���࿿��
	BSRYTCTRL_ACTION_FAR				= 7,	//������Զ
	BSRYTCTRL_ACTION_WIDE				= 8,	//�䱶����
	BSRYTCTRL_ACTION_TELE				= 9,	//�䱶��С
	BSRYTCTRL_ACTION_AUTO				= 10,	//�Զ�״̬
	BSRYTCTRL_ACTION_STOP				= 11,	//ֹͣ����

	BSRYTCTRL_ACTION_SETPRE				= 12,	//����Ԥ��λ
	BSRYTCTRL_ACTION_GOPRE				= 13,	//ת��Ԥ��λ
	BSRYTCTRL_ACTION_DELPRE				= 14,	//ɾ��Ԥ��λ

	BSRYTCTRL_ACTION_SETPATTERNSTART	= 15,	//�����켣
	BSRYTCTRL_ACTION_SETPATTERNSTOP		= 16,	//ֹͣ�켣
	BSRYTCTRL_ACTION_RUNPATTREN			= 17,	//���й켣

	BSRYTCTRL_ACTION_SETAUX				= 18,	//���ø���
	BSRYTCTRL_ACTION_DELAUX				= 19,	//ɾ������

	BSRYTCTRL_ACTION_ADDSCANDOT			= 20,	//���Ѳ����
	BSRYTCTRL_ACTION_DELSCANDOT			= 21,	//ɾ��Ѳ����

	BSRYTCTRL_ACTION_RUNSCAN			= 22,	//��ʼѲ��
	BSRYTCTRL_ACTION_ENDSCAN			= 23,	//����Ѳ��

	BSRYTCTRL_ACTION_AUXALM				= 24	//��������
}BSRCLIENT_PTZCTRLPARABYLOCAL;

//��̨����Ԥ��λ����
typedef enum
{
	PTZPRESET_SET		= 1,	//����Ԥ�õ�
	PTZPRESET_DEL		= 2,	//���Ԥ�õ�
	PTZPRESET_GOTO		= 3		//ת��Ԥ�õ�
}BSRCLIENT_PTZPRESET;

//��̨�켣��������
typedef enum
{
	PTZTRACK_START		= 1,	//��ʼ��¼
	PTZTRACK_STOP		= 2,	//ֹͣ��¼
	PTZTRACK_RUN		= 3,	//��ʼ����
	PTZTRACK_STOPRUN	= 4		//ֹͣ����
}BSRCLIENT_PTZTRACKCMD;

//��̨Э������
typedef enum
{
	PTZPROTOCOL_PLCOD		= 0,	//PLCO-DЭ��
	PTZPROTOCOL_PLCOP		= 1,	//PLCO-PЭ��
	PTZPROTOCOL_ADAB		= 2,	//AD/ABЭ��
	PTZPROTOCOL_KDT			= 3,	//KDTЭ��
	PTZPROTOCOL_ADT			= 4,	//ADTЭ��

	//�����Э���V6.03B09/V6.10B09֧��
	PTZPROTOCOL_PHILIPS		= 5,	//PHILIPSЭ��
	PTZPROTOCOL_SONY		= 6,	//SONYЭ��

	PTZPROTOCOL_HY			= 7,	//HYЭ��
	PTZPROTOCOL_SAMSUNG		= 8,	//SAMSUNGЭ��
	PTZPROTOCOL_SHARP		= 9,	//SHARPЭ��
	PTZPROTOCOL_TIANDI		= 10,	//TIANDIЭ��
	PTZPROTOCOL_TWHC		= 11,	//TWHCЭ��
	PTZPROTOCOL_NONE		= 12,	//NONEЭ��
	PTZPROTOCOL_AD1614M		= 13,	//AD1614MЭ��
	PTZPROTOCOL_ADMATRIX	= 14,	//ADMATRIXЭ��

	PTZPROTOCOL_BANKNOTE	= 15,	//BANKNOTEЭ��
	PTZPROTOCOL_BOCOM		= 16,	//BOCOMЭ��
	PTZPROTOCOL_DHMATRIX	= 17,	//DH-MATRIXЭ��
	PTZPROTOCOL_DSCP		= 18,	//DSCPЭ��
	PTZPROTOCOL_KALATEL		= 19,	//KALATELЭ��
	PTZPROTOCOL_CUSTOM		= 20,	//CUSTOMЭ��
	PTZPROTOCOL_JVC			= 21,	//JVCЭ��
	PTZPROTOCOL_YAAN		= 22,	//YAANЭ��
	PTZPROTOCOL_B01			= 23,	//B01Э��
	PTZPROTOCOL_MAINVAN		= 24,	//MAINVANЭ��
	PTZPROTOCOL_LILIN		= 25,	//LILINЭ��
	PTZPROTOCOL_722LD		= 26,	//722-LDЭ��
	PTZPROTOCOL_3032103X	= 27,	//3032103XЭ��
	PTZPROTOCOL_INFINOVA	= 28,	//INFINOVAЭ��
	PTZPROTOCOL_SYYT		= 29,	//SYYTЭ��
	PTZPROTOCOL_HONEYWELL	= 30,	//HONEYWELLЭ��
	PTZPROTOCOL_PIH717		= 31,	//PIH717Э��
}BSRCLIENT_PTZPROTOCOL;

//��̨��ַ����
typedef enum
{
	//����汾
	PTZADDTYPE_BLUESTAR	= 0,	//BSR��ַ
	PTZADDTYPE_COMMON	= 1,	//ͨ�õ�ַ

	//B09�汾
	PTZADDTYPE_PTZ485	= 0,	//��̨485
	PTZADDTYPE_CTRL485	= 1,	//�ܿ�485
	PTZADDTYPE_RS232	= 2		//����232
}BSRCLIENT_PTZADDTYPE;

//��̨������
typedef enum
{
	PTZBITRATE_VALUE75		= 0,	//75
	PTZBITRATE_VALUE110		= 1,	//110
	PTZBITRATE_VALUE134		= 2,	//134
	PTZBITRATE_VALUE150		= 3,	//150
	PTZBITRATE_VALUE300		= 4,	//300
	PTZBITRATE_VALUE600		= 5,	//600
	PTZBITRATE_VALUE1200	= 6,	//1200
	PTZBITRATE_VALUE1800	= 7,	//1800
	PTZBITRATE_VALUE2400	= 8,	//2400
	PTZBITRATE_VALUE4800	= 9,	//4800
	PTZBITRATE_VALUE7200	= 10,	//7200
	PTZBITRATE_VALUE9600	= 11,	//9600
	PTZBITRATE_VALUE14400	= 12,	//14400
	PTZBITRATE_VALUE19200	= 13,	//19200
	PTZBITRATE_VALUE38400	= 14,	//38400
	PTZBITRATE_VALUE57600	= 15,	//57600
	PTZBITRATE_VALUE115200	= 16	//115200
}BSRCLIENT_PTZBITRATE;

//Ѳ����
typedef struct tagBSCP_CONTROL_CRUISEPOS
{	
	UCHAR presetId;			//Ԥ��λ���
	UCHAR cruiseV;			//Ѳ���ٶ�
	USHORT stopTime;		//ͣ��ʱ��
}BSCP_CONTROL_CRUISEPOS, *PBSCP_CONTROL_CRUISEPOS;

//Ѳ��·��
typedef struct tagBSCP_CONTROL_CRUISEPOSITEM
{
	UCHAR chanId;			//ͨ�����
	UCHAR CruisepathId;		//Ѳ��·�����
	USHORT res;				//����
	BSCP_CONTROL_CRUISEPOS cruise[MAX_CRUISEDOT_NUM];
}BSCP_CONTROL_CRUISEPOSITEM, *PBSCP_CONTROL_CRUISEPOSITEM;

//����Ѳ��·��
typedef struct tagBSCP_CONTROL_CRUISEPOSLIST
{
	BSCP_CONTROL_CRUISEPOSITEM item[MAX_CHAN_NUM];	//Ѳ��·����
}BSCP_CONTROL_CRUISEPOSLIST, *PBSCP_CONTROL_CRUISEPOSLIST;

//DVR��̨�����ܿز�����,��������
typedef struct tagBSCP_CONTROL_CTRLPARA_ITEM
{
	UCHAR addr;				//���Ƶ�ַ
	UCHAR resv;				//����λ
	USHORT protocol;		//��̨����Э��BSRCLIENT_PTZPROTOCOL

	UCHAR data_bit;			//����λ4,5,6,7,8
	UCHAR stop_bit;			//ֹͣλ1, 1.5 2
	UCHAR check_bit;		//У��λ ��,ż,��
	UCHAR baud_rate;		//������BSRCLIENT_PTZBITRATE
}BSCP_CONTROL_CTRLPARA_ITEM, *PBSCP_CONTROL_CTRLPARA_ITEM;

//DVR��̨�����ܿز���,��������
typedef struct tagBSCP_CONTROL_CTRLPARA_LIST
{
	USHORT addr_type;		//��̨��ַ����BSRCLIENT_PTZADDTYPE
	USHORT dvr_addr;		//DVR��ַ(��ЧֵΪ0-255֮��)
	BSCP_CONTROL_CTRLPARA_ITEM uart_ctrl[MAX_CHAN_NUM+1];	//MAX_CHAN_NUM ����̨,һ���ܿ�
}BSCP_CONTROL_CTRLPARA_LIST, *PBSCP_CONTROL_CTRLPARA_LIST;

//=============>ATMר�ô������ýṹ,DVR�ͺ�6003
//DVR��̨�����ܿز�����
typedef struct tagBSCP_CONTROL_ATMCTRLPARA_ITEM
{
	UCHAR addr;				//���Ƶ�ַ
	UCHAR resv;				//����λ
	UCHAR protocol;			//��̨����Э��BSRCLIENT_PTZPROTOCOL
	UCHAR data_bit;			//����λ4,5,6,7,8

	UCHAR stop_bit;			//ֹͣλ1, 1.5 2
	UCHAR check_bit;		//У��λ ��,ż,�� 
	UCHAR baud_rate;		//������BSRCLIENT_PTZBITRATE
	UCHAR rev;				//����λ
}BSCP_CONTROL_ATMCTRLPARA_ITEM, *PBSCP_CONTROL_ATMCTRLPARA_ITEM;

#define BSRCLIENT_UARTCOUNT	3	//��������������
//DVR��̨�����ܿز���
typedef struct tagBSRCLIENT_ATMCTRLPARA_GROUP
{
	BSCP_CONTROL_ATMCTRLPARA_ITEM uart_ctrl[BSRCLIENT_UARTCOUNT];	//������
	UCHAR dvraddr;			//¼�����ַ1-255
	UCHAR AtmSelect;		//ATMѡ��0-��,1-�����ͨ,2-����,3-���ױ���,4-ӡ��(�����봮��HUB�ظ�)
	UCHAR ConmmMode;		//ͨ�ŷ�ʽ0-��,1-����232
	UCHAR AtmRecorddelay;	//¼����ʱ10-255
	UCHAR startDepartment;	//�Ƿ������ڱ��� 0�� 1��(�����봮��HUB�ظ�)
	UCHAR startRecorddelay;	//�Ƿ�����¼����ʱ0�� 1��
	UCHAR rev1;				//����
	UCHAR rev2;				//����
}BSRCLIENT_ATMCTRLPARA_GROUP, *PBSRCLIENT_ATMCTRLPARA_GROUP;
//=============>ATMר�ô������ýṹ,DVR�ͺ�6003

///DVR��̨�����ܿز���
typedef struct tagBSRCLIENT_ATMCTRLPARA_B02
{
	BSCP_CONTROL_ATMCTRLPARA_ITEM uart_ctrl[BSRCLIENT_UARTCOUNT];	///������
	UCHAR dvraddr;			///¼�����ַ1-255
	UCHAR AtmSelect;		///ATMѡ��0-��,1-�����ͨ,2-����,3-���ױ���,4-ӡ��(�����봮��HUB�ظ�)
	UCHAR ConmmMode;		///ͨ�ŷ�ʽ0-��,1-����232
	UCHAR AtmRecorddelay;	///¼����ʱ10-255
	UCHAR startDepartment;	///�Ƿ������ڱ��� 0�� 1��(�����봮��HUB�ظ�)
	UCHAR startRecorddelay;	///�Ƿ�����¼����ʱ0�� 1��
	UCHAR upload;				    ///�Ƿ񿨺��ϴ�
	UCHAR rev;				
}BSRCLIENT_ATMCTRLPARA_B02, *PBSRCLIENT_ATMCTRLPARA_B02;
///=============>ATMר�ô������ýṹ,DVR�ͺ�B02

///DVR��̨�����ܿز���
typedef struct tagBSRCLIENT_ATMCTRLPARA_61X
{
	BSCP_CONTROL_ATMCTRLPARA_ITEM uart_ctrl[BSRCLIENT_UARTCOUNT];	///������
	UCHAR dvraddr;			///¼�����ַ1-255
	UCHAR AtmSelect;		///ATMѡ��0-��,1-�����ͨ,2-����,3-���ױ���,4-ӡ��(�����봮��HUB�ظ�)
	UCHAR ConmmMode;		///ͨ�ŷ�ʽ0-��,1-����232
	UCHAR AtmRecorddelay;	///¼����ʱ10-255
	UCHAR startDepartment;	///�Ƿ������ڱ��� 0�� 1��(�����봮��HUB�ظ�)
	UCHAR startRecorddelay;	///�Ƿ�����¼����ʱ0�� 1��
	UCHAR upload;				    ///�Ƿ񿨺��ϴ�
	UCHAR rev;				      ///recordDelay ��λ
}BSRCLIENT_ATMCTRLPARA_61X, *PBSRCLIENT_ATMCTRLPARA_61X;
///=============>ATMר�ô������ýṹ,DVR�ͺ�B02

//���翨������(�������������������ص�)
typedef struct tagBSRCLIENT_NETCARDPROTOCOL
{
	ULONG nRecvIP;			//���յĿ��ŵ�IP��ַ
	USHORT nRecordDelay;	//¼����ʱ:10��600
	UCHAR nProtocol;		//����Э��BSRCLIENT_CARDPROTOCOL(RJ45����)
	UCHAR nRes;				//�����ֶ�
}BSRCLIENT_NETCARDPROTOCOL, *PBSRCLIENT_NETCARDPROTOCOL;

//����,͸����,�ԱȶȻ�ȡ���
typedef enum
{
	VEFFECTCMD_BRIGHT	= 0xA,	//����
	VEFFECTCMD_TRANS	= 0xB,	//͸����
	VEFFECTCMD_CONTRAST	= 0xC	//�Աȶ�
}BSRCLIENT_EFFECTTYPE;

//�ֳ���ʱ�ַ�����ʾλ��
typedef enum
{
	SCENESTRPOS_360_280	= 0,	//(360,280)
	SCENESTRPOS_60_40,			//(60,40)
	SCENESTRPOS_640_40,			//(640,40)
	SCENESTRPOS_60_450,			//(60,450)
	SCENESTRPOS_640_450			//(640,450)
}BSRCLIENT_SCENESTRINGPOS;

//DVRӲ����Ϣ
typedef struct tagBSCP_DVR_GETHARDINFO
{
	CHAR mark[8];				//�Ƿ���Ч�ı��:bstar
	CHAR mode[16];				//��Ʒ�ͺ�
	CHAR sn[16];				//S/N
	CHAR pn[16];				//P/N
}BSCP_DVR_GETHARDINFO, *PBSCP_DVR_GETHARDINFO;

//��������Ϣ
typedef struct tagBSCP_SLAVECPU_BUFINFO
{
	UINT start;			//
	UINT end;			//
	UINT limit;			//
	UINT totalSize;		//
}BSCP_SLAVECPU_BUFINFO, *PBSCP_SLAVECPU_BUFINFO;

//¼�����
typedef struct tagBSCP_SLAVECPU_RECORDPARA
{
	UINT FrameRate;			//֡��
	UINT BitRate;			//����
	UINT VideoMode;			//0: Pal��ʽ(Ĭ��) 1: NTSC

	UINT quality;			//����
	UINT MotionRecordType;	//�ƶ����¼������
	UINT RecordType;		//����:��ͨ,����,��ʱ,�ƶ����
	UINT mv_JG;				//�趨�˶�����������

	UINT max_JG;			//�趨����Ĺؼ�����
	UINT filter_level;		//ָ����������
	UINT method;			//¼��ʽ:352X288,704X288��
	UINT method_select;		//¼��ʽ��ѡ��
	UINT quantity;			//����ֵ
}BSCP_SLAVECPU_RECORDPARA, *PBSCP_SLAVECPU_RECORDPARA;

//ͨ����ʱ������Ϣ
typedef struct _tagBSCP_DISK_TIMERBACKUPINFO
{
	ULONG backup_day;		//�������� 1-7
	ULONG enable;			//�������� 0:�� 1:��
	ULONG wHour;			//���ݹ���ʱ��:Сʱ
	ULONG wMinute;			//���ݹ���ʱ��:��
	ULONG wSecond;			//���ݹ���ʱ��:��
	ULONG bHour;			//����¼��ʼʱ��:Сʱ
	ULONG eHour;			//����¼�����ʱ��:Сʱ

	ULONG bMinute;			//����¼��ʼʱ��:����
	ULONG eMinute;			//����¼�����ʱ��:����
	ULONG bSecond;			//����¼��ʼʱ��:��
	ULONG eSecond;			//����¼�����ʱ��:��
	ULONG rec_type;			//����¼������
	ULONG backup_time;		//��������:��������,����һ - ������
}BSCP_DISK_TIMERBACKUPINFO, *PBSCP_DISK_TIMERBACKUPINFO;

//��ʱ������Ϣ
typedef struct _tagBSCP_DISK_TIMERBACKUP
{
	BSCP_DISK_TIMERBACKUPINFO time_backup[MAX_CHAN_NUM];		//��Ϣ��
}BSCP_DISK_TIMERBACKUP, *PBSCP_DISK_TIMERBACKUP;

//��CPU��ϸ��Ϣ
typedef struct tagBSCP_SLAVECPU_SCPUINFO
{
	UINT CurState;					//Current System State
	ULONG PrevState;				//Current System State

	ULONG generalRecordTime;		//��ͨ¼���¼��ʱ��
	UCHAR recordType;				//¼������
	UCHAR recordStarted;			//¼���Ƿ��Ѿ�����
	UCHAR cpu;						//ID of Slave CPU
	UCHAR state;					//״̬
	UINT waitTime;					//Wait Time of Stopping Record
	BSCP_SLAVECPU_BUFINFO bi;		//Buffer Information
	ULONG dataReady[2][16];			//Data Ready Answer Counts
	ULONG totalBlocks;				//Total Record Blocks of 512 bytes
	ULONG leftBlocks;				//Blocks left in buffer
	BSCP_SLAVECPU_RECORDPARA  rp;	//Record Parameters
	ULONG DataStatusUpdate[4];		//Data Status Update
	ULONG transFail;				//Times of transfer failure
	UCHAR isUp;						//Is this CPU UP ?
	UCHAR transfer;					//Is this CPU transfering ?
	UCHAR mode;						//Mode of Transferring data
	UCHAR onlyTransfer;				//Only Transfer ?
	ULONG resetTimes;				//Times of hard reset
	ULONG backup;					//Interrupt Acknowlegements
	UCHAR pause;					//Paused ?
	UCHAR reset;					//Reset ?
	UCHAR stopped;					//Stopped ?
	UCHAR disabled;					//Disabled ?
	ULONG RecordTime;				//¼��ʱ��
	ULONG remoteBlocks;				//Slave�˲��������ݿ�
	ULONG localBlocks;				//�õ����ص����ݿ�
	ULONG lostBlocks;				//�������������������

	//���Ե�
	ULONG checkPoints[MAX_CHECKPOINTS_NUM];		//���ӵĲ��Ե����
	ULONG PrevInterrupts;			//
	ULONG CurrInterrupts;			//
}BSCP_SLAVECPU_SCPUINFO, *PBSCP_SLAVECPU_SCPUINFO;

//��CPU״̬��Ϣ
typedef struct tagBSCP_SLAVECPU_SCPUSTATUS
{
	UINT tmAssert;			//is tmAssert ?

	UCHAR CurState;			//Current System State
	UCHAR PrevState;		//Current System State

	UCHAR cpu;				//Which CPU?
	UCHAR type;				//¼������
	UCHAR ReallyStart;		//¼���Ƿ���������
	UCHAR res;				//
	USHORT res1;			//

	UINT bitRate;			//����
	UINT totalBlocks;		//¼�������

	BSCP_DATATIME sdt;		//��CPU��ʱ������
}BSCP_SLAVECPU_SCPUSTATUS, *PBSCP_SLAVECPU_SCPUSTATUS;

//RTP������Ϣ
typedef struct tagBSRCLIENT_RTPCONFIGINFO
{
	CHAR szIPAddress[16];	//IP��ַ
	WORD wPort;				//�˿�
	WORD wMode;				//����ģʽ0-������ 1-������
}BSRCLIENT_RTPCONFIGINFO, *PBSRCLIENT_RTPCONFIGINFO;

//DVR����������Ϣ
#define	INTFS_MAX_NAME_LENGTH			32	//
#define	INTFS_MAX_SMTP_LENGTH			64	//
#define RTCP_VCTL_MAX					4	//���Ʒ�������

//�����ϴ���������
typedef enum
{
	NETUPEVENT_EVERYTHING_OK	= 0,	//��������
	NETUPEVENT_REPORT_IP		= 1,	//����IP��ַ
	NETUPEVENT_ALARM_OUT		= 2,	//�������
	NETUPEVENT_HDISK_ERROR		= 3,	//���̴���
	NETUPEVENT_VIDEO_LOST		= 4,	//��Ƶ��ʧ
	NETUPEVENT_VIDEO_RESTORE	= 5,	//��Ƶ�ָ�
	NETUPEVENT_ALLEVENT			= 6		//��������
}BSRCLIENT_UPLOADALARMEVENT;

//�鲥����С
typedef enum
{
	NETPACKSIZE_512		= 0,	//512BYTE
	NETPACKSIZE_1024	= 1,	//1024BYTE
	NETPACKSIZE_2048	= 2,	//2048BYTE
	NETPACKSIZE_4096	= 3,	//4096BYTE
	NETPACKSIZE_8192	= 4,	//8192BYTE
	NETPACKSIZE_16384	= 5,	//16384BYTE
}BSRCLIENT_NETPACKSIZE;

//SMTP����
typedef enum
{
	SMTPHOST_SOHU	= 0,	//�Ѻ�
	SMTPHOST_SINA	= 1,	//����
	SMTPHOST_CHINA	= 2,	//�л�
	SMTPHOST_OTHER	= 3		//����
}BSRCLIENT_SMTPHOST;

//DDNS������
typedef enum
{
	DDNSSVR_DISABLE		= 0,	//����
	DDNSSVR_HUASHENGKE	= 1,	//������
	DDNSSVR_JINWANWEI	= 2		//����ά(��V6.03/V6.10��B09�汾֧�ִ�ѡ��)
}BSRCLIENT_DDNSTYPE;

//�������ýṹ
typedef struct tagBSCP_NETWORK_NETCFGINFO
{
	UCHAR mode;						//0:�ֹ� 1:DHCP 2:ADSL
	UCHAR which;					//�ڼ�·����
	UCHAR network;					//���绷�� 1:Ҫ�ذ� 0:��Ҫ�ذ�
	UCHAR transfer_mode;			//���䷽ʽ:0-TCP, 1-����, 2-����
	UCHAR delay;					//�ӳ�:0-1024
	UCHAR packet_size;				//����СBSRCLIENT_NETPACKSIZE

	UCHAR atm_protocol;				//����Э��:0-����,1-�ɶ�,2-�½�,3-����(���������翨�������ص�,������Ĳ�������)
	UCHAR alarm_method;				//��������Э��:0-����, 1-TCP, 2-UDP

	ULONG MulticastIP;				//�鲥��ַ
	USHORT port;					//����˿�
	UCHAR ttl;						//�㲥����TTL
	UCHAR res1;						//����

	ULONG cardSelect;				//���յĿ��ŵ�IP��ַ(���������翨�������ص�)

	ULONG IPAddress;				//IP��ַ
	ULONG netMask;					//��������
	ULONG defaultGateway;			//ȱʡ·��

	ULONG alarmCenter;				//��������IP��ַ
	USHORT serial;					//�ϱ�IP��ַʱ�����
	USHORT res2;   
	CHAR ddnsname[INTFS_MAX_NAME_LENGTH];	//�������û���
	CHAR ddnspass[INTFS_MAX_NAME_LENGTH];	//�����ǿ���

	ULONG dnsServer;						//����������IP��ַ
	CHAR mailName[INTFS_MAX_SMTP_LENGTH];	//�ʼ�������ַ
	UCHAR enable_ddns;						//DDNSѡ��BSRCLIENT_DDNSTYPE
	UCHAR enable_mail;						//�Ƿ������ʼ�����

	UCHAR smtphost;					//�ʼ�ת������BSRCLIENT_SMTPHOST
	UCHAR alarmEvent;				//ͨ���ʼ��������¼�BSRCLIENT_UPLOADALARMEVENT

	CHAR mailuser[INTFS_MAX_NAME_LENGTH];	//ESMTP��֤�û���
	CHAR mailpass[INTFS_MAX_NAME_LENGTH];	//ESMTP��֤����
	CHAR smtpHost[INTFS_MAX_NAME_LENGTH];	//ESMTP������

	CHAR username[INTFS_MAX_NAME_LENGTH];	//�û���
	CHAR password[INTFS_MAX_NAME_LENGTH];	//����

	ULONG log_server;				//��־������IP��ַ,0��ʾ����

	//RTP�Ŀ�����ѡ��
	ULONG RTPMaxBitRate;			//RTCP�������
	ULONG RTPMinBitRate;			//RTCP��С����
	USHORT RTPIncreaseStep;			//���ӵĲ���
	USHORT RTPDecreaseStep;			//��С�Ĳ���

	UCHAR RTCPEnabled;				//0:������,1:����
	UCHAR applyChanges;				//Ӧ�õ�ǰ����
	UCHAR AudioEnabled;				//��Ƶ����

	UCHAR RTPFlowControl;			//RTP�Ƿ�ʹ������Э��,Ĭ�ϲ�ʹ������

	UCHAR StartReversedAudio;		//����������Ƶ
	UCHAR ReversedAudioState;		//������Ƶ״̬

	UCHAR displayRecordTime;		//�Ƿ���ʾ����¼��

	UCHAR BitrateControlStrategy[RTCP_VCTL_MAX];	//���ʿ��Ʋ���

	UCHAR NiMode;    //�����Ĺ���ģʽ,�������λΪ1ʱ��Ч,
	//����λ�ĺ���0: Auto Negotiation; 1: 100Mbps & Full-duplex;
	//2: 100Mbps & Half-duplex; 3: 10Mbps & Full-duplex; 4: 10Mbps & Half-duplex
	UCHAR res3[3];					//����
}BSCP_NETWORK_NETCFGINFO, *PBSCP_NETWORK_NETCFGINFO;

/////////////////////////���ýṹ����///////////////////////////////
//¼���ϵͳ��Ϣ(ֻ��),�ܳ�����512���ֽ�
typedef struct tagBSCP_DVR_DEVICESYSINFO_READ
{
	UCHAR r_version;		//��_�汾��
	UCHAR w_version;		//д_�汾��
	UCHAR encoded;			//�Ƿ����
	UCHAR alarm_in_num;		//������������(V6.10B09֮��汾���ֶ���Ч)

	ULONG typeandmode;		//���ͼ�ģʽ
	ULONG subType;			//�����ͼ�ģʽ	BSRCLIENT_DEVSUBTYPE
	ULONG fpStatus;			//¼���״̬  λ��BSRCLIENT_DEVSTATUS, ��ǰ����(fpStatus & 0x7) + 1)

	UCHAR slaveCPUs;		//��CPU�ĸ���
	UCHAR channumber;		//ͨ����
	UCHAR videoNumber;		//��Ƶ�������
	UCHAR maxDiskNumber;	//���Ӳ�̸���

	UCHAR transferType;		//������������� 0-MP4����1-ASP(�����㷨)
	UCHAR encoderType;		//�洢���������� 0-MP4����1-ASP(�����㷨)
	UCHAR nicNumber;		//������Ŀ
	UCHAR uartNumber;		//������Ŀ

	UCHAR flashType;		//Flash����
	UCHAR flashSize;		//Flash��С
	SHORT trans_maxwidth;	//�������ֱ���(���),�߶ȹ̶�Ϊ288, 704��ʾ����ΪHalf D1, 352��ʾֻ��CIF

	CHAR sysver[40];		//ϵͳ�汾��
	ULONG softwarever;		//����汾��

	UCHAR quarter_status;	//�໭���״̬��
	UCHAR hardwarever;		//Ӳ���汾��
	UCHAR ptz_ctrlmode;		//��̨����ģʽ 0-�ر�	1-��
	UCHAR iic_opspeed;		//IIC�����ٶ� 0-���� 1-����
}BSCP_DVR_DEVICESYSINFO_READ, *PBSCP_DVR_DEVICESYSINFO_READ;

//¼���ϵͳ��Ϣ(��д),�ܳ�����512���ֽ�
typedef struct tagBSCP_DVR_DEVICESYSINFO_WRITE
{
	UCHAR rtpIncluded;			//�Ƿ�֧��RTPЭ��
	UCHAR transparentControl;	//͸��Э�鴮��

	USHORT year;				//��
	UCHAR month;				//��
	UCHAR day;					//��
	UCHAR hour;					//ʱ
	UCHAR minute;				//��
	UCHAR second;				//��
	UCHAR bRes;					//����
	USHORT wRes;				//����
}BSCP_DVR_DEVICESYSINFO_WRITE, *PBSCP_DVR_DEVICESYSINFO_WRITE;

//¼���ϵͳ����Ϣ, �ܵĳ�����1024���ֽ�
typedef struct tagBSCP_DVR_DEVICESYSINFO
{
	union
	{
		UCHAR padding[512];					//�ֶ����
		BSCP_DVR_DEVICESYSINFO_READ data;	//ֻ����Ϣ
	} r;
	union
	{
		UCHAR padding[512];					//�ֶ����
		BSCP_DVR_DEVICESYSINFO_WRITE data;	//��д��Ϣ
	} w;
}BSCP_DVR_DEVICESYSINFO, *PBSCP_DVR_DEVICESYSINFO;

//�˵�͸���ȷ�Χ
typedef enum
{
	TRANSRANGE_MIN	= 20,		//͸������Сֵ
	TRANSRANGE_MAX	= 255		//͸�������ֵ
}BSRCLIENT_TRANSRANGE;

//ϵͳ�趨���ڳ������
typedef struct tagBSCP_DVR_SYSCFG_GENERAL
{
	UCHAR language;				//����0:���� 1:English 2:����
	UCHAR video_system;			//��Ƶ��ʽ1:NTSC 0:PAL
	UCHAR motion_detect;		//�ƶ����߿��Ƿ���ʾ0:����ʾ 1:��ʾ   
	UCHAR vga_mode;				//VGA�ֱ���:0:800X600 1:1024X768
	UCHAR vga_freq;				//VGAƵ��:0:60HZ 1:75HZ
	UCHAR menu_trans;			//�˵�͸����20-255,�ο�BSRCLIENT_TRANSRANGE
	UCHAR vga_field;			//VGA�Ƿ񵥳����,0:��ͨ(2��)1:����(1��)
	UCHAR rev;					//����
}BSCP_DVR_SYSCFG_GENERAL, *PBSCP_DVR_SYSCFG_GENERAL;

//ϵͳ�趨�����豸��ʶ����
typedef struct tagBSCP_DVR_PAGEDEVID
{
	ULONG id;					//�豸���1-9999
	UCHAR name[16];				//�豸����
	UCHAR addr[16];				//�豸��ַ
	UCHAR remark[32];			//�豸��ע
}BSCP_DVR_PAGEDEVID, *PBSCP_DVR_PAGEDEVID;

//ϵͳ�趨�����豸��ʶ����
typedef struct tagBSCP_DVRPAGEDEVIDEX
{
  ULONG id;					    //�豸���1-9999
  UCHAR name[16];				//�豸����
  UCHAR addr[16];				//�豸��ַ
  UCHAR remark[32];			//�豸��ע
  UCHAR namePos;        //�豸������ʾλ��
  UCHAR bShow;          //�Ƿ���ʾ
  UCHAR res[2];         //������
}BSCP_DVR_PAGEDEVIDEX, *PBSCP_DVR_PAGEDEVIDEX;

//ͨ��¼���趨״̬
typedef struct tagBSCP_RECORD_CHANRECSTAT
{
	UCHAR startup;			//�Ƿ����� 0������ 1����
	UCHAR quality;          //�ֶ�¼����
	USHORT rev;             //����
}BSCP_RECORD_CHANRECSTAT, *PBSCP_RECORD_CHANRECSTAT;

//¼���趨�������
typedef struct tagBSCP_RECORD_PAGEGENERAL
{
	UCHAR rewrite;          //�����Ƿ��Զ����� 0:�� 1:��
	UCHAR full_alarm;       //�����Ƿ񱨾� 0:�� 1:��
	UCHAR mode;             //ģʽ 0:�ֶ� 1:�Զ�
	UCHAR channel;          //��·��
	BSCP_RECORD_CHANRECSTAT state[MAX_CHAN_NUM];  //ÿ��ͨ����״̬:�Ƿ��������ֶ�¼��Ļ���
}BSCP_RECORD_PAGEGENERAL, *PBSCP_RECORD_PAGEGENERAL;

//¼�����
typedef struct tagBSCP_RECORD_QUALITY
{
	UCHAR video_size;       //ѹ����Ƶ�Ĵ�С 0:CIF 1:DCIF 2:1/2D1 3: 2CIF 4:D1
	UCHAR bit_rate_type;    //�������� 0:������ 1:������ 3:�����ޱ�����
	UCHAR frame_rate;		//֡�� 1-25
	UCHAR filter;			//�˲� ��,1,2
	UCHAR Iframe_interval;	//I֡���25-200
	UCHAR quantity;			//���� 2-31
	UCHAR audio_bit_rate;	//��Ƶ����32,64 KBPS
	UCHAR quality;			//��������
	UCHAR media_type;		//��������:  0: �����;1 :��Ƶ��
	UCHAR rev;				//����
	USHORT video_bit_rate;	//��Ƶ���� 64-4096 KBPS
}BSCP_RECORD_QUALITY, *PBSCP_RECORD_QUALITY;

#define INTERF_QP_MAX_NUM  (6)

//¼���趨�߼�����
typedef struct tagBSCP_RECORD_PAGEADVANCE
{
	BSCP_RECORD_QUALITY quality[INTERF_QP_MAX_NUM+1];		//���ʶ��� 0-5 6Ϊ����
}BSCP_RECORD_PAGEADVANCE, *PBSCP_RECORD_PAGEADVANCE;

//��ʱ¼��ʱ���
typedef struct tagBSCP_RECORD_TIMERSECT
{
	ULONG bMinute;			//��ʼʱ��ķ� 0-59
	ULONG eMinute;			//����ʱ��ķ� 0-59
	ULONG bHour;			//��ʼʱ���Сʱ 0-23
	ULONG eHour;			//����ʱ���Сʱ 0-23
	ULONG quality;			//���� 0 - 5
	ULONG start;			//�Ƿ����� 0:������ 1:����
	ULONG rev;				//����
}BSCP_RECORD_TIMERSECT, *PBSCP_RECORD_TIMERSECT;

//��ʱ¼�����
typedef struct tagBSCP_RECORD_PAGETIMER
{
	BSCP_RECORD_TIMERSECT dayTimer[MAX_CHAN_NUM][7][4];	//ÿ��4��ʱ���,һ��7��,0-7��ʾ����,��һ...������
}BSCP_RECORD_PAGETIMER, *PBSCP_RECORD_PAGETIMER;

//������
#ifndef _TAG_BSCP_INTERF_POINT_
#define _TAG_BSCP_INTERF_POINT_

typedef struct tagBSCP_INTERF_POINT
{
	USHORT x;				//������
	USHORT y;				//������
}BSCP_INTERF_POINT, *PBSCP_INTERF_POINT;
typedef BSCP_INTERF_POINT  VCA_POINT;
typedef PBSCP_INTERF_POINT  PVCA_POINT;

#endif

//��������
typedef struct tagBSCP_RECT
{
	ULONG left;				//��
	ULONG top;				//��
	ULONG bottom;			//��
	USHORT right;			//��
	USHORT inUse;			//�Ƿ���ʹ����
	USHORT rev;				//����1
	USHORT rev_t;			//����2
}BSCP_RECT, *PBSCP_RECT;

//�ֳ��趨
typedef struct tagBSCP_RECORD_SCENCE
{
	BSCP_INTERF_POINT mane_xy;		//��ʾͨ����������
	UCHAR mane[MAX_STRINGLEN];		//ͨ����(���16��Ӣ���ַ�)
	BSCP_RECT mask_rect;			//�ڸ�����
	UCHAR mask_display;				//�ڸ�״̬ 0:���ڸ� 1:�ڸ� 2:�ڸǵ�����ʾ
	UCHAR audio;					//�ֳ���Ƶѡ��:1-8,��
	UCHAR mane_display;				//�Ƿ���ʾͨ����
	UCHAR res;						//����
}BSCP_RECORD_SCENCE, *PBSCP_RECORD_SCENCE;

//�ֳ��趨ͨ�����ò���
typedef struct tagBSCP_RECORD_PAGESCENCE
{
	BSCP_RECORD_SCENCE chanState[MAX_CHAN_NUM];	//
	BSCP_INTERF_POINT pip_xy;						//
}BSCP_RECORD_PAGESCENCE, *PBSCP_RECORD_PAGESCENCE;

//�ֳ��趨(��չ��,֧��V6.10B09),��չ���ڸ�������
typedef struct tagBSCP_RECORD_SCENCEEX
{
	BSCP_INTERF_POINT mane_xy;		//��ʾͨ����������
	UCHAR mane[MAX_STRINGLEN];		//ͨ����(���16��Ӣ���ַ�)
	BSCP_RECT mask_rect[4];			//�ڸ�����
	UCHAR mask_display;				//�ڸ�״̬ 0:���ڸ� 1:�ڸ� 2:�ڸǵ�����ʾ
	UCHAR audio;					//�ֳ���Ƶѡ��:1-8,��
	UCHAR mane_display;				//�Ƿ���ʾͨ����
	UCHAR res;						//����
}BSCP_RECORD_SCENCEEX, *PBSCP_RECORD_SCENCEEX;

//�ֳ��趨ͨ�����ò���(��չ��,֧��V6.10B09)
typedef struct tagBSCP_RECORD_PAGESCENCEEX
{
	BSCP_RECORD_SCENCEEX chanState[MAX_CHAN_NUM];	//
	BSCP_INTERF_POINT pip_xy;						//
}BSCP_RECORD_PAGESCENCEEX, *PBSCP_RECORD_PAGESCENCEEX;

//�ֳ��趨��Ѳ���ò���
typedef struct tagBSCP_RECORD_AUTOPATROL
{
	USHORT start;					//�Ƿ�������Ѳ0: ������ 1:����
	USHORT audio;					//�ֳ����� ��,1-8,�涯
	USHORT multi_aduio;				//�໭����Ƶ ��,1-8
	USHORT mode;					//����ģʽ:0:������
	USHORT time;					//��Ѳʱ�� 0-3600��
	USHORT res;						//
	UCHAR order[MAX_CHAN_NUM];		//��Ѳ˳��
}BSCP_RECORD_AUTOPATROL, *PBSCP_RECORD_AUTOPATROL;

//����ʱ��
typedef struct tagBSCP_RECORD_DEFENCETIME
{	
	ULONG bMinute;			//��ʼʱ��ķ� 0-59     
	ULONG eMinute;			//����ʱ��ķ� 0-59
	ULONG bHour;			//��ʼʱ���Сʱ 0-23
	ULONG eHour;			//����ʱ���Сʱ 0-23   
	ULONG sensity;			//������ 0 - 9         
	ULONG quality;			//���� 0 - 5         
	ULONG start;			//�Ƿ����� 0:������ 1:����
	ULONG rev;				//����
}BSCP_RECORD_DEFENCETIME, *PBSCP_RECORD_DEFENCETIME;

//��������
typedef struct tagBSCP_RECALARM_OUTMANAGE
{
	UCHAR alarm;        //�������,��λ����,1Ϊ��Ч
	UCHAR sound;        //������ʾ 0:�� 1:��
	UCHAR net_report;   //�����ϴ� 0:�� 1:��
	UCHAR PTZ;          //��̨���� 0:�� 1:Ԥ��λ 2:�켣 3:Ѳ��·��
	USHORT rev;			//����
	UCHAR pre_set;      //Ԥ��λ��
	UCHAR path;         //Ѳ��·����
}BSCP_RECALARM_OUTMANAGE, *PBSCP_RECALARM_OUTMANAGE;

#ifdef BSCP_ENUM_VALUE

#define MULTI_DETECT_NUM     4
#define MOTION_DETECTSEC_NUM 4

#endif


//�ƶ����ʱ���
typedef struct tagBSCP_RECORD_MOTIONSECT
{
	BSCP_RECT  md_rect[MULTI_DETECT_NUM];		//�������
	BSCP_RECORD_DEFENCETIME time[MOTION_DETECTSEC_NUM];		//����ʱ���
	BSCP_RECALARM_OUTMANAGE alarm_out;			//��������
	USHORT start;								//�Ƿ����� 0:�� 1:��
	USHORT recordType;							//¼������ 0:��ͨ 1:���� 
	USHORT rev;									//����
	USHORT rec_second;							//¼��ʱ�� 10 - 3600
}BSCP_RECORD_MOTIONSECT, *PBSCP_RECORD_MOTIONSECT;

//�ƶ����ҳ
typedef struct tagBSCP_RECORD_PAGEMOTION
{
	BSCP_RECORD_MOTIONSECT MotionDetect[MAX_CHAN_NUM];	//
}BSCP_RECORD_PAGEMOTION, *PBSCP_RECORD_PAGEMOTION;

//������ʱ���ʽ
typedef struct tagBSCP_ALARM_DEFENCETIME
{	
	ULONG bMinute;               //��ʼʱ��ķ� 0-59   
	ULONG eMinute;               //����ʱ��ķ� 0-59  
	ULONG bHour;                 //��ʼʱ���Сʱ 0-23
	ULONG eHour;                 //����ʱ���Сʱ 0-23    
	ULONG start;                 //�Ƿ����� 0:������ 1:����         
	ULONG rev;                   //����
}BSCP_ALARM_DEFENCETIME, *PBSCP_ALARM_DEFENCETIME;

//��ȡ/���ò�����ҳ������
typedef struct tagBSCP_ALARM_PAGEDEFENCE
{
	BSCP_ALARM_DEFENCETIME time[7][4];  //����ʱ��,һ��7��,0-7��ʾ����,��һ...������
	USHORT nStart;						//�Ƿ��������� 0:�� 1:��
	USHORT rev;							//����
	USHORT out_delay[4];				//���������ʱ 0 - 3600��
}BSCP_ALARM_PAGEDEFENCE, *PBSCP_ALARM_PAGEDEFENCE;

//����¼��״̬
typedef struct tagBSCP_ALARM_ALARMRECORDSTAT
{
	UCHAR startup;			//�Ƿ����� 0:������ 1:����
	UCHAR quality;          //�ֶ�¼����
	USHORT rev;             //����
}BSCP_ALARM_ALARMRECORDSTAT, *PBSCP_ALARM_ALARMRECORDSTAT;

//�����쳣�������
typedef enum
{
	INTERF_DISK_ERROR			= 0,		//���̴�
	INTERF_DISK_FULL_ERROR,					//����
	INTERF_NET_ERROR,						//����Ͽ�
	INTERF_USER_ERROR,						//�Ƿ�����
	INTERF_DISK_RW_ERROR,					//���̶�д����
	INTERF_FAN_ERROR,						//���ȴ���
	INTERF_CHANNEL_ERROR,					//ͨ������
	INTERF_DATETIME_ERROR,					//ʱ�����
	INTERF_MAX_ERROR			= 16		//����쳣��
}ALARM_EXCEPTION_TYPE;

//=============>�±����ṹ
//�����¼�����(����BSRCLIENT_ALARMZONESET���ýṹ��)
typedef enum
{
	BSRALARM_EVENT_INPUT		= 0,		//��������,��������Ӿ���
	BSRALARM_EVENT_VLOSS		= 1,		//��Ƶ��ʧ,������ͨ������
	BSRALARM_EVENT_VCOVER		= 2,		//��������,������ͨ������
	BSRALARM_EVENT_VMOTION		= 3,		//�ƶ���ⱨ��,������ͨ������
	BSRALARM_EVENT_CARDNO		= 4,		//���ű���,�ɿ��ž���
	BSRALARM_EVENT_EXCEPT		= 5,		//�쳣����,��������¼��
	BSRALARM_EVENT_MAX			= 6			//�쳣�����ػ���
}ALARM_EVENT_TYPE;

//����ʱ��
typedef struct tagBSCP_ALARM_TIME
{	
	ULONG bMinute;		//��ʼʱ��ķ� 0-59
	ULONG eMinute;		//����ʱ��ķ� 0-59
	ULONG bHour;		//��ʼʱ���Сʱ 0-23
	ULONG eHour;		//����ʱ���Сʱ 0-23
	ULONG bStart;		//�Ƿ����� 0������ 1����
	ULONG nRev;			//����
}BSCP_ALARM_TIME, *PBSCP_ALARM_TIME;

//��������
typedef enum
{
	BSRALARMAREA_24HOUR		= 0,
	BSRALARMAREA_PROMPT		= 1,
	BSRALARMAREA_DELAY		= 2,
	BSRALARMAREA_SIDEROAD	= 3
};

//��������
typedef struct tagBSCP_ALARMZOME_ITEM
{
	USHORT alarm_type;			//������������  0:��· 1:��·
	USHORT alarm_area_type;		//�������� 0:24Сʱ���� 1:��ʱ���� 2:��ʱ���� 3:��·
	USHORT alarm_area_delay;	//������ʱ 0-3600
}BSCP_ALARMZOME_ITEM, *PBSCP_ALARMZOME_ITEM;

//�������
typedef struct tagBSCP_ALARMDEFENCE_SET
{
	BSCP_ALARM_TIME time[7][4];		//����ʱ��,һ��7��,0-7��ʾ����,��һ...������
	USHORT bStart;					//�Ƿ��������� 0:�� 1:��
	USHORT nRev;
}BSCP_ALARMDEFENCE_SET, *PBSCP_ALARMDEFENCE_SET;

//������������
typedef struct tagBSRCLIENT_ALARMZONESET
{
	BSCP_ALARMZOME_ITEM alarmZone[BSRALARM_EVENT_MAX][MAX_ALARMIN_NUM];
	BSCP_ALARMDEFENCE_SET alarmDefence;
}BSRCLIENT_ALARMZONESET, *PBSRCLIENT_ALARMZONESET;

//����¼����չ�ṹ
typedef struct tagBSCP_ALARMRECORD_EXT
{
	USHORT mode;		//ģʽ 0:�ֶ� 1:�Զ�
	USHORT rec_time;	//������¼��ʱ�� 10 - 3600��
	BSCP_ALARM_ALARMRECORDSTAT state[MAX_CHAN_NUM];		//����¼��
}BSCP_ALARMRECORD_EXT, *PBSCP_ALARMRECORD_EXT;

//������ѯ
typedef struct tagBSCP_ALARM_SWITCH
{
	USHORT almSwitchStart;	//�Ƿ�����������ѯ 0:�� 1:��
	USHORT switchtime;		//������ѯʱ��0-3600
}BSCP_ALARM_SWITCH, *PBSCP_ALARM_SWITCH;

//����¼������/��ȡ
typedef struct tagBSRCLIENT_ALARMRECORD_NEW
{
	BSCP_ALARMRECORD_EXT alarm_rec[BSRALARM_EVENT_MAX][MAX_ALARMIN_NUM];
	USHORT rec_time[MAX_CHAN_NUM];
	BSCP_ALARM_SWITCH almSwitch;
}BSRCLIENT_ALARMRECORD_NEW, *PBSRCLIENT_ALARMRECORD_NEW;

//�����������
typedef struct tagBSCP_ALARMOUT_CONTROL
{
	USHORT out_delay[4];		//���������ʱ 0 - 3600��
	ULONG bMinute1;				//��ʼʱ��ķ� 0-59
	ULONG eMinute1;				//����ʱ��ķ� 0-59
	ULONG bHour1;				//��ʼʱ���Сʱ 0-23
	ULONG eHour1;				//����ʱ���Сʱ 0-23
	ULONG out1;					//��Ч�������,����λ����
	ULONG res1;					//����λ1
	ULONG bMinute2;				//��ʼʱ��ķ� 0-59
	ULONG eMinute2;				//����ʱ��ķ� 0-59
	ULONG bHour2;				//��ʼʱ���Сʱ 0-23
	ULONG eHour2;				//����ʱ���Сʱ 0-23
	ULONG out2;					//��Ч�������,����λ����
	ULONG res2;					//����λ2
}BSCP_ALARMOUT_CONTROL, *PBSCP_ALARMOUT_CONTROL;

//�����������
typedef struct tagBSRCLIENT_ALARMOUT_PARA
{
	BSCP_RECALARM_OUTMANAGE alarm_out[BSRALARM_EVENT_MAX][INTERF_MAX_ERROR];
	BSCP_ALARMOUT_CONTROL alarm_out_ctl;
}BSRCLIENT_ALARMOUT_PARA, *PBSRCLIENT_ALARMOUT_PARA;
//<=============�±����ṹ

//��������ʱ���ʽ
typedef struct tagBSCP_ALARMIN_TIME
{
	USHORT alarm_type;					//�������� 0:��· 1:��·
	USHORT alarm_area_type;				//�������� 0:24Сʱ���� 1:��ʱ���� 2:��ʱ���� 3:��·
	USHORT rev;							//����
	USHORT alarm_area_delay;			//������ʱ 0 - 3600
	BSCP_RECALARM_OUTMANAGE alarm_out;	//��������
}BSCP_ALARMIN_TIME, *PBSCP_ALARMIN_TIME;

//��ȡ/���ñ�������ҳ������
typedef struct tagBSCP_ALARM_PAGEALARMIN
{
	BSCP_ALARMIN_TIME alarm_manage[MAX_CHAN_NUM];		//�����������
	BSCP_ALARMIN_TIME vloss_manage[MAX_CHAN_NUM];		//��Ƶ��ʧ����
	BSCP_ALARMIN_TIME vcover_manage[MAX_CHAN_NUM];		//�ڵ���������
	BSCP_ALARMIN_TIME vmotion_manage[MAX_CHAN_NUM];		//�ƶ���ⱨ������
	BSCP_ALARMIN_TIME disk_full_manage;					//������������
}BSCP_ALARM_PAGEALARMIN, *PBSCP_ALARM_PAGEALARMIN;

//����¼��ҳ����
typedef struct tagBSCP_ALARM_PAGERECORDSTAT
{
	USHORT mode;                    //ģʽ 0:�ֶ� 1:�Զ�
	USHORT rec_time;                //������¼��ʱ�� 10 - 3600��
	BSCP_ALARM_ALARMRECORDSTAT state[MAX_CHAN_NUM];      //����¼��
}BSCP_ALARM_PAGERECORDSTAT, *PBSCP_ALARM_PAGERECORDSTAT;

//��ȡ/���ñ���¼��ҳ������
typedef struct tagBSCP_ALARM_PAGERECORD
{
	BSCP_ALARM_PAGERECORDSTAT alarm_rec[MAX_CHAN_NUM];	//
}BSCP_ALARM_PAGERECORD, *PBSCP_ALARM_PAGERECORD;

//��ȡ/�����쳣ҳ������
typedef struct tagBSCP_ALARM_PAGEEXCEPT
{
	BSCP_RECALARM_OUTMANAGE alarm_out[INTERF_MAX_ERROR];	//
}BSCP_ALARM_PAGEEXCEPT, *PBSCP_ALARM_PAGEEXCEPT;

//����ʱ�����
typedef struct tagBSCP_ALARM_TIMECTRL
{
	UCHAR tBeginMinute;		//��ʼʱ��ķ�0-59
	UCHAR tEndMinute;		//����ʱ��ķ�0-59
	UCHAR tBeginHour;		//��ʼʱ���Сʱ0-23
	UCHAR tEndHour;			//����ʱ���Сʱ0-23
	UCHAR nMask;			//�����������,��λ����,1Ϊ�ܿ�
	UCHAR nStart;			//ʹ�ܱ�ʶ,1Ϊ��Ч
	UCHAR res[2];			//����
}BSCP_ALARM_TIMECTRL, *PBSCP_ALARM_TIMECTRL;

//��ȡ/�����쳣ҳ������(V6.10B09��չ)
typedef struct tagBSCP_ALARM_PAGEEXCEPTEX
{
	BSCP_RECALARM_OUTMANAGE alarm_out[INTERF_MAX_ERROR];	//
	BSCP_ALARM_TIMECTRL tmZoneCtrl[4];
}BSCP_ALARM_PAGEEXCEPTEX, *PBSCP_ALARM_PAGEEXCEPTEX;

//��ȡ/����Ԥ¼ҳ������
typedef struct tagBSCP_ALARM_PAGEPRERECORD
{
	UCHAR start;            //�Ƿ�����Ԥ¼
	UCHAR mode;             //ģʽ 0:�ֶ� 1:�Զ�
	UCHAR rec_time;         //Ԥ¼ʱ�� 5,10,15,20,25,30,���
	UCHAR cover_rec;        //�ڵ�����¼�� 0:�� 1:��
	UCHAR loss_rec;         //��Ƶ��ʧ¼�� 0:�� 1:��
	UCHAR rev;				//����
	UCHAR wrev;				//����
	BSCP_ALARM_ALARMRECORDSTAT state[MAX_CHAN_NUM];
}BSCP_ALARM_PAGEPRERECORD, *PBSCP_ALARM_PAGEPRERECORD;

//����HUB��ȡ/���ýṹ
typedef struct tagBSRCLIENT_UARTHUBPARA
{
	UCHAR useHub;			//�Ƿ�ʹ�ô���HUB,0-����,1-����
	UCHAR type[4];			//Э������0-����,1-�ڱ���,2-����,3-BSTAR

	//ע��:���������ṹ��ATM�±����ṹ����ȫ���ظ���
	UCHAR useNBJ;			//�Ƿ������ڱ���,0-����,1-����
	UCHAR cardType;			//����Э������,0-��,1-�����ͨ,2-����,3-���ױ���,4-ӡ��
	UCHAR portBaud[5];			//IN�ڼ�4��PORT,5�����ڵĲ�����
}BSRCLIENT_UARTHUBPARA, *PBSRCLIENT_UARTHUBPARA;
/////////////////////////���ýṹ����///////////////////////////////

typedef enum
{
	ALGBSRALARM_EVENT_INPUT		= 0,		//��������,��������Ӿ���
	ALGALARM_EVENT_INPUT = 0,    
	ALGBSRALARM_EVENT_VLOSS		= 1,		//��Ƶ��ʧ,������ͨ������
	ALGALARM_EVENT_VLOSS = 1, 
	ALGBSRALARM_EVENT_VCOVER	= 2,		//��������,������ͨ������
	ALGALARM_EVENT_VCOVER = 2,
	ALGBSRALARM_EVENT_VMOTION	= 3,		//�ƶ���ⱨ��,������ͨ������
	ALGALARM_EVENT_VMOTION = 3,
	ALGBSRALARM_EVENT_CARDNO	= 4,		//����,�ɿ��ž���
	ALGALARM_EVENT_CARDNO = 4,
	ALGBSRALARM_EVENT_EXCEPT	= 5,		//�쳣����,��������¼��,ע���쳣��Զ���ڵ����ڶ�,�����Ҫ��ӱ�������,����֮ǰ���
	ALGALARM_EVENT_EXCEPT = 5,
	ALGBSRALARM_EVENT_ALG		= 6,		//���ܱ���,������ͨ������
	ALGALARM_EVENT_ALG = 6, 
	ALGBSRALARM_EVENT_MAX		= 7,
	ALGALARM_EVENT_MAX = 7  
}ALGALARM_EVENT_TYPE;
//////////////////////////////B02������Ϣר�ýṹ,�������ֱ�����չ////////////////////////////////////
//=================>���ܻ��ͱ�����չ
//���ܻ��ͱ����¼�����(����ALGBSRCLIENT_ALARMZONESET���ýṹ��,�������ܻ���ʹ��)
typedef BSCP_ALARMZOME_ITEM  VCA_ALARMZOME_ITEM;
typedef PBSCP_ALARMZOME_ITEM  PVCA_ALARMZOME_ITEM;
typedef BSCP_ALARMDEFENCE_SET VCA_ALARMDEFENCE_SET;
typedef	PBSCP_ALARMDEFENCE_SET PVCA_ALARMDEFENCE_SET;
typedef BSCP_ALARMRECORD_EXT   VCA_ALARMRECORD_EXT;
typedef PBSCP_ALARMRECORD_EXT  PVCA_ALARMRECORD_EXT;
typedef BSCP_ALARM_SWITCH   VCA_ALARM_SWITCH;
typedef PBSCP_ALARM_SWITCH  PVCA_ALARM_SWITCH;

//���ܱ�����������
typedef struct tagALGCLIENT_ALARMZONESET   
{   
	VCA_ALARMZOME_ITEM alarmZone[ALGALARM_EVENT_MAX][MAX_ALARMIN_NUM];   
	VCA_ALARMDEFENCE_SET alarmDefence;   
}ALGCLIENT_ALARMZONESET, *PALGCLIENT_ALARMZONESET; 

//���ܱ���¼������/��ȡ
typedef struct tagALGCLIENT_ALARMRECORD   
{   
	VCA_ALARMRECORD_EXT alarm_rec[ALGALARM_EVENT_MAX][MAX_ALARMIN_NUM];   
	USHORT rec_time[MAX_CHAN_NUM];   
	VCA_ALARM_SWITCH almSwitch;   
}ALGCLIENT_ALARMRECORD, *PALGCLIENT_ALARMRECORD;   

//���ܼ�����
#ifndef _ENUM_VCA_ALGTYPE_
#define _ENUM_VCA_ALGTYPE_

typedef enum   
{   
	VCA_DECT_MIN = 0, //��Чֵ,Сֵ�ػ���(�������ܲ�ѯʱ��ʾȫ����������)   
	VCA_VIDEO_TOTALNUM, //����ͳ��   
	VCA_VIDEO_LEFTOBJECT, //������   
	VCA_VIDEO_STUMLINE, //����   
	VCA_VIDEO_PRIMITER, //�ܽ籣��   
	VCA_VIDEO_NONFACE, //���������   
	VCA_VIDEO_LOSSOBJECT, //��ʧ���   
	VCA_VIDEO_SENTRY , //���ڼ��   
	VCA_VIDEO_CONDUCT,	//���м��
	VCA_AUDIO_DETECT,  //��Ƶ����   
	VCA_DECT_MAX       //��Чֵ,��ֵ�ػ���   
}VCA_ALGTYPE;   

#endif

//���ܼ������(ע��:�������õĸ�������߽緶ΧΪ[(2,2), (350,286)],Ҳ����˵��2 ���� �ı߽�)
#ifndef _ENUM_VCA_RECT_
#define _ENUM_VCA_RECT_

typedef enum   
{   
	VCA_RECTWIDTH = 350,  //�������õ�����ϵX ���ֵ   
	VCA_RECTHEIGHT = 280, //�������õ�����ϵY ���ֵ   
	VCA_RECTBORDERX = 2,  //ˮƽ����ı߽�   
	VCA_RECTBORDERY = 2   //��ֱ����ı߽�   
}; 

#endif

#ifndef _TAG_VCA_LINE_
#define _TAG_VCA_LINE_

typedef struct tagVCA_LINE 
{   
	VCA_POINT begin; //���   
	VCA_POINT end;   //�յ�   
}VCA_LINE, *PVCA_LINE;   

#endif

//��Է�CIF��ʽ��Ҫ���н��д�С����ӳ��(����ʱ) 
#ifndef _TAG_VCA_RECT_
#define _TAG_VCA_RECT_

typedef struct tagVCA_RECT   
{   
	VCA_POINT left_up;    //���Ͻ�����   
	VCA_POINT right_down; //���½�����   
}VCA_RECT, *PVCA_RECT;   

#endif


#ifndef _TAG_VCA_QUAD_
#define _TAG_VCA_QUAD_

typedef struct tagVCA_QUAD   
{   
	VCA_POINT points[4]; //�����Ͻǿ�ʼ˳ʱ��   
}VCA_QUAD, *PVCA_QUAD;   

#endif

//ͨ��������� 
#ifndef _TAG_VCA_SPOT_
#define _TAG_VCA_SPOT_

typedef struct tagVCA_SPOT   
{   
	UCHAR vertices;       //2-16 
	UCHAR enabled;        //YES(1)|NO(0) 
	UCHAR type;           //��������(1) |  ��������(2) |  �ų�����(3) 
	UCHAR concerns;       //��ע����Ϊ��(bitmap)��ȡֵ����������Ϣ֡��ʽ�� 
	VCA_POINT points[16]; //�����Ͻǿ�ʼ˳ʱ��   
}VCA_SPOT, *PVCA_SPOT; 

#endif

//ͨ����Ϊ���� 
typedef struct tagVCA_PARAMS_56   
{ 
	USHORT delay_time;    //����ʱ��(��)��0δ���� 
	USHORT alarm_time;    //��������ʱ��(��),10-600,Ĭ��Ϊ30����������/��ʧ����Ч 
	USHORT lo_limit;      //����(0-255)��0 δ����,    //������ͳ����Ч 
	USHORT hi_limit;      //����(2-255)��0 δ���ã�//������ͳ����Ч 
	USHORT aux_port;      //�����������(1-8)��0 δ���ã����������͸�����Ч 
	USHORT scene_choice;
	USHORT stop_overtime;
  USHORT left_scene;    ///* �����ﳡ����ѡ1:��ͨ����2:ATM���� */
	UCHAR resx[16];   
} VCA_PARAMS_56, PVCA_PARAMS_56; 

//ͨ�ö������ 
#ifndef _TAG_VCA_OBJECT_
#define _TAG_VCA_OBJECT_

typedef struct tagVCA_OBJECT   
{ 
	USHORT object_height;       //��׼̽����ʵ�ʸ߶�(��λ: cm)   
	UCHAR res;   
	UCHAR object_depth;         //Ŀ�꾰�����������3--����,1--����   
	VCA_RECT object_figures[3]; //��׼̽��������������   
} VCA_OBJECT, PVCA_OBJECT; 

#endif

#ifndef _TAG_VCA_TIME_
#define _TAG_VCA_TIME_

typedef struct tagVCA_TIME   
{   
	UCHAR bMinute; //��ʼʱ��ķ� 0-59   
	UCHAR eMinute; //����ʱ��ķ� 0-59   
	UCHAR bHour;   //��ʼʱ���Сʱ 0-23   
	UCHAR eHour;   //����ʱ���Сʱ 0-23   
	UCHAR sensitivity; //����������0-9,Ĭ��ֵ�Զ�0(ǰ���뱳���Ĳ���)   
	UCHAR tolerance;   //������ݶ�0-9,Ĭ��ֵ5(����ߴ�仯�ٷֱ�%)   
	UCHAR rev;         //����   
	UCHAR start;       //�Ƿ����� 0:������  1:����   
}VCA_TIME, *PVCA_TIME;  

#endif

//ĳһ���ͷ����㷨   
typedef struct tagVCA_ALG_56   
{   
	UCHAR enabled;    //YES(1)|NO(0) 
	UCHAR algType;    //�㷨���ͣ�ȡֵVCA_ALGTYPE 
	UCHAR res[2];    //������ 
	VCA_PARAMS_56 defined_params;    //��Ϊ������ 
	VCA_OBJECT    defined_object; //��������� 
	VCA_SPOT defined_spots[4];    //���������   
}VCA_ALG_56, *PVCA_ALG_56;   

typedef struct tagVCA_NONFACE_56   
{   
	VCA_PARAMS_56 defined_params;     //��Ϊ������ 
	VCA_OBJECT    defined_object;  //��������� 
	VCA_SPOT defined_spots[4];     //���������   
}VCA_NONFACE_56, *PVCA_NONFACE_56;   

//�ܽ籣����ֵ����   
#define VCA_PERIMTER_AREA_NUM (4) //�ܽ����������   
#define VCA_STUM_LINE_NUM (4) //���������   
#define VCA_LEFT_AREA_NUM (4) //�����������   
#define VCA_LOSS_AREA_NUM (4) //��ʧ�������   
#define VCA_SENTRY_AREA_NUM (1) //�������������  

//�ܽ籣��   
typedef struct tagVCA_PERIMETER_56   
{   
	VCA_PARAMS_56 defined_params;    //��Ϊ������ 
	VCA_OBJECT    defined_object; //��������� 
	VCA_SPOT defined_spots[4];    //���������   
}VCA_PERIMETER_56, *PVCA_PERIMETER_56;   

//����������   
typedef struct tagVCA_STUMLINE_56   
{   
	VCA_PARAMS_56   defined_params;   //��Ϊ������ 
	VCA_OBJECT    defined_object;  //��׼̽�������� 
	VCA_SPOT defined_spots[4];     //��������   
}VCA_STUMLINE_56, *PVCA_STUMLINE_56;  

//����ͳ��   
typedef struct tagVCA_TOTALPEPSONNUM_56   
{   
	VCA_PARAMS_56   defined_params;  //��Ϊ������ 
	VCA_OBJECT    defined_object; //��׼̽�������� 
	VCA_SPOT defined_spots[4];    //������������   
}VCA_TOTALPEPSONNUM_56, *PVCA_TOTALPEPSONNUM_56;   

//��������   
typedef struct tagVCA_LEFTOBJECT_56   
{   
	VCA_PARAMS_56 defined_params;    //��Ϊ������ 
	VCA_OBJECT    defined_object; //��׼̽�������� 
	VCA_SPOT defined_spots[4];    //������������   
}VCA_LEFTOBJECT_56, *PVCA_LEFTOBJECT_56;   

//��ʧ��Ʒ���   
typedef struct tagVCA_LOSSOBJECT_56   
{   
	VCA_PARAMS_56 defined_params; //��Ϊ������ 
	VCA_OBJECT    defined_object; //��׼̽�������� 
	VCA_SPOT defined_spots[4]; //������������   
}VCA_LOSSOBJECT_56, *PVCA_LOSSOBJECT_56;   

//����   
typedef struct tagVCA_SENTRY_56   
{   
	VCA_PARAMS_56 defined_params; //��Ϊ������ 
	VCA_OBJECT    defined_object; //��׼̽�������� 
	VCA_SPOT defined_spots[4]; //��������,����������[0]/������[1]/�ų���[2-3]   
}VCA_SENTRYOBJECT_56, *PVCA_SENTRYOBJECT_56;   

//ĳһͨ�������ܷ����ܽṹ 
typedef struct tagVCA_CFG_56   
{   
	UCHAR start; //�Ƿ�����   
	UCHAR playDisplay; //�Ƿ�ط���ʾ,0-������,1-����   
	UCHAR sceneDisplay; //�Ƿ��ֳ���ʾ,0-������,1-����   
	UCHAR videoSize; //������Ƶ�ķֱ��ʴ�С,1:DCIF 2:CIF 3:1/2D1 4:D1 4:QCIF,��ʱֻ֧��CIF   
	UINT rev; //����   
	VCA_TIME time[7][4]; //���ʱ���,һ��7��,0-7��ʾ����,��һ...������   
	//�����㷨 
	VCA_ALG_56 alg[8];    //ÿͨ����������4 �ֲ�ͬ�����㷨,ͨ�� alg[i].enabled�ж� 
	//��ǰ��ʹ��[0]   
}VCA_CFG_56, *PVCA_CFG_56;   

typedef struct tagVCA_ALGDECTVIDEOPARA_56   
{   
	char channel; //ͨ����ţ�1-16 
	char res[3];    // 
	VCA_CFG_56 vcaPara;
}VCA_ALGDECTVIDEOPARA_56, *PVCA_ALGDECTVIDEOPARA_56;

//////////////////////////////������Ϣר�ýṹ,�������ֱ�����չ////////////////////////////////////
//=================>���ܻ��ͱ�����չ
//���ܻ��ͱ����¼�����(����ALGBSRCLIENT_ALARMZONESET���ýṹ��,�������ܻ���ʹ��)

//���ܱ�����������
typedef struct tagALGBSRCLIENT_ALARMZONESET
{
	BSCP_ALARMZOME_ITEM alarmZone[ALGBSRALARM_EVENT_MAX][MAX_ALARMIN_NUM];
	BSCP_ALARMDEFENCE_SET alarmDefence;
}ALGBSRCLIENT_ALARMZONESET, *PALGBSRCLIENT_ALARMZONESET;


//���ܱ���¼������/��ȡ
typedef struct tagALGBSRCLIENT_ALARMRECORD
{
	BSCP_ALARMRECORD_EXT alarm_rec[ALGBSRALARM_EVENT_MAX][MAX_ALARMIN_NUM];
	USHORT rec_time[MAX_CHAN_NUM];
	BSCP_ALARM_SWITCH almSwitch;
}ALGBSRCLIENT_ALARMRECORD, *PALGBSRCLIENT_ALARMRECORD;

//���ܱ����������
typedef struct tagALGBSRCLIENT_ALARMOUT_PARA
{
	BSCP_RECALARM_OUTMANAGE alarm_out[ALGBSRALARM_EVENT_MAX][INTERF_MAX_ERROR];
	BSCP_ALARMOUT_CONTROL alarm_out_ctl;
}ALGBSRCLIENT_ALARMOUT_PARA, *PALGBSRCLIENT_ALARMOUT_PARA;
//<=================���ܻ��ͱ�����չ

//���ܼ�����
typedef enum
{
	BSR_ALG_DECT_MIN		= 0,	  //��Чֵ,Сֵ�ػ���(�������ܲ�ѯʱ��ʾȫ����������)
	BSR_ALG_VIDEO_TOTALNUM,			//����ͳ��
	BSR_ALG_VIDEO_LEFTOBJECT,		//������
	BSR_ALG_VIDEO_STUMLINE,			//����
	BSR_ALG_VIDEO_PRIMITER,			//�ܽ籣��
	BSR_ALG_VIDEO_NONFACE,			//���������
	BSR_ALG_VIDEO_LOSSOBJECT,		//��ʧ���

	//����ڱ����ְ�ܷ���
	BSR_ALG_VIDEO_ARMYGUARD,  //���ܷ������ڱ����

	BSR_ALG_AUDIO_DETECT,			//��Ƶ����
	BSR_ALG_DECT_MAX				//��Чֵ,��ֵ�ػ���
}BSRALGDECT_ALGDECTTYPE;

//���ܼ������(ע��:�������õĸ�������߽緶ΧΪ[(2,2), (350,286)],Ҳ����˵��2���صı߽�)
typedef enum
{
	BSR_ALG_RECTWIDTH	= 350,		//�������õ�����ϵX���ֵ
	BSR_ALG_RECTHEIGHT	= 286,		//�������õ�����ϵY���ֵ
	BSR_ALG_RECTBORDERX	= 2,		//ˮƽ����ı߽�
	BSR_ALG_RECTBORDERY	= 2			//��ֱ����ı߽�
}; 

//��Է�CIF��ʽ��Ҫ���н��д�С����ӳ��
#ifndef _TAG_BSRALGDECT_RECT_
#define _TAG_BSRALGDECT_RECT_

typedef struct tagBSRALGDECT_RECT
{
	BSCP_INTERF_POINT left_up;		//���Ͻ�����
	BSCP_INTERF_POINT right_down;	//���½�����
}BSRALGDECT_RECT, *PBSRALGDECT_RECT;

#endif

//ʱ�����
typedef struct tagBSRALGDECT_TIME
{
	UCHAR bMinute;                  //��ʼʱ��ķ�0-59
	UCHAR eMinute;                  //����ʱ��ķ�0-59
	UCHAR bHour;                    //��ʼʱ���Сʱ0-23
	UCHAR eHour;                    //����ʱ���Сʱ0-23
	UCHAR sensity;                  //������0-9,Ĭ��ֵ5
	//UCHAR noise;                    //��������ϵ��0-5,Ĭ��ֵ4
	UCHAR tolerance;                //������ݶ�0-9,Ĭ��ֵ5(����ߴ�仯�ٷֱ�%)
	UCHAR rev;						//����
	UCHAR start;                    //�Ƿ�����0:������ 1:����
}BSRALGDECT_TIME, *PBSRALGDECT_TIME;

//��������
typedef struct tagBSRALGDECT_NONFACE
{    
	USHORT Detect_Timer;			//���ʱ��,20~180,Ĭ��ֵ20
	USHORT alm_port;				//������������,��Χ0-5,Ĭ��5
	BSRALGDECT_RECT Defined_size;	//�����������
}BSRALGDECT_NONFACE, *PBSRALGDECT_NONFACE;

//�ܽ籣����ֵ����
#define BSRALGDECT_PERIMTER_AREA_NUM		(4)		//�ܽ����������
#define BSRALGDECT_STUM_LINE_NUM			(4)		//���������
#define BSRALGDECT_LEFT_AREA_NUM			(4)		//�����������
#define BSRALGDECT_LOSS_AREA_NUM			(4)		//��ʧ�������

//�ܽ籣��
typedef struct tagBSRALGDECT_PERIMETER
{
	BSRALGDECT_RECT object_size;	//��׼̽��������
	ULONG Define_num;				//��Ч��������λ����
	BSRALGDECT_RECT Defined_area[BSRALGDECT_PERIMTER_AREA_NUM];	//�ܽ�������
}BSRALGDECT_PERIMETER, *PBSRALGDECT_PERIMETER;

//������������
typedef struct tagBSRALGDECT_PER_STUMLINE
{
	BSRALGDECT_RECT object_area;	//��������
	INT direction;					//����,1-����, 2-����, 0-��ʾ��Ч
}BSRALGDECT_PER_STUMLINE, *PBSRALGDECT_PER_STUMLINE;

//����������
typedef struct tagBSRALGDECT_STUMLINE
{     
	BSRALGDECT_RECT object_size;	//��׼̽��������    
	BSRALGDECT_PER_STUMLINE Defined_area[BSRALGDECT_STUM_LINE_NUM];	//��������
}BSRALGDECT_STUMLINE, *PBSRALGDECT_STUMLINE;

//����ͳ��
typedef struct tagBSRALGDECT_TOTALPEPSONNUM
{  
	USHORT num_limit;				//��������,1-99,Ĭ��Ϊ2
	USHORT delay_time;				//������ʱʱ��,10-600,Ĭ��Ϊ30
	BSRALGDECT_RECT object_size;	//��׼̽��������
	BSRALGDECT_RECT Defined_area;	//������������
}BSRALGDECT_TOTALPEPSONNUM, *PBSRALGDECT_TOTALPEPSONNUM;

//��������
typedef struct tagBSRALGDECT_LEFTOBJECT
{
	USHORT left_time;				//�������ʱ��,10-600,Ĭ��Ϊ30
	USHORT alm_resettime;			//��������ʱ��,10-600,Ĭ��Ϊ30
	USHORT Define_num;				//��Ч��������λ����
	USHORT res;						//����λ
	BSRALGDECT_RECT object_size;	//��׼̽��������
	BSRALGDECT_RECT Defined_area[BSRALGDECT_LEFT_AREA_NUM];	//������������
}BSRALGDECT_LEFTOBJECT, *PBSRALGDECT_LEFTOBJECT;

//��ʧ��Ʒ���
typedef struct tagBSRALGDECT_LOSSOBJECT
{
	USHORT loss_time;				//���ʧʱ��,10-600,Ĭ��Ϊ30
	USHORT alm_resettime;			//��������ʱ��,10-600,Ĭ��Ϊ30
	USHORT Define_num;				//��Ч����,�ֱ���λ����
	USHORT res;						//����λ
	BSRALGDECT_RECT object_size;	//��׼̽��������
	BSRALGDECT_RECT Defined_area[BSRALGDECT_LOSS_AREA_NUM];	//������������
}BSRALGDECT_LOSSOBJECT, *PBSRALGDECT_LOSSOBJECT;

//��Ƶ����ͨ���ṹ
typedef struct tagBSRALGDECT_VIDEOPARA_CHAN
{
	UCHAR start;			//�Ƿ�����
	UCHAR playDisplay;		//�Ƿ�ط���ʾ,0-������,1-����
	UCHAR SenceDisplay;		//�Ƿ��ֳ���ʾ,0-������,1-����
	UCHAR video_size;		//������Ƶ�ķֱ��ʴ�С,1:DCIF 2:CIF 3:1/2D1 4:D1 4:QCIF,��ʱֻ֧��CIF
	USHORT rev;				//����
	USHORT starAlgType;		//�������ܵ�����

	BSRALGDECT_TIME time[7][4];			//���ʱ���,һ��7��,0-7��ʾ����,��һ...������
	BSRALGDECT_PERIMETER perimeter;		//�ܽ籣��
	BSRALGDECT_STUMLINE stum_line;		//����
	BSRALGDECT_LEFTOBJECT leftobject;	//��������
	BSRALGDECT_TOTALPEPSONNUM TotalNum;	//����ͳ��
	BSRALGDECT_NONFACE  NonFace;		//����������
	BSRALGDECT_LOSSOBJECT LossObject;	//��ʧ��Ʒ���
}BSRALGDECT_VIDEOPARA_CHAN, *PBSRALGDECT_VIDEOPARA_CHAN;

//��Ƶ���������ܽṹ
typedef struct tagBSRCLIENT_ALGDECTVIDEOPARA
{
	BSRALGDECT_VIDEOPARA_CHAN algPara[MAX_CHAN_NUM];
}BSRCLIENT_ALGDECTVIDEOPARA, *PBSRCLIENT_ALGDECTVIDEOPARA;






//�����������dvr��Ƶ���ܷ�����ؽṹ����֮ǰ�������豸������
typedef struct tagBSRALGDECT_QUAD
{
	BSCP_INTERF_POINT points[4];  //�����Ͻǿ�ʼ 
}BSRALGDECT_QUAD, *PBSRALGDECT_QUAD;


//���ܷ�������
typedef struct tagBSRALGDECT_POLYGON
{
	unsigned char verices; //2-16;
	unsigned char res[3];
	BSCP_INTERF_POINT points[16];
}BSRALGDECT_POLYGON, *PBSRALGDECT_POLYGON;

#ifndef _TAG_BSRALGDECT_LINE_
#define _TAG_BSRALGDECT_LINE_

typedef struct tagBSRALGDECT_LINE   
{   
	BSCP_INTERF_POINT begin; //���   
	BSCP_INTERF_POINT end; //�յ�   
}BSRALGDECT_LINE, *PBSRALGDECT_LINE;

#endif

#define MAX_STDOBJ_NUM 3 //����������

#ifndef _TAG_BSRALGDECT_OBJECT_
#define _TAG_BSRALGDECT_OBJECT_

typedef struct tagBSRALGDECT_OBJECT
{
	USHORT object_height; //��׼̽����ʵ�ʸ߶�(��λ: cm)  
	UCHAR res; 
	UCHAR object_depth; //Ŀ�꾰�1 ��ʾ�� object_size ��Ч��   
	BSRALGDECT_RECT object_figures[MAX_STDOBJ_NUM]; //��׼̽�����С   
}BSRALGDECT_OBJECT, *PBSRALGDECT_OBJECT;

#endif


//��DVR�ܽ籣���ṹ����
typedef struct tagBSRALGDECT_PERIMETER_EX
{
	unsigned short Define_num;    //��Ч��������λ����
	unsigned short res;
	BSRALGDECT_OBJECT  defined_size;//��׼̽��������
	BSRALGDECT_POLYGON Defined_area[BSRALGDECT_PERIMTER_AREA_NUM]; //�ܽ�������

}BSRALGDECT_PERIMETER_EX, *PBSRALGDECT_PERIMETER_EX;

//������������
#ifndef _TAG_BSRALGDECT_PER_STUMLINE_EX_
#define _TAG_BSRALGDECT_PER_STUMLINE_EX_

typedef struct tagBSRALGDECT_PER_STUMLINE_EX
{
	BSRALGDECT_LINE object_area; //��������
	INT direction;     //����,1-����, 2-����, 0-��ʾ��Ч
}BSRALGDECT_PER_STUMLINE_EX, *PBSRALGDECT_PER_STUMLINE_EX;

#endif

//����������
#ifndef  _TAG_BSRALGDECT_STUMLINE_EX_
#define  _TAG_BSRALGDECT_STUMLINE_EX_

typedef struct tagBSRALGDECT_STUMLINE_EX
{     
	BSRALGDECT_OBJECT  defined_size;//��׼̽��������
	BSRALGDECT_PER_STUMLINE_EX Defined_area[BSRALGDECT_STUM_LINE_NUM]; //��������
}BSRALGDECT_STUMLINE_EX, *PBSRALGDECT_STUMLINE_EX;

#endif


//����ͳ��
typedef struct tagBSRALGDECT_TOTALPEPSONNUM_EX
{  
	USHORT num_limit;    //��������,1-99,Ĭ��Ϊ2
	USHORT delay_time;    //������ʱʱ��,10-600,Ĭ��Ϊ30
	BSRALGDECT_OBJECT  defined_size;//��׼̽��������
	BSRALGDECT_POLYGON Defined_area; //������������
}BSRALGDECT_TOTALPEPSONNUM_EX, *PBSRALGDECT_TOTALPEPSONNUM_EX;

//��������
typedef struct tagBSRALGDECT_LEFTOBJECT_EX
{
	USHORT left_time;    //�������ʱ��,10-600,Ĭ��Ϊ30
	USHORT alm_resettime;   //��������ʱ��,10-600,Ĭ��Ϊ30
	USHORT Define_num;    //��Ч��������λ����
	USHORT res;      //����λ
	BSRALGDECT_OBJECT  defined_size;//��׼̽��������
	BSRALGDECT_POLYGON Defined_area[BSRALGDECT_LEFT_AREA_NUM]; //������������
}BSRALGDECT_LEFTOBJECT_EX, *PBSRALGDECT_LEFTOBJECT_EX;

//��ʧ��Ʒ���
typedef struct tagBSRALGDECT_LOSSOBJECT_EX
{
	USHORT loss_time;    //���ʧʱ��,10-600,Ĭ��Ϊ30
	USHORT alm_resettime;   //��������ʱ��,10-600,Ĭ��Ϊ30
	USHORT Define_num;    //��Ч����,�ֱ���λ����
	USHORT res;      //����λ
	BSRALGDECT_OBJECT  defined_size;//��׼̽��������
	BSRALGDECT_POLYGON Defined_area[BSRALGDECT_LOSS_AREA_NUM]; //������������
}BSRALGDECT_LOSSOBJECT_EX, *PBSRALGDECT_LOSSOBJECT_EX;


#define BSRALGDECT_SENTRY_AREA_NUM 16

//����
typedef struct tagBSRALGDECT_SENTRYOBJECT
{
	USHORT shift_port;   //���ڶ���(1-6)
	USHORT shift_time;    //���ڳ���ʱ��(��) 
	USHORT Define_num;    //��Ч����,�ֱ���λ����
	USHORT zones;   ////  bit0 = Ϊ��������, bit1 = ��ʾ��������2-15��ʾ�ų�����Ŀǰ����

	BSRALGDECT_OBJECT  defined_size;                              //��׼̽��������
	BSRALGDECT_POLYGON defined_area;  //������������   
	BSRALGDECT_POLYGON guard_area;    //������������ 

}BSRALGDECT_SENTRYOBJECT, *PBSRALGDECT_SENTRYOBJECT;

//��Ƶ����ͨ���ṹ
typedef struct tagBSRALGDECT_VIDEOPARA_CHAN_EX
{
	UCHAR start;   //�Ƿ�����
	UCHAR playDisplay;  //�Ƿ�ط���ʾ,0-������,1-����
	UCHAR SenceDisplay;  //�Ƿ��ֳ���ʾ,0-������,1-����
	UCHAR video_size;  //������Ƶ�ķֱ��ʴ�С,1:DCIF 2:CIF 3:1/2D1 4:D1 4:QCIF,��ʱֻ֧��CIF
	USHORT rev;    //����
	USHORT starAlgType;  //�������ܵ�����

	BSRALGDECT_TIME time[7][4];   //���ʱ���,һ��7��,0-7��ʾ����,��һ...������
	BSRALGDECT_PERIMETER_EX perimeter;  //�ܽ籣��
	BSRALGDECT_STUMLINE_EX stum_line;  //����
	BSRALGDECT_LEFTOBJECT_EX leftobject; //��������
	BSRALGDECT_TOTALPEPSONNUM_EX TotalNum; //����ͳ��
	BSRALGDECT_NONFACE  NonFace;  //����������
	BSRALGDECT_LOSSOBJECT_EX LossObject; //��ʧ��Ʒ���
	BSRALGDECT_SENTRYOBJECT sentryObject; //����
}BSRALGDECT_VIDEOPARA_CHAN_EX, *PBSRALGDECT_VIDEOPARA_CHAN_EX;

typedef struct tagCLIENT_ALGDECTVIDEOPARA   
{   
	char channel; //ͨ����ţ�1-16 

	char res[3];    // 

	BSRALGDECT_VIDEOPARA_CHAN_EX algPara;

}CLIENT_ALGDECTVIDEOPARA, *PCLIENT_ALGDECTVIDEOPARA;

//�¼�������չ�ӿ�
typedef struct tagCLIENT_ALGATMSCENEOBJ //atmȫ���ӿڲ����ṹ
{
  unsigned char channel; //ͨ�����1-16;
  unsigned char algType; //��Ӧ����������;
  unsigned char res1[2];
  BSRALGDECT_RECT peopleRec; //̽����Ա����
  

}CLIENT_ALGATMSCENEOBJ;

//��Ƶ����ͨ���ṹ
typedef struct tagBSRALGDECT_AUDIOPARA_CHAN
{
	UCHAR start;				//�Ƿ�����
	UCHAR rev[3];				//����λ
	BSRALGDECT_TIME time[7][4];	//���ʱ���,һ��7��,0-7��ʾ����,��һ...������
	UCHAR bottomTresh0;			//��Χ0~100 
	UCHAR topThresh0;			//��ΧbottomTresh0~100      
	UCHAR bottomTresh1;			//��Χ0~100
	UCHAR topThresh1;			//��ΧbottomTresh0~100
}BSRALGDECT_AUDIOPARA_CHAN, *PBSRALGDECT_AUDIOPARA_CHAN;

//��Ƶ���������ܽṹ
typedef struct tagBSRCLIENT_ALGDECTAUDIOPARA
{
	BSRALGDECT_AUDIOPARA_CHAN AudioAlm[MAX_CHAN_NUM];
}BSRCLIENT_ALGDECTAUDIOPARA, *PBSRCLIENT_ALGDECTAUDIOPARA;
//////////////////////////////������Ϣ�ṹ////////////////////////////////////


////////////////////////DVS���ݽṹ����///////////////////////////////////
typedef struct tagBSRCLIENT_DVS_NICITEM
{
	INT sn;				//���0-17
	SHORT type;			//���Ͷ��ڵڶ���������Ч
	SHORT port;			//�˿ڶ��ڵڶ���������Ч
	ULONG addr;			//ip��ַ
}BSRCLIENT_DVS_NICITEM, *PBSRCLIENT_DVS_NICITEM;

//DVS�ڶ�����������֮���DVR��������Ϣ
typedef struct tagBSRCLIENT_DVS_NICGROUP
{
	BSRCLIENT_DVS_NICITEM nicInfo[17];	//������Ϣ��
}BSRCLIENT_DVS_NICGROUP, *PBSRCLIENT_DVS_NICGROUP;

//���DVR״̬��
typedef enum
{
	BSRCLIENT_DVSNOTEXIST	= 0,	//������
	BSRCLIENT_DVSTIMEOUT	= 1,	//PING��ʱ
	BSRCLIENT_DVSCORRECT	= 2		//������Ӧ
}BSRCLIENT_DVSDVRSTATUS;

//DVS���¼���״̬
typedef struct tagBSRCLIENT_DVS_DVRSTATUS
{
	UCHAR nDVRState[16];		//���庬��μ�BSRCLIENT_DVSDVRSTATUS
}BSRCLIENT_DVS_DVRSTATUS, *PBSRCLIENT_DVS_DVRSTATUS;

typedef struct tagBSRCLIENT_DVS_ALARMZONEITEM
{
	UCHAR bypass;				//��·����,��ʱ����,00-ʹ��,01-��·
	UCHAR alarm_type;			//������������00-����,01-����
	UCHAR alarm_area_type;		//��������00-��·,01-24Сʱ,02-��ʱ,03-��ʱ
	UCHAR res;					//����λ
}BSRCLIENT_DVS_ALARMZONEITEM, *PBSRCLIENT_DVS_ALARMZONEITEM;

//1644������������ṹ
typedef struct tagBSCPDVS_ALARMZONEGROUP
{
	USHORT queue_time;			//����Ƶ�Ŷ�ʱ��,0-3600��
	USHORT entry_delay;			//�����ӳ�,0-3600��
	USHORT exit_delay;			//�˳��ӳ�,0-3600��
	UCHAR defence_start;		//��������
	UCHAR res;					//�����ֶ�
	BSRCLIENT_DVS_ALARMZONEITEM alarmzone[MAX_ALARMIN_NUM];
}BSCPDVS_ALARMZONEGROUP, *PBSCPDVS_ALARMZONEGROUP;

typedef enum
{
	INTERF_M1644_VIDEO_IN	= 16,
	INTERF_M1644_AUDIO_IN	= 16,
	INTERF_M1644_VIDEO_CHAN	= 8,
	INTERF_M1644_UART_CHAN	= 4,
	INTERF_M1644_ALARM_OUT	= 4,
}BSRCLIENT_DVS_MAXVALUE;

typedef struct tagBSRCLIENT_DVS_ALARMOUTITEM
{
	UCHAR alarm[INTERF_M1644_ALARM_OUT];		//�̵�������,��4��,00-������,01-����
	USHORT alarm_time[INTERF_M1644_ALARM_OUT];	//�̵�������ʱ��,0-3600��
	UCHAR sound;								//����������,00 ������,01ģʽ1,02ģʽ2,03ģʽ3
	USHORT sound_time;							//����������ʱ��,0-3600��
	UCHAR av_switch;							//�Զ��л�����,00-������,01-��Ƶ,02-����Ƶ
	UCHAR ch[INTERF_M1644_VIDEO_CHAN];			//�Զ��л�ͨ��,�����±����8�����,ֵ��������,����Ϊ1-16ͨ��
	UCHAR ptz;									//�Ƿ������̨00-������,01-����
	UCHAR ptz_proto;							//��̨Э��00-PELCOD, 01-PELCOP, 02-JVC
	UCHAR ptz_addr;								//��̨��ַ
	UCHAR ptz_preset;							//��̨Ԥ��λ
}BSRCLIENT_DVS_ALARMOUTITEM, *PBSRCLIENT_DVS_ALARMOUTITEM;

//1644������������ṹ
typedef struct tagBSCPDVS_ALARMOUTGROUP
{
	BSRCLIENT_DVS_ALARMOUTITEM alarmout[INTERF_MAX_ERROR];
}BSCPDVS_ALARMOUTGROUP, *PBSCPDVS_ALARMOUTGROUP;

typedef struct tagBSRCLIENT_DVS_SCENESWITCHITEM
{
	UCHAR start;			//�����Զ��л�00-ֹͣ,01 - ����
	UCHAR mode;				//�Զ��л�ģʽ00��Ƶ,01��Ƶ,02����Ƶ
	USHORT switch_time;		//�Զ��л�ʱ����,0-3600��
	USHORT channel;			//�Զ��л�ͨ���Ű�λ����
	USHORT wres;			//�����ֶ�
}BSRCLIENT_DVS_SCENESWITCHITEM, *PBSRCLIENT_DVS_SCENESWITCHITEM;

//1644�ֳ��Զ��л�״̬
typedef struct tagBSRCLIENT_DVS_SCENESWITCHGROUP
{
	BSRCLIENT_DVS_SCENESWITCHITEM videoch[INTERF_M1644_VIDEO_CHAN];
}BSRCLIENT_DVS_SCENESWITCHGROUP, *PBSRCLIENT_DVS_SCENESWITCHGROUP;

typedef struct tagBSRCLIENT_DVS_DEVCONFIGITEM
{
	UCHAR baud_rate;		//������00-1200, 01-2400, 02-4800, 03-9600
	UCHAR bres;				//����λ
	UCHAR wres;				//����λ
}BSRCLIENT_DVS_DEVCONFIGITEM, *PBSRCLIENT_DVS_DEVCONFIGITEM;

//1644Ӳ������
typedef struct tagBSRCLIENT_DVS_DEVCONFIGGROUP
{
	UCHAR targetid;			//644EN������ַ
	UCHAR dvr_proto;		//00-BSR-DVRЭ��
	UCHAR ptz_proto;		//00:PELCO_D, 01:PELCO_P, 02:JVC��̨Э��
	UCHAR res;				//����λ
	BSRCLIENT_DVS_DEVCONFIGITEM uart[INTERF_M1644_UART_CHAN];	//0-����,1-¼���,2-��̨,3-RS232
}BSRCLIENT_DVS_DEVCONFIGGROUP, *PBSRCLIENT_DVS_DEVCONFIGGROUP;

//1644EN�������״̬
typedef struct tagBSRCLIENT_DVS_NETCTLSTATUS
{
	INT PanelLock;			//00-���粻�ɿ�,01-����ɿ�
}BSRCLIENT_DVS_NETCTLSTATUS, *PBSRCLIENT_DVS_NETCTLSTATUS;

///////////��ѶЭ��ṹ///////////
//��ǰע��״̬
#ifndef _TAG_NET_SEC_BUREAU_STATE_
#define _TAG_NET_SEC_BUREAU_STATE_

typedef struct tagNET_SEC_BUREAU_STATE
{
	SHORT status;		//��ǰע��״̬��С��0Ϊ���ߣ� WAITING(-2), TRYING(-1), OFFLINE(0), ONLINE(1)
	SHORT error;		//�ϴ�ע�᷵�صĴ�����
	UINT  serverAddr;	//��ǰ��������ַ
} BSRCLIENT_NET_SEC_BUREAU_STATE, *PBSRCLIENT_NET_SEC_BUREAU_STATE;

#endif

//ע�����������
#ifndef _TAG_NET_SEC_BUREAU_CFG_
#define _TAG_NET_SEC_BUREAU_CFG_

typedef struct tagNET_SEC_BUREAU_CFG
{
	CHAR	enabled;	//NO(0) | YES(1) | WAKEN(2), �Ƿ�����ע�Ṧ��,ȱʡ2 
	CHAR	dial;		//NONE(0)|PPPOE(1)|CDMA(2)
	CHAR	res[2];
	UINT	serverAddr1; //ע�������IPAddr,ȱʡΪ0 
	UINT	serverAddr2; //ע�������IPAddr,ȱʡΪ0
	USHORT	serverPort; //ע��������˿�
	USHORT	resx; 
}BSRCLIENT_NET_SEC_BUREAU_CFG, *PBSRCLIENT_NET_SEC_BUREAU_CFG;

#endif

////////////////////////DVS���ݽṹ����///////////////////////////////////


/////////////////////////�¼����翨��////////////////////////////////////////
/*  atm����Э��ṹ��   */

typedef struct
{
  unsigned char link;     /*  ���ӷ�ʽ    */
  /*  RS232(1)    RS485(2)    RS232Hub(3) RJ45(4) */

  unsigned char proto;    /*  ����Э��    */
  unsigned char forwards; /*  ����ת������:NONE(0)    CARDNO(1)   */
  unsigned char res;  /*  reserved    */
  unsigned long addr;  /*  ��������ַ  IP��ַ��rs485��ַ   */
  unsigned int period;    /*  ����ʱ��(¼��ʱ��,��λ ��)  */

  /* proto����Э��
  * Э���ţ� 
  *
  * ����(link=RJ45)�� 
  * �½�(1)|�ɶ�(2)|����(3)|ӡ��(4)|
  * ����(5)|���ױ���(6)�Ϻ�(7) 
  *
  * ����(link=RS232)�� 
  * �����ͨ(1)|����(2)|���ױ���(3)|
  * ӡ��(4)|NCR(5)|����(6) 
  */

}BSRCLIENT_NETCARDCFG;


/*�����������*/
struct BSRCLIENT_NETDETECT
{
  unsigned long serverip;      /*�ο�IP��Ĭ����124.65.141.254*/
  unsigned char retry;         /*ping�Ĵ���*/
  unsigned char controlport;   /*ping���ܿ��ƶ˿�*/
  unsigned char timerenabled;  /*��ʱping�����Ƿ�����1:������0:�ر�,ȱʡ�ر�*/
  unsigned char recv0;                                 
  BSCP_DATATIME DateTime1;         /*��ʱping ʱ���1*/
  BSCP_DATATIME DateTime2;         /*��ʱping ʱ���2*/
  unsigned char periodicenable; /*��ʱ����ping�����Ƿ�����1:������0:�ر�,ȱʡ�ر�*/
  unsigned char recv[3];                                   
  unsigned long periodictime;    /*ʱ����,��λ����*/

} ;

/*����ע����������*/
struct BSRCLIENT_RIGISTEREXP
{
  char enabled; //0 do not enable 
                //1 enable ����ת���ڱ��ֲ����ı��� 
                // 2 enable ���ǲ�ת���ڱ��ֱ���
  char res; 
  char link; //!�������ͣ���������(1) 
  char port; //��ۺţ�1-4
};

/**����ע���������*/
struct BSRCLIENT_AUTOREGISTER
{
  char            enabled;    //!�Ƿ�����ע�Ṧ��,ȱʡ 0   //NO(0) | YES(1) 
  char            res[3]; 
  unsigned int    serverAddr;  //ע������� IPAddr,ȱʡΪ 0  
  unsigned short  serverPort;  //ע��������˿ڣ�ȱʡΪ 8008 
  unsigned char   retries;  //����ʧ�ܵ�������Դ��� //0 Ϊ�������� //ȱʡΪ5  
  unsigned char   res2;   
  unsigned int serverKey;   //ע���������Ҫ����֤�룬 //ȱʡΪ123456 
	unsigned int register_standby;    //ע��ʧ�ܺ��ٴη���ע���ʱ����(��λ:��)
};

/** ������� */
struct BSRCLIENT_STANDBY
{
  unsigned int register_standby;  //������standbyʱ��
};

/** �Զ�ע��״̬����״ֻ̬�� */
struct BSRCLIENT_REGISTERSTAT
{
  short status;   //!��ǰע��״̬,<=0 Ϊ���� 
                 //WAITING(-2)|TRYING(-1)|  
                 //OFFLINE(0) |ONLINE (1)| 
  short error;   //�ϴ�ע�᷵�صĴ����� 
  char  serverName[16];   //��ǰע��ɹ��ķ���������
};

/** ͨ�������ϴ�״̬ */
struct BSRCLIENT_CHNWARNREPORTSTAT 
{
  unsigned long device_num; /* dvr ��� ��0��ʼ */ 
  unsigned long state[16]; /* 16����1-16ͨ�� */ /* ״̬��λ���� ת����� YES(1) NO(0)*/ /* bit1-32:bitmap ����ʹ��(8)|������ֹ(7) |����(6)|����(5)| Ӳ�̴���(4)|��Ƶ��ʧ(3)| �ڵ�(2)|�ƶ����(1)| ��������(0) */
};

/** �ڱ���Э������ */
struct BSRCLIENT_REGISTERAGENT
{
  char enabled; //�����أ�
  char proto; //�ڱ���(1) 
  char link; //!�������ͣ� //RS232 (1)|RS485 
  char port; //�˿ںţ�1-4
};

/** �ֶ������� */
struct BSRCLIENT_MANUALOPRATING
{
  char res; //����
};
////////////////////////////API//////////////////////////////////////////////
//���������
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetLastError();
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetLastError(LONG nError);

//�������
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_IsSupport();
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetSDKVersion();
BSRCLIENT_API LPCSTR BSRCLIENT_STDCALL BSR_DVR_GetSDKPubishName();
BSRCLIENT_API LPCSTR BSRCLIENT_STDCALL BSR_DVR_GetSDKUpdateInfos();
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RegistMessage(HWND hWnd, UINT nMessage);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetConnectTime(DWORD dwWaitTime, DWORD dwTryTimes);
BSRCLIENT_API VOID BSRCLIENT_STDCALL BSR_DVR_SetDebugPara(LONG nLevel, BOOL bScopeMode);

//SDK��ʼ��
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_InitSDK();
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DeInitSDK();

//��¼���
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_Login(LPCSTR lpDVRIP, WORD wDVRPort, LPCSTR lpUserName, LPCSTR lpPasswd, BSRCLIENT_DEVICEINFO *lpDeviceInfo);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_Logout(HANDLE hUserHandle);

//Ԥ�����
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_StartRealPlay(HANDLE hUserHandle, BSRCLIENT_CLIENTINFO *lpClientInfo);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_StopRealPlay(HANDLE hRealHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetChannelTitle(HANDLE hRealHandle, LPCSTR lpszTitle, BOOL bDVRShow);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetChannelTitleOther(HANDLE hUserHandle, LONG nChannel, LPCSTR lpszTitle, BOOL bDVRShow);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetOSDScenePara(HANDLE hRealHandle, BOOL bShow, COLORREF *lpColor);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetOSDTimePara(HANDLE hRealHandle, BOOL bShow, POINT *lpPoint, COLORREF *lpColor);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetOSDTitlePara(HANDLE hRealHandle, BOOL bShow, POINT *lpPoint, COLORREF *lpColor);

BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RegistDrawFunc(HANDLE hRealHandle, CLIENTTITLEDRAWFUN lpDrawFunProc, LPVOID lpVoid);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RegistWndHookFunc(HANDLE hRealHandle, CLIENTHWNDHOOKFUN lpWndFunProc, LPVOID lpVoid);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetRealDataCallBack(HANDLE hRealHandle, CLIENTDATACALLBACKFUN lpfnReal, LPVOID lpVoid);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetRealDataCallBackXM( HANDLE hRealHandle, fRealDataXM lpfnReal, LPVOID lpVoid );

BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_CaptureBMPPicture(HANDLE hRealHandle, LPCSTR lpszFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SaveRealData(HANDLE hRealHandle, LPCSTR lpszFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_StopSaveRealData(HANDLE hRealHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetRealSaveStat(HANDLE hRealHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetPlayerBufNumber(HANDLE hRealHandle, DWORD dwBufNum);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetVideoEffectByReal(HANDLE hRealHandle, LONG *lpBright, LONG *lpTrans, LONG *lpContrast);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetVideoEffectByReal(HANDLE hRealHandle, UINT nType, LONG nValue);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetVideoEffectByUser(HANDLE hUserHandle, LONG nChannel, LONG *lpBright, LONG *lpTrans, LONG *lpContrast);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetVideoEffectByUser(HANDLE hUserHandle, LONG nChannel, UINT nType, LONG nValue);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_OpenSound(HANDLE hRealHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_CloseSound();
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetVolume(HANDLE hRealHandle, DWORD dwVolume);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetVideoStreamPara(HANDLE hRealHandle, UINT nType, INT *lpValue);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetVideoStreamPara(HANDLE hRealHandle, UINT nType, INT nValue);
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_GetUserHandle(HANDLE hRealHandle);
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetPlayerPort(HANDLE hRealHandle);
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetStreamID(HANDLE hRealHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetIconState(HANDLE hRealHandle, BOOL *lpVideoCap, BOOL *lpAudioCom);

//�ֳ������ƺ��������ຯ��ֻ����򿪡��ر��ֳ���ID

//�����ֳ���
BSRCLIENT_API int  BSRCLIENT_STDCALL BSR_DVR_RealStreamOpen(HANDLE hUserHandle, BSRCLIENT_CLIENTINFO *lpClientInfo);
//�ر��ֳ���
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RealStreamClose(HANDLE hUserHandle, int streamId );
//��ʼ������Ǳ���ģʽ��������ô˺�����ʼ���ݴ��䣬��������ģʽ���ڵ���BSR_DVR_RealStreamStop����֮��ʼ�ֳ�������
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RealStreamStart(HANDLE hUserHandle, int streamId );
//��ͣ�ֳ������䣬�������������Ǳ���
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RealStreamPause(HANDLE hUserHandle,  int streamId );

BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RealCapOneBmp( HANDLE hDvr, int chnnl, int port, HWND hWnd, LPCSTR pathname, BOOL bShowTime );

//�����Խ�
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_VoiceComStartByUser(HANDLE hUserHandle, BSRCLIENT_CLIENTINFO *lpClientInfo);
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_VoiceComStartByReal(HANDLE hRealHandle, WORD wDVRPort);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_VoiceComStop(HANDLE hVoiceHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_VoiceComSendData(HANDLE hVoiceHandle, BYTE *lpBuffer, DWORD dwSize);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetTalkStreamPara(HANDLE hVoiceHandle, UINT nType, INT *lpValue);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetTalkStreamPara(HANDLE hVoiceHandle, UINT nType, INT nValue);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetVoiceStatus(HANDLE hUserHandle, LONG nStatus, LONG nChannel);



//��Ƶ�ɼ�
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ClientAudioStart(BSRCLIENT_AUDIOWAVEIN *lpWaveInPara);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ClientSetVoiceCallBack(VOICEDATACALLBACKFUN lpfnWaveProc, LPVOID lpVoid, UINT nType);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ClientAudioStop();

//����ؿ���
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RtpStateCtrl(HANDLE hUserHandle, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PurgeVideoStream(HANDLE hUserHandle, LONG nSessionID);
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetVideoStreamList(HANDLE hUserHandle, BSRCLIENT_STREAMLIST *lpList);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetVideoStreamInfo(HANDLE hUserHandle, BSRCLIENT_STREAMINFOS *lpInfos);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_CloseVideoStream(HANDLE hUserHandle, LONG nStreamID);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PurgeTalkStream(HANDLE hUserHandle, LONG nSessionID);
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetTalkStreamList(HANDLE hUserHandle, BSRCLIENT_STREAMLIST *lpList);

//�û���Ϣ
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetSessionID(HANDLE hUserHandle);
BSRCLIENT_API LPCSTR BSRCLIENT_STDCALL BSR_DVR_GetObjectIP(HANDLE hUserHandle);
BSRCLIENT_API LPCSTR BSRCLIENT_STDCALL BSR_DVR_GetCurUserName(HANDLE hUserHandle);
BSRCLIENT_API LPCSTR BSRCLIENT_STDCALL BSR_DVR_GetCurPassword(HANDLE hUserHandle);

//DVR�û�����
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DeleteUser(HANDLE hUserHandle, DWORD dwUserID);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ModifyUser(HANDLE hUserHandle, DWORD dwUserID, LPCSTR lpszUserName, LPCSTR lpszPasswd, DWORD dwUserGrade);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_AddUser(HANDLE hUserHandle, DWORD dwUserID, LPCSTR lpszUserName, LPCSTR lpszPasswd, DWORD dwUserGrade);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetLoginUsers(HANDLE hUserHandle, BSCP_USER_CURLOGINUSR *lpUsers);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetAllUsers(HANDLE hUserHandle, BSCP_USER_ALLUSER *lpUsers);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_KickNetUser(HANDLE hUserHandle, LONG nSession);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDialogInfos(HANDLE hUserHandle, BSRCLIENT_DIALOGINFOS *lpInfos);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetTransInfos(HANDLE hUserHandle, BSRCLIENT_TRANSCHANINFO *lpInfos);

//�������
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_StartListen(LPCSTR lpszLocalIP, WORD wLocalPort);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_StopListen();
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetDVRMsgCallBack(MESSAGECALLBACKFUN lpfnMsgCallBack, LPVOID lpVoid);

//�澯����
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DefenceCtrl(HANDLE hUserHandle, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PreRecordCtrl(HANDLE hUserHandle, BOOL bStart);

//������
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SendVirtualKey(HANDLE hUserHandle, INT nVirtualKey);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetPassApplyStat(HANDLE hUserHandle, BOOL bApply);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetPassApplyStat(HANDLE hUserHandle, BOOL *lpApply);

//��̨����
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZConfirmCruisePath(HANDLE hUserHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZGetAllCruisePath(HANDLE hUserHandle, BSCP_CONTROL_CRUISEPOSLIST *lpAllCruisePath);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZPreset(HANDLE hRealHandle, DWORD dwPTZPresetCmd, DWORD dwPresetIndex);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZPresetOther(HANDLE hUserHandle, LONG nChannel, DWORD dwPTZPresetCmd, DWORD dwPresetIndex);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZAddCruisePos(HANDLE hRealHandle, DWORD dwPTZPathIndex, DWORD dwPTZDotIndex, DWORD dwPresetIndex, DWORD dwSpeed, DWORD dwStopTime);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZAddCruisePosOther(HANDLE hUserHandle, LONG nChannel, DWORD dwPTZPathIndex, DWORD dwPTZDotIndex, DWORD dwPresetIndex, DWORD dwSpeed, DWORD dwStopTime);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZDelCruisePos(HANDLE hRealHandle, DWORD dwPTZPathIndex, DWORD dwPTZDotIndex);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZDelCruisePosOther(HANDLE hUserHandle, LONG nChannel, DWORD dwPTZPathIndex, DWORD dwPTZDotIndex);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZCruiseCtrl(HANDLE hRealHandle, DWORD dwPTZPathIndex, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZCruiseCtrlOther(HANDLE hUserHandle, LONG nChannel, DWORD dwPTZPathIndex, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZTrackCtrl(HANDLE hRealHandle, DWORD dwPTZCommand);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZTrackCtrlOther(HANDLE hUserHandle, LONG nChannel, DWORD dwPTZCommand);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZAssistCtrl(HANDLE hRealHandle, DWORD dwAuxIndex, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZAssistCtrlOther(HANDLE hUserHandle, LONG nChannel, DWORD dwAuxIndex, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZControl(HANDLE hRealHandle, DWORD dwCtrlPara);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZControlOther(HANDLE hUserHandle, LONG nChannel, DWORD dwCtrlPara);

//�ֳ�����
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ScenePatrolCtrl(HANDLE hUserHandle, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SceneDisplayInfos(HANDLE hUserHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SceneDisplayString(HANDLE hUserHandle, LPCSTR lpszString, UINT nPos);

//¼�����
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RecordCtrl(HANDLE hUserHandle, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ModifyQuality(HANDLE hUserHandle, LONG nChannel, LONG nQuality);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DVRLocalPlayCtrl(HANDLE hUserHandle, DWORD dwPlayCommand);

//��CPU����
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SCPUReboot(HANDLE hUserHandle, UINT nCPUIndex);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SCPURecordCtrl(HANDLE hUserHandle, LONG nChannel, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SCPUGetStates(HANDLE hUserHandle, LONG nChannel, BSCP_SLAVECPU_SCPUINFO *lpStates);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SCPUGetStatus(HANDLE hUserHandle, LONG nChannel, BSCP_SLAVECPU_SCPUSTATUS *lpStatus);

//��������
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDVRConfig(HANDLE hUserHandle, DWORD dwCommand, LPVOID lpOutBuffer, DWORD dwBufSize, LPDWORD lpBytesRecved);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetDVRConfig(HANDLE hUserHandle, DWORD dwCommand, LPVOID lpInBuffer, DWORD dwBufSize);

//�豸�����Ϣ
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetBriefDevInfo(HANDLE hUserHandle, PBSRCLIENT_DEVICEINFO lpDeviceInfo);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDeviceSysInfos(HANDLE hUserHandle, BSCP_DVR_DEVICESYSINFO *lpDevinfo);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDeviceStatus(HANDLE hUserHandle, BSCP_DVR_DEVICESTAT *lpDevStatus);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDeviceInfos(HANDLE hUserHandle, BSCP_DVR_DEVICEINFO *lpDevInfo);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetActualChans(HANDLE hUserHandle, BSRCLIENT_ACTUALCHANS *lpChanInfos);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDeviceLoadInfos(HANDLE hUserHandle, BSRCLIENT_DVRLOADINFO *lpDVRLoadInfos);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDeviceRunInfos(HANDLE hUserHandle, BSRCLIENT_DVRRUNINFO *lpDVRRunInfos);

//�������
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SendEmail(HANDLE hUserHandle, LPCSTR lpszContent);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetCardNumber(HANDLE hUserHandle, LPCSTR lpszCardNumber);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DVRLocalControl(HANDLE hUserHandle, DWORD dwCommand, DWORD dwCtrlPara);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_Reboot(HANDLE hUserHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RestoreConfig(HANDLE hUserHandle);

//���ڴ���
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetUartAgentPara(HANDLE hUserHandle, DWORD dwLinkType, DWORD dwPort, BSRCLIENT_UARTAGENTPARA *lpAgentPara);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetUartAgentPara(HANDLE hUserHandle, BSRCLIENT_UARTAGENTPARA *lpAgentPara);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SendUartData(HANDLE hUserHandle, DWORD dwLinkType, DWORD dwPort, BYTE *lpBuffer, DWORD dwDataSize);

//��־����
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_FindDVRLogs(HANDLE hUserHandle, LPCSTR lpszOperator, DWORD dwMajorType, DWORD dwMinorType, SYSTEMTIME *lpBegin, SYSTEMTIME *lpEnd, LONG *lpPageCount, LONG *lpLogCount);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindNextLog(HANDLE hLogHandle, LONG nPageIndex, BSCP_DVR_LOG *lpLogData, DWORD *lpdwCount);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindLogClose(HANDLE hLogHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ClearAllLogs(HANDLE hUserHandle, DWORD dwDiskNumber);

//Զ������
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_RemoteUpgrade(HANDLE hUserHandle, WORD wDVRPort, LPCSTR lpszFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_CloseUpgradeHandle(HANDLE hUpgradeHandle);
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetUpgradeState(HANDLE hUpgradeHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_MakeUpgradeDisk(HANDLE hUserHandle);

//���̲���
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SleepHardDisk(HANDLE hUserHandle, DWORD dwDiskNumber);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_WakenHardDisk(HANDLE hUserHandle, DWORD dwDiskNumber);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FormatHardDisk(HANDLE hUserHandle, DWORD dwDiskNumber);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDiskInfo(HANDLE hUserHandle, DWORD dwDiskNumber, BSRCLIENT_DISKINFO *lpDiskInfo);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetDiskType(HANDLE hUserHandle, DWORD dwDiskNumber, UINT nType, UINT nSubType);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_WriteDisk(HANDLE hUserHandle, LONG nLBAAddress, LONG nCount, LONG nValue);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ChangeDisk(HANDLE hUserHandle, DWORD dwDiskNumber);

//�ļ�����

BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_FindFileByTime(HANDLE hUserHandle, LONG nChannel, DWORD dwFileType, DWORD dwDiskNumber, SYSTEMTIME *lpBegin, SYSTEMTIME *lpEnd, LONG *lpPageCount, LONG *lpFileCount);
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_FindFileByTimeEx(HANDLE hUserHandle, LONG nChannel, DWORD dwFileType, DWORD dwDiskNumber, UINT beginTime, UINT endTime, LONG *lpPageCount, LONG *lpFileCount);
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_FindFileByCard(HANDLE hUserHandle, LONG nChannel, DWORD dwFileType, DWORD dwDiskNumber, SYSTEMTIME *lpBegin, SYSTEMTIME *lpEnd, LPCSTR lpszCardNumber, LONG *lpPageCount, LONG *lpFileCount);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindNextFile(HANDLE hFindHandle, LONG nPageIndex, BSRCLIENT_FILELIST *lpFileData, DWORD *lpdwCount);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindFileClose(HANDLE hFindHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindFileConvert(HANDLE hUserHandle, UINT64 nFileHandle, BSRCLIENT_FILETIME2LEN *lpInfos);

//���ܼ�������
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_AlgFindFileByTime(HANDLE hUserHandle, LONG nChannel, DWORD dwAlgType, DWORD dwDiskNumber, SYSTEMTIME *lpBegin, SYSTEMTIME *lpEnd, LONG *lpPageCount, LONG *lpFileCount);
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_AlgDownFileByName(HANDLE hUserHandle, WORD wDVRPort, UINT64 nFileHandle, UINT64 nOffset, UINT64 nDownSize, LPCSTR lpszSaveFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_AlgFindFileConvert(HANDLE hUserHandle, UINT64 nFileHandle, BSRCLIENT_FILETIME2LEN *lpInfos);

//�ļ�������չ�ӿ�
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_FindFileEx(HANDLE hUserHandle, BSRCLIENT_REMOTEFINDEX *lpFindCond);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindNextFileEx(HANDLE hFindHandleEx, LONG nPageIndex, BSRCLIENT_FILELIST *lpFileData, DWORD *lpdwCount);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindFileCloseEx(HANDLE hFindHandleEx);

//Զ������
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_DownFileByName(HANDLE hUserHandle, WORD wDVRPort, UINT64 nFileHandle, UINT64 nOffset, UINT64 nDownSize, LPCSTR lpszSaveFileName);
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_DownFileGetPos(HANDLE hDownHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DownFileStop(HANDLE hDownHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DownSetDataCallBack(HANDLE hDownHandle, CLIENTDATACALLBACKFUN lpfnCallBackFun, LPVOID lpVoid);

//Զ�̻ط�
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ChangeCurTransRate(HANDLE hUserHandle, LONG nBitrate);
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_PlayBackByName(HANDLE hUserHandle, WORD wDVRPort, WORD RealPort, UINT64 nFileHandle, UINT64 nOffset, UINT64 nFileSize, HWND hWnd, DWORD dwDeviceNum, BOOL bFrameMode = FALSE, BOOL bShowTime = TRUE );
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackSetPlayList(HANDLE hPlayHandle, PBSCP_NETTRANS_FILEINFO pzfileInfo, DWORD nFileCount);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackPlayListSetTimePos(HANDLE hPlayHandle, DWORD time);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackStop(HANDLE hPlayHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackCaptureBMPPicture(HANDLE hPlayHandle, LPCSTR lpszFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackRefresh(HANDLE hPlayHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackGetOsdTime(HANDLE hPlayHandle, SYSTEMTIME *lpOsdTime);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackSetOSDTimePara(HANDLE hPlayHandle, BOOL bShow, POINT *lpPoint, COLORREF *lpColor);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackSetOSDTitlePara(HANDLE hPlayHandle, BOOL bShow, POINT *lpPoint, COLORREF *lpColor);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackSetDataCallBack(HANDLE hPlayHandle, CLIENTDATACALLBACKFUN lpfnCallBackFun, LPVOID lpVoid);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackWndHookFunc(HANDLE hPlayHandle, CLIENTHWNDHOOKFUN lpfnWndFunProc, LPVOID lpVoid);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackDrawFunc(HANDLE hPlayHandle, CLIENTTITLEDRAWFUN lffnDrawFunProc, LPVOID lpVoid);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackSaveData(HANDLE hPlayHandle, LPCSTR lpszFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackStopSave(HANDLE hPlayHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackControl(HANDLE hPlayHandle, DWORD dwControlCode, DWORD dwInValue, DWORD *lpOutValue);

BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RecordBMPCapture( HANDLE hUserHandle, int chnnl, int port, SYSTEMTIME time, LPCSTR pathname, HWND hWnd );

//DVSר�ýӿ�
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVS_FrontBoardUpgrade(HANDLE hUserHandle, WORD wDVSPort, LPCSTR lpszFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVS_GetBackDVRStatus(HANDLE hUserHandle, BSRCLIENT_DVS_DVRSTATUS *lpDVRStatus);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVS_SendRS485Command(HANDLE hUserHandle, LPCSTR lpszCmdChar, DWORD dwCMDSize);

BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVS_Control0816AlarmHost(HANDLE hUserHandle, UINT nHostID, LPCSTR lpszCmdChar, DWORD dwCMDSize);
//////////////////////////////////////////////////////////////////////////

//DVS��Ѷ�ӿ�
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_NetSecBureauLogon( HANDLE hUserHandle );
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_NetSecBureauLogoff( HANDLE hUserHandle );


/*
  fn:BSR_DVR_OutputControl
  brief:������ӿ���
  param[in]: hUserHandle dvr���
  param[in]: port ������� 0-3
  param[in]: bOpen TRUE = ������ӿ�; FALSE = ������ӱպ�
*/

BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_OutputControl( HANDLE hUserHandle, unsigned char port, BOOL bOpen );


//////////////////////////////////////////////////////////////////////////
#endif //__BLUESTAR_BSRCLIENT_INTERFACE_H__