#pragma once
#include <ShellAPI.h>
#include <ShlObj.h>

class CListUIFile : public CListUI
{
public:
	CListUIFile(CPaintManagerUI& paint_manager);
	~CListUIFile();

public:
	void AddNode(CString strName, SHFILEINFO& shfi, int nLeg, ListItem It);
	bool OnItemlbEvent(void* p); //�йܵ�OnEvent��Ϣ
private:
	int m_nBmpCount;
	bool bDlgBuilder;
	CPaintManagerUI& paint_manager_;
	CDialogBuilder m_dlgBuilder;
};
