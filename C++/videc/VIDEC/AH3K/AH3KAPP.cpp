// AH3KAPP.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "AH3KAPP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	注意！
//
//		如果此 DLL 动态链接到 MFC
//		DLL，从此 DLL 导出并
//		调入 MFC 的任何函数在函数的最前面
//		都必须添加 AFX_MANAGE_STATE 宏。
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

// AH3KApp

BEGIN_MESSAGE_MAP(AH3KApp, CWinApp)
END_MESSAGE_MAP()


// AH3KApp 构造

AH3KApp::AH3KApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 AH3KApp 对象

AH3KApp theApp;

// AH3KApp 初始化

BOOL AH3KApp::InitInstance()
{
	CWinApp::InitInstance();


	return TRUE;
}
