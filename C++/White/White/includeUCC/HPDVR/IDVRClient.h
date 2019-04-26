#pragma once
#ifndef __IDVRCLIENT_H_
#define __IDVRCLIENT_H_

#define WM_NOTIFY_LOGOUT		(WM_USER+0x103)

class IDVRClientNotify //EXE�̳�
{
public: /*GUI to Device*/
	virtual void OnIDVRClientNotify_SendSMS(string& regioncode, string& phonenumber, string& text) = 0;
	virtual void OnIDVRClientNotify_XMLDocumentElement(KXMLElement* t) = 0;
	virtual void OnIDVRClientNotify_Exit(int nType=0) = 0;/*0�˳���1������2�������*/
	virtual void OnIDVRClientNotify_Message(UINT nCMD, WPARAM wParam, LPARAM lParam) = 0;
};

class IDVRClient //DLL�̳�
{
public:
	IDVRClient() {};
	virtual ~IDVRClient() {};

public:
	virtual bool Connect(HWND hParent, int nConnectType) = 0;
	virtual void ReleaseConnection() = 0;
	virtual void SaveLastOperation() = 0;

public:
	virtual void SetWindowPosition(int x, int y, int cx, int cy) = 0;
	virtual void ShowWindow(int showType)=0;
	virtual void PopupMsgText(LPCTSTR lpText)=0;
	virtual CWnd* GetClientWND()=0;
	virtual void ShowConfig(int nType)=0;
	virtual HWND GetMsgHwnd(void)=0; //��ȡ������Ϣ������
	virtual HWND GetMsgComboxHwnd(void)=0; //��ȡ������Ϣ�����Combox�ؼ����
	virtual void DoLogout(void)=0;//ע������
	virtual bool CheckLDVRAccount(LPCTSTR szAccount, LPCTSTR szPassword)=0;/*��֤�û�������*/
public: /*Device to GUI*/
	virtual void OnXMLDocumentElement(KXMLElement* t) = 0;
};

AFX_EXT_CLASS IDVRClient* CreateDVRClient(IDVRClientNotify& notify);

AFX_EXT_API bool dvr_server_data_upgrade(int new_version);
AFX_EXT_API bool dvr_client_data_upgrade(int new_version);

#endif