#pragma once
#include "afxwin.h"
#include "wbedit.h"
#include "wbfunctionbtn.h"
#include "stdafx.h"
#include "WBAttributeStatic.h"
#include <Windows.h>


#ifndef _JPEG2PDF_H_
#define _JPEG2PDF_H_

/* Defined for Compiling on Windows. Might need to be changed for other compiler */
//typedef unsigned char unsigned char;
//typedef unsigned long unsigned long;
//typedef int int;

#define logMsg printf
#define ERROR 1
#define OK 0
/* */

#define JPEG2PDF_DEBUG	0

#define MAX_PDF_PAGES		256

#define PDF_TOP_MARGIN			(0.0 * PDF_DOT_PER_INCH)
#define PDF_LEFT_MARGIN			(0.0 * PDF_DOT_PER_INCH)

#define PDF_DOT_PER_INCH		72

#define MAX_PDF_PREFORMAT_SIZE 	256		/* Format Before each image, Usually less than 142 Bytes */
#define MAX_PDF_PSTFORMAT_SIZE 	512		/* Format After  each image, Usually less than 400 Bytes */

struct jpeg2pdf_Node_struct {
	struct jpeg2pdf_Node_struct *pNext;
	char  *pJpeg;
	long JpegSize;
	long JpegW, JpegH;
	long PageObj;
	char  preFormat[MAX_PDF_PREFORMAT_SIZE];
	char  pstFormat[MAX_PDF_PSTFORMAT_SIZE];
}; 

typedef struct jpeg2pdf_Node_struct JPEG2PDF_NODE, *PJPEG2PDF_NODE;

#define XREF_ENTRY_LEN		20		/* Each XREF entry is 20 Bytes */
#define OBJNUM_EXTRA		3		/* First Free Object; Kids Object; Catalog Object */
#define OBJNUM_PER_IMAGE	5

#define MAX_KIDS_STRLEN		10		/* Kids Strh Looks Like: "X 0 R ", X = OBJNUM_EXTRA + OBJNUM_PER_IMAGE * (pageNum - 1) */

#define MAX_PDF_XREF		(MAX_PDF_PAGES * OBJNUM_PER_IMAGE + OBJNUM_EXTRA)
#define MAX_PDF_HEADER		64		/* PDF Header, Usually less than 40 Bytes */
#define MAX_PDF_TAILER		( ( MAX_PDF_PAGES * (MAX_KIDS_STRLEN + (OBJNUM_PER_IMAGE * XREF_ENTRY_LEN)) ) + (OBJNUM_EXTRA * XREF_ENTRY_LEN) + 256 )

typedef struct {
	/* Link List Stuff */
	PJPEG2PDF_NODE pFirstNode;
	PJPEG2PDF_NODE pLastNode;
	long nodeCount;
	/* PDF Stuff */
	char pdfHeader[MAX_PDF_HEADER];
	char pdfTailer[MAX_PDF_TAILER];					/* 28K Bytes */
	char pdfXREF[MAX_PDF_XREF][XREF_ENTRY_LEN + 1];	/* 27K Bytes */
	long pageW, pageH, pdfObj, currentOffSet, imgObj;
} JPEG2PDF, *PJPEG2PDF;

#endif /* _JPEG2PDF_H_ */


enum SavePage
{
	WBSaveCurPage = 0,
	WBSaveAllPage,
	WBSaveSelectPage
};
typedef struct WBSavePictureToFile
{
	CRect rectPage;
	BOOL bSave;
	//CString strFilePath;
	HDC		PageHdc;
}WBSavePictureFile,*pWBSavePictureFile;
// CWBSavePictureDlg 对话框

class CWBSavePictureDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBSavePictureDlg)

public:
	CWBSavePictureDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBSavePictureDlg();

// 对话框数据
	enum { IDD = IDD_SAVE_PICTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRadioCurPage();
	afx_msg void OnRadioAllPage();
	afx_msg void OnRadioSelePage();

	CString	GetbIsOK() ;
	void PrintCurrentPage();
private:
	CButton m_BtnClose;
	pWBSavePictureFile m_pWBSSavePictureFile;
	CBitmap*			m_pBitmap;
	int					m_xScreen;
	int					m_yScreen;
	std::vector<int>	m_nPageNumber;
private:
	CWBAttributeStatic m_StrPageId;
	CButton m_BtnOK;
	CWBFunctionBtn m_BtnCurPage;
	CWBFunctionBtn m_BtnALLPage;
	CWBFunctionBtn m_BtnSelectPage;
	CWBEdit m_EditPageCount;
	CString m_strWritePage;
	CString m_strtempath;
	SavePage m_nSavePage;
	bool m_bIsShow;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void SetIsSelectBtn(int  BtnId = 0);
	HBITMAP CopyPageBitmapToPicture(pWBSavePictureFile pFile,CString strName = _T(""));  //考贝桌面到位图

	void	SaveAllPageBitmapToPicture(CString strFilePath = _T(""));
	void	SaveCurPageBitmapToPicture(CWBPage *page = NULL,CString strFilePath = _T(""));
	void	SaveSelctPageBitmapToPicture(CString strFilePath = _T(""));

	CString GetFileName(CString str);
	CString GetFilePath(CString str);
	CString GetFileExt(CString str);
	bool	CheckPageStr();
	string U2A( CString strUnicodeString);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	
	bool	SavePageNumberOk(int nPage);
	void	CleanVectorPageNumber();

	PJPEG2PDF 	Jpeg2PDF_BeginDocument(double pdfW, double pdfH);	/* pdfW, pdfH: Page Size in Inch ( 1 inch=25.4 mm ) */
	int 		Jpeg2PDF_AddJpeg(PJPEG2PDF pPDF, unsigned long imgW, unsigned long imgH, unsigned long fileSize, unsigned char *pJpeg, unsigned char isColor);
	unsigned long 		Jpeg2PDF_EndDocument(PJPEG2PDF pPDF);
	int 		Jpeg2PDF_GetFinalDocumentAndCleanup(PJPEG2PDF pPDF, unsigned char *outPDF, unsigned long *outPDFSize);
	int get_jpeg_size(unsigned char* data, unsigned int data_size, unsigned short *width, unsigned short *height);
	int SavePdf(void);
	void insertJPEGFile(const char *fileName, int fileSize, PJPEG2PDF pdfId);
	void Jpeg2PDF_SetXREF(PJPEG2PDF pPDF, int index, int offset, char c);
	BOOL DeleteDirectory(CString strDir);
};
