#ifndef __UIMENU_H__
#define __UIMENU_H__

#include "UIList.h"

namespace CoUI {

/////////////////////////////////////////////////////////////////////////////////////
//
#define MENUF_MODAL 0x0001	//模态菜单
enum MenuAlignment
{
	eMenuAlignment_Left = 1 << 1,
	eMenuAlignment_Top = 1 << 2,
	eMenuAlignment_Right = 1 << 3,
	eMenuAlignment_Bottom = 1 << 4,
};

// MenuUI

class CListUI;
class CMenuWnd;
class CMenuElementUI;
class CDialogBuilder;
class UILIB_API CMenuUI : public CListUI 
{
	friend CMenuElementUI;
public:
	CMenuUI();
	~CMenuUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    virtual bool Add(CControlUI* pControl);
    virtual bool AddAt(CControlUI* pControl, int iIndex);

    virtual int GetItemIndex(CControlUI* pControl) const;
    virtual bool SetItemIndex(CControlUI* pControl, int iIndex);
    virtual bool Remove(CControlUI* pControl);

	SIZE EstimateSize(SIZE szAvailable);

	void PaintBkColor(HDC hDC);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	////////////////////////////////////////////////////////////////////////////
	// 菜单操作
	static CMenuUI* LoadMenu(STRINGorID xml, LPCTSTR pSkinType = NULL);

	/*-------------------------------------------------------
	可被设置在参数uFlags里的标志
	MF_STRING：指定菜单项是一个正文字符串（缺省）；参数pstrNewItem指向该字符串。
	MF_BITMAP：将一个位图用作菜单项。参数pstrNewItem是图片路径。（暂时无用）
	MF_ENABLED：使菜单项有效，使该项能被选择，并使其从变灰的状态恢复(缺省)。
　　MF_DISABLED：使菜单项无效，使该项不能被选择，但不使菜单项变灰。
　　MF_GRAYED：使菜单项无效并变灰，使其不能被选择。
　　MF_SEPARATOR：画一条水平区分线。此标志只被下拉式菜单、子菜单或快捷菜单使用。此区分线不能被变灰、无效或加亮。参数nIDNewItem和pstrNewItem无用。
  　MF_CHECKED：在菜单项旁边放置一个选取标记。如果应用程序提供一个选取标记，位图（参见SetMenultemBitmaps），则将选取标记位图放置在菜单项旁边。
	MF_UNCHECKED：不放置选取标记在菜单项旁边（缺省）。如果应用程序提供一个选取标记位图（参见SetMenultemBitmaps），则将选取标记位图放置在菜单项旁边。
	-------------------------------------------------------*/
	bool AppendMenu(UINT nFlags, UINT nIDNewItem = 0, LPCTSTR pstrNewItem = NULL);

	// 此函数暂时无用
	bool AppendMenu(UINT nFlags, UINT nIDNewItem, LPCTSTR pstrNewItem, HBITMAP hBmp);

	/*-------------------------------------------------------
	uIDCheckItem：指定要修改的菜单项。
　　uCheck：表示标记的状态。此参数可取下列值之一：
　　	MF_BYCOMMAND：表示参数uId给出菜单项的标识符。如果MF_BYCOMMAND和MF_BYPOSITION都没被指定，则MF_BYCOMMAND是缺省值。
　　	MF_BYPOSITION：表示参数uId给出菜单项相对于零的位置。
		MF_CHECKED：在菜单项旁边放置一个选取标记。如果应用程序提供一个选取标记，位图（参见SetMenultemBitmaps），则将选取标记位图放置在菜单项旁边。
		MF_UNCHECKED：不放置选取标记在菜单项旁边（缺省）。如果应用程序提供一个选取标记位图（参见SetMenultemBitmaps），则将选取标记位图放置在菜单项旁边。
	-------------------------------------------------------*/
	bool CheckMenuItem(UINT nIDCheckItem, UINT nCheck);

	/*-------------------------------------------------------
	uIDCheckItem：指定要修改的菜单项。
　	nEnable ，参考ModifyMenu函数中的菜单常数标志定义表，其中列出了允许使用的所有常数。
	 对于这个函数，只能指定下述常数：MF_BYCOMMAND，MF_BYPOSITION，MF_ENABLED，MF_DISABLED以及MF_GRAYED
	-------------------------------------------------------*/
	bool EnableMenuItem(UINT nIDEnableItem, UINT nEnable);

	UINT GetMenuItemID(int nPos) const;
	
	UINT GetMenuState(UINT nID, UINT nFlags);

	/*-------------------------------------------------------
　  uIDItem：指定要获取文本的菜单项，其含义由参数uFlag决定。
　　uFlag：指定参数ulDltem如何被解释。此参数可取下列值之一：
　　	MF_BYCOMMAND：表示参数uIDltem给出菜单项的标识符。如果MF_BYCOMMAND和MF_BYPOSITION都没被指定，则MF_BYCOMMAND是缺省值。
　　	MF_BYPOSITION：表示参数uIDltem给出菜单项相对于零的位置。
	-------------------------------------------------------*/
	LPCTSTR GetMenuString(UINT nIDItem, UINT nFlags);

	CMenuUI GetSubMenu(int nPos);
	bool IsSubMenu(void);

	bool InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0, LPCTSTR pstrNewItem = NULL);

	bool ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0, LPCTSTR pstrNewItem = NULL);

	bool RemoveMenu(UINT nPosition, UINT nFlags);
	/*-------------------------------------------------------
	uPosition：指定将被修改的菜单项。其含义由参数uFlags决定。
    UFlags：指定参数uPosition将如何解释。此参数必须是下列值之一：
		MF_BYCOMMAND：表示参数uPosition给出菜单项的标识符。如果MF_BYCOMMAND和MF_POSITION都没被指定，则MF_BYCOMMAND是缺省标志。
		MF_BYPOSITION：表示参数uPosition给出菜单项相对于零的位置。
	图标大小17*17
    strUncheckedImage：当菜单项没被选取时显示的位图的路径。
    strCheckedImage：当菜单项被选取时显示的位图的路径。
	-------------------------------------------------------*/
	bool SetMenuItemBitmaps(UINT nPosition, UINT nFlags, LPCTSTR strUncheckedImage, LPCTSTR strCheckedImage);

	// 设置菜单箭头图片 图标大小20*20
	LPCTSTR GetArrowImage() const;
	void SetArrowImage(LPCTSTR pStrImage); 

	// 设置左侧图标区背景颜色，默认颜色0xFFE8F3FD
	void SetImageBKColor(DWORD dwColor);
	DWORD GetImageBKColor();

	// 设置区分线颜色，默认颜色0xFFE8F3FD
	void SetSeparatorColor(DWORD dwColor);
	DWORD GetSeparatorColor();

	// 此函数暂时无用
	bool CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags);

	// 得到菜单所有者窗口句柄
	HWND GetOwnerWnd();

	// 在指定位置显示快捷菜单，并跟踪菜单项的选择。快捷菜单可出现在屏幕上的任何位置。
	//目前：当nFlags为模态标签（MENUF_MODAL）时，弹出模态窗口，返回菜单的ID（返回0表示没有选中）
	//其它情况为非模态，正常弹出返回1，否则返回0
	//后两个参数依次为：菜单父窗口句柄，反馈消息(WM_COMMAND)的目标窗口句柄[参见相应的成员变量]
	UINT TrackPopupMenu(UINT nFlags, int x, int y, HWND hParentWnd, HWND hMsgWnd = NULL);
	
	//统一设置菜单项的高度
	void SetElementFixedHeight(int nHeight);
	int GetElementFixedHeight(void);
	//设置是否完整显示分隔线（默认左边图标部分不显示分隔线）
	void SetFullLineSeparator(bool bFull=true);
	bool IsFullLineSeparator(void);

	//////////////////////////////////////////////////////////////////////////
protected:
	CMenuUI(CMenuElementUI* pElement);
	bool CreatMenuWnd();
	CMenuElementUI* GetElementByCommand(UINT nCommand);
	CMenuElementUI* GetElementByPos(UINT nID);

protected:
	CDialogBuilder* m_pBuilder;
	CMenuWnd* m_pWindow;
	STRINGorID m_xml;
	CStdString m_sType;
	CStdString m_sArrowImage;
	DWORD m_dwImageBKColor;
	DWORD m_dwSeparatorColor;
	CMenuElementUI* m_pElement;
	HWND m_hOwner;
	int m_nElementFixedHeight;
	bool m_bFullSeparator;
	//消息发送到指定目标窗口（不设置的话，默认发送它的owner窗口）
	HWND m_hDesMsgWnd;
};

/////////////////////////////////////////////////////////////////////////////////////
//

// MenuElementUI
class CListContainerElementUI;
class UILIB_API CMenuElementUI : public CListContainerElementUI
{
  	friend CMenuWnd;
public:
    CMenuElementUI(UINT nFlags = MF_STRING);
	~CMenuElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoPaint(HDC hDC, const RECT& rcPaint);

	void DrawCheckImage(HDC hDC);

	void DrawArrowImage(HDC hDC);

	void DrawItemText(HDC hDC, const RECT& rcItem);

	SIZE EstimateSize(SIZE szAvailable);

	bool Activate();

	void SetPos(RECT rc);

	void DoEvent(TEventUI& event);

	bool CreateMenuUI();

	UINT GetElementFlags();
	void SetElementFlags(UINT nFlags);

	void ChecckElement(UINT nCheck);

	LPCTSTR GetUnCheckImage() const;
	void SetUnCheckImage(LPCTSTR pStrImage); 

	LPCTSTR GetCheckImage() const;
	void SetCheckImage(LPCTSTR pStrImage); 

protected:
	CMenuUI* m_pMenu;
	UINT m_nFlags;
	CStdString m_sUncheckImage;
	CStdString m_sCheckImage;
	CStdString m_sImageModify;
};

} // namespace CoUI

#endif // __UIMENU_H__
