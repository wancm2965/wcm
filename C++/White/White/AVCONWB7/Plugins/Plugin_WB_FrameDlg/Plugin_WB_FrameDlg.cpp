// Plugin_WB_FrameDlg.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "Plugin_WB_FrameDlg.h"
#include "AutoLockResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CPlugin_WB_FrameDlgApp

BEGIN_MESSAGE_MAP(CPlugin_WB_FrameDlgApp, CWinApp)
END_MESSAGE_MAP()


// CPlugin_WB_FrameDlgApp ����

CPlugin_WB_FrameDlgApp::CPlugin_WB_FrameDlgApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPlugin_WB_FrameDlgApp ����

CPlugin_WB_FrameDlgApp theApp;


// CPlugin_WB_FrameDlgApp ��ʼ��

BOOL CPlugin_WB_FrameDlgApp::InitInstance()
{
	CWinApp::InitInstance();
 	g_hInstance = theApp.m_hInstance;
	return TRUE;
}
