#pragma once
#include "Node.h"

typedef BOOL (*LPCMPFUN)( CNode* pItem1, CNode* pItem2);

class CListUIEx : public CListUI
{
public:
	CListUIEx(CPaintManagerUI& paint_manager);
	~CListUIEx();

public:
	void Init(CString pStrHotImage, CString pStrSelImage);
	bool Add(CControlUI* pControl);
	bool AddAt(CControlUI* pControl, int iIndex);
	CNode* AddNode(const MeetingListItemInfo& item, CNode* parent = NULL);

	bool Remove(CControlUI* pControl);
	bool RemoveNode(CNode* node);
	bool RemoveAt(int nIndex);
	void RemoveAll();

	void DoEvent(TEventUI& event);

	CNode* GetRoot();
	bool CanExpand(CNode* node) const;
	bool SelectItem(int iIndex, bool bTakeFocus = false);
	void SetChildVisible(CNode* node, bool visible);
	void SortItems(LPCMPFUN lpCmpFun, CNode* pParant);

	void SetItemText(CNode* pNode);
	void SetItemText(CListContainerElementUI* pListElement, CString strRoomName);
	void SetItemPic(CListContainerElementUI* pListElement, CString strLogo, CString strAppendLogo = _T(""), GroupStyle eStyle = STYLE_MEMBER);
	void SetAppendPic(CListContainerElementUI* pListElement, CString strAppendLogo);
	void SetStatusItemPic(CListContainerElementUI* pListElement, CString strSubCtrlName, CString strLogo);

	void EnableBtn(CListContainerElementUI* pListElement, CString strSubCtrlName, bool bEnble);
	void InsertItem(CNode* pLastNode, CNode* pCurtNode);
	void SetOperatorBtnPic(CListContainerElementUI* pListElement, CString strSubCtrlName, CString strImage);
private:
	CNode*				m_RootNode;
	CNode*				m_SelNode;
	UICRect				m_TextPadding;
	int					m_LevelTextStartPos;
	CString			m_LevelExpandImage;
	CString			m_LevelCollapseImage;

	CPaintManagerUI&	paint_manager_;
	CDialogBuilder		m_dlgBuilder;
};
