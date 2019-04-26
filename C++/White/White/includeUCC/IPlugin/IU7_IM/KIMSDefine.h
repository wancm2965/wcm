#pragma once

//=======================================================================================
// ���Ͷ���
//=======================================================================================

//-------------------------------------------
// ��ϵ����Ϣ�ṹ(������ϵ�˵ĵ�ַ����)
typedef struct _IMS_CONTACT_ITEM
{
	std::string userid;			// �ʻ�ID
	std::string username;		// �ʻ�����
	int         userlevel;		// �û�����

	std::string gender;			// �Ա�
	std::string age;			// ����
	std::string country;		// ����
	std::string province;		// ʡ�ݼ���
	std::string city;			// ����
	std::string address;		// ͨѶ��ַ
	std::string postcode;		// �ʱ�
	std::string show;			// public=�������� protected=����ϵ�˿ɼ� private=������

	std::string group;			// ��ϵ��������
	std::string initiator;		// �����ϵ�˵ķ�����
	std::string acceptflag;		// ���ձ�־: true=������ϵ�� false=��δ��Ϊ��ϵ��

	std::string departname;		//��������
	std::string orgid;			//��������
	std::string orgname;		//��������

	std::string phone;			// �绰
	std::string email;			// ����
	std::string smscode;		// ���˶����ʺ�
	std::string voipcode;		// voip�ʺż�����
	std::string voippasswd;		// B��voip����
	unsigned char authtype;		// ��֤����
	unsigned int faxcode;		// ����ֻ���
	std::string userinfo;		// �û���Ϣ��ʽ�ַ���

	std::string svrid;			// ������ID
	std::string svraddr;		// ��������ַ
	unsigned short svrport;		// �������˿�
	std::string svrtype;		// ����������

	std::string nodeid;			// ��ϵ��NODEID
	std::string nataddr;		// NAT��ַ
	std::string localaddr;		// ���ص�ַ
	unsigned short localport;	// ���ض˿�
	unsigned short termtype;	// �ն�����
	std::string termname;		// �ն�����
	std::string status;			// online offline busy away

	// add 2010/11/30
	int 	    nUserTypeID;    // �û�����ID

	// add 2012/01/05
	unsigned int nOrderID;

	_IMS_CONTACT_ITEM()
	{
		userid		= "";
		username	= "";
		userlevel	= 0;

		gender		= "";
		age			= "";
		country		= "";
		province	= "";
		city		= "";
		address		= "";
		postcode	= "";
		show		= "private";

		group		= "";
		initiator	= "";
		acceptflag	= "false";

		departname  = "";
		orgid		= "";
		orgname		= "";

		phone		= "";
		email		= "";
		smscode		= "";
		voipcode	= "";
		voippasswd	= "";
		authtype	= 0;
		faxcode		= 0;
		userinfo	= "";

		svrid		= "";
		svraddr		= "";
		svrport		= 0;
		svrtype		= "";

		nodeid		= "";
		nataddr		= "";
		localaddr	= "";
		localport	= 0;
		termtype	= 0;
		termname	= "";
		status		= "offline";

		nUserTypeID = 0;
		nOrderID     = 0;
	};
}IMS_CONTACT_ITEM,*PIMS_CONTACT_ITEM;

typedef std::map<std::string,PIMS_CONTACT_ITEM> IMS_CONTACT_ITEM_MAP;

//-------------------------------------------
// �����û���Ϣ�ṹ
typedef struct _IMS_ORGUSER_ITEM
{
	std::string userid;			// �ʻ�ID
	std::string username;		// �ʻ�����
	std::string orgid;			// ����ID
	std::string departid;		// ����ID
	std::string nodeid;			// ��ϵ��NODEID
	std::string status;			// ״̬: 0=���� 1=����

	_IMS_ORGUSER_ITEM()
	{
		userid		= "";
		username	= "";
		orgid		= "";
		departid	= "";
		nodeid		= "";
		status		= "offline";
	};
}IMS_ORGUSER_ITEM,*PIMS_ORGUSER_ITEM;

typedef std::map<std::string,PIMS_ORGUSER_ITEM> IMS_ORGUSER_ITEM_MAP;

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
// �豸
typedef enum IMS_DEVICE
{
	IMS_DEVICE_CAMERA	  = 0,
	IMS_DEVICE_MICROPHONE = 1,
};

//-------------------------------------------
//��ط���
//�����豸����Ϣ
typedef struct _MONGROUP
{
	unsigned long	groupid;
	std::string		groupname;
	std::string		devid;
	unsigned long	devtype;
	unsigned long	parentid;
	_MONGROUP()
	{
		groupid=0;
		parentid=0;
		devtype=0;
	};
}MONGROUP,*PMONGROUP;

//-------------------------------------------
//ͨ����Ϣ
typedef struct _MONCHANNEL
{
	std::string channelid;
	std::string channelname;
	std::string devid;
	std::string devinfo;
	std::string strbindmemberid;
	std::string nodeid;
	unsigned long groupid;
	std::string mcuid;
	std::string mcuaddr;
	unsigned short mcuport;
	std::string nataddr;
	std::string localaddr;
	unsigned short localport;
	unsigned short termtype;
	std::string termname;
	unsigned long videocid;
	unsigned long audiocid;
	unsigned short status;
	unsigned char controlflag;
	_MONCHANNEL()
	{
		groupid=0;
		mcuport=0;
		localport=0;
		termtype=0;
		videocid=0;
		audiocid=0;
		status=0;
		controlflag=0;
	}
}MONCHANNEL,*PMONCHANNEL;

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

//---------------------------------------------------------
// �û�SESSION����
typedef struct _USERSESSION
{
	std::string userid;
	std::string username;
	std::string svrid;
	std::string svraddr;
	unsigned short svrport;
	std::string svrtype;
	std::string nodeid;
	std::string nataddr;
	std::string localaddr;
	unsigned short localport;
	unsigned short termtype;
	std::string termname;
	std::string status;
	std::string macaddr;
	std::string phone;
	std::string email;
	std::string smscode;
	std::string voipcode;
	unsigned int faxcode;
	std::string userinfo;
	_USERSESSION()
	{
		userid		= "";
		username	= "";
		svrid		= "";
		svraddr		= "";
		svrport		= 0;
		svrtype		= "";
		nodeid		= "";
		nataddr		= "";
		localaddr	= "";
		localport	= 0;
		termtype	= 0;
		termname	= "";
		status		= "offline";
		macaddr		= "";
		phone		= "";
		email		= "";
		smscode		= "";
		voipcode	= "";
		faxcode		= 0;
		userinfo	= "";
	};
}USERSESSION,*PUSERSESSION;

// �û�SESSION�б�
typedef std::list<PUSERSESSION> USERSESSIONLIST;

// Ⱥ��ͼƬ,�̳�NDS(�����ļ�)
#ifdef NDS_TEAM__
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