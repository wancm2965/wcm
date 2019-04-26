#pragma once
#include "KIMSDefine.h"


#define  DEFAULT_GROUP_NAME  _T("�½���ϵ����")
#define  FIRST_GROUP_ID      100000
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

// �������Ͷ���
typedef enum {
	HPAV_VIDEO_NETWORK_INTERNET		= 0,	// ������
	HPAV_VIDEO_NETWORK_LAN,					// ������
	HPAV_VIDEO_NETWORK_PRIVATE,				// ר��
	HPAV_VIDEO_NETWORK_SATELLITE,			// ������
	HPAV_VIDEO_NETWORK_CDMA,				// CDMA
}THPAVVideoNetwork;

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
// �������б�
typedef std::map<std::string,std::string> IMS_FRIEND_GROUP_MAP;
typedef IMS_FRIEND_GROUP_MAP::iterator IMS_FRIEND_GROUP_MAP_IT;


//---------------------------------------------------------------------------------------
// ���Һ����б�
typedef std::map<std::string,IMS_CONTACT_ITEM> IMS_SEARCH_FRIEND_MAP;
typedef IMS_SEARCH_FRIEND_MAP::iterator IMS_SEARCH_FRIEND_MAP_IT;


//---------------------------------------------------------------------------------------
// �����ļ��б�
typedef struct _NDS_FILE_ITEM
{
	unsigned long	ulFileID;
	std::string		strFileName;
	std::string		strFileDesc;
	unsigned long	ulFileSize;
	std::string		strSendID;
	std::string		strRecvID;
	std::string     strFileMD5;//liangchuan2011/9/20
	_NDS_FILE_ITEM()
	{
		ulFileID		= 0;
		strFileName		= "";
		strFileDesc		= "";
		strSendID		= "";
		strRecvID		= "";
		strFileMD5      = ""; //liangchuan2011/9/20
	};
}NDS_FILE_ITEM,*PNDS_FILE_ITEM;

typedef std::map<unsigned long,NDS_FILE_ITEM> NDS_FILE_ITEM_MAP;
typedef NDS_FILE_ITEM_MAP::iterator NDS_FILE_ITEM_MAP_IT;