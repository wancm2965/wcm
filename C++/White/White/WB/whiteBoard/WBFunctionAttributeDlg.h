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

//wangxiaobin属性对话框中属性结构体存储属性数据
typedef struct _WBFunctionAttribute
{
	_WBFunctionAttribute();
	_WBFunctionAttribute(const _WBFunctionAttribute &pItem);//拷贝构造

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

    /*新增属性*/
	int IcoStyleStatic; 
	int IcoStyleComBox;
}WBFunctionAttribute;
// CWBFunctionAttributeDlg 对话框
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
		CString nFontFace = _T("宋体"),
		int nFontSize = 24);   // 标准构造函数
	virtual ~CWBFunctionAttributeDlg();
	virtual BOOL OnInitDialog();
	virtual void PreInitDialog();
	virtual void OnCancel();
	void SetComboxItem(HICON *iconArray, CImageList *imList,int iconCount,UINT cionResForstId,CComboBoxEx *comboxData );

	void SetComboxItem();//插入combox项 从文件加载bitmap
	BOOL SaveBitmap(CDC *pDC, CRect rect, const char *filename); //将文字转化为bitmap保存

	void InsertStr(CComboBoxEx *comboxData,CString str);
	void SetLineType(int PenLineType);
	void SetLineCap(LineCap nStartLineCap, LineCap nEndLineCap);
	void SetLineWidth(int PenLineWidth);
	void SetIcoStyle(CString strIcoStyle);
	void SetSliderCtrlPos();//设置滑动块位置
	//void SetTransparEditValue();//初始透明度显示值

	CWBFunctionBtn *GetFontBoldBtn();
	CWBFunctionBtn *GetFontItalicBtn();
	CWBFunctionBtn *GetFontUnderlineBtn();
	CWBFunctionBtn *GetFontVerticalBtn();

	/*	void UpdateAttributeDlg();*/

	//int m_nCurWidth; 
	COLORREF m_clrLine ; //线颜色
	int m_nLineType;  //线型
	int m_nCurArrow ; //是否有箭头
	CString m_nLineWidth; //线宽
	BOOL m_bIsSolid; //是否填充
	COLORREF m_clrSolid; //填充颜色
	LineCap m_nStartLineCap; //起点线帽
	LineCap m_nEndLineCap; //终点线帽
	CString m_nTranspar; //透明度
	CString m_nFontFace;//字体类型
	CString m_strFontSize;//字号大小
	CString m_strBrushSize; //擦子大小
	int m_nPreFontSize;    //输入不合法显示的字体大小
	/*	int m_nPosition;//滑块位置*/

	// 对话框数据
	enum { IDD = IDD_WB_ATTRIBUTEDLG};
private:
	HICON m_LineWidthicon[7];
	CImageList m_LineWidthImageList;

	HICON m_IcoStyleicon[7]; //Ico图标句柄数组
	CImageList m_IcoStyleImageList; //Ico图标Image列表

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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	CTransparentButtonEx *m_LineColorButton;//颜色按钮
	CWBColorPaletteDlg *m_pAttriButeColorPatetteDlg;//线色调色板
	CWBColorPaletteDlg *m_pAttriButeSolidPatetteDlg;//填充色调色板
	CWBPromptFontSetDlg *m_pAttriButePromptFontSetDlg;//属性面板的字体
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

	CWBAttributeStatic m_IcoStyleStatic; //声明图标样式Static

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
	CWBEdit m_TextEditBox;// 文本编辑框
	CString m_Textstr; //文本内容
	CWBAttributeStatic m_cAttributeTransperStatic;
	CWBFunctionBtn m_cReduceTransperBtn;
	CWBFunctionBtn m_cAddTransparBtn;
	//CWBFunctionBtn m_DefaultBackGroundBtn;
	/*	CEdit m_LineWidthValue;*/

	//文档手势触摸操作 luowei 2012-05-03
	CWBAttributeStatic m_cAttMTOperate;
	CWBFunctionBtn m_btnMTRotate;
	CWBFunctionBtn m_btnMTZoom;
	int m_nDrawPattern ;
	afx_msg void OnEnChangeLinewidthedit();             //编辑线宽
	afx_msg void OnCbnSelchangeLinewidthcomboboxex();  //选择线宽
	afx_msg void OnBnClickedCheckSolid();				//选择填充
	afx_msg void OnClickedCbtnLinecolor();              //选择线的颜色
	afx_msg void OnCbnSelchangeLinetypecomboboxex();	//选择线型
	afx_msg void OnCbnSelchangeArrowcomboboxex();		//选择线的箭头
	afx_msg void OnBnClickedCbtnSolidcolor();			//选择填充色
	afx_msg void OnBnClickedReducetranspar();			//减少透明度
	afx_msg void OnBnClickedAddtranspar();				//增加透明度
	afx_msg void OnEnChangeTRANSPAREDIT();				//编辑改变透明度
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
	void CloseColorPaletteDlg();         //关闭调色板
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnKillfocusAtributeLinewidthedit();     //线宽的编辑失去焦点 
	afx_msg void OnEnKillfocusAtributeTransparedit();		//透明度编辑失去焦点
	/*.......... ...........................................................................*/
	//wangxiaobin
	void WBFunctionAttributeShowWindow(WBFunctionAttribute functionattribute);//按钮显示状态
	void WBFunctionAttributeMoveWindowUnSelect();//没有选中或者未点击对象时候属性面板显示状态
	void WBFunctionAttributeMoveWindowDrawLine();//画线时候属性面板显示状态
	void WBFunctionAttributeMoveWindowDrawPology();//画图形时候属性面板显示状态
	void WBFuncitonAttributeMoveWindowDrawIcoCurve();//画图标曲线属性面板显示状态
	void WBFunctionAttributeSelectEraser();
	/*.........................................................................................*/
	void SetAttributeBrushSize();
	//afx_msg void OnCbnSelchangeAtributeBrushsizecomboboxex();
	afx_msg void OnCbnSelchangeAttributebrushsizecombo();
	void WBFunctionAttributeHideWindow();
	
	CComboBoxEx m_IcoStyleCombx;
	afx_msg void OnCbnSelchangeAtributeIcostylecomboboxex();

	afx_msg void OnCbnSelchangeWbFontfacecombox();      //选择字体名
	afx_msg void OnCbnSelchangeWbFontsizecombox();		//选择字体大小
	afx_msg void OnCbnEditchangeAttributebrushsizecombo();	//擦子大小
	afx_msg void OnCbnEditchangeWbFontsizecombox();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void SetAllComboxExStatue(); //与会者控制用到的
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
	void ResetVScroll(UINT nSBCode);//选择对象或没选中对象的时候把滚动条重新置顶
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
