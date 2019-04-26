#pragma once
#include <list>
#include "AVCONSys/KIMSDefine.h"

//=======================================================================================
// ���Ͷ���
//=======================================================================================

// ���ӷ�����״̬
enum {
	SERVER_CONNECTING		= 1,		// ��������
	SERVER_RECONNECTED,					// ��������
	SERVER_BUSY,						// ������æ
	SERVER_FAILED,						// ����ʧ��
	SERVER_DISCONNECTED,				// ���ӶϿ�
	SERVER_CONNECTED,					// ������
};

//---------------------------------------------------------------------------------------
// ��������Ϣ�ṹ
typedef struct _IMS_SERVER_ITEM
{
	std::string		strMCUID;
	std::string		strMCUType;
	unsigned long	ulConnections;
	std::string		strBandWidth;
	unsigned long	ulActivity;
	std::string		strIPAddr;
	unsigned short	usIPPort;
	std::string		strIPName;
	int				nMcuRightType;
}IMS_SERVER_ITEM,*PIMS_SERVER_ITEM;

typedef std::map<std::string,IMS_SERVER_ITEM> IMS_SERVER_ITEM_MAP;
typedef IMS_SERVER_ITEM_MAP::iterator IMS_SERVER_ITEM_MAP_IT;

//---------------------------------------------------------------------------------------
// ������Ϣ�ṹ
typedef struct _IMS_ORG_ITEM
{
	std::string strOrgID;
	std::string strOrgName;

	_IMS_ORG_ITEM()
	{
		strOrgID	= "";
		strOrgName	= "";
	};
}IMS_ORG_ITEM,*PIMS_ORG_ITEM;

typedef std::map<std::string,PIMS_ORG_ITEM> IMS_ORG_ITEM_MAP;
typedef IMS_ORG_ITEM_MAP::iterator IMS_ORG_ITEM_MAP_IT;

typedef struct _IMS_ORG_ITEM_EX :public IMS_ORG_ITEM
{
	std::string strParentKey;
	int		nLevelID;

	_IMS_ORG_ITEM_EX()
	{
		strParentKey	= "";
		nLevelID		= 1;
	};
}IMS_ORG_ITEM_EX,*PIMS_ORG_ITEM_EX;
typedef std::map<std::string,PIMS_ORG_ITEM_EX> IMS_ORG_ITEM_EX_MAP;
typedef IMS_ORG_ITEM_EX_MAP::iterator IMS_ORG_ITEM_EX_MAP_IT;


// ����������Ϣ�ṹ
typedef struct _IMS_DEPART_ITEM
{
	std::string strOrgID;
	std::string strDepartID;
	std::string strDepartName;
	int			nLevelID;
	std::string strUpgradeID;
	int			nOrderID;

	_IMS_DEPART_ITEM()
	{
		strOrgID		= "";
		strDepartID		= "";
		strDepartName	= "";
		nLevelID		= 0;
		strUpgradeID	= "";
		nOrderID		= 0;
	};
}IMS_DEPART_ITEM,*PIMS_DEPART_ITEM;

typedef std::map<std::string,PIMS_DEPART_ITEM> IMS_DEPART_ITEM_MAP;
typedef IMS_DEPART_ITEM_MAP::iterator IMS_DEPART_ITEM_MAP_IT;

// �����û���Ϣ�ṹ
typedef struct _IMS_ORGUSER_ITEM_EX
{
	std::string			strOrgID;
	std::string			strDepartID;
	IMS_CONTACT_ITEM	item;
	int					nOrderID;

	_IMS_ORGUSER_ITEM_EX()
	{
		strOrgID	= "";
		strDepartID	= "";
		nOrderID	= 0;
	};
}IMS_ORGUSER_ITEM_EX,*PIMS_ORGUSER_ITEM_EX;

typedef std::map<std::string,PIMS_ORGUSER_ITEM_EX> IMS_ORGUSER_ITEM_EX_MAP;
typedef IMS_ORGUSER_ITEM_EX_MAP::iterator IMS_ORGUSER_ITEM_EX_MAP_IT;


//---------------------------------------------------------------------------------------
#define  DEFAULT_GROUP_NAME  _T("�½���ϵ����")		//Ĭ����ϵ���飬����ʹ�ú��ѷ���ĵط�����Ӧ����˷���

//---------------------------------------------------------------------------------------
// ��Ա״̬����
#define USER_STATUS_ONLINE			"online"			// ��Ա״̬������
#define USER_STATUS_OFFLINE         "offline"			// ��Ա״̬������
#define USER_STATUS_BUSY			"busy"				// ��Ա״̬��æµ
#define USER_STATUS_AWAY			"away"				// ��Ա״̬���뿪
#define USER_STATUS_HERMIT			"hermit"			// ��Ա״̬������
#define USER_STATUS_MEETING			"meeting"			// ��Ա״̬�����飨������������Ϊ��״̬��

// �������б�
typedef std::map<std::string,std::string> IMS_FRIEND_GROUP_MAP;
typedef IMS_FRIEND_GROUP_MAP::iterator IMS_FRIEND_GROUP_MAP_IT;

//---------------------------------------------------------------------------------------
// ���Һ����б�
//typedef std::map<std::string,IMS_CONTACT_ITEM> IMS_SEARCH_FRIEND_MAP;
//typedef IMS_SEARCH_FRIEND_MAP::iterator IMS_SEARCH_FRIEND_MAP_IT;

//---------------------------------------------------------------------------------------
// Ⱥ�б�
typedef std::map<unsigned long, TEAM_DES> IMS_TEAM_MAP;
typedef IMS_TEAM_MAP::iterator IMS_TEAM_MAP_IT;

//---------------------------------------------------------------------------------------
//�����鶨��
typedef std::list<std::string> LST_MEMBER;
typedef LST_MEMBER::iterator  LST_MEMBER_IT;

typedef struct _IMS_GROUP_INFO
{
	unsigned long ulGroupID;
	std::string strName;
	LST_MEMBER lstMember;

	_IMS_GROUP_INFO()
	{
		ulGroupID	= 0;
		strName		= "";
		lstMember.clear();
	}
}IMS_GROUP_INFO, *PIMS_GROUP_INFO;

typedef std::map<unsigned long, IMS_GROUP_INFO> IMS_GROUP_MAP;
typedef IMS_GROUP_MAP::iterator IMS_GROUP_MAP_IT;

//---------------------------------------------------------------------------------------
// URL�б�
typedef struct _IMS_URL_ITEM
{
	std::string strName;
	std::string strURL;
	std::string strParams;

	_IMS_URL_ITEM()
	{
		strName		= "";
		strURL		= "";
		strParams	= "";
	};
}IMS_URL_ITEM,*PIMS_URL_ITEM;

typedef std::map<std::string,IMS_URL_ITEM> IMS_URL_ITEM_MAP;
typedef IMS_URL_ITEM_MAP::iterator IMS_URL_ITEM_MAP_IT;


//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//������������
#define APPLYMEET_ERR_CALLMEET				"InCallMeet"		// �ں��л�����
#define APPLYMEET_ERR_PERMISSISIONDENIED	"PermissionDenied"	// û�л���Ȩ��
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// �ͷ�ҵ����Ϣ�б�
typedef struct _IMS_BUSINESSINFO_ITEM
{
	int				nID;
	int				nBussiness_Type;
	int				nParent_Bussiness_ID;
	std::string		strBussiness_Name;

	_IMS_BUSINESSINFO_ITEM()
	{
		nID					= -1;
		nBussiness_Type		= -1;
		nParent_Bussiness_ID= -1;
		strBussiness_Name	= "";
	};
}IMS_BUSINESSINFO_ITEM,*PIMS_BUSINESSINFO_ITEM;

typedef std::map<int,PIMS_BUSINESSINFO_ITEM>			IMS_BUSINESSINFO_ITEM_MAP;
typedef IMS_BUSINESSINFO_ITEM_MAP::iterator				IMS_BUSINESSINFO_ITEM_MAP_IT;


//---------------------------------------------------------------------------------------