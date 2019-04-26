#pragma once
#include "afxbutton.h"
#include "afxcmn.h"
#include "afxcolorbutton.h"
#include "afxwin.h"
#include "WBAttributeStatic.h"
#include "WBFunctionBtn.h"
#include "WBImageSliderCtrl.h"
#include "WBEdit.h"
#include "TransparentButtonEx.h"
#include "WBColorPaletteDlg.h"
#include "WBPromptFontSetDlg.h"

//wangxiaobin���ԶԻ��������Խṹ��洢��������
typedef struct _WBFunctionAttribute
{
	_WBFunctionAttribute();
	_WBFunctionAttribute(const _WBFunctionAttribute &pItem);//��������

	int LineColorStatic;
	int LineColorButton;
	int LineWidthStatic;
	int LineWidthEdit;
	int LineWidthCombx;
	int LineTypeStatic;
	int LineTypeCombx;
	int SolidColorStatic;
	int SolidCheckBtn;
	int SolidCheckStatic;
	int SolidColorButton;
	int ArrowStatic;
	int ArrowCombx;
	int cTextStatic;
	int TextEditBox;
	int FontStatic;
	int FontFaceComBox;
	int FontSizeComBox;
	int FontBoldBtn;
	int FontItalicBtn;
	int FontUnderlineBtn;
	int VerticalBtn;
	int BrushSizeComBox;
	int BrushSizeStatic;

    /*��������*/
	int IcoStyleStatic; 
	int IcoStyleComBox;
}WBFunctionAttribute;
// CWBFunctionAttributeDlg �Ի���
class CWBFunctionDlg;
class CWBFunctionAttributeDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBFunctionAttributeDlg)

public:
	CWBFunctionAttributeDlg(
		int nWidth = 1,
		COLORREF clr = RGB(0, 0, 0), 
		BOOL nBIsSolid = FALSE,
		int nLineType = PS_SOLID, 
		int nArrow = 0,
		int nTranspar = 0,
		CWnd* pParent = NULL,
		CString nFontFace = _T("����"),
		int nFontSize = 24);   // ��׼���캯��
	virtual ~CWBFunctionAttributeDlg();
	virtual BOOL OnInitDialog();
	virtual void PreInitDialog();
	virtual void OnCancel();
	void SetComboxItem(HICON *iconArray, CImageList *imList,int iconCount,UINT cionResForstId,CComboBoxEx *comboxData );

	void SetComboxItem();//����combox�� ���ļ�����bitmap
	BOOL SaveBitmap(CDC *pDC, CRect rect, const char *filename); //������ת��Ϊbitmap����

	void InsertStr(CComboBoxEx *comboxData,CString str);
	void SetLineType(int PenLineType);
	void SetLineCap(LineCap nStartLineCap, LineCap nEndLineCap);
	void SetLineWidth(int PenLineWidth);
	void SetIcoStyle(CString strIcoStyle);
	void SetSliderCtrlPos();//���û�����λ��
	//void SetTransparEditValue();//��ʼ͸������ʾֵ

	CWBFunctionBtn *GetFontBoldBtn();
	CWBFunctionBtn *GetFontItalicBtn();
	CWBFunctionBtn *GetFontUnderlineBtn();
	CWBFunctionBtn *GetFontVerticalBtn();

	/*	void UpdateAttributeDlg();*/

	//int m_nCurWidth; 
	COLORREF m_clrLine ; //����ɫ
	int m_nLineType;  //����
	int m_nCurArrow ; //�Ƿ��м�ͷ
	CString m_nLineWidth; //�߿�
	BOOL m_bIsSolid; //�Ƿ����
	COLORREF m_clrSolid; //�����ɫ
	LineCap m_nStartLineCap; //�����ñ
	LineCap m_nEndLineCap; //�յ���ñ
	CString m_nTranspar; //͸����
	CString m_nFontFace;//��������
	CString m_strFontSize;//�ֺŴ�С
	CString m_strBrushSize; //���Ӵ�С
	int m_nPreFontSize;    //���벻�Ϸ���ʾ�������С
	/*	int m_nPosition;//����λ��*/

	// �Ի�������
	enum { IDD = IDD_WB_ATTRIBUTEDLG};
private:
	HICON m_LineWidthicon[7];
	CImageList m_LineWidthImageList;

	HICON m_IcoStyleicon[7]; //Icoͼ��������
	CImageList m_IcoStyleImageList; //Icoͼ��Image�б�

	HICON m_LineTypeicon[5];
	CImageList m_LineTypeImageList;
	HICON m_Arrowicon[10];
	CImageList m_ArrowImageList;
	HICON m_FontSizeicon[17];
	CImageList m_FontSizeImageList;
	HICON m_FontFaceicon[46];
	CImageList m_FontFaceImageList;
	//CFont m_Font;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
public:
	CTransparentButtonEx *m_LineColorButton;//��ɫ��ť
	CWBColorPaletteDlg *m_pAttriButeColorPatetteDlg;//��ɫ��ɫ��
	CWBColorPaletteDlg *m_pAttriButeSolidPatetteDlg;//���ɫ��ɫ��
	CWBPromptFontSetDlg *m_pAttriButePromptFontSetDlg;//������������
	CComboBoxEx m_ClrSolidCombx;
	CComboBoxEx m_LineWidthCombx;
	CComboBoxEx m_LineTypeCombx;
	CComboBoxEx m_BrushSizeCombx;
	CComboBoxEx m_ArrowCombx;
	CTransparentButtonEx *m_SolidColorButton;
	CWBEdit m_TransparEdit;
	CWBEdit m_LineWidthEdit;
	CWBImageSliderCtrl m_TransparSlider;
	CWBAttributeStatic m_SolidColorStatic;
	CWBAttributeStatic m_LineColorStatic;
	CWBAttributeStatic m_LineWidthStatic;

	CWBAttributeStatic m_IcoStyleStatic; //����ͼ����ʽStatic

	CWBAttributeStatic m_BrushSizeStatic;
	CWBAttributeStatic m_LineTypeStatic;
	CWBAttributeStatic m_ArrowStatic;
	CWBAttributeStatic m_TransparStatic;
	CWBAttributeStatic m_PerStatic;
	CWBAttributeStatic m_SolidCheckStatic;
	CWBFunctionBtn m_SolidCheckBtn;

	CWBAttributeStatic m_FontStatic;
	CComboBoxEx m_FontFaceComBox;
	CComboBoxEx m_FontSizeComBox;
	CWBFunctionBtn m_FontBoldBtn;
	CWBFunctionBtn m_FontItalicBtn;
	CWBFunctionBtn m_FontUnderlineBtn;
	CWBFunctionBtn m_VerticalBtn;

	CWBAttributeStatic m_cTextStatic;	
	CWBEdit m_TextEditBox;// �ı��༭��
	CString m_Textstr; //�ı�����
	CWBAttributeStatic m_cAttributeTransperStatic;
	CWBFunctionBtn m_cReduceTransperBtn;
	CWBFunctionBtn m_cAddTransparBtn;
	//CWBFunctionBtn m_DefaultBackGroundBtn;
	/*	CEdit m_LineWidthValue;*/

	//�ĵ����ƴ������� luowei 2012-05-03
	CWBAttributeStatic m_cAttMTOperate;
	CWBFunctionBtn m_btnMTRotate;
	CWBFunctionBtn m_btnMTZoom;
	int m_nDrawPattern ;
	afx_msg void OnEnChangeLinewidthedit();             //�༭�߿�
	afx_msg void OnCbnSelchangeLinewidthcomboboxex();  //ѡ���߿�
	afx_msg void OnBnClickedCheckSolid();				//ѡ�����
	afx_msg void OnClickedCbtnLinecolor();              //ѡ���ߵ���ɫ
	afx_msg void OnCbnSelchangeLinetypecomboboxex();	//ѡ������
	afx_msg void OnCbnSelchangeArrowcomboboxex();		//ѡ���ߵļ�ͷ
	afx_msg void OnBnClickedCbtnSolidcolor();			//ѡ�����ɫ
	afx_msg void OnBnClickedReducetranspar();			//����͸����
	afx_msg void OnBnClickedAddtranspar();				//����͸����
	afx_msg void OnEnChangeTRANSPAREDIT();				//�༭�ı�͸����
	afx_msg void OnOK();
	
	//afx_msg void OnCbnSelchangeWbFontfacecombox();
//	afx_msg void OnCbnSelchangeWbFontsizecombox();
//	afx_msg void OnCbnEditchangeWbFontsizecombox();
	afx_msg void OnBnClickedWbFontboldbtn();
	afx_msg void OnBnClickedWbFontitalicbtn();
	afx_msg void OnBnClickedWbFontunderlinebtn();
	afx_msg void OnBnClickedWbVerticalBtn();
	afx_msg void OnBnClickedWbMTRotateBtn();
	afx_msg void OnBnClickedWbMTZoomBtn();
	afx_msg void OnEnChangeAttributeEdbox();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	afx_msg LRESULT OnCloseColorPaletteMessage(WPARAM wParam, LPARAM lParam);
	//afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void SetBtnState(int BtnId,WB_MOUSE_STATE MouseState);
	void CloseColorPaletteDlg();         //�رյ�ɫ��
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnKillfocusAtributeLinewidthedit();     //�߿�ı༭ʧȥ���� 
	afx_msg void OnEnKillfocusAtributeTransparedit();		//͸���ȱ༭ʧȥ����
	/*.......... ...........................................................................*/
	//wangxiaobin
	void WBFunctionAttributeShowWindow(WBFunctionAttribute functionattribute);//��ť��ʾ״̬
	void WBFunctionAttributeMoveWindowUnSelect();//û��ѡ�л���δ�������ʱ�����������ʾ״̬
	void WBFunctionAttributeMoveWindowDrawLine();//����ʱ�����������ʾ״̬
	void WBFunctionAttributeMoveWindowDrawPology();//��ͼ��ʱ�����������ʾ״̬
	void WBFuncitonAttributeMoveWindowDrawIcoCurve();//��ͼ���������������ʾ״̬
	void WBFunctionAttributeSelectEraser();
	/*.........................................................................................*/
	void SetAttributeBrushSize();
	//afx_msg void OnCbnSelchangeAtributeBrushsizecomboboxex();
	afx_msg void OnCbnSelchangeAttributebrushsizecombo();
	void WBFunctionAttributeHideWindow();
	
	CComboBoxEx m_IcoStyleCombx;
	afx_msg void OnCbnSelchangeAtributeIcostylecomboboxex();

	afx_msg void OnCbnSelchangeWbFontfacecombox();      //ѡ��������
	afx_msg void OnCbnSelchangeWbFontsizecombox();		//ѡ�������С
	afx_msg void OnCbnEditchangeAttributebrushsizecombo();	//���Ӵ�С
	afx_msg void OnCbnEditchangeWbFontsizecombox();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void SetAllComboxExStatue(); //����߿����õ���
    ///*afx_msg*/ void OnEnKillfocusAttributeEdbox();

	void SetIsEditingText(bool isEdit){m_bTextIsEdit = isEdit;}
	bool GetIsEditintText(){return m_bTextIsEdit;}
private:
	bool m_bTextIsEdit;
public:
	CWBFunctionBtn m_DefaultBackGround;
	 SCROLLINFO m_si; 
	afx_msg void OnBnClickedDefaultbackground();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void ResetVScroll(UINT nSBCode);//ѡ������ûѡ�ж����ʱ��ѹ����������ö�
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
