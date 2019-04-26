// WBDicomp.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "WBDicomp.h"
#include "DicomToBmp.h"

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

// CWBDicompApp

BEGIN_MESSAGE_MAP(CWBDicompApp, CWinApp)
END_MESSAGE_MAP()


// CWBDicompApp ����

CWBDicompApp::CWBDicompApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CWBDicompApp ����

CWBDicompApp theApp;
extern CDicomToBmp* g_Dicomtobmp = NULL;

// CWBDicompApp ��ʼ��

BOOL CWBDicompApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

BOOL CWBDicompApp::ExitInstance()
{
	/*if (NULL != g_Dicomtobmp)
	{
	delete g_Dicomtobmp;
	g_Dicomtobmp = NULL;
	}*/
	return TRUE;
}


extern "C" _declspec(dllexport) int DicomToBmpDll(CString strFilePath, CString g_strAvconTempPath)
{   
	g_Dicomtobmp = new CDicomToBmp;
	int Dicom_Return = g_Dicomtobmp->ConvertDicomToBMP(strFilePath,g_strAvconTempPath);//������ ��ʼת�� ��������·��

	return Dicom_Return;
}

 extern "C" _declspec(dllexport) BOOL GetDicomBmpPath(char *pszPath, int nLen)
{
	if (nLen < 512)
	{
		return FALSE;
	}
	else
	{
		g_Dicomtobmp->GetBMPFileName(pszPath,nLen);
		return TRUE;
	}
	
	
}
