#pragma once
#include "resource.h"
#include "BitmapTabCtr.h"
#include "afxwin.h"
#include "afxext.h"
class CWhiteBoardView;
// CWBResourceDlg 对话框

class CWBResourceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBResourceDlg)

public:
	CWBResourceDlg(CWhiteBoardView* pParent = NULL);   // 标准构造函数
	virtual ~CWBResourceDlg();
	CString m_strFileName;
	// 对话框数据
	enum { IDD = IDD_WBRESOURCEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持	

	CImageList				m_ImageListThumb;		// image list holding the thumbnails
	std::vector<CString>	m_VectorImageNames;		// vector holding the image names	
	CImageList				*m_ImageListTab;			//标签的背景
	CBitmapTabCtr           m_TabDir;	                            //文件夹标签
	std::vector<CString>	m_VectorDirPath;	    //各标签所对应的文件夹路径
	std::vector<CBitmapButton *> m_vecButtons;


	BOOL  GetImageFileNames();	// gather the image file names
	void  DrawThumbnails();		// draw the thumbnails in list control
	void  CheckType(int nItem);	//判断文件类型


	// Generated message map functions
	//{{AFX_MSG(CThumbnailDlg)
	virtual BOOL OnInitDialog();

	// 获取缩略图的句柄
	HRESULT CreateThumbnail( CString strFileName, HBITMAP* pThumbnail );

	void LoadThumb();	//读取缩略图
	BOOL SetTab();	//设置标签
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
	CString	m_strImageDir;	//当前选中标签的文件夹路径
	BOOL m_bAddItem;	   //判断是否是添加对象状态

	enum
	{
		PICTURE,
		MOVIE,
		FLASH,
		DOC,
		MUISC
	}m_nFileType;	//文件类型

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
