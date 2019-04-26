#include "stdafx.h"
#include "ListUIEx.hpp"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

CListUIEx::CListUIEx(CPaintManagerUI& paint_manager)
: m_RootNode(NULL)
, m_LevelExpandImage(_T("<i list_icon_b.png>"))
, m_LevelCollapseImage(_T("<i list_icon_a.png>"))
, m_LevelTextStartPos(20)
, m_TextPadding(10, 0, 0, 0)
, paint_manager_(paint_manager)
{
	SetItemShowHtml(true);

	m_RootNode = new CNode;
	NodeData& nodedata = m_RootNode->GetData();
	nodedata.nLevel = -1;
	nodedata.bChildVisible = true;
	nodedata.eStyle = STYLE_GROUP;
	nodedata.pListElment = NULL;
}

CListUIEx::~CListUIEx()
{
	delete m_RootNode;
	m_RootNode = NULL;
}

bool CListUIEx::Add(CControlUI* pControl)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	return CListUI::Add(pControl);
}

bool CListUIEx::AddAt(CControlUI* pControl, int iIndex)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	return CListUI::AddAt(pControl, iIndex);
}

bool CListUIEx::Remove(CControlUI* pControl)
{
	if (!pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	if (reinterpret_cast<CNode*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()) == NULL)
	{
		return CListUI::Remove(pControl);
	}
	else
	{
		return RemoveNode(reinterpret_cast<CNode*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()));
	}
}

bool CListUIEx::RemoveAt(int nIndex)
{
	CControlUI* pControl = GetItemAt(nIndex);
	if (NULL == pControl)
		return false;

	if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
		return false;

	if (reinterpret_cast<CNode*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()) == NULL)
	{
		return CListUI::RemoveAt(nIndex);
	}
	else
	{
		return RemoveNode(reinterpret_cast<CNode*>(static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement")))->GetTag()));
	}
	return true;
}

void CListUIEx::RemoveAll()
{
	CListUI::RemoveAll();

// 	if (NULL == m_RootNode)
// 		return;
// 
// 	delete m_RootNode;

// 	m_RootNode = new CNode;
// 	m_RootNode->GetData().nLevel = -1;
// 	m_RootNode->GetData().bChildVisible = true;
// 	m_RootNode->GetData().eStyle = STYLE_GROUP;
// 	m_RootNode->GetData().pListElment = NULL;
}

void CListUIEx::DoEvent(TEventUI& event) 
{
	CListUI::DoEvent(event);
}

CNode* CListUIEx::GetRoot()
{
	return m_RootNode;
}

CNode* CListUIEx::AddNode(const MeetingListItemInfo& item, CNode* parent)
{
	if (!parent)
		parent = m_RootNode;
	CListContainerElementUI* pListElement = NULL;

	if( !m_dlgBuilder.GetMarkup()->IsValid() )
	{
		pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("Meeting_List_Item.xml"), (UINT)0, NULL, &paint_manager_));
	}
	else 
	{
		pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create((UINT)0, &paint_manager_));
	}

	if (pListElement == NULL)
		return NULL;

	CNode* node = new CNode;
	NodeData& nodedata = node->GetData();
	const NodeData& parentData = parent->GetData();
	nodedata.nLevel			= parentData.nLevel + 1;
/*	node->GetData().bFolder			= item.bFolder;*/
	nodedata.bChildVisible	= false;
/*	node->GetData().strID			= item.id;*/
	nodedata.strName			= item.strName;
	nodedata.pListElment		= pListElement;
/*	node->GetData().eStyle			= item.eStyle;*/

	if (!parentData.bChildVisible)
	{
		pListElement->SetVisible(false);
	}

	if (parent != m_RootNode && !parentData.pListElment->IsVisible())
	{
		pListElement->SetVisible(false);
	}

	UICRect rcPadding = m_TextPadding;

	for (int i = 0; i < node->GetData().nLevel; ++i)
	{
		rcPadding.left += m_LevelTextStartPos;		
	}

	pListElement->SetPadding(rcPadding);

	//SetItemPic(pListElement, item.strLogo, item.strAppendLogo, item.eStyle);

// 	if (item.eStyle != STYLE_GROUP && !item.strLogo.IsEmpty())
// 	{
// 		SetItemPic(pListElement, item.strLogo, item.strAppendLogo, item.eStyle);
// 	}
// 	else 
// 	{
		CContainerUI* pLogoContainer = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pListElement, kLogoContainerControlName));
		/*if (pLogoContainer != NULL)
			pLogoContainer->SetVisible(false);*/
/*	}*/

// 	if (STYLE_CUSTOMCHANE == item.eStyle)自定义通道的删除
// 	{
// 		CContainerUI* pDelContainer = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pListElement, kHLayoutDelChannelControlName));
// 		pDelContainer->SetVisible(true);
// 
// 		CButtonUI* pBtnDel = static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, kBtnDelChannelControlName));
// 		pBtnDel->SetTag((UINT_PTR)node);
// 	}

	SetItemText(node);
	pListElement->SetFixedHeight(kFriendListItemNormalHeight);
	pListElement->SetTag((UINT_PTR)node);

	int index = 0;
	if (parent->HasChildren())
	{
		CNode* prev = parent->GetLastChild();
		index = prev->GetData().pListElment->GetIndex() + 1;
	}
	else 
	{
		if (parent == m_RootNode)
			index = 0;
		else
			index = parent->GetData().pListElment->GetIndex() + 1;
	}

	if (!CListUI::AddAt(pListElement, index))
	{
		delete pListElement;
		delete node;
		node = NULL;
	}

// 	parent->AddChild(node);
// 	m_RootNode->AddChild(item.id,node);

	return node;
}

bool CListUIEx::RemoveNode(CNode* node)
{
	if (!node || node == m_RootNode) 
		return false;

	CListUI::Remove(node->GetData().pListElment);

	node->GetParent()->RemoveChild(node);
	delete node;

	return true;
}

void CListUIEx::SetChildVisible(CNode* node, bool visible)
{
	if (!node || node == m_RootNode || node->GetData().bChildVisible == visible)
		return;

	NodeData& nodedata = node->GetData();
	nodedata.bChildVisible = visible;

	CString strHtmlText;
	CString strLeve;
	if (STYLE_GROUP == nodedata.eStyle)
	{
		if (nodedata.bChildVisible)
			strHtmlText += m_LevelExpandImage;
		else
			strHtmlText += m_LevelCollapseImage;

		strLeve.Format(_T("<x %d>"), m_LevelTextStartPos);
		strHtmlText += strLeve;
		strHtmlText += nodedata.strName;

		CLabelUI* pLabelName = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(nodedata.pListElment, kLableNameControlName));
		if (pLabelName != NULL)
		{
			pLabelName->SetText(strHtmlText);
		}
	}

	if (!nodedata.pListElment->IsVisible() || !node->HasChildren())
		return;

	for (int i = 0; i < node->NumChildren(); ++i)
	{
		if (!visible)
		{
			CNode* pChildNode = node->GetChild(i);
			if (pChildNode->HasChildren())
			{
				SetChildVisible(pChildNode, visible);
			}
		}

		CControlUI* control = (CControlUI*)node->GetChild(i)->GetData().pListElment;
		if (_tcsicmp(control->GetClass(), _T("ListContainerElementUI")) == 0)
		{
			if (visible) 
			{
				CNode* pLocalParent = ((CNode*)control->GetTag())->GetParent();
				if(NULL == pLocalParent)
					return;

				if (pLocalParent->GetData().bChildVisible && pLocalParent->GetData().pListElment->IsVisible())
				{
					control->SetVisible(true);
				}
			}
			else
			{
				control->SetVisible(false);
			}
		}
	}
}

bool CListUIEx::CanExpand(CNode* node) const
{
	if (!node || node == m_RootNode)
		return false;

	if (STYLE_GROUP == node->GetData().eStyle || STYLE_MEMBER == node->GetData().eStyle)
		return true;

	return false;
}

bool CListUIEx::SelectItem(int iIndex, bool bTakeFocus)
{
	if( iIndex == m_iCurSel ) 
		return true;

	if( m_iCurSel >= 0 ) 
	{
		CControlUI* pControl = GetItemAt(m_iCurSel);
		if( pControl != NULL)
		{
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem != NULL )
			{
 				//会议列表的子选项存储的标记并不是CNode*，将某个整型值强制转换为CNode*，该指针并不会为空，但该指针指向
 				//的内存数据被CNode解析后的值是不确定的。所以会导致会议列表中某子选项被点击后高度被改变的问题。

// 				CNode* node = (CNode*)pControl->GetTag();
// 
// 				CListContainerElementUI* pMemberListItem = static_cast<CListContainerElementUI*>(pControl);
// 				if ((pMemberListItem != NULL) && (node != NULL) && !node->IsFolder())
// 				{
// 					pMemberListItem->SetFixedHeight(kFriendListItemNormalHeight);
// 					CContainerUI* pOperatorPannel = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pMemberListItem, kOperatorPannelControlName));
// 					if (pOperatorPannel != NULL)
// 					{
// 						pOperatorPannel->SetVisible(false);
// 					}
// 				}
				pListItem->Select(false);
			}
		}
		m_iCurSel = -1;
	}

	if( iIndex < 0 )
		return false;

	if (!__super::SelectItem(iIndex, bTakeFocus))
		return false;

	CControlUI* pControl = GetItemAt(m_iCurSel);
	if( pControl != NULL)
	{
		CNode* node = (CNode*)pControl->GetTag();

		CListContainerElementUI* pMemberListItem = static_cast<CListContainerElementUI*>(pControl);
		if ((pMemberListItem != NULL) && (node != NULL) && !node->IsFolder())
		{
			if (STYLE_CARD == node->GetData().eStyle || STYLE_CUSTOMCHANE == node->GetData().eStyle )
			{
				CContainerUI* pOperatorPannel = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pMemberListItem, kOperatorPannelControlName));
				if (pOperatorPannel != NULL)
				{
					pOperatorPannel->SetVisible(true);
					pMemberListItem->SetFixedHeight(kFriendListItemSelectedHeight);
				}
			}
		}
	}
	return true;
}

void CListUIEx::SetItemPic(CListContainerElementUI* pListElement, CString strLogo, CString strAppendLogo, GroupStyle eStyle)
{	
	if(NULL == pListElement)
		return;

	CLabelUI* pLblLogo = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kLableLogoControlName));
	if (pLblLogo)
	{
		//pLblLogo->
		if (STYLE_CARD == eStyle || STYLE_ADDCUSTOMCHANE == eStyle ||  STYLE_CUSTOMCHANE == eStyle)
		{
			pLblLogo->SetFixedHeight(72);
			pLblLogo->SetFixedWidth(53);
			UICRect rcPadding(17, 17, 0, 0); 
			pLblLogo->SetPadding(rcPadding);
		}

		CString strImage;
		strImage.Format(_T("file='%s'"), strLogo);
		pLblLogo->SetBkImage(strImage);	
	}

	CLabelUI* pLblAppendLogo = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kLableLogoAppendControlName));
	if (pLblAppendLogo)
	{

		CString strAppendImage;
		strAppendImage.Format(_T("file='%s'"), strAppendLogo);
		pLblAppendLogo->SetBkImage(strAppendImage);
	}
}

void CListUIEx::SetStatusItemPic(CListContainerElementUI* pListElement, CString strSubCtrlName, CString strLogo)
{	
	if(NULL == pListElement)
		return;

	CLabelUI* pLabel = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, strSubCtrlName));
	if (pLabel != NULL)
	{
		pLabel->SetBkImage(strLogo);
	}
}

void CListUIEx::SetOperatorBtnPic(CListContainerElementUI* pListElement, CString strSubCtrlName, CString strImage)
{	
	if(NULL == pListElement)
		return;

	CButtonUI* pBtn= static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, strSubCtrlName));
	if (pBtn != NULL)
	{
		CString strNormalImage;
		strNormalImage.Format(_T("file='%s' source='0,0,52,52'"), strImage);
		pBtn->SetNormalImage(strNormalImage);

		CString strHotImage;
		strHotImage.Format(_T("file='%s' source='52,0,104,52'"), strImage);
		pBtn->SetHotImage(strHotImage);

		CString strPushedImage;
		strPushedImage.Format(_T("file='%s' source='104,0,156,52'"), strImage);
		pBtn->SetPushedImage(strPushedImage);

		CString strDisableImage;
		strDisableImage.Format(_T("file='%s' source='156,0,209,52'"), strImage);
		pBtn->SetPushedImage(strDisableImage);
	}
}

void CListUIEx::SetAppendPic( CListContainerElementUI* pListElement, CString strAppendLogo )
{
	CLabelUI* pLblAppendLogo = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kLableLogoAppendControlName));
	if (pLblAppendLogo)
	{
		CString strAppendImage;
		strAppendImage.Format(_T("file='%s'"), strAppendLogo);
		pLblAppendLogo->SetBkImage(strAppendImage);
	}
}

void CListUIEx::SetItemText(CNode* pNode)
{	
	if(NULL == pNode)
		return;

	CString strHtmlText;
	CString strName;
	NodeData& nodedata = pNode->GetData();
	if (STYLE_CARD != nodedata.eStyle)
	{
		if (STYLE_GROUP == nodedata.eStyle)
		{
			if (nodedata.bChildVisible)
			{
				strHtmlText += m_LevelExpandImage;
			}
			else
			{
				strHtmlText += m_LevelCollapseImage;
			}		

			strName.Format( _T("<x %d>"), m_LevelTextStartPos);
			strHtmlText += strName;
		}

		if (nodedata.bFolder)
		{
			strHtmlText += nodedata.strName;
		}
		else
		{
			strName.Format( _T("%s"), nodedata.strName);
			strHtmlText += strName;
		}
	}
	else
	{
		strName.Format( _T("%s"), nodedata.strName);
		strHtmlText += strName;
	}
}

void CListUIEx::SetItemText( CListContainerElementUI* pListElement, CString strRoomName )
{
	CLabelUI* pLabelName = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kLableNameControlName));
	if (pLabelName != NULL)
	{
		pLabelName->SetText(strRoomName);
	}
}
void CListUIEx::SortItems(LPCMPFUN lpCmpFun, CNode* pParant)
{
	if (NULL == pParant)
		return;

	int nSize = pParant->NumChildren();
	int nLoop = 0;
	for (nLoop = 0; nLoop < nSize; ++nLoop)
	{
		CNode* pCmdItem = pParant->GetChild(nLoop);
		for (int nInLoop = nLoop + 1; nInLoop < nSize; ++nInLoop)
		{
			CNode* pItem = pParant->GetChild(nInLoop);		

			int nResult = lpCmpFun(pCmdItem, pItem);
			if(nResult == 0)
				continue;
			else if(nResult == 1)
			{
				int nIndex = pCmdItem->GetData().pListElment->GetIndex();
				SetItemIndex(pItem->GetData().pListElment, nIndex);
				for(int i = 0; i < pItem->NumChildren(); i++)
				{
					int nChildIndex = pItem->GetChild(i)->GetData().pListElment->GetIndex();
					SetItemIndex(pItem->GetChild(i)->GetData().pListElment, nIndex + i + 1);
				}
				nIndex = pParant->GetChildIndex(pCmdItem);
				pParant->SetNodeIndex(pItem, nIndex);
			}
			break;
		}
	}
	for (nLoop = 0; nLoop < nSize; ++nLoop)
	{
		CNode* pItem = pParant->GetChild(nLoop);
		if (pItem->HasChildren())
		{
			SortItems(lpCmpFun, pItem);
		}
	}
}

void CListUIEx::InsertItem( CNode* pLastNode, CNode* pCurtNode)
{
	int nIndex = pCurtNode->GetData().pListElment->GetIndex();
	SetItemIndex(pLastNode->GetData().pListElment, nIndex);

	CNode* pParentNode = pLastNode->GetParent();
	if (pParentNode)
	{
		int nLastIndex = pParentNode->GetChildIndex(pLastNode);
		pParentNode->SetNodeIndex(pLastNode, nLastIndex+1);
	}
}
void CListUIEx::Init(CString pStrHotImage, CString pStrSelImage)
{
	SetHotItemImage(pStrHotImage);
	SetSelectedItemImage(pStrSelImage);
}

void CListUIEx::EnableBtn( CListContainerElementUI* pListElement, CString strSubCtrlName, bool bEnble )
{
	CButtonUI* pBtn = static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, strSubCtrlName));
	if(NULL == pBtn)
		return;

	pBtn->SetEnabled(bEnble);
}
