// VIDECYNCAPP.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "VIDECYNCAPP.h"

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

// VIDECYNCApp

BEGIN_MESSAGE_MAP(VIDECYNCApp, CWinApp)
END_MESSAGE_MAP()


// VIDECYNCApp ����

VIDECYNCApp::VIDECYNCApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� VIDECYNCApp ����

VIDECYNCApp theApp;
HINSTANCE g_hInstance=NULL;

// VIDECYNCApp ��ʼ��

BOOL VIDECYNCApp::InitInstance()
{
	CWinApp::InitInstance();

	g_hInstance=m_hInstance;

	return TRUE;
}

//
//#include "../VIDEC_FRE/VIDECFREHeader.h"
//
//void * FreMalloc(size_t size,char * fileName,char * func,int nLine)
//{
//	void * p=NULL;
//	p = malloc(size);
//	if(p == NULL)
//		DebugOutput("FreMalloc NULL,File:%s,FUN:%s,Line:%d \n", fileName,   func,  nLine);
//
//	return p;
//}
