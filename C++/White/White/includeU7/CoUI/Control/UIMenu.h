#ifndef __UIMENU_H__
#define __UIMENU_H__

#include "UIList.h"

namespace CoUI {

/////////////////////////////////////////////////////////////////////////////////////
//
#define MENUF_MODAL 0x0001	//ģ̬�˵�
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
	// �˵�����
	static CMenuUI* LoadMenu(STRINGorID xml, LPCTSTR pSkinType = NULL);

	/*-------------------------------------------------------
	�ɱ������ڲ���uFlags��ı�־
	MF_STRING��ָ���˵�����һ�������ַ�����ȱʡ��������pstrNewItemָ����ַ�����
	MF_BITMAP����һ��λͼ�����˵������pstrNewItem��ͼƬ·��������ʱ���ã�
	MF_ENABLED��ʹ�˵�����Ч��ʹ�����ܱ�ѡ�񣬲�ʹ��ӱ�ҵ�״̬�ָ�(ȱʡ)��
����MF_DISABLED��ʹ�˵�����Ч��ʹ����ܱ�ѡ�񣬵���ʹ�˵����ҡ�
����MF_GRAYED��ʹ�˵�����Ч����ң�ʹ�䲻�ܱ�ѡ��
����MF_SEPARATOR����һ��ˮƽ�����ߡ��˱�־ֻ������ʽ�˵����Ӳ˵����ݲ˵�ʹ�á��������߲��ܱ���ҡ���Ч�����������nIDNewItem��pstrNewItem���á�
  ��MF_CHECKED���ڲ˵����Ա߷���һ��ѡȡ��ǡ����Ӧ�ó����ṩһ��ѡȡ��ǣ�λͼ���μ�SetMenultemBitmaps������ѡȡ���λͼ�����ڲ˵����Աߡ�
	MF_UNCHECKED��������ѡȡ����ڲ˵����Աߣ�ȱʡ�������Ӧ�ó����ṩһ��ѡȡ���λͼ���μ�SetMenultemBitmaps������ѡȡ���λͼ�����ڲ˵����Աߡ�
	-------------------------------------------------------*/
	bool AppendMenu(UINT nFlags, UINT nIDNewItem = 0, LPCTSTR pstrNewItem = NULL);

	// �˺�����ʱ����
	bool AppendMenu(UINT nFlags, UINT nIDNewItem, LPCTSTR pstrNewItem, HBITMAP hBmp);

	/*-------------------------------------------------------
	uIDCheckItem��ָ��Ҫ�޸ĵĲ˵��
����uCheck����ʾ��ǵ�״̬���˲�����ȡ����ֵ֮һ��
����	MF_BYCOMMAND����ʾ����uId�����˵���ı�ʶ�������MF_BYCOMMAND��MF_BYPOSITION��û��ָ������MF_BYCOMMAND��ȱʡֵ��
����	MF_BYPOSITION����ʾ����uId�����˵�����������λ�á�
		MF_CHECKED���ڲ˵����Ա߷���һ��ѡȡ��ǡ����Ӧ�ó����ṩһ��ѡȡ��ǣ�λͼ���μ�SetMenultemBitmaps������ѡȡ���λͼ�����ڲ˵����Աߡ�
		MF_UNCHECKED��������ѡȡ����ڲ˵����Աߣ�ȱʡ�������Ӧ�ó����ṩһ��ѡȡ���λͼ���μ�SetMenultemBitmaps������ѡȡ���λͼ�����ڲ˵����Աߡ�
	-------------------------------------------------------*/
	bool CheckMenuItem(UINT nIDCheckItem, UINT nCheck);

	/*-------------------------------------------------------
	uIDCheckItem��ָ��Ҫ�޸ĵĲ˵��
��	nEnable ���ο�ModifyMenu�����еĲ˵�������־����������г�������ʹ�õ����г�����
	 �������������ֻ��ָ������������MF_BYCOMMAND��MF_BYPOSITION��MF_ENABLED��MF_DISABLED�Լ�MF_GRAYED
	-------------------------------------------------------*/
	bool EnableMenuItem(UINT nIDEnableItem, UINT nEnable);

	UINT GetMenuItemID(int nPos) const;
	
	UINT GetMenuState(UINT nID, UINT nFlags);

	/*-------------------------------------------------------
��  uIDItem��ָ��Ҫ��ȡ�ı��Ĳ˵���京���ɲ���uFlag������
����uFlag��ָ������ulDltem��α����͡��˲�����ȡ����ֵ֮һ��
����	MF_BYCOMMAND����ʾ����uIDltem�����˵���ı�ʶ�������MF_BYCOMMAND��MF_BYPOSITION��û��ָ������MF_BYCOMMAND��ȱʡֵ��
����	MF_BYPOSITION����ʾ����uIDltem�����˵�����������λ�á�
	-------------------------------------------------------*/
	LPCTSTR GetMenuString(UINT nIDItem, UINT nFlags);

	CMenuUI GetSubMenu(int nPos);
	bool IsSubMenu(void);

	bool InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0, LPCTSTR pstrNewItem = NULL);

	bool ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0, LPCTSTR pstrNewItem = NULL);

	bool RemoveMenu(UINT nPosition, UINT nFlags);
	/*-------------------------------------------------------
	uPosition��ָ�������޸ĵĲ˵���京���ɲ���uFlags������
    UFlags��ָ������uPosition����ν��͡��˲�������������ֵ֮һ��
		MF_BYCOMMAND����ʾ����uPosition�����˵���ı�ʶ�������MF_BYCOMMAND��MF_POSITION��û��ָ������MF_BYCOMMAND��ȱʡ��־��
		MF_BYPOSITION����ʾ����uPosition�����˵�����������λ�á�
	ͼ���С17*17
    strUncheckedImage�����˵���û��ѡȡʱ��ʾ��λͼ��·����
    strCheckedImage�����˵��ѡȡʱ��ʾ��λͼ��·����
	-------------------------------------------------------*/
	bool SetMenuItemBitmaps(UINT nPosition, UINT nFlags, LPCTSTR strUncheckedImage, LPCTSTR strCheckedImage);

	// ���ò˵���ͷͼƬ ͼ���С20*20
	LPCTSTR GetArrowImage() const;
	void SetArrowImage(LPCTSTR pStrImage); 

	// �������ͼ����������ɫ��Ĭ����ɫ0xFFE8F3FD
	void SetImageBKColor(DWORD dwColor);
	DWORD GetImageBKColor();

	// ������������ɫ��Ĭ����ɫ0xFFE8F3FD
	void SetSeparatorColor(DWORD dwColor);
	DWORD GetSeparatorColor();

	// �˺�����ʱ����
	bool CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags);

	// �õ��˵������ߴ��ھ��
	HWND GetOwnerWnd();

	// ��ָ��λ����ʾ��ݲ˵��������ٲ˵����ѡ�񡣿�ݲ˵��ɳ�������Ļ�ϵ��κ�λ�á�
	//Ŀǰ����nFlagsΪģ̬��ǩ��MENUF_MODAL��ʱ������ģ̬���ڣ����ز˵���ID������0��ʾû��ѡ�У�
	//�������Ϊ��ģ̬��������������1�����򷵻�0
	//��������������Ϊ���˵������ھ����������Ϣ(WM_COMMAND)��Ŀ�괰�ھ��[�μ���Ӧ�ĳ�Ա����]
	UINT TrackPopupMenu(UINT nFlags, int x, int y, HWND hParentWnd, HWND hMsgWnd = NULL);
	
	//ͳһ���ò˵���ĸ߶�
	void SetElementFixedHeight(int nHeight);
	int GetElementFixedHeight(void);
	//�����Ƿ�������ʾ�ָ��ߣ�Ĭ�����ͼ�겿�ֲ���ʾ�ָ��ߣ�
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
	//��Ϣ���͵�ָ��Ŀ�괰�ڣ������õĻ���Ĭ�Ϸ�������owner���ڣ�
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
