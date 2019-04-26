/* 
	���ߣ�������
	���ܣ�windows ����ƻ� 
	Ҫ��
		1.ϵͳ�汾��Vista������	
		2.ʹ��ǰ�������COM��ʼ��
*/
#pragma once
#include <string>
#include <atlbase.h>  
#include <comdef.h>  
#include <taskschd.h>  
# pragma comment(lib, "taskschd.lib")  
# pragma comment(lib, "comsupp.lib")  

//���ã�windows�ƻ���������������ܸ��ӣ�����Ҫʱ�����ơ�
typedef struct _structTask
{
	char chJobName[MAX_PATH]; //��������
	char chExePath[MAX_PATH]; //����·�������������ִ�г���
	_structTask()
	{
		memset(chJobName,0,MAX_PATH*sizeof(char));
		memset(chExePath,0,MAX_PATH*sizeof(char));	
	}
}structTask;

class CCommWinTaskFunc
{
public:
	//������û�г�ʼ��com �����ʹ�����³�ʼ��
	static void InitializeCom(void)
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� CoInitializeEx : %x"), hr);
			return;
		}

		hr = CoInitializeSecurity(
			NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			0,
			NULL);

		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� CoInitializeSecurity : %x"), hr);
			return;
		}
	}
	//����ʼ��
	static void UnInitializeCom(void)
	{
		CoUninitialize();
	}

	//��ȡϵͳ�汾 
	static void GetWinVersion(DWORD &dwMajorVersion/*���汾�� visita = 6*/, DWORD &dwMinorVersion /*�Ӱ汾��*/)
	{
		DWORD dwVersion = 0; 		
		dwVersion = GetVersion();
		dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
		dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));	
	}

	// �������ƻ�
	static bool AddWinTaskjob(const structTask &taskJob)
	{
		DWORD dwMajorVersion = 0,dwMinorVersion = 0;
		GetWinVersion(dwMajorVersion,dwMinorVersion);
		if (dwMajorVersion < 6)
		{
			TRACE(_T("\n ϵͳ�汾̫��"));
			return false;
		}

		std::string strJob(taskJob.chJobName);
		if (strJob.empty())
		{
			TRACE(_T("\n SetWindowTaskjob ��������"));
			return false;
		}

		char  szBuffer[MAX_PATH];  
		DWORD dwNameLen; 
		dwNameLen = MAX_PATH;  
		if (!GetUserNameA(szBuffer, &dwNameLen))
			return false;	
		std::string strUserName(szBuffer); //�û���

		memset(szBuffer,0,MAX_PATH*sizeof(char));
		dwNameLen = MAX_PATH;  
		if (!GetComputerNameA(szBuffer, &dwNameLen))
			return false;
		std::string strPcName(szBuffer);	//�������

		ITaskService *pTaskService = NULL; //ϵͳ����ƻ����� ָ��
		HRESULT hr = CoCreateInstance(CLSID_TaskScheduler,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_ITaskService,
			(void**)&pTaskService);  
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� ITaskService CoCreateInstance: %x"), hr);
			return false;
		}
		//��������ƻ����� _variant_t()ʹ�õ�ǰ�û���������
		hr = pTaskService->Connect(_variant_t(), _variant_t(),
			_variant_t(), _variant_t());
		if (FAILED(hr) )
		{
			TRACE(_T("ʧ�� ITaskService::Connect : %x"), hr);
			pTaskService->Release();
			return false;
		}

		ITaskFolder *pRootFolder = NULL;// ����ƻ���Ŀ¼
		hr = pTaskService->GetFolder(_bstr_t( L"\\") , &pRootFolder);
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� ITaskFolder GetFolder: %x"), hr);
			pTaskService->Release();
			return false;
		}
		//����Ѿ��������� ����ɾ��
		std::string strTaskName(strJob);
		pRootFolder->DeleteTask(_bstr_t(strTaskName.c_str()), 0);	

		//�½�����
		ITaskDefinition *pTaskdef = NULL;
		hr = pTaskService->NewTask(0, &pTaskdef);
		pTaskService->Release();  //���˿����ͷ�pService
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� ITaskDefinition: %x"), hr);
			pRootFolder->Release();
			return false ;
		}

		IRegistrationInfo *pRegInfo= NULL;	//��������
		hr = pTaskdef->get_RegistrationInfo(&pRegInfo);
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� get_RegistrationInfo : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}
		hr = pRegInfo->put_Author(L"��ƽ��Ϣ����"); //��������
		pRegInfo->Release(); //���˿����ͷ�pRegInfo
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� put identification info: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		ITaskSettings *pSettings = NULL;//��������
		hr = pTaskdef->get_Settings(&pSettings);
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� get_Settings: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}
		HRESULT hr1 = pSettings->put_StartWhenAvailable(VARIANT_TRUE);		//��ʱ����ʹ���˶�ʱ��ʱ�䣬Ҳ�������У�
		HRESULT hr2 = pSettings->put_ExecutionTimeLimit(_bstr_t(L"PT0S")); //���Զ�ֹͣ ,Ĭ��Ϊ72Сʱֹͣ
		HRESULT hr3 = pSettings->put_DisallowStartIfOnBatteries(VARIANT_BOOL(false));	//��ع����ʱ����������
		HRESULT hr4 = pSettings->put_StopIfGoingOnBatteries(VARIANT_BOOL(false));		//��ع��������е�ʱ��ֹͣ	
		HRESULT hr5 = pSettings->put_RunOnlyIfIdle(VARIANT_BOOL(false));				//�ǿ���ʱ��Ҳ�����У�
		pSettings->Release();
		if (FAILED(hr1) || FAILED(hr2) || FAILED(hr3)|| FAILED(hr4) || FAILED(hr5))
		{
			TRACE(_T("\n ʧ�� ITaskSettings : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		ITriggerCollection *pTriggerCollection = NULL;
		hr = pTaskdef->get_Triggers(&pTriggerCollection);
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� get_Triggers: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		ITrigger *pTrigger = NULL;//���񴥷�����.
		hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);  //���û�����ʱ����
		pTriggerCollection->Release();
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� ITrigger: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		IPrincipal *pPrinciPal = NULL;//��������Ȩ��:
		hr = pTaskdef->get_Principal(&pPrinciPal);
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� get_Principal : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}
		hr = pPrinciPal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);//����ԱȨ������
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� put_RunLevel: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}
		hr = pPrinciPal->put_Id(_bstr_t(strTaskName.c_str()));  //�˴�ֱ��ʹ�� �������� ��֤ID Ψһ
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� put_Id : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}	
		hr = pPrinciPal->put_UserId(_bstr_t(strUserName.c_str()));
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� put_UserId : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}
		hr = pPrinciPal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� put_LogonType : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}
		pPrinciPal->Release();

		IActionCollection *pActionCollection = NULL;//����������
		hr = pTaskdef->get_Actions(&pActionCollection );
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� get_Actions : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		IAction *pAction = NULL;//����������
		hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);	
		pActionCollection->Release();
		if (FAILED(hr) )
		{
			TRACE(_T("\n ʧ�ܣ� ����������: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		IExecAction *pExecAction = NULL;//���嶯������
		hr = pAction->QueryInterface( 
			IID_IExecAction, (void**) &pExecAction);
		pAction->Release();
		if (FAILED(hr) )
		{
			TRACE(_T("\n ʧ�� IExecAction: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		hr = pExecAction->put_Path(_bstr_t(taskJob.chExePath)) ; //��Ҫ�����ĳ���
		//	pExecAction->put_Arguments(CComBSTR(L"-systray")); //��������д��ִ�г���Ĳ���  
		pExecAction->Release(); 
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� IExecAction: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		std::string strGroupUser = strPcName + ("\\") +strUserName; //���������û�������Ҫ��,Ĭ��ʹ�õ�ǰ�û�
		//��������ƻ�
		IRegisteredTask *pRegisteredTask = NULL;
		hr = pRootFolder->RegisterTaskDefinition(
			_bstr_t(strTaskName.c_str()),
			pTaskdef,
			TASK_CREATE_OR_UPDATE, 
			_variant_t(strGroupUser.c_str()), //_variant_t(L"Local Service")�� system ����			
			_variant_t(), 
			TASK_LOGON_INTERACTIVE_TOKEN,
			_variant_t(L""),
			&pRegisteredTask);

		if (S_OK == hr)
		{
			TRACE(_T("\n Success! RegisterTaskDefinition"));
		}
		else if (E_OUTOFMEMORY == hr)
		{
			TRACE(_T("The application does not have enough memory to complete the operation or the user or password has at least one null and one non-null value."));
		}
		else if (SCHED_S_BATCH_LOGON_PROBLEM == hr)
		{
			TRACE(_T("The task is registered, but may fail to start. Batch logon privilege needs to be enabled for the task principal."));
		}
		else if (SCHED_S_SOME_TRIGGERS_FAILED == hr)
		{
			TRACE(_T("The task is registered, but not all specified triggers will start the task."));
		}

		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� RegisterTaskDefinition"));
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		//�ͷ�
		pRootFolder->Release();
		pTaskdef->Release();
		pRegisteredTask->Release();
		return true;
	}

	// ɾ������ƻ�
	static bool DelWinTaskjob(const std::string &strJobName)
	{
		DWORD dwMajorVersion = 0,dwMinorVersion = 0;
		GetWinVersion(dwMajorVersion,dwMinorVersion);
		if (dwMajorVersion < 6)
		{
			TRACE(_T("\n ϵͳ�汾̫��"));
			return false;
		}	

		if (strJobName.empty())
		{
			return false;
		}

		ITaskService *pTaskService = NULL; //ϵͳ����ƻ����� ָ��
		HRESULT hr = CoCreateInstance(CLSID_TaskScheduler,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_ITaskService,
			(void**)&pTaskService);  
		if (FAILED(hr))
		{
			TRACE(_T("\n ʧ�� ITaskService CoCreateInstance: %x"), hr);
			return false;
		}
		//��������ƻ����� _variant_t()ʹ�õ�ǰ�û���������
		hr = pTaskService->Connect(_variant_t(), _variant_t(),
			_variant_t(), _variant_t());
		if (FAILED(hr) )
		{
			TRACE(_T("ʧ�� ITaskService::Connect : %x"), hr);
			pTaskService->Release();
			return false;
		}

		ITaskFolder *pRootFolder = NULL;// ����ƻ���Ŀ¼
		hr = pTaskService->GetFolder(_bstr_t( L"\\") , &pRootFolder);
		if (FAILED(hr) )
		{
			TRACE(_T("\n ʧ�� ITaskFolder GetFolder: %x"), hr);
			pTaskService->Release();
			return false;
		}

		bool bResult = true;
		hr = pRootFolder->DeleteTask(_bstr_t(strJobName.c_str()), 0);	 //������񲻴��� Ҳ��ɾ��ʧ��
		if (FAILED(hr))
		{
			bResult = false;
			TRACE(_T("\n ɾ������ʧ��: %x"), hr);
		}	

		pRootFolder->Release();
		pTaskService->Release();
		return bResult;
	}
};