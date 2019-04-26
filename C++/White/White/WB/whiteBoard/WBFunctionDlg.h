#pragma once
#include "WBPreviewDlg.h"
#include "WBBackgroundDlg.h"
#include "WBFunctionAttributeDlg.h"
#include "afxwin.h"
#include "WBFunctionBtn.h"
#include "WBTeachToolDlg.h"
//#include "../WB/whiteBoard/common/WBMedia/WBGlobalMediaCtrl.h"
#include "WBFunctionResDlg.h"

#define ID_TIMER_HIDE           WM_USER + 4000
#define ID_TIMER_SHOW           WM_USER + 4001	
#define ID_TIMER_HIDETIME       WM_USER + 4003
#define ID_TIEMR_STARTGIF       WM_USER + 4004
#define ID_TIEMR_MAGICPEN       WM_USER + 4005
#define ID_TIEMR_REFRESH        WM_USER + 4006
 
// CWBFunctionDlg �Ի���

class CWBFunctionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBFunctionDlg)

public:
	CWBFunctionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBFunctionDlg();

	// �Ի�������
	enum { IDD = IDD_SINWB_FUNCTIONDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CWBPreviewDlg     *m_wbPreviewDlg;
	CWBFunctionAttributeDlg    *m_wbAttributeDlg;  //���ԶԻ���
	CWBFunctionResDlg    *m_wbResDlg;  //��Դ�Ի���
	CWBBackgroundDlg   *m_wbBackgroundDlg; //�����Ի���
	CWBTeachToolDlg   *m_wbTeachToolDlg;  //���߶Ի���
	CRect   m_rect;    //�������ͻ�������
	CWnd* m_pParent;
	CRect   m_PreviewRect; //Ԥ���Ի���ͷ�������
	//CWBFunctionBtn m_cHideFuncdlgButton;//���ػ���ʾ������尴ť
	//CWBAttributeStatic m_cFuncdlgHideStatic;
	//CWBFunctionBtn m_btnCloseFuncPad;
	CWBFunctionBtn m_cCloseButton;
	CWBFunctionBtn m_cPreviewButton;
	CWBFunctionBtn m_cAttributeButton;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg LRESULT OnWM_SELECTWBOBJ(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_SELECTWBPEN(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_NOSELECTWBOBJ(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_OPENARRIBUTEDLG(WPARAM wParam, LPARAM lParam);
	afx_msg	LRESULT OnWM_PALETTECOLOR(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_OPENPREVIEWDLG(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_OPENRESDLG(WPARAM wParam, LPARAM lParam);
	afx_msg	LRESULT OnWM_OPENBACKGROUNDDLG(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_OPENTEACHTOOLDLG(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWM_SELECTWBERASER(WPARAM wParam, LPARAM lParam);
	//afx_msg void OnLButtonDown( UINT nFlags, CPoint point);
	afx_msg void OnBnClickedClosewbfunctiondlg();
	afx_msg void OnOK();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	CWBFunctionAttributeDlg *GetAttributeDlg();
	CWBPreviewDlg* GetPreviewDLG();
	CWBFunctionResDlg* GetResDlg(){return m_wbResDlg;}
	CWBBackgroundDlg *GetBackGroundDlg();
	CWBTeachToolDlg *GetTeachtoolDlg();
	void SetBtnState(int BtnId,WB_MOUSE_STATE MouseState);
	virtual void PostNcDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void ShowOrHideFunctionDlg(bool bIsShow); 
	
private:
	DWORD m_dLeaveTime;     //�뿪�������ϵͳ����ʱ��
	DWORD m_dLeaveTimeDiff;//ʱ���
	bool  m_bIsClickSolidBtn;//��ע��䰴ť��ֹ���
	CRect m_rectPreviewMenu; //���Ԥ���˵��ľ�������
	bool  m_bIshidewindow;   //�жϹ�������Ƿ����أ����ڽ����U7-T��ģ�����Ļ��SHOW��HIDE��


public:
	bool GetClickSolidBtn() {return m_bIsClickSolidBtn;}
	void SetClickSolidBtn(bool bl) {m_bIsClickSolidBtn = bl;}
	void DiffTimeHideFunctionDlg();
	void SetMouseLeaveTime(DWORD dw);
	void SetPreviewMenuRect(CRect rect);
	CRect m_rectShowOrHideFunction; //��ʾ�����ع������ľ���
	int  m_nShowOrHideCount;    //�������ػ�Show�Ĵ������Ա��ƶ��������
	bool GetWindowIsHide(){return m_bIshidewindow;} //��ȡ��������Ƿ�����
	void SetWindowIsHide(bool ishide){m_bIshidewindow = ishide;} //���ù�������Ƿ�����
};

