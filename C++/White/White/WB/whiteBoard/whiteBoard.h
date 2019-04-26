// whiteBoard.h : whiteBoard DLL 的主头文件
//

#pragma once
#pragma comment(lib,"Setupapi.lib")
#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "WhiteBoardDlg.h"
#include "MedioVideoScreenDlg.h"
#include "ScreenDrawDlg.h"
#include <Setupapi.h>



// CwhiteBoardApp
// 有关此类实现的信息，请参阅 whiteBoard.cpp
//

class CwhiteBoardApp : public CWinApp
{
public:
	CwhiteBoardApp();
	virtual ~CwhiteBoardApp();   //重载析构函数  
	
// 重写
public:
	virtual BOOL InitInstance();
	void KillExe();  //白板退出时如果有打印对话框也一起退出
	DECLARE_MESSAGE_MAP()
	
	virtual int ExitInstance();

	//wangxin 2010-12-28 删除一个文件夹
	BOOL DeleteDirectory(CString strDir);
	//是否获得了触摸授权许可
	BOOL IsDeviceSupported();
private:
	HINSTANCE m_hinstRE41;        //声明实例对象
};

extern CWhiteBoardView *g_pWhiteBoardView; //当前试图窗口
extern CDialog *g_whiteboardDlg;
extern CWhiteBoardView *g_pBackWhiteBoardView;
extern CWBFile *g_pWBFile; //白板文件指针
extern CWBPage *g_pWBPage; //白板页指针
extern BOOL g_bIsFullScreen; //是否全屏状态
extern IHPNetNotify *g_pHPWBDataNotify; //发送同步消息指针
extern IHPWBNotify *g_pConferenceRecordNotify; //录像消息指针
extern void *g_pParent ; //父窗口
extern CPoint g_ptComRotCenter; //组合的旋转中心点
extern CPoint g_ptDocRotCenter; //文档的旋转中心点
extern double g_dbComRotAngle; //组合的旋转角度
extern double g_dbDocRotAngle; //文档的旋转角度
extern CPoint g_ptComZoomCenter; //组合的缩放中心点
extern CPoint g_ptDocZoomCenter; //文档的缩放中心点
extern BOOL g_bRunWater;         //水波纹特效
extern BOOL g_bProcessWater;    //控制白板最小化时显示水波纹
extern int  g_nBrushSize;      //擦子大小
//组合对象的缩放比例
extern double g_dbXScale;
extern double g_dbYScale;
//文档对象的缩放比例
extern double g_dbDocXScale;
extern double g_dbDocYScale;
//新功能控制
extern bool g_bShowWBNewFun;
extern bool g_bSimplifyWB;//是否有媒体功能
//view 缩小
extern double g_reduce;
//白板view的长和宽
extern int g_nWidth;
extern int g_nHeight;

extern DWORD g_dwPreZoomLayerIndex; //前一个触摸缩放的对象图层号
extern BOOL g_bIsDrawScreen;                  //屏幕绘图是否开启标志
extern unsigned long g_ulSessionID; //客户端ID
extern CString g_strAvconUserID; //AVCON本地登录的用户ID，由AVCON通过接口函数设置
extern BOOL g_bYunNanDeviceSupported;//专用于云南公安硬件授权
extern bool g_bAuthorizedByHandWare ; //从硬件层面是否有触摸触摸授权
extern bool g_bWBWndIsneedShowToolBtn;//根据白板大小来判断是否显示按钮
extern CString g_strLocalUserName;//设置本地用户名称
//有AVCON提供的一些临时路径
extern CString g_strAvconLanguageFile;
extern CString g_strAvconIniPath;  //配置文件目录
extern CString g_strAvconTempPath;   //临时目录
extern HMODULE g_IsMedicalVersion;//判断是否是医疗版本；
extern bool g_bAvconLock;   //AVCON是否锁定白板功能，当锁定时，白板打开后没有工具栏
extern bool g_bIsConferenceRecord; //由AVCON设置，是否开始会议录像
extern WB_TYPE g_WBType;			//当前版本类型
extern bool g_bIsSupportMTTouch;//是否最终授权使用多点触摸屏
extern BOOL g_bNoSupportTouchInWB;//当触摸屏没有授权时，包括单点的触摸操作都屏蔽
extern BOOL g_bSupportU7OpenFile ;// 判断是否为U7中的打开文件方式
extern CString g_bU7FileTempDir ;// 设置U7的文件默认路径
extern BOOL g_MediaIntToSin;//视频网络到单机
extern int g_PageCount;//页号，视频打开文件时候
extern HWND g_hVedioHwnd;
extern BOOL g_bMedioScreenWndEnd;//区域录制窗口意外关闭
extern bool g_bShowFullScreenBtn; //显示全屏按钮；
extern CString g_strMediaPhoto; //视频双击拍照存放路径；
extern HMODULE g_IsMidiVersion; //midi版本
//添加是否为I7项目
extern bool g_bSupportI7;
extern CPoint g_preScrollPoint ;//I7中滚动条的上一次位置；
extern bool g_bU7NewStyle;//新u7中白板的功能界面削减优化