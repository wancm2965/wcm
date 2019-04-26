#pragma once

/************************************************************************/
/* 透明窗口类显示              by hLongQ                                                    */ 
/************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include "SemiTransWnd.h"
#include "ImageEx.h"
// CTransparent_Dlg 对话框

class CTransparent_Dlg : public CDialog
{
	DECLARE_DYNAMIC(CTransparent_Dlg)

public:
	CTransparent_Dlg(CWnd* pParent = NULL);   // 标准构造函数
	CTransparent_Dlg(CWnd* pParent,bool bRrawPencil);
	virtual ~CTransparent_Dlg();

// 对话框数据
	enum { IDD = IDD_TRANSPARENT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
protected:
	CSemiTransWnd m_cstw; //透明窗口类
	ImageEx*  m_pImage;
	string	m_strname;
	bool		m_bRawPencil;//是否画笔
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void DrawSomething();
	void SetBitmap(UINT nIDResource);
	void SetStrname(string str_name);
	string GetUsrName() {return m_strname;};
	void OnInitImageEx();
	afx_msg void OnClose();
};
