// IMXECApp.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "IMXECApp.h"

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

// CIMXECApp

BEGIN_MESSAGE_MAP(CIMXECApp, CWinApp)
END_MESSAGE_MAP()


// CIMXECApp ����

CIMXECApp::CIMXECApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CIMXECApp ����

CIMXECApp theApp;
HINSTANCE g_AppInstance = NULL;	


// CIMXECApp ��ʼ��

BOOL CIMXECApp::InitInstance()
{
	CWinApp::InitInstance();

	g_AppInstance=m_hInstance;

	//_CrtSetBreakAlloc(105);

	return TRUE;
}

int CIMXECApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
