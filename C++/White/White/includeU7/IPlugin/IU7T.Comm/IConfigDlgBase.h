#ifndef __AVCON_U7_CONFIG_DLG_BASE_H__
#define __AVCON_U7_CONFIG_DLG_BASE_H__
//////////////////////////////////////////////////////////////////////////

///�Ի�������ӿ�
class IDlgBase
{
public:
	virtual ~IDlgBase(void){};
public:
	///��ȡ���ھ��
	virtual const HWND GetWndHWnd() = 0;
};
///���öԻ�������ӿ�
class IConfigDlgBase : public IDlgBase
{
public:
	///����Ӧ��(����)
	virtual bool OnApply(void) = 0;
	///ȡ���޸�(�����ݳ������ϴα����ʱ���)
	virtual void OnCancel(void) = 0;
};
#endif//__AVCON_U7_CONFIG_DLG_BASE_H__
