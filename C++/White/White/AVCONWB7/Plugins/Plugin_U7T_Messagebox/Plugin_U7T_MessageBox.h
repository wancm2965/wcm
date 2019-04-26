#pragma once
#include "IPlugin\IU7_MessageBox\IPlugin_U7T_MessageBox.h"
#include "GlobalMessageBoxManager.h"

namespace AVCONPlugin
{
	class CPlugin_U7T_MessageBox : public IPlugin_U7T_MessageBox
	{
	public:
		CPlugin_U7T_MessageBox(void);
		~CPlugin_U7T_MessageBox(void);

	// �̳���IPlugin�ӿ�
	public:
		// ��ȡ�������
		virtual std::string Name();
		// ��ȡ������ܵ�
		virtual std::string ExtendPoint();
		// ��ʼ�����
		virtual bool InitPlugin();
		// ����˳�����
		virtual bool ClosePlugin();

	// �̳���IPlugin_U7T_MessageBox�ӿ�
	public:
		///����һ�����ģ̬����
		virtual int MessageBox(HWND hOwner,CString strTitle,CString strContent,UINT nStye, bool bAutoDestroy = false);
		///�ر����е���ʽ�Ĵ���
		virtual void CloseAllMessageBox();

	private:
		CGlobalMessageBoxManager m_GlobalMessageBoxManager;
	};
}
