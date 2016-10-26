// CommandMode.cpp : �������̨Ӧ�ó������ڵ㡣
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


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
	}

	//������
	CReceiver* pReceive = new CReceiver();

	//���������
	CCommandBaseClass* pComand = new CConreteCommand(pReceive);

	//������
	CInvoker* pInvoker = new CInvoker(pComand);

	//������ ����֪�����ĸ�����������������
	pInvoker->call();


	delete pInvoker;
	pInvoker = NULL;
	delete pComand;
	pComand = NULL;
	delete pReceive;
	pReceive = NULL;


	//CCookCommand �ṩ����������Լ���ͬ����Ĳ�ͬ������
	CCookCommand* pCookCmd = new CCookCommand();
	CCustment* pCustment = new CCustment(pCookCmd);
	//CallDrink���ݲ�ͬ��Ҫ��  ��ȷ����ͬ�Ĵ���������������
	pCustment->CallDrink(ActionType::Tea);
	pCustment->CallDrink(ActionType::Coffee);
	pCustment->CallDrink(ActionType::Water);

	delete pCustment;
	pCustment = NULL;
	delete pCookCmd;
	pCookCmd = NULL;

	return nRetCode;
}
