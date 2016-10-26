// CommandMode.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CommandMode.h"
#include "Invoker.h"
#include "CommandBaseClass.h"
#include "ConreteCommand.h"
#include "Receiver.h"

#include "Custment.h"
#include "CookCommand.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
	}

	//处理类
	CReceiver* pReceive = new CReceiver();

	//命令控制类
	CCommandBaseClass* pComand = new CConreteCommand(pReceive);

	//请求类
	CInvoker* pInvoker = new CInvoker(pComand);

	//请求类 并不知道是哪个处理类来处理请求
	pInvoker->call();


	delete pInvoker;
	pInvoker = NULL;
	delete pComand;
	pComand = NULL;
	delete pReceive;
	pReceive = NULL;


	//CCookCommand 提供命令控制类以及不同命令的不同处理类
	CCookCommand* pCookCmd = new CCookCommand();
	CCustment* pCustment = new CCustment(pCookCmd);
	//CallDrink根据不同的要求  来确定不同的处理类来处理命令
	pCustment->CallDrink(ActionType::Tea);
	pCustment->CallDrink(ActionType::Coffee);
	pCustment->CallDrink(ActionType::Water);

	delete pCustment;
	pCustment = NULL;
	delete pCookCmd;
	pCookCmd = NULL;

	return nRetCode;
}
