#pragma once
#include "WBPreviewView.h"
#include "WBFunctionBtn.h"

#define VIEWWIDTH   150
#define VIEWRATIO   ((float)g_nWidth / (float)VIEWWIDTH) //预览比例 
#define VIEWHIGHT   ((float)g_nHeight/VIEWRATIO)  //预览框的高度


// CWBRreviewDlg 对话框
class CWBFunctionDlg;
class CWBPreviewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBPreviewDlg)
public:
	CWBPreviewDlg(CWnd* pParent = NULL);   // 标准构造函数
	CWBPreviewDlg(CRect rect,CWnd* pParent = NULL);
	virtual ~CWBPreviewDlg();
    bool ClearVistList();
	bool CreateViewList();
	// 对话框数据
	enum { IDD = IDD_WB_PREVIEWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
private:
	UINT m_pageCount;
	UINT m_uOldCourrtPageIndex; //上一次的当前页
	CObList *m_pViewList;        //存放预览中view的列表
	CWBPreviewView * m_pPreviewView;
	CWBPreviewView* m_LastPreView ;
	CRect m_viewPosRt,m_rect;
	CRect m_RectTempPriview;
	CWBPage *m_pPage;
	BOOL m_bLineDown,m_bLineUp;  //滚动条在两头时的判断
	SCROLLINFO m_si;             //滚动条信息
	UINT m_ViewListWidht;//view列表的宽度

	CRect m_NewButtonRect; //新产生的按钮生成的位置
	HCURSOR m_hMyCurosr; //鼠标光标句柄
	bool m_bReduceListWidth; //是否缩小预览对话框的宽度
//************页交换*******************************
	int m_ExchangePageA;//交换页A的页号
	int m_ExchangePageB;
	CRect m_ExchangePageARect;//交换页B的rect
	CRect m_ExchangePageBRect;
    CWBPreviewView *m_pPreViewA;
	CWBPreviewView *m_pPreViewB;
	bool m_bAlreadyOnInit; //是否已经初始化预览框

	
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT  OnNewViewSynchronizer(WPARAM wParam, LPARAM lParam);
	afx_msg void  OnPreviewBtnClicked();
public:

	void SetLastPreview(void);
	void SetBtnState(int BtnId,WB_MOUSE_STATE MouseState);
	void SetPreviewRect(CRect rect);//获取预览框的矩形
	CWBPreviewView *GetIndexPreviewPage(UINT CurPageIndex); //获得当前页的指针
	void ShowButtonPlace(void);
	void SetCourrtPrvRatio();//设置预览当前页的比例
	CWBPreviewView* GetCurPreView();
	void GetLastPreview(void);

private:
	//	CButton m_buttonRightPreviw;
	CBitmap NewPageBmp; //新建页
	CBitmap PrePageBmp; //上一页
	CBitmap NextPageBmp; //下一页
	CBitmap FirstPageBmp; //第一页
	CBitmap LastPageBmp; //末页
	CBitmap INSERTPageBmp; //插入页
	CBitmap DELETEPageBmp; //删除页
	CWBFunctionBtn m_BtnRightDown;
public:

//*******弹出菜单******************
	afx_msg void OnWbNewpageb();
	afx_msg void OnWbForepageb();
	afx_msg void OnNextpage();
	afx_msg void OnFirstpage();
	afx_msg void OnWbLastpageb();
	afx_msg void OnInsertpageb();
	afx_msg void OnDelpage();
//*********页操作*******************	
	UINT InsertPreview(void);
	void InterchangPage(void);
	void InterchangView(UINT nFormerPage,UINT nTargetPage);
	void InterchangView(void);
	BOOL MovePage(void);
	void ExchangePageB(CWBPreviewView* ExchangeviewB, int ExchangeB);
	void ExchangePageA(CWBPreviewView* ExchangeviewA, int ExchangeA);
	
		
///*********刷新预览*****************
	bool bLoadPreviews();
	void RflashViewList();
    BOOL OpenPreviewRefubish(void);// 在保存中打开功能对话框涮新
	BOOL RefurbishPreview(void);
	BOOL RefurbishInserPreview();
	void SetViewListWidth();
	UINT GetViewListWidth();
	bool GetNeedRedueListWidth();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void ClearAllView();
	bool GetIsNeedRefessh();//为单机版进会退会使用
	void SetIsNeedRefesh(bool isneed);
protected:
	afx_msg LRESULT OnWbFunction_AddPreview(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);//用于截取预览上滚动条的右键弹出菜单消息
	CWBPreviewView* GetPreviewView();
private:
	bool m_bNeedRefesh; //为单机版进会退会使用
};
