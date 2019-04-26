#pragma once

#include "HPDLG_Base.h"

//�������������б��ļ�����
typedef enum _BROWSER_TYPE
{
	BROWSER_TYPE_ALL = 0,
	BROWSER_TYPE_MEDIA,       //AVMPlayer ���ļ�����
	BROWSER_TYPE_CONFERENCE,  //ConferencePlayer ���ļ�����
}BROWSER_TYPE;

class AFX_EXT_CLASS HPDLG_FileBrowser
	: public HPDLG_Base
{
public:
	HPDLG_FileBrowser(void);
	virtual~ HPDLG_FileBrowser(void);

	/*
	�������ܣ���ģ̬�Ի��򣬴�Ϊֱ�������ļ�
	��    ����pParent	������
	�� �� ֵ�������Ƿ�ɹ�
	*/
	bool Create(CWnd* pParent = NULL, BROWSER_TYPE nBrowserType = BROWSER_TYPE_ALL);

	/*
	�������ܣ�ģ̬�Ի��򣬴򿪷����ļ�·��
	��    ������
	�� �� ֵ��
	*/
	INT_PTR DoModal(BROWSER_TYPE nBrowserType = BROWSER_TYPE_ALL);

	/* ǿ�ƹرնԻ���*/
	void Destroy();

	//�����ļ�·��
	CString GetCurSelFilePath();

private:
	CString m_strFilePath;
};
