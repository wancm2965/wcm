// TestAH2K.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "TestAH2K.h"
#include "DlgMain.h"

#ifndef _WIN64
#include <NETEC/NETEC_Core.h>
#include <AUDEC/AUDEC_Engine.h>
#endif

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestAH2KApp

BEGIN_MESSAGE_MAP(CTestAH2KApp, CWinApp)
END_MESSAGE_MAP()


// CTestAH2KApp 构造

CTestAH2KApp::CTestAH2KApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CTestAH2KApp 对象

CTestAH2KApp theApp;


// CTestAH2KApp 初始化

BOOL CTestAH2KApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	::CoInitialize(NULL);

	//int m_aPacketsPerMS[1000];
	//int m_nMaxBitrate=960;
	//int nPacketsPerSecond=m_nMaxBitrate/8;
	//int nPacketsInSecond=nPacketsPerSecond%1000;
	//nPacketsPerSecond=nPacketsPerSecond/1000;
	//double dblPacketInterval=(double)nPacketsInSecond/1000.00;
	//int nPacketIndex=0;
	//int nLastPacketIndex=-1;
	//for (int i=0;i<1000;i++)
	//{
	//	m_aPacketsPerMS[i]=nPacketsPerSecond;
	//	nPacketIndex=i*dblPacketInterval;
	//	if (nPacketIndex!=nLastPacketIndex && nPacketIndex<nPacketsInSecond)
	//	{
	//		m_aPacketsPerMS[i]++;
	//		nLastPacketIndex=nPacketIndex;
	//	}
	//}


	//{
	//	for (int i=0;i<1000;i++)
	//	{
	//		if (m_aPacketsPerMS[i]!=0)
	//		{
	//			TRACE("i=%d Count=%d\n",i,m_aPacketsPerMS[i]);
	//		}
	//	}
	//}

	//return FALSE;


	//AVVIDEO_PACKET_HEADER tOldHeader;
	//PVIDEC_HEADER pHeader=(PVIDEC_HEADER)&tOldHeader;
	//int nOldSize=sizeof(AVVIDEO_PACKET_HEADER);
	//int nNewSize=sizeof(VIDEC_HEADER);
	//memset(&tOldHeader,0XFF,nOldSize);
	//pHeader->keyframe=0;
	//pHeader->keyframe=1;

	//tOldHeader.bKeyFrame=true;
	//tOldHeader.ulTimeStamp=0;
	//tOldHeader.bKeyFrame=false;
	//tOldHeader.ulTimeStamp=1;
	//tOldHeader.bKeyFrame=true;
	//tOldHeader.ulTimeStamp=1;
	//tOldHeader.AVVideoFrameHeader.ucFrameRate=15;
	//tOldHeader.AVVideoFrameHeader.ucVideoCodec=10;
	//tOldHeader.AVVideoFrameHeader.usVideoWidth=352;
	//tOldHeader.AVVideoFrameHeader.usVideoHeight=288;

#ifndef _WIN64
	NETEC_Core::Start();
	AUDEC_Engine::AUDEC_Init();
#endif

	DlgMain dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	}

#ifndef _WIN64
	AUDEC_Engine::AUDEC_Terminate();
	NETEC_Core::Stop();
#endif

	::CoUninitialize();

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}

