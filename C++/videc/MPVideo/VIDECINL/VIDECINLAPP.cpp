// VIDECINLAPP.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "VIDECINLAPP.h"

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

// VIDECINLApp

BEGIN_MESSAGE_MAP(VIDECINLApp, CWinApp)
END_MESSAGE_MAP()


// VIDECINLApp ����

VIDECINLApp::VIDECINLApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� VIDECINLApp ����

VIDECINLApp theApp;
HINSTANCE g_hInstance=NULL;

// VIDECINLApp ��ʼ��

BOOL VIDECINLApp::InitInstance()
{
	CWinApp::InitInstance();

	g_hInstance=m_hInstance;

	return TRUE;
}
