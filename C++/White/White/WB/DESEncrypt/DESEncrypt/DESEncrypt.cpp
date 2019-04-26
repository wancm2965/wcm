// DESEncrypt.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "DESEncrypt.h"


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

// CDESEncryptApp

BEGIN_MESSAGE_MAP(CDESEncryptApp, CWinApp)
END_MESSAGE_MAP()


// CDESEncryptApp ����

CDESEncryptApp::CDESEncryptApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CDESEncryptApp ����

CDESEncryptApp theApp;

extern EncryptAddUnencrypt* g_EncryptAddUnencrypt = NULL;
// CDESEncryptApp ��ʼ��

BOOL CDESEncryptApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

BOOL CDESEncryptApp::ExitInstance()
{
	if (g_EncryptAddUnencrypt)
	{
		delete g_EncryptAddUnencrypt;
		g_EncryptAddUnencrypt = NULL;
	}
	return TRUE;
}

extern "C" _declspec(dllexport)  void _EncryptCode(CString EncryptData)
{   
	if (NULL == g_EncryptAddUnencrypt)
	{
		g_EncryptAddUnencrypt = new EncryptAddUnencrypt;
	};
	g_EncryptAddUnencrypt->EncryptCode(EncryptData);//�����ഫ��Ҫת�����ַ���
}


extern "C" _declspec(dllexport) void _UnencryptCode(CString FilePath,char* resultData,int &resultDataLenth)
{   
	if (NULL == g_EncryptAddUnencrypt)
	{
		g_EncryptAddUnencrypt = new EncryptAddUnencrypt;
	}
	g_EncryptAddUnencrypt->UnencryptCode(FilePath,resultData,resultDataLenth);//�����ഫ��Ҫת�����ַ���
}

extern "C" _declspec(dllexport) void _SetKeyOriginalData( CString KeyOriginalData )
{
	if (NULL == g_EncryptAddUnencrypt)
	{
		g_EncryptAddUnencrypt = new EncryptAddUnencrypt;
	}
	g_EncryptAddUnencrypt->SetKeyOriginalData(KeyOriginalData);
}
