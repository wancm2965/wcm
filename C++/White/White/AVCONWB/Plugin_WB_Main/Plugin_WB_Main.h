// Plugin_WB_Main.h : Plugin_WB_Main DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "IPlugin\WB_Main\IPlugin_WB_Main.h"



// CPlugin_WB_MainApp
// �йش���ʵ�ֵ���Ϣ������� Plugin_WB_Main.cpp
//
class CPlugin_WB_MainApp : public CWinApp
{
public:
	CPlugin_WB_MainApp();

	// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

namespace AVCONPlugin
{
	class CPlugin_WB_Main : public IPlugin_WB_Main
	{
	public:
		CPlugin_WB_Main();
		~CPlugin_WB_Main();

	// ��д
	public:

		// �̳�IPlugin_WB_Main�ӿ�
		// ��ȡ�������
		virtual std::string Name();
		// ��ȡ������ܵ�
		virtual std::string ExtendPoint();
		// ��ʼ�����
		virtual bool InitPlugin();
		// ����˳�����
		virtual bool ClosePlugin();

		// ִ�нӿ�
		virtual bool Init();
		virtual bool Run();
		virtual bool Close();

		virtual void Exit(void);

		//DECLARE_MESSAGE_MAP()
	};
}
