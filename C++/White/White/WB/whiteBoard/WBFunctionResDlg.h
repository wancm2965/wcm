#pragma once
#include "afxcmn.h"

#include "ResData.h"


#define  PICTURETYPE "bmp;jpg;gif;tif;png;dib;tiff;jfif;ico;jpeg"
#define  DOCTYPE "doc;ppt;xls;pdf;docx;xlsx;pptx"
#define  MOVIETYPE "avi;wmv;mpeg;mpg"//;asf;mov;dat;rmvb;vob;mp4;f4v;mp3;wm;wma;rm;mts;mkv*
#define  MUISCTYPE ""
#define  FLASHTYPE "swf;"

class CWBFunctionResDlg;
//加载图片线程信息结构体
typedef struct ResPadThumbThread{
	LPCTSTR strDirectory;
	CWBFunctionResDlg *pSinWBResDlg;
	HANDLE hThread;
	DWORD ThreadID;
public:
	struct ResPadThumbThread():
		hThread(NULL){}
}SResPadThumbThread;
//缩略图vector
typedef std::vector<CString> VThumbnail;

// CWBFunctionResDlg 对话框
class CWBFunctionDlg;
class CWBFunctionResDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBFunctionResDlg)

public:
	CWBFunctionResDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBFunctionResDlg();

	// 对话框数据
	enum { IDD = IDD_SINWB_RESDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


	void  InitDatas() ;             //加载资源名字、路径
	// 获取缩略图的句柄
	HRESULT CreateThumbnail( CString strFileName, HBITMAP* pThumbnail );
	void LoadThumb();	          //读取缩略图
	void  DrawThumbnails();		   // 在列表框中画图
	BOOL  GetImageFileNames();        //采集图片的名字 
	//等到当前缩略图的名字
	VThumbnail *GetVecAddress(){return &m_vecSupportFileName;}
private: 

	enum
	{
		PICTURE,
		MOVIE,
		FLASH,
		DOC,
		MUISC
	}m_nFileType;	//文件类型

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOK();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	void CreateResTree();
	void CreateResListCtl();
	void InitTreeView(HTREEITEM hParent);
	BOOL AddDrives(CString strDrive, HTREEITEM hParent);
	void AddDummyNode(HTREEITEM hItem);
	CString GetPathFromItem(HTREEITEM hItem);
	BOOL IsMediaValid(CString &strPathName);
	HTREEITEM GetDriveNode(HTREEITEM hItem);
	UINT DeleteChildren(HTREEITEM hItem);
	BOOL IsPathValid(CString &strPathName);
	UINT AddDirectoryNodes(HTREEITEM hItem, CString &strPathName);
	void SetButtonState(HTREEITEM hItem, CString &strPathName);
	BOOL HasSubdirectory(CString &strPathName);
	BOOL IsDriveNode(HTREEITEM hItem);
	CTreeCtrl *GetResTree();
	CImageList m_imageList;//资源列表的图形链表

private:
	CTreeCtrl *m_pResoureTree;
	CListCtrl *m_pResoureListCtl;
	CRect m_rect;//资源对话框显示区域
	HTREEITEM m_hRoot;//列表根目录
	CImageList m_ImageList;//列表前的图标链表
	CString m_LocalPath;
	HTREEITEM m_CurrenthItemOfTreeCtl;//树控件中选中的项
	LVITEM m_CurrenthItemOfListCtrl;//列表控件中选中的项

	CListCtrl   m_lstThumbnail;         //  缩略图表  
	CImageList  m_imlNull;              //用图像是来增加 ListCtrl 的 行 高  
	CImageList  m_ImageListThumb; // image list holding the thumbnails
	CString 	m_strImageDir;  	 //当前选中标签的文件夹路径
	//当前标签中白板所支持的文件
	std::vector<CString>	 m_vecSupportFileName;// vector holding the image names
	SResPadThumbThread *m_pResThread;   //多线程指针	
	BOOL m_bRenderLocked;//绘制锁定与否的标识	
	BOOL m_isThreadRun;//控制线程是否继续执行
	CString  m_strFileName;
private:
	ResMapArray *m_pResMap;             //xml结构数组指针
public:
	afx_msg void OnTvnSelchangedWbResouretree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpandedWbResouretree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBegindragWbResouretree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkWbResouretree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBegindragWbResdlgListctl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkWbResdlgListctl(NMHDR *pNMHDR, LRESULT *pResult);
	static UINT AfxThreadProc( LPVOID pParam );
	void LockRender(BOOL bLockRender){m_bRenderLocked = bLockRender;}
	BOOL bRenderLocked(){return m_bRenderLocked;}
	BOOL m_bIsSpread;
	CString m_SpreadStr;
};