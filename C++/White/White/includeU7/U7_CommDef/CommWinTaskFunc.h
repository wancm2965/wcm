/* 
	作者：周书敏
	功能：windows 任务计划 
	要求：
		1.系统版本在Vista及以上	
		2.使用前必须进行COM初始化
*/
#pragma once
#include <string>
#include <atlbase.h>  
#include <comdef.h>  
#include <taskschd.h>  
# pragma comment(lib, "taskschd.lib")  
# pragma comment(lib, "comsupp.lib")  

//作用：windows计划任务参数，参数很复杂，待需要时逐渐完善。
typedef struct _structTask
{
	char chJobName[MAX_PATH]; //任务名称
	char chExePath[MAX_PATH]; //程序路径，如果任务是执行程序
	_structTask()
	{
		memset(chJobName,0,MAX_PATH*sizeof(char));
		memset(chExePath,0,MAX_PATH*sizeof(char));	
	}
}structTask;

class CCommWinTaskFunc
{
public:
	//程序若没有初始化com 则可以使用以下初始化
	static void InitializeCom(void)
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 CoInitializeEx : %x"), hr);
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
			TRACE(_T("\n 失败 CoInitializeSecurity : %x"), hr);
			return;
		}
	}
	//反初始化
	static void UnInitializeCom(void)
	{
		CoUninitialize();
	}

	//获取系统版本 
	static void GetWinVersion(DWORD &dwMajorVersion/*主版本号 visita = 6*/, DWORD &dwMinorVersion /*子版本号*/)
	{
		DWORD dwVersion = 0; 		
		dwVersion = GetVersion();
		dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
		dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));	
	}

	// 添加任务计划
	static bool AddWinTaskjob(const structTask &taskJob)
	{
		DWORD dwMajorVersion = 0,dwMinorVersion = 0;
		GetWinVersion(dwMajorVersion,dwMinorVersion);
		if (dwMajorVersion < 6)
		{
			TRACE(_T("\n 系统版本太低"));
			return false;
		}

		std::string strJob(taskJob.chJobName);
		if (strJob.empty())
		{
			TRACE(_T("\n SetWindowTaskjob 参数错误"));
			return false;
		}

		char  szBuffer[MAX_PATH];  
		DWORD dwNameLen; 
		dwNameLen = MAX_PATH;  
		if (!GetUserNameA(szBuffer, &dwNameLen))
			return false;	
		std::string strUserName(szBuffer); //用户名

		memset(szBuffer,0,MAX_PATH*sizeof(char));
		dwNameLen = MAX_PATH;  
		if (!GetComputerNameA(szBuffer, &dwNameLen))
			return false;
		std::string strPcName(szBuffer);	//计算机名

		ITaskService *pTaskService = NULL; //系统任务计划管理 指针
		HRESULT hr = CoCreateInstance(CLSID_TaskScheduler,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_ITaskService,
			(void**)&pTaskService);  
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 ITaskService CoCreateInstance: %x"), hr);
			return false;
		}
		//连接任务计划服务， _variant_t()使用当前用户名、密码
		hr = pTaskService->Connect(_variant_t(), _variant_t(),
			_variant_t(), _variant_t());
		if (FAILED(hr) )
		{
			TRACE(_T("失败 ITaskService::Connect : %x"), hr);
			pTaskService->Release();
			return false;
		}

		ITaskFolder *pRootFolder = NULL;// 任务计划根目录
		hr = pTaskService->GetFolder(_bstr_t( L"\\") , &pRootFolder);
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 ITaskFolder GetFolder: %x"), hr);
			pTaskService->Release();
			return false;
		}
		//如果已经存在任务 ，先删除
		std::string strTaskName(strJob);
		pRootFolder->DeleteTask(_bstr_t(strTaskName.c_str()), 0);	

		//新建任务
		ITaskDefinition *pTaskdef = NULL;
		hr = pTaskService->NewTask(0, &pTaskdef);
		pTaskService->Release();  //至此可以释放pService
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 ITaskDefinition: %x"), hr);
			pRootFolder->Release();
			return false ;
		}

		IRegistrationInfo *pRegInfo= NULL;	//任务描述
		hr = pTaskdef->get_RegistrationInfo(&pRegInfo);
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 get_RegistrationInfo : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}
		hr = pRegInfo->put_Author(L"华平信息技术"); //任务作者
		pRegInfo->Release(); //至此可以释放pRegInfo
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 put identification info: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		ITaskSettings *pSettings = NULL;//任务设置
		hr = pTaskdef->get_Settings(&pSettings);
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 get_Settings: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}
		HRESULT hr1 = pSettings->put_StartWhenAvailable(VARIANT_TRUE);		//定时任务即使过了定时的时间，也可以运行；
		HRESULT hr2 = pSettings->put_ExecutionTimeLimit(_bstr_t(L"PT0S")); //不自动停止 ,默认为72小时停止
		HRESULT hr3 = pSettings->put_DisallowStartIfOnBatteries(VARIANT_BOOL(false));	//电池供电的时候不允许启动
		HRESULT hr4 = pSettings->put_StopIfGoingOnBatteries(VARIANT_BOOL(false));		//电池供电在运行的时候不停止	
		HRESULT hr5 = pSettings->put_RunOnlyIfIdle(VARIANT_BOOL(false));				//非空闲时间也能运行；
		pSettings->Release();
		if (FAILED(hr1) || FAILED(hr2) || FAILED(hr3)|| FAILED(hr4) || FAILED(hr5))
		{
			TRACE(_T("\n 失败 ITaskSettings : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		ITriggerCollection *pTriggerCollection = NULL;
		hr = pTaskdef->get_Triggers(&pTriggerCollection);
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 get_Triggers: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		ITrigger *pTrigger = NULL;//任务触发条件.
		hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);  //当用户启动时触发
		pTriggerCollection->Release();
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 ITrigger: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		IPrincipal *pPrinciPal = NULL;//设置运行权限:
		hr = pTaskdef->get_Principal(&pPrinciPal);
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 get_Principal : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}
		hr = pPrinciPal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);//管理员权限运行
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 put_RunLevel: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}
		hr = pPrinciPal->put_Id(_bstr_t(strTaskName.c_str()));  //此处直接使用 任务名称 保证ID 唯一
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 put_Id : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}	
		hr = pPrinciPal->put_UserId(_bstr_t(strUserName.c_str()));
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 put_UserId : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}
		hr = pPrinciPal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 put_LogonType : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}
		pPrinciPal->Release();

		IActionCollection *pActionCollection = NULL;//任务动作集合
		hr = pTaskdef->get_Actions(&pActionCollection );
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 get_Actions : %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		IAction *pAction = NULL;//创建任务动作
		hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);	
		pActionCollection->Release();
		if (FAILED(hr) )
		{
			TRACE(_T("\n 失败！ 创建任务动作: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		IExecAction *pExecAction = NULL;//具体动作调整
		hr = pAction->QueryInterface( 
			IID_IExecAction, (void**) &pExecAction);
		pAction->Release();
		if (FAILED(hr) )
		{
			TRACE(_T("\n 失败 IExecAction: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		hr = pExecAction->put_Path(_bstr_t(taskJob.chExePath)) ; //需要启动的程序
		//	pExecAction->put_Arguments(CComBSTR(L"-systray")); //向动作里面写入执行程序的参数  
		pExecAction->Release(); 
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 IExecAction: %x"), hr);
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		std::string strGroupUser = strPcName + ("\\") +strUserName; //用来查找用户（很重要）,默认使用当前用户
		//保存任务计划
		IRegisteredTask *pRegisteredTask = NULL;
		hr = pRootFolder->RegisterTaskDefinition(
			_bstr_t(strTaskName.c_str()),
			pTaskdef,
			TASK_CREATE_OR_UPDATE, 
			_variant_t(strGroupUser.c_str()), //_variant_t(L"Local Service")随 system 运行			
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
			TRACE(_T("\n 失败 RegisterTaskDefinition"));
			pRootFolder->Release();
			pTaskdef->Release();
			return false;
		}

		//释放
		pRootFolder->Release();
		pTaskdef->Release();
		pRegisteredTask->Release();
		return true;
	}

	// 删除任务计划
	static bool DelWinTaskjob(const std::string &strJobName)
	{
		DWORD dwMajorVersion = 0,dwMinorVersion = 0;
		GetWinVersion(dwMajorVersion,dwMinorVersion);
		if (dwMajorVersion < 6)
		{
			TRACE(_T("\n 系统版本太低"));
			return false;
		}	

		if (strJobName.empty())
		{
			return false;
		}

		ITaskService *pTaskService = NULL; //系统任务计划管理 指针
		HRESULT hr = CoCreateInstance(CLSID_TaskScheduler,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_ITaskService,
			(void**)&pTaskService);  
		if (FAILED(hr))
		{
			TRACE(_T("\n 失败 ITaskService CoCreateInstance: %x"), hr);
			return false;
		}
		//连接任务计划服务， _variant_t()使用当前用户名、密码
		hr = pTaskService->Connect(_variant_t(), _variant_t(),
			_variant_t(), _variant_t());
		if (FAILED(hr) )
		{
			TRACE(_T("失败 ITaskService::Connect : %x"), hr);
			pTaskService->Release();
			return false;
		}

		ITaskFolder *pRootFolder = NULL;// 任务计划根目录
		hr = pTaskService->GetFolder(_bstr_t( L"\\") , &pRootFolder);
		if (FAILED(hr) )
		{
			TRACE(_T("\n 失败 ITaskFolder GetFolder: %x"), hr);
			pTaskService->Release();
			return false;
		}

		bool bResult = true;
		hr = pRootFolder->DeleteTask(_bstr_t(strJobName.c_str()), 0);	 //如果任务不存在 也会删除失败
		if (FAILED(hr))
		{
			bResult = false;
			TRACE(_T("\n 删除任务失败: %x"), hr);
		}	

		pRootFolder->Release();
		pTaskService->Release();
		return bResult;
	}
};