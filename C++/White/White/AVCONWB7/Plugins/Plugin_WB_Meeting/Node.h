#pragma once

//list的名字
const TCHAR* const kMemberListControlName			= _T("meetings");
const TCHAR* const kSearchMemberListControlName		= _T("SearchMembers");
//人员，邀请，请出列表中的按钮和编辑框名字
//const TCHAR* const kOKButtonControlName				= _T("BtnOK");
const TCHAR* const kCloseButtonControlName			= _T("btnClose");
const TCHAR* const kCancleButtonControlName			= _T("BtnCancel");
const TCHAR* const kSearchButtonControlName			= _T("BtnSearch");
const TCHAR* const kKeyButtonControlName			= _T("BtnKye");
const TCHAR* const kCloseSearchButtonControlName	= _T("BtnCloseSearch");
const TCHAR* const kSearchEditControlName			= _T("EditSearch");


//列表项中的一些名字
const TCHAR* const kLableLogoControlName			= _T("lblLogo");
const TCHAR* const kLableLogoAppendControlName		= _T("LblAppendLogo");
const TCHAR* const kLogoContainerControlName		= _T("VLayoutLogo");
const TCHAR* const kLableNameControlName			= _T("lblName");
const TCHAR* const kOperatorPannelControlName		= _T("VLayoutOperation");

//list中项的高度
const int kFriendListItemNormalHeight = 60;
const int kFriendListItemSelectedHeight = 110;

typedef enum 
{
	STYLE_GROUP	= 0,	
	STYLE_MEMBER,
	STYLE_CARD,
	STYLE_ADDCUSTOMCHANE,
	STYLE_CUSTOMCHANE,
}GroupStyle;

struct NodeData
{
	int							nLevel;
	bool						bFolder;
	bool						bChildVisible;
	GroupStyle					eStyle;
	CStdString					strID;
	CStdString					strName;
	CListContainerElementUI*	pListElment;
	CStdString					strLogo;

	NodeData()
	{
		nLevel			= 0;
		bFolder			= false;
		bChildVisible	=false;
		strID			= _T("");
		strName			= _T("");
		pListElment		= NULL;
	}
};

struct MemberListItemInfo
{
	bool		bFolder;
	CStdString	id;
	
	CStdString	strAppendLogo;
	CStdString	strName;
	GroupStyle	eStyle;
};

struct MeetingListItemInfo
{
	CStdString	strLogo;
	CStdString strName;
	CStdString strMember;
};

class CNode;
typedef std::map<CStdString,CNode*> MEMBER_NOTEDATA_MAP;
typedef MEMBER_NOTEDATA_MAP::iterator MEMBER_NOTEDATA_MAP_IT;

class CNode
{
public:
	CNode(void);
	CNode(NodeData t, CNode* parent);

	~CNode();

	NodeData&	GetData();
	CNode*		GetParent();
	CNode*		GetChild(int i);
	int			GetChildIndex(CNode* pChild);
	CNode*		GetLastChild();
	CNode*		GetNode(CStdString MemberID);

	void		AddChild(CNode* child);
	void		AddChild(CStdString strID, CNode* child);

	void		RemoveChild(CNode* child);

	void		SetParent(CNode* parent);
	void		SetNodeIndex(CNode* child, int nIndex);
	bool		IsFolder() const;
	int			NumChildren() const;
	bool		HasChildren() const;

	MEMBER_NOTEDATA_MAP& GetMembernNoteDateMap(){ return m_mapMemberNoteData; }
private:
	MEMBER_NOTEDATA_MAP				m_mapMemberNoteData;
	typedef std::vector <CNode*>	Children;

	Children						vecChildren;
	CNode*							m_parent;
	NodeData						m_NodeData;

};


