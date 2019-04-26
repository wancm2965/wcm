// WBMIDI.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "WBMIDI.h"
#include "WBMidiConvert.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CWBMIDIApp

BEGIN_MESSAGE_MAP(CWBMIDIApp, CWinApp)
END_MESSAGE_MAP()


// CWBMIDIApp 构造

CWBMIDIApp::CWBMIDIApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CWBMIDIApp 对象

CWBMIDIApp theApp;


// CWBMIDIApp 初始化

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