#ifndef __HPDVRDLL_H_
#define __HPDVRDLL_H_

class IHPDVRDLLNotify
{
public:
	IHPDVRDLLNotify(){}
	virtual ~IHPDVRDLLNotify(){}
public:
	virtual void OnIHPDVRDLLNotify_Exit(int nType=0) = 0;/*0�˳���1������2�������*/
};

class IHPDVRDLL
{
public:
	IHPDVRDLL() {};
	virtual ~IHPDVRDLL() {};

public:
	virtual bool Connect(HWND hParent, int nType) = 0;
	virtual void Release(void) = 0;
	virtual HWND GetAppHwnd(void) = 0; 
	virtual HWND GetGuiHwnd(void) = 0; 
	virtual bool IsVisibled(void) = 0;
	virtual bool ShowWindow(bool bShow) = 0; 
	virtual void ShowConfig(int type) = 0;/*1��DVR�û����� 2��¼������ 3��¼��ط� 4��ϵͳ��־*/
	virtual HWND GetMsgHwnd(void) =0; //��ȡ������Ϣ������
	virtual HWND GetMsgComboxHwnd(void)=0; //��ȡ������Ϣ�����Combox�ؼ����

};

AFX_EXT_CLASS IHPDVRDLL* CreateDVRDevice(IHPDVRDLLNotify& notify);
AFX_EXT_CLASS void InitDVRDevice(unsigned long ulVIDECFlag=0);
AFX_EXT_CLASS void UnInitDVRDevice(void);

#endif