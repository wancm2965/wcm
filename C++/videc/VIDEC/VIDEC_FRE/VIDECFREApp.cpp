// VIDECFREAPP.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "VIDECFREAPP.h"

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

// VIDECFREApp

BEGIN_MESSAGE_MAP(VIDECFREApp, CWinApp)
END_MESSAGE_MAP()


// VIDECFREApp ����

VIDECFREApp::VIDECFREApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� VIDECFREApp ����

VIDECFREApp theApp;
HINSTANCE g_hInstance=NULL;

// VIDECFREApp ��ʼ��

BOOL VIDECFREApp::InitInstance()
{
	CWinApp::InitInstance();

	g_hInstance=m_hInstance;

	return TRUE;
}
