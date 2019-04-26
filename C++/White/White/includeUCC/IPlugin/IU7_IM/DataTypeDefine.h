#pragma once
#include "KIMSDefine.h"


#define  DEFAULT_GROUP_NAME  _T("新建联系人组")
#define  FIRST_GROUP_ID      100000
//=======================================================================================
// 类型定义
//=======================================================================================

// 连接服务器状态
enum {
	SERVER_CONNECTING		= 1,		// 正在连接
	SERVER_RECONNECTED,					// 重新连接
	SERVER_BUSY,						// 服务器忙
	SERVER_FAILED,						// 连接失败
	SERVER_DISCONNECTED,				// 连接断开
	SERVER_CONNECTED,					// 已连接
};

// 网络类型定义
typedef enum {
	HPAV_VIDEO_NETWORK_INTERNET		= 0,	// 互联网
	HPAV_VIDEO_NETWORK_LAN,					// 局域网
	HPAV_VIDEO_NETWORK_PRIVATE,				// 专网
	HPAV_VIDEO_NETWORK_SATELLITE,			// 卫星网
	HPAV_VIDEO_NETWORK_CDMA,				// CDMA
}THPAVVideoNetwork;

//---------------------------------------------------------------------------------------
// 服务器信息结构
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
// 机构信息结构
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


// 机构部门信息结构
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

// 机构用户信息结构
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
// 好友组列表
typedef std::map<std::string,std::string> IMS_FRIEND_GROUP_MAP;
typedef IMS_FRIEND_GROUP_MAP::iterator IMS_FRIEND_GROUP_MAP_IT;


//---------------------------------------------------------------------------------------
// 查找好友列表
typedef std::map<std::string,IMS_CONTACT_ITEM> IMS_SEARCH_FRIEND_MAP;
typedef IMS_SEARCH_FRIEND_MAP::iterator IMS_SEARCH_FRIEND_MAP_IT;


//---------------------------------------------------------------------------------------
// 离线文件列表
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