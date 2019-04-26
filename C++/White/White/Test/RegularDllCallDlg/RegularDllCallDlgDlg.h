
// RegularDllCallDlgDlg.h : ͷ�ļ�
//

#pragma once

#include "FullMenuDlg.h"

// CRegularDllCallDlgDlg �Ի���
class CRegularDllCallDlgDlg : public CDialogEx
{
// ����
public:
	CRegularDllCallDlgDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CRegularDllCallDlgDlg();
// �Ի�������
	enum { IDD = IDD_REGULARDLLCALLDLG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��



// ʵ��
protected:
	HICON m_hIcon;

	//ȫ���µĲ˵�
	CFullMenuDlg *m_FullMenu;
	// �������ܲ���    12-25���jin
	int m_UndoSetpCount;
	bool IsScreenDrawTrans();   //�õ���ǰ��Ļ��ͼ�Ի����Ƿ�͸��
	bool IsFullScreen();        //�õ���ǰ��Ļ��ͼ�Ի����Ƿ�ȫ��
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCancel();
	
	//��������
	afx_msg void OnWbNewfileb();
	afx_msg void OnWbOpenfileb();
	afx_msg void OnWbSavefileb();
	afx_msg void OnWbResavefileb();
	afx_msg void OnWbPrintfileb();
	afx_msg void OnWbClosefileb();
	afx_msg void OnWbNewpageb();
	afx_msg void OnWbForepageb();
	afx_msg void OnWbNextpageb();
	afx_msg void OnWbFirstpageb();
	afx_msg void OnWbLastpageb();
	afx_msg void OnPicturelayer();
	afx_msg void OnWbForelayerb();
	afx_msg void OnWbNextlayerb();
	afx_msg void OnWbToplayerb();
	afx_msg void OnWbBottomlayerb();
	afx_msg void OnWbLineb();
	afx_msg void OnWbSelectb();
	afx_msg void OnWbCurveb();

	afx_msg void OnWbLitterrubberb();
	afx_msg void OnWbBigrubberb();
	afx_msg void OnWbSingleobjectrubberb();
	afx_msg void OnWbCurveselectrubberb();

	afx_msg void OnWbRectangleb();
	afx_msg void OnWbRoundb();
	afx_msg void OnWbTriangleb();
	afx_msg void OnWbRhombusb();
	afx_msg void OnWbPolygonb();
	afx_msg void OnWbCharfontb();
	afx_msg void OnWbChartextb();
	afx_msg void OnWbAnnotationsb();

	afx_msg void OnWbPictureb();
	afx_msg void OnOpenAttributeDlg();
	afx_msg void OnLinewidthb1();
	afx_msg void OnWbDocumentb();
	afx_msg void OnWbCutscreenb();
	afx_msg void OnWbDeleteb();
	afx_msg void OnWbUccb();
	afx_msg void OnWbResourcecatagoryb();
	afx_msg void OnWbResourceManger();
	afx_msg void OnWbFlashb();
	
	afx_msg void OnWbFullscreenb();
	afx_msg void OnWbUndob();
	afx_msg void OnWbRedob();
	afx_msg void OnWbCloseb();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	afx_msg void OnRecscreen();
	afx_msg void OnWbRecbck();
	afx_msg void OnClose();
	afx_msg void OnWbFunctionpanelb();
	afx_msg void OnInsertpageb();
	afx_msg void OnDeletepageb();
	afx_msg void OnScreenDraw();
	afx_msg void OnDestory();
	afx_msg void OnFullscreenShift();
	afx_msg void OnCtrlDlg();
	afx_msg void OnGuiseshift();
	afx_msg void OnWbMedia();
	afx_msg void OnWbCamera();
	afx_msg void OnWbDustbin();
	afx_msg void OnSinver();
	afx_msg void OnMultiver();
};
