#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"
#include "resource.h"
#include "WBColorPatetteBtn.h"
#define BTN_CLOSECOLORPALETTEDLG_ID WM_USER + 100

// CWBColorPaletteDlg �Ի���
//class CWMBtn;
class CWBColorPaletteDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBColorPaletteDlg)

public:
	CWBColorPaletteDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBColorPaletteDlg();

	// �Ի�������
	enum { IDD = IDD_WB_COLORPALETTEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CWnd* m_pParentDlg;
	CTransparentButtonEx *m_pParentBtn;
	Bitmap* m_pBitmap;
	COLORREF m_changecolor; //���������ε���ɫ
	COLORREF m_color;//�洢ɫ����Ϣ
	CWBColorPatetteBtn m_autoColorBtn;
	CWBColorPatetteBtn m_oneColorBtn;
	CWBColorPatetteBtn m_twoColorBtn;
	CWBColorPatetteBtn m_threeColorBtn;
	CWBColorPatetteBtn m_fourColorBtn;
	CWBColorPatetteBtn m_fiveColorBtn;
	CWBColorPatetteBtn m_sixColorBtn;
	CWBColorPatetteBtn m_sevenColorBtn;
	CWBColorPatetteBtn m_eightColorBtn;
	CWBColorPatetteBtn m_nineColorBtn;
	CWBColorPatetteBtn m_otherColorBtn;
	//CWMBtn* m_pPaletteDlgClose; //�رհ�ť
	virtual void OnCancel();
	virtual void OnOK();
	UINT m_nParentBtnID;
	eType m_ParentBtnType;
public:
	virtual BOOL OnInitDialog();
	void Relayout();
	void SetSkinImage(CString strPath);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedOnecolorBtn();
	afx_msg void OnBnClickedTwocolorBtn();
	afx_msg void OnBnClickedThreecolorBtn();
	afx_msg void OnBnClickedForcolorBtn();
	afx_msg void OnBnClickedFivecolorBtn();
	afx_msg void OnBnClickedSixcolorBtn();
	afx_msg void OnBnClickedSevencolorBtn();
	afx_msg void OnBnClickedEightcolorBtn();
	afx_msg void OnBnClickedNinecolorBtn();
	afx_msg void OnBnClickedOthercolorBtn();
	afx_msg void OnBnClickedAutocolorBtn();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//CWMBtn *GetColorPalleteDlgCloseBtn(void);
	void ColorFunction();//������
	COLORREF GetColor();
	BOOL bInTriangle(CPoint point);
	//CWMBtn *GetColorPalleteCloseBtn();//��ùرհ�ť
};
