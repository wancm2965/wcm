// VIDECUDAAPP.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "VIDECUDAAPP.h"

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

// VIDECUDAApp

BEGIN_MESSAGE_MAP(VIDECUDAApp, CWinApp)
END_MESSAGE_MAP()


// VIDECUDAApp ����

VIDECUDAApp::VIDECUDAApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� VIDECUDAApp ����

VIDECUDAApp theApp;
HINSTANCE g_hInstance=NULL;

// VIDECUDAApp ��ʼ��

BOOL VIDECUDAApp::InitInstance()
{
	CWinApp::InitInstance();

	g_hInstance=m_hInstance;

	return TRUE;
}
