//========================================================================
//类名：MessageBox
//功能：MessageBox
//作者：戴磊
//时间：2010-5-25
//说明：
//--------------------------------------------------------------------------
#pragma once


class AFX_EXT_CLASS HPDLG_MessageBox
{
public:
	HPDLG_MessageBox(void){};
	virtual~ HPDLG_MessageBox(void){};

	// 显示对话框
	virtual int MessageBox_DoModal(void) = 0;

	// 关闭对话框
	virtual void MessageBox_Close(void) = 0;

	virtual CWnd* GetMessageBox_Parent() = 0;
	virtual HWND GetMsgSafeHwnd() = 0;
};

// 创建对话框
HPDLG_API HPDLG_MessageBox* CreateMessageBox(CWnd* pParent, LPCTSTR lpszText, LPCTSTR lpszCaption = NULL, UINT nType = MB_OK);
