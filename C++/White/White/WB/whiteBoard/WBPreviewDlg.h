#pragma once
#include "WBPreviewView.h"
#include "WBFunctionBtn.h"

#define VIEWWIDTH   150
#define VIEWRATIO   ((float)g_nWidth / (float)VIEWWIDTH) //Ԥ������ 
#define VIEWHIGHT   ((float)g_nHeight/VIEWRATIO)  //Ԥ����ĸ߶�


// CWBRreviewDlg �Ի���
class CWBFunctionDlg;
class CWBPreviewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBPreviewDlg)
public:
	CWBPreviewDlg(CWnd* pParent = NULL);   // ��׼���캯��
	CWBPreviewDlg(CRect rect,CWnd* pParent = NULL);
	virtual ~CWBPreviewDlg();
    bool ClearVistList();
	bool CreateViewList();
	// �Ի�������
	enum { IDD = IDD_WB_PREVIEWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
private:
	UINT m_pageCount;
	UINT m_uOldCourrtPageIndex; //��һ�εĵ�ǰҳ
	CObList *m_pViewList;        //���Ԥ����view���б�
	CWBPreviewView * m_pPreviewView;
	CWBPreviewView* m_LastPreView ;
	CRect m_viewPosRt,m_rect;
	CRect m_RectTempPriview;
	CWBPage *m_pPage;
	BOOL m_bLineDown,m_bLineUp;  //����������ͷʱ���ж�
	SCROLLINFO m_si;             //��������Ϣ
	UINT m_ViewListWidht;//view�б�Ŀ��

	CRect m_NewButtonRect; //�²����İ�ť���ɵ�λ��
	HCURSOR m_hMyCurosr; //�������
	bool m_bReduceListWidth; //�Ƿ���СԤ���Ի���Ŀ��
//************ҳ����*******************************
	int m_ExchangePageA;//����ҳA��ҳ��
	int m_ExchangePageB;
	CRect m_ExchangePageARect;//����ҳB��rect
	CRect m_ExchangePageBRect;
    CWBPreviewView *m_pPreViewA;
	CWBPreviewView *m_pPreViewB;
	bool m_bAlreadyOnInit; //�Ƿ��Ѿ���ʼ��Ԥ����

	
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
	void SetPreviewRect(CRect rect);//��ȡԤ����ľ���
	CWBPreviewView *GetIndexPreviewPage(UINT CurPageIndex); //��õ�ǰҳ��ָ��
	void ShowButtonPlace(void);
	void SetCourrtPrvRatio();//����Ԥ����ǰҳ�ı���
	CWBPreviewView* GetCurPreView();
	void GetLastPreview(void);

private:
	//	CButton m_buttonRightPreviw;
	CBitmap NewPageBmp; //�½�ҳ
	CBitmap PrePageBmp; //��һҳ
	CBitmap NextPageBmp; //��һҳ
	CBitmap FirstPageBmp; //��һҳ
	CBitmap LastPageBmp; //ĩҳ
	CBitmap INSERTPageBmp; //����ҳ
	CBitmap DELETEPageBmp; //ɾ��ҳ
	CWBFunctionBtn m_BtnRightDown;
public:

//*******�����˵�******************
	afx_msg void OnWbNewpageb();
	afx_msg void OnWbForepageb();
	afx_msg void OnNextpage();
	afx_msg void OnFirstpage();
	afx_msg void OnWbLastpageb();
	afx_msg void OnInsertpageb();
	afx_msg void OnDelpage();
//*********ҳ����*******************	
	UINT InsertPreview(void);
	void InterchangPage(void);
	void InterchangView(UINT nFormerPage,UINT nTargetPage);
	void InterchangView(void);
	BOOL MovePage(void);
	void ExchangePageB(CWBPreviewView* ExchangeviewB, int ExchangeB);
	void ExchangePageA(CWBPreviewView* ExchangeviewA, int ExchangeA);
	
		
///*********ˢ��Ԥ��*****************
	bool bLoadPreviews();
	void RflashViewList();
    BOOL OpenPreviewRefubish(void);// �ڱ����д򿪹��ܶԻ�������
	BOOL RefurbishPreview(void);
	BOOL RefurbishInserPreview();
	void SetViewListWidth();
	UINT GetViewListWidth();
	bool GetNeedRedueListWidth();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void ClearAllView();
	bool GetIsNeedRefessh();//Ϊ����������˻�ʹ��
	void SetIsNeedRefesh(bool isneed);
protected:
	afx_msg LRESULT OnWbFunction_AddPreview(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);//���ڽ�ȡԤ���Ϲ��������Ҽ������˵���Ϣ
	CWBPreviewView* GetPreviewView();
private:
	bool m_bNeedRefesh; //Ϊ����������˻�ʹ��
};
