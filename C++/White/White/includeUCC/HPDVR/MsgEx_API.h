
// MsgEx_API.h

#pragma once

#ifdef MsgEx_API
#define DllExport_wt extern "C" __declspec(dllexport)  
#else
#define DllExport_wt extern "C" __declspec(dllimport) 
#endif

// 本模块向外发送的消息（发送方式为SendMessage()） 通知外部 本模块发送来一条消息WPARAM参数为MsgEx_API_Data结构的
// 临时指针 表示消息内容
#define WM_MsgEx_API_Data (WM_USER+0x1999)

// 外部向本模块发送到消息（注意：发送方式应该为PostMessage()，否则有可能造成死锁）
// 外部通知本模块一个MsgEx_API_Data结构已经处理 WPARAM参数为MsgEx_API_Data结构的临时指针
#define WM_MsgEx_API_Data_Pop WM_USER+1001

struct MsgEx_API_Data 
{
	DWORD data; // Combobox选择内容

	CString text; // 文本内容
	CString fontface; // 文本字体名称
	CString fontsize; // 文本字体尺寸
	CString fontcolor; // 文本字体颜色
	CString fontEffects; // 文本字体效果

	CString textTime; // 时间戳文本
	CString fontfaceTime; // 时间字体名称
	CString fontsizeTime; // 时间字体尺寸
	CString fontcolorTime; // 时间字体颜色
	CString fontEffectsTime; // 时间字体效果
};

// 创建操作 创建一个模块实例 在创建之前 所有其他操作都惊被忽略
// 参数：
//      hWnd：父窗口句柄
//      hWndMsg：外部消息处理模块句柄。如果该参数为NULL，则本模块只有文本显示界面
DllExport_wt BOOL CreateMsgWindow(HWND hWnd, HWND hWndMsg);

// 获得模块的HWND句柄
DllExport_wt HWND GetMsgWindowHWND();

// 获得模块中CComboBox控件的HWND句柄
// 此控件为人员列表
// 默认有一个数据项“所有人” ItemData为0
// 其他项由外部添加， 注意ItemData不可以为0
DllExport_wt HWND GetComboHWND();

// 创建的逆操作
DllExport_wt void ReleaseMsgWindow();


// 向本文显示框添加一段文本
// 参数与MsgEx_API_Data结构对应
DllExport_wt void AppendMsg(LPCTSTR text,LPCTSTR fontface,LPCTSTR fontsize,LPCTSTR fontcolor,LPCTSTR fontEffects);

DllExport_wt void AppendMsgEx(MsgEx_API_Data* pData);