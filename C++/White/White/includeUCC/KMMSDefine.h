#pragma once

#include "KGeneralFunc.h"
//=======================================================================================
// ���Ͷ���
//=======================================================================================
//-------------------------------------
// �������Ͷ���
//-------------------------------------
typedef enum _MMS_ROOM_TYPE
{
	MRT_ADVANCE			= 1,	//�߼�����
	MRT_TEMPORARY		= 2,	//��׼����
	MRT_MONITOR			= 3,	//��ػ���
	MRT_PERSONAL		= 4,	//���˻���
	MRT_FREE			= 5,	//���ɻ���
	MRT_HALL			= 6,	//��������
	MRT_MOBILECHANNEL   = 7,    //�ƶ����Ƶ��
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
}MMS_MEMBER_TYPE;

//-------------------------------------
// �����Ա״̬����
//-------------------------------------
typedef enum _MMS_MEMBER_STATUS
{
	MMS_OFFLINE			= 0,	//����
	MMS_ONLINE			= 1,	//����
}MMS_MEMBER_STATUS;

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

    // add 2010/11/30
    int             nPhyRoomID;                 // ���������ID
    int		        nPhyRoomTypeID;             // �������������ID

    // add 2011/03/01
    bool 		    bUseMeetingRecord;			//�Ƿ����û���¼��
    int			    UseReocrdType;				//ȫ��¼������
    std::string     strRecordUser;				//��·¼����

	_MMS_ROOM()
	{
		roomtype=0;
		membersum=0;
		onlinesum=0;
        nPhyRoomID = 0;
        nPhyRoomTypeID = 0;

        bUseMeetingRecord = false;
        UseReocrdType = 0;
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
	unsigned int	orderid;				//�������
	_MMS_MEMBER()
	{
		membertype=0;
		orderid=0;
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
	_MMS_MEMBER_EX()
	{
		videocount	= 0;
		audiocount	= 0;
		svrport		= 0;
		localport	= 0;
		termtype	= 0;
		status		= 0;
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
		for_each( mapWndPro.begin(), mapWndPro.end(), SafeDelMapSecond<int, MMS_WND_PRO>);
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
		for_each( mapScreenPro.begin(), mapScreenPro.end(), SafeDelMapSecond<int, MMS_SCREEN_PRO>);
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

