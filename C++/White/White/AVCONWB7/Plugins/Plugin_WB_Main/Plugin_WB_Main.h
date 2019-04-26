#pragma once
#include "MainParentWnd.h"
#include "DlgMainFrame.h"
#include "IPlugin/Plugin_WB_Main/IPlugin_WB_Main.h"
namespace AVCONPlugin
{
	class CPlugin_WB_Main : public IPlugin_WB_Main
	{
	public:
		CPlugin_WB_Main();
		~CPlugin_WB_Main();
		//��д
	public:
		// ��ȡ�������
		virtual std::string Name();
		// ��ȡ������ܵ�
		virtual std::string ExtendPoint();
		// ��ʼ�����
		virtual bool InitPlugin();
		// ����˳�����
		virtual bool ClosePlugin();

		//������ӿ�
		// ִ�нӿ�
		virtual bool Init();
		virtual bool Run();        // to do   ��������в���
		virtual bool Close();

		virtual void Exit(void);
		
		virtual HWND GetMainWnd();
		virtual HWND GetExeParentWnd();
	private:
		CMainParentWnd* m_pMainParent;
		CDlgMainFrame* m_pDlgMainFrame;
	};
}