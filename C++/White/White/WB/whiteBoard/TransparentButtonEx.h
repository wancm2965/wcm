#pragma once
#include <GdiPlus.h>
#include "common/ResourceDef.h"
using namespace Gdiplus;
#include <vector>
#include "WBToolTip.h"
#include "WBAboutDlg.h"


// CTransparentButtonEx

class CTransparentButtonEx : public CButton
{
	DECLARE_DYNAMIC(CTransparentButtonEx)

public:
	CTransparentButtonEx();
	virtual ~CTransparentButtonEx();

protected:
	DECLARE_MESSAGE_MAP()
protected:

	
	//m_pImg��ť��ͼƬ����Ϊ��͸��ͼ����m_pImgBG�Ƕ�������˱���ͼ
	Bitmap			 *m_pImg,*m_pImgBG;
	//��ťͼƬ���ļ�·��
	CString			 m_strPath;
	//����ť������
	eType            m_btnType;
	//��ť���ڵĸ�����ָ��
	CWnd             *m_pParentWnd;
	//��ť���µ���������
	CPoint           m_downPoint;
	//��ť�������������
	CPoint           m_upPoint;
	//��ť��ID��
	UINT             m_btnID;
	//��ť�ı����ı�
	CString			 m_strName;
	//
	BOOL            m_bMouseIn;
	DWORD            m_beginTime,m_endTime;//�����Ŀ�ʼ����ʱ��
	int				 m_iTimerID; //�ж�ʱ������ʱ��
    XCritSec			m_ButtonUpWriteLock;	//��ť����ʱд��ťλ����Ϣ����
	BOOL          m_bBTNStatus;
	
public:
	static MAP_PATH		 m_mapPath;
	static eType g_GraphicsbuttonType;  //��¼ͼ�ε�ǰѡ��İ�ť���ͣ�eType��
	static eType g_LineTypebuttonType;  //��¼���͵�ǰѡ��İ�ť���ͣ�eType��
	static eType g_RubberbuttonType;  //��¼���ӵ�ǰѡ��İ�ť���ͣ�eType��

	static void Init();
    void SetParentWnd(CWnd* pWnd);
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    void SetButtonImage(CString filename);//���ð�ťͼƬ

    void SetType(eType enumType,bool isboxbtn = false);//��ð�ťͼƬ·��
	eType GetType();

	void SetName(CString strName);
	CString GetName();
	void SetButtonStatus(BOOL bStatus, eType ntype = TYPE_NONE);
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT  OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnMouseHover(WPARAM wParam, LPARAM lParam);

	afx_msg void OnTimer(UINT nIDEvent);


public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};


