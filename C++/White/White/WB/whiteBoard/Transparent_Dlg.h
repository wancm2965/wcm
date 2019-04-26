#pragma once

/************************************************************************/
/* ͸����������ʾ              by hLongQ                                                    */ 
/************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include "SemiTransWnd.h"
#include "ImageEx.h"
// CTransparent_Dlg �Ի���

class CTransparent_Dlg : public CDialog
{
	DECLARE_DYNAMIC(CTransparent_Dlg)

public:
	CTransparent_Dlg(CWnd* pParent = NULL);   // ��׼���캯��
	CTransparent_Dlg(CWnd* pParent,bool bRrawPencil);
	virtual ~CTransparent_Dlg();

// �Ի�������
	enum { IDD = IDD_TRANSPARENT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
protected:
	CSemiTransWnd m_cstw; //͸��������
	ImageEx*  m_pImage;
	string	m_strname;
	bool		m_bRawPencil;//�Ƿ񻭱�
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
