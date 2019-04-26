#pragma once

//=======================================================================================
// ���Ͷ���
//=======================================================================================
// �û�������Ϣ�ṹ
typedef struct _IMS_USER_ITEM_BASE
{
	std::string orgid;			// ����ID
	std::string departid;		// ����ID
	std::string userid;			// �ʻ�ID
	std::string username;		// �ʻ�����
	std::string nodeid;			// �ڵ�
	std::string status;			// ״̬
	std::string perlevel;		// Ȩ�޼���
	_IMS_USER_ITEM_BASE()
	{
		orgid		= "";
		departid	= "";
		userid		= "";
		username	= "";
		nodeid		= "";
		status		= "offline";
		perlevel	= "";
	};
	_IMS_USER_ITEM_BASE& operator=(const _IMS_USER_ITEM_BASE& ui)
	{
		orgid			= ui.orgid;
		departid		= ui.departid;
		userid			= ui.userid;
		username		= ui.username;
		nodeid			= ui.nodeid;
		status			= ui.status;
		perlevel		= ui.perlevel;

		return *this;
	}
}IMS_USER_ITEM_BASE,*PIMS_USER_ITEM_BASE;
typedef std::map<std::string,PIMS_USER_ITEM_BASE> IMS_USER_ITEM_BASE_MAP;

//-------------------------------------------
// �û�������Ϣ�ṹ
typedef struct _IMS_USER_ITEM
{
	std::string		orgid;			// ����ID
	std::string		departid;		// ����ID
	std::string		userid;			// �ʺ�ID
	std::string		username;		// �ʺ�����
	std::string		nickname;		// �˻��ǳ�
	int				userlevel;		// �û�����
	std::string		usertype;		// �û����� ��ͨ�û�normal ��� console �������� callcenter
	unsigned short	termtype;		// �ն�����
	std::string		termname;		// �ն�����
	std::string		gender;			// �Ա�
	std::string		age;			// ����
	std::string		country;		// ����
	std::string		province;		// ʡ�ݼ���
	std::string		city;			// ����
	std::string		address;		// ͨѶ��ַ
	std::string		postcode;		// �ʱ�
	std::string		email;			// ����
	std::string		smscode;		// ���˶��ź�
	std::string		voipcode;		// voip�ʺż�����
	std::string		voippassword;	// voippassword
	unsigned char   authtype;
	unsigned int	faxcode;		// ����ֻ���
	std::string		userinfo;		// �û���Ϣ��ʽ�ַ���
	std::string		show;			// public=��������, protected=����ϵ�˿ɼ�, private=������
	std::string		status;			// ״̬
	unsigned int	orderid;		// orderid
	std::string     nodeid;         // nodeid

	_IMS_USER_ITEM()
	{
		orgid			= "";
		departid		= "";
		userid			= "";
		username		= "";
		nickname		= "";
		userlevel		= 0;
		usertype		= "normal";
		termtype		= 0;
		termname		= "";
		gender			= "";
		age				= "";
		country			= "";
		province		= "";
		city			= "";
		address			= "";
		postcode		= "";
		email			= "";
		smscode			= "";
		voipcode		= "";
		voippassword	= "";
        authtype        = 0;
		faxcode			= 0;
		userinfo		= "";
		show			= "private";
		status			= "offline";
		orderid			= 0;
		nodeid          = "";
	}

	_IMS_USER_ITEM& operator=(const _IMS_USER_ITEM& ui)
	{
		orgid			= ui.orgid;
		departid		= ui.departid;
		userid			= ui.userid;
		username		= ui.username;
		nickname		= ui.nickname;
		userlevel		= ui.userlevel;
		usertype		= ui.usertype;
		termtype		= ui.termtype;
		termname		= ui.termname;
		gender			= ui.gender;
		age				= ui.age;
		country			= ui.country;
		province		= ui.province;
		city			= ui.city;
		address			= ui.address;
		postcode		= ui.postcode;
		email			= ui.email;
		smscode			= ui.smscode;
		voipcode		= ui.voipcode;
		voippassword	= ui.voippassword;
        authtype        = ui.authtype;
		faxcode			= ui.faxcode;
		userinfo		= ui.userinfo;
		show			= ui.show;
		status			= ui.status;
		orderid			= ui.orderid;
		nodeid          = ui.nodeid;

		return *this;
	}

}IMS_USER_ITEM, *PIMS_USER_ITEM;
typedef std::map<std::string, PIMS_USER_ITEM> IMS_USER_ITEM_MAP;


//-------------------------------------------
//�û�������Ϣ�ṹ
typedef struct _USER_SESSION
{
	std::string		userid;
	unsigned short	termtype;
	std::string		termname;
	std::string		svrid;
	std::string		svraddr;
	unsigned short  svrport;
	std::string		svrtype;
	std::string		nodeid;
	std::string		nataddr;
	std::string		nattype;
	std::string		localaddr;
	unsigned short	localport;
	std::string		macaddr;
	std::string		status;

	_USER_SESSION()
	{
		userid		= "";
		termtype	= 0;
		termname	= "";
		svrid		= "";
		svraddr		= "";
		svrport		= 0;
		svrtype		= "";
		nodeid		= "";
		nataddr		= "";
		nattype		= "";
		localaddr	= "";
		localport	= 0;
		macaddr		= "";
		status		= "offline";
	}

	_USER_SESSION& operator=(const _USER_SESSION& us)
	{
		userid		= us.userid;
		termtype	= us.termtype;
		termname	= us.termname;
		svrid		= us.svrid;
		svraddr		= us.svraddr;
		svrport		= us.svrport;
		svrtype		= us.svrtype;
		nodeid		= us.nodeid;
		nataddr		= us.nataddr;
		nattype		= us.nattype;
		localaddr	= us.localaddr;
		localport	= us.localport;
		macaddr		= us.macaddr;
		status		= us.status;

		return *this;
	}

}USER_SESSION, *PUSER_SESSION;
typedef std::list<PUSER_SESSION> USER_SESSION_LIST;

//-------------------------------------------
// ��ϵ����Ϣ�ṹ(������ϵ�˵ĵ�ַ����)
typedef struct _IMS_CONTACT_ITEM
{

	std::string group;			// ��ϵ��������
	std::string initiator;		// �����ϵ�˵ķ�����
	std::string acceptflag;		// ���ձ�־: true=������ϵ�� false=��δ��Ϊ��ϵ��

	std::string departname;		//��������
	std::string orgname;		//��������
	std::string strUserTypeID;  // �û���¼����('normal','ocx','TV','channel','remote','record','console')

	IMS_USER_ITEM		userItem;
	USER_SESSION		userSession;

	_IMS_CONTACT_ITEM()
	{
		group		  = "";
		initiator	  = "";
		acceptflag	  = "false";

		departname    = "";
		orgname		  = "";
		strUserTypeID = "";
	}

	_IMS_CONTACT_ITEM& operator=(const _IMS_CONTACT_ITEM& ci)
	{
		group			= ci.group;
		initiator		= ci.initiator;
		acceptflag		= ci.acceptflag;
		departname  	= ci.departname;
		orgname			= ci.orgname;
		strUserTypeID	= ci.strUserTypeID;
		userItem		= ci.userItem;
		userSession 	= ci.userSession;

		return *this;
	}
}IMS_CONTACT_ITEM,*PIMS_CONTACT_ITEM;
typedef std::map<std::string,PIMS_CONTACT_ITEM> IMS_CONTACT_ITEM_MAP;

typedef enum MSG_TYPE
{
    MSG_PERSON = 0,
    MSG_ORG,
    MSG_TEAM,
    MSG_DISCUSS,
};

//-------------------------------------------
// ��ʱ��Ϣ��Ϣ�ṹ(��ʱ��Ϣ��ʽ)
typedef struct _XMSG
{
	std::string type;		// ����,��ʹ��
	std::string head;
	std::string body;
	std::string format;
	std::string datetime;
	std::string attachment;
}XMSG,*PXMSG;

//-------------------------------------------
// ������Ϣ��Ϣ�ṹ(������Ϣ��ʽ)
typedef struct _XOFLMSG : public _XMSG
{
    unsigned int msgType;
    std::string nodeID;
    std::string orgID;
    std::string orgName;
    std::string departID;
    std::string departName;
    std::string userName;
    std::string teamID;
    std::string discussID;
    std::string discussName;

    _XOFLMSG()
        :msgType(MSG_PERSON)
    {
    };
}XOFLMSG,*PXOFLMSG;

//-------------------------------------------
// �豸
typedef enum IMS_DEVICE
{
	IMS_DEVICE_CAMERA	  = 0,
	IMS_DEVICE_MICROPHONE = 1,
};

typedef enum ENUM_OPERATION
{
	ONLYREAD = 0,	//��ȡ����
	ONLYWRITE,		//������
	READANDWRITE,	//��ȡ�����ֶ���
}OPERATION;
//-------------------------------------------
//��ط���
//������Ϣ
typedef struct _MONGROUP
{
	std::string		groupid;
	std::string		groupname;
	std::string		parentid;
	unsigned int	nLevelID;
    unsigned int    nOrderID;
	_MONGROUP()
	{
		groupid="";
		groupname="";
		parentid="";
		nLevelID=0;
        nOrderID=0;
	};
}MONGROUP,*PMONGROUP;

//-------------------------------------------
// �豸��Ϣ
typedef struct _MONDEV
{
	std::string		devid;
    std::string     devname;
	std::string     devpassword;
	unsigned long	devtype;
	std::string		parentid;
	std::string 	nodeid;
	std::string 	mcuid;
	std::string 	mcuaddr;
	unsigned short	mcuport;
	std::string		nataddr;
	std::string		localaddr;
	unsigned short	localport;
	std::string     strOrderID;
	std::string		strbindmemberid;
    unsigned short  status;
	unsigned short	usTermtype;
	std::string		strTermname;
    std::string     strMGSID;

	_MONDEV()
	{
		devid="";
        devname="";
		devtype=0;
		parentid="";
		nodeid="";
		mcuid="";
		mcuaddr="";
		mcuport=0;
		nataddr="";
		localaddr="";
		localport=0;
		strOrderID="";
		strbindmemberid="";	
        status=0;
		usTermtype=0;
		strTermname="";	
	};
}MONDEV,*PMONDEV;

//-------------------------------------------
// ͨ����Ϣ
typedef struct _MONCHANNEL
{
	std::string channelid;
	std::string channelname;
	std::string devid;
	std::string devinfo;
	unsigned long videocid;
	unsigned long audiocid;
	unsigned short status;
	unsigned char controlflag;
	unsigned short videocodetype;
	_MONCHANNEL()
	{
		videocid=0;
		audiocid=0;
		status=0;
		controlflag=0;
		videocodetype=0;
	}
}MONCHANNEL,*PMONCHANNEL;

typedef struct _VQD_CHANINFO
{
	std::string		channelid;
	std::string		channelname;
	std::string		devid;
	unsigned long	videocid;
	unsigned long	audiocid;
	unsigned short	status;

	std::string     devname;
	std::string 	nodeid;
	std::string 	mcuid;
	std::string 	mcuaddr;
	unsigned short	mcuport;
	std::string		nataddr;
	std::string		localaddr;
	unsigned short	localport;
	std::string     strlevel;//�����Ŀ�ķ�ֵ���ö���
	std::string     strData1;
	std::string		strData2;
	_VQD_CHANINFO()
	{
		channelid	= "";
		videocid	= 0;
		audiocid	= 0;
		status	    = 0;	
		strlevel	= "";
		strData1	= "";
		strData2	= "";
	}
}VQD_CHANINFO,*pVQD_CHANINFO;

typedef std::list<pVQD_CHANINFO> VQD_CHANNEL_LST;

typedef struct _ALARMLOG_INFO 
{
	unsigned int	unID;
	std::string		strDevID;
	std::string		strDevName;
	std::string		strChanID;
	std::string		strChanName;
	unsigned short	usChanNo;
	unsigned int	unChanType;
	std::string		strChanTypeName;
	std::string		strAlarmType;
	std::string		strAlarmTypeName;
	unsigned int	unAlarmLevelID;
	std::string		strAlarmTime;
	std::string		strAlarmDesc;
	std::string		strOperateTime;
	unsigned int	unOperateStatus;
	std::string		strAlarmUser;
	std::string		strResultDesc;

	_ALARMLOG_INFO()
	{
		unID			= 0;
		strDevID		= "";
		strDevName		= "";
		strChanID		= "";
		strChanName		= "";
		usChanNo		= 0;
		unChanType		= 0;
		strAlarmType	= "";
		unAlarmLevelID	= 0;
		strAlarmTime	= "";
		strAlarmDesc	= "";
		strOperateTime	= "";
		unOperateStatus	= 0;
		strAlarmUser	= "";
		strResultDesc	= "";
	}
}ALARMLOG_INFO, *PALARMLOG_INFO;

typedef std::list<ALARMLOG_INFO> ALARMLOG_INFO_LST;
//-----------------------------------------------
//���ӵ�ͼ
typedef struct _MAP_BZ_ITEM
{
	std::string strGroupID;  //����ID
	std::string strTreeID; //ͨ��ID
	std::string strBzID;   //�ȵ�ID
}MAP_BZ_ITEM,*PMAP_BZ_ITEM;

typedef std::list<PMAP_BZ_ITEM> MAP_BZ_LST;

//-----------------------------------------------
// AVC������
typedef struct tagAVC_CALL_ITEM
{
	std::string nodeid;
	int			status;
	bool		iscaller;
}AVC_CALL_ITEM,*PAVC_CALL_ITEM;

//-----------------------------------------------
// AVC������MAP
typedef std::map<std::string,PAVC_CALL_ITEM> AVC_CALL_ITEM_MAP;

//-----------------------------------------------
// �װ�ͨѶ��Ϣ�ṹ
typedef struct tagWHB_CALL_ITEM
{
	std::string nodeid;
	int			status;
	bool		iscaller;
}WHB_CALL_ITEM,*PWHB_CALL_ITEM;

//-----------------------------------------------
typedef std::map<std::string,PWHB_CALL_ITEM> WHB_CALL_ITEM_MAP;

//-----------------------------------------------
// SRV��Ļ��ƵͨѶ��Ϣ�ṹ
typedef struct tagSRV_CALL_ITEM
{
	std::string nodeid;
	int			status;
	bool		iscaller;
}SRV_CALL_ITEM,*PSRV_CALL_ITEM;

//-----------------------------------------------
// SRV MAP
typedef std::map<std::string,PSRV_CALL_ITEM> SRV_CALL_ITEM_MAP;

//������ṹ
typedef struct _DIS_ITEM
{
	unsigned long ulID;  
	std::string strMember; 
	std::string strName;   
}DIS_ITEM,*PDIS_ITEM;

//////////////////////////////////////////////////////////////////////////
// ���������ص����ݽṹ

//-----------------------------------------------
// �û�����
typedef struct _IMS_USERTYPE_ITEM
{
    int				nUserTypeID;            // �û�����id
    std::string		strTypeName;            // �û���������
    int				nOrderID;               // �û�����
    std::string		strRoomType;            // �ɴ����Ļ���������
    std::string		strIMRight;		        // �û�Ȩ��
    std::string		strSeeUserType;         // �û��Ŀɼ���Ա����
}IMS_USERTYPE_ITEM, *PIMS_USERTYPE_ITEM;

typedef std::list<PIMS_USERTYPE_ITEM> IMS_USERTYPE_ITEM_LST;

//-------------------------------------
// �㲥������
typedef struct _IMS_VOD_SVR
{
    std::string			strIP;				// �㲥������ IP ��ַ
    std::string			strName;			// �㲥����������
} IMS_VOD_SVR,*PIMS_VOD_SVR;

typedef std::list<PIMS_VOD_SVR> IMS_VOD_SVR_LST;
//-------------------------------------
//Ⱥ����Ϣ�ṹ
typedef struct _TEAM_DES
{
	unsigned long ulID;                              //ȺID
	std::string strTeamName;                         //Ⱥ����
	std::string strFounder;                          //������
	size_t TeamType;                                 //Ⱥ���ͣ��ݲ�ʹ�ã�
	std::string strIntroduce;                        //Ⱥ����
	std::string strNotice;                           //Ⱥ����
	size_t AuthType;                                 //Ⱥ����1:����Ҫ��֤ 2:��Ҫ��֤ 3:��ֹ��������
	std::map<std::string,unsigned short> mapMember;  //Ⱥ��Ա�б�,key��Ա���ƣ�value��Ա�ȼ�
                                                     //1:��ͨ��Ա
	_TEAM_DES()                                      //2:����Ա
	{                                                //3:������
		TeamType = 0;
		AuthType  = 2;
		ulID = 0;
	}
	bool operator == (const _TEAM_DES& a) const
	{
		return this->strTeamName==a.strTeamName&&this->TeamType==a.TeamType&&this->strIntroduce==a.strIntroduce&&this->strNotice==a.strNotice&&this->AuthType==a.AuthType;
	}
	bool operator != (const _TEAM_DES& a) const
	{
		return this->strTeamName!=a.strTeamName||this->TeamType!=a.TeamType||this->strIntroduce!=a.strIntroduce||this->strNotice!=a.strNotice||this->AuthType!=a.AuthType;
	}
}TEAM_DES,*PTEAM_DES;
//---------------------------------------
//Ⱥ��������Ϣ�ṹ
typedef struct _TEAM_REQUEST
{
	unsigned long ulID;               //ȺID
	std::string strSender;            //������
	std::string strReceiver;          //������
	std::string strTeamName;          //Ⱥ����
	enum type                         
	{
		msgInvite = 1,                //����
		msgApply  = 2                 //����
	};
	type MsgType;                     //��Ϣ����
	std::string strDesc;              //��������
}TEAM_REQUEST,*PTEAM_REQUEST;
//Ⱥ������Ϣ�ṹ
typedef struct _TEAM_HANDLE_ACCEPT
{
	unsigned long ulID;               //ȺID
	std::string strHandleMember;      //������
	std::string strFromID;            //������Դ
	short result ;                    //������ 1.ͬ�� 2.��ͬ��
	std::string strTeamName;          //Ⱥ����
	enum type                         
	{
		msgInvite = 1,                //����
		msgApply  = 2                 //����
	};
	type MsgType;                     //��Ϣ����
}TEAM_HANDLE_ACCEPT,*PTEAM_HANDLE_ACCEPT;
//Ⱥ���������Ϣ��Ϣ���ؽṹ
typedef struct _TEAM_REQUEST_RESULT
{
	unsigned long ulID;               //ȺID
	std::string strSendTo;            //��Ϣ������
	std::string strHandleMember;      //������
	std::string strBeHandleMember;    //��������
	short result ;                    //������ 1.�ɹ� 2.ʧ��
	std::string strTeamName;          //Ⱥ����
	enum type                         
	{
		msgInvite    = 1,                //����
		msgApply     = 2,                //����
		msgExit      = 3,                //�˳�
		msgKick      = 4,                //�߳�
		msgDissolve  = 5,                //��ɢ
		msgPromotion = 6,                //����
		msgDemote    = 7                 //����
	};
	type MsgType;                     //��Ϣ����
}TEAM_REQUEST_RESULT,*PTEAM_REQUEST_RESULT;

// Ⱥ��ͼƬ,�̳�NDS(�����ļ�)
#ifndef NDS_TEAM__
#define NDS_TEAM__
#include "KNDSDefine.h"
#endif

//---------------------------------------------------------
//������Ϣ
typedef struct _DOMAININFO
{
	std::string		strDomain;
	std::string		strParentDomain;
	std::string		strOrgName;
	_DOMAININFO()
	{
		strDomain	= "";
		strParentDomain = "";
		strOrgName = "";
	}
}DOMAININFO,*PDOMAININFO;

//������Ϣ�б�
typedef std::list<PDOMAININFO> DOMAININFOLIST;

typedef struct _ORGINFO
{
	std::string	   strOrgID;
	std::string	   strOrgName;

	_ORGINFO()
	{
		strOrgID	= "";
		strOrgName  = "";
	}
}ORGINFO,*PORGINFO;

typedef std::list<PORGINFO> ORGINFOLIST;

typedef struct _DEPARTINFO
{
	std::string	   strDepartID;
	std::string	   strDepartName;

	_DEPARTINFO()
	{
		strDepartID	= "";
		strDepartName  = "";
	}
}DEPARTINFO,*PDEPARTINFO;

typedef std::list<PDEPARTINFO> DEPARTINFOLIST;

//ȡĳ����Դ�б�ʱ��
typedef struct _DOMAINUPDATETIME
{
	std::string	   strDomain;
	std::string	   strTime;

	_DOMAINUPDATETIME()
	{
		strDomain	= "";
		strTime  = "";
	}
}DOMAINUPDATETIME,*PDOMAINUPDATETIME;

typedef std::list<DOMAINUPDATETIME> DOMAINUPDATETIMELST;


/************************************************************************
* ��ػ�����ϵͳ
************************************************************************/
typedef struct _SUBSYSTEM_DOMAININFO
{
	std::string domain;				//����
	std::string domainname;         //������
	std::string dtype;				//������ parent=���� child=���� NULL=������
	std::string parent;				//������
	std::string svraddr;			//���ַ

	unsigned short status;			//��״̬ status=1���ߣ� status=0������
}SUBSYSTEM_DOMAININFO, *PSUBSYSTEM_DOMAININFO;
typedef std::map<std::string, SUBSYSTEM_DOMAININFO> MAP_SUBSYSTEM_DOMAININFO;
typedef std::map<std::string, SUBSYSTEM_DOMAININFO>::iterator MAP_SUBSYSTEM_DOMAININFO_IT;


typedef struct _SUBSYSTEM_MCUINFO
{
	std::string domain;				//MCU������
	std::string mcuid;				//MCUID
	std::string mtype;				//MCU������
	std::string version;			//MCU�汾
	std::string mcuaddr;			//MCU IP��ַ

	unsigned long activity;			//MCU������
	unsigned short status;			//MCU��״̬ status=1���ߣ� status=0������
}SUBSYSTEM_MCUINFO, *PSUBSYSTEM_MCUINFO;
typedef std::map<std::string, SUBSYSTEM_MCUINFO> MAP_SUBSYSTEM_MCUINFO;
typedef std::map<std::string, SUBSYSTEM_MCUINFO>::iterator MAP_SUBSYSTEM_MCUINFO_IT;

typedef struct _SUBSYSTEM_DEVICEINFO
{
	std::string devid;				//�豸ID
	std::string devname;			//�豸����
	std::string bindmemberid;		//���豸����Ա
	
	std::string localaddr;			//�豸��ַ
	std::string nataddr;			//�豸ӳ���ַ
	
	std::string mcuid;				//�豸��¼��MCUID
	std::string mcuaddr;			//�豸��¼��MCU��ַ
	std::string mgsid;              //����ID
	
	unsigned short devicetype;		//�豸����
	unsigned short localprot;		//�豸�˿�
	unsigned short mcuport;			//�豸��¼��MCU�˿�
	unsigned short channelnum;		//�豸ͨ����
	unsigned short status;			//�豸״̬ status=1���ߣ� status=0������
}SUBSYSTEM_DEVICEINFO, *PSUBSYSTEM_DEVICEINFO;
typedef std::map<std::string, SUBSYSTEM_DEVICEINFO> MAP_SUBSYSTEM_DEVICEINFO;
typedef std::map<std::string, SUBSYSTEM_DEVICEINFO>::iterator MAP_SUBSYSTEM_DEVICEINFO_IT;

//-------------------------------------
// �����������Ͷ���
//-------------------------------------
typedef enum _IMS_SEARCH_TYPE
{
	IMS_SEARCH_AND = 0,
	IMS_SEARCH_ID,
	IMS_SEARCH_NAME,
	IMS_SEARCH_OR,
}IMS_SEARCH_TYPE;