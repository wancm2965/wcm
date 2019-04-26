#pragma once
#include "afxcmn.h"

#include "ResData.h"


#define  PICTURETYPE "bmp;jpg;gif;tif;png;dib;tiff;jfif;ico;jpeg"
#define  DOCTYPE "doc;ppt;xls;pdf;docx;xlsx;pptx"
#define  MOVIETYPE "avi;wmv;mpeg;mpg"//;asf;mov;dat;rmvb;vob;mp4;f4v;mp3;wm;wma;rm;mts;mkv*
#define  MUISCTYPE ""
#define  FLASHTYPE "swf;"

class CWBFunctionResDlg;
//����ͼƬ�߳���Ϣ�ṹ��
typedef struct ResPadThumbThread{
	LPCTSTR strDirectory;
	CWBFunctionResDlg *pSinWBResDlg;
	HANDLE hThread;
	DWORD ThreadID;
public:
	struct ResPadThumbThread():
		hThread(NULL){}
}SResPadThumbThread;
//����ͼvector
typedef std::vector<CString> VThumbnail;

// CWBFunctionResDlg �Ի���
class CWBFunctionDlg;
class CWBFunctionResDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBFunctionResDlg)

public:
	CWBFunctionResDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBFunctionResDlg();

	// �Ի�������
	enum { IDD = IDD_SINWB_RESDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��


	void  InitDatas() ;             //������Դ���֡�·��
	// ��ȡ����ͼ�ľ��
	HRESULT CreateThumbnail( CString strFileName, HBITMAP* pThumbnail );
	void LoadThumb();	          //��ȡ����ͼ
	void  DrawThumbnails();		   // ���б���л�ͼ
	BOOL  GetImageFileNames();        //�ɼ�ͼƬ������ 
	//�ȵ���ǰ����ͼ������
	VThumbnail *GetVecAddress(){return &m_vecSupportFileName;}
private: 

	enum
	{
		PICTURE,
		MOVIE,
		FLASH,
		DOC,
		MUISC
	}m_nFileType;	//�ļ�����

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
	CImageList m_imageList;//��Դ�б��ͼ������

private:
	CTreeCtrl *m_pResoureTree;
	CListCtrl *m_pResoureListCtl;
	CRect m_rect;//��Դ�Ի�����ʾ����
	HTREEITEM m_hRoot;//�б��Ŀ¼
	CImageList m_ImageList;//�б�ǰ��ͼ������
	CString m_LocalPath;
	HTREEITEM m_CurrenthItemOfTreeCtl;//���ؼ���ѡ�е���
	LVITEM m_CurrenthItemOfListCtrl;//�б�ؼ���ѡ�е���

	CListCtrl   m_lstThumbnail;         //  ����ͼ��  
	CImageList  m_imlNull;              //��ͼ���������� ListCtrl �� �� ��  
	CImageList  m_ImageListThumb; // image list holding the thumbnails
	CString 	m_strImageDir;  	 //��ǰѡ�б�ǩ���ļ���·��
	//��ǰ��ǩ�аװ���֧�ֵ��ļ�
	std::vector<CString>	 m_vecSupportFileName;// vector holding the image names
	SResPadThumbThread *m_pResThread;   //���߳�ָ��	
	BOOL m_bRenderLocked;//�����������ı�ʶ	
	BOOL m_isThreadRun;//�����߳��Ƿ����ִ��
	CString  m_strFileName;
private:
	ResMapArray *m_pResMap;             //xml�ṹ����ָ��
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