#pragma once
#include "IPlugin/IU7T_OpenSave/IPlugin_U7T_OpenSave.h"
#include "DlgOpenSave.h"

namespace AVCONPlugin
{
	class CPlugin_U7T_OpenSave :
		public IPlugin_U7T_OpenSave
	{
	public:
		CPlugin_U7T_OpenSave(void);
		~CPlugin_U7T_OpenSave(void);

	public:
		//////////////////////////////////////////////////////////////////////////
		//�̳�IPlugin_U7T_OpenSave�ӿ�
		virtual HWND CreateOpenSave(BOOL bOpenSave,LPCTSTR lpszDefExt = NULL,LPCTSTR lpszFilter = NULL,LPCTSTR strMemberID=_T("Local"),HWND hParent=NULL ,LPCTSTR strFilePath =_T(""));
		virtual void ShowDlg(bool bShow);
		virtual const char* GetOpenFile(void);
		virtual void DestroyOpenSave(void);
		//////////////////////////////////////////////////////////////////////////
		//�̳�IPlugin�ӿ�
		// ��ȡ�������
		virtual std::string Name();
		// ��ȡ������ܵ�
		virtual std::string ExtendPoint();
		// ��ʼ�����
		virtual bool InitPlugin();
		// ����˳�����
		virtual bool ClosePlugin();

	private:
		CDlgOpenSave*	m_pDlgOpenSave;
		std::string		m_strFilePathName;
	};
}
