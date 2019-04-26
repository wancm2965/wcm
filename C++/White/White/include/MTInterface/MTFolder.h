#pragma once
#include <windows.h>
#ifdef MTFOLER_EXPORTS
#define DLL_MTFOLER_API __declspec(dllexport)
#else
#define DLL_MTFOLER_API __declspec(dllimport)
#endif
//若对话框为模态对话框，则返回值有效，若为非模态对话框，
//则由该消息(WPARAM)接收宽字节类型字符串指针,(LPARAM)始终为UWM_FOLERFILE
#define UWM_FOLERFILE (WM_USER+112)	
typedef BOOL(*PMTFILETYPE)(const wchar_t *);//文件类型过滤回调函数类型

#define DRIVECONFIG_UNKNOWN 0x01 //未知设备
//#define DRIVECONFIG_NO_ROOT_DIR 0x02 //指定路径是无效的
#define DRIVECONFIG_REMOVABLE 0x04 //移动磁盘
#define DRIVECONFIG_FIXED 0x08 //硬盘
#define DRIVECONFIG_REMOTE 0x10 //网络磁盘
#define DRIVECONFIG_CDROM 0x20 // 光驱
#define DRIVECONFIG_RAMDISK 0x40 //未知设备
#define DRIVECONFIG_ALL 0xFF //全选

struct MTFOLDERCONFIG{
	MTFOLDERCONFIG():bModal(FALSE),x(450),y(200),WndWidth(400),WndHeight(450),
		bMoreSelect(FALSE),bShowFile(FALSE),bSysTouch(TRUE),bShow(NULL),
		InitPage(NULL),DiscConfig(DRIVECONFIG_FIXED|DRIVECONFIG_REMOVABLE|DRIVECONFIG_CDROM),
		FileConfig(0xFFFF),bShowRecently(FALSE),InitPath(NULL),DriveName(NULL),future(0){};
	BOOL bModal;//是否为模态
	INT x;//对话框起点
	INT y;//该坐标对应屏幕坐标
	INT WndWidth;//对话框宽
	INT WndHeight;//对话框高
	BOOL bMoreSelect;//是否支持多选
	BOOL bShowFile;//若为TRUE则为文件选择，否则为文件夹选择
	BOOL bSysTouch;//若为TRUE则使用系统触摸，否则使用自定义触摸服务。若没有自定义触摸服务则不提供触摸支持
	PMTFILETYPE bShow;//回调过滤函数指针，若为NULL则使用默认过滤
	const wchar_t *InitPage;//配置标签（格式:路径<标签名>路径<标签名>...），支持多标签添加，标签名中不要含有'\'':'等特殊字符，若为NULL则可切换所有标签，若为“/”则只显示我的电脑
	DWORD DiscConfig;//默认显示硬盘和可移动磁盘
	DWORD FileConfig;//默认显示所有文件（除.和..）
	BOOL bShowRecently;//是否添加配置文件记录历史操作目录
	const wchar_t *InitPath;//指定初始目录，为NULL则显示上次退出时的目录
	const wchar_t *DriveName;//驱动器别名（格式:<系统磁盘()><本地磁盘()><可移动磁盘(0)>）别名后面加()表示显示驱动器符号,加(0)则所有别名都在其后自动加序号
	//const wchar_t *InitLanguage; //配置标签(格式:确定,取消,文件选择,文件夹选择)
	DWORD future;//备用
};

extern "C" DLL_MTFOLER_API void IniLanguage(const wchar_t *,const wchar_t *);

extern "C" DLL_MTFOLER_API const wchar_t *ShowFileUI(HWND,MTFOLDERCONFIG* pconfig = NULL);
extern "C" DLL_MTFOLER_API BOOL CloseFolder();


#define UWM_FOLERDEL (WM_USER+114)	
extern "C" DLL_MTFOLER_API const wchar_t *ShowDialogUI(HWND);
extern "C" DLL_MTFOLER_API BOOL CloseDialogUI();