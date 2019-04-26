#include "stdafx.h"
#include "MMComFunction.h"
#include "IPlugin/IU7_IM/U7_IMSDefine.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif



bool CompareFunOrg( CControlUI* pControl1, CControlUI* pControl2 )
{
	ItemData* pItem1 = (ItemData*)pControl1->GetTag();
	ItemData* pItem2 = (ItemData*)pControl2->GetTag();

	//�Է�������
	if (pItem1->bFolder && !pItem2->bFolder)
	{
		return true;
	}
	if ( !pItem1->bFolder && pItem2->bFolder)
	{
		return false;
	}
	if(pItem1->bFolder && pItem2->bFolder)//���Ƿ���ʱ
	{
		//���ԡ���ϵ�ˡ��ڵ�����
		//if (pItem1->strNodename == _T("��ϵ��"))
		//{
		//	return true;
		//}
		//if (pItem2->strNodename == _T("��ϵ��"))
		//{
		//	return false;
		//}
		//���Ի�������
		if (pItem1->strItemType == ITEM_STYLE_ORG && pItem2->strItemType != ITEM_STYLE_ORG)
		{
			return true;
		}
		if (pItem1->strItemType != ITEM_STYLE_ORG && pItem2->strItemType == ITEM_STYLE_ORG)
		{
			return false;
		}	
	}
	//�Ƚ������������
// 	if (IsUserOnline(pItem1) && !IsUserOnline(pItem2))
// 	{
// 		return true;
// 	}
// 	if (!IsUserOnline(pItem1) && IsUserOnline(pItem2))
// 	{
// 		return false;
// 	}
	//�Ƚ������������
	if (IsUserInMeeting(pItem1) && !IsUserInMeeting(pItem2))
	{
		return true;
	}
	if (!IsUserInMeeting(pItem1) && IsUserInMeeting(pItem2))
	{
		return false;
	}
	return true;
}
BOOL IsUserOnline(ItemData* pUserNode)
{
	ASSERT(pUserNode);
	if (pUserNode->strStatus == USER_STATUS_OFFLINE || pUserNode->strStatus  == USER_STATUS_HERMIT)
	{
		return FALSE;
	}
	return TRUE;
}


BOOL IsUserOnline( std::string status )
{
	if (status == USER_STATUS_OFFLINE || status  == USER_STATUS_HERMIT)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL IsUserInMeeting(ItemData* pUserNode)
{
	ASSERT(pUserNode);
	if (pUserNode->ucMeetingStatus == 0 )
	{
		return FALSE;
	}
	return TRUE;
}

bool AddNode(CTreeUI* pTree, ItemData* pItemData, ItemData* pParentData, CPaintManagerUI& paint_manager_, int nCode /*= UITREE_INSERT_ROOT*/, LPTREESORT lpTreeSort /*= NULL*/, IDialogBuilderCallback* pCallback/* = NULL*/)
{
	if (NULL == pTree  || NULL == pItemData )
	{
		return false;
	}
	bool isOK = false;
	if (pItemData->bFolder && STYLE_GROUP == pItemData->gStyle )//����ڵ�
	{
		CTreeItemUI* pGroupItem = new CTreeItemUI();
		if (NULL == pGroupItem )
		{
			return false;
		}
		pGroupItem->SetShowGroupImage(1); 
		pGroupItem->SetText(A2U(pItemData->csNodeName));
		pGroupItem->SetTag((UINT_PTR)pItemData);
		pItemData->pControlUI = (CControlUI*)pGroupItem;
		
		//���÷���߶�
		pGroupItem->SetFixedHeight(HEIGHT_GROUP_ITEM);

		//���ڵ������
		if (UITREE_INSERT_ROOT == nCode )
		{
			isOK = pTree->Insert((CControlUI*)pGroupItem, NULL, nCode, lpTreeSort);
		}
		else
		{
			ASSERT(pParentData);
			isOK = pTree->Insert((CControlUI*)pGroupItem, pParentData->pControlUI, nCode, lpTreeSort);
		}
		if (!isOK)
		{
			delete pGroupItem;
			pGroupItem = NULL;
			return false;
		}
	}
	else//�û��ڵ���߿��ڵ�
	{
		paint_manager_.SetResourcePath(GetFolderPath());

		CTreeItemUI* pUserItem = NULL;
		static CDialogBuilder dlgBuilder;
		if( !dlgBuilder.GetMarkup()->IsValid() ) 
		{
			pUserItem = static_cast<CTreeItemUI*>(dlgBuilder.Create(_T("MemberList.xml"), (UINT)0, pCallback,&paint_manager_));
		}
		else
		{
			pUserItem = static_cast<CTreeItemUI*>(dlgBuilder.Create(pCallback, &paint_manager_));
		}
		if (NULL == pUserItem )
			return false;
		
		pUserItem->SetShowGroupImage(-1);
		pUserItem->SetTag((UINT_PTR)pItemData);
		pItemData->pControlUI = (CControlUI*)pUserItem;

		//����ͷ��
		CString strLogo  = strOfflineHeadPicture;
		if (ITEM_STYLE_MEETING_MEMBER == pItemData->strItemType )//�����б��û��ڵ㡢��������ڵ㣻����б���Ա�ڵ�
		{
			if(IsUserOnline(pItemData))
			{
				strLogo = strOnlineHeadPicture;
			}
			else
			{
				strLogo = strOfflineHeadPicture;
			}
		}
		SetItemPic(pItemData, strLogo, paint_manager_);

		//������ʾ�ı�
		SetItemText(pItemData, paint_manager_);

		//���ø߶�
		pUserItem->SetFixedHeight(HEIGHT_USER_ITEM);

		//���ڵ������
		if (UITREE_INSERT_ROOT == nCode )
		{
			isOK = pTree->Insert((CControlUI*)pUserItem, NULL, nCode, lpTreeSort);
		}
		else
		{
			ASSERT(pParentData);
			isOK = pTree->Insert((CControlUI*)pUserItem, pParentData->pControlUI, nCode, lpTreeSort);
		}
		if (isOK == false)
		{
			//delete pItemData;�Ǵ�����������
			delete pUserItem;
			pUserItem = NULL;
			return false;
		}
		if (ITEM_STYLE_MEETING_MEMBER == pItemData->strItemType )
		{
			//���¸��ڵ���������
			ItemData* pParentsNode = (ItemData*)pUserItem->GetParentItem()->GetTag();
			while(NULL != pParentsNode )
			{
				if (1 == pItemData->ucMeetingStatus)
				{
					pParentsNode->nOlineCount++;
				}
				pParentsNode->nTotalCount++;

				CTreeItemUI* pParentTreeItem = static_cast<CTreeItemUI*>(pParentsNode->pControlUI);
				if (pParentTreeItem == NULL)
				{
					return false;
				}
				pParentsNode = (ItemData*)pParentTreeItem->GetParentItem()->GetTag();
			}
		}
	}
	return isOK;
}

void SetItemPic(ItemData* pUserData, CString strLogo, CPaintManagerUI& paint_manager_)
{	
	if(NULL == pUserData || NULL == pUserData->pControlUI || strLogo.IsEmpty() )
	{
		return;
	}
	//����Ƥ��·��
	paint_manager_.SetResourcePath(GetFolderPath());

	GroupStyle gStyle = pUserData->gStyle;
	CLabelUI* pLblLogo = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pUserData->pControlUI, kLableLogoControlName));
	if (pLblLogo)
	{
		strLogo = GetFolderPath()  + strLogo;
		CString strImage;
		strImage.Format(_T("file='%s'"), strLogo);
		pLblLogo->SetBkImage(strImage);	
	}
}


void SetLogoPic( ItemData* pUserData, CString strLogo, CPaintManagerUI& paint_manager_ )
{
	if(NULL == pUserData || NULL == pUserData->pControlUI || strLogo.IsEmpty() )
	{
		return;
	}
	CLabelUI* pLblLogo = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pUserData->pControlUI, kLableLogoControlName));
	if (pLblLogo)
	{
		/*if (STYLE_CARD == gStyle || STYLE_ADDCUSTOMCHANE == gStyle ||  STYLE_CUSTOMCHANE == gStyle)
		{
			pLblLogo->SetFixedHeight(25);
			pLblLogo->SetFixedWidth(25);
			UICRect rcPadding(17, 17, 0, 0); 
			pLblLogo->SetPadding(rcPadding);
		}*/
		strLogo = GetFolderPath()  + strLogo;
		CString strImage;
		strImage.Format(_T("file='%s'"), strLogo);
		pLblLogo->SetBkImage(strImage);	
	}
}

bool OnTextEvent(void* param)
{
	TEventUI* pEvent = (TEventUI*)param;
	if (pEvent != NULL && pEvent->pSender != NULL
		&& (pEvent->Type == UIEVENT_MOUSEENTER || pEvent->Type == UIEVENT_MOUSELEAVE))
	{
		CControlUI* pItem = (CControlUI*)pEvent->pSender->GetTag();
		if (pItem != NULL)
		{
			pItem->DoEvent(*pEvent);
		}
	}
	return true;
}

void SetItemText(ItemData* pUserData, CPaintManagerUI& paint_manager_)
{	
	if(NULL == pUserData || NULL == pUserData->pControlUI)
		return;

	CLabelUI* pLabelName = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pUserData->pControlUI, kLableNameControlName));
	if (NULL != pLabelName)
	{
		pLabelName->SetVisible(true);
		pLabelName->SetShowHtml(true);
		pLabelName->SetText(A2U(pUserData->csNodeName));
		pLabelName->SetToolTip(A2U(pUserData->csNodeName));
		pLabelName->SetTag((UINT_PTR)pUserData->pControlUI);
		pLabelName->OnEvent += MakeDelegate(&OnTextEvent);
	}
}

void UpdateParentText(ItemData* pItemData)
{
	VEC_NODE vecParent = GetListParentsNode(pItemData);
	VEC_NODE_IT it = vecParent.begin();
	for (; it != vecParent.end(); it++)
	{
		ItemData* pNode = *it;
		UpdateGroupText(pNode);
	}
}

void UpdateGroupText( ItemData* pItemData )
{
	if (NULL == pItemData)
	{
		return;
	}
	CTreeItemUI* pTreeItem = static_cast<CTreeItemUI*>(pItemData->pControlUI);
	ASSERT(pTreeItem);
	if (pTreeItem->GetLevel() != -1)//�ų����ڵ�
	{
		CString strText;
		strText.Format(_T("%s(%d/%d)"), pItemData->csNodeName, pItemData->nOlineCount, pItemData->nTotalCount);
		pTreeItem->SetText(strText);
		pTreeItem->SetUserData(strText);
	}
}

VEC_NODE GetListParentsNode( ItemData* pNode )
{
	VEC_NODE vecParentsNode;
	CTreeItemUI* pTreeItem = static_cast<CTreeItemUI*>(pNode->pControlUI);
	ItemData* pParentsNode = (ItemData*)pTreeItem->GetParentItem()->GetTag();
	while(pParentsNode != NULL)
	{
		vecParentsNode.push_back(pParentsNode);
		CTreeItemUI* pParentTreeItem = static_cast<CTreeItemUI*>(pParentsNode->pControlUI);
		pParentsNode = (ItemData*)pParentTreeItem->GetParentItem()->GetTag();
		//pParentsNode = pParentsNode->parent();
	}
	return vecParentsNode;
}

void OutputSystemTime( CString strTip /*= _T("")*/ )
{
	CString strThisTime;
	strThisTime = ReadConfigText(_T("PluginText"), _T("MEMBER_Time"));
	if (strThisTime.IsEmpty())
	{
		strThisTime = _T("��ǰʱ��Ϊ");
	}
	if (strTip == _T(""))
	{
		strTip = strThisTime;
	}
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	CString strTime;
	strTime.Format(_T("%s��%02d:%02d:%02d.%03d \r\n"),strTip, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
	OutputDebugString(strTime);
}

bool IsPresider( ItemData* pItemData )
{
	if (MMT_OWNER == pItemData->nMemberType
		|| MMT_PRESIDER == pItemData->nMemberType
		|| MMT_PRESIDERINVITEE == pItemData->nMemberType
		|| MMT_PRESIDERTEMP == pItemData->nMemberType
		|| MMT_MP == pItemData->nMemberType)
	{
		return true;
	}
	return false;
}

bool IsPresider( unsigned short nMemberType )
{
	if (MMT_OWNER == nMemberType
		|| MMT_PRESIDER == nMemberType
		|| MMT_PRESIDERINVITEE == nMemberType
		|| MMT_PRESIDERTEMP == nMemberType
		|| MMT_MP == nMemberType)
	{
		return true;
	}
	return false;
}





