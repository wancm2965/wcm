#ifndef _WBLOGINDLG_H_
#define _WBLOGINDLG_H_

#include "stdafx.h"
#include "IPlugin/IU7_IM/U7_IMSDefine.h"
#include "win_impl_base.hpp"
#include "LoginDefault.h"

class CWBLoginDlg:
	public WindowImplBase
	,public IPlugin_U7_IM_Login_Observer //��¼��Ϣ����
{
public:
	CWBLoginDlg(void);
	~CWBLoginDlg(void);

public:
	//�̳�IPlugin_U7_IM_Login_Observer
	virtual void OnConnectStatus(unsigned long ulStatus, int nErrCode);
	virtual void OnServerListItem(const char* strMCUID,const char* strMCUType,
		unsigned long ulConnections,const char* strBandWidth,unsigned long ulActivity,
		const char* strIPAddr,unsigned short usIPPort,const char* strIPName){};
	virtual void OnServerListEnd(void){};
	// nErrCode:������룬Ϊ0���¼�ɹ�����Ϊ0���¼ʧ�ܣ���ΪERR_IMS_CLIENTAUTH(20013)ʱ����ʾ
	// ����֤ʧ�ܣ���ʱhr�����壬�ɽ���hr�еĺ�������ʾ������Ϣ�����������hr��ΪS_OK��������Դ�
	// �߼��е����������ڴ˴�ע��
	virtual void OnLogin(int nErrCode , HRESULT hr);
	virtual void OnLogout(int nErrCode);
	
	
	
	
public:
	//�̳�IPlugin_U7_IM_Room
	
	//�̳�WindowImplBase
public:

	virtual void Init();  //CDlgLogin��������ݵĳ�ʼ��
	virtual LPCTSTR GetWindowClassName() const;	//�õ����ڵ�����
	virtual CString GetSkinFile();//�õ���½���ڵ�Ƥ���ļ�
protected:
	virtual void Notify(TNotifyUI& msg);//��½���ڵ�����Ϣ������
	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);//��½���ڵ�����Ӧ��������Ϣ
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);	 //��½���ڵ���Ĭ���¼�����Ϣ����
private:
	CString m_strServer;//�û�IP
	CString m_strUserName;//�û�����
	CString m_strPassWord;//�û���¼����

	CRichEditUI* m_pEditServe;//IP�༭��ָ��
	CRichEditUI* m_pEditUserName;//�û����༭��ָ��
	CRichEditUI* m_pEditPassWord;//����༭��ָ��

	CButtonUI*	m_pBtnNetStatus;//������¼
	CLabelUI *  m_pLblTitle;//��ʾ��¼��Ϣ��lab

	bool m_bLogout;//�Ƿ��˳���¼��
	bool m_bIsCloseBtn;//�رհ�ť
	bool m_bIsLoginCancle;//ȡ����¼

	WBLoginInfo m_LoginInfoArr[SAVEUSERINFOLONG];//����û���Ϣ
	int m_indexUserInfoPos;//����ͳ�Ʊ���Ľ�λ�ã��ǰ���SAVEUSERINFOLONG-1����ѭ������
	CString m_strLanguageFile;//�õ�Ƥ��xml�ļ���

public:
	void OnLoginNet(void);//��Ӧ������¼��Ϣ
	void OnHideDlg(bool bflag = true);//�رհ�ť
	void OnCancleLogin(void);//ȡ����¼

	void SaveLoginInfo();//�����¼��Ϣ

	void InitLoginInfo();//��ʼ����¼��Ϣ
	CString GetSaveLoginPath();//·��

	BOOL CheckData(void);  //������ݵ������ʽ�Ƿ���ȷ
	BOOL CheckServerIP(CString IPAddr);//�����������ʱ�����������ĸ�����Զ��ں�̨�Ѵ�д��ΪСд

	CButtonUI* GetButtonUI();
	CLabelUI *GetLabelUI() {return m_pLblTitle;}
	void SetLoginCancle(bool bflag) {m_bIsLoginCancle = bflag;}
	void DisConnect();
	void GetSkinPath();
	bool OnEditEvent( void* p );
	
};




#endif