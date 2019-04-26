// TestAH2K.cpp : ����Ӧ�ó��������Ϊ��
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


// CTestAH2KApp ����

CTestAH2KApp::CTestAH2KApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CTestAH2KApp ����

CTestAH2KApp theApp;


// CTestAH2KApp ��ʼ��

BOOL CTestAH2KApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

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
		// TODO: �ڴ˷��ô����ʱ�á�ȷ�������ر�
		//�Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//�Ի���Ĵ���
	}

#ifndef _WIN64
	AUDEC_Engine::AUDEC_Terminate();
	NETEC_Core::Stop();
#endif

	::CoUninitialize();

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	// ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

