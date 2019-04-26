#pragma once
#include "WhiteBoardView.h"
#include "atltypes.h"
#include "afxwin.h"
#include "MTGestureImport.h"
#include "WBMTDrawObjectTally.h"


// CScreenDrawDlg �Ի���
class CScreenDrawDlg : public CDialog, public CWBObject
{
	DECLARE_DYNAMIC(CScreenDrawDlg)

public:
	CScreenDrawDlg(CWnd* pParent);   // ��׼���캯��
	CScreenDrawDlg(CRect& rcScreenDlg);
	virtual ~CScreenDrawDlg();
	virtual void OnFinalRelease();
	// �Ի�������
	enum { IDD = IDD_SCREENDRAWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
public:
	void ScreenClearTouchMap(DWORD dwLayerIndex);			///���m_mapMTDrawPar������
	void SetMoveIsNeedSendData(BOOL MoveIsNeedSendData);    ///������move��ʱ����Ҫˢ��
	BOOL GetMoveIsNeedSendData();                           ///��ȡ��move��ʱ����Ҫˢ�µ��ж�
	CRect& GetScreenDrawDlgRect(){return m_rect;}			///��ȡ����
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	bool m_LButtonDown;           //�ж��������Ƿ��°���Ϊtrue���ɿ�Ϊfalse
	afx_msg void OnDestroy(); //ȡ��ȫ��ʱ����ʽ���õĺ���
	afx_msg LRESULT OnDestroyScreen(WPARAM wParam, LPARAM lParam); //�ر���Ļ��ͼʱ����ʾ���õ����ٴ��ں���
public:
	///add by aolingwen 2012.11.23
	///�̳���AVCON_SDK_MTTouch::CWBMTObject
	//��������
	virtual void touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te);
	//�����ƶ�
	virtual void touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te);
	//��������
	virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);
	///end

	//���ڴ�����ͼ��������add by huanglongquan 2012��12��18�� 
	CWBMTDrawObjectTally m_MTDrawObjectTally;
private:
	CPoint m_ptPointLineCurve;//������갴��ʱ��ֱ��q���ߵĵ�����
	///���ھ���
	CRect m_rect;
	///��ͼ��
	XCritSec m_csDraw;
	//����������
	DWORD m_dwTimeGetTime; //��¼��һ������ƶ�ʱ����ͬ����Ϣ��ʱ��
	BOOL  m_bMoveIsNeedSendData;//�ж���MouseMoveʱ�Ƿ���Ҫ����ͬ����Ϣ
	
private:
	//����ϵͳ�ģ������κδ���
	afx_msg LRESULT OnGesture(WPARAM /*wParam*/, LPARAM lParam);
	afx_msg LRESULT OnTouch(WPARAM wParam, LPARAM lParam);

	//������wm_touch��Ϣ
	std::map<DWORD, MTCursor*>	m_mapTouchCursor;
	//touch
private:
	//�������Ʋ����ṹ
	typedef struct touchDrawingParameter
	{
	private:
		DWORD dwID;//�����ݿ��Բ���
		int nDrawType;
		CWBObject *pObject;
		DWORD dwTime;
		CPoint ptValid;
		CPoint pt;
		void *pData;
		
	public:
		touchDrawingParameter(DWORD _dwID, int _nDrawType, CWBObject*_pObject, CPoint _ptValid=CPoint(0,0), DWORD _dwTime=0, CPoint _pt=CPoint(0,0))
			: dwID(_dwID), pObject(_pObject), dwTime(_dwTime), ptValid(_ptValid), nDrawType(_nDrawType), pt(_pt){}
		void UpdateValidPt(CPoint _ptValid){ptValid=_ptValid;};
		void UpdatePt(CPoint _pt){pt=_pt;};
		void UpdateTime(DWORD _dwtime){dwTime=_dwtime;};
		void SetAppendData(void *_pData){pData=_pData;};
		int GetType(){return nDrawType;};
		CWBObject *GetWBObject(){return pObject;};
		DWORD GetTime(){return dwTime;};
		CPoint GetPt(){return pt;};
		CPoint GetValidPt(){return ptValid;};
		void *GetAppendData(){return pData;};
	}MTDrawPar, *PMTDrawPar;
	std::map<DWORD, MTDrawPar*> m_mapMTDrawPar;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};