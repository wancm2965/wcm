// AH2KAPP.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "AH2KAPP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	ע�⣡
//
//		����� DLL ��̬���ӵ� MFC
//		DLL���Ӵ� DLL ������
//		���� MFC ���κκ����ں�������ǰ��
//		��������� AFX_MANAGE_STATE �ꡣ
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

// AH2KApp

BEGIN_MESSAGE_MAP(AH2KApp, CWinApp)
END_MESSAGE_MAP()


// AH2KApp ����

AH2KApp::AH2KApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� AH2KApp ����

AH2KApp theApp;
HINSTANCE g_hInstance=NULL;

// AH2KApp ��ʼ��

BOOL AH2KApp::InitInstance()
{
	CWinApp::InitInstance();

	g_hInstance=m_hInstance;

	return TRUE;
}
