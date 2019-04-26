#pragma once

#include "WhiteBoardView.h"
#include "atltypes.h"
#include "WBFullDlg.h"
#include "WBToolbarCtrlDlg.h"
#include "WBToolbarDlg.h"
#include "WBRedoAndUndoDlg.h"
#include "WBPageDlg.h"
//#include "MultiWBFunctionDlg.h"
#include "WBEagelEye.h"
#include "WBAboutDlg.h"
#include "WBOptionDlg.h"


// CWhiteBoardDlg �Ի���
class CWhiteBoardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWhiteBoardDlg)
	
public:
	CWhiteBoardDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWhiteBoardDlg();
	//��ȫ���Ի���
	void setFullDlg();
	//�ر�ǰȫ���Ի���
	void closeFullDlg();

	virtual BOOL OnInitDialog();
	//�������������غ���
	//����view�Ŀ��Ӵ�С
	void MoveCWBView();
	void MoveToolbar();
	void ShowIsTouchStatic();
	void HideShowToolbar();  //��ʾ�����ع�����
	CWBFunctionDlg *GetWBFunctionDlg();
	CWBOptionDlg *CreateOptionDlg();
	void MoveWBFunctionDlg(BOOL isShow);
	void MoveOptionDlg(BOOL isShow);
	void DeleteFuncPad();
//	void ChangeToolbarPos();
	//����������Դ����
	HRESULT CreateResDlg(WPARAM wPar, LPARAM lPar);

	void OpenPic(CString strFilePath);
	void ShowAllToolbars(bool bShow = true);

	CWBFunctionDlg* GetSinFunctionDlg();//���m_sinFunctiondlgָ��
	CWBOptionDlg *GetOptionDlg();//���optiondlgָ��
// �Ի�������
	enum { IDD = IDD_WHITEBOARD_DLG };

	CWhiteBoardView* m_pWhiteboard_view;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	
	DECLARE_MESSAGE_MAP()
	



private:
	CWBFullDlg m_wbFullDlg;
	CRect m_rect;
	CRect m_WinRect;
	CString m_strCurFilePath;
	BOOL m_bSinFuncDlgShow;//����function�Ƿ���ʾ
	BOOL m_bSinOptDlgShow;//����option�Ƿ���ʾ
	//����������ر���
	CWBToolbarCtrlDlg *m_toolbarCtrlDlg;
	CWBToolbarDlg *m_toolbarDlg;
	CWBRedoAndUndoDlg * m_redoDlg;
	CWBPageDlg  *m_pPageDlg;	
	CWBOptionDlg *m_pWBOptionDlg;
	CWBFunctionDlg *m_pWBFuncDlg;
	CWBEagelEye  *m_pWBEagelEye;
	int m_nToolNum; //��������ʾ��ť����
	bool m_IsCloseScreenDraw;//ȫ��״̬�ı�ʱ�������ͼ�򿪵�����£�����view��SendLocalMessage�����йر������ͼ���ڴ˶Ի����onsize�����д�

	DWORD m_dFirstTime;  //���ڣ�������崰�ڵ�һ�δ���Ҫ�ӳ�N�룬�Ի����ļ���ҳ��Ԥ��ҳ����һ�δ���������������ʱ��
public:
	int m_winX,m_winY;
/*private:*/

	//��ȡ��Դ����Ĵ���λ��
	BOOL GetResGalleryRect(CRect &rct);
	//CWBGlobalMediaCtrl* m_pWBGlobalMediaCtrl;
	//void CreateMediaCtrl();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//HRESULT OnReceiveDragResouce(WPARAM wPar, LPARAM lPar);
	//��������ť�¼�
	virtual void OnOK();
	virtual void OnCancel();
	CWBToolbarCtrlDlg *GetToolbarCtrDlg();
	CWBToolbarDlg * GetToolbarDlg();
	CWBRedoAndUndoDlg *GetRedoAndUndoDlg();
	CWBPageDlg *GetPageDlg();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CString GetCurFilePath();
	/*CWBGlobalMediaCtrl* GetGlobalMediaCtrl();*/
	CWBEagelEye* GetWBEagleEyeDlg(); //���ӥ�۶Ի����ָ��
	BOOL SetAndShowEagleEye(CRect rect);//��ʾ����ӥ�۵�λ��
	void CreateAboutDlg();
	void SetIsOpenScreenDraw(bool IsCloseScreenDraw);


	void SetSinFuncShow(BOOL show);
	BOOL GetSinFuncShow();
	void SetSinOptShow(BOOL show);
	BOOL GetSinOptShow();
	
	void SetViewPos(CRect rc);//����view��λ��
	CWBFunctionDlg * GetFuntDlg();
	void DeleteWBFunctionDlg();
};
