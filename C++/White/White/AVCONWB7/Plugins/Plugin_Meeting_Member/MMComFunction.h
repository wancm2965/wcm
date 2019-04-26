/************************************************************************/
/* 通用变量、通用函数定义                                               */
/************************************************************************/
#ifndef _WB_MMDEFINE_H__
#define _WB_MMDEFINE_H__
#pragma  once
#include "stdafx.h"

//定义人员列表节点类型
typedef enum 
{
	STYLE_GROUP	= 0,	
	STYLE_MEMBER,
	STYLE_CARD,
	STYLE_ADDCUSTOMCHANE,
	STYLE_CUSTOMCHANE,
 	STYLE_H323,
 	STYLE_STREAM,
}GroupStyle;

enum SortType 
{
	STYLE_NAME	= 0,	
	STYLE_USERID,
	STYLE_PRIORITY,
};

//列表所用
struct ItemData
{
	bool		bFolder;			//是否为分组节点
	int			nOlineCount;		//该节点下所以在线用户
	int			nTotalCount;		//该节点下所以用户

	bool		bVisible;			//自身是否可见,用于菜单“只显示在线用户”
	string		csID;				//节点id
	string		csNodeName;			//节点名称
	CControlUI* pControlUI;			//皮肤控件指针
	GroupStyle	gStyle;				//节点类型
	std::string strItemType;		//机构列表用户分类：机构"org"，部门"depart",机构人员"orguser",搜索结果"searchresult",会议分组"meetinggroup",会议人员"member",通道"channel"

	//机构用户、人员节点专用
	int				nlevel;			//等级、层次
	std::string		strStatus;		//用户状态，online,offline,hermit等
	unsigned char	ucMeetingStatus;//保存节点的在会状态，1在会，0不再会
	unsigned short	nMemberType;	//成员类型MMS_MEMBER_TYPE定义
	std::string		strNodeID;		//人员NodeID

	bool			bLoad;			//人员列表分组节点专用：与会者获取会议人员数据，判断当前分组是否已经加载人员数据

	ItemData()
	{
		bFolder = false;
		nOlineCount = 0;
		nTotalCount = 0;

		bVisible = true;
		csID = "";
		csNodeName = "";
		pControlUI = NULL;
		gStyle = STYLE_GROUP;
		strItemType = "";

		nlevel = 0;
		strStatus = "";
		ucMeetingStatus = 0;
		nMemberType = 0;		//不是任何类型
		bLoad = false;
	}
};
typedef std::map<string, ItemData*> MAP_NODE;
typedef MAP_NODE::iterator MAP_NODE_IT;

typedef std::vector<ItemData*> VEC_NODE;
typedef VEC_NODE::iterator VEC_NODE_IT;



//组的信息
typedef struct _MMS_GROUP_INFO
{
	string	strGroupID;
	string	strGroupName;
	string	strParentID;
	int		levelid;			//组的层次


	int		nMemberCount;
	int		nInMeetingCount;

	_MMS_GROUP_INFO()
	{
		strGroupID		= "";
		strGroupName	= "";
		strParentID		= "";
		nMemberCount	= 0;
		nInMeetingCount	= 0;
	}
}MMS_GROUP_INFO,*PMMS_GROUP_INFO;
// key:strGroupID
typedef std::map<string,MMS_GROUP_INFO> MAP_MMS_GROUP_INFO;
typedef MAP_MMS_GROUP_INFO::iterator MAP_MMS_GROUP_INFO_IT;

//人员信息,MP信息
typedef struct _MEMEBER_INFO
{
	std::string		strMemberID;
	std::string 	strMemberName;
	unsigned short  nMemberType;

	std::string		strParentID;
	unsigned char	ucStatus;

	_MEMEBER_INFO()
	{
		strMemberID		= "";
		strMemberName	= "";
		strParentID		= "";
		nMemberType     = 0;
		ucStatus		= 0;
	}

}MEMEBER_INFO,*PMEMEBER_INFO;
// key:strMemberID
typedef std::map<string,MEMEBER_INFO> MAP_MEMEBER_INFO;
typedef MAP_MEMEBER_INFO::iterator MAP_MEMEBER_INFO_IT;


const CString strOfflineHeadPicture				= _T("outline.png");
const CString strOnlineHeadPicture				= _T("online.png");
//listItem背景
const CString strListItemHotBg					= _T("..\\CommRes\\ListItemHot.png");
const CString strListItemSelBg						= _T("..\\CommRes\\ListItemSel.png");


//list的名字
const TCHAR* const kMemberListControlName			= _T("MeetingMember");

//列表项中的一些名字 
const TCHAR* const kLableLogoControlName			= _T("lblLogo");
const TCHAR* const kLogoContainerControlName		= _T("VLayoutLogo");
const TCHAR* const kLableNameControlName			= _T("lblName");
const TCHAR* const kLogoutRoomName						= _T("btnLogoutMeeting");
const TCHAR* const kHLayoutStatusControlName		= _T("HLayoutAllStatus");
const TCHAR* const kOptionOnlyShowOnlineUser		= _T("OptionOnline");


//定义人员列表各种节点名称
#define ITEM_STYLE_ORG  "org"							
#define ITEM_STYLE_DEPART "depart"
#define ITEM_STYLE_ORG_USER "orguser"
#define ITEM_STYLE_MEETING_GROUP "meetinggroup"
#define ITEM_STYLE_MEETING_MEMBER "meetingmember"

//list中项的高度
const int kFriendListItemNormalHeight = 50;
const int kFriendListItemSelectedHeight = 100;

//定义列表项的高度
#define HEIGHT_GROUP_ITEM 50
#define HEIGHT_USER_ITEM  50


/************************************************************************/
/* 全局函数区                                                           */
/************************************************************************/
bool CompareFunOrg( CControlUI* pControl1, CControlUI* pControl2 );//排序函数
BOOL IsUserOnline(ItemData* pUserNode);//判断用户是否在线
BOOL IsUserOnline(std::string status);
BOOL IsUserInMeeting(ItemData* pUserNode);
bool IsPresider(ItemData* pUserNode);
bool IsPresider(unsigned short nMemType);

//添加节点,使用之前必须设置皮肤文件的决定路径
bool AddNode(CTreeUI* pTree, ItemData* pItem, ItemData* pParent, CPaintManagerUI& paint_manager_, int nCode = UITREE_INSERT_ROOT, LPTREESORT lpTreeSort = NULL, IDialogBuilderCallback* pCallback = NULL);

//设置节点图片
void SetItemPic(ItemData* pUserData, CString strLogo, CPaintManagerUI& paint_manager_);
void SetLogoPic(ItemData* pUserData, CString strLogo, CPaintManagerUI& paint_manager_);

//设置节点文本
void SetItemText(ItemData* pUserData, CPaintManagerUI& paint_manager_);

//设置分组节点人数
//void UpdateParentCount(ItemData* pItemData, bool bOnline);
void UpdateParentText(ItemData* pItemData);
void UpdateGroupText(ItemData* pItemData);

//获取父节点列表
VEC_NODE GetListParentsNode( ItemData* pNode );

//输出当前系统时间
void OutputSystemTime(CString strTip = _T(""));

/************************************************************************/
/* 全局变量                                                             */
/************************************************************************/

#endif


