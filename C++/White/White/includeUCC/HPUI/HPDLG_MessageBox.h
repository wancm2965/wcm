//========================================================================
//������MessageBox
//���ܣ�MessageBox
//���ߣ�����
//ʱ�䣺2010-5-25
//˵����
//--------------------------------------------------------------------------
#pragma once


class AFX_EXT_CLASS HPDLG_MessageBox
{
public:
	HPDLG_MessageBox(void){};
	virtual~ HPDLG_MessageBox(void){};

	// ��ʾ�Ի���
	virtual int MessageBox_DoModal(void) = 0;

	// �رնԻ���
	virtual void MessageBox_Close(void) = 0;

	virtual CWnd* GetMessageBox_Parent() = 0;
	virtual HWND GetMsgSafeHwnd() = 0;
};

// �����Ի���
HPDLG_API HPDLG_MessageBox* CreateMessageBox(CWnd* pParent, LPCTSTR lpszText, LPCTSTR lpszCaption = NULL, UINT nType = MB_OK);
