#pragma once
#include "resource.h"
#include "BitmapTabCtr.h"
#include "afxwin.h"
#include "afxext.h"
class CWhiteBoardView;
// CWBResourceDlg �Ի���

class CWBResourceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBResourceDlg)

public:
	CWBResourceDlg(CWhiteBoardView* pParent = NULL);   // ��׼���캯��
	virtual ~CWBResourceDlg();
	CString m_strFileName;
	// �Ի�������
	enum { IDD = IDD_WBRESOURCEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��	

	CImageList				m_ImageListThumb;		// image list holding the thumbnails
	std::vector<CString>	m_VectorImageNames;		// vector holding the image names	
	CImageList				*m_ImageListTab;			//��ǩ�ı���
	CBitmapTabCtr           m_TabDir;	                            //�ļ��б�ǩ
	std::vector<CString>	m_VectorDirPath;	    //����ǩ����Ӧ���ļ���·��
	std::vector<CBitmapButton *> m_vecButtons;


	BOOL  GetImageFileNames();	// gather the image file names
	void  DrawThumbnails();		// draw the thumbnails in list control
	void  CheckType(int nItem);	//�ж��ļ�����


	// Generated message map functions
	//{{AFX_MSG(CThumbnailDlg)
	virtual BOOL OnInitDialog();

	// ��ȡ����ͼ�ľ��
	HRESULT CreateThumbnail( CString strFileName, HBITMAP* pThumbnail );

	void LoadThumb();	//��ȡ����ͼ
	BOOL SetTab();	//���ñ�ǩ
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchangeResourcetab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListResource(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAddresource();
	afx_msg void OnBnClickedDelresource();
	afx_msg void OnBnClickedSetdefault();
	afx_msg void OnLvnBegindragListResource(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListResource(NMHDR *pNMHDR, LRESULT *pResult);
private:
	CWhiteBoardView *m_pWhiteBoardView;		
	CListCtrl	m_ListThumbnail;	
	CString	m_strImageDir;	//��ǰѡ�б�ǩ���ļ���·��
	BOOL m_bAddItem;	   //�ж��Ƿ�����Ӷ���״̬

	enum
	{
		PICTURE,
		MOVIE,
		FLASH,
		DOC,
		MUISC
	}m_nFileType;	//�ļ�����

public:
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedHideresource();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void  CreateButton();
	void  DestroyButtons();
};
