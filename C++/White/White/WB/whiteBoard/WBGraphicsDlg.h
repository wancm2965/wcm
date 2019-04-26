#pragma once
#include "TransparentButtonEx.h"
#include "afxwin.h"

// CWBGraphicsDlg 对话框

class CWBGraphicsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBGraphicsDlg)

public:
	CWBGraphicsDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBGraphicsDlg();

// 对话框数据
	enum { IDD = IDD_WB_GRAPHICSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();

	
	virtual void OnOK();
public:
	void Relayout();
	void SetSkinImage(CString strPath);
	CString GetCaptureFromType(eType btnEmumType);
	virtual BOOL OnInitDialog();

private:
	CWnd* m_pParentDlg; //主工具条窗口
	Bitmap* m_pBitmap;
	CTransparentButtonEx *m_pParentBtn;
	CTransparentButtonEx m_rectangleBtn;
	CTransparentButtonEx m_roundBtn;
	CTransparentButtonEx m_triangleBtn;
	CTransparentButtonEx m_rhombBtn;
	CTransparentButtonEx m_polygonBtn;
	CTransparentButtonEx m_AtrTriangleBtn; //任意三角形按钮
	CTransparentButtonEx m_parallelogamBtn;//平行四边形按钮
	CTransparentButtonEx m_pentagonBtn; //五边形
	CTransparentButtonEx m_hexagonBtn; //六边形
	CTransparentButtonEx m_fivepointstartBtn;//五角星
	CTransparentButtonEx m_trapeziumBtn;//梯形按钮
	CTransparentButtonEx m_dyTriangleBtn;//等腰三角形
	CTransparentButtonEx m_zjTriangleBtn;//直角三角形
	CTransparentButtonEx m_dyzjTriangleBtn;//等腰直角三角形
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
