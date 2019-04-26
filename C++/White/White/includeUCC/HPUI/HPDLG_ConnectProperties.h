#pragma once


//========================================================================
//�����������������Դ�����Դ��
//���ܣ���ʾ��Ƶ����������Ƶ�����ӷ�ʽ��IP��ַ��ͨ��ID
//���ߣ�ţ��ʢ
//ʱ�䣺2008-10-31
//˵����ͨ�����ûص�������ַʵ�ֲ����ص�
//--------------------------------------------------------------------------

class AFX_EXT_CLASS HPDLG_ConnectPropertiesNotify
{
public:
	HPDLG_ConnectPropertiesNotify(){};
	~HPDLG_ConnectPropertiesNotify(){};
public:
	/*
	�������ܣ����ڹرջص�����
	��    ������
	�� �� ֵ����
	*/
	virtual void IsShowVideoInfoNotify(bool bShowWindow)=0;
};

class AFX_EXT_CLASS HPDLG_ConnectProperties
	: public HPDLG_Base
{	
public:
	HPDLG_ConnectProperties(void);
	virtual~ HPDLG_ConnectProperties(void);

	bool Create(HPDLG_ConnectPropertiesNotify& rNotify, CWnd* pParent);
	void ShowWindow(int nCmdShow);
	void ShowWindow();

	void Destroy();/* ǿ�ƹرնԻ���*/
public:
	/*HPDLG_SetType����
	���ܣ���ʾ��Ƶ�����ӷ�ʽ��IP��ַ��ͨ��ID;
	������VideoType ��Ƶ���ӷ�ʽ��
	VideoIp IP��ַ��
	VideoId ͨ��ID 
	������ͬ
	*/
	void SetConProperties(std::string& mcuid, std::string& mcuaddr, unsigned long VideoId, unsigned long AudioId, CString ChannelName, int videotype, int audiotype);

	//  [4/15/2009 licong]
	/*
	�������ܣ��޸ĸ�����
	��    ����pParent	������ָ��
	�� �� ֵ����
	*/
	void ChangeParent(CWnd* pParent);
};
