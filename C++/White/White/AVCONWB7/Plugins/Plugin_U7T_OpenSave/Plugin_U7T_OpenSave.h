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
		//继承IPlugin_U7T_OpenSave接口
		virtual HWND CreateOpenSave(BOOL bOpenSave,LPCTSTR lpszDefExt = NULL,LPCTSTR lpszFilter = NULL,LPCTSTR strMemberID=_T("Local"),HWND hParent=NULL ,LPCTSTR strFilePath =_T(""));
		virtual void ShowDlg(bool bShow);
		virtual const char* GetOpenFile(void);
		virtual void DestroyOpenSave(void);
		//////////////////////////////////////////////////////////////////////////
		//继承IPlugin接口
		// 获取插件名称
		virtual std::string Name();
		// 获取插件功能点
		virtual std::string ExtendPoint();
		// 初始化插件
		virtual bool InitPlugin();
		// 插件退出处理
		virtual bool ClosePlugin();

	private:
		CDlgOpenSave*	m_pDlgOpenSave;
		std::string		m_strFilePathName;
	};
}
