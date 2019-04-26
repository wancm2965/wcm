#ifndef __AVCON_U7_CONFIG_DLG_BASE_H__
#define __AVCON_U7_CONFIG_DLG_BASE_H__
//////////////////////////////////////////////////////////////////////////

///对话框基础接口
class IDlgBase
{
public:
	virtual ~IDlgBase(void){};
public:
	///获取窗口句柄
	virtual const HWND GetWndHWnd() = 0;
};
///配置对话框基础接口
class IConfigDlgBase : public IDlgBase
{
public:
	///保存应用(配置)
	virtual bool OnApply(void) = 0;
	///取消修改(即数据撤销到上次保存的时间点)
	virtual void OnCancel(void) = 0;
};
#endif//__AVCON_U7_CONFIG_DLG_BASE_H__
