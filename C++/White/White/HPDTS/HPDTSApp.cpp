// HPDTS.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "HPDTSApp.h"

#ifdef WIN32

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
//		extern "C" bool PASCAL EXPORT ExportedFunction()
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

// CHPDTSApp

BEGIN_MESSAGE_MAP(CHPDTSApp, CWinApp)
END_MESSAGE_MAP()


// CHPDTSApp ����

CHPDTSApp::CHPDTSApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CHPDTSApp ����

CHPDTSApp theApp;


// CHPDTSApp ��ʼ��

BOOL CHPDTSApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

#endif