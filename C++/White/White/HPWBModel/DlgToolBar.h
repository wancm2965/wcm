#pragma once
#include "TransparentButtonEx.h"
#include "afxwin.h"
#include <GdiPlus.h>
#include "DlgFuck.h"
#include "ResourceDef.h"
#include "Resource.h"
#include "DlgResource.h"
#include "DlgRubber.h"
#include "DlgGraphics.h"
#include "DlgFile.h"
#include "DlgColor.h"
#include "DlgPage.h"
#include "DLGDesktopDraw.h"
using namespace Gdiplus;
// CDlgToolBar �Ի���
#define TIMER_ID_TOOL 2003
#define WIDTH_BTN 54
#define HEIGHT_BTN 54
#define WIDTH 70
#define HEIGHT 589
#define SPACE 2
#define WIDTH_HEAD 35
#define WM_FLASH WM_USER + 0x0001
class CDlgToolBar : public CDialog
{
	DECLARE_DYNAMIC(CDlgToolBar)

public:
	CDlgToolBar(CWnd* pParent = NULL);   //��׼���캯��
	virtual ~CDlgToolBar();

// �Ի�������
	enum { IDD = IDD_DLG_TOOLBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    //DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
    void FlashBackground();//���ع���������
    void ProcessCmd(eType cmdType,bool bFromBox = false);
	void SetWBMain(CWnd* pWnd);
    void OpenToolBox();
	void ReLayout();
	void ChangButton(CTransparentButtonEx *pDownBtn,int downY,CTransparentButtonEx * pUpBtn,int upY); //���������ڲ���ť��λ��
    void DelAndNewBtn(CTransparentButtonEx *pBtn,int btnY,eType btnEmumType,LPCTSTR lpNewCaption); //ɾ�������ɰ�ť
	CString GetCaptureFromType(eType btnEmumType);
	void CloseAllChildToolBox();
		//�����еĺ���
    void OpenResource();//����Դ�Ի���
	void setResourceDlgNULL();// ������Դ�Ի���ָ��Ϊ��
	void OpenRubber();//�򿪲��ӶԻ���
    void setRubberDlgNULL();// ���ò��ӶԻ���ָ��Ϊ��
    void OpenGraphics();//��ͼ�ζԻ���
    void setGraphicsDlgNULL();// ����ͼ�ζԻ���ָ��Ϊ��
    void OpenFile();//���ļ��Ի���
    void setFileDlgNULL();// �����ļ��Ի���ָ��Ϊ��
	void OpenColor();
	void setColorDlgNULL();
    void OpenPage();
	void setPageDlgNULL();
    void OpenDesktopDraw();
	void setDesktopDrawDlgNULL();



	
private:
	int x; //��ť�ڹ�������X���λ��

	CRect m_foreWindowRt; //��¼��������һ�ε�λ��
	Bitmap* m_pBitmap; //����������λͼ
	CDlgFuck* m_pToolBox; //���öԻ���ָ��
	//�����еı���
    CDlgResource *m_pResourceDlg;//��Դ�Ի���ָ��
    CDlgRubber * m_pRubber;//���ӶԻ���ָ��
    CDlgGraphics * m_pGraphics;//ͼ�ζԻ���ָ��
	CDlgFile * m_pFileDlg; //�ļ��Ի���ָ��
	CDlgColor *m_pColor;
    CDlgPage  *m_pPage;
	CDLGDesktopDraw *m_pDesktopDraw;
    
    CToolTipCtrl m_ToolTip;//������ʾ�ؼ�
	CWnd* m_pWbWnd;
	bool m_bFullScreen;

	CTransparentButtonEx *m_btnOne;
	CTransparentButtonEx *m_btnTwo;
	CTransparentButtonEx *m_btnThree;
	CTransparentButtonEx *m_btnFour;
	CTransparentButtonEx *m_btnFive;
	CTransparentButtonEx *m_btnSix;
	CTransparentButtonEx *m_btnSeven;
	CTransparentButtonEx *m_btnEight;
	CTransparentButtonEx *m_btnNine;
	CTransparentButtonEx *m_btnTen;
public:
    static HWND g_toolBarHWND;
protected:
	virtual void OnOK();
	virtual void OnCancel();

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtn1();
	afx_msg void OnBnClickedBtn2();
	afx_msg void OnBnClickedBtn3();
	afx_msg void OnBnClickedBtn4();
	afx_msg void OnBnClickedBtn5();
	afx_msg void OnBnClickedBtn6();
	afx_msg void OnBnClickedBtn7();
	afx_msg void OnBnClickedBtn8();
	afx_msg void OnBnClickedBtn9();
	afx_msg void OnBnClickedBtn10();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnCloseToolbarMessage(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
