#pragma once

class INotifyMsg
{
public:

	//----------------------------------
	//�������ܣ����õ�����Ϣ·��
	//----------------------------------
	virtual	void	SetSkinFilePath(CString strSkinFilePath)=0;

	virtual	void	NotifyMsg(CString sMsg)=0;		//������ص���Ϣ
	//----------------------------------
	//�������ܣ����ٶ�Ӧ�ĶԻ���
	//----------------------------------
	virtual void Destory()=0;

};

//----------------------------------
//�������ܣ�����������Ϣ��
//����˵����pParentWnd:������ָ�� pNotify:�ص��ӿ�ָ��
//����ֵ  ��������̨�ӿ�ָ��
//----------------------------------

AFX_EXT_CLASS  INotifyMsg* CreateNotifyMsg(CWnd* pParentWnd);