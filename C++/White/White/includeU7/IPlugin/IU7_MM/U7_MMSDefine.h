#pragma once
#include "AVCONSys/KMMSDefine.h"

//------------------------------------------------------------------------
// ͨ��ID����
typedef enum {
	HPCHANNEL_INDEX_MIN			= 0,		// ��Աͨ������Сֵ
	HPIPCAM_INDEX				= 50,		// IP Camͨ��ID
	HPCOMPOUNDCHANNEL_INDEX		= 99,		// �ϳ�ͨ��ID
	HPMEDIACHANNEL_INDEX		= 100,		// ý�干��ͨ��ID
	HPXSSECHANNEL_INDEX,					// ��Ļ����ͨ��ID
	HPCHATCHANNEL_INDEX,					// ����ͨ��ID
	HPWBCHANNEL_INDEX,						// �װ�ͨ��ID
	HPDOCSHARECHANNEL_INDEX,				// �ĵ�����ͨ��ID
	HPFILESENDCHANNEL_INDEX,				// �ļ��ַ�ͨ��ID
	HPEMAPCHANNEL_INDEX,					// ���ӵ�ͼͨ��ID
	HPAVWBCHANNEL_INDEX,					// �װ�AVͨ��ID
	HPAVDOCSHARECHANNEL_INDEX,				// �ĵ�����AVͨ��ID
	HPROUNDCHANNEL_INDEX		= 200,		// ��ѯͨ��200-599
	HPROUNDCHANNEL_LOCAL_INDEX	= 600,		// ��ѭ����ͨ��600-699
	HPPREVIEW_LOCAL_INDEX		= 700,		// Ԥ��������Ƶ700-799
	HPH323CALL_INDEX			= 800,		// H323����ͨ��ID 800-899
	HPH323CALL_SECOND_INDEX		= 900,		// H323���и���ͨ��900-999
	HPIMCAMER_INDEX				= 1000,		// IMʹ�õ�ͨ��ID
	HPIMCAMER_INDEX_MAX			= 1009,		// IMʹ�õ����ͨ��ID
	HPLOCALCOMPOUND_ACTIVE		= 1010,		// ���غϳ�ͨ��ID������ǰ������ʹ��
	HPLOCALCOMPOUND_INDEX		= 1011,		// ���غϳ�ͨ����ʼID
	HPCOMPOUNDCHANNEL_INDEX_MAX	= 1049,		// ���غϳ�ͨ�����ID
	HPAVSET_INDEX,							// ����Ƶ����ʱ�ڲ���ͨ��ID
	HPCHANNEL_INDEX_MAX			= 1100, 	// ��Աͨ�������ֵ
}TCardIndex;

//------------------------------------------------------------------------
// �������б�����
typedef std::map<std::string,PMMS_GROUP_EX> MMS_GROUP_EX_MAP;		//key:strDomain_strRoomID_strGroupID
typedef MMS_GROUP_EX_MAP::iterator MMS_GROUP_EX_MAP_IT;

//------------------------------------------------------------------------
// ��Աͨ�����ݽṹ
typedef struct _MMS_CARD
{
	std::string		memberid;		//��ԱID
	int				cardindex;		//��ID
	std::string		cardname;		//������
	unsigned long	audiochnid;		//��Ƶͨ��ID
	unsigned long	videochnid;		//��Ƶͨ��ID
	std::string		mcuid;			//MCU ID
	std::string		mcuaddr;		//MCU ��ַ
	unsigned short	mcuport;		//MCU �˿�
	std::string		devinfo;		//�豸��Ϣ

	int				screenid;		//��Ļ��
	int				windowid;		//���ں�

	_MMS_CARD()
	{
		memberid	= "";
		cardindex	= 0;
		cardname	= "";
		audiochnid	= 0;
		videochnid	= 0;
		mcuid		= "";
		mcuaddr		= "";
		mcuport		= 0;
		devinfo		= "";

		screenid	= -1;
		windowid	= -1;
	};
}MMS_CARD,*PMMS_CARD;

typedef std::map<std::string,PMMS_CARD> MMS_CARD_MAP;		//key:memberid_cardindex
typedef MMS_CARD_MAP::iterator MMS_CARD_MAP_IT;

//////////////////////////////////////////////////////////////////////////
//MPS
typedef struct _ROOM_MPS
{
	std::string		strMPSUserID;
	std::string		strMCUID;
	std::string		strMCUIP;
	unsigned short	usMCUPort;
	std::string		strNodeID;
	std::string		strNATIP;
	std::string		strLocalIP;
	unsigned short	usLocalPort;
	unsigned long	ulMPSMaxCardIndex;
	std::string		strType;
	bool			bSecondFlow;

	_ROOM_MPS()
	{
		strMPSUserID		= "";
		strMCUID			= "";
		strMCUIP			= "";
		usMCUPort			= 0;
		strNodeID			= "";
		strNATIP			= "";
		strLocalIP			= "";
		usLocalPort			= 0;
		ulMPSMaxCardIndex	= 0;
		strType				= "";
		bSecondFlow			= false;
	}
}ROOM_MPS, *PROOM_MPS;
typedef std::map<std::string, PROOM_MPS> MAP_ROOM_MPS;
typedef MAP_ROOM_MPS::iterator MAP_ROOM_MPS_IT;

//������Ϣ
typedef struct _tagAVCON_MON_ALARMLOG
{
	unsigned long ulAlarmid;		//������־ID
	int			 nChanid;			//ͨ�����
	int          nChannelType;		//ͨ������Ϊö������AVCON_ALARMLOG_CHANTYPE
	char		 szChannelType[32];	//ͨ����������
	char		 szDevID[64];		//�豸ID
	char		 szDevName[64];		//�豸����
	char		 szChanID[64];		//��Ƶͨ��ID
	char		 szChanName[64];	//ͨ������
	int          nAlarmType;		//��������Ϊö������AVCON_ALARMLOG_ALARMTYPE
	char		 szAlarmType[32];	//������������
	int          nAlarmLevel;		//��������
	int          nAlarmlinkID;		//����Ԥ��ID
	char		 szAlarmTime[64];	//����ʱ��
	_tagAVCON_MON_ALARMLOG()
	{
		ulAlarmid = -1;
		nChanid = -1;
		nChannelType = -1;
		nAlarmType = 0;
		nAlarmLevel = 0;
		nAlarmlinkID = 0;
		memset(szChannelType,0,32);
		memset(szDevID,0,64);
		memset(szDevName,0,64);
		memset(szChanID,0,64);
		memset(szChanName,0,64);
		memset(szAlarmType,0,32);
		memset(szAlarmTime,0,64);
	}
}AVCON_MON_ALARMLOG,*LPAVCON_MON_ALARMLOG;
//////////////////////////////////////////////////////////////////////////