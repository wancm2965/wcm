// WBMIDI.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "WBMIDI.h"
#include "WBMidiConvert.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


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

// CWBMIDIApp

BEGIN_MESSAGE_MAP(CWBMIDIApp, CWinApp)
END_MESSAGE_MAP()


// CWBMIDIApp ����

CWBMIDIApp::CWBMIDIApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CWBMIDIApp ����

CWBMIDIApp theApp;


// CWBMIDIApp ��ʼ��

BOOL CWBMIDIApp::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}

int CWBMIDIApp::ExitInstance()
{

	if (CWBMidiConvert::GetIns() != NULL)
	{
		CWBMidiConvert::DestroyIns();
	}
	return CWinApp::ExitInstance();
}


int WBMIDI_API ShowMidiFileInWB( CString strfilepath )
{
	if (strfilepath.IsEmpty())
	{
		return FALSE;
	}
	if (CWBMidiConvert::GetIns() != NULL)
	{
		CWBMidiConvert::DestroyIns();
	}
	if (CWBMidiConvert::GetIns() == NULL)
	{
		CWBMidiConvert::CreateIns();
	}
	int height = 0;
	CWBMidiConvert::GetIns()->OpenMidiFile(strfilepath,height);
	
	return height;
}

// void WBMIDI_API SetZoomSize(float nvalue,CDC* hdc)
// {
// 	if (CWBMidiConvert::GetIns() == NULL)
// 	{
// 		return;
// 	}
// 	else
// 	{
// 		CWBMidiConvert::GetIns()->SetZoom(nvalue);
// 		CWBMidiConvert::GetIns()->Draw(hdc);
// 	}
// }

void WBMIDI_API DrawMidiFiveStaff(CDC* hdc)
{
	if (CWBMidiConvert::GetIns() == NULL)
	{
		return;
	}
	CWBMidiConvert::GetIns()->Draw(hdc);
}

void WBMIDI_API CloseConvert()
{
	if (CWBMidiConvert::GetIns() != NULL)
	{
		CWBMidiConvert::DestroyIns();
	}
}