#ifndef __UITREE_H__
#define __UITREE_H__
#include <vector>
#include "../Layout/UIVerticalLayout.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace CoUI {
/////////////////////////////////////////////////////////////////////////////////////
// UITree

// Flags for CTreeUI Insert()
#define UITREE_INSERT_ROOT			0x0001
#define UITREE_INSERT_FIRST			0x0002
#define UITREE_INSERT_LAST			0x0004
#define UITREE_INSERT_SORT			0x0008

// Flags for CTreeUI FindSelectable()
#define UITREE_FIND_PREV			0x0001
#define UITREE_FIND_NEXT			0x0002
#define UITREE_FIND_FIRST			0x0004
#define UITREE_FIND_LAST			0x0008

typedef struct tagTTreeInfoUI
{
	int nFont;
	UINT uTextStyle;
	RECT rcTextPadding;
	DWORD dwTextColor;
	DWORD dwBkColor;
	CStdString sBkImage;
	DWORD dwSelectedTextColor;
	DWORD dwSelectedBkColor;
	CStdString sSelectedImage;
	DWORD dwHotTextColor;
	DWORD dwHotBkColor;
	CStdString sHotImage;
	DWORD dwPushedTextColor;
	DWORD dwPushedBkColor;
	CStdString sPushedImage;
	DWORD dwDisabledTextColor;
	DWORD dwDisabledBkColor;
	CStdString sDisabledImage;
	bool bShowHtml;
	bool bMultiExpandable;
	CStdString sGroupCollapseImage;
	CStdString sGroupExpandImage;
	bool bDownSelect;
	UINT nIndent;
} TTreeInfoUI;

typedef struct tagTTreeItemUI
{
	CControlUI* pParent;
	CControlUI* pPrevItem;
	CControlUI* pNextItem;
	tagTTreeItemUI()
	{
		pParent = NULL;
		pPrevItem = NULL;
		pNextItem = NULL;
	}
} TTreeItemUI;

typedef bool (*LPTREESORT)(CControlUI* pItem1, CControlUI* pItem2);

class ITreeUI
{
public:
	virtual CControlUI* GetCurSel() const = 0;
	virtual bool SelectItem(CControlUI* pItem, bool bTakeFocus = false) = 0;
	virtual void DoEvent(TEventUI& event) = 0;  

	virtual TTreeInfoUI* GetTreeInfo() = 0;

	virtual bool GetMultiExpandable() const = 0;
	virtual void SetMultiExpandable(bool bMultiExpand) = 0;
	virtual bool ExpandItem(CControlUI* pItem, bool bExpand) = 0;
	// 如果开启了允许展开多项，则此函数返回值为空（后续可根据需求完善）
	virtual CControlUI* GetExpandedItem() const = 0;

	// 开启双击展开项（默认开启）
	virtual void EnableDBClickExpand(bool bEnable) = 0;
	virtual bool IsEnableDBClickExpand() const = 0;

	// 此函数用在对树中大量数据排序时使用，如果树为显示的，排序一个节点会立刻刷新
	// 树，所以为了避免大量刷新，应在大量数据排序前关闭刷新，并在排序后开启刷新
	virtual void EnableUpdate(bool bUpdate) = 0;

	// 可能有多个，这里只能获取第一个
	virtual CControlUI* GetRoot() = 0;
};

class ITreeItemUI
{
#define SORT_KEY_COUNT		5		// 排序Key的数目
public:
	virtual ITreeUI* GetOwner() = 0;
	virtual void SetOwner(ITreeUI* pOwner) = 0;

	virtual bool IsSelected() const = 0;
	virtual bool Select(bool bSelect = true) = 0;

	//************************************
	// 说明:    设置节点的排序key,如果使用key排序，则会逐个按照key的
	//			索引顺序进行字符串比较,使用第一个不相等的结果作为排序比较结果
	// 参数:	nIndex	索引值在SORT_KEY_COUNT范围内,不在正确范围设置将无效
	// 参数:	pStrKey	Key值
	// 参数:	bSort	是否立刻重新排序其父节点
	// 返回值:  void
	//************************************
	virtual void SetSortKey(int nIndex, LPCTSTR pStrKey, bool bSort = false) = 0;
	virtual LPCTSTR GeSorttKey(int nIndex) = 0;

	// 设置项图标，项图标大小是项高度所组成的一个正方形，排在组图标后面
	virtual void SetItemImage(STRINGorID pStrImage) = 0;
	virtual LPCTSTR GetItemImage() const = 0;
	virtual void SetShowItemImage(bool bShow) = 0;
	virtual bool IsShowItemImage() = 0;

	virtual CControlUI* GetParentItem() const = 0;

	// 获取前一个兄弟项
	virtual CControlUI* GetPrevSiblingItem() = 0;
	// 获取后一个兄弟项
	virtual CControlUI* GetNextSiblingItem() = 0;

	virtual bool IsExpanded() const = 0;
	virtual bool Expand(bool bExpand = true) = 0;

	virtual int GetCount() const = 0;
	virtual bool IsHasChildren() = 0;

	virtual int GetLevel() = 0;

	// 获取第一个子项
	virtual CControlUI* GetChildItem() = 0;
};

class CTreeItemUI;

class UILIB_API CTreeUI : public CVerticalLayoutUI, public ITreeUI
{
	friend class CTreeItemUI;
public:
	CTreeUI(void);
	~CTreeUI(void);

	LPCTSTR GetClass() const;
	UINT GetControlFlags() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	//////////////////////////////////////////////////////////////////////////
	// 继承自CVerticalLayoutUI
	int GetCount() const;
	bool Add(CControlUI* pControl);
	bool AddAt(CControlUI* pControl, int iIndex) { return false; }
	CControlUI* GetItemAt(int iIndex) const;

	void SetVisible(bool bVisible = true);
	void SetInternVisible(bool bVisible = true);

	void SetScrollPos(SIZE szPos);
	void SetPos(RECT rc);

	void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);
	CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	
	void DoPaint(HDC hDC, const RECT& rcPaint);
	void DoEvent(TEventUI& event);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	//////////////////////////////////////////////////////////////////////////
	// 继承自ITreeUI
	CControlUI* GetCurSel() const;
	virtual bool SelectItem(CControlUI* pItem, bool bTakeFocus = false);
	// 允许取消选中，即选中空白处后失去选中，默认允许,true:允许，false:不允许
	void EnableDeselect(bool bEnable);
	bool IsDeselect(void) const;
    // 是否在按下时选中，默认true,true:按下选中，false:点击选中
	void SetDownSelect(bool bDownSelect);
	bool IsDownSelect() const;

	TTreeInfoUI* GetTreeInfo();
	bool GetMultiExpandable() const;
	void SetMultiExpandable(bool bMultiExpand);
	bool ExpandItem(CControlUI* pItem, bool bExpand);
	CControlUI* GetExpandedItem() const;
	void EnableDBClickExpand(bool bEnable);
	bool IsEnableDBClickExpand() const;
	// 此函数用在对树中大量数据排序时使用，如果树为显示的，排序一个节点会立刻刷新
	// 树，所以为了避免大量刷新，应在大量数据排序前关闭刷新，并在排序后开启刷新
	void EnableUpdate(bool bUpdate);
	CControlUI* GetRoot();

	// nCode只可能是以下四个值中的一个
	// UITREE_INSERT_ROOT		插入一个根节点，此时将忽略pParent参数
	// UITREE_INSERT_FIRST		插入到所有子节点之前
	// UITREE_INSERT_LAST		插入到所有子节点之后
	// UITREE_INSERT_SORT		插入后立即排序,应将排序函数指针传入，否则无法排序
	bool Insert(CControlUI* pControl, CControlUI* pParent = NULL, int nCode = UITREE_INSERT_ROOT, LPTREESORT lpTreeSort = NULL);
	bool Remove(CControlUI* pControl);
	void RemoveAll();

	// 排序某个节点的子节点，如果pControl为NULL，则排序整棵树
	// bRecursive 是否递归排序其所有有子节点的子节点（有点儿绕…）
	bool SortChildren(CControlUI* pControl, LPTREESORT pSort, bool bRecursive = true);
	// 通过节点的Key排序，如果pControl为NULL，则排序整棵树
	// 虽然没有设置Key依然可以排序，但我保证结果绝不是你希望的
	bool SortChildrenByKey(CControlUI* pControl, bool bRecursive = true);

	void EnsureVisible(CControlUI* pControl, bool bTop = false);

	void SetItemFont(int index);
	void SetItemTextStyle(UINT uStyle);
	void SetItemTextPadding(RECT rc);
	void SetItemTextColor(DWORD dwTextColor);
	void SetItemBkColor(DWORD dwBkColor);
	void SetItemBkImage(LPCTSTR pStrImage);
	void SetSelectedItemTextColor(DWORD dwTextColor);
	void SetSelectedItemBkColor(DWORD dwBkColor);
	void SetSelectedItemImage(LPCTSTR pStrImage); 
	void SetHotItemTextColor(DWORD dwTextColor);
	void SetHotItemBkColor(DWORD dwBkColor);
	void SetHotItemImage(LPCTSTR pStrImage);
	void SetPushedItemImage(LPCTSTR pStrImage);
	void SetPushedItemTextColor(DWORD dwTextColor);
	void SetPushedItemBkColor(DWORD dwBkColor);
	void SetDisabledItemTextColor(DWORD dwTextColor);
	void SetDisabledItemBkColor(DWORD dwBkColor);
	void SetDisabledItemImage(LPCTSTR pStrImage);
	bool IsItemShowHtml();
	void SetItemShowHtml(bool bShowHtml = true);
	RECT GetItemTextPadding() const;
	DWORD GetItemTextColor() const;
	DWORD GetItemBkColor() const;
	LPCTSTR GetItemBkImage() const;
	DWORD GetSelectedItemTextColor() const;
	DWORD GetSelectedItemBkColor() const;
	LPCTSTR GetSelectedItemImage() const;
	DWORD GetHotItemTextColor() const;
	DWORD GetHotItemBkColor() const;
	LPCTSTR GetHotItemImage() const;
	LPCTSTR GetPushedItemImage() const;
	DWORD GetPushedItemTextColor() const;
	DWORD GetPushedItemBkColor() const;
	DWORD GetDisabledItemTextColor() const;
	DWORD GetDisabledItemBkColor() const;
	LPCTSTR GetDisabledItemImage() const;
	void SetGroupCollapseImage(LPCTSTR pStrImage);
	LPCTSTR GetGroupCollapseImage() const;
	// 设置树的组图片，组图片的大小项高度所组成的正方形
	void SetGroupExpandImage(LPCTSTR pStrImage);
	LPCTSTR GetGroupExpandImage() const;
	void SetIndent(int nIndent);
	int GetIndent() const;

	//交换父节点相同的两项
	void ExchangeItem(CTreeItemUI* pItem1, CTreeItemUI* pItem2);

	bool IsShowTip();
	void SetShowTip(bool bShowTip = false);
	bool AddTip(CControlUI* pTip);
	CTreeItemUI* FindTipItem();
	CTreeItemUI* GetCurTipItem();


protected:
	CControlUI* FindSelectable(CControlUI* pControl, UINT uFlags) const;

	bool			m_bDeselect;
	bool			m_bDBClickExpand;
	bool			m_bEnableUpdate;
	CControlUI*		m_pCurSel;
	CControlUI*		m_pExpandedItem;
	TTreeInfoUI		m_TreeInfo;
	CTreeItemUI*	m_pRoot;

	bool            m_bShowTip;
	CControlUI*     m_pTip;
	CTreeItemUI*    m_pCurTipItem;

private:
	LONG	m_lDelayDeltaY;
	DWORD	m_dwDelayNumber;
	DWORD	m_dwDelayLeft;
};

class UILIB_API CTreeItemUI : public CControlUI, public ITreeItemUI, public IContainerUI
{
	friend class CTreeUI;
	friend class CComboTreeUI;
public:
	CTreeItemUI();
	~CTreeItemUI();

	LPCTSTR GetClass() const;
	UINT GetControlFlags() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	//////////////////////////////////////////////////////////////////////////
	// 继承自IContainerUI
	CControlUI* GetItemAt(int iIndex) const;
	int GetItemIndex(CControlUI* pControl) const { return -1; }
	bool SetItemIndex(CControlUI* pControl, int iIndex) { return false; }
	bool Add(CControlUI* pControl);	
	bool AddAt(CControlUI* pControl, int iIndex) { return false; }
	bool RemoveAt(int iIndex) { return true ;}

	//////////////////////////////////////////////////////////////////////////
	// 继承自CControlUI
	void SetEnabled(bool bEnable = true);

	void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
	bool Activate();

	void SetPos(RECT rc);
	SIZE EstimateSize(SIZE szAvailable);
	void DoPaint(HDC hDC, const RECT& rcPaint);
	bool DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);
	void PaintGroupImage(HDC hDC);
	void PaintItemImage(HDC hDC);
	void PaintText(HDC hDC);
	void PaintItemBk(HDC hDC);
	void DoEvent(TEventUI& event);

	void SetVisible(bool bVisible = true);
	void SetInternVisible(bool bVisible = true);

	void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);
	CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	//////////////////////////////////////////////////////////////////////////
	// 继承自 ITreeItemUI
	ITreeUI* GetOwner();
	void SetOwner(ITreeUI* pOwner);

	bool IsSelected() const;
	bool Select(bool bSelect = true);

	void SetSortKey(int nIndex, LPCTSTR pStrKey, bool bSortParent = false);
	LPCTSTR GeSorttKey(int nIndex);

	// 设置项图标
	void SetItemImage(STRINGorID pStrImage);
	LPCTSTR GetItemImage() const;
	void SetShowItemImage(bool bShow);
	bool IsShowItemImage();
	// 设置是否显示组图标。nShow大于0则一直显示，等于0则有子节点才显示,无子节点则不显示
	// 小于0则一直不显示，默认nShow等于0
	void SetShowGroupImage(int nShow);
	int IsShowGroupImage();

	CControlUI* GetParentItem() const;
	// 获取前一个兄弟项
	CControlUI* GetPrevSiblingItem();
	// 获取后一个兄弟项
	CControlUI* GetNextSiblingItem();

	int GetCount() const;
	bool IsHasChildren();

	int GetLevel();

	void ExchangeItem(CTreeItemUI* pItem1, CTreeItemUI* pItem2);
	void ItemUp();
	void ItemDown();

	// 获取第一个子项
	CControlUI* GetChildItem();
	CControlUI* GetLastItem();

	bool IsExpanded() const;
	bool Expand(bool bExpand = true);
protected:
	bool Insert(CControlUI* pControl, int nCode = UITREE_INSERT_LAST, LPTREESORT lpTreeSort = NULL);
	bool Remove(CControlUI* pControl);
	void RemoveAll(); 
	void SortChildren(LPTREESORT pSort, bool bRecursive = true);
	void SortChildrenByKey(bool bRecursive = true);
	// 初始化所有子节点的亲属关系
	void InitRelation();
	void SetLevel(int level);
	CTreeItemUI* FindTipItem();

private:
	static bool Compare( CControlUI* pControl1, CControlUI* pControl2 );

protected:
	int m_nLevel;
	bool m_bSelected;
	bool m_bShowImage;
	int m_nShowGroupImage;
	UINT m_uButtonState;

	CStdString m_sItemImage;
	CStdString m_aStrSortKey[SORT_KEY_COUNT];

	ITreeUI* m_pOwner;
	TTreeItemUI m_TreeItem;

	CContainerUI* m_pContainer;
	bool m_bExpand;
	std::vector<CControlUI*> m_vecChildren;
	bool m_bDelayedDestroy;
};

} // namespace CoUI

#pragma warning(pop)
#endif // __UITREE_H__
