#pragma once

#define NR_EMOTIONS			60
#include "KBASE.h"

//---------------------------------------------------------------------------------------
// KRichEditCtrl类定义
class AFX_EXT_CLASS KRichEditCtrl : public CRichEditCtrl
{
public:
	KRichEditCtrl();
	virtual ~KRichEditCtrl();

public:
	virtual BOOL Create( DWORD dwStyle, const RECT& rcRect, CWnd* pParentWnd, UINT nID ) ;
	static LPCTSTR emotions[];
	virtual BOOL AutoURLDetect(BOOL bEnable) ;

	virtual void SetImage(CImageList& pImageList);
	virtual void SetText(CString csText);
	virtual CString GetText();

	virtual void SetSysTextFormat(CHARFORMAT2& cf){m_cfSystem=cf;};
	virtual CHARFORMAT2& GetSysTextFormat(void){return m_cfSystem;};
	
	virtual void SetLineSpace(int nSpace);

	virtual void InsertText(CString csText);
	virtual void InsertSystemText(CString csText,HBITMAP hBitmap);
	virtual void InsertTitleText(CString csText);
	virtual void InsertChatText(CString csText,CHARFORMAT2& cf);
	virtual void InsertEmotion(int i);
	virtual void InsertBitmap(UINT nIDResource);
	virtual void InsertBitmap(HBITMAP hBitmap);
	virtual void InsertGIF(CString csGifPath);
	virtual void ClearAllText(void);

	virtual void SaveToFile(LPCTSTR lpszFileName);
	virtual void LoadFromFile(LPCTSTR lpszFileName);
	virtual void ScrollToBottom();

	virtual void ConvertTextFormat(CHARFORMAT& cf,std::string& cfstring);
	virtual void ConvertTextFormat(const std::string& cfstring,CHARFORMAT& cf);

protected:
	CImageList* m_pImageList;
	HBITMAP GetImage(CImageList& ImageList, int nNum);
	CHARFORMAT2 m_cfSystem;
	CHARFORMAT2 m_cfTitle1;	//标题格式1
	CHARFORMAT2 m_cfTitle2;	//标题格式2

	int FindEmotion(LPCTSTR str);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLink(NMHDR *pNMHDR, LRESULT *result);
public:
	afx_msg void OnEnChange();
};
