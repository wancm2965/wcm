#include <CoUI/UIlibDeclare.h>
/*
 *	2013.11.18
 *	应用需求：手机可作为鼠标控制应用程序，
 *	当点击edit控件或edit控件获取到焦点（失去焦点）时，
 *	界面底层能向上反馈相应的信息。
 */
//编辑框获得焦点信号
#define LPSZ_COUI_EDIT_SETFOCUS_EVENT	TEXT("165F2C34-96B2-4403-A8B1-87E63CE28EAD_CoUI_SETFOCUS_EVENT")
//编辑框获得焦点信号(前置信号)
#define LPSZ_COUI_EDIT_PRESETFOCUS_EVENT	TEXT("E9B0DB7F-1067-448a-B945-D0B8CCC06EE6_CoUI_PRESETFOCUS_EVENT")
//编辑框失去焦点信号
#define LPSZ_COUI_EDIT_KILLFOCUS_EVENT	TEXT("E4D5A7B0-05E3-4c69-B7A0-780612348C02_CoUI_KILLFOCUS_EVENT}")
//编辑框失去焦点信号(前置信号)
#define LPSZ_COUI_EDIT_PREKILLFOCUS_EVENT	TEXT("7FB5D36B-63A9-4866-83C6-FC893293E97A_CoUI_PreKILLFOCUS_EVENT}")

namespace CoUI
{
	//开启(关闭)Edit焦点监听
	void UILIB_API SetEditFocusListeningState(bool bOpen);
	//获取当前Edit焦点监听状态（是否在监听）
	bool UILIB_API GetEditFocusListeningState();
}//end of namespace CoUI