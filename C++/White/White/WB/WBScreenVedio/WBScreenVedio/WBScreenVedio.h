// WBScreenVedio.h : WBScreenVedio DLL 的主头文件
//

#pragma once

// #ifndef __AFXWIN_H__
// 	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
// #endif

#include "resource.h"		// 主符号


// CWBScreenVedioApp
// 有关此类实现的信息，请参阅 WBScreenVedio.cpp
//
#include <cmath>
using namespace std;

/*------------------------------------------------------------------------------------------*/

#define VDEIODLL_EXPORT_API __declspec(dllexport)

/*------------------------------------------------------------------------------------------*/
extern "C"
{
	//获取录制窗口的起点坐标和终点坐标
	void VDEIODLL_EXPORT_API GetRect(CPoint& Point1,CPoint& Point2);
}
/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//获取文件保存路径
	CString VDEIODLL_EXPORT_API GetFilePath();
}
/*------------------------------------------------------------------------------------------*/
extern "C"
{
	//根据给定的文件目录，返回该目录中的.avi文件的数目
	CString VDEIODLL_EXPORT_API GetFileName(CString strPath,int& nCount);
}
/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//启动视频录制
	void VDEIODLL_EXPORT_API StartVedio(CString strFilePath,CString strFileName,int nRate,
		CPoint Point1,CPoint Point2,int nFlag,HWND GetCurHwnd,CPoint pt = CPoint(0,0));
}

/*------------------------------------------------------------------------------------------*/
extern "C"
{	
	//关闭视频录制
	void VDEIODLL_EXPORT_API StopVedio();
}

/*------------------------------------------------------------------------------------------*/

extern "C"
{  
	//挂起视频录制
	void VDEIODLL_EXPORT_API PauseVedio();
}

/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//恢复视频录制
	void VDEIODLL_EXPORT_API UnPauseVedio();
}


/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//获取指定路径所在磁盘的剩余空间
	CString VDEIODLL_EXPORT_API GetResidualPlace(CString strPath);
}

/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//画窗口
	void   VDEIODLL_EXPORT_API DrawWindow(HWND hwnd);
}

/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//通讯指针
	void   VDEIODLL_EXPORT_API SetU7HPWBDataNotify(IHPNetNotify* HPWBDataNotify);
}

/*------------------------------------------------------------------------------------------*/
extern "C"
{
	//U7是否支持
	void   VDEIODLL_EXPORT_API SetSupportU7Open(BOOL bSupportU7OpenFile);
}

/*------------------------------------------------------------------------------------------*/

extern "C"
{
	//画窗口
	void   VDEIODLL_EXPORT_API GetAreaVideoScreenHwnd(HWND& hwnd);
}
/*------------------------------------------------------------------------------------------*/
class CWBScreenVedioApp : public CWinApp
{
public:
	CWBScreenVedioApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
