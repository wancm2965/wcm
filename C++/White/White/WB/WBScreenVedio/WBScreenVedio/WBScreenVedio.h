// WBScreenVedio.h : WBScreenVedio DLL ����ͷ�ļ�
//

#pragma once

// #ifndef __AFXWIN_H__
// 	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
// #endif

#include "resource.h"		// ������


// CWBScreenVedioApp
// �йش���ʵ�ֵ���Ϣ������� WBScreenVedio.cpp
//
#include <cmath>
using namespace std;

/*------------------------------------------------------------------------------------------*/

#define VDEIODLL_EXPORT_API __declspec(dllexport)

/*------------------------------------------------------------------------------------------*/
extern "C"
{
	//��ȡ¼�ƴ��ڵ����������յ�����
	void VDEIODLL_EXPORT_API GetRect(CPoint& Point1,CPoint& Point2);
}
/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//��ȡ�ļ�����·��
	CString VDEIODLL_EXPORT_API GetFilePath();
}
/*------------------------------------------------------------------------------------------*/
extern "C"
{
	//���ݸ������ļ�Ŀ¼�����ظ�Ŀ¼�е�.avi�ļ�����Ŀ
	CString VDEIODLL_EXPORT_API GetFileName(CString strPath,int& nCount);
}
/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//������Ƶ¼��
	void VDEIODLL_EXPORT_API StartVedio(CString strFilePath,CString strFileName,int nRate,
		CPoint Point1,CPoint Point2,int nFlag,HWND GetCurHwnd,CPoint pt = CPoint(0,0));
}

/*------------------------------------------------------------------------------------------*/
extern "C"
{	
	//�ر���Ƶ¼��
	void VDEIODLL_EXPORT_API StopVedio();
}

/*------------------------------------------------------------------------------------------*/

extern "C"
{  
	//������Ƶ¼��
	void VDEIODLL_EXPORT_API PauseVedio();
}

/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//�ָ���Ƶ¼��
	void VDEIODLL_EXPORT_API UnPauseVedio();
}


/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//��ȡָ��·�����ڴ��̵�ʣ��ռ�
	CString VDEIODLL_EXPORT_API GetResidualPlace(CString strPath);
}

/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//������
	void   VDEIODLL_EXPORT_API DrawWindow(HWND hwnd);
}

/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//ͨѶָ��
	void   VDEIODLL_EXPORT_API SetU7HPWBDataNotify(IHPNetNotify* HPWBDataNotify);
}

/*------------------------------------------------------------------------------------------*/
extern "C"
{
	//U7�Ƿ�֧��
	void   VDEIODLL_EXPORT_API SetSupportU7Open(BOOL bSupportU7OpenFile);
}

/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//������
	void   VDEIODLL_EXPORT_API GetAreaVideoScreenHwnd(HWND& hwnd);
}
/*------------------------------------------------------------------------------------------*/
class CWBScreenVedioApp : public CWinApp
{
public:
	CWBScreenVedioApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
