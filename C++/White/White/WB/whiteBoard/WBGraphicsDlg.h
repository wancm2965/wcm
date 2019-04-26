#pragma once
#include "TransparentButtonEx.h"
#include "afxwin.h"

// CWBGraphicsDlg �Ի���

class CWBGraphicsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBGraphicsDlg)

public:
	CWBGraphicsDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBGraphicsDlg();

// �Ի�������
	enum { IDD = IDD_WB_GRAPHICSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();

	
	virtual void OnOK();
public:
	void Relayout();
	void SetSkinImage(CString strPath);
	CString GetCaptureFromType(eType btnEmumType);
	virtual BOOL OnInitDialog();

private:
	CWnd* m_pParentDlg; //������������
	Bitmap* m_pBitmap;
	CTransparentButtonEx *m_pParentBtn;
	CTransparentButtonEx m_rectangleBtn;
	CTransparentButtonEx m_roundBtn;
	CTransparentButtonEx m_triangleBtn;
	CTransparentButtonEx m_rhombBtn;
	CTransparentButtonEx m_polygonBtn;
	CTransparentButtonEx m_AtrTriangleBtn; //���������ΰ�ť
	CTransparentButtonEx m_parallelogamBtn;//ƽ���ı��ΰ�ť
	CTransparentButtonEx m_pentagonBtn; //�����
	CTransparentButtonEx m_hexagonBtn; //������
	CTransparentButtonEx m_fivepointstartBtn;//�����
	CTransparentButtonEx m_trapeziumBtn;//���ΰ�ť
	CTransparentButtonEx m_dyTriangleBtn;//����������
	CTransparentButtonEx m_zjTriangleBtn;//ֱ��������
	CTransparentButtonEx m_dyzjTriangleBtn;//����ֱ��������
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClickedPolygonGrphicsbtn();
	afx_msg void OnClickedRectangleGrphicsbtn();
	afx_msg void OnClickedRhombGrphicsbtn();
	afx_msg void OnClickedRoundGrphicsbtn();
	afx_msg void OnClickedTriangleGrphicsbtn();
	afx_msg void OnClickedAtrTrianglebtn();
	afx_msg void OnClickedDYTrianglebtn();
	afx_msg void OnClickedDYZJTrianglebtn();
	afx_msg void OnClickedZJTrianglebtn();
	afx_msg void OnClickedParallelogambtn();
	afx_msg void OnClickedPentagonGrphicsbtn();
	afx_msg void OnClickedHexagonGrphicsbtn();
	afx_msg void OnClickedFivepointstartGrphicsbtn();
	afx_msg void OnClickedTrapeziumGrphicsbtn();
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
