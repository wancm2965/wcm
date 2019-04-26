#pragma once



//=======================================================================================
#define  MON_KEY_ID             "LOGID:"
#define  MON_KEY_PASSWORD       "PASSWORD:"
#define  MON_KEY_FROM           "FROM:"
#define  MON_KEY_TO             "TO:"
#define  MON_KEY_ATTR           "ATTR:"
#define  MON_KEY_OPERATE        "OPERATE:"
#define  MON_KEY_REFID          "REFID:"
#define  MON_KEY_EERCODE        "ERRCODE:"
#define  MON_KEY_USERID         "USERID:"
#define  MON_KEY_CHNAME			"CHANNELNAME:"
#define  MON_KEY_CONTROLFLAG	"CONTROLFLAG"
#define  MON_KEY_DEVTYPE        "DEVTYPE:"
#define  MON_KEY_DEVID          "DEVID"
#define  MON_KEY_DEVNAME        "DEVNAME"
#define  MON_KEY_CHSTATUS       "STATUS:"
#define  MON_KEY_ORDERID		"ORDERID:"
#define  MON_KEY_MCUID			"MCUID:"
#define  MON_KEY_MCUADDR		"MCUADDR:"
#define  MON_KEY_MCUPORT		"MCUPORT:"
#define  MON_KEY_MCUTYPE		"MCUTYPE:"
#define  MON_KEY_VIDEOCID       "VIDEOCID:"
#define  MON_KEY_AUDIOCID       "AUDIOCID:"
#define  MON_KEY_CHNUM          "CHANNELNUM:"
#define  MON_KEY_CHCNT          "CHANNELCOUNT:"
#define  MON_KEY_M_E_C_ID       "M_E_C_ID:"
#define  MON_KEY_SDPDATA        "SDPDATA:"
#define  MON_KEY_NODEID         "NODEID:"
#define  MON_KEY_NATADDR        "NATADDR:"
#define  MON_KEY_LOCALADDR      "LOCALADDR:"
#define  MON_KEY_LOCALPORT      "LOCALPORT:"
#define  MON_KEY_CALLID         "CALLID:"
#define  MON_KEY_CTRLINFO       "CTRLINFO:"
#define  MON_KEY_ALERTLEVEL     "ALERTLEVEL:"
#define  MON_KEY_TREENAME       "TREENAME:"
#define  MON_KEY_TREEID         "TREEID:"
#define  MON_KEY_TREEPID        "TREEPARENTID:"
#define  MON_KEY_TREENODETYPE   "TREENODETYPE:"
#define  MON_KEY_RSSADDR        "RSSADDR:"
#define  MON_KEY_RSSPORT        "RSSPORT:"
#define  MON_KEY_VODIP          "VODIP:"
#define  MON_KEY_TYPE           "TYPE:"
#define  MON_KEY_DATA           "DATA:"
#define  MON_KEY_RSPCODE        "RSPCODE:"
#define  MON_KEY_MASK	        "MASK:"
#define  MON_KEY_ALERT_SUBLEVEL	"ALERT_SUBLEVEL:"
#define  MON_KEY_SRVADDR        "SRVADDR:"
#define  MON_KEY_FROMNODE       "FROMNODE:"
#define  MON_KEY_TALKTYPE       "TALKTYPE"  //0:Ϊ�����Խ� 1:�Խ�����
#define  MON_KEY_TERMTYPE       "TERMTYPE"  //0:Ϊ�����Խ� 1:�Խ�����
#define  MON_KEY_TERMNAME       "TERMNAME"  //0:Ϊ�����Խ� 1:�Խ�����
#define  MON_KEY_DEVINFO        "MON_KEY_DEVINFO" 
#define  MON_KEY_CHANNELID      "MON_KEY_CHANNELID"
#define  MON_KEY_NAME           "NAME"
#define  MON_KEY_SERIALNO       "SERIALNO"
#define  MON_KEY_VERSION        "VERSION"
#define  MON_KEY_SYSSOFTVERSION "SYSSOFTVERSION"
#define  MON_KEY_CLIENTTYPE     "CLIENTTYPE"
#define  MON_KEY_NATTYPE        "NATATYPE"
#define  MON_KEY_FACTORY		"FACTORY"
#define	 MON_KEY_BINDMEMBERID	"BINDMEMBERID"
#define  MON_KEY_MGSID          "MGSID"
//�����Խ����lbj20110721
#define	 MON_KEY_USER_DATA		"USERDATA"
#define	 CMD_CH_AUDIO_START		"CH_AUDIO_START"
#define  CMD_CH_AUDIO_ON_START	"CH_AUDIO_ON_START"
#define	 CMD_CH_AUDIO_STOP		"CH_AUDIO_STOP"
#define  CMD_CH_AUDIO_DEV_STOP	"CH_AUDIO_DEV_STOP"
#define	 CMD_CH_AUDIO_CHANGE	"CH_AUDIO_CHANGE"
#define  TALK_TYPE_MEETING		"MEETINGTALK"
#define  TALK_TYPE_MON			"MONTALK"
#define MON_KEY_TALK_CUSTOM_ENABLE	"USEAUDIOCODETYPE"
#define MON_KEY_TALK_CODECTYPE		"AUDIOCODETYPE"
#define MON_KEY_TALK_DEVTYPE		"DEVTYPE"

#define AVCON_CMD_CONNECTION_CHANGED	"CONNECTION_CHANGED"
#define AVCON_KEY_CLIENTID				"CLIENT_ID"
#define AVCON_KEY_SERVERIP				"SERVER_IP"
#define AVCON_KEY_SERVERPORT			"SERVER_PORT"
#define AVCON_KEY_ACCOUNT				"ACCOUNT"
#define AVCON_KEY_PASSWORD				"PASSWORD"

//REQ20080325001
#define  MON_KEY_YT_TYPE		"YTTYPE:"
#define  MON_KEY_YT_COM			"YTCOM:"
#define  MON_KEY_YT_NO			"YTNO:"		//��̨���
#define  MON_KEY_PRESET_NO		"PRESETNO:"	//Ԥ�õ���
#define  MON_KEY_PRESET_NAME	"PRESETNAME:"	//Ԥ�õ�����
#define  MON_KEY_YT_SPEED		"YTSPEED:"
#define  MON_KEY_YT_CMD			"YTCMD:"
#define  MON_KEY_PTYPE			"PTYPE:"
#define  MON_CONTROL_TYPE		"CTRLTYPE:"


//=======================================================================================
#define  MON_AV_EID			    "EID"
#define  MON_AV_CID			    "CID"
#define  MON_AV_ENAME		    "ENAME"
#define  MON_AV_CHNAME		    "CH_NAME"
#define  MON_AV_HDDEVICE	    "MON_AV_HDDEVICE"

//=======================================================================================
// ���������Ϣ
typedef struct _XPROXY_INFO
{
    unsigned int	proxytype;		// ��������: 0=direct 1=http proxy 2=socks5 3=http tunnel
    std::string		proxyhost;		// �����������ַ
    unsigned short	proxyport;		// ����������˿�
    std::string		proxyuser;		// �����˻�
    std::string		proxypass;		// ��������
}XPROXY_INFO,*PXPROXY_INFO;

#define ERRNO_INVALID_ACCOUNT		301
#define ERRNO_INVALID_PASSWORD		302
#define ERRNO_DUPLICATE_LOGIN		304
#define ERRNO_DATABASE				305
#define ERRNO_UNKNOWN				306

//=======================================================================================
enum _MON_TREENODE_TYPE
{
	MON_TREENODE_REGION = 0,
	MON_TREENODE_DEV,
	MON_TREENODE_DEVCH,
	MON_TREENODE_DEVCH_UPDATE
};

//=======================================================================================
//����״̬
enum _ROOM_STATUS
{
	ROOM_STATUS_NONE = 0,	//δ����
	ROOM_STATUS_MET,		//����
	ROOM_STATUS_MON,		//���
};

typedef enum _MON_DEVTYPE
{
    MON_DEV_MON = 0,    // ����նˣ���ͨ�豸
    MON_DEV_MGW,        // �������
    MON_DEV_MP,         // ��������
    MON_DEV_MET,        // �����ն�
    MON_DEV_MOB,        // �ֻ��ն�
    MON_DEV_H323T,      // H323�ն�
    MON_DEV_H323M,      // H323 MCU(����)
    MON_DEV_TVS,        // ����ǽ��
    MON_DEV_OCX,        // OCX
    MON_DEV_ENCODE,     //��Ƶ�ϳ��ն�

    MON_DEV_INVALID = 999,  // �Ƿ�
}MON_DEVTYPE;

//-------------------------------------
//��������
typedef enum _MON_FACTORY
{
	//ǰ���豸
	HPD_DVR = 0, /*��ƽ*/
	HIK_DVR, /*����*/
	DAH_DVR, /*��*/
	KEDA_DVR, /*�ƴ�*/
	HHS_DVR, /*�ƺ�*/
	HBT_DVR, /*����߿�*/
	YAA_DVR, /*����ǰ�*/
	TDY_DVR, /*���ΰҵ*/
	CJV_DVR, /*ʮ����*/
	JTN_DVR, /*���ݾ���*/
	PLT_DVR, /*ƽ̨*/
	TLV_DVR, /*�Ϻ�ͨ��*/
	BSC_DVR, /*����ͨ*/
	XINQI_DVR, /*о��*/
	SKY_DVR, /*��ɫ�Ǽ�*/
	MAX_DVR,

	//�ֻ���ƽ���ն�
	HPD_MOB = 0x99,		/*��ƽ�ֻ�*/
	HDS_1000,			/*�����豸 OEM NXP*/
	HDS_2000,			/*ͨ��*/

	//ƽ̨�豸
	PLT_C7  = 0x300,	/*����ƽ̨*/
	PLT_KEDA,			/*�ƴ�ƽ̨*/
	MAX_PLT,

	//HDS�����豸
	HDS_100 = 0x500,	/*��ƽ�����ն�*/
	MAX_HDS,

	//NDS�����豸
	NDS_100 = 0x600,	/*OEM����*/
	NDS_200,			/*OEM����*/
	NDS_300,			/*OEM�ƺ�*/
	NDS_410,			/*OEMͨ��*/
	MAX_NDS,

	//MCSϵ���豸
	MCS_2000 = 0x700,	/*���������*/
	MAX_MCS,

	//RTSP
	RTSP_HHS = 0x1000, /*�ƺ�*/
	RTSP_ALK, /*������*/
	RTSP_KEDA, /*�ƴ�*/
	RTSP_SANYO, /*����*/
	MAX_RTSP,
}MON_FACTORY;

#define HP_DVR HPD_DVR

//-------------------------------------
// �����豸��Ϣ
typedef struct _BASE_MON_DEV_INFO
{
    _BASE_MON_DEV_INFO()
    {
        usMcuPort       = 0;
        usLocalPort     = 0;
        usTermType      = 0;
		usFactory		= 0;
        ulVersion       = 0;
		usControlFlag	= 0;
		usStatus		= 0;
		strOrderID		= "";
        eDevType        = MON_DEV_INVALID;
    }

    // ע��ķ�����Ϣ
    std::string		strMcuID;
    std::string		strMcuAddr;
    unsigned short	usMcuPort;
    std::string     strMcuType;
    std::string     strMGSID;

    // �豸�Ļ�����Ϣ
    std::string		strID;      // �豸��ID
    std::string		strNodeID;
    std::string		strNatAddr;
    std::string     strNatType;
    std::string		strLocalAddr;
    unsigned short	usLocalPort;
	unsigned short	usFactory;

    // �豸�ı�ʶ��
    std::string		strPassword;
    MON_DEVTYPE     eDevType;
    unsigned short	usTermType;
    std::string		strTermName;
    std::string		strName;
    std::string		strSerialno;
    unsigned long   ulVersion;
    std::string		strSysSoftVersion;
	std::string		strBindMemberID;
    std::string     strParentID;
	unsigned short	usControlFlag;
	std::string     strOrderID;
	unsigned short  usStatus;
	unsigned short  usChannelNum;
	
} BASE_MON_DEV_INFO,*PBASE_MON_DEV_INFO;

//-------------------------------------
// ������Ϣ
typedef struct _MON_GATEWAY_INFO : public BASE_MON_DEV_INFO
{
} MON_GATEWAY_INFO,*PMON_GATEWAY_INFO;

//-------------------------------------
// ����豸��Ϣ
typedef struct _MON_DEV_INFO : public BASE_MON_DEV_INFO
{
} MON_DEV_INFO, *PMON_DEV_INFO;

//-------------------------------------
// ���ͨ����Ϣ
typedef struct _MON_CHANNEL_INFO : public BASE_MON_DEV_INFO
{
    _MON_CHANNEL_INFO()
    {
        usChannelNum = 0;
        usSpType = 0;
        usStatus = 0;
        ulVideocID = 0;
		usVideoCodeType = 0;
        ulAudiocID = 0;
		nControlFlag = 0;
    }
    std::string     strChannelID;
    unsigned short  usChannelNum;
    std::string		strChannelName;
    std::string		strDevName;
    std::string     strDevID;
    unsigned short  usSpType;
    unsigned short  usStatus;
    unsigned long	ulVideocID;
	unsigned short  usVideoCodeType;
    unsigned long	ulAudiocID;
    std::string		strDevInfo;
	unsigned int	nControlFlag;
} MON_CHANNEL_INFO, *PMON_CHANNEL_INFO;

//=======================================================================================
//UDP����������Ϣ
typedef struct _UDP_RCV_ALART_INFO
{
	_UDP_RCV_ALART_INFO()
	{
		usWall = 1; //0:���ϴ���,1:�ϴ���; �������Ƶ��(strType=SPD03)ʱ������ʾ��ʽ(ȫ����ʾ��0��׷����ʾ��1)
		usMonitorType = 0; //0��ʾ�Ӿ���1��ʾ������Ĭ��Ϊ0
	}
	std::string strChannelID;
	std::string strType;
	std::string strSender;
	std::string strReceiver;
	std::string strSendTime;
	std::string	strEventID;
	std::string strOrg;
	std::string strLongitude;
	unsigned short  usWall;
	unsigned short  usMonitorType;
}UDP_RCV_ALART_INFO,*PUDP_RCV_ALART_INFO;
//=======================================================================================

typedef struct _YT_Control_Parameters
{
public:
	_YT_Control_Parameters()
	{
		nChannelNum = -1;
		strPresetNO = "";
		strPresetName= "";
		nSpeed = 50;
	}

	_YT_Control_Parameters& operator=(const _YT_Control_Parameters& rChannelItem)
	{
		nChannelNum = rChannelItem.nChannelNum;
		strPresetNO = rChannelItem.strPresetNO;
		strPresetName = rChannelItem.strPresetName;
		nSpeed = rChannelItem.nSpeed;

		return *this;
	}

public:
	unsigned short nChannelNum;	//ͨ����
	std::string strPresetNO;	//����Ԥ�õ�ı�ţ��硰1,3,5������','�ָ���
	std::string strPresetName;	//����Ԥ�õ�����ƣ��硰ǰ��,��Ժ,���š�����','�ָ������������Ӧ��
	unsigned short nSpeed;

}YT_CONTROL_PARAMETERS;

//=======================================================================================
typedef enum _MON_DVR_ATTR
{
	MON_DVR_DEVICE = 0,
	MON_DVR_MEET,
	MON_DVR_VIEWER,
}MON_DVR_ATTR;

//=======================================================================================
typedef enum _MONERR_CODE 
{
	MONERR_PACKET=0,
	MONERR_DB,
	MONERR_CH_INVALID,   // ��Чͨ��
	MONERR_OFFLINE,
	MONERR_CH_UPDATE_FAIL,
	MONERR_UNKNOW,
}MON_ERR_CODE;

//-------------------------------------
// ¼��ṹ
typedef struct _MON_RECORD
{
	std::string filename;	//�ļ���
	std::string writedate;
	std::string completedate;
	unsigned long filesize;
	std::string dvrid;
	unsigned short channelid;
	unsigned short filetype;
	std::string filestarttime;
	std::string fileendtime;
	std::string serverip0;
	std::string serverip1;
	std::string serverip2;
}MON_RECORD,*PMON_RECORD;

//=======================================================================================
typedef enum _MONERR_DEVCHECK
{
	MONERR_DEVCHECK_NONE=0,
	MONERR_DEVCHECK_OLDSERIALNO_ISEMPTY,	//�����豸���к�Ϊ��
	MONERR_DEVCHECK_SERIALNO_ISNOTSAME,		//�����豸�ʺź����豸�ʺ���ͬ,�������кŲ�ͬ
	MONERR_DEVCHECK_DEVID_ISNOTSAME,		//�����豸�ʺź����豸�ʺŲ�ͬ,�������к���ͬ
	MONERR_DEVCHECK_NEWDEVID_ISNOTEXIST,	//���豸�ʺŲ�����
	MONERR_DEVCHECK_NEWDEVID_SERIALNO,		//���豸�ʺ��Ѿ��������к�
	MONERR_DEVCHECK_NEWDEVID_LOGIN,			//���豸�ʺ��Ѿ���¼
	MONERR_DEVCHECK_NEWSERIALNO_USED,		//���豸���к��Ѿ���ʹ��
}MONERR_DEVCHECK;

typedef struct _RegionItem
{
	unsigned int nType;
	unsigned int ID;
	std::string strName;
	std::string strDevID;
	_RegionItem()
	{
		nType=0;
		ID=0;
	};
}RegionItem;

typedef struct tagMemInfo
{
	std::string	nodeid;
	std::string memberid;
} MEMINFO;