#pragma once
#include <list>
#include "AVCONSys/KIMSDefine.h"

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
#define  DEFAULT_GROUP_NAME  _T("新建联系人组")		//默认联系人组，所有使用好友分组的地方，都应加入此分组

//---------------------------------------------------------------------------------------
// 人员状态定义
#define USER_STATUS_ONLINE			"online"			// 人员状态：在线
#define USER_STATUS_OFFLINE         "offline"			// 人员状态：离线
#define USER_STATUS_BUSY			"busy"				// 人员状态：忙碌
#define USER_STATUS_AWAY			"away"				// 人员状态：离开
#define USER_STATUS_HERMIT			"hermit"			// 人员状态：隐身
#define USER_STATUS_MEETING			"meeting"			// 人员状态：会议（不允许主动改为此状态）

// 好友组列表
typedef std::map<std::string,std::string> IMS_FRIEND_GROUP_MAP;
typedef IMS_FRIEND_GROUP_MAP::iterator IMS_FRIEND_GROUP_MAP_IT;

//---------------------------------------------------------------------------------------
// 查找好友列表
//typedef std::map<std::string,IMS_CONTACT_ITEM> IMS_SEARCH_FRIEND_MAP;
//typedef IMS_SEARCH_FRIEND_MAP::iterator IMS_SEARCH_FRIEND_MAP_IT;

//---------------------------------------------------------------------------------------
// 群列表
typedef std::map<unsigned long, TEAM_DES> IMS_TEAM_MAP;
typedef IMS_TEAM_MAP::iterator IMS_TEAM_MAP_IT;

//---------------------------------------------------------------------------------------
//讨论组定义
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
// URL列表
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
//申请进会错误码
#define APPLYMEET_ERR_CALLMEET				"InCallMeet"		// 在呼叫会议中
#define APPLYMEET_ERR_PERMISSISIONDENIED	"PermissionDenied"	// 没有会议权限
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
// 客服业务信息列表
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