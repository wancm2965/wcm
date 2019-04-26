#pragma once

//=======================================================================================
// ���Ͷ���
//=======================================================================================
//-------------------------------------
// �������Ͷ���
//-------------------------------------
typedef enum _MMS_ROOM_TYPE
{
	MRT_ADVANCE			= 1,	//�߼�����
	MRT_TEMPORARY		= 2,	//��ʱ����
	MRT_MONITOR			= 3,	//��ػ���
	MRT_PERSONAL		= 4,	//���˻���
	MRT_FREE			= 5,	//���ɻ���
	MRT_HALL			= 6,	//��������
	MRT_MOBILECHANNEL   = 7,    //�ƶ����Ƶ��
	MRT_MULTIPLY        = 8,    //��ϻ��飬���������缶����
}MMS_ROOM_TYPE;

//-------------------------------------
// �����Ա���Ͷ���
//-------------------------------------
typedef enum _MMS_MEMBER_TYPE
{
	MMT_OWNER			= 1,	//����������(ֻ�����ʱ����������Ч)
	MMT_PRESIDER		= 2,	//������
	MMT_PRESIDERINVITEE	= 3,	//������������
	MMT_PRESIDERTEMP	= 4,	//��ʱ������
	MMT_ATTENDEE		= 5,	//�����
	MMT_INVITEE			= 6,	//�����������
	MMT_CHANNEL			= 7,	//���ͨ��
	MMT_TVWALL			= 8,	//����ǽ
	MMT_TVCTRL			= 9,	//����ǽ������
	MMT_AUDITOR			= 10,	//������
	MMT_RECORD			= 11,	//¼�������
	MMT_MP				= 12,	//MP
	MMT_TVPRESIDERTEMP	= 13,	//TV��ʱ������
	MMT_CONSOLE			= 14,	//���������
    MMT_ANONYMOUS		= 15,	//�����û�
	MMT_ENCODECLIENT	= 16,	//�ϳ���Ƶ�ն�
	MMT_3GGW			= 17,	//3G�ֻ�����
	MMT_OBSERVER        = 18,   //ҽ������ʾ�̹�Ħ�� ����AGENT����
}MMS_MEMBER_TYPE;

//-------------------------------------
//�������� ���õĶ���
//--------------------------------------
typedef enum _MMS_SYNCOBJECT
{
	SYNCOBJ_ALL				= 0,	//����������Ա
	SYNCOBJ_PRESIDER		= 1,	//������
	SYNCOBJ_ATTENDEE		= 2,	//�����
	SYNCOBJ_APPOINT			= 3,	//�ض���Ա
}MMS_SYNCOBJECT;

//--------------------------------------
//����������Ч״̬������
//--------------------------------------
typedef enum _MMS_PRM_AVLB_STTS
{
	PAS_PRIVATE           = 0,    //���������ڻ���ʱ��Ч
	PAS_MEETING           = 1,    //���û�������ʱ��Ч�����������˳��û���ʱʧЧ
	PAS_MANUAL            = 2,    //�ֶ����Ƹ�����
}MMS_PRMAVLBSTTS;
//---------------------------------------------------------
// ��ԱSESSION�ṹ
//---------------------------------------------------------
typedef struct _MEMBER_SESSION
{
	std::string strRoomID;
	std::string strGroupID;
	std::string strMemberID;
	unsigned short usMemberType;
	unsigned long ulActiveGroupID;
	std::string strNodeID;
	std::string strNatAddr;
	std::string strLocalAddr;
	unsigned short usLocalPort;
	unsigned short usTermType;
	std::string strTermName;
    std::string strOrderID;
	_MEMBER_SESSION()
	{
		strRoomID="";
		strGroupID="";
		usMemberType=0;
		ulActiveGroupID=0;
		usLocalPort = 0;
		usTermType = 0;
        strOrderID = "";
	};
}MEMBER_SESSION,*PMEMBER_SESSION;

//---------------------------------------------------------
// ��Ա��Ϣ��(�ɹ��û��޸ĵ�����)
//---------------------------------------------------------
typedef struct _MEMBER_DEFINED_INFO
{
	std::string strMemberID;
	std::string strTermName;//�ն���(TERMINAL,TR2,MOB,PC...)
	unsigned short usDisplayScreen;//�û���Ļ����
	//...
	_MEMBER_DEFINED_INFO()
	{
		strTermName="";
		usDisplayScreen=0;
	};
}MEMBER_DEFINED_INFO,*PMEMBER_DEFINED_INFO;

typedef std::list<MEMBER_DEFINED_INFO> MEMBER_DEFINED_INFO_LST,*PMEMBER_DEFINED_INFO_LST;

//-------------------------------------
// �����Ա״̬����
//-------------------------------------
typedef enum _MMS_MEMBER_STATUS
{
	MMS_OFFLINE			= 0,	//����
	MMS_ONLINE			= 1,	//����
}MMS_MEMBER_STATUS;

//-------------------------------------------
// ���ڻ���ļ���豸��Ϣ
typedef struct NOINROOMDEV
{
	std::string		strDevID;
	std::string     strDevName;
	unsigned long	strDevType;
	unsigned short  usStatus;
	std::string		strParentName;
	std::string		strBindMemberid;
	NOINROOMDEV()
	{
		strDevID="";
		strDevName="";
		strDevType=0;
		usStatus=0;
		strParentName="";
		strBindMemberid="";
	};
}NOINROOMDEV,*PNOINROOMDEV;
//-------------------------------------
// ������ṹ
typedef struct _MMS_ROOM
{
	std::string		domain;						//������������
	std::string		roomid;						//������ID
	std::string		roomname;					//����������
	unsigned short	roomtype;					//����������MMS_ROOM_TYPE����
	std::string		roomcode;					//������CODE
	std::string		roommode;					//����ģʽ
	std::string		password;					//����������
	std::string		limitcode;					//����������λ: ������ʱ����������Ч
	std::string		desc;						//����������
	std::string		adminid;					//�����ҹ���Ա�ʺ�,������ʱ��������adminidΪ�����ߣ����ڸ߼���������adminidΪ����������.
	int				membersum;					//�������е�������
	int				onlinesum;					//�������еĵ�ǰ��������
	std::string		schedule;					//none=���¼����żƻ���appointed=ָ�����ڻ���(��ʱ����)��weekly=ÿ�����ᣬmonthly=ÿ�����ᣬdialy=ÿ�ջ��飬manual=�ֹ�����
	std::string		startdate;					//���鿪ʼ����
	std::string		enddate;					//�����������
	std::string		starttime;					//���鿪ʼʱ��
	std::string		endtime;					//�������ʱ��
	std::string		dayflag;					//���ڱ�־
	double			m_dMaxCodeStream;			//������������,��λM
	double			m_dMinCodeStream;			//�������С����

    // add 2010/11/30
    int             nPhyRoomID;                 // ���������ID
    int		        nPhyRoomTypeID;             // �������������ID

    // add 2011/03/01
    bool 		    bUseMeetingRecord;			//�Ƿ����û���¼��
    int			    UseReocrdType;				//ȫ��¼������
    std::string     strRecordUser;				//��·¼����

	std::string		hjid;						//����ID
	std::string		remark;						//��ע
	bool			bBind;						//�Ƿ񱻰�,ҽ�Ʒ�����

	_MMS_ROOM()
	{
		roomtype=0;
		membersum=0;
		onlinesum=0;
        nPhyRoomID = 0;
        nPhyRoomTypeID = 0;

        bUseMeetingRecord = false;
        UseReocrdType = 0;
		m_dMaxCodeStream =0.0;
		m_dMinCodeStream=0.0;
	};
}MMS_ROOM,*PMMS_ROOM;

typedef std::list<PMMS_ROOM> MMS_ROOM_LST;

//-------------------------------------
// ������ṹ
typedef struct _MMS_GROUP
{
	std::string domain;						//������������
	std::string	roomid;						//������ID
	std::string	groupid;					//��������ID
	std::string	groupname;					//������������
	int			levelid;					//��Ĳ��
	int			orderid;					//������
	std::string	parentid;					//������ID
	std::string devgroupid;					//�豸��ID
	_MMS_GROUP()
	{
		levelid=0;
		orderid=0;
	};
}MMS_GROUP,*PMMS_GROUP;

typedef std::list<PMMS_GROUP> MMS_GROUP_LST;

//-------------------------------------
// ����ģ����ṹ
typedef struct _MMS_SCREEN
{
	int		temtype;				//��ģ������
	int		temid;					//��ģ���
	bool	temshow;				//�Ƿ���ʾ������
	int		temwinnum;				//��������
	int		temfloatx;				//��������X
	int		temfloaty;				//��������Y
	int		temfloatcx;				//��������CX
	int		temfloatcy;				//��������CY

	_MMS_SCREEN()
	{
		temtype		= -1;
		temid		= -1;
		temshow		= false;
		temwinnum	= -1;
		temfloatx	= 0;
		temfloaty	= 0;
		temfloatcx	= 0;
		temfloatcy	= 0;
	}
}MMS_SCREEN,*PMMS_SCREEN;

//-------------------------------------
// ��������ϸ��Ϣ�ṹ
typedef struct _MMS_GROUP_EX : public _MMS_GROUP
{
	bool			bautotem;					//�Ƿ�Ϊ�Զ�ģ��
	int				curscrid;					//��ǰ��Ļ��(1����/2����/3����/4����)
	
	MMS_SCREEN		screen[4];					//4����Ļ��Ϣ

	std::string		flag;						//�����Ƿ������ٿ�������� "Y": ����   "N":������
	std::string		devid;						//�豸ID(��Ϊ������,�ǿ���Ϊ�豸)

	int				membernum;					//�����ܳ�Ա��
	int				onlinenum;					//�������߳�Ա��

	bool			subnode;					//�Ƿ�ΪҶ�ӽڵ㣨20081102 Ϊ��ݹ��ж����ӱ��λadd by dqs��
	int				subonline;					//�����ӽڵ�����
	int				subtotal;					//�ӽڵ�����

	_MMS_GROUP_EX()
	{
		bautotem=false;
		curscrid=0;		

		flag="Y";
		devid="";

		membernum=0;
		onlinenum=0;

		subnode=true;
		subonline=0;
		subtotal=0;
	};
}MMS_GROUP_EX,*PMMS_GROUP_EX;

//-------------------------------------
// �����Ա�������ݽṹ
typedef struct _MMS_MEMBER
{
	std::string		domain;					//������������
	std::string		roomid;					//������ID
	std::string		groupid;				//��������ID
	std::string		memberid;				//��ԱID
	std::string		membername;				//��Ա����
	unsigned short	membertype;				//��Ա����MMS_MEMBER_TYPE����
	std::string		strOrderID;				//�������
	_MMS_MEMBER()
	{
		membertype=0;
		strOrderID="";
	};
}MMS_MEMBER,*PMMS_MEMBER;

//-------------------------------------
// �����Ա��չ���ݽṹ
typedef struct _MMS_MEMBER_EX : public _MMS_MEMBER
{
	std::string		activegroupid;			//���
	int				videocount;				//��Ƶ��
	int				audiocount;				//��Ƶ��
	std::string		operflag;				//������־
	std::string		svrid;					//������ID
	std::string		svraddr;				//��������ַ
	unsigned short	svrport;				//�������˿�
	std::string		svrtype;				//����������
	std::string		nodeid;					//NODEID
	std::string		nataddr;				//NAT��ַ
	std::string		localaddr;				//���ص�ַ
	unsigned short	localport;				//���ض˿�
	unsigned short	termtype;				//�ն�����
	std::string		termname;				//�ն�����
	unsigned char	status;					//״̬: 0=���ڻ����� 1=�ڻ�����
	std::string		statusoff;				//statusoff="":�����˻� statusoff="Abnormal"�쳣�˻�
	_MMS_MEMBER_EX()
	{
		videocount	= 0;
		audiocount	= 0;
		svrport		= 0;
		localport	= 0;
		termtype	= 0;
		status		= 0;
		statusoff	= "";
	}
}MMS_MEMBER_EX,*PMMS_MEMBER_EX;

typedef std::map<std::string,PMMS_MEMBER_EX> MMS_MEMBER_MAP;
typedef std::list<PMMS_MEMBER_EX> MMS_MEMBER_LST;

//-------------------------------------
// ����ͶƱ�ṹ
typedef struct _MMS_VOTE
{
	unsigned long	id;
	std::string		name;
	unsigned short	count;
	_MMS_VOTE()
	{
		id=0;
		count=0;
	};
}MMS_VOTE,*PMMS_VOTE;

typedef std::list<PMMS_VOTE> MMS_VOTE_LST;

//-------------------------------------
// ����ṹ
typedef struct _MMS_LOSS
{
	std::string		id;			//������
	std::string		carid;		//���ƺ�
	std::string		userid;		//����Ա�˺�
	std::string		factoryid;	//������
	std::string		starttime;	//��ʼʱ��
	double			money;		//������
	unsigned short	status;		//����״̬��0=������,1=�������,2=�Ժ���
	_MMS_LOSS()
	{
		money=0.00;
	};
}MMS_LOSS,*PMMS_LOSS;

//-------------------------------------
// ¼��ƻ��ṹ
typedef struct _MMS_RECPLAN
{
	std::string		strStartTime;
	std::string		strEndTime;
}MMS_RECPLAN,*PMMS_RECPLAN;

typedef std::list<PMMS_RECPLAN> MMS_RECPLAN_LST;

//-------------------------------------
// ¼���ʽ�ṹ
typedef struct _MMS_RECFORMAT
{
	std::string		strVODDis;		//��Ļ�㲥�ֱ���
	unsigned short  usVODFPS;		//��Ļ�㲥֡��
	unsigned short	usVODBPS;		//��Ļ�㲥����
	std::string		strArchiveDis;	//��Ļ�鵵�ֱ���
	unsigned short  usArchiveFPS;	//��Ļ�鵵֡��
	unsigned short	usArchiveBPS;	//��Ļ�鵵����
	unsigned short  usPPTFPS;		//PPT֡��
	unsigned short	usPPTBPS;		//PPT����
	std::string		strSingleDis;	//���˷ֱ���
	unsigned short  usSingleFPS;	//����֡��
	unsigned short	usSingleBPS;	//��������
	bool			bRecTogether;	//�Ƿ�һ��¼��
	_MMS_RECFORMAT()
	{
		strVODDis="680*480";
		usVODFPS=15;
		usVODBPS=148;
		strArchiveDis="680*480";
		usArchiveFPS=30;
		usArchiveBPS=512;
		usPPTFPS=15;
		usPPTBPS=148;
		strSingleDis="320*240";
		usSingleFPS=15;
		usSingleBPS=148;
		bRecTogether=false;
	};
}MMS_RECFORMAT,*PMMS_RECFORMAT;

//////////////////////////////////////////////////////////////////////////
// ���������ص����ݽṹ
// add 2010/11/30

//-------------------------------------
// ����
typedef struct _MMS_WND_PRO
{
	int			                nWndid;             // ��,��,С <==> 0,1,2
	int			                resolution;			// 
	int			                imaquality;			// ͼ������
	int			                stream;				// ����
	int			                framerate;			// ֡��
}MMS_WND_PRO,*PMMS_WND_PRO;

typedef std::map<int, PMMS_WND_PRO> MMS_WND_PRO_MAP;

//-------------------------------------
// ��Ļ
typedef struct _MMS_SCREEN_PRO
{
	int 	                    nScreenID;          // ���ţ�(0,1,2,3)
	int				            nTMID;		        // ģ���
	MMS_WND_PRO_MAP             mapWndPro;          // ���еĴ��ڱ�

	_MMS_SCREEN_PRO()
	{
		nScreenID = 0;
		nTMID = 0;
	}

	~_MMS_SCREEN_PRO()
	{
	    for(MMS_WND_PRO_MAP::iterator it=mapWndPro.begin();it!=mapWndPro.end();++it)
	    {
            if(it->second != NULL)
            {
                delete it->second;
                it->second = NULL;
            }
        }
	}
}MMS_SCREEN_PRO,*PMMS_SCREEN_PRO;

typedef std::map<int, PMMS_SCREEN_PRO> MMS_SCREEN_PRO_MAP;

//-------------------------------------
// ����������
typedef struct _MMS_ROOMTYPE
{
	int							nRoomTypeID;        // ����������id
	std::string					strRoomTypeName;	// ��������������
	std::string					strFucList;			// �����б� |1|2|3|4|5|
	int							nScreenCount;		// ��������Ļ��
	int							nPreempType;        // �Ƿ�����ռʽ����������
	int 						nExspokeswin;		// �����������������ڵĶ���

	MMS_SCREEN_PRO_MAP          mapScreenPro;       // �������е���Ļ��

	_MMS_ROOMTYPE()
	{
		nRoomTypeID = 0;
		nScreenCount = 0;
	}

	~_MMS_ROOMTYPE()
	{
	    for(MMS_SCREEN_PRO_MAP::iterator it=mapScreenPro.begin();it!=mapScreenPro.end();++it)
	    {
            if(it->second != NULL)
            {
                delete it->second;
                it->second = NULL;
            }
        }
		//for_each( mapScreenPro.begin(), mapScreenPro.end(), SafeDelMapSecond<int, MMS_SCREEN_PRO>);
	}

	bool operator == ( const struct _MMS_ROOMTYPE& lhs)
	{
		return (lhs.nRoomTypeID == this->nRoomTypeID);
	}
}MMS_ROOMTYPE,*PMMS_ROOMTYPE;

typedef std::list<PMMS_ROOMTYPE> MMS_ROOMTYPE_LST;

//-------------------------------------
// �������
typedef struct _MMS_PHY_ROOM
{
	int					        nPhyRoomID;			    // ����id
	std::string			        strRoomName; 	  		// ��������
	int					        nRoomTypeID;			// ��������ID
	std::string			        strOrderID; 	  		// �������
} MMS_PHY_ROOM,*PMMS_PHY_ROOM;

typedef std::list<PMMS_PHY_ROOM> MMS_PHY_ROOM_LST;

//-------------------------------------
// ��������
typedef struct _MMS_TALK_SET
{
	std::string			strFucList;				// conference
	int					resolution;				//  resolution
	int					stream;					//  stream
	int					framerate;				//  framerate
} MMS_TALK_SET,*PMMS_TALK_SET;

typedef enum _MMS_SEQ_TYPE
{
    MMS_FREE_TYPE = 0,        //����ģʽ�������˶����Է���
    MMS_DISCUSS_TYPE,         //����ģʽ������ǰ��λ�Ŀ���ͬʱ����
    MMS_GRAP_TYPE,            //����ģʽ��������Ϊ1�Ŀ��Է���
}MMS_SEQ_TYPE;

typedef struct _MMS_SEQ_MEMBER
{
    std::string strMemberID;    //��ԱID
    unsigned long ulStartTime;  //��ʼ����ʱ��(������ʱ��)
    unsigned int nExTime;     //����������Ϊ��������ӵ�ʱ��

    _MMS_SEQ_MEMBER()
    {
        ulStartTime = 0;
        nExTime = 0;
    }

    bool operator==(const _MMS_SEQ_MEMBER& other)
    {
		return strMemberID.compare(other.strMemberID.c_str()) == 0;
    }
}MMS_SEQ_MEMBER;

typedef std::list<MMS_SEQ_MEMBER> MMS_SEQ_MEMBER_LST,*PMMS_SEQ_MEMBER_LST;

typedef struct _MMS_MIC_SEQ
{
    unsigned short usType;        //��������
    unsigned int nDuration;  //����ά��ʱ��
    unsigned int nMaxSize;    //ͬʱ��������
    unsigned int nSpUserNum;  //��ǰ�ڽ���������
    MMS_SEQ_MEMBER_LST lstMember;    //������Ա�б�
    std::string strTag;      //��չ
    _MMS_MIC_SEQ()
    {
        usType = MMS_FREE_TYPE;
        nDuration = 1200000;
        nMaxSize = 5;
        nSpUserNum = 0;
    }
}MMS_MIC_SEQ;
     
typedef std::map<std::string,MMS_MIC_SEQ> MIC_SEQ_MAP;


typedef enum _MMS_WNDPRESET_TYPE
{
	MMS_SELF_TYPE = 0,			//����Ӧ
	MMS_PRE_TYPE,				//����Ԥ��
	MMS_PLUS_TYPE,				//������ǿ
}MMS_WNDPRESET_TYPE;

//-------------------------------------
// ����Ԥ��ṹ��
typedef struct _MMS_WNDPRESET
{
	std::string			strRoomID;					//������ID
	std::string			strGroupID;					//��������ID
	unsigned int		uTempID;					//ģ���
	unsigned int		uWndID;						//ģ�崰�ں�
	unsigned int		uWidthSD;					//����ֱ��ʿ�
	unsigned int		uHeightSD;					//����ֱ��ʸ�
	unsigned int		uWidthHD;					//����ֱ��ʿ�
	unsigned int		uHeightHD;					//����ֱ��ʿ�
	unsigned int		uStream;					//����
	MMS_WNDPRESET_TYPE	eType;						//Ӧ������
	
	_MMS_WNDPRESET()
	{
		strRoomID	= "";
		strGroupID	= "";
		uTempID		= 0;
		uWndID		= 0;
		uWidthSD	= 0;
		uHeightSD	= 0;
		uWidthHD	= 0;
		uHeightHD	= 0;
		uStream		= 0;
		eType		= MMS_SELF_TYPE;
	};
}MMS_WNDPRESET,*PMMS_WNDPRESET;

typedef enum _IMAGEQUALITY
{
	IMAGEQUALITY_MIN=0,
	IMAGEQUALITY_CLEAR,	//��������
	IMAGEQUALITY_FLUENCY ,	//��������
	IMAGEQUALITY_MAX
}IMAGEQUALITY;