#pragma once

#include <gdiplus.h>	//Image
#include <algorithm>	//find
#include <vector>
#include <hash_map>

#pragma comment(lib,"gdiplus.lib")
#pragma warning(disable:4996)

using namespace Gdiplus;
using namespace std;

// Helper Routine
extern void GradientFillRect(CDC *pDC, CRect &rtDC, COLORREF crFrom, COLORREF crTo, bool bGradient);
typedef struct tagANIMAP
{
	Image*	pImg;
	int		x;
	int		y;
	int		frame;
	int		offset;		// ������x����ƫ����
	int		align;		// ���뷽ʽ 0:��������� 1:�ؼ��Ҷ���
}ANIMAP, *PANIMAP;
typedef struct
{
	Image*	image;		//ͼƬ
	int		index;		//ͼƬ����
	int		frame;		//��ǰ֡
	int		x;			//x����
	int		y;			//y����...
	int		offset;		// ������x����ƫ����
	int		align;		// ���뷽ʽ 0:��������� 1:�ؼ��Ҷ���
}KITEMDATAINFO;

#define KITEMDATAINFO_SIZE	5	//ITEMMAP::KITEMDATAINFO ��С

typedef stdext::hash_map< int, HICON> ICONMAP;
typedef stdext::hash_map<HTREEITEM, KITEMDATAINFO *> ITEMMAP;
typedef stdext::hash_map<HTREEITEM, CString> ITEMTEXT;
typedef stdext::hash_map< int, Image*> IMAGEMAP;	//��̬ͼ���б�

		//�ػ���̬ͼ��
//---------------------------------------------------------------------------------------
// �Զ�����Ϣ
#define KTN_LBUTTONDOWN_TEXT	WM_USER + 0x0001
#define KTN_LBUTTONDOWN_ICON	WM_USER + 0x0002
#define KTN_LBUTTONUP_TEXT		WM_USER + 0x0003
#define KTN_LBUTTONUP_ICON		WM_USER + 0x0004
#define KTN_RBUTTONDOWN_TEXT	WM_USER + 0x0005
#define KTN_RBUTTONDOWN_ICON	WM_USER + 0x0006
#define KTN_RBUTTONUP_TEXT		WM_USER + 0x0007
#define KTN_RBUTTONUP_ICON		WM_USER + 0x0008

#define KTN_LDBCLICK_TEXT		WM_USER + 0x0009
#define KTN_LDBCLICK_ICON		WM_USER + 0x000A
#define KTN_RDBCLICK_TEXT		WM_USER + 0x000B
#define KTN_RDBCLICK_ICON		WM_USER + 0x000C

//---------------------------------------------------------------------------------------
// KTreeCtrl �ඨ��
class AFX_EXT_CLASS KTreeCtrl : public CTreeCtrl //KTreeCtrlToolTip
{
	DECLARE_DYNAMIC(KTreeCtrl)
public:
	enum BG_MODE
	{
		BG_MODE_BMP			= 0,
		BG_MODE_GRADIENT	= 1,
		BG_MODE_FILL		= 2,
	};

	KTreeCtrl();
	virtual ~KTreeCtrl();

	// ���ú��ӽڵ�Ľڵ�ͼ��
	void SetNodeIcon(HICON hOpenedIcon,HICON hClosedIcon);
	void DeleteAllItems();
	
	// ����ͼ����Դ
	BOOL SetItemIconList(int nIndexID, HICON hIcon);
	BOOL SetItemIconList(int nIndexID, Image* pImage);
	BOOL SetItemIconList(int nIndexID, LPCTSTR pszFile);

	BOOL SetItemAttachText(HTREEITEM hItem, CString text);

	// ���ýڵ�ͼ��
	BOOL SetItemIcon(HTREEITEM hItem,int nIndexID, int nType = 0);
	void SetItemIconFrame(HTREEITEM hItem, int nFrame, int nType = 0);

	// ����ͼ��λ�� offset ƫ���� align 0:������� 1:�����Ҷ� 2:�ؼ��Ҷ�
	void SetItemIconOffset(HTREEITEM hItem, int nType = 0, int align = 0, int offset = 0);

	// ȡ�ڵ�ͼ��
	int GetItemIconIndex(HTREEITEM hItem, int nType = 0);
	Image* GetItemImageHandle(HTREEITEM hItem, int nType = 0);

#ifdef MACRO_USE_ICON_
	HICON GetItemIconHandle(HTREEITEM hItem, int nType = 0);
#endif

	// ����ͼ�������Ľڵ�
	BOOL SetFlashIcon(HTREEITEM hItem, BOOL bEnable=TRUE, int nType = 0);

	// ������̬ͼ����ʾ
	void EnableAni(BOOL bEnable=TRUE);

	// ���ö�̬ͼ��֡��(ms)
	void SetFrameSpeed(DWORD dwTime);

	void SetGradientColor(COLORREF crSelect,COLORREF crFrom, COLORREF crTo);
	COLORREF GetSelectGradientColor(void);
	COLORREF GetFromGradientColor(void);
	COLORREF GetToGradientColor(void);

	BG_MODE GetBackgroundMode(void){return m_BackgroundMode;}
	void SetBackgroundMode(BG_MODE bgmode){m_BackgroundMode=bgmode;}

	bool GetBitmapTiledMode(void){return m_bBmpTiledMode;}
	void SetBitmapTiledMode(bool bTiled){m_bBmpTiledMode=bTiled;}
	void SetBitmapID(UINT nBitmapID);

	bool GetGradientHorz(void){return m_bGradientHorz;}
	void SetGradientHorz(bool bHorz){m_bGradientHorz = bHorz;} 
	
	bool ItemIsVisible(HTREEITEM item);

private:
	COLORREF	m_FromGradientColor;		// Gradient variables
	COLORREF	m_ToGradientColor;
	COLORREF	m_SelectGradientColor;
	bool		m_bGradientHorz;			// horz or vertical gradient

	BG_MODE		m_BackgroundMode;			// Current Background mode
    CBitmap		m_bmpBackground;			// Background bitmap image
	HICON		m_hOpenedIcon;				// Tree opened Icon Image(marker)
	HICON		m_hClosedIcon;				// Tree closed Icon Image(marker)
	bool		m_bBmpTiledMode;			// Tile background image

	CRect		m_rtClient;					// The client rect when drawing
	int			m_nOffsetH;					// 0... -x (scroll offset)
	int			m_nSizeH;					// width of unclipped window
	int			m_nOffsetV;					// 0... -y (scroll offset)
	int			m_nSizeV;					// height of unclipped window

#ifdef MACRO_USE_ICON_
	ICONMAP		m_mapIcon;
#endif

//=====================================================
	ITEMMAP		m_mapItemData;
	/* Gif���� */
	IMAGEMAP	m_mapAni;			// ��Դ
	DWORD		m_dwFrameSpeed;		// ֡��

	//Gif
	ANIMAP		m_arrAni[50];		// ��ˢ���ŵĶ�����Դ��λ��
	int			m_nAni;				// ˢ���ŵĶ�������
	ITEMTEXT	m_mapItemAttachText;
	
	CImageList	m_ImageList;

private:
//

	void DrawBackGround(CDC* pDC);	// Background Paint code
	void DrawItems(CDC* pDC);		// Item Paint Code
	void DrawAni(CDC *pDC);			// Gif Paint code

	//
	int KSendMessage(UINT uMsg, UINT nFlags, CPoint point);

protected:
	//{{AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	//HOOK�Խڵ�Ĳ���
private:
	afx_msg LRESULT PrivateInsertItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT PrivateDeleteItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT PrivateSetItem(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};


