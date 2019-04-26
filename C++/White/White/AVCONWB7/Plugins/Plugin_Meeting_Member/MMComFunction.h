/************************************************************************/
/* ͨ�ñ�����ͨ�ú�������                                               */
/************************************************************************/
#ifndef _WB_MMDEFINE_H__
#define _WB_MMDEFINE_H__
#pragma  once
#include "stdafx.h"

//������Ա�б�ڵ�����
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

//�б�����
struct ItemData
{
	bool		bFolder;			//�Ƿ�Ϊ����ڵ�
	int			nOlineCount;		//�ýڵ������������û�
	int			nTotalCount;		//�ýڵ��������û�

	bool		bVisible;			//�����Ƿ�ɼ�,���ڲ˵���ֻ��ʾ�����û���
	string		csID;				//�ڵ�id
	string		csNodeName;			//�ڵ�����
	CControlUI* pControlUI;			//Ƥ���ؼ�ָ��
	GroupStyle	gStyle;				//�ڵ�����
	std::string strItemType;		//�����б��û����ࣺ����"org"������"depart",������Ա"orguser",�������"searchresult",�������"meetinggroup",������Ա"member",ͨ��"channel"

	//�����û�����Ա�ڵ�ר��
	int				nlevel;			//�ȼ������
	std::string		strStatus;		//�û�״̬��online,offline,hermit��
	unsigned char	ucMeetingStatus;//����ڵ���ڻ�״̬��1�ڻᣬ0���ٻ�
	unsigned short	nMemberType;	//��Ա����MMS_MEMBER_TYPE����
	std::string		strNodeID;		//��ԱNodeID

	bool			bLoad;			//��Ա�б����ڵ�ר�ã�����߻�ȡ������Ա���ݣ��жϵ�ǰ�����Ƿ��Ѿ�������Ա����

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
		nMemberType = 0;		//�����κ�����
		bLoad = false;
	}
};
typedef std::map<string, ItemData*> MAP_NODE;
typedef MAP_NODE::iterator MAP_NODE_IT;

typedef std::vector<ItemData*> VEC_NODE;
typedef VEC_NODE::iterator VEC_NODE_IT;



//�����Ϣ
typedef struct _MMS_GROUP_INFO
{
	string	strGroupID;
	string	strGroupName;
	string	strParentID;
	int		levelid;			//��Ĳ��


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

//��Ա��Ϣ,MP��Ϣ
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
//listItem����
const CString strListItemHotBg					= _T("..\\CommRes\\ListItemHot.png");
const CString strListItemSelBg						= _T("..\\CommRes\\ListItemSel.png");


//list������
const TCHAR* const kMemberListControlName			= _T("MeetingMember");

//�б����е�һЩ���� 
const TCHAR* const kLableLogoControlName			= _T("lblLogo");
const TCHAR* const kLogoContainerControlName		= _T("VLayoutLogo");
const TCHAR* const kLableNameControlName			= _T("lblName");
const TCHAR* const kLogoutRoomName						= _T("btnLogoutMeeting");
const TCHAR* const kHLayoutStatusControlName		= _T("HLayoutAllStatus");
const TCHAR* const kOptionOnlyShowOnlineUser		= _T("OptionOnline");


//������Ա�б���ֽڵ�����
#define ITEM_STYLE_ORG  "org"							
#define ITEM_STYLE_DEPART "depart"
#define ITEM_STYLE_ORG_USER "orguser"
#define ITEM_STYLE_MEETING_GROUP "meetinggroup"
#define ITEM_STYLE_MEETING_MEMBER "meetingmember"

//list����ĸ߶�
const int kFriendListItemNormalHeight = 50;
const int kFriendListItemSelectedHeight = 100;

//�����б���ĸ߶�
#define HEIGHT_GROUP_ITEM 50
#define HEIGHT_USER_ITEM  50


/************************************************************************/
/* ȫ�ֺ�����                                                           */
/************************************************************************/
bool CompareFunOrg( CControlUI* pControl1, CControlUI* pControl2 );//������
BOOL IsUserOnline(ItemData* pUserNode);//�ж��û��Ƿ�����
BOOL IsUserOnline(std::string status);
BOOL IsUserInMeeting(ItemData* pUserNode);
bool IsPresider(ItemData* pUserNode);
bool IsPresider(unsigned short nMemType);

//��ӽڵ�,ʹ��֮ǰ��������Ƥ���ļ��ľ���·��
bool AddNode(CTreeUI* pTree, ItemData* pItem, ItemData* pParent, CPaintManagerUI& paint_manager_, int nCode = UITREE_INSERT_ROOT, LPTREESORT lpTreeSort = NULL, IDialogBuilderCallback* pCallback = NULL);

//���ýڵ�ͼƬ
void SetItemPic(ItemData* pUserData, CString strLogo, CPaintManagerUI& paint_manager_);
void SetLogoPic(ItemData* pUserData, CString strLogo, CPaintManagerUI& paint_manager_);

//���ýڵ��ı�
void SetItemText(ItemData* pUserData, CPaintManagerUI& paint_manager_);

//���÷���ڵ�����
//void UpdateParentCount(ItemData* pItemData, bool bOnline);
void UpdateParentText(ItemData* pItemData);
void UpdateGroupText(ItemData* pItemData);

//��ȡ���ڵ��б�
VEC_NODE GetListParentsNode( ItemData* pNode );

//�����ǰϵͳʱ��
void OutputSystemTime(CString strTip = _T(""));

/************************************************************************/
/* ȫ�ֱ���                                                             */
/************************************************************************/

#endif


